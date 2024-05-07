//! UDP example
//!
//! Every UDP datagram received is returned to the sender.
//!
//! To achieve this, three tasks cooperate:
//! * Driver task: The driver task uses the iLLD driver to provide a network driver
//! * Smoltcp task: The smoltcp task makes use of the [smoltcp] crate to provide a TCP/IP stack.
//! * Mirror task: The mirror task returns every UDP datagram received to its sender.
//!
//! The communication is done via message passing over the PXROS provided message passing APIs.
//!
//! ```Driver task <--> smoltcp task <--> ping task```
#![no_std]
use veecle_pxros::pxros::name_server::TaskName;
use veecle_pxros::pxros::task::TaskCreationConfig;

use crate::network::NetworkStackTask;
use crate::udp_mirror::UdpMirrorTask;

mod hardcoded_bindings;
mod network;
mod network_config;
mod service;
mod udp_mirror;

/// UDP mirror task name.
const UDP_MIRROR_TASK_NAME: TaskName = TaskName::new(5);

/// Definition and configuration of auto-created tasks.
#[no_mangle]
pub static TASK_LIST: &[TaskCreationConfig] = &[
    TaskCreationConfig::from_task::<UdpMirrorTask>("UdpMirrorTaskCreation"),
    TaskCreationConfig::from_task::<NetworkStackTask>("NetworkStackTaskCreation"),
];
