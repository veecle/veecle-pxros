use std::env;
use std::process::{Command, Stdio};

use anyhow::{bail, Context};

use crate::build;

/// Build and emulate the target binary on the `TSIM` emulator.
pub fn emulate(options: &build::Options) -> anyhow::Result<()> {
    build(options.clone())?;

    let elf_path = env::current_dir()?
        .join("target/app-tc37x")
        .join("tc37x_pxros_bsp_example.elf");

    let tsim_config_path = env::current_dir()?.join("tsim_config");

    let mut tsim_command = Command::new("tsim16p_e");
    let spawned_tsim = tsim_command
        .args([
            "-tc162p",
            "-config_file_path",
            tsim_config_path.to_str().unwrap(),
            "-disable-watchdog",
            "-s",
            "-U",
            "-x",
            "0000000",
            "-H",
            "-t",
            "-o",
            elf_path.to_str().unwrap(),
        ])
        .stdout(Stdio::piped())
        .spawn()
        .context("Cannot run TSIM emulator")?;

    let mut defmt_print = Command::new("defmt-print");
    let mut spawned_defmt = defmt_print
        .args(["-e", elf_path.to_str().unwrap()])
        .stdin(Stdio::from(spawned_tsim.stdout.unwrap()))
        .spawn()
        .context("Cannot spawn defmt-print process")?;

    let exit_status = spawned_defmt
        .wait()
        .context("TSIM emulator didn't terminate correctly")?;

    if !exit_status.success() {
        bail!("TSIM emulator failed with exit status {exit_status}")
    }

    Ok(())
}
