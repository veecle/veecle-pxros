//! Solution to show for exercise 3.3
use futures::future::{select, Either};
use futures::pin_mut;
use pxros::bindings::*;
use pxros::PxResult;
use veecle_pxros::pxros::events::{AsyncEventReceiver, Signaller};
use veecle_pxros::pxros::messages::AsyncMessageReceiver;
use veecle_pxros::pxros::name_server::NameServer;
use veecle_pxros::pxros_run;

use crate::{FlagEvents, TickerEvents};

/// This async function shall `.await` on either the termination signal or a message
/// flag nibble; each nibble shall be appended to a heapless vector.
///
/// The moment the [FlagEvents::Terminate] signal is received, the
/// task shall print the flag and return.
///
/// * This must allocate up to 16 bytes via [heapless::Vec] to store the flag
/// * This must use [futures::future::select] to process two futures concurrently.
async fn message_handler(
    mut flagger: AsyncMessageReceiver,
    mut termination: AsyncEventReceiver<FlagEvents>,
) -> PxResult<()> {
    let mut flag = heapless::Vec::<u8, 16>::new();

    loop {
        // Create the futures & pin them
        let term_fut = termination.wait();
        let flag_fut = flagger.wait();
        pin_mut!(term_fut);
        pin_mut!(flag_fut);

        // We use select to perform a non-blocking choice
        match select(term_fut, flag_fut).await {
            Either::Left(_) => {
                let flag = core::str::from_utf8(flag.as_slice()).unwrap_or_default();
                defmt::info!("The flag for (3.3) is {:?}", flag);
                return Ok(());
            },
            Either::Right((mut message, _)) => {
                flag.extend_from_slice(message.data().expect("Message should contain data."))
                    .unwrap(); // Flag won't be above 16 bytes
                if let Err(error) = message.release() {
                    defmt::info!("Message could not be released: {}", error);
                }
            },
        }
    }
}

/// This function shall use the [pxros_run] macro to spawn an async task via [message_handler].
///
/// The goal is to process two futures at once via `select` to demonstrate how concurrency can be achieved
/// in multi-async setup.
pub fn ex3_3_solution(mailbox: PxMbx_t) {
    let flagger = AsyncMessageReceiver::new();
    let termination = AsyncEventReceiver::new(FlagEvents::Terminate);

    // We inform the task to start transmitting the flag
    let flag_task = NameServer::query(&crate::FLAG_TASK_NAME, TickerEvents::Ticker1).unwrap();
    Signaller::new(FlagEvents::StartTransmission, flag_task)
        .signal()
        .unwrap();

    pxros_run!(mailbox, FlagEvents, PxResult<()>, message_handler(flagger, termination));
}
