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

use veecle_pxros::pxros::task::TaskCreationConfig;

use crate::backend::HiddenTask;
use crate::ex4_1::Task1;
use crate::ex4_2::Task2;

mod backend;
mod ex4_1;
mod ex4_2;

#[no_mangle]
static TASK_LIST: &[TaskCreationConfig] = &[
    TaskCreationConfig::new_from_task::<HiddenTask>("Ex4_Hidden_Creation"),
    TaskCreationConfig::new_from_task::<Task1>("Task1_Creation"),
    TaskCreationConfig::new_from_task::<Task2>("Task2_Creation"),
];
