//! Exercise 1.1
use veecle_pxros::pxros::task::log_id;

use crate::Ex1Task;

extern "Rust" {
    /// This is an example of a Foreign Function Interface (FFI). It is unsafe
    /// to call directly as the compiler cannot guarantee the design contract
    /// of the function is respected.
    fn secret_flag() -> &'static str;
}

impl Ex1Task {
    /// The function should call (via `unsafe`) the FFI and print
    /// the returned flag via the [defmt] framework.
    pub fn ex1_1_solution() {
        let (task_debug_name, task_id) = log_id::<Self>();

        let flag = unsafe { secret_flag() };

        defmt::info!("[{}: {}] The (1.1) flag is: {}", task_debug_name, task_id, flag);
    }
}
