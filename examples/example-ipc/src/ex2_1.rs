//! Exercise 2.1
use core::time::Duration;

use pxros::bindings::PxMbx_t;
use pxros::PxResult;
use veecle_pxros::pxros::ticker::Ticker;

use crate::MyEvents;

/// This safe `FFI` returns Some(flag) if called multiple
/// times at correct intervals.
///
/// Calls needs to be precise!
fn call_ticker() -> Option<&'static str> {
    extern "Rust" {
        fn ffi_ticker() -> Option<&'static str>;
    }

    unsafe { ffi_ticker() }
}

/// This function must be converted to a pxros task which starts a Ticker
/// which calls the `ffi` at each tick, util the flag is returned.
///
/// NOTE: The tick time must be `150ms`.
///
/// For this exercise, you may want to look at:
/// * [veecle_pxros::pxros_task]: procedural macro to wrap a correct function into a task (example in example-1).
/// * [Ticker]: utility to work with tickers.
/// * [core::time]: Rust's basic time structures and APIs.
#[veecle_pxros::pxros_task(auto_spawn(core = 1, priority = 18))]
fn loop_ticker_task(_: PxMbx_t) -> PxResult<()> {
    defmt::debug!("[Loop ticker task] started");

    // Create a ticker using my task events
    let mut ticker = Ticker::every(MyEvents::TickerEx1, Duration::from_millis(150))?;

    // Naive loop that trigger the FFI at each *tick*
    loop {
        ticker.wait();

        if let Some(flag) = call_ticker() {
            defmt::info!("The (2.1) flag is {}", flag);
            break;
        }
    }

    // Wait for one second to allow all tasks (2.2, 2.3, 2.4) to finish
    // before triggering a panic.
    drop(ticker);
    Ticker::after(MyEvents::TickerEx1, Duration::from_millis(1_000))?;
    defmt::panic!("The exercise is terminated")
}
