//! Application code to demonstrate the demo
use core::ffi::CStr;

use pxros::bindings::*;
use pxros::PxResult;
use veecle_pxros::pxros::name_server::TaskName;
use veecle_pxros::pxros::task::PxrosTask;

use crate::network::udp::{UdpMailbox, UdpMessage};
use crate::Service;

bitflags::bitflags! {
    /// Events used by the application
    #[derive(Copy, Clone)]
    pub struct AppEvents: u32 {
        const Ticker = 0b0001_0000;
    }
}

/// Example application running on core 2 at priority 10.
///
/// This application opens a socket via the network task and performs
/// basic gateway functionalities by forwarding packets between parties.
///
/// No special permission or resource is required.

pub(crate) struct CustomerApp;

impl PxrosTask for CustomerApp {
    fn task_main(mailbox: PxMbx_t) -> PxResult<()> {
        // Wait for the global UDP service to start on core 1.
        let tx_mailbox = Service::Network.wait_for_service(PXCORE_1, AppEvents::Ticker)?;

        // Register the socket.
        let mut udp = UdpMailbox::register(mailbox, tx_mailbox);

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
        Some(TaskName::new(5))
    }

    fn debug_name() -> &'static CStr {
        CStr::from_bytes_with_nul("Customer_App_Task\0".as_bytes())
            .expect("The debug name should be a valid, zero-terminated C string.")
    }

    fn priority() -> PxPrio_t {
        PxPrio_t(10)
    }

    fn core() -> u32 {
        2
    }
}
