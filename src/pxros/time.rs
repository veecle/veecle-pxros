//! Time utilities.
use core::time::Duration;

use pxros::bindings::{PxTickGetTicksFromMilliSeconds, PxTickGetTimeInMilliSeconds, PxTicks_t};

/// Returns the [`Duration`] since boot.
pub fn time_since_boot() -> Duration {
    // Safety:
    // Documentation states no conditions for function call and returned type is primitive type (u32).
    let px_milliseconds = unsafe { PxTickGetTimeInMilliSeconds() };

    Duration::from_millis(u64::from(px_milliseconds))
}

/// Converts a [`Duration`] to [`PxTicks_t`].
pub fn duration_to_ticks(duration: Duration) -> PxTicks_t {
    // Safety:
    // Documentation states "millis must be a plausible value". We assume this covers every u32.
    // Returned type is wrapped primitive, no safety concerns there.
    unsafe { PxTickGetTicksFromMilliSeconds(duration.as_millis() as u32) }
}
