use pxros::bindings::*;
use pxros::PxResult;
use veecle_pxros::pxros::events::Event;
use veecle_pxros::pxros::messages::{NewMessageEvents, RawMessage};

/// Mailbox for processing CAN packets.
#[derive(Debug, Clone, Copy)]
pub struct CanMailbox {
    rx_mailbox: PxMbx_t,
}

impl CanMailbox {
    /// Initializes the mailbox.
    pub fn init(rx_mailbox: PxMbx_t) -> Self {
        Self { rx_mailbox }
    }

    /// Sends CAN bytes to the mailbox.
    pub fn send(&mut self, bytes: &[u8], mailbox: PxMbx_t) -> PxResult<()> {
        let mut handle = RawMessage::request(bytes.len() as u32, PxMc_t::default(), PxOpool_t::default())?;

        // Copy the raw bytes into the message.
        handle.data_mut()?.copy_from_slice(bytes);

        // Send it to the mailbox.
        handle.send(mailbox).map_err(|error| {
            defmt::warn!("Error {:?} while sending message: {:?}", error, handle);
            handle.release().expect("Could not release message");

            error
        })?;

        Ok(())
    }

    /// Receives a CAN [RawMessage] or returns upon [Event].
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
