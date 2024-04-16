//! Utilities to simplify the creation and execution of tasks.

use core::ffi::CStr;
use core::marker::PhantomData;

use pxros::bindings::*;
use pxros::mem::{MemoryRegion, Privileges, StackSpec};
use pxros::PxResult;

use super::name_server::TaskName;

/// Trait defining a PXROS task.
///
/// This trait allows defining a PXROS task by overwriting its default functions.
/// Apart from [`PxrosTask::task_main`], all functions provide a reasonable default implementation.
/// The [`PxrosTask::task_main`] functions needs to be overwritten to execute custom logic within a task.
///
/// # Auto-Creation
/// To auto-create tasks when PXROS's init-task executes, a `static` `TASK_LIST` has to be defined.
/// By marking it `#[no_mangle]`, it becomes possible to link this into the auto-deploy module of this crate.
/// The PXROS init-task will call the `RustDeploy` function, which in turn will iterate over the `TASK_LIST` slice and
/// create any task for which a [`TaskCreationConfig`] is present.
/// If no task should be auto-created, an empty `TASK_LIST` is required.
/// Not defining a `TASK_LIST` will result in a linker error.
///
/// # Usage from C
/// `#[no_mangle]` or `#[export_name = "..."]` cannot be applied to trait methods.
/// This makes it hard to link trait functions from C.
/// Wrapping a trait function in a free function or using functions pointers works around this issue.
/// Functions for use in C must still be marked with `extern "C"` to use the C ABI.
///
/// # Example
/// ```rust
/// # use std::ffi::CStr;
/// # use pxros::bindings::PxMbx_t;
/// # use pxros::PxResult;
/// # use veecle_pxros::pxros::task::{PxrosTask, TaskCreationConfig, TaskCreationConfigBuilder};
/// // The TASK_LIST static is required to defined exactly once per PXROS application.
/// #[no_mangle]
/// static TASK_LIST: &[TaskCreationConfig] = &[
///     TaskCreationConfigBuilder::from_task::<Task>()
///         .override_core(0)
///         .override_priority(15)
///         .build("Task_Creation")];
///
/// struct Task;
/// impl PxrosTask for Task {
///     fn debug_name() -> &'static CStr {
///         CStr::from_bytes_with_nul("TaskDebugName\0".as_bytes())
///             .expect("The debug name should be a valid, zero-terminated C string.")
///     }
///
///     fn task_main(_mailbox: PxMbx_t) -> PxResult<()> {
///         let (task_debug_name, task_id) = <Self as PxrosTask>::log_id();
///
///         defmt::info!("[{}: {}] Hello!", task_debug_name, task_id,);
///         Ok(())
///     }
/// }
/// ```
pub trait PxrosTask {
    /// The main function of the task.
    // We cannot call this function `main` and use it in doctest.
    // The examples add a main function for tests, but will not do this if a main() function is present.
    // Since this only seems to check functions names, this does not seem to take traits into accounts.
    // https://doc.rust-lang.org/rustdoc/write-documentation/documentation-tests.html#pre-processing-examples
    fn task_main(_mailbox: PxMbx_t) -> PxResult<()>;

    /// Returns the [`TaskName`] to register for this task. If none is returned, the task will not be registered.
    ///
    /// Override this function to change the task name.
    ///
    /// Defaults to `None`.
    fn task_name() -> Option<TaskName> {
        None
    }

