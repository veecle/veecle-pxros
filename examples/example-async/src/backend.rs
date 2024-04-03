//! Hidden backend code for the exercise 3 to validate flags

use core::sync::atomic::AtomicU32;
use core::sync::atomic::Ordering::Relaxed;
use core::time::Duration;

use pxros::bindings::PxMbx_t;
use pxros::PxResult;
use veecle_pxros::pxros::events::{Receiver, Signaller};
use veecle_pxros::pxros::messages::MailSender;
use veecle_pxros::pxros::name_server::{NameServer, TaskName};
use veecle_pxros::pxros::ticker::Ticker;
use veecle_pxros::pxros::time::time_since_boot;

const VALIDATION_TASK_NAME: TaskName = TaskName::new(2);
const MAIN_TASK_NAME: TaskName = TaskName::new(44);

#[no_mangle]
fn ffi_ex1_ticker_1() -> Option<&'static str> {
    static mut TICKER_CORRECT_COUNT: usize = 0;
    static mut LAST_TIME: Duration = Duration::ZERO;

    let current_time = time_since_boot();
    let delta_time = (current_time - unsafe { LAST_TIME }).as_millis();
    unsafe { LAST_TIME = current_time };

    match (150..155).contains(&delta_time) {
        true => unsafe { TICKER_CORRECT_COUNT += 1 },
        false => unsafe { TICKER_CORRECT_COUNT = 0 },
    }

    match unsafe { TICKER_CORRECT_COUNT } >= 5 {
        true => Some("2apUS"),
        false => None,
    }
}

#[no_mangle]
fn ffi_ex1_ticker_2() -> Option<&'static str> {
    static mut TICKER_CORRECT_COUNT: usize = 0;
    static mut LAST_TIME: Duration = Duration::ZERO;

    let current_time = time_since_boot();
    let delta_time = (current_time - unsafe { LAST_TIME }).as_millis();
    unsafe { LAST_TIME = current_time };

    match (220..225).contains(&delta_time) {
        true => unsafe { TICKER_CORRECT_COUNT += 1 },
        false => unsafe { TICKER_CORRECT_COUNT = 0 },
    }

    match unsafe { TICKER_CORRECT_COUNT } >= 5 {
        true => Some("Malv0"),
        false => None,
    }
}

/// Store millis since boot (should never fail unless we sleep for a lot before
/// starting)
static START_MILLIS: AtomicU32 = AtomicU32::new(0);

#[no_mangle]
fn ffi_read_sensor_1() -> u32 {
    let current_time = time_since_boot();

    if START_MILLIS.load(Relaxed) == 0 {
        let millis = u32::try_from(current_time.as_millis()).unwrap();
        START_MILLIS.store(millis, Relaxed);
    }

    let delta_time = current_time - Duration::from_millis(START_MILLIS.load(Relaxed) as u64);
    match delta_time.as_millis() {
        0..50 => 4,
        50..200 => 10,
        200..350 => 3,
        350..500 => 19,
        // Take 19 twice so people can't trick the system by looking at changes in the return value
        500..650 => 19,
        650..800 => 23,
        800..950 => 67,
        950..1100 => 2,
        _ => 15,
    }

    // All correct values added together make 147
}

#[no_mangle]
fn ffi_read_sensor_2() -> Option<u32> {
    let current_time = time_since_boot();

    if START_MILLIS.load(Relaxed) == 0 {
        let millis = u32::try_from(current_time.as_millis()).unwrap();
        START_MILLIS.store(millis, Relaxed);
    }

    let delta_time = current_time - Duration::from_millis(START_MILLIS.load(Relaxed) as u64);
    match delta_time.as_millis() {
        0..50 => Some(16034),
        50..270 => Some(599),
        270..490 => Some(1),
        490..710 => Some(52),
        710..930 => Some(10),
        930..1150 => Some(125),
        1150..1370 => Some(32),
        1370..1590 => Some(3),
        1590..1810 => Some(68),
        1810..2030 => Some(9),
        2030..2250 => Some(234),
        2250..2470 => Some(98),
        2470..2690 => Some(16),
        _ => None,
    }
}

#[no_mangle]
fn ffi_submit_result(sensor_1: u32, sensor_2: u32) {
    match sensor_1 {
        0..147 => defmt::warn!("[Sensor1 task] Value is too small"),
        147 => defmt::info!("[Sensor1 task] Value is correct! The first half of the (3.2) flag is 4KegZ"),
        _ => defmt::warn!("[Sensor1 task] Value is too big"),
    }

    match sensor_2 {
        0..2106 => defmt::warn!("[Sensor2 task] Value is too small"),
        2106 => defmt::info!("[Sensor2 task] Value is correct! The second half of the (3.2) flag is OPeHt"),
        _ => defmt::warn!("[Sensor2 task] Value  is too big"),
    }
}

bitflags::bitflags! {
    #[derive(Copy, Clone)]
    pub struct FlagEvents: u32 {
        const Ticker = 0b0000_0001;
        const StartTransmission = 0b0000_0100;
        const Terminate = 0b0000_1000;
    }
}

#[veecle_pxros::pxros_task(task_name = VALIDATION_TASK_NAME, auto_spawn(core = 0, priority = 15))]
fn flag_message_task(mailbox: PxMbx_t) -> PxResult<()> {
    // Get the user task
    let user_task = NameServer::query(&MAIN_TASK_NAME, FlagEvents::Ticker)?;

    // Get a sender for the flag
    let mut sender = MailSender::new(user_task)?;
    let mut signaller = Signaller::new(FlagEvents::Terminate, user_task);
    let mut start_signal = Receiver::new(mailbox, FlagEvents::StartTransmission);

    loop {
        let (event, _) = start_signal.receive();
        if event.contains(FlagEvents::StartTransmission) {
            break;
        }
    }

    const FLAG: &[u8] = b"PdfADhtQnH";
    let mut ticker = Ticker::every(FlagEvents::Ticker, Duration::from_millis(250))?;

    for &byte in FLAG {
        sender.send_bytes(&[byte])?;
        ticker.wait();
    }

    signaller.signal()?;

    loop {
        ticker.wait();
    }
}
