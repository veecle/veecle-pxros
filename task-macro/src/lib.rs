use proc_macro::TokenStream;

mod task;

/// This macro allows to annotate a valid function as pxros task; attributes
/// allows for configuration of various task's aspects
///
/// ```
/// # pub mod veecle_pxros {
/// #   pub mod pxros {
/// #       pub mod task {
/// #           use ::pxros::bindings::*;
/// #           use ::pxros::PxResult;
/// #           pub struct TaskName;
/// #           pub fn run_task(_: impl FnMut(PxMbx_t) -> PxResult<()>, _: PxTask_t, _: PxMbx_t, _: Option<&TaskName>) {
/// #               todo!()
/// #           }
/// #       }
/// #   }
/// # }
/// # use task_macro::pxros_task;
/// # use pxros::bindings::PxMbx_t;
/// # use pxros::PxResult;
///
/// /// Prepare the structure to manually spawn a pxros task; core & priority
/// /// selection are deferred to the spawner in "C"
/// #[pxros_task]
/// fn px_task(mailbox: PxMbx_t) -> PxResult<()> {
///     // Task logic here
/// #   todo!()
/// }
/// ```
///
/// In this context
/// * `mailbox`: an handle to the task's mailbox; this is used to receive messages or events from other tasks.
///
/// In its primitive usage, the macro does nothing more than creating the task's
/// constructor method: the spawning of the task **must** be done in PxROS via
/// extern `Rust` call.
///
/// To facilitate spawning from Rust, the macro provides an additional attribute
/// `auto_spawn` which ensures the task is spawned without requiring any change
/// in the PxROS code. **This is the preferred method of spawning when no complex
/// configuration is required**.
///
/// ```
/// # pub mod veecle_pxros {
/// #   pub mod pxros {
/// #       pub mod auto_deploy {
/// #           use ::pxros::bindings::*;
/// #           pub struct TaskDefinition {
/// #               pub entry: extern "C" fn(_: PxPrio_t, _: PxEvents_t, _: PxMc_t, _: PxOpool_t) -> PxTask_t,
/// #               pub debug_name: &'static str,
/// #               pub priority: PxPrio_t,
/// #               pub core: PxUInt_t,
/// #           }
/// #       }
/// #       pub mod task {
/// #           use ::pxros::bindings::*;
/// #           use ::pxros::PxResult;
/// #           pub struct TaskName;
/// #           pub fn run_task(_: impl FnMut(PxMbx_t) -> PxResult<()>, _: PxTask_t, _: PxMbx_t, _: Option<&TaskName>) {
/// #               todo!()
/// #           }
/// #       }
/// #   }
/// # }
/// # use task_macro::pxros_task;
/// # use pxros::bindings::PxMbx_t;
/// # use pxros::PxResult;
///
/// /// Automatically spawn a pxros task in core 0 with priority 15
/// #[pxros_task(auto_spawn(core = 0, priority = 15))]
/// fn px_task(mailbox: PxMbx_t) -> PxResult<()> {
///     // Task logic here
/// #   todo!()
/// }
/// ```
///
/// `auto_spawn` supports two attributes:
/// * `core = N`, where N is the core where the task is meant to be spawned & pinned to
/// * `priority = P`, where P is the task's priority; higher priority (lower P) tasks can preempt lower priority ones.
///
/// ## Additional attributes
/// Outside of `auto_spawn` the macro supports other attributes:
/// * `stack_size`: the desired stack size in bytes. If not set the default is 4KB
/// * `interrupt_stack_size`: the desired interrupt stack size in bytes. If not set the default is 32B
/// * `task_name`: the task "name" when queried from through name server. If set, this task will be automatically
///   registered to the NameServer on spawn.
///
/// ```
/// # pub mod veecle_pxros {
/// #   pub mod pxros {
/// #       pub mod name_server {
/// #           pub struct TaskName;
/// #           impl TaskName { pub const fn new(_: i32) -> Self { Self } }
/// #       }
/// #       pub mod task {
/// #           use ::pxros::bindings::*;
/// #           use ::pxros::PxResult;
/// #           use super::name_server::TaskName;
/// #           pub fn run_task(_: impl FnMut(PxMbx_t) -> PxResult<()>, _: PxTask_t, _: PxMbx_t, _: Option<&TaskName>) {
/// #               todo!()
/// #           }
/// #       }
/// #   }
/// # }
/// # use task_macro::pxros_task;
/// # use pxros::bindings::PxMbx_t;
/// # use pxros::PxResult;
/// use veecle_pxros::pxros::name_server::TaskName;
///
/// /// My unique task name
/// const MY_NAME: TaskName = TaskName::new(13);
///
/// /// Ensure that the task is registered in the name server with name `13`; other tasks
/// /// can then obtain the task handle by query the name server with `13`
/// #[pxros_task(task_name = MY_NAME, stack_size = 32_000)]
/// fn px_task(mailbox: PxMbx_t) -> PxResult<()> {
///     // Task logic here
/// #   todo!()
/// }
/// ````
///
/// * `privileges`: tasks privilege level (required for supervisor actions or to access protected peripherals). This can
///   be set via [pxros::mem::Privileges]. If not set, this the permission level is set to `Privileges::NoDirectAccess`
/// * `regions`: describe one or more MPU regions the task can access; this must be sent by pointing to a `const` array
///   of [pxros::mem::MemoryRegion]. The kernel shall ensure the task can access those regions without triggering a trap
///
/// ```
/// # pub mod veecle_pxros {
/// #   pub mod pxros {
/// #       pub mod task {
/// #           use ::pxros::bindings::*;
/// #           use ::pxros::PxResult;
/// #           pub struct TaskName;
/// #           pub fn run_task(_: impl FnMut(PxMbx_t) -> PxResult<()>, _: PxTask_t, _: PxMbx_t, _: Option<&TaskName>) {
/// #               todo!()
/// #           }
/// #       }
/// #   }
/// # }
/// # use pxros::mem::{MemoryRegion, Privileges::DirectAccess};
/// # use pxros::PxResult;
/// # use task_macro::pxros_task;
/// # use pxros::bindings::{PxMbx_t, PxProtectType_t};
/// const EXTRA_REGION: [MemoryRegion; 1] = [MemoryRegion::new(
///     0xB000_FF00..0xB000_FFFF,
///     PxProtectType_t::WRProtection,
/// )];
///
/// #[pxros_task(regions = EXTRA_REGION, privileges = DirectAccess)]
/// fn px_task(mailbox: PxMbx_t) -> PxResult<()> {
///     // Task logic here
/// #   todo!()
/// }
/// ````
///
/// Attributes can be combined at user discretion
#[proc_macro_attribute]
pub fn pxros_task(arguments: TokenStream, function: TokenStream) -> TokenStream {
    task::run(function.into(), arguments.into())
        .unwrap_or_else(|error| error.emit_as_item_tokens())
        .into()
}
