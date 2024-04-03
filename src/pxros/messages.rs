//! Abstraction over Pxros message API.

use core::pin::Pin;
use core::slice;
use core::task::{Context, Poll};

use futures::{pin_mut, Future, Stream};
use pxros::bindings::{
    PxError_t,
    PxEvents_t,
    PxGetError,
    PxHandle,
    PxMbx_t,
    PxMc_t,
    PxMsgAwaitRel,
    PxMsgAwaitRel_EvWait,
    PxMsgAwaitRel_NoWait,
    PxMsgData_t,
    PxMsgEvent_t,
    PxMsgGetBuffersize,
    PxMsgGetData,
    PxMsgGetMetadata,
    PxMsgGetOwner,
    PxMsgGetProtection,
    PxMsgGetSender,
    PxMsgGetSize,
    PxMsgInstallRelmbx,
    PxMsgMetadata_t,
    PxMsgReceive,
    PxMsgReceive_EvWait,
    PxMsgReceive_NoWait,
    PxMsgRelease,
    PxMsgRequest,
    PxMsgRequest_EvWait,
    PxMsgRequest_NoWait,
    PxMsgSend,
    PxMsgSend_Prio,
    PxMsgSetMetadata,
    PxMsgSetProtection,
    PxMsgSetToAwaitRel,
    PxMsg_t,
    PxOpool_t,
    PxProtectType_t,
    PxTaskGetMbx,
    PxTask_t,
};
use pxros::PxResult;

use super::executor::local_data::wait_for_message;
use crate::pxros::events::Event;
use crate::pxros::name_server::{NameServer, TaskName};

/// Wraps [`PxMsgEvent_t`] in a Rust enum for requesting or receiving a message with events.
#[derive(Debug, defmt::Format)]
pub enum NewMessageEvents {
    Message(RawMessage),
    Events(PxEvents_t),
    Both((RawMessage, PxEvents_t)),
}

impl TryFrom<PxMsgEvent_t> for NewMessageEvents {
    type Error = PxError_t;

    fn try_from(message_event: PxMsgEvent_t) -> Result<Self, Self::Error> {
        let PxMsgEvent_t {
            msg: message_handle,
            events,
        } = message_event;
        // On PXERR_MSG_NOMSG, events can still be set.
        if message_handle.error() != PxError_t::PXERR_MSG_NOMSG {
            let _ = message_handle.checked()?;
        }
        match (message_handle.is_valid(), events.0 != 0) {
            (false, false) => {
                defmt::unreachable!("PxMsgEvent_t should contain an error or at least one message or event.");
            },
            (true, false) => Ok(NewMessageEvents::Message(RawMessage { message_handle })),
            (false, true) => Ok(NewMessageEvents::Events(events)),
            (true, true) => Ok(NewMessageEvents::Both((RawMessage { message_handle }, events))),
        }
    }
}

/// Wraps [`PxMsgEvent_t`] in a Rust enum for the awaiting message release with events.
#[derive(Debug, defmt::Format)]
pub enum ReleaseWithEvent {
    Released(Option<PxEvents_t>),
    NotReleased(PxEvents_t),
}

impl TryFrom<PxMsgEvent_t> for ReleaseWithEvent {
    type Error = PxError_t;

    fn try_from(message_event: PxMsgEvent_t) -> Result<Self, Self::Error> {
        let PxMsgEvent_t {
            msg: message_handle,
            events,
        } = message_event;
        // On PXERR_MSG_NOMSG, events can still be set.
        if message_handle.error() != PxError_t::PXERR_MSG_NOMSG {
            let _ = message_handle.checked()?;
        }
        match (message_handle.is_valid(), events.0 != 0) {
            (false, false) => {
                defmt::unreachable!("PxMsgEvent_t should contain an error or at least one message or event.");
            },
            (true, false) => Ok(ReleaseWithEvent::Released(None)),
            (false, true) => Ok(ReleaseWithEvent::NotReleased(events)),
            (true, true) => Ok(ReleaseWithEvent::Released(Some(events))),
        }
    }
}