    /// Returns the debug name of the task.
    ///
    /// Corresponds to [`PxTaskSpec_T::ts_name`].
    ///
    /// Override this function to provide custom values for the [`PxTaskSpec_T`] of this task.
    ///
    /// Defaults to "Default_Task_Debug_Name".
    fn debug_name() -> &'static CStr {
        CStr::from_bytes_with_nul("Default_Task_Debug_Name\0".as_bytes())
            .expect("The debug name should be a valid, zero-terminated C string.")
    }

    /// Returns the priority with which this task will be spawned.
    ///
    /// Override this function to change the priority.
    ///
    /// Defaults to 25.
    fn priority() -> PxPrio_t {
        PxPrio_t(25)
    }

    /// The core on which the auto-spawn will call the entry function of this task.
    ///
    /// Override this function to change the core.
    ///
    /// Defaults to 0.
    fn core() -> u32 {
        0
    }

    /// Returns the access rights of the task.
    ///
    /// Corresponds to [`PxTaskSpec_T::ts_accessrights`].
    ///
    /// Override this function to provide custom values for the [`PxTaskSpec_T`] of this task.
    ///
    /// Defaults to 0 (no access rights).
    fn access_rights() -> u32 {
        0
    }

    /// Returns the stack size of the task in bytes.
    ///
    /// Corresponds to [`PxTaskSpec_T::ts_taskstack`].
    ///
    /// If PXROS triggers traps at seemingly random locations, a too small stack size may be the issue.
    ///
    /// Override this function to provide custom values for the [`PxTaskSpec_T`] of this task.
    ///
    /// Defaults to 4096.
    fn task_stack_size() -> u32 {
        4096
    }

    /// Returns the interrupt stack size of the task in bytes.
    ///
    /// Corresponds to [`PxTaskSpec_T::ts_inttaskstack`].
    ///
    /// Override this function to provide custom values for the [`PxTaskSpec_T`] of this task.
    ///
    /// Defaults to 32.
    fn interrupt_stack_size() -> u32 {
        32
    }

    /// Returns the abort stack size of the task in abort frames.
    ///
    /// Corresponds to [`PxTaskSpec_T::ts_abortstacksize`].
    ///
    /// Override this function to provide custom values for the [`PxTaskSpec_T`] of this task.
    ///
    /// Defaults to 32.
    fn abort_stack_size() -> u32 {
        1
    }

    /// Returns the privileges of the task.
    ///
    /// Corresponds to [`PxTaskSpec_T::ts_privileges`].
    ///
    /// Override this function to provide custom values for the [`PxTaskSpec_T`] of this task.
    ///
    /// Defaults to [`Privileges::NoDirectAccess`].
    fn privileges() -> PxArg_t {
        PxArg_t(Privileges::NoDirectAccess as i32)
    }

    /// Returns the task context.
    ///
    /// To ensure the lifetime required by PXROS, this function returns a `'static` lifetime reference instead of a raw
    /// pointer.
    ///
    /// Corresponds to [`PxTaskSpec_T::ts_context`].
    ///
    /// Override this function to provide custom values for the [`PxTaskSpec_T`] of this task.
    ///
    /// Check the source to view the default.
    fn task_context() -> &'static PxTaskContext_T {
        static TASK_CONTEXT: PxTaskContext_T = PxTaskContext_T {
            protection: [
                MemoryRegion::new(0..0, PxProtectType_t::NoAccessProtection),
                MemoryRegion::new(u32::MAX..u32::MAX, PxProtectType_t::NoAccessProtection),
            ],
        };
        &TASK_CONTEXT
    }

    /// Returns the task activation events.
    ///
    /// If zero is returned, the task will start immediately.
    ///
    /// Override this function to customize the tasks activation events.
    ///
    /// Defaults to 0.
    fn activation_events() -> PxEvents_t {
        PxEvents_t(0)
    }

    /// Returns the memory class of the task.
    ///
    /// Corresponds to [`PxTaskSpec_T::ts_mc`].
    ///
    /// Override this function to provide custom values for the [`PxTaskSpec_T`] of this task.
    ///
    /// Defaults to [`PxMc_t::default`].
    fn memory_class() -> PxMc_t {
        PxMc_t::default()
    }

    /// Returns the object pool of the task.
    ///
    /// Corresponds to [`PxTaskSpec_T::ts_opool`].
    ///
    /// Override this function to provide custom values for the [`PxTaskSpec_T`] of this task.
    ///
    /// Defaults to [`PxOpool_t::default`].
    fn object_pool() -> PxOpool_t {
        PxOpool_t::default()
    }

    /// Returns the memory protection regions of the task.
    ///
    /// To ensure the lifetime required by PXROS, this function returns a `'static` lifetime slice instead of a raw
    /// pointer. The [´MemoryRegion`] slice needs to be terminated by an all-zero element.
    /// For convenience this can be created by calling [`MemoryRegion::zeroed`].
    ///
    /// Corresponds to [`PxTaskSpec_T::ts_protect_region`].
    ///
    /// Override this function to provide custom values for the [`PxTaskSpec_T`] of this task.
    ///
    /// Check the source to view the default.
    fn memory_protection_regions() -> &'static [MemoryRegion] {
        // TASK_REGIONS needs to be const or static to get linked into the RODATA section by
        // default. This is required to satisfy PXROS requirements on the pointer to the task
        // regions array to be constant.
        static MEMORY_REGIONS: [MemoryRegion; 2] = [
            // Add RODATA & DATA to memory regions as the second to last element.
            // Defmt uses RODATA to store logging information.
            // Upper and lower bound are taken from the linker script for the dlmu_cpu0 to
            // dlmu_cpu3.
            // The last kilobyte was skipped for example 4 exercise 1, but is now superfluous.
            MemoryRegion::new(0xB0000000..0xB0030000 - 1024, PxProtectType_t::WRProtection),
            MemoryRegion::zeroed(),
        ];

        &MEMORY_REGIONS
    }

    /// Returns the timeslicing reload value for the task.
    ///
    /// Corresponds to [`PxTaskSpec_T::ts_timeslices`].
    ///
    /// Override this function to provide custom values for the [`PxTaskSpec_T`] of this task.
    ///
    /// Defaults to 0 (does not use timeslicing).
    fn timeslices() -> PxTicks_t {
        PxTicks_t(0)
    }

    /// Returns the task's debug name and ID for logging purposes.
    fn log_id<'a>() -> (&'a str, u16) {
        let task_debug_name = Self::debug_name()
            .to_str()
            .expect("Task debug name should be valid UTF-8.");
        let current_task_id = PxGetId().id();
        (task_debug_name, current_task_id)
    }

    /// The task entry function.
    ///
    /// This function is called by PXROS when the task is started.
    /// Internally it calls the [`Self::task_main`] function.
    /// To comply with PXROS requirements, this function uses the C ABI (`extern "C"`).
    ///
    /// Override this function to customize the entrypoint of the task.
    ///
    /// Defaults to registering the [`Self::task_name`] before executing [`Self::task_main`].
    extern "C" fn entry_function(task: PxTask_t, mailbox: PxMbx_t, _activation_events: PxEvents_t) {
        let (task_debug_name, current_task_id) = Self::log_id();
        defmt::debug!("[{}: {}] Starting execution.", task_debug_name, current_task_id);

        if let Some(task_name) = Self::task_name() {
            defmt::debug!("[{}: {}] Registering to NameServer.", task_debug_name, current_task_id);
            task_name.register(task);
            defmt::debug!("[{}: {}] Registered to NameServer.", task_debug_name, current_task_id);
        }

        defmt::debug!("[{}: {}] Starting execution of main function.", task_debug_name, current_task_id);
        match Self::task_main(mailbox) {
            Ok(()) => {
                defmt::debug!("[{}: {}] Terminated successfully.", task_debug_name, current_task_id);
            },
            Err(error) => {
                defmt::error!("[{}: {}] Terminated with error: {:?}", task_debug_name, current_task_id, error);
            },
        }
    }

    /// Generates the task's [`PxTaskSpec_T`].
    ///
    /// This function uses the C ABI (`extern "C"`) to allow using C functions in the Rust task creation mechanism.
    ///
    /// Override this function to customize the [`PxTaskSpec_T`] generation of the task.
    ///
    /// Defaults to using [`Self`] trait functions to fill [`PxTaskSpec_T`].
    extern "C" fn generate_specification() -> PxTaskSpec_T {
        let (task_debug_name, current_task_id) = Self::log_id();
        defmt::debug!("[{}] Generating task specification for: {}", current_task_id, task_debug_name);

        PxTaskSpec_T {
            ts_name: Self::debug_name().as_ptr() as *const PxChar_t,
            ts_fun: PxTaskfun_t(Some(Self::entry_function)),

            // Memory
            ts_mc: Self::memory_class(),
            ts_opool: Self::object_pool(),

            // Stack
            ts_taskstack: StackSpec::new(Self::task_stack_size(), Self::memory_class()),
            ts_inttaskstack: StackSpec::new(Self::interrupt_stack_size(), Self::memory_class()),
            ts_abortstacksize: Self::abort_stack_size(),

            // Protection
            ts_context: PxTaskContext_ct(Self::task_context() as *const _),
            ts_protect_region: PxProtectRegion_ct(Self::memory_protection_regions().as_ptr()),
            ts_privileges: Self::privileges(),
            ts_accessrights: Self::access_rights(),

            // Scheduling
            ts_timeslices: Self::timeslices(),

            // Ignored by PXROS, parameters are only kept for compatibility:
            ts_prio: PxPrio_t(0),
            ts_actevents: PxEvents_t(0),

            // Not implemented by PXROS:
            ts_sched_extension: PxTaskSchedExt_ct(core::ptr::null()),
            ts_sched_initarg: PxArg_t(0),
        }
    }

    /// Creates the task by calling [`PxTaskCreate`].
    ///
    /// This function uses the C ABI (`extern "C"`) to allow using C functions in the Rust task creation mechanism.
    ///
    /// Override this function to customize this task's creation.
    ///
    /// Defaults to generating the task specification via `task_spec_generation_function` and checking the result
    /// of the [`PxTaskCreate`] call.
    extern "C" fn create(
        object_pool: PxOpool_t,
        priority: PxPrio_t,
        activation_events: PxEvents_t,
        task_spec_generation_function: extern "C" fn() -> PxTaskSpec_T,
    ) -> PxTask_t {
        {
            let task_spec = task_spec_generation_function();

            let task_spec_debug_name = task_spec.ts_name;
            let task_spec_debug_name = if !task_spec_debug_name.is_null() {
                let task_debug_name = unsafe { CStr::from_ptr(task_spec_debug_name as *const core::ffi::c_char) };
                task_debug_name
                    .to_str()
                    .expect("Task debug name should be valid UTF-8.")
            } else {
                ""
            };
            let (trait_task_debug_name, current_task_id) = Self::log_id();

            defmt::debug!(
                "[{}] Creating task using \"{}\" create function for task spec \"{}\".",
                current_task_id,
                trait_task_debug_name,
                task_spec_debug_name
            );

            // Create the task
            // Safety: not really checked... big TODO
            let result = unsafe {
                PxTaskCreate(object_pool, PxTaskSpec_ct(&task_spec as *const _), priority, activation_events)
            };

            match result.checked() {
                Ok(created_task) => defmt::debug!(
                    "[{}] Task creation success: [{}: {}]",
                    PxGetId().id(),
                    task_spec_debug_name,
                    created_task.id()
                ),
                Err(error) => {
                    defmt::error!("[{}: {}] Task creation failed: {}", task_spec_debug_name, PxGetId().id(), error)
                },
            }
            result
        }
    }
}

