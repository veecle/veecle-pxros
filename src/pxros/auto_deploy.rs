//! Implementation of the task auto-creation feature for [`PxrosTask`](crate::pxros::task::PxrosTask).
use pxros::bindings::{PxGetId, PxUInt_t};

use crate::pxros::task::TaskCreationConfig;
use crate::pxros::tsim;

/// Creates all [`PxrosTask`](crate::pxros::task::PxrosTask) defined for auto-creation.
///
/// The `#[no_mangle]` defined `TASK_LIST` static is used to iterate over [`TaskCreationConfig`] and to create the
/// tasks.
/// If the program is run on the TSIM, all tasks will be created on the same core irrespective of their core
/// configuration.
#[no_mangle]
extern "C" fn RustDeploy(core_id: PxUInt_t) {
    let task_id = PxGetId().id();
    defmt::debug!("Deploying tasks on core {}.", core_id);

    extern "Rust" {
        static TASK_LIST: &'static [TaskCreationConfig];
    }

    let task_list: &[TaskCreationConfig] = unsafe { TASK_LIST };

    if task_list.is_empty() {
        defmt::warn!("[{}] No tasks are spawned automatically, this instance does nothing.", task_id);
    }

    let tasks_to_deploy = task_list.iter().filter(|to_deploy| {
        if to_deploy.core() == core_id {
            true
        } else if tsim::is_running_on_tsim() {
            defmt::warn!(
                "[{}] Task {} was meant to run on core {} but will now run on core {}.",
                task_id,
                to_deploy.task_creation_identifier(),
                to_deploy.core(),
                core_id
            );
            true
        } else {
            false
        }
    });

    if tsim::is_running_on_tsim() {
        defmt::warn!("[{}] Detected to be running in the TSIM - deploying all tasks on core {}.", task_id, core_id);
    }

    for task_creation_config in tasks_to_deploy {
        defmt::debug!(
            "[{}] Spawning: {} on core {}.",
            task_id,
            task_creation_config.task_creation_identifier(),
            core_id
        );
        match task_creation_config.create_task().checked() {
            Ok(task) => {
                defmt::info!(
                    "[{}] Spawned task (ID: {}, creation ident: {}) successfully on core {}.",
                    task_id,
                    task.id(),
                    task_creation_config.task_creation_identifier(),
                    core_id
                )
            },
            Err(error) => {
                defmt::panic!(
                    "[{}] Failed to start task {:?} on core {} with error: {:?}",
                    task_id,
                    task_creation_config.task_creation_identifier(),
                    core_id,
                    error
                )
            },
        }
    }
}