/// Wraps a [PxMsg_t] to provide checked access to message methods.
#[derive(Debug, Eq, PartialEq)]
pub struct RawMessage {
    message_handle: PxMsg_t,
}

impl RawMessage {
    /// Awaits the release of a marked message.
    ///
    /// See [`PxMsgAwaitRel`] for details.
    pub fn await_release(&self) -> PxResult<()> {
        PxMsgAwaitRel(self.message_handle).checked().map(|_| ())
    }

    /// Awaits the release of a marked message or returns if an event occurs.
    ///
    /// See [`PxMsgAwaitRel_EvWait`] for details.
    pub fn await_release_with_events(&self, events: PxEvents_t) -> PxResult<ReleaseWithEvent> {
        PxMsgAwaitRel_EvWait(self.message_handle, events).try_into()
    }

    /// Awaits the release of a marked message without blocking.
    ///
    /// See [`PxMsgAwaitRel_NoWait`] for details.
    pub fn await_release_no_wait(&self) -> PxResult<()> {
        PxMsgAwaitRel_NoWait(self.message_handle).checked().map(|_| ())
    }

    /// Returns the buffer size.
    ///
    /// See [`PxMsgGetBuffersize`] for details.
    pub fn buffer_size(&self) -> PxResult<u32> {
        let size = PxMsgGetBuffersize(self.message_handle);
        if size == 0 && PxGetError() != PxError_t::PXERR_NOERROR {
            return Err(PxGetError());
        }
        Ok(size)
    }

    /// Returns the metadata.
    ///
    /// See [`PxMsgGetMetadata`] for details.
    pub fn metadata(&self) -> PxResult<PxMsgMetadata_t> {
        let metadata = PxMsgGetMetadata(self.message_handle);

        if metadata.0 == 0 && PxGetError() != PxError_t::PXERR_NOERROR {
            return Err(PxGetError());
        }
        Ok(metadata)
    }

    /// Returns the owner.
    ///
    /// See [`PxMsgGetOwner`] for details.
    pub fn owner(&self) -> PxResult<PxTask_t> {
        PxMsgGetOwner(self.message_handle).checked()
    }

    /// Returns the protection mode.
    ///
    /// See [`PxMsgGetProtection`] for details.
    pub fn protection(&self) -> PxResult<PxProtectType_t> {
        let protection = PxMsgGetProtection(self.message_handle);
        if protection == PxProtectType_t::NoAccessProtection && PxGetError() != PxError_t::PXERR_NOERROR {
            return Err(PxGetError());
        }
        Ok(protection)
    }

    /// Returns the sender.
    ///
    /// See [`PxMsgGetSender`] for details.
    pub fn sender(&self) -> PxResult<PxTask_t> {
        PxMsgGetSender(self.message_handle).checked()
    }

    /// Returns the data size.
    ///
    /// See [`PxMsgGetSize`] for details.
    pub fn size(&self) -> PxResult<u32> {
        let size = PxMsgGetSize(self.message_handle);
        if size == 0 && PxGetError() != PxError_t::PXERR_NOERROR {
            return Err(PxGetError());
        }
        Ok(size)
    }

    /// Installs a release mailbox.
    ///
    /// See [`PxMsgInstallRelmbx`] for details.
    pub fn install_release_mailbox(&mut self, mailbox: PxMbx_t) -> PxResult<()> {
        PxMsgInstallRelmbx(self.message_handle, mailbox).into()
    }

    /// Receives a message.
    ///
    /// See [`PxMsgReceive`] for details.
    pub fn receive(mailbox: PxMbx_t) -> PxResult<Self> {
        let message_handle = PxMsgReceive(mailbox).checked()?;
        Ok(Self { message_handle })
    }

    /// Receives a message or returns if an event occurs.
    ///
    /// See [`PxMsgReceive_EvWait`] for details.
    pub fn receive_with_events(mailbox: PxMbx_t, events: PxEvents_t) -> PxResult<NewMessageEvents> {
        PxMsgReceive_EvWait(mailbox, events).try_into()
    }

