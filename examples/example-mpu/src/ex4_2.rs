//! Exercise 4.2
use core::time::Duration;

use pxros::bindings::PxMbx_t;
use pxros::PxResult;
use veecle_pxros::pxros::ticker::Ticker;
use veecle_pxros::pxros::time::time_since_boot;

bitflags::bitflags! {
    /// Use this to start a one-shot delay event via [Ticker].
    #[derive(Copy, Clone)]
    pub struct TaskAEvents: u32 {
        const Delay = 0b000_0001;
    }
}

/// This task *must* execute at highest priority in core 2, but directly yeld execution
/// for `1s` to allow the flag task to boot.
///
/// Following, it will need to measure, busy looping, when `1100ms` have passed, and
/// the moment it is so delay again for `1s`.
///
/// If all is done correctly, the correct flag will be printed.
///
/// You may want to look at [time_since_boot] for precise time measure
///
/// ### The goal
/// The goal here is to show how preemption and yielding works.
#[veecle_pxros::pxros_task(auto_spawn(core = 2, priority = 0))]
fn user_code_4_2(_: PxMbx_t) -> PxResult<()> {
    Ticker::after(TaskAEvents::Delay, Duration::from_millis(1000)).unwrap();
    let start = time_since_boot();

    loop {
        let now = time_since_boot();
        if now - start > Duration::from_millis(1100) {
            Ticker::after(TaskAEvents::Delay, Duration::from_millis(1000)).unwrap();
        }
    }
}
