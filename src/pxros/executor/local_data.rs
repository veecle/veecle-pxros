//! Data stored in the Waker, defined by a task and used in asynchronous contexts.

use core::future::poll_fn;
use core::task::{Context, Poll};

use crate::pxros::events::Event;
use crate::pxros::messages::RawMessage;

/// Task-specific context used by the executor to provide async
/// functionalities.
///
/// This is an implementation detail of the PxROS executor.
#[derive(Default)]
pub struct PxrosData {
    /// All events that a task is waiting for
    ///
    /// This field is _set_ by the task logic and _cleared_ by the executor after
    /// each trigger
    awaiting_events: u32,
    /// Lists all events that have been triggered for the task
    ///
    /// This field is _set_ by the executor and _cleared_ by the task logic
    triggered_events: u32,
    /// If true it means this task is awaiting a message; this must be _set_
    /// by the task logic and and _cleared_ by the executor
    awaiting_message: bool,
    /// Message payload that shall be filled by the executor when clearing the
    /// _message_ flag
    message: Option<RawMessage>,
}

impl PxrosData {
    /// Run a closure on the task local data.
    ///
    /// # Panics
    /// This function will panic if called outside the executor.
    pub fn access<F, R>(callback: F, cx: &Context<'_>) -> Poll<R>
    where
        F: FnMut(&mut Self) -> Poll<R>,
    {
        crate::executor::task::with_local_data(callback, cx)
    }

    /// Poll the task for the given events
    ///
    /// This function will return [Poll::Ready] if all [Event] have been
    /// triggered, otherwise it will return [Poll::Pending] and register
    /// the task to be woken on event reception
    pub fn poll_event<E: Event>(&mut self, to_wait: E) -> Poll<()> {
        let overlap = self.triggered_events & to_wait.bits();
        self.triggered_events ^= overlap;

        // This can happen if the events are used in the same task simultaneously
        // but the generic parameter is not the same
        let triggered = E::from_bits(overlap).expect("Event type not used correctly");
        let event = to_wait.difference(triggered);

        if event.is_empty() {
            Poll::Ready(())
        } else {
            // If not all events have been received, mark them
            self.awaiting_events |= event.bits();
            Poll::Pending
        }
    }

    /// Poll the task for the given message
    ///
    /// This function will return [Poll::Ready] if a message have been
    /// received, otherwise it will return [Poll::Pending] and register
    /// the task to be woken on message reception
    pub fn poll_message(&mut self) -> Poll<RawMessage> {
        match self.message.take() {
            Some(message) => {
                self.awaiting_message = false;
                Poll::Ready(message)
            },
            None => {
                self.awaiting_message = true;
                Poll::Pending
            },
        }
    }

    /// Trigger the given events
    ///
    /// Returns whether an event that was awaited for has been triggered
    pub fn trigger_events(&mut self, events: u32) -> bool {
        let relevant_events = events & self.awaiting_events;
        self.awaiting_events ^= relevant_events;

        // TODO What happens when the task doesn't pull a triggered event out
        // of the context in time?
        assert!(relevant_events & self.triggered_events == 0, "The context can store only one triggered event");
        self.triggered_events |= relevant_events;

        relevant_events != 0
    }

    /// Trigger message reception
    ///
    /// Returns true if the message was waiting for
    pub fn trigger_message(&mut self, message: RawMessage) -> bool {
        if self.awaiting_message {
            self.message = Some(message);
            true
        } else {
            false
        }
    }

    /// Return the events being waiting by this task
    pub const fn awaiting_events(&self) -> u32 {
        self.awaiting_events
    }

    /// Return true if the task is waiting for a message
    pub const fn awaiting_message(&self) -> bool {
        self.awaiting_message
    }
}

/// Async wait for the given event
///
/// FIXME: This function may return immediately even if the event happened _before_
/// calling this function. This is correlated to the task running this function
/// may be waiting for an event in a different place, e.g. while waiting for an
/// event
pub async fn wait_for_event<E: Event>(event: E) {
    poll_fn(|cx| PxrosData::access(|data| data.poll_event(event), cx)).await
}

/// Wait the task for a message
///
/// # Panics
/// Within the same executor, only one task can wait for a message at the same time. This
/// limitation may be lifted in the future if APIs allows for it
///
/// If this is not respected, the executor *will* panic
pub async fn wait_for_message() -> RawMessage {
    poll_fn(|cx| PxrosData::access(|data| data.poll_message(), cx)).await
}
