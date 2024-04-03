//! Abstraction over Pxros ticker API.

use core::pin::Pin;
use core::task::{Context, Poll};
use core::time::Duration;

use futures::{pin_mut, FutureExt, Stream, StreamExt};
use pxros::bindings::{PxEvents_t, PxOpool_t, PxPeRelease, PxPeRequest, PxPeStart, PxPe_t};
use pxros::PxResult;

use super::events::{Event, Receiver};
use crate::pxros::executor::local_data::wait_for_event;
use crate::pxros::time::duration_to_ticks;

/// A ticker that "ticks" at a frequency.
///
/// A ticker works by scheduling a kernel [Event] to be triggered
/// every [Duration]. It can also be used to run simple one-shot
/// delay jobs through [Ticker::after].
///
/// For the async variant refer to [AsyncTicker].
///
/// ## Note
/// Memory pools are current not supported (the default task pool is
/// used).
pub struct Ticker<E: Event> {
    event: E,
    handle: PxPe_t,
}

impl<E: Event> Ticker<E> {
    /// Starts a ticker that triggers an event at a given frequency.
    ///
    /// The event can be waited upon via [Ticker::wait].
    ///
    /// This may return error if [PxPeRequest] or [PxPeStart] fails.
    pub fn every(event: E, frequency: Duration) -> PxResult<Self> {
        let pool = PxOpool_t::default();
        let ticks = duration_to_ticks(frequency);

        // Safety: safe to call from any context, errors are checked.
        let handle = unsafe { PxPeRequest(pool, ticks, PxEvents_t(event.bits())) }.checked()?;

        // By creating the object here we ensure that we drop in case of failure.
        let ticker = Ticker { event, handle };

        // Safety: PxPe_t known to be a valid object, in case of failure release
        // happens on drop.
        let result = unsafe { PxPeStart(ticker.handle) };
        PxResult::from(result).map(|_| ticker)
    }

    /// Waits for a one-shot delay job to complete.
    ///
    /// This may return error if [PxPeRequest] or [PxPeStart] fails.
    pub fn after(event: E, duration: Duration) -> PxResult<()> {
        Self::every(event, duration).map(|mut ticker| {
            ticker.wait();
        })
    }

    /// Waits for the next tick event in blocking manner.
    pub fn wait(&mut self) {
        let evt = Receiver::await_events(self.event);

        if evt.0 != self.event.bits() {
            defmt::panic!("Received unexpected event {}, expected {}", evt.0, self.event.bits());
        }
    }

    /// Returns the event owned by the ticker
    pub const fn event(&self) -> E {
        self.event
    }
}

impl<E: Event> Drop for Ticker<E> {
    fn drop(&mut self) {
        // Safety: we cannot handle failures yet, so we panic
        // in case we fail at releasing the object.
        let _ = unsafe { PxPeRelease(self.handle) }
            .checked()
            .expect("Failed at releasing PxPe_t");
    }
}

/// Async variant of [Ticker].
///
/// This ticker implements [Stream] for async usage.
pub struct AsyncTicker<E: Event> {
    ticker: Ticker<E>,
}

impl<E: Event + Unpin> AsyncTicker<E> {
    /// Start a new ticker that triggers the event at a frequency.
    ///
    /// See [Ticker::every] for details.
    pub fn every(event: E, frequency: Duration) -> PxResult<Self> {
        let ticker = Ticker::every(event, frequency)?;
        Ok(Self { ticker })
    }

    /// Asynchronously wait for a one-shot delay job to complete.
    ///
    /// See [Ticker::after] for details.
    pub async fn after(event: E, duration: Duration) -> PxResult<()> {
        let ticker = AsyncTicker::every(event, duration)?;
        pin_mut!(ticker);

        StreamExt::next(&mut ticker).await;
        Ok(())
    }
}

impl<E: Event + Unpin> Stream for AsyncTicker<E> {
    type Item = ();

    fn poll_next(self: Pin<&mut Self>, context: &mut Context<'_>) -> Poll<Option<Self::Item>> {
        let future = wait_for_event(self.ticker.event());
        pin_mut!(future);

        future.poll_unpin(context).map(Some)
    }
}
