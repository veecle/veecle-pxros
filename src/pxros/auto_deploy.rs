//! Implementation of the auto-spawn feature of Pxros tasks.
//!
//! See [crate::pxros_task] for usage details.
use pxros::bindings::{PxEvents_t, PxMc_t, PxOpool_t, PxPanic, PxPrio_t, PxTask_t, PxUInt_t};

use crate::pxros::tsim;

/// Definition of a task, used to start the task.
#[repr(C)]
pub struct TaskDefinition {
    /// The task that is called to start the task.
    ///
    /// This is not the task entry function, this function is invoked on the core
    /// at bootup and is expected not to block.
    pub entry: extern "C" fn(
        task_priority: PxPrio_t,
        events: PxEvents_t,
        mem_class: PxMc_t,
        object_pool: PxOpool_t,
    ) -> PxTask_t,
    /// The name of this task.
    ///
    /// This is used for logging purposes and has no semantic meaning.
    pub debug_name: &'static str,
    /// The priority of the task.
    pub priority: PxPrio_t,
    /// The core on which the entry function should be called.
    pub core: PxUInt_t,
}

/// Default auto-deploy macro.
#[no_mangle]
extern "C" fn RustDeploy(core_id: PxUInt_t) {
    defmt::trace!("Deploying tasks on core {}", core_id);

    extern "Rust" {
        /// This constant is defined in the linker script (`__rust_auto_task_start = .;`) and is
        /// populated by the linker. It points to the first byte of the `.rust_auto_tasks` section
        /// in the resulting binary. The contents of this section are defined by the
        /// [`pxros_task`](crate::pxros_task) macro.
        #[link_name = "__rust_auto_task_start"]
        static SECTION_START: TaskDefinition;
        /// See `SECTION_START` for details.
        #[link_name = "__rust_auto_task_end"]
        static SECTION_END: TaskDefinition;
    }

    let section_start = unsafe { &SECTION_START as *const TaskDefinition } as usize;
    let section_end = unsafe { &SECTION_END as *const TaskDefinition } as usize;
    let section_size = section_end - section_start;
    let element_count = section_size / core::mem::size_of::<TaskDefinition>();

    // Sanity check: if the section only contains TaskDefinition, its size must
    // be a multiple of that size
    if element_count * core::mem::size_of::<TaskDefinition>() != section_size {
        unsafe { PxPanic() }
    }

    let tasks = unsafe { core::slice::from_raw_parts(section_start as *const TaskDefinition, element_count) };

    if tasks.is_empty() {
        defmt::warn!("No tasks are spawned automatically, this instance does nothing.");
    }

    if tsim::is_running_on_tsim() {
        defmt::warn!("Detected to be running in the TSIM - deploying all tasks on core {}", core_id);
    }

    let tasks_to_deploy = tasks.iter().filter(|to_deploy| {
        if to_deploy.core == core_id {
            true
        } else if tsim::is_running_on_tsim() {
            defmt::warn!(
                "Task {:?} was meant to run on core {} but will now run on core {}",
                to_deploy.debug_name,
                to_deploy.core,
                core_id
            );
            true
        } else {
            false
        }
    });

    for task in tasks_to_deploy {
        defmt::trace!("Deploying task {:?} on core {} with priority {}", task.debug_name, task.core, task.priority.0);

        let pool = PxOpool_t::default();
        let class = PxMc_t::default();

        if let Err(error) = (task.entry)(task.priority, PxEvents_t(0), class, pool).checked() {
            panic!("Failed to start task {:?}, error {:?}", task.debug_name, error);
        }
    }
}
