//! # Veecle-pxros.
//!
//! A runtime for writing automotive and industrial Rust on top of HighTec's safety
//! kernel for the Infineon AURIX™ Platform.
//!
//! For more information visit: <https://github.com/veecle/veecle-pxros>.
//!
//! ## Note
//!
//! This library is in its early stages and may not work as intended. Feedback,
//! suggestions, or contributions are welcome!
//!
//! ## Basic usage
//!
//! This runtime allows to seamlessly deploy PXROS Rust tasks in parallel to C tasks
//! directly from Rust. Kernel APIs can be found in the [pxros] module.
//!
//! 1) Create a Rust crate with this minimal `Cargo.toml`.
//!
//! ```toml
//! [package]
//! name = "hello-world"
//!
//! [lib]
//! # Static linking is required by PXROS CMake.
//! crate-type = ["staticlib"]
//!
//! [dependencies]
//! # To enable logging via defmt.
//! defmt.workspace = true
//! # Kernel bindings.
//! pxros.workspace = true
//! # This repository - RT enabled to import Defmt and Panic runtime.
//! veecle-pxros = { workspace = true, features = ["rt"] }
//! ```
//!
//! 2) In the lib.rs define a PXROS task that will be auto-spawned on
//! core 0 with priority 10.
//! ```ignore,
//! #[veecle_pxros::pxros_task(auto_spawn(core = 0, priority = 10))]
//! fn hello_world(_: PxMbx_t) -> PxResult<()> {
//!     defmt::info!("Hello World");
//!     Ok(())
//! }
//! ```
//!
//! 3) Compile and run via `cargo xtask run --binary hello-world --target tc162`.
//!
//! Additional examples can be found in the examples folder.
#![cfg_attr(not(test), no_std)]
#![feature(waker_getters)]
#![feature(type_alias_impl_trait)]
#![feature(macro_metavar_expr)]
#![deny(unsafe_op_in_unsafe_fn)]
#![deny(clippy::missing_safety_doc)]

pub mod executor;
pub mod pxros;

pub use static_cell::StaticCell;
pub use task_macro::pxros_task;
