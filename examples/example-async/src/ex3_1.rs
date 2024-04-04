//! Exercise 3.1
use core::time::Duration;

use futures::StreamExt;
use pxros::bindings::*;
use pxros::PxResult;
use veecle_pxros::pxros::task::PxrosTask;
use veecle_pxros::pxros::ticker::AsyncTicker;
use veecle_pxros::pxros_run;

use crate::{AsyncExecutorTask, TickerEvents};

/// Invoke periodically with the correct timing: at a certain point this
/// will result in a flag being released.
fn call_ticker_1() -> Option<&'static str> {
    extern "Rust" {
        fn ffi_ex1_ticker_1() -> Option<&'static str>;
    }
    unsafe { ffi_ex1_ticker_1() }
}

/// Invoke periodically with the correct timing: at a certain point this
/// will result in a flag being released.
fn call_ticker_2() -> Option<&'static str> {
    extern "Rust" {
        fn ffi_ex1_ticker_2() -> Option<&'static str>;
    }
    unsafe { ffi_ex1_ticker_2() }
}

impl AsyncExecutorTask {
    /// This is an example of a generic async function: in contrast to blocking code, async
    /// allows to "generalize" a single complex implementation into a multitude of *isolated*
    /// async function each `.awaiting` the blocking code.
    ///
    /// The goal is to fill-up the code here so that the exercise 3.1 can be solved in a generic
    /// manner.
    pub async fn ex1_ticker<F>(mut ticker: AsyncTicker<TickerEvents>, callback: F) -> PxResult<()>
    where
        F: Fn() -> Option<&'static str>,
    {
        let (task_debug_name, log_task_id) = <Self as PxrosTask>::log_id();
        while (ticker.next().await).is_some() {
            if let Some(flag) = callback() {
                defmt::info!("[{}: {}] The (3.1) flag is: {}", task_debug_name, log_task_id, flag);
                return Ok(());
            }
        }

        defmt::panic!("[{}: {}] Ticker terminated.", task_debug_name, log_task_id);
    }

    /// This function shall use the [pxros_run] macro to spawn two async tasks via
    /// generic [ex1_ticker] function.
    ///
    /// Each async task shall call either [call_ticker_1] & [call_ticker_2] and print the flag
    /// when `Some(_)`.
    ///
    /// When *done*, each async function shall return Ok(()).
    ///
    /// Ticker timings shall be `150ms` for `Ticker1` and `220ms` for `Ticker2`.
    pub fn ex3_1_solution(mailbox: PxMbx_t) {
        let ticker_1 = AsyncTicker::every(TickerEvents::Ticker1, Duration::from_millis(150)).unwrap();
        let ticker_2 = AsyncTicker::every(TickerEvents::Ticker2, Duration::from_millis(220)).unwrap();

        pxros_run!(
            mailbox,
            TickerEvents,
            PxResult<()>,
            Self::ex1_ticker(ticker_1, call_ticker_1),
            Self::ex1_ticker(ticker_2, call_ticker_2)
        );
    }
}
