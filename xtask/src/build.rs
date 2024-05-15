use std::path::PathBuf;
use std::{env, fs, process};

use anyhow::{bail, Context};
use clap::Parser;
use normpath::PathExt;

use crate::{detect_tricore_toolchain, execute_command, query_workspace_root_location};

/// CLI interface build options.
#[derive(Debug, Parser, Clone)]
pub struct Options {
    /// Build in release mode. !!! CURRENTLY NOT SUPPORTED !!!
    #[clap(long)]
    pub release: bool,

    /// Tricore-probe log level.
    #[arg(long, value_enum, required = false, default_value_t = LogLevel::Info)]
    pub probe_log_level: LogLevel,

    /// Application folder path relative to the workspace root.
    #[arg(long, required = false, default_value = default_app_folder_path().into_os_string())]
    pub app_folder: PathBuf,

    /// Path where cmake expects the compiled Rust task lib relative to the workspace root.
    #[arg(long, required = false, default_value = default_rust_task_lib_path().into_os_string())]
    pub rust_task_lib_path: PathBuf,

    /// Binary name (Rust package within this workspace).
    #[arg(long)]
    pub binary: String,

    /// Tricore target.
    #[arg(long, value_enum)]
    pub target: TricoreTarget,

    /// Specific rust toolchain to use.
    ///
    /// Will attempt to find any tricore toolchain if not specified.
    #[arg(long, required = false)]
    pub toolchain: Option<String>,

    /// make job count ("-j").
    #[arg(long, required = false, default_value_t = 4)]
    pub jobs: usize,

    /// Compile Infineon iLLD drivers from sources, else use the precompiled ones.
    ///
    /// This is an expensive operation so it is disabled by default.
    #[arg(long, required = false)]
    pub build_illd: bool,

    /// Include the Ethernet driver task into PXROS task list.
    ///
    /// This is required if the Ethernet driver is used.
    #[arg(long, required = false)]
    pub include_ethernet_task: bool,
}

/// Tricore probe log level.
#[derive(clap::ValueEnum, Clone, Debug)]
pub enum LogLevel {
    Warn,
    Info,
    Debug,
    Trace,
}

impl From<LogLevel> for &str {
    fn from(value: LogLevel) -> Self {
        match value {
            LogLevel::Warn => "warn",
            LogLevel::Info => "info",
            LogLevel::Debug => "debug",
            LogLevel::Trace => "trace",
        }
    }
}

/// The tricore target architecture to compile for.
#[derive(clap::ValueEnum, Clone, Copy, Debug)]
pub enum TricoreTarget {
    TC162,
    TC18,
    TC18A,
}

impl From<TricoreTarget> for &str {
    fn from(value: TricoreTarget) -> Self {
        match value {
            TricoreTarget::TC162 => "tc162-htc-none",
            TricoreTarget::TC18 => "tc18-htc-none",
            TricoreTarget::TC18A => "tc18a-htc-none",
        }
    }
}

/// Provides the default app folder path for the CLI interface.
pub fn default_app_folder_path() -> PathBuf {
    PathBuf::from("app-tc37x")
}

/// Provides the default rust task lib path for the CLI interface.
pub fn default_rust_task_lib_path() -> PathBuf {
    default_app_folder_path().join("ld/librust_tasks.a")
}

/// Compiles a PXROS Rust binary.
///
/// # Errors
/// Returned errors should be user-facing.
pub fn build(options: Options) -> anyhow::Result<()> {
    let workspace_root_dir = query_workspace_root_location()?;
    let debug_release_path_component = if options.release { "release" } else { "debug" };
    let rust_lib_name = format!("lib{}.a", options.binary.replace('-', "_"));

    let toolchain = {
        if let Some(toolchain) = options.toolchain {
            toolchain.clone()
        } else if let Ok(tricore_toolchain) = detect_tricore_toolchain() {
            tricore_toolchain.clone()
        } else {
            bail!("Failed to autodetect toolchain, please supply installed toolchain.");
        }
    };

    let mut rustup_args = vec![
        "run",
        &toolchain,
        "cargo",
        "build",
        "--color",
        "always",
        "--target",
        options.target.into(),
        "--package",
        &options.binary,
    ];
    if options.release {
        rustup_args.push("-r");
    }

    execute_command("rustup", rustup_args)?;

    let rust_lib_path = workspace_root_dir
        .join("target")
        .join::<&str>(options.target.into())
        .join(debug_release_path_component)
        .join(rust_lib_name);

    let pxros_lib_path = workspace_root_dir.join(options.rust_task_lib_path);
    fs::copy(rust_lib_path, pxros_lib_path)?;

    let current_directory = env::current_dir()?;

    let build_directory = workspace_root_dir.join("target/app-tc37x");

    env::set_current_dir(workspace_root_dir.join(options.app_folder))?;

    let build_directory = build_directory.to_str().expect("Path should be valid UTF-8.");
    let make_result = make_pxros(build_directory, options.jobs, options.build_illd, options.include_ethernet_task);

    env::set_current_dir(current_directory)?;

    make_result
}

/// Helper to transform a &str to a normalized String.
fn to_os_path(path: &str) -> String {
    PathBuf::from(path)
        .normalize()
        .expect("Could not normalize path")
        .into_os_string()
        .into_string()
        .unwrap()
}

/// Wraps calls to PXROS (c)make.
///
/// This function is not inlined because we want to revert and go back to our
/// old folder even if we encounter an error.
fn make_pxros(
    build_dir: &str,
    make_job_count: usize,
    build_illd: bool,
    include_ethernet_task: bool,
) -> anyhow::Result<()> {
    let mut env_vars = vec![
        ("PXROS_ROOT_PATH", to_os_path(pxros_hr::TRI_8_2_1_EVAL_KERNEL)),
        ("PXROS_UTILS", to_os_path(pxros_hr::TRI_8_2_1_EVAL_UTILS)),
        ("ILLD_BASE", to_os_path(illd_rs::ILLD_BASE)),
        ("ILLD_PRECOMPILED", to_os_path(illd_rs::ILLD_LIB)),
    ];

    if build_illd {
        env_vars.push(("BUILD_ILLD", "1".into()));
    }

    if include_ethernet_task {
        env_vars.push(("INCLUDE_ETHERNET_DRIVER_TASK", "1".into()));
    }

    if !process::Command::new("cmake")
        .args(["-B", build_dir, "--fresh", "-G", "Unix Makefiles"])
        .envs(env_vars.clone())
        .status()
        .context("Failed to execute cmake.")?
        .success()
    {
        bail!("Failed to execute cmake.")
    }

    if !process::Command::new("make")
        .args(["--no-print-directory", "-C", build_dir, "clean"])
        .envs(env_vars.clone())
        .status()
        .context("Failed to execute make.")?
        .success()
    {
        bail!("Failed to execute make.")
    }

    if !process::Command::new("make")
        .envs(env_vars)
        .args([
            "--no-print-directory",
            "-C",
            build_dir,
            "-j",
            &make_job_count.to_string(),
        ])
        .status()
        .context("Failed to execute cmake.")?
        .success()
    {
        bail!("Failed to execute cmake.")
    }

    Ok(())
}
