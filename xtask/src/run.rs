use std::env;

use crate::{build, execute_command};

/// Compiles and runs a PXROS Rust binary on a connected board over `tricore-probe`.
///
/// # Errors
/// Returned errors should be user-facing.
pub fn run(options: build::Options) -> anyhow::Result<()> {
    build(options.clone())?;
    let elf_path = env::current_dir()?
        .join("target/app-tc37x")
        .join("tc37x_pxros_bsp_example.elf");
    execute_command("tricore-probe", [
        "--log-level",
        options.probe_log_level.into(),
        &elf_path.to_string_lossy(),
    ])?;
    Ok(())
}
