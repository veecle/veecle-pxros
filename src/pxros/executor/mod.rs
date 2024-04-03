//! # Pxros asynchronous executor
//! This module implements an asynchronous executor that implements efficient
//! asynchronous functionality on top of Pxros.
pub mod local_data;
pub mod util;

use pxros::bindings::PxMbx_t;

use super::events::{Event, Receiver};
use crate::executor::{RawExecutor, TaskContext};
use crate::pxros::executor::local_data::PxrosData;

/// Implementation of [RawExecutor] based on Pxros.
///
/// This implementation supports efficiently waiting on events and messages. It
/// will ensure that no unnecessary work is done when no event or message was
/// received but also return in a timely manner when an event or message has
/// occurred.
pub struct PxrosExecutor<E: Event> {
    mailbox: Receiver<E>,
}

impl<E: Event> PxrosExecutor<E> {
    /// Creates a new executor, using the provided mailbox to provide message
    /// functionalities.
    pub fn new(mailbox: PxMbx_t) -> Self {
        Self {
            mailbox: Receiver::new(mailbox, E::all()),
        }
    }
}

impl<E: Event> RawExecutor for PxrosExecutor<E> {
    type TaskLocalData = PxrosData;

    fn wait<C: TaskContext<Self>>(&mut self, tasks: &mut [C], may_block: bool) {
        loop {
            let mut do_not_continue_loop = !may_block;

            // Wait for any possible event that this executor might receive
            //
            // If we were *woken* in the global context, reset the events and
            // do not block
            let (events, message) = if do_not_continue_loop {
                defmt::trace!("Global wake: resetting events");
                let events = Receiver::reset_events(E::all());
                let message = self.mailbox.try_message_receive();
                (events.0, message)
            } else {
                defmt::trace!("Blocking await for events or messages...");
                let (events, message) = self.mailbox.receive();
                (events.bits(), message)
            };
            defmt::trace!(
                "Status: non-blocking: {}, received: {:b}, total: {:b}",
                do_not_continue_loop,
                events,
                E::all().bits()
            );

            for task in tasks.iter_mut() {
                // First, we do a consistency check to ensure a task only waits for subset of
                // supported events; else we panic
                let _ = E::from_bits(task.local_data().awaiting_events())
                    .expect("The task is awaiting on unsupported events");

                // If any event from the task was triggered, mark this task ready to perform
                // work so that the executor will poll it again
                if task.local_data().trigger_events(events) {
                    task.mark_ready();
                    do_not_continue_loop = true;
                }
            }

            if let Some(message) = message {
                let (count, task) = tasks.iter_mut().fold((0, None), |(count, found), task| {
                    if task.local_data().awaiting_message() {
                        (count + 1, Some(task))
                    } else {
                        (count, found)
                    }
                });
                defmt::assert!(count <= 1, "Only one task per time can wait for message");

                // Is it found? If not drop the message
                if let Some(task) = task {
                    task.local_data().trigger_message(message);
                    task.mark_ready();
                    do_not_continue_loop = true;
                } else {
                    defmt::warn!("No tasks waiting for message {:?}, discarding", message);
                }
            }

            if do_not_continue_loop {
                break;
            }
        }
    }

    fn new_context(&self) -> Self::TaskLocalData {
        PxrosData::default()
    }
}
