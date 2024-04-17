//! UDP mailbox utilities.
use pxros::bindings::*;
use pxros::PxResult;
use smoltcp::wire::{IpAddress, IpEndpoint, Ipv4Address};
use veecle_pxros::pxros::events::Event;
use veecle_pxros::pxros::messages::{NewMessageEvents, RawMessage};

/// Naive [RawMessage] payload wrapper for udp messages.
#[derive(Debug, Clone, defmt::Format)]
pub struct UdpMessage<'a> {
    endpoint: IpEndpoint,
    payload: &'a [u8],
}

impl<'a> UdpMessage<'a> {
    /// Create a new UdpMessage from raw fields.
    pub fn new(endpoint: impl Into<IpEndpoint>, payload: &'a [u8]) -> UdpMessage<'a> {
        Self {
            endpoint: endpoint.into(),
            payload,
        }
    }

    /// Return the size this structure requires to be serialized into
    /// raw bytes.
    pub fn byte_size(&self) -> usize {
        let ip_size_in_bytes = 4;
        let port_size_in_bytes = 2;
        self.payload.len() + ip_size_in_bytes + port_size_in_bytes
    }

    /// Raw serialize self into bytes (not FFI compliant, nor checked).
    pub fn to_bytes(&self, bytes: &mut [u8]) {
        assert_eq!(bytes.len(), self.byte_size(), "This buffer must be exactly byte_size() long.");

        bytes[..4].copy_from_slice(self.endpoint.addr.as_bytes());
        bytes[4..6].copy_from_slice(&self.endpoint.port.to_ne_bytes());
        bytes[6..].copy_from_slice(self.payload);
    }

    /// Raw deserialize self from bytes (not FFI compliant, nor checked).
    pub fn from_bytes(bytes: &'a [u8]) -> UdpMessage<'a> {
        let ip_v4 = Ipv4Address::from_bytes(&bytes[..4]);
        let port = u16::from_ne_bytes(
            bytes[4..6]
                .try_into()
                .expect("A 2 byte slice should always be convertible to a [u8;2]."),
        );

        let endpoint = IpEndpoint::new(IpAddress::from(ip_v4), port);

        UdpMessage::new(endpoint, &bytes[6..])
    }

    /// Return the payload of the message.
    pub fn payload(&self) -> &[u8] {
        self.payload
    }

    /// Return the endpoint of the message.
    pub fn endpoint(&self) -> IpEndpoint {
        self.endpoint
    }
}

/// Mailbox for processing UDP packets.
///
/// ### TODO
/// * Support tracking of socket handles.
/// * Support client/server channels.
/// * Support endpoint configuration and connection.
/// * Support complex data type for <T: serialize/deserialize> in FFI compatible way.
#[derive(Debug, Clone, Copy)]
pub struct UdpMailbox {
    rx_mailbox: PxMbx_t,
}

impl UdpMailbox {
    /// Initializes the mailbox.
    pub fn init(rx_mailbox: PxMbx_t) -> Self {
        Self { rx_mailbox }
    }

    /// Register a mailbox.
    pub fn register(rx_mailbox: PxMbx_t) -> Self {
        UdpMailbox::init(rx_mailbox)
    }

    /// Sends UDP bytes to the task's private mailbox.
    pub fn send_to(&mut self, message: UdpMessage<'_>, task: PxTask_t) -> PxResult<()> {
        unsafe { PxTaskGetMbx(task) }
            .checked()
            .and_then(|mailbox| self.send(message, mailbox))
    }

    /// Sends a [UdpMessage] to the mailbox.
    pub fn send(&mut self, message: UdpMessage<'_>, mailbox: PxMbx_t) -> PxResult<()> {
        let size = message.byte_size();
        let mut handle = RawMessage::request(size as u32, PxMc_t::default(), PxOpool_t::default())?;

        // Copy the raw bytes into the message.
        message.to_bytes(handle.data_mut()?);

        // Send it to the mailbox.
        handle.send(mailbox).map_err(|error| {
            defmt::warn!("Error {:?} while sending message: {:?}", error, handle);
            handle.release().expect("Could not release message");

            error
        })?;

        Ok(())
    }

    /// Receives a UDP [RawMessage].
    ///
    /// The function is blocking.
    pub fn receive(&mut self) -> PxResult<RawMessage> {
        RawMessage::receive(self.rx_mailbox)
    }

    /// Receives a UDP [RawMessage] or returns upon [Event].
    ///
    /// The function is blocking, but it allows events reception to early return
    /// even if a message is not received.
    pub fn receive_or_event<E: Event>(&mut self, events: E) -> PxResult<Option<RawMessage>> {
        let events = PxEvents_t(events.bits());
        let result = RawMessage::receive_with_events(self.rx_mailbox, events)?;
        match result {
            NewMessageEvents::Message(message) => Ok(Some(message)),
            NewMessageEvents::Events(_) => Ok(None),
            NewMessageEvents::Both((message, _)) => Ok(Some(message)),
        }
    }
}
