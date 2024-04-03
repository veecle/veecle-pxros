//! Abstraction of Pxros event API.

use core::pin::Pin;
use core::task::{Context, Poll};

use bitflags::Flags;
use futures::{pin_mut, Future, Stream};
use pxros::bindings::{PxAwaitEvents, PxEvents_t, PxMbx_t, PxResetEvents, PxTaskSignalEvents, PxTask_t};
use pxros::PxResult;

use super::executor::local_data::wait_for_event;
use super::messages::{NewMessageEvents, RawMessage};
use super::name_server::{NameServer, TaskName};

/// Specialized trait compatible with PXROS events (u32).
///
/// This utilizes [bitflags] under the hood to simplify
/// arithmetic between events.
pub trait Event: Copy + Flags<Bits = u32> {}

// Any u32 is an event.
impl<F: Copy + Flags<Bits = u32>> Event for F {}

/// A Signaller that sends [Event] to another task.
pub struct Signaller<E: Event> {
    signal: E,
    task: PxTask_t,
}

impl<E: Event> Signaller<E> {
    /// Create a new Signaller.
    pub const fn new(signal: E, task: PxTask_t) -> Self {
        Self { signal, task }
    }

    /// Utility method to return a signaller given a [TaskName].
    ///
    /// This uses the [NameServer] to query the task internally; the ticker
    /// event is used only during the query duration. After that it can be
    /// reused.
    pub fn new_for<T: Event>(&mut self, signal: E, ticker: T, name: &TaskName) -> PxResult<Self> {
        NameServer::query(name, ticker).map(|task| Self::new(signal, task))
    }

    /// Signal the task in non-blocking manner.
    ///
    /// This function may return an error if the kernel fails at delivering
    /// the event. See [PxTaskSignalEvents] for details.
    ///
    /// ## Panics
    /// This will panic if called from ISR context.
    pub fn signal(&mut self) -> PxResult<()> {
        let events = PxEvents_t(self.signal.bits());
        // Safety: this is safe to call only from tasks; this may panic.
        let result = unsafe { PxTaskSignalEvents(self.task, events) };
        PxResult::from(result)
    }
}

/// Entity that allows receiving [RawMessage] and [Event] from other
/// tasks.
///
/// For the async variant refer to [AsyncEventReceiver] and
/// [super::messages::AsyncMessageReceiver].
///
/// # Note
/// This interface is incomplete. Future PRs shall ensure this is
/// generated at compile time to avoid mistakes.
pub struct Receiver<E: Event> {
    mailbox: PxMbx_t,
    event: E,
}

impl<E: Event> Receiver<E> {
    /// Returns a [Receiver] able to receive both [RawMessage] and [Event].
    pub fn new(mailbox: PxMbx_t, event: E) -> Receiver<E> {
        Receiver { mailbox, event }
    }

    /// Resets the specified events. The reset events are returned.
    pub fn reset_events(events: E) -> PxEvents_t {
        // Safety:
        // Documentation states no conditions.
        unsafe { PxResetEvents(PxEvents_t(events.bits())) }
    }

    /// Awaits specified events.
    ///
    /// ### Panics
    /// This will panic if events == 0.
    pub fn await_events(events: E) -> PxEvents_t {
        let events = PxEvents_t(events.bits());
        defmt::assert!(events.0 != 0, "Cannot await on zero events - will wait forever.");

        // Safety:
        // Precondition of all-zero bitmask checked, documentation states no other conditions.
        unsafe { PxAwaitEvents(events) }
    }

    /// Receives a message or event in blocking manner.
    ///
    /// This is guaranteed to return when either an event or a message is
    /// received. See [pxros::bindings::PxMsgReceive_EvWait] for details.
    pub fn receive(&mut self) -> (E, Option<RawMessage>) {
        let events = PxEvents_t(self.event.bits());

        let Ok(message_events) = RawMessage::receive_with_events(self.mailbox, events) else {
            return (E::empty(), None);
        };

        match message_events {
            NewMessageEvents::Message(message) => (E::empty(), Some(message)),
            NewMessageEvents::Events(event) => {
                let events = E::from_bits(event.0).expect("Message events should not contain unknown bits.");
                if !self.event.contains(events) {
                    defmt::panic!(
                        "Received events: {}, but this is not contained in {}",
                        events.bits(),
                        self.event.bits()
                    );
                }
                (events, None)
            },
            NewMessageEvents::Both((message, event)) => {
                let event = E::from_bits(event.0).expect("Message events should not contain unknown bits.");
                if !self.event.contains(event) {
                    defmt::panic!(
                        "Received events: {}, but this is not contained in {}",
                        event.bits(),
                        self.event.bits()
                    );
                }
                (event, Some(message))
            },
        }
    }

    /// Tries to receive a message in non-blocking manner.
    ///
    /// This method does not check for events.
    pub fn try_message_receive(&self) -> Option<RawMessage> {
        RawMessage::receive_no_wait(self.mailbox).ok()
    }
}

/// Async variant of [Receiver] to receive events.
///
/// This receiver implements [Stream] for async usage.
#[derive(Debug)]
pub struct AsyncEventReceiver<E: Event> {
    event: E,
}

impl<E: Event> AsyncEventReceiver<E> {
    /// Creates a new receiver.
    pub fn new(event: E) -> Self {
        Self { event }
    }

    /// Asynchronously waits for the event to be received.
    pub async fn wait(&mut self) {
        wait_for_event(self.event).await;
    }
}

impl<E: Event> Stream for AsyncEventReceiver<E> {
    type Item = ();

    fn poll_next(self: Pin<&mut Self>, cx: &mut Context<'_>) -> Poll<Option<Self::Item>> {
        let future = wait_for_event(self.event);
        pin_mut!(future);

        future.poll(cx).map(Some)
    }
}
