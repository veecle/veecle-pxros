//! Utility to compile and/or run PXROS Rust tasks.
#![deny(unsafe_code)]
#![warn(missing_docs)]
#![warn(clippy::pedantic)]

mod build;
mod check;
mod emulate;
mod run;

use std::ffi::OsStr;
use std::path::PathBuf;
use std::process;

use anyhow::{bail, Context};
use clap::Parser;

use crate::build::build;
use crate::check::check;
use crate::emulate::emulate;
use crate::run::run;

/// Utility to compile and/or run PXROS Rust tasks.
#[derive(Parser)]
#[command(version, about, long_about = None)]
struct Cli {
    #[command(subcommand)]
    command: Commands,
}

/// Available CLI commands.
#[derive(Debug, Parser)]
pub enum Commands {
    /// Compile and run a PXROS Rust binary on real hardware.
    Run(build::Options),
    /// Compile and emulate a PXROS Rust binary on the TSIM.
    Emulate(build::Options),
    /// Compile a PXROS Rust binary.
    Build(build::Options),
    /// Run the CI checks locally.
    Check,
}

fn main() -> anyhow::Result<()> {
    let cli_input = Cli::parse();

    match cli_input.command {
        Commands::Run(options) => run(options),
        Commands::Emulate(options) => emulate(&options),
        Commands::Build(options) => build(options),
        Commands::Check => check(),
    }
}

/// Detects the last tricore toolchain listed by [`query_rustup_installed_toolchains`].
///
/// # Errors
/// Returns an error if:
/// * no toolchains can be queried.
/// * no tricore toolchain is installed.
pub fn detect_tricore_toolchain() -> anyhow::Result<String> {
    let available_toolchains = query_rustup_installed_toolchains()?;

    let tricore_toolchains: Vec<String> = available_toolchains
        .iter()
        .filter(|toolchain| toolchain.contains("tricore-htc-none"))
        .cloned()
        .collect();
    if let Some(tricore_toolchain) = tricore_toolchains.last() {
        Ok(tricore_toolchain.to_owned())
    } else {
        bail!("No tricore toolchain installed.")
    }
}

/// Queries `cargo` for the current workspace root location.
///
/// # Errors
/// Returns an error if:
/// * `cargo locate-project` exits with error.
/// * `cargo locate-project` output cannot be parsed.
/// * `cargo locate-project` output is empty.
pub fn query_workspace_root_location() -> anyhow::Result<PathBuf> {
    let cargo_output = capture_command_output("cargo", ["locate-project", "--workspace", "--message-format", "plain"])?;

    let workspace_location = cargo_output.trim().to_string();
    if workspace_location.is_empty() {
        bail!("Cargo locate project returned empty response.")
    }
    let mut workspace_root_path =
        PathBuf::try_from(workspace_location).context("Could not parse workspace root to path.")?;

    // Remove last segment ("Cargo.toml") from path.
    workspace_root_path.pop();
    Ok(workspace_root_path)
}

/// Queries `rustup` for the installed toolchains.
///
/// # Errors
/// Returns an error if:
/// * `rustup toolchain list` exits with error.
/// * `rustup toolchain list` output cannot be parsed.
/// * `rustup toolchain list` output is empty.
///
/// # Panics
/// Panics if `rustup toolchain list` returns an empty line.
pub fn query_rustup_installed_toolchains() -> anyhow::Result<Vec<String>> {
    let rustup_output = capture_command_output("rustup", ["toolchain", "list"])?;

    let toolchains: Vec<String> = rustup_output
        .lines()
        .map(ToOwned::to_owned)
        .map(|line| {
            line.split_whitespace()
                .next()
                .expect("No empty lines should be returned by rustup.")
                .to_string()
        })
        .collect();

    if toolchains.is_empty() {
        bail!("rustup toolchain list returned empty response.")
    }
    Ok(toolchains)
}

/// Run a command via [`process::Command::new()`] and capture its output.
///
/// `stdout` will not be printed, but instead returned from this function.
///
/// # Errors
/// Returns an error if:
/// * the command fails to execute.
/// * the command exits with a non-success exit code.
/// * the `stdout` or `stderr` output fails to parse to `UTF-8`.
pub fn capture_command_output<I, S>(command: &str, args: I) -> anyhow::Result<String>
where
    I: IntoIterator<Item = S>,
    S: AsRef<OsStr>,
{
    let output = process::Command::new(command)
        .args(args)
        .output()
        .context(format!("Failed to execute {command}."))?;
    if !output.status.success() {
        let stderr = String::from_utf8(output.stderr).context(format!("Could not parse {command} stderr output."))?;
        eprintln!("Failed to execute {command}:");
        bail!("{stderr}")
    }

    String::from_utf8(output.stdout).context(format!("Could not parse {command} stdout output."))
}

/// Run a command via [`process::Command::new()`] while printing `stdout` and `stderr` to the user.
///
/// Handles string conversion, exit status and error output.
///
/// # Errors
/// Returns an error if:
/// * the command fails to execute.
/// * the command exits with a non-success exit code.
pub fn execute_command<I, S>(command: &str, args: I) -> anyhow::Result<()>
where
    I: IntoIterator<Item = S>,
    S: AsRef<OsStr>,
{
    let status = process::Command::new(command)
        .args(args)
        .status()
        .context(format!("Failed to execute {command}."))?;
    if !status.success() {
        bail!("Failed to execute {command}.")
    }

    Ok(())
}
