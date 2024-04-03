#![allow(unused)]

use core::arch::asm;
use core::time::Duration;

use pxros::bindings::*;
use pxros::PxResult;
use veecle_pxros::pxros::time::time_since_boot;

// Work-around for compilation on macOS where section names are more restricted.
#[cfg_attr(target_os = "macos", link_section = ".uninit,mpu_protected")]
#[cfg_attr(not(target_os = "macos"), link_section = ".mpu_protected")]
static FLAG_4_1: [u8; 16] = *b"OsU7rwolFq3ItiJ5"; // Keep this at 16 characters always, this is hardcoded

/// We can make this a bit more sophisticated
#[veecle_pxros::pxros_task(auto_spawn(core = 2, priority = 10))]
fn hidden_code_4_2(_: PxMbx_t) -> PxResult<()> {
    let mut start = time_since_boot();

    if start > Duration::from_millis(100) {
        panic!("You need to get me going asap!");
    }

    loop {
        let now = time_since_boot();
        let exempted_time = now - start;

        if exempted_time < Duration::from_millis(10) {
            start = now;
            continue; // Do not spam the log output
        } else if exempted_time < Duration::from_millis(1000) {
            start = now;
            defmt::info!("You called the task too early");
            continue;
        } else if exempted_time > Duration::from_millis(1500) {
            start = now;
            defmt::info!("You called the task too late");
            continue;
        }
        panic!("Got it! The secret flag is \"vxPyBljl5A\"")
    }
}
