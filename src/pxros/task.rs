//! Utilities to simplify the creation and execution of tasks.
use pxros::bindings::*;
use pxros::PxResult;

use super::name_server::TaskName;

/// Runs a PXROS task and performs basic logging and error checking.
///
/// Entry function of a PXROS task.
/// Returned errors will be printed.
///
/// Used by [crate::pxros_task] to simplify error propagation.
pub fn run_task<F>(mut main: F, task: PxTask_t, mailbox: PxMbx_t, name_server: Option<&TaskName>)
where
    F: FnMut(PxMbx_t) -> PxResult<()>,
{
    defmt::debug!("[Task: {}] spawned.", task.id());

    if let Some(name_server) = name_server {
        defmt::info!("[Task: {}] registering to NameServer.", task.id());
        name_server.register(task);
    }

    match main(mailbox) {
        Ok(()) => {
            defmt::debug!("[TASK: {}] terminated.", task.id());
        },
        Err(error) => {
            defmt::error!("[TASK: {}] terminated with error {:?}.", task, error);
        },
    }
}
