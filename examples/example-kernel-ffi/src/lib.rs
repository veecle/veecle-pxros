//! # Exercise 1: Getting started
//! This exercise is meant to test everyone setup and explain the basis.
//!
//! ### Exercise 1.1
//! The user has to invoke a function over FFI. This function returns a string representing
//! the secret flag that needs to be printed over defmt.
//!
//! There is no particular challenge with this exercise; its main purpose is to verify the setup
//! and the basis.
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
use pxros::bindings::PxMbx_t;
use pxros::PxResult;

mod backend;
mod ex1_1;
mod ex1_2;
mod ex1_3;

/// Example of task that is automatically spawned in PXROS in core 0 with
/// priority "12".
///
/// Our procedural macro takes care of creating & allocating all required
/// data structure and ensuring lifetimes.
///
/// Configuration of the task can be done through proc-macro attributes.
#[veecle_pxros::pxros_task(auto_spawn(core = 0, priority = 12))]
fn rust_task(_: PxMbx_t) -> PxResult<()> {
    // Solution for (1.1)
    ex1_1::ex1_1_solution();

    // Solution for (1.2)
    ex1_2::ex1_2_solution();

    // Solution for (1.3)
    ex1_3::ex1_3_solution();

    // Panic to give access to others
    defmt::panic!("The exercise is terminated")
}
