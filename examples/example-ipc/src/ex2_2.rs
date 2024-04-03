//! Exercise 2.2
use core::time::Duration;

use pxros::bindings::PxMbx_t;
use pxros::PxResult;
use veecle_pxros::pxros::events::Signaller;
use veecle_pxros::pxros::name_server::NameServer;
use veecle_pxros::pxros::ticker::Ticker;

use crate::{FlagEvents, MyEvents};

/// This task must query the runtime ID of the `validation_task` and signal to
/// it to print the flag.
///
/// The `&'static` identifier for the task is [crate::VALIDATION_TASK_NAME] and the
/// event to be signalled is [FlagEvents::PrintSecretFlag].
///
/// For this exercise, you may want to look at:
/// * [NameServer]: integrated task that allows to obtain a task's runtime ID from its static name.
/// * [Signaller]: utility to signal tasks via the kernel signalling system.
#[veecle_pxros::pxros_task(auto_spawn(core = 1, priority = 15))]
fn send_event_task(_: PxMbx_t) -> PxResult<()> {
    // Use the name server to query the ID
    //
    // Due to runtime timing, a single call could fail so we repeat it up to 5 times via this API
    // call (see the code for details)
    let flag_task = NameServer::query(&crate::VALIDATION_TASK_NAME, MyEvents::TickerEx2)?;

    // Obtain an handle to the events for the server, and use those to generate a
    // signaller to be able to send events
    let mut signaller = Signaller::new(FlagEvents::PrintSecretFlag, flag_task);

    // Create a ticker & wait forever to avoid crashes
    let mut ticker = Ticker::every(MyEvents::TickerEx2, Duration::from_millis(500))?;

    // Wait once to allow all tasks to boot (for 2.3).
    ticker.wait();

    // Signal the task to print.
    signaller.signal()?;
    loop {
        ticker.wait();
    }
}
