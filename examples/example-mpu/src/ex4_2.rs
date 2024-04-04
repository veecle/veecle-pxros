//! Exercise 4.2
use core::ffi::CStr;
use core::time::Duration;

use pxros::bindings::{PxMbx_t, PxPrio_t};
use pxros::PxResult;
use veecle_pxros::pxros::task::PxrosTask;
use veecle_pxros::pxros::ticker::Ticker;
use veecle_pxros::pxros::time::time_since_boot;

bitflags::bitflags! {
    /// Use this to start a one-shot delay event via [Ticker].
    #[derive(Copy, Clone)]
    pub struct TaskAEvents: u32 {
        const Delay = 0b000_0001;
    }
}

/// This task *must* execute at highest priority in core 2, but directly yield execution
/// for `1s` to allow the flag task to boot.
///
/// Following, it will need to measure, busy looping, when `1100ms` have passed, and
/// the moment it is so delay again for `1s`.
///
/// If all is done correctly, the correct flag will be printed.
///
/// You may want to look at [time_since_boot] for precise time measure.
///
/// ### The goal
/// The goal here is to show how preemption and yielding works.
pub(crate) struct Task2;
impl PxrosTask for Task2 {
    fn debug_name() -> &'static CStr {
        CStr::from_bytes_with_nul("Task2\0".as_bytes())
            .expect("The debug name should be a valid, zero-terminated C string.")
    }

    fn priority() -> PxPrio_t {
        PxPrio_t(0)
    }

    fn core() -> u32 {
        2
    }

    fn task_main(_mailbox: PxMbx_t) -> PxResult<()> {
        Ticker::after(TaskAEvents::Delay, Duration::from_millis(1000)).unwrap();
        let start = time_since_boot();

        loop {
            let now = time_since_boot();
            if now - start > Duration::from_millis(1100) {
                Ticker::after(TaskAEvents::Delay, Duration::from_millis(1000)).unwrap();
            }
        }
    }
}
