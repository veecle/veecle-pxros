//! Task mirroring every UDP datagram to its sender.
use core::ffi::CStr;

use pxros::bindings::*;
use pxros::PxResult;
use veecle_pxros::pxros::name_server::TaskName;
use veecle_pxros::pxros::task::PxrosTask;

use crate::network::udp::{UdpMailbox, UdpMessage};
use crate::service::Service;
use crate::UDP_MIRROR_TASK_NAME;

bitflags::bitflags! {
    /// Events used by the UDP mirror.
    #[derive(Copy, Clone)]
    pub struct UdpMirrorEvents: u32 {
        const Ticker = 0b0001_0000;
    }
}

/// Mirrors every UDP datagram to its sender.
pub(crate) struct UdpMirrorTask;

impl PxrosTask for UdpMirrorTask {
    fn task_main(mailbox: PxMbx_t) -> PxResult<()> {
        // Wait for the global UDP service to start on core 1.
        let tx_mailbox = Service::Network.wait_for_service(PXCORE_1, UdpMirrorEvents::Ticker)?;

        // Register the socket.
        let mut udp = UdpMailbox::register(mailbox);

        defmt::info!("Entering main application loop");
        loop {
            // Wait for a UDP frame to arrive; return on error.
            let mut raw_message = udp.receive()?;

            // Validate the message payload and deserialize the UdpMessage.
            let received_udp_message = UdpMessage::from_bytes(raw_message.data()?);

            // Obtain the endpoint and payload.
            let payload = received_udp_message.payload();
            let endpoint = received_udp_message.endpoint();

            defmt::info!(
                "Received \"{}\" from {}, returning packet.",
                core::str::from_utf8(payload).unwrap().strip_suffix('\n').unwrap(),
                endpoint
            );

            // Echo payload to sender.
            let message = UdpMessage::new(endpoint, payload);
            udp.send(message, tx_mailbox)?;

            // Release the message.
            raw_message.release()?;
        }
    }

    fn task_name() -> Option<TaskName> {
        Some(UDP_MIRROR_TASK_NAME)
    }

    fn debug_name() -> &'static CStr {
        CStr::from_bytes_with_nul("Udp_Mirror_Task\0".as_bytes())
            .expect("The debug name should be a valid, zero-terminated C string.")
    }

    fn priority() -> PxPrio_t {
        PxPrio_t(10)
    }

    fn core() -> u32 {
        2
    }
}
