//! Solution to show for exercise 3.2
use core::time::Duration;

use futures::{future, StreamExt};
use pxros::bindings::*;
use pxros::PxResult;
use veecle_pxros::pxros::ticker::AsyncTicker;
use veecle_pxros::pxros_run;

use crate::{AsyncExecutorTask, TickerEvents};

/// Invoke periodically with the correct timing to yield a specific sequence of numbers.
fn read_sensor_1() -> u32 {
    extern "Rust" {
        fn ffi_read_sensor_1() -> u32;
    }
    unsafe { ffi_read_sensor_1() }
}

/// Invoke periodically with the correct timing to yield a specific sequence of numbers.
fn read_sensor_2() -> Option<u32> {
    extern "Rust" {
        fn ffi_read_sensor_2() -> Option<u32>;
    }
    unsafe { ffi_read_sensor_2() }
}

/// Invoking this function with the correct number will print the result.
fn submit_result(sensor_1: u32, sensor_2: u32) {
    extern "Rust" {
        fn ffi_submit_result(sensor_1: u32, sensor_2: u32);
    }
    unsafe { ffi_submit_result(sensor_1, sensor_2) }
}

/// This function shall exploit the fact that tickers are [futures::Stream]
/// to process **ticks** functionally. Please, take a look at [futures::StreamExt].
///
/// This function must
/// * `.map()` each tick to a sensor reading.
/// * `.take` only the correct amount of ticker (8).
/// * ``fold` the result into an accumulator that sums all the readings.
///
/// The returned value shall be returned to the executor.
async fn sensor_1(ticker: AsyncTicker<TickerEvents>) -> PxResult<u32> {
    let number = ticker
        .map(|_| read_sensor_1()) // usize
        .take(8)
        .fold(0, |acc, x| future::ready(acc + x))
        .await;

    Ok(number)
}

/// This function shall exploit the fact that tickers are [futures::Stream]
/// to process **ticks** functionally. Please, take a look at [futures::StreamExt].
///
/// This function must
/// * `.map()` each tick to a sensor reading.
/// * `.take_while` the value are `Some`.
/// * `.map()` unwrap each Some(_) since they are known to be Some().
/// * `.enumerate()` the readings via index.
/// * `.fold` the result into an accumulator that:
///     - the initial accumulator value shall be 1.
///     - each iteration shall be `acc = MAX(acc, iteration_index).
///
/// The returned value shall be returned to the executor.
async fn sensor_2(ticker: AsyncTicker<TickerEvents>) -> PxResult<u32> {
    let number = ticker
        .map(|_| read_sensor_2()) // Option<usize> -> Exit the first time it's none
        .take_while(|x| future::ready(x.is_some()))
        .map(|sensor| sensor.unwrap())
        .enumerate()
        .fold(1, |acc, (i, x)| {
            future::ready(u32::max(acc, i as u32 * x))
        }).await;

    // Await the result
    Ok(number)
}

impl AsyncExecutorTask {
    /// This function shall use the [pxros_run] macro to spawn two async tasks via
    /// [sensor_1] and [sensor_2] functions.
    ///
    /// Each async task shall return a `u32` value to the executor, which can be then collected as
    /// an array via `let = pxros_run!(...)`.
    ///
    /// Each returned `u32` shall then be submit via [submit_result]: if correct the flag shall
    /// be printed.
    pub fn ex3_2_solution(mailbox: PxMbx_t) {
        let ticker_1 = AsyncTicker::every(TickerEvents::Ticker1, Duration::from_millis(150)).unwrap();
        let ticker_2 = AsyncTicker::every(TickerEvents::Ticker2, Duration::from_millis(220)).unwrap();

        let result = pxros_run!(mailbox, TickerEvents, PxResult<u32>, sensor_1(ticker_1), sensor_2(ticker_2));

        let sensor_1 = result[0].unwrap().unwrap();
        let sensor_2 = result[1].unwrap().unwrap();
        submit_result(sensor_1, sensor_2);
    }
}