/// Task creation configuration used to override aspects of the [`TaskNativeCreationConfig`] in [`TaskCreationConfig`].
// For function pointer reasoning, see `TaskNativeCreationConfig`.
#[derive(Debug, Copy, Clone)]
pub struct TaskCreationConfigOverrides {
    core: Option<PxUInt_t>,
    object_pool: Option<PxOpool_t>,
    priority: Option<PxPrio_t>,
    activation_events: Option<PxEvents_t>,
    task_creation_function:
        Option<extern "C" fn(PxOpool_t, PxPrio_t, PxEvents_t, extern "C" fn() -> PxTaskSpec_T) -> PxTask_t>,
    specification_function: Option<extern "C" fn() -> PxTaskSpec_T>,
}

impl TaskCreationConfigOverrides {
    /// Returns a default [`TaskCreationConfigOverrides`].
    ///
    /// The [`Default`] trait cannot be used in `const` contexts, so this method provides the
    /// same functionality.
    pub const fn const_default() -> Self {
        Self {
            core: None,
            object_pool: None,
            priority: None,
            activation_events: None,
            task_creation_function: None,
            specification_function: None,
        }
    }
}

/// Task creation configuration derived from a [`PxrosTask`] implementation for use in [`TaskCreationConfig`].
// Function pointers:
// The task creation configuration must be creatable in `const` contexts for the current auto-start mechanism leveraging
// the linker. This means we cannot use any trait methods (at the time of writing, the const trait in Rust is not in a
// production-ready state). Using function pointers sidesteps the limitation and allows us to define a task creation
// configuration that, at runtime, can then use the provided functions to create a task specification.
#[derive(Debug, Copy, Clone)]
pub struct TaskNativeCreationConfig {
    core: fn() -> PxUInt_t,
    object_pool: fn() -> PxOpool_t,
    priority: fn() -> PxPrio_t,
    activation_events: fn() -> PxEvents_t,
    task_creation_function: extern "C" fn(PxOpool_t, PxPrio_t, PxEvents_t, extern "C" fn() -> PxTaskSpec_T) -> PxTask_t,
    specification_function: extern "C" fn() -> PxTaskSpec_T,
}

