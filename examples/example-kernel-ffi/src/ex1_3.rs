//! Exercise 1.3
use pxros::bindings::{PxGetId, PxTask_t};
use veecle_pxros::pxros::task::PxrosTask;

use crate::Ex1Task;

extern "Rust" {
    /// This function shall validate the provided task ID; if valid
    /// it will return the flag, else it will [panic].
    fn secret_flag_from_task_id(task_id: PxTask_t) -> &'static str;
}

impl Ex1Task {
    /// The FFI should be called with the runtime task ID as parameter.
    ///
    /// The ID can be obtained through a system-call; the Rust bindings to
    /// the "C" kernel can be found in [pxros::bindings].
    ///
    /// In real scenarios, direct usage of bindings is not advised.
    /// FFIs won't guarantee lifetime or memory ownership.
    pub fn ex1_3_solution() {
        let (task_debug_name, log_task_id) = <Self as PxrosTask>::log_id();

        // Perform a system call
        let task_id = PxGetId();

        // Deliver it to the flag
        let flag = unsafe { secret_flag_from_task_id(task_id) };

        defmt::info!("[{}: {}] The (1.3) secret flag is: {}", task_debug_name, log_task_id, flag);
    }
}
