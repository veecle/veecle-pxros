//! # Exercise 3: Asynchronous Rust
//! This exercise introduces async Rust. It is meant to show how using asynchronous code can
//! heavily simplify implementation of concurrent operations (especially when state tracking,
//! mutability or ownership is required).
//!
//! ### Exercise 3.1
//! The participant is expected to invoke two callbacks periodically. The template guides the
//! implementation towards using an async executor that handles 2 tasks. If the callbacks are
//! invoked correctly for a number of times the flag will be returned.
//!
//! This exercise is meant to introduce generic async and to show the difference from blocking code.
//!
//! ### Exercise 3.2
//! The participant has to create 2 tasks. Both use a ticker to periodically obtain a new number
//! from a "virtual" sensor. The first task must sum all readings, while the second must return always
//! the maximum (with some minor math in-between).
//!
//! When done, each async function shall returns the result to the executor which, when done, return
//! the results to the user. The user shall then submit them through a `FFI`: if correct the flag will
//! be printed
//!
//! The goal is to show `iterators` and `stream``: these are both fundamental concepts in Rust & async.
//! If done properly each of these functions should be 'one-liners'
//!
//! ### Exercise 3.3
//! Like exercise 3.2 but instead of tickers the user has to listen to events and messages. In this case
//! the user will have to `select` between two concurrent async function to decide what to process
//!
//! The goal is to teach async messages, events and selecting between rust's futures.
#![no_std]
#![feature(type_alias_impl_trait)]
#![feature(macro_metavar_expr)]
#![feature(exclusive_range_pattern)]
use pxros::bindings::PxMbx_t;
use pxros::PxResult;
use veecle_pxros::pxros::name_server::TaskName;

/// This is pxros-specific way on how to identify tasks at runtime via a name.
///
/// In this case the name "2" can be used to query the runtime ID of
/// the task via [veecle_pxros::pxros::name_server].
const FLAG_TASK_NAME: TaskName = TaskName::new(2);

/// User task name
const USER_TASK_NAME: TaskName = TaskName::new(44);

mod backend;
mod ex3_1;
mod ex3_2;
mod ex3_3;

bitflags::bitflags! {
    /// Internal tickers events.
    #[derive(Copy, Clone)]
    pub struct TickerEvents: u32 {
        /// Generic ticker 1.
        const Ticker1 = 0b0001_0000;
        /// Generic ticker 2.
        const Ticker2 = 0b0010_0000;
    }

    /// Events used to communicate with the flag task for (3.3).
    #[derive(Copy, Clone)]
    pub struct FlagEvents: u32 {
        /// The start transmission flag shall be sent to the flag
        /// task to initialize transmission of flag nibbles via
        /// messages.
        const StartTransmission = 0b0000_0100;

        /// This event shall be emitted by the flag-task to inform
        /// the task in exercise (3.3) that the flag transmission
        /// is complete.
        const Terminate = 0b0000_1000;
    }
}

/// User executor code.
#[veecle_pxros::pxros_task(task_name = USER_TASK_NAME, auto_spawn(core = 0, priority = 15))]
fn async_executor(mailbox: PxMbx_t) -> PxResult<()> {
    // Solution for (3.1)
    ex3_1::ex3_1_solution(mailbox);

    // Solution for (3.2)
    ex3_2::ex3_2_solution(mailbox);

    // Solution for (3.3)
    ex3_3::ex3_3_solution(mailbox);

    // Panic to give access to others
    defmt::panic!("The exercise is terminated")
}
