//! Fatal error routines.
//!
//! This module defines behavior of the system when encountering a fatal error
//! that cannot be recovered from.
use core::panic::PanicInfo;

/// Helper to abort the system (required by runtime)
#[no_mangle]
fn abort() -> ! {
    #[cfg(any(target_arch = "tc162", target_arch = "tc18", target_arch = "tc18a"))]
    unsafe {
        // The debug statement makes the debugger/tricore-probe halt
        core::arch::asm!("debug");
        loop {
            // Yields execution of the core, since there is no work to do for
            // this task
            core::arch::asm!("wait");
        }
    }
    // Pxros is only supported by the tricore architecture
    unreachable!("This function is only called within Pxros");
}

/// Panic-handler: print the panic & invoke [pxros::bindings::PxPanic]
#[cfg_attr(not(test), panic_handler)]
pub fn panic(panic: &PanicInfo<'_>) -> ! {
    defmt::error!("{}", defmt::Display2Format(panic));

    unsafe {
        pxros::bindings::PxPanic();
    }

    unreachable!("PxPanic never returns")
}
