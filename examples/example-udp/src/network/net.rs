//! [smoltcp] network implementation.
//!
//! This is super basic. In general, we would require a stack that allows to
//! bind "sockets" to specific message APIs and deliver those to tasks (sort
//! of like 1:1 channels).
//!
//! For this to work we need more sophisticated message passing APIs. Ideally we
//! need support for automated `enum` serialization (through metadata + payload).
//!
//! Until then, this is sort of "hacked together" and not really proper.
use core::time::Duration;

use smoltcp::iface::{Config, Interface, SocketSet, SocketStorage};
use smoltcp::socket::udp::{self, PacketMetadata, RecvError};
use smoltcp::storage::PacketBuffer;
use smoltcp::time::Instant;
use smoltcp::wire::{HardwareAddress, IpCidr, IpEndpoint};
use veecle_pxros::pxros::name_server::NameServer;
use veecle_pxros::pxros::ticker::Ticker;
use veecle_pxros::pxros::time::time_since_boot;

use super::udp::UdpMailbox;
use super::{NetworkEvents, PxDevice};
use crate::config::{ETH_ADDRESS, MY_IP, MY_PORT, UDP_MTU};
use crate::network::udp::UdpMessage;
use crate::CUSTOMER_APP_TASK_NAME;

/// Runs the network stack, forever.
///
/// ## TODO
/// * This is all allocated in the stack, move to 'static or increase stack size.
///
/// ### Panics
/// Panics on smolTCP error.
pub fn run_network_stack(mut device: PxDevice, mut udp_server: UdpMailbox) -> ! {
    // **UDP** socket buffers. For now in the stack - can be 'static in the future.
    let mut tx_buffer = [0_u8; UDP_MTU * 2];
    let mut rx_buffer = [0_u8; UDP_MTU * 2];
    let mut tx_meta_buffer = [PacketMetadata::EMPTY; 2];
    let mut rx_meta_buffer = [PacketMetadata::EMPTY; 2];

    // Socket storage (up to 1).
    let mut sockets_buffer = [SocketStorage::EMPTY; 1];

    // Configuration for smolTCP L2.
    let config = Config::new(HardwareAddress::Ethernet(ETH_ADDRESS));

    // Initialize the L2 device.
    let mut iface = Interface::new(config, &mut device, smoltcp_now());

    // Configure IPs and default routes.
    iface.update_ip_addrs(|ip_addrs| {
        ip_addrs.push(IpCidr::new(MY_IP, 24)).unwrap();
    });

    // Create a socket set.
    let mut sockets = SocketSet::new(sockets_buffer.as_mut_slice());

    // Create UDP buffers.
    let tx_buffer = PacketBuffer::new(tx_meta_buffer.as_mut_slice(), tx_buffer.as_mut_slice());
    let rx_buffer = PacketBuffer::new(rx_meta_buffer.as_mut_slice(), rx_buffer.as_mut_slice());

    // Create the UDP socket and add it to the set.
    let mut socket = udp::Socket::new(rx_buffer, tx_buffer);
    socket.bind(IpEndpoint::new(MY_IP, MY_PORT)).unwrap();
    let handle = sockets.add(socket);

    // We support a single connected task and we do not check
    // if it is valid.
    let task_connected = NameServer::query(&CUSTOMER_APP_TASK_NAME, NetworkEvents::Ticker).unwrap();

    // TODO
    // Create a ticker that trigger a network event every 100ms...
    // this is an "hack" for handling ARP requests or so... the proper way
    // would be to use iface.poll_at and schedule a single-time event to be
    // triggered after the returned time.
    let _ticker = Ticker::every(NetworkEvents::EthernetEvent, Duration::from_millis(100)).unwrap();

    loop {
        // Perform network I/O.
        let _ = iface.poll(smoltcp_now(), &mut device, &mut sockets);

        // Perform socket I/O by pulling potentially queued packets and delivering
        // them to the connected task.
        let udp = sockets.get_mut::<udp::Socket>(handle);
        match udp.recv() {
            Ok((bytes, endpoint)) => {
                defmt::debug!(
                    "Delivering RX-UDP packet from {:?} - {:?} bytes long to task {:?}.",
                    endpoint,
                    bytes.len(),
                    task_connected
                );

                let message = UdpMessage::new(endpoint.endpoint, bytes);
                if let Err(error) = udp_server.send_to(message, task_connected) {
                    defmt::warn!("Could not send message to mailbox of {:?} - error: {:?}", task_connected, error);
                }
            },
            Err(RecvError::Exhausted) => {
                // TODO
                // Nothing received. Here we would register an async handle. This is ignored for now.
            },
            Err(RecvError::Truncated) => {
                defmt::warn!("Truncated error while received packet (not enough buffer space).")
            },
        }

        // Wait for an event to happen:
        // * If an ethernet event is received, there is nothing do to. By looping we poll the stack which will then poll
        //   eventually queued messages from the driver.
        //
        // * If a UDP message arrives, pop it out and push it into the socket queue. By polling the stack that packet
        //   will be then sent to the network.
        match udp_server.receive_or_event(NetworkEvents::EthernetEvent) {
            Ok(None) => {
                // No UDP message received, but Ethernet has work to do. Just loop and poll the
                // stack again.
            },
            Ok(Some(mut message)) => {
                let bytes = message.data().expect("Failed to get message data");

                // Decode the UdpMessage and extract payload + endpoint
                let udp_message = UdpMessage::from_bytes(bytes);
                let payload = udp_message.payload();
                let endpoint = udp_message.endpoint();

                match udp.send(payload.len(), endpoint) {
                    Ok(bytes) => {
                        bytes.copy_from_slice(payload);
                        defmt::debug!("Queue UDP packet for: {:?} of length: {:?} bytes.", endpoint, payload.len());
                    },
                    Err(e) => {
                        // Nothing sent. Here we would register an async handle
                        defmt::warn!("Cannot send packet: {:?} - freeing buffers.", e);

                        // For demo purposes we clear the buffer as to not stop the socket from
                        // sending frames. This is in case stuff disconnects or whatever while testing.
                        udp.clear_tx_buffer();
                    },
                }

                // Finally, release the message.
                message.release().expect("Failed to release message.");
            },
            Err(error) => {
                defmt::error!("Failed while receiving UDP: {:?}", error);
            },
        }
    }
}

/// Returns a smoltcp [Instant](smoltcp::time::Instant) from HighTec time.
pub fn smoltcp_now() -> Instant {
    let millis = i64::try_from(time_since_boot().as_millis()).unwrap_or(i64::MAX);
    Instant::from_millis(millis)
}