impl TaskNativeCreationConfig {
    /// Creates a new [`TaskNativeCreationConfig`] populated by generic type parameter `PT`'s associated functions.
    pub const fn from_task<PT>() -> Self
    where
        PT: PxrosTask,
    {
        Self {
            core: PT::core,
            object_pool: PT::object_pool,
            priority: PT::priority,
            activation_events: PT::activation_events,
            task_creation_function: PT::create,
            specification_function: PT::generate_specification,
        }
    }
}

/// Central configuration for creating tasks.
///
/// This provides a more ergonomic configuration mechanism than using the [`PxrosTask::create`] function directly.
/// All aspects of the creation process can be overridden from the defaults defined by the [`PxrosTask`] trait.
/// With this, tasks can be reused while modifying all aspects of the task specification and more.
/// Since tasks can be reused, so will their debug name.
/// To allow correlation between creation config and task creation, an identifier can be defined per
/// [`TaskCreationConfig`].
///
/// For ease of use, utility functions are provided for often used configuration overrides.
#[derive(Debug)]
pub struct TaskCreationConfig {
    task_creation_identifier: &'static str,
    task_config: TaskNativeCreationConfig,
    overrides: TaskCreationConfigOverrides,
}

impl TaskCreationConfig {
    /// Creates a new [`TaskCreationConfig`] from the provided identifier, configuration and overrides.
    pub const fn new(
        task_creation_identifier: &'static str,
        task_spawn_config: TaskNativeCreationConfig,
        overrides: TaskCreationConfigOverrides,
    ) -> Self {
        Self {
            task_creation_identifier,
            task_config: task_spawn_config,
            overrides,
        }
    }