    /// Receives a message without blocking.
    ///
    /// See [`PxMsgReceive_NoWait`] for details.
    pub fn receive_no_wait(mailbox: PxMbx_t) -> PxResult<Self> {
        let message_handle = PxMsgReceive_NoWait(mailbox).checked()?;
        Ok(Self { message_handle })
    }

    /// Requests a message.
    ///
    /// See [`PxMsgRequest`] for details.
    pub fn request(message_size: u32, memory_class: PxMc_t, object_pool: PxOpool_t) -> PxResult<Self> {
        let message_handle = PxMsgRequest(message_size, memory_class, object_pool).checked()?;
        Ok(Self { message_handle })
    }

    /// Requests a message or returns if an event occurs.
    ///
    /// See [`PxMsgRequest_EvWait`] for details.
    pub fn request_with_events(
        message_size: u32,
        memory_class: PxMc_t,
        object_pool: PxOpool_t,
        events: PxEvents_t,
    ) -> PxResult<NewMessageEvents> {
        PxMsgRequest_EvWait(message_size, memory_class, object_pool, events).try_into()
    }

    /// Requests a message without blocking.
    ///
    /// See [`PxMsgRequest_NoWait`] for details.
    pub fn request_no_wait(message_size: u32, memory_class: PxMc_t, object_pool: PxOpool_t) -> PxResult<Self> {
        let message_handle = PxMsgRequest_NoWait(message_size, memory_class, object_pool).checked()?;
        Ok(Self { message_handle })
    }

    /// Sends a message.
    ///
    /// See [`PxMsgSend`] for details.
    pub fn send(&mut self, mailbox: PxMbx_t) -> PxResult<()> {
        PxMsgSend(self.message_handle, mailbox).checked().map(|_| ())
    }

    /// Sends a message with priority.
    ///
    /// See [`PxMsgSend_Prio`] for details.
    pub fn send_prio(&mut self, mailbox: PxMbx_t) -> PxResult<()> {
        PxMsgSend_Prio(self.message_handle, mailbox).checked().map(|_| ())
    }

    /// Sets the metadata.
    ///
    /// See [`PxMsgSetMetadata`] for details.
    pub fn set_metadata(&mut self, metadata: PxMsgMetadata_t) -> PxResult<()> {
        PxMsgSetMetadata(self.message_handle, metadata).into()
    }

    /// Sets the protection mode.
    ///
    /// See [`PxMsgSetProtection`] for details.
    pub fn set_protection(&mut self, protection: PxProtectType_t) -> PxResult<()> {
        PxMsgSetProtection(self.message_handle, protection).into()
    }

    /// Sets the message to be release awaitable.
    ///
    /// See [`PxMsgSetToAwaitRel`] for details.
    pub fn set_await_release(&mut self) -> PxResult<()> {
        PxMsgSetToAwaitRel(self.message_handle).into()
    }

    /// Returns the data.
    ///
    /// See [`PxMsgData_t`] for details.
    pub fn data(&self) -> PxResult<&[u8]> {
        // Safety:
        // PXROS methods check their parameters.
        // If the size is zero, the last error is checked.
        let PxMsgData_t(data_pointer) = unsafe { PxMsgGetData(self.message_handle) };
        if data_pointer.is_null() {
            return Err(PxGetError());
        }

        let message_size = self.size()?;
        // Safety: The data pointer has been checked to be non-null and the message size is provided by PXROS.
        let data_area = unsafe { slice::from_raw_parts_mut(data_pointer as *mut u8, message_size as usize) };
        Ok(data_area)
    }

    /// Returns the data mutable.
    ///
    /// See [`PxMsgData_t`] for details.
    pub fn data_mut(&mut self) -> PxResult<&mut [u8]> {
        // Safety:
        // PXROS methods check their parameters.
        // If the size is zero, the last error is checked.
        let PxMsgData_t(data_pointer) = unsafe { PxMsgGetData(self.message_handle) };
        if data_pointer.is_null() {
            return Err(PxGetError());
        }

        let message_size = self.size()?;
        // Safety: The data pointer has been checked to be non-null and the message size is provided by PXROS.
        let data_area = unsafe { slice::from_raw_parts_mut(data_pointer as *mut u8, message_size as usize) };
        Ok(data_area)
    }

