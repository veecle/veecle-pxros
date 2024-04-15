//! Application code to demonstrate the demo
use pxros::bindings::*;
use pxros::PxResult;

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
#[veecle_pxros::pxros_task(auto_spawn(core = 2, priority = 10))]
fn customer_app(mailbox: PxMbx_t) -> PxResult<()> {
    // Wait for the global UDP service to start on core 1.
    let tx_mailbox = Service::Network.wait_for_service(PXCORE_1, AppEvents::Ticker)?;

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

        // Echo payload to sender.
        let message = UdpMessage::new(endpoint, payload);
        udp.send(message, tx_mailbox)?;

        // Release the message.
        raw_message.release()?;
    }
}