    /// Creates a new [`TaskNativeCreationConfig`] derived from generic type parameter `PT`.
    pub const fn from_task<PT>(task_creation_identifier: &'static str) -> Self
    where
        PT: PxrosTask,
    {
        Self::new(
            task_creation_identifier,
            TaskNativeCreationConfig::from_task::<PT>(),
            TaskCreationConfigOverrides::const_default(),
        )
    }

    /// Creates a new task from the configuration and overrides.
    pub fn create_task(&self) -> PxTask_t {
        let object_pool = self.overrides.object_pool.unwrap_or((self.task_config.object_pool)());
        let priority = self.overrides.priority.unwrap_or((self.task_config.priority)());
        let activation_events = self
            .overrides
            .activation_events
            .unwrap_or((self.task_config.activation_events)());
        let task_creation_function = self
            .overrides
            .task_creation_function
            .unwrap_or(self.task_config.task_creation_function);
        let specification_function = self
            .overrides
            .specification_function
            .unwrap_or(self.task_config.specification_function);
        task_creation_function(object_pool, priority, activation_events, specification_function)
    }

    /// Returns the core the task will be spawned on.
    pub fn core(&self) -> PxUInt_t {
        self.overrides.core.unwrap_or((self.task_config.core)())
    }

    /// Returns an identifier allowing association between logs and spawned tasks.
    ///
    /// Multiple tasks can be created from the same implementation of [`PxrosTask`], all sharing the same debug name.
    /// To distinguish between different [`TaskCreationConfig`], this identifier is used.
    pub const fn task_creation_identifier(&self) -> &'static str {
        self.task_creation_identifier
    }
}

