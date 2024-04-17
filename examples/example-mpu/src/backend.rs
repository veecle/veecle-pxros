#![allow(unused)]

use core::arch::asm;
use core::ffi::CStr;
use core::time::Duration;

use pxros::bindings::*;
use pxros::PxResult;
use veecle_pxros::pxros::task::{log_id, PxrosTask};
use veecle_pxros::pxros::time::time_since_boot;

// Currently not present in the linker script anymore.
// Work-around for compilation on macOS where section names are more restricted.
#[cfg_attr(target_os = "macos", link_section = ".uninit,mpu_protected")]
#[cfg_attr(not(target_os = "macos"), link_section = ".mpu_protected")]
static FLAG_4_1: [u8; 16] = *b"OsU7rwolFq3ItiJ5"; // Keep this at 16 characters always, this is hardcoded

pub(crate) struct HiddenTask;
impl PxrosTask for HiddenTask {
    fn debug_name() -> &'static CStr {
        CStr::from_bytes_with_nul("HiddenTask\0".as_bytes())
            .expect("The debug name should be a valid, zero-terminated C string.")
    }

    fn priority() -> PxPrio_t {
        PxPrio_t(10)
    }

    fn core() -> u32 {
        2
    }

    /// We can make this a bit more sophisticated
    fn task_main(_mailbox: PxMbx_t) -> PxResult<()> {
        let (task_debug_name, current_task_id) = log_id::<Self>();
        let mut start = time_since_boot();

        if start > Duration::from_millis(100) {
            panic!("[{}: {}] You need to get me going asap!", task_debug_name, current_task_id);
        }

        loop {
            let now = time_since_boot();
            let exempted_time = now - start;

            if exempted_time < Duration::from_millis(10) {
                start = now;
                continue; // Do not spam the log output
            } else if exempted_time < Duration::from_millis(1000) {
                start = now;
                defmt::info!("[{}: {}] You called the task too early.", task_debug_name, current_task_id);
                continue;
            } else if exempted_time > Duration::from_millis(1500) {
                start = now;
                defmt::info!("[{}: {}] You called the task too late.", task_debug_name, current_task_id);
                continue;
            }
            panic!("[{}: {}] Got it! The secret flag is: \"vxPyBljl5A\"", task_debug_name, current_task_id)
        }
    }
}
