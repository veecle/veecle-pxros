//! Some example async functions used by the example.
use core::task::Poll;

use futures::StreamExt;
use pxros::bindings::PxError_t;
use pxros::PxResult;
use veecle_pxros::pxros::events::{AsyncEventReceiver, Event};
use veecle_pxros::pxros::messages::AsyncMessageReceiver;
use veecle_pxros::pxros::ticker::AsyncTicker;

use crate::AsyncEvent;

pub async fn async_task_yield_1() -> PxResult<i64> {
    for i in 0..10 {
        defmt::info!("Async task A -> {:?}", i);
        yield_once().await;
    }
    Ok(150)
}

pub async fn async_task_yield_2() -> PxResult<i64> {
    for i in 0..10 {
        defmt::info!("Async task C -> {:?}", i);
        yield_once().await;
    }

    Ok(300)
}

pub async fn async_task_on_event() -> PxResult<i64> {
    // Example of event stream
    let mut recv = AsyncEventReceiver::new(AsyncEvent::SERVER).enumerate();

    while let Some((i, _)) = recv.next().await {
        defmt::info!("Async task B waiting for server event to happen (iteration {:?})", i);
    }

    Err(PxError_t::PXERR_EVENT_ZERO)
}

pub async fn async_task_on_ticker<E: Event + Unpin>(ticker: AsyncTicker<E>) -> PxResult<i64> {
    // Example of ticker stream
    let mut ticker = ticker.enumerate().take(5);

    while let Some((i, _)) = ticker.next().await {
        defmt::info!("Async ticker [iteration: {}]", i);
    }

    Err(PxError_t::PXERR_EVENT_ZERO)
}

pub async fn async_task_on_message() -> PxResult<i64> {
    // Example of message stream
    let mut recv = AsyncMessageReceiver::new();

    while let Some(mut message) = recv.next().await {
        defmt::info!("Async message {:?}", message);
        if let Err(error) = message.release() {
            defmt::info!("Message could not be released: {}", error);
        }
    }

    Err(PxError_t::PXERR_EVENT_ZERO)
}

/// Yeld execution of the current task
pub async fn yield_once() {
    let mut did_yield = false;

    core::future::poll_fn(|cx| {
        if did_yield {
            Poll::Ready(())
        } else {
            did_yield = true;
            cx.waker().wake_by_ref();
            Poll::Pending
        }
    })
    .await;
}