/// Marker type for [`TaskCreationConfigBuilder`].
pub struct NoCoreMarker;
/// Marker type for [`TaskCreationConfigBuilder`].
pub struct OverridesCoreMarker;
/// Marker type for [`TaskCreationConfigBuilder`].
pub struct NoObjectPoolMarker;
/// Marker type for [`TaskCreationConfigBuilder`].
pub struct OverridesObjectPoolMarker;
/// Marker type for [`TaskCreationConfigBuilder`].
pub struct NoPriorityMarker;
/// Marker type for [`TaskCreationConfigBuilder`].
pub struct OverridesPriorityMarker;
/// Marker type for [`TaskCreationConfigBuilder`].
pub struct NoActivationEventsMarker;
/// Marker type for [`TaskCreationConfigBuilder`].
pub struct OverridesActivationEventsMarker;
/// Marker type for [`TaskCreationConfigBuilder`].
pub struct NoTaskCreationFunctionMarker;
/// Marker type for [`TaskCreationConfigBuilder`].
pub struct OverridesTaskCreationFunctionMarker;
/// Marker type for [`TaskCreationConfigBuilder`].
pub struct NoSpecificationFunctionMarker;
/// Marker type for [`TaskCreationConfigBuilder`].
pub struct OverridesSpecificationFunctionMarker;

/// Builder for [`TaskCreationConfig`].
///
/// Allows overriding the task creation configuration defined in a [`PxrosTask`] to customize the creation behavior.
pub struct TaskCreationConfigBuilder<
    Core,
    ObjectPool,
    Priority,
    ActivationEvents,
    TaskCreationFunction,
    SpecificationFunction,
> {
    task_config: TaskNativeCreationConfig,
    overrides: TaskCreationConfigOverrides,
    marker: PhantomData<(Core, ObjectPool, Priority, ActivationEvents, TaskCreationFunction, SpecificationFunction)>,
}

impl
    TaskCreationConfigBuilder<
        NoCoreMarker,
        NoObjectPoolMarker,
        NoPriorityMarker,
        NoActivationEventsMarker,
        NoTaskCreationFunctionMarker,
        NoSpecificationFunctionMarker,
    >
{
    /// Creates a new [`TaskCreationConfigBuilder`] from generic type parameter `PT`.
    pub const fn from_task<PT>() -> Self
    where
        PT: PxrosTask,
    {
        Self {
            task_config: TaskNativeCreationConfig::from_task::<PT>(),
            overrides: TaskCreationConfigOverrides::const_default(),
            marker: PhantomData,
        }
    }
}

impl<ObjectPool, Priority, ActivationEvents, TaskCreationFunction, SpecificationFunction>
    TaskCreationConfigBuilder<
        NoCoreMarker,
        ObjectPool,
        Priority,
        ActivationEvents,
        TaskCreationFunction,
        SpecificationFunction,
    >
{
    /// Overrides the core configuration.
    pub const fn override_core(
        self,
        core: u32,
    ) -> TaskCreationConfigBuilder<
        OverridesCoreMarker,
        ObjectPool,
        Priority,
        ActivationEvents,
        TaskCreationFunction,
        SpecificationFunction,
    > {
        TaskCreationConfigBuilder {
            task_config: self.task_config,
            overrides: TaskCreationConfigOverrides {
                core: Some(core),
                ..self.overrides
            },
            marker: PhantomData,
        }
    }
}

impl<Core, Priority, ActivationEvents, TaskCreationFunction, SpecificationFunction>
    TaskCreationConfigBuilder<
        Core,
        NoObjectPoolMarker,
        Priority,
        ActivationEvents,
        TaskCreationFunction,
        SpecificationFunction,
    >
{
    /// Overrides the object pool configuration.
    pub const fn override_object_pool(
        self,
        object_pool: PxOpool_t,
    ) -> TaskCreationConfigBuilder<
        Core,
        OverridesObjectPoolMarker,
        Priority,
        ActivationEvents,
        TaskCreationFunction,
        SpecificationFunction,
    > {
        TaskCreationConfigBuilder {
            task_config: self.task_config,
            overrides: TaskCreationConfigOverrides {
                object_pool: Some(object_pool),
                ..self.overrides
            },
            marker: PhantomData,
        }
    }
}

