use darling::FromMeta;
use proc_macro2::{Literal, TokenStream};
use proc_macro2_diagnostics::Diagnostic;
use quote::quote;
use syn::spanned::Spanned;
use syn::{parse, Ident, ItemFn, LitInt, ReturnType};

#[derive(Debug, Default, FromMeta)]
#[darling(default)]
struct TaskAttributes {
    regions: Option<Ident>,
    privileges: Option<Ident>,
    stack_size: Option<u32>,
    interrupt_stack_size: Option<u32>,
    task_name: Option<Ident>,
    /// Set if the task should be spawned automatically by veecle code
    ///
    /// This works by collecting an array of task definitions within the linker
    /// script in a common section, which is then iterated over by rust code
    auto_spawn: Option<SpawnParameters>,
}

#[derive(Debug, FromMeta)]
struct SpawnParameters {
    /// The core on which the task should be spawned
    core: LitInt,
    /// The priority of the spawned task
    priority: LitInt,
}

pub fn run(function: TokenStream, arguments: TokenStream) -> Result<TokenStream, Diagnostic> {
    let function: ItemFn = parse(function.into())?;

    // Check that the number of arguments of the function is 1.
    if function.sig.inputs.len() != 1 {
        return Err(syn::Error::new(
            function.sig.inputs.span(),
            "Function must have signature '(PxMbx_t) -> PxResult<()>'.",
        )
        .into());
    }

    // Check that the number of output of the function is 1
    if function.sig.output == ReturnType::Default {
        return Err(syn::Error::new(
            function.sig.inputs.span(),
            "Function must have signature '(PxMbx_t) -> PxResult<()>'.",
        )
        .into());
    }

    let attributes = darling::export::NestedMeta::parse_meta_list(arguments.clone())?;
    let static_task_configuration = TaskAttributes::from_list(&attributes).map_err(syn::Error::from)?;

    #[cfg(not(feature = "auto_spawn"))]
    if static_task_configuration.auto_spawn.is_some() {
        return Err(syn::Error::new(arguments.span(), "Cannot use autospawn feature with 'rt' feature disabled").into());
    }

    // The task name
    let task_name = function.sig.ident.clone();
    let pxros_name = Literal::byte_string(task_name.to_string().as_bytes());
    let debug_name = task_name.to_string();

    // Stack variables
    let stack_size = static_task_configuration.stack_size.unwrap_or(0x1000);
    let interrupt_stack_size = static_task_configuration.interrupt_stack_size.unwrap_or(32);
    let privileges = static_task_configuration
        .privileges
        .unwrap_or_else(|| syn::parse2(quote::quote_spanned!(arguments.span() => NoDirectAccess)).unwrap());
    let regions = match static_task_configuration.regions {
        Some(regions) => quote! {
            const CUSTOM_REGIONS: [pxros::mem::MemoryRegion; #regions.len()] = #regions;
        },
        None => quote! {
            const CUSTOM_REGIONS: [pxros::mem::MemoryRegion; 0] = [];
        },
    };

    // Do we need to automatically register the name in the name server?
    let task_server_name = match static_task_configuration.task_name {
        Some(name) => quote!(Some(&#name)),
        None => quote!(None),
    };

    let task_creation_function = quote::quote! {
        #[no_mangle]
        extern "C" fn #task_name(prio: pxros::bindings::PxPrio_t, events: pxros::bindings::PxEvents_t, mem_class: pxros::bindings::PxMc_t, obj_pool: pxros::bindings::PxOpool_t) -> pxros::bindings::PxTask_t {
            #function

            // Namespace for macro imports.
            {
                use pxros::bindings::*;
                use pxros::mem::*;
                use veecle_pxros::pxros::task;

                extern "C" fn __task_entry(task: PxTask_t, mailbox: PxMbx_t, _: PxEvents_t) {
                    task::run_task(#task_name, task, mailbox, #task_server_name);
                }

                // Define the main and interrupt stack.
                let ts_taskstack = StackSpec::new(#stack_size, mem_class);
                let ts_inttaskstack = StackSpec::new(#interrupt_stack_size, mem_class);

                static TASK_CONTEXT: PxTaskContext_T = PxTaskContext_T { protection: [
                    MemoryRegion::new(0..0, PxProtectType_t::NoAccessProtection),
                    MemoryRegion::new(u32::MAX..u32::MAX, PxProtectType_t::NoAccessProtection),
                ]};

                #regions

                // TASK_REGIONS needs to be const or static to get linked into the RODATA section by
                // default. This is required to satisfy PXROS requirements on the pointer to the task
                // regions array to be constant.
                const TASK_REGIONS: [MemoryRegion; CUSTOM_REGIONS.len() + 2] = {
                    // Initializes task regions with zero initialized protection regions.
                    let mut expanded: [MemoryRegion; CUSTOM_REGIONS.len() + 2] = [MemoryRegion::zeroed(); CUSTOM_REGIONS.len() + 2];

                    // Set CUSTOM_REGIONS.len() regions.
                    let mut index = 0;
                    while index < CUSTOM_REGIONS.len() {
                        expanded[index] = CUSTOM_REGIONS[index];
                        index += 1;
                    }

                    // Add RODATA & DATA to memory regions as the second to last element.
                    // Defmt uses RODATA to store logging information.
                    // Upper and lower bound are taken from the linker script for the dlmu_cpu0 to
                    // dlmu_cpu3.
                    //
                    // The last kilobyte is skipped to allow example 4 ex 1 to work.
                    expanded[index] = MemoryRegion::new(0xB0000000..0xB0030000 - 1024, PxProtectType_t::WRProtection);

                    // The last element is left zero initialized. PXROS expects an element with
                    // lower and upper bound zero as the terminator of the array.
                    expanded
                };

                // Prepare the stack specs
                let task_spec = PxTaskSpec_T {
                    ts_name: #pxros_name as *const u8 as *const PxChar_t,
                    ts_fun: PxTaskfun_t(Some(__task_entry)),
                    ts_actevents: PxEvents_t(0),
                    // Memory
                    ts_mc: mem_class,
                    ts_opool: obj_pool,
                    // Stack
                    ts_taskstack,
                    ts_inttaskstack,
                    ts_abortstacksize: 0,
                    // Protection
                    ts_context: PxTaskContext_ct(&TASK_CONTEXT as *const _),
                    ts_protect_region: PxProtectRegion_ct(&TASK_REGIONS[0] as *const _),
                    ts_privileges: PxArg_t(Privileges::#privileges as i32),
                    ts_accessrights: PXACCESS_HANDLERS | PXACCESS_INSTALL_HANDLERS,
                    // Priority/scheduling
                    ts_prio: prio,
                    ts_timeslices: PxTicks_t(0),
                    ts_sched_extension: PxTaskSchedExt_ct(core::ptr::null()),
                    ts_sched_initarg: PxArg_t(0),
                };

                // Create the task
                // Safety: not really checked... big TODO
                unsafe { pxros::bindings::PxTaskCreate(obj_pool, pxros::bindings::PxTaskSpec_ct(&task_spec as *const _), prio, events) }
            }
        }
    };

    let output = if let Some(spawn_configuration) = static_task_configuration.auto_spawn {
        let task_priority = spawn_configuration.priority;
        let core = spawn_configuration.core;
        quote! {
            const _: () = {
                use veecle_pxros::pxros::auto_deploy::TaskDefinition;

                /// Defines a [TaskDefinition] for the linker to include into the
                /// `.rust_auto_tasks` section. See veecle-pxros->pxros->auto_deploy.rs for more details.
                // Work-around for compilation on macOS where section names are more restricted.
                #[cfg_attr(target_os = "macos", link_section = ".uninit,rust_auto_tasks")]
                #[cfg_attr(not(target_os = "macos"), link_section = ".rust_auto_tasks")]
                // https://rust-lang.github.io/rfcs/2386-used.html
                // The used attribute signals the compiler to keep the static even though it is not
                // referenced in the program. In release mode, this would otherwise be optimized away
                // and thus break the linker trick employed here.
                #[used]
                static TASK_DEFINITION: TaskDefinition = TaskDefinition {
                    entry: #task_name,
                    priority: pxros::bindings::PxPrio_t(#task_priority),
                    core: #core,
                    debug_name: #debug_name,
                };

                #task_creation_function
            };
        }
    } else {
        task_creation_function
    };

    Ok(output)
}
