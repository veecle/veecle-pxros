//! # Exercise 1: Getting started
//! This exercise is meant to test everyone's setup and explain the basics.
//!
//! ### Exercise 1.1
//! The user has to invoke a function over FFI. This function returns a string representing
//! the secret flag that needs to be printed over defmt.
//!
//! There is no particular challenge with this exercise; its main purpose is to verify the setup
//! and the basics.
//!
//! ### Exercise 1.2
//! The user needs to call additional extern functions. Each function returns part of a flag that
//! must be printed over defmt using different formatters.
//!
//! This introduces some utility in defmt useful for later exercises.
//!
//! ### Exercise 1.3
//! The user needs to invoke an extern function passing the task id (which can be obtained by a system
//! call).
//!
//! When the function is called with the correct parameter, it prints the flag.
#![no_std]

use core::ffi::CStr;

use pxros::bindings::PxMbx_t;
use pxros::PxResult;
use veecle_pxros::pxros::task::{log_id, PxrosTask, TaskCreationConfig};

mod backend;
mod ex1_1;
mod ex1_2;
mod ex1_3;

/// Definition and configuration of auto-created tasks.
#[no_mangle]
static TASK_LIST: &[TaskCreationConfig] = &[TaskCreationConfig::from_task::<Ex1Task>("Ex1Task_Creation")];

/// A PXROS task.
/// Its functionality and configuration is defined in the [`PxrosTask`] trait.
pub(crate) struct Ex1Task;
impl PxrosTask for Ex1Task {
    fn debug_name() -> &'static CStr {
        CStr::from_bytes_with_nul("Ex1_Task\0".as_bytes())
            .expect("The debug name should be a valid, zero-terminated C string.")
    }

    fn task_main(_mailbox: PxMbx_t) -> PxResult<()> {
        let (task_debug_name, task_id) = log_id::<Self>();

        // Solution for (1.1)
        Self::ex1_1_solution();

        // Solution for (1.2)
        Self::ex1_2_solution();

        // Solution for (1.3)
        Self::ex1_3_solution();

        // Panic to give access to others
        defmt::panic!("[{}: {}] The exercise is terminated", task_debug_name, task_id,)
    }
}