impl<Core, ObjectPool, ActivationEvents, TaskCreationFunction, SpecificationFunction>
    TaskCreationConfigBuilder<
        Core,
        ObjectPool,
        NoPriorityMarker,
        ActivationEvents,
        TaskCreationFunction,
        SpecificationFunction,
    >
{
    /// Overrides the priority configuration.
    pub const fn override_priority(
        self,
        priority: u32,
    ) -> TaskCreationConfigBuilder<
        Core,
        ObjectPool,
        OverridesPriorityMarker,
        ActivationEvents,
        TaskCreationFunction,
        SpecificationFunction,
    > {
        TaskCreationConfigBuilder {
            task_config: self.task_config,
            overrides: TaskCreationConfigOverrides {
                priority: Some(PxPrio_t(priority)),
                ..self.overrides
            },
            marker: PhantomData,
        }
    }
}

impl<Core, ObjectPool, Priority, TaskCreationFunction, SpecificationFunction>
    TaskCreationConfigBuilder<
        Core,
        ObjectPool,
        Priority,
        NoActivationEventsMarker,
        TaskCreationFunction,
        SpecificationFunction,
    >
{
    /// Overrides the activation event configuration.
    pub const fn override_activation_events(
        self,
        activation_events: u32,
    ) -> TaskCreationConfigBuilder<
        Core,
        ObjectPool,
        Priority,
        OverridesActivationEventsMarker,
        TaskCreationFunction,
        SpecificationFunction,
    > {
        TaskCreationConfigBuilder {
            task_config: self.task_config,
            overrides: TaskCreationConfigOverrides {
                activation_events: Some(PxEvents_t(activation_events)),
                ..self.overrides
            },
            marker: PhantomData,
        }
    }
}

impl<Core, ObjectPool, Priority, ActivationEvents, SpecificationFunction>
    TaskCreationConfigBuilder<
        Core,
        ObjectPool,
        Priority,
        ActivationEvents,
        NoTaskCreationFunctionMarker,
        SpecificationFunction,
    >
{
    /// Overrides the creation function configuration.
    pub const fn override_creation_function(
        self,
        creation_function: extern "C" fn(PxOpool_t, PxPrio_t, PxEvents_t, extern "C" fn() -> PxTaskSpec_T) -> PxTask_t,
    ) -> TaskCreationConfigBuilder<
        Core,
        ObjectPool,
        Priority,
        ActivationEvents,
        OverridesTaskCreationFunctionMarker,
        SpecificationFunction,
    > {
        TaskCreationConfigBuilder {
            task_config: self.task_config,
            overrides: TaskCreationConfigOverrides {
                task_creation_function: Some(creation_function),
                ..self.overrides
            },
            marker: PhantomData,
        }
    }
}

impl<Core, ObjectPool, Priority, ActivationEvents, TaskCreationFunction>
    TaskCreationConfigBuilder<
        Core,
        ObjectPool,
        Priority,
        ActivationEvents,
        TaskCreationFunction,
        NoSpecificationFunctionMarker,
    >
{
    /// Overrides the specification function configuration.
    pub const fn override_specification_function(
        self,
        specification_function: extern "C" fn() -> PxTaskSpec_T,
    ) -> TaskCreationConfigBuilder<
        Core,
        ObjectPool,
        Priority,
        ActivationEvents,
        TaskCreationFunction,
        OverridesSpecificationFunctionMarker,
    > {
        TaskCreationConfigBuilder {
            task_config: self.task_config,
            overrides: TaskCreationConfigOverrides {
                specification_function: Some(specification_function),
                ..self.overrides
            },
            marker: PhantomData,
        }
    }
}

impl<Core, ObjectPool, Priority, ActivationEvents, TaskCreationFunction, SpecificationFunction>
    TaskCreationConfigBuilder<Core, ObjectPool, Priority, ActivationEvents, TaskCreationFunction, SpecificationFunction>
{
    /// Builds [`TaskCreationConfig`] with configured overrides.
    pub const fn build(self, task_creation_identifier: &'static str) -> TaskCreationConfig {
        TaskCreationConfig {
            task_creation_identifier,
            task_config: self.task_config,
            overrides: self.overrides,
        }
    }
}