    /// Releases the message.
    ///
    /// ## ID Reuse
    /// **While this method does not consume `self`, using a handle that has been successfully released may result in
    /// unexpected behavior. PXROS reuses message IDs immediately, which can lead to released but valid again handles.
    /// While this is safe from a Rust perspective as no memory safety invariant is broken, it should be avoided in all
    /// circumstances.**
    ///
    /// See [`PxMsgRelease`] for details.
    pub fn release(&mut self) -> Result<(), PxError_t> {
        defmt::trace!("Releasing (internal): {}", self.message_handle.id());
        // Safety:
        // PXROS methods check their parameters.
        // The returned PxMsg_t is checked for errors then discarded.
        let message_handle = unsafe { PxMsgRelease(self.message_handle) };
        message_handle.error().into()
    }
}

impl defmt::Format for RawMessage {
    fn format(&self, fmt: defmt::Formatter) {
        let owner = self.owner().unwrap_or(PxHandle::invalid());
        let sender = self.sender().unwrap_or(PxHandle::invalid());
        let data = self.data().unwrap_or(&[]);

        defmt::write!(fmt, "Message (sender: {:?} owner: {:?}, data: {:?})", sender, owner, data);
    }
}

/// Allows to send *messages* to other tasks in non-blocking manner.
///
/// ## Note
/// At the moment this only allows for sending raw bytes. Future PRs will
/// provide better usage through serializers, sinks, iterators, etc.
pub struct MailSender {
    mailbox: PxMbx_t,
    class: PxMc_t,
    pool: PxOpool_t,
}

impl MailSender {
    /// Return a MailSender that allows to send messages to the task.
    ///
    /// See [PxTaskGetMbx] for details and failure reasons.
    pub fn new(task: PxTask_t) -> PxResult<Self> {
        // Safety: this is safe to call and errors are handled
        let mailbox = unsafe { PxTaskGetMbx(task) }.checked();

        mailbox.map(|mailbox| MailSender {
            mailbox,
            class: Default::default(),
            pool: Default::default(),
        })
    }

    /// Utility method to return a MailSender given a [TaskName].
    ///
    /// This uses the [NameServer] to query the task internally; the ticker
    /// event is used only during the query duration. After that it can be
    /// reused.
    pub fn new_for<T: Event>(&mut self, name: &TaskName, ticker: T) -> PxResult<Self> {
        NameServer::query(name, ticker).and_then(Self::new)
    }

    /// Send raw bytes.
    ///
    /// This function clones the bytes in the mailbox and transfers
    /// them to the recipient. For details see [PxMsgRequest], [PxMsgGetData]
    /// and [PxMsgSend].
    pub fn send_bytes(&mut self, src: &[u8]) -> PxResult<()> {
        let length = src.len() as u32;

        let mut message = RawMessage::request(length, self.class, self.pool)?;
        message.data_mut()?.copy_from_slice(src);

        // Send the message and clear any memory
        message.send(self.mailbox)
    }
}

/// Async variant of [super::events::Receiver] to receive messages.
///
/// This receiver implements [Stream] for async usage.
#[derive(Debug, Default)]
pub struct AsyncMessageReceiver {}

impl AsyncMessageReceiver {
    /// Create a new receiver.
    pub fn new() -> Self {
        Self {}
    }

    /// Asynchronously wait for a message to be received.
    pub async fn wait(&mut self) -> RawMessage {
        wait_for_message().await
    }
}

impl Stream for AsyncMessageReceiver {
    type Item = RawMessage;

    fn poll_next(self: Pin<&mut Self>, cx: &mut Context<'_>) -> Poll<Option<Self::Item>> {
        let future = wait_for_message();
        pin_mut!(future);

        future.poll(cx).map(Some)
    }
}
