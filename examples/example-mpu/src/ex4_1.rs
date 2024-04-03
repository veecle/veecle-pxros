//! Exercise 4.1
use pxros::bindings::{PxMbx_t, PxProtectType_t};
use pxros::mem::MemoryRegion;
use pxros::PxResult;

fn flag_4_1() -> &'static str {
    const DATA_ADDRESS: usize = 0xB002FC00;

    let slice = unsafe { core::slice::from_raw_parts(DATA_ADDRESS as *const u8, 16) };

    core::str::from_utf8(slice).unwrap()
}

const EXTRA_REGION: [MemoryRegion; 1] = [MemoryRegion::new(0xB002FC00..0xB002FFFF, PxProtectType_t::WRProtection); 1];

/// The address where the flag is stored is located in a region that is MPU protected;
/// unwanted accesses shall return in a trap.
///
/// The only way to read the flag is by:
/// * Communicating with a task that is able to access the memory area (usually what you want to do :) but not the
///   solution here).
///
/// * Add the target memory regions to the list of allowed ones; this can be achieved via the [veecle_pxros::pxros_task]
///   `regions` attribute; you may want to look at [pxros::task::MemoryRegion].
///
/// To read the flag one needs to add the region to the list of allowed one.
#[veecle_pxros::pxros_task(auto_spawn(core = 0, priority = 0), regions = EXTRA_REGION)]
#[allow(clippy::empty_loop)]
fn user_code_4_1(_: PxMbx_t) -> PxResult<()> {
    defmt::info!("Hi, I am going to read the flag by accessing the memory. If I get an MPU trap I will simply die");
    let reference = flag_4_1();

    defmt::info!("The flag for task 4.1 is {}", reference);
    loop {}
}
