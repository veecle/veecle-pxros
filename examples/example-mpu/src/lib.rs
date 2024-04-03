//! ## Exercise 4: Advanced Concepts
//! In this exercise we explore the capabilities of the Aurix MPU and how it is used in PXROS.
//! Furthermore task priorities and core assignment are explored.
//!
//! ### Exercise 4.1
//! The participant is expected to invoke a function that returns a reference to the secret flag. The reference
//! points to a protected memory region which by default tasks do not have access to.
//!
//! The participant will have to add access to the memory region to complete the task.
//!
//! ### Exercise 4.2
//! To complete this exercise the participant must have a good understanding of priorities and scheduler. The goal
//! is to preempt a hidden task in the correct order. The hidden task must be started quickly after the system boots-up,
//! and must then be preempted (somehow) for exactly for one second.
//!
//! After that, it must be allowed to work again. If all is correct it will print the flag.
#![no_std]

mod backend;
mod ex4_1;
mod ex4_2;
