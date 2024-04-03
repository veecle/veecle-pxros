//! Rusty PXROS abstractions.
//!
//! This module implements utilities on top of PXROS.

#[cfg(feature = "rt")]
pub mod auto_deploy;
#[cfg(feature = "rt")]
mod defmt_rtt;
pub mod events;
pub mod executor;
pub mod messages;
pub mod name_server;
#[cfg(feature = "rt")]
pub mod panic;
pub mod task;
pub mod ticker;
pub mod time;
pub mod tsim;
