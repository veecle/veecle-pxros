//! Hidden backend code for the exercise 2 to validate flags

use core::ffi::CStr;
use core::time::Duration;

use pxros::bindings::PxMbx_t;
use pxros::PxResult;
use veecle_pxros::pxros::events::Receiver;
use veecle_pxros::pxros::messages::MailSender;
use veecle_pxros::pxros::name_server::{NameServer, TaskName};
use veecle_pxros::pxros::task::{log_id, PxrosTask};
use veecle_pxros::pxros::time::time_since_boot;

const VALIDATION_TASK_NAME: TaskName = TaskName::new(2);
const RECEIVE_TASK_NAME: TaskName = TaskName::new(3);
const SALT_TASK_NAME: TaskName = TaskName::new(5);

bitflags::bitflags! {
    #[derive(Copy, Clone)]
    pub struct FlagEvents: u32 {
        const PrintSecretFlag   = 0b0000_0001;
        const ReceiveSecretFlag = 0b0000_0010;
    }
}

#[no_mangle]
fn ffi_ticker() -> Option<&'static str> {
    static mut TICKER_CORRECT_COUNT: usize = 0;
    static mut LAST_TIME: Duration = Duration::ZERO;

    let current_time = time_since_boot();
    let delta_time = (current_time - unsafe { LAST_TIME }).as_millis();
    unsafe { LAST_TIME = current_time };

    match (145..155).contains(&delta_time) {
        true => unsafe { TICKER_CORRECT_COUNT += 1 },
        false => unsafe { TICKER_CORRECT_COUNT = 0 },
    }

    match unsafe { TICKER_CORRECT_COUNT } >= 5 {
        true => Some("qkYFy3RaOK"),
        false => None,
    }
}

pub(crate) struct ValidationTask;
impl PxrosTask for ValidationTask {
    fn task_name() -> Option<TaskName> {
        Some(VALIDATION_TASK_NAME)
    }

    fn debug_name() -> &'static CStr {
        CStr::from_bytes_with_nul("ValidationTask\0".as_bytes())
            .expect("The debug name should be a valid, zero-terminated C string.")
    }

    fn task_main(mailbox: PxMbx_t) -> PxResult<()> {
        let (task_debug_name, current_task_id) = log_id::<Self>();        defmt::info!("[{}: {}] Started and waiting for signals", task_debug_name, current_task_id);

        // We receive anything :)
        let mut receiver = Receiver::new(mailbox, FlagEvents::all());

        const PRINT_FLAG: &[u8] = b"PdfADhtQnH";
        const RECEIVE_FLAG: &[u8] = b"5h4eZ6wOZH";
        const SALT_FLAG: &[u8] = b"ItXVjcxhbU";

        loop {
            let (events, message) = receiver.receive();

            if !(events & FlagEvents::PrintSecretFlag).is_empty() {
                let flag = core::str::from_utf8(PRINT_FLAG).unwrap();
                defmt::info!("[{}: {}] Congratulations! The (2.2) flag is: {}", task_debug_name, current_task_id, flag);

                // See below; again this may fail until we user reaches the correct
                // exercise; to TEST
                if let Ok(receive_task) = NameServer::try_query(&RECEIVE_TASK_NAME) {
                    let mut receive_sender = MailSender::new(receive_task)?;
                    receive_sender.send_bytes(RECEIVE_FLAG)?;
                }
            }

            // This is the last part of the exercise; so the query mail fail until the user
            // reaches that point
            if let Ok(salt_task) = NameServer::try_query(&SALT_TASK_NAME) {
                // # TODO
                // This may crash? To validate what happens if create more than once
                let mut salt_sender = MailSender::new(salt_task)?;

                if let Some(mut message) = message {
                    let mut bytes = [0_u8; 10];
                    bytes.copy_from_slice(message.data().expect(" Message should contain data."));

                    if bytes == RECEIVE_FLAG {
                        salt_sender.send_bytes(SALT_FLAG)?;
                    } else if let Err(error) = message.release() {
                        defmt::info!(
                            "[{}: {}] Message could not be released: {}",
                            task_debug_name,
                            current_task_id,
                            error
                        );
                    }
                }
            }
        }
    }
}
