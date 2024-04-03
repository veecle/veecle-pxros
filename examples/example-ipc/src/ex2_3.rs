//! Exercise 2.3
use pxros::bindings::PxMbx_t;
use pxros::PxResult;
use veecle_pxros::pxros::events::Receiver;

use crate::{FlagEvents, RECEIVE_TASK_NAME};

/// This task must receive a message containing a UTF-8 encoded flag.
///
/// In order to do that, the task must be given `name = 3`, otherwise the server
/// won't be able to reach it and will crash.
///
/// Assigning the name to a task can be done in two ways:
/// 1. By using the procedural-attribute `task_name = RECEIVE_TASK_NAME`: in this case our proc-macro will automatically
///    register the name on boot.
///
/// 2. By manually calling the [NameServer::register()] call at boot: this gives more freedom but comes at the cost of
///    usability.
///
/// For receiving messages or events, you may want to look at:
/// * [Receiver]: provides blocking APIs to receive messages and events.
///
/// Any received message [veecle_pxros::pxros::messages::Message] is automatically wrapped
/// in a Rust structure that ensures memory lifetime & ownership: no risk of
/// shared access or forgetting to release the payload on drop.
///
/// Finally, some utility to convert bytes to UTF-8: [core::str].
#[veecle_pxros::pxros_task(task_name = RECEIVE_TASK_NAME, auto_spawn(core = 2, priority = 15))]
fn receive_event_task(mailbox: PxMbx_t) -> PxResult<()> {
    // Create a receiver able to receive messages & events
    let mut receiver = Receiver::new(mailbox, FlagEvents::all());

    // Naive loop that process messages as they come
    loop {
        let (_, message) = receiver.receive();

        // We know the flag is a UFT-8 string, so we can safely decode the bytes via Rust APIs
        if let Some(mut message) = message {
            let flag = core::str::from_utf8(message.data().expect("Message should contain data."))
                .unwrap_or("Oh; Veecle made a mistake with UTF-8");
            defmt::info!("The (2.3) flag is {}", flag);
            if let Err(error) = message.release() {
                defmt::info!("Message could not be released: {}", error);
            }
        }
    }
}
