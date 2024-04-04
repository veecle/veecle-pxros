//! Exercise 2.3

use core::ffi::CStr;

use pxros::bindings::PxMbx_t;
use pxros::PxResult;
use veecle_pxros::pxros::events::Receiver;
use veecle_pxros::pxros::name_server::TaskName;
use veecle_pxros::pxros::task::PxrosTask;

use crate::{FlagEvents, RECEIVE_TASK_NAME};

/// This task must receive a message containing a UTF-8 encoded flag.
///
/// In order to do that, the task must be given `name = 3`, otherwise the server
/// won't be able to reach it and will crash.
///
/// Assigning the name to a task can be done in two ways:
/// 1. By overwriting the [`PxrosTask::task_name`] function: in this case our trait's task entry function will
///    automatically register the name on boot.
///
/// 2. By manually calling the [NameServer::register()] call at boot: this gives more freedom but comes at the cost of
///    usability.
///
/// For receiving messages or events, you may want to look at:
/// * [Receiver]: provides blocking APIs to receive messages and events.
///
/// Any received message [veecle_nos::pxros::messages::Message] is automatically wrapped
/// in a Rust structure that ensures memory lifetime & ownership: no risk of
/// shared access or forgetting to release the payload on drop.
///
/// Finally, some utility to convert bytes to UTF-8: [core::str].
pub(crate) struct Ex2_3Task;
impl PxrosTask for Ex2_3Task {
    fn task_name() -> Option<TaskName> {
        Some(RECEIVE_TASK_NAME)
    }

    fn debug_name() -> &'static CStr {
        CStr::from_bytes_with_nul("Ex2_3_Task\0".as_bytes())
            .expect("The debug name should be a valid, zero-terminated C string.")
    }

    fn task_main(mailbox: PxMbx_t) -> PxResult<()> {
        let (task_debug_name, current_task_id) = Self::log_id();
        // Create a receiver able to receive messages & events
        let mut receiver = Receiver::new(mailbox, FlagEvents::all());

        // Naive loop that process messages as they come
        loop {
            let (_, message) = receiver.receive();

            // We know the flag is a UFT-8 string, so we can safely decode the bytes via Rust APIs
            if let Some(mut message) = message {
                let flag = core::str::from_utf8(message.data().expect("Message should contain data."))
                    .unwrap_or("Oh; Veecle made a mistake with UTF-8");
                defmt::info!("[{}: {}] The (2.3) flag is: {}", task_debug_name, current_task_id, flag);
                if let Err(error) = message.release() {
                    defmt::info!("[{}: {}] Message could not be released: {}", task_debug_name, current_task_id, error);
                }
            }
        }
    }
}
