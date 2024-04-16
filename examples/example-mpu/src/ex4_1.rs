//! Exercise 4.1
use core::ffi::CStr;

use pxros::bindings::{PxMbx_t, PxPrio_t, PxProtectType_t};
use pxros::mem::MemoryRegion;
use pxros::PxResult;
use veecle_pxros::pxros::task::{log_id, PxrosTask};

fn flag_4_1() -> &'static str {
    const DATA_ADDRESS: usize = 0xB002FC00;

    let slice = unsafe { core::slice::from_raw_parts(DATA_ADDRESS as *const u8, 16) };

    core::str::from_utf8(slice).unwrap()
}

const EXTRA_REGION: MemoryRegion = MemoryRegion::new(0xB002FC00..0xB002FFFF, PxProtectType_t::WRProtection);

/// The address where the flag is stored is located in a region that is MPU protected;
/// unwanted accesses shall return in a trap.
///
/// The only way to read the flag is by:
/// * Communicating with a task that is able to access the memory area (usually what you want to do :) but not the
///   solution here).
///
/// * Add the target memory regions to the list of allowed ones; this can be achieved via the
///   [`PxrosTask::memory_protection_regions`] function; you may want to look at [MemoryRegion].
///
/// To read the flag one needs to add the region to the list of allowed ones.
pub(crate) struct Task1;
impl PxrosTask for Task1 {
    fn debug_name() -> &'static CStr {
        CStr::from_bytes_with_nul("Task1\0".as_bytes())
            .expect("The debug name should be a valid, zero-terminated C string.")
    }

    fn priority() -> PxPrio_t {
        PxPrio_t(0)
    }

    fn core() -> u32 {
        0
    }

    fn memory_protection_regions() -> &'static [MemoryRegion] {
        static MEMORY_REGIONS: [MemoryRegion; 3] = [
            EXTRA_REGION,
            MemoryRegion::new(0xB0000000..0xB0030000 - 1024, PxProtectType_t::WRProtection),
            MemoryRegion::zeroed(),
        ];

        &MEMORY_REGIONS
    }

    fn task_main(_mailbox: PxMbx_t) -> PxResult<()> {
        let (task_debug_name, current_task_id) = log_id::<Self>();
        defmt::info!(
            "[{}: {}] Hi, I am going to read the flag by accessing the memory. If I get an MPU trap I will simply die",
            task_debug_name,
            current_task_id
        );
        let reference = flag_4_1();

        defmt::info!("[{}: {}] The flag for task 4.1 is: {}", task_debug_name, current_task_id, reference);
        #[allow(clippy::empty_loop)]
        loop {}
    }
}
