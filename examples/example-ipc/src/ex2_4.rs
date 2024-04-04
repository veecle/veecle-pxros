//! Exercise 2.4
use core::ffi::CStr;
use core::time::Duration;

use pxros::bindings::PxMbx_t;
use pxros::PxResult;
use veecle_pxros::pxros::events::Receiver;
use veecle_pxros::pxros::messages::MailSender;
use veecle_pxros::pxros::name_server::{NameServer, TaskName};
use veecle_pxros::pxros::task::PxrosTask;
use veecle_pxros::pxros::ticker::Ticker;

use crate::{FlagEvents, MyEvents, SALT_TASK_NAME};

/// This task must perform a message challenge-response with the VALIDATION_TASK
/// in order to receive the correct flag.
///
/// The task must be given `name = SALT_TASK_NAME` otherwise the server won't be able
/// to reach it.
///
/// This task has to use a [MailSender] to transmit the flag received in (2.3) to
/// the VALIDATION TASK.
///
/// In case the transmitted flag is correct, the TASK will reply with a message
/// containing an UTF-8 encoded flag
///
/// For that one has to use (again) a [Receiver].
pub(crate) struct Ex2_4Task;
impl PxrosTask for Ex2_4Task {
    fn task_name() -> Option<TaskName> {
        Some(SALT_TASK_NAME)
    }

    fn debug_name() -> &'static CStr {
        CStr::from_bytes_with_nul("Ex2_4_Task\0".as_bytes())
            .expect("The debug name should be a valid, zero-terminated C string.")
    }

    fn task_main(mailbox: PxMbx_t) -> PxResult<()> {
        let (task_debug_name, current_task_id) = Self::log_id();

        let flag_task = NameServer::query(&crate::VALIDATION_TASK_NAME, MyEvents::TickerEx4).unwrap();

        // Create a sender and register
        let mut receiver = Receiver::new(mailbox, FlagEvents::all());
        let mut sender = MailSender::new(flag_task).unwrap();

        // Send the previous flag (byte-encoded) to the flag
        //
        // # Note
        // Here one could use Serde or similar framework to properly encode the bytes
        // into typed structures
        let previous_flag = b"5h4eZ6wOZH";
        sender.send_bytes(previous_flag).expect("Could not send the message");

        // Wait for a message reply
        let (_, message) = receiver.receive();

        if let Some(mut message) = message {
            let flag = core::str::from_utf8(message.data().expect("Message should contain data."))
                .unwrap_or("Oh; Veecle made a mistake with UTF-8");
            defmt::info!("[{}: {}] The (2.4) flag is: {}", task_debug_name, current_task_id, flag);
            if let Err(error) = message.release() {
                defmt::info!("[{}: {}] Message could not be released: {}", task_debug_name, current_task_id, error);
            }
        }

        // Busy wait for it to print the flag
        let mut ticker = Ticker::every(MyEvents::TickerEx4, Duration::from_millis(500)).unwrap();
        loop {
            ticker.wait();
        }
    }
}
