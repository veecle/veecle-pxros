use crate::execute_command;

// Keep in sync with CI!
/// Runs all CI checks locally.
pub fn check() -> anyhow::Result<()> {
    // Clippy:
    execute_command("cargo", ["clippy", "--workspace", "--all-targets"])?;
    // Tests:
    execute_command("cargo", ["test", "-p", "veecle-pxros", "--all-targets", "--all-features"])?;
    execute_command("cargo", ["test", "-p", "task-macro", "--all-targets", "--all-features"])?;
    execute_command("cargo", ["test", "-p", "veecle-pxros", "--doc", "--no-default-features"])?;
    execute_command("cargo", ["test", "-p", "task-macro", "--doc", "--all-features"])?;
    // Fmt:
    execute_command("cargo", ["fmt", "--check", "--all"])?;
    // Doc:
    execute_command("cargo", ["doc", "--workspace", "--all-features"])?;

    Ok(())
}
