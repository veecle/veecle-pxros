//! A hello world example showing off a bit of everything.
#![no_std]
#![feature(type_alias_impl_trait)]
#![feature(macro_metavar_expr)]
#![allow(non_camel_case_types)]
use core::time::Duration;

mod async_tasks;
mod constants;

use constants::peripheral_addresses::{MODULE_P00, MODULE_P34};
use pxros::bindings::*;
use pxros::mem::MemoryRegion;
use pxros::PxResult;
use veecle_pxros::pxros::events::{Receiver, Signaller};
use veecle_pxros::pxros::messages::MailSender;
use veecle_pxros::pxros::name_server::TaskName;
use veecle_pxros::pxros::ticker::{AsyncTicker, Ticker};
use veecle_pxros::pxros_run;

use crate::async_tasks::*;

/// Name of the async executor task
const ASYNC_EXECUTOR: TaskName = TaskName::new(44);
/// Name of the server task
const SERVER_TASK: TaskName = TaskName::new(2);

// Top-level definition of task related events
bitflags::bitflags! {
    /// Events used by task A.
    #[derive(Copy, Clone)]
    pub struct TaskAEvents: u32 {
        const Ticker = 0b0001_0000;
    }

    /// Events used by task B.
    #[derive(Copy, Clone)]
    pub struct TaskBEvents: u32 {
        const Ticker = 0b0001_0000;
    }

    /// The events for Task B
    #[derive(Copy, Clone)]
    pub struct AsyncEvent: u32 {
        const SERVER = 0b0001_0000;
        const Ticker = 0b0010_0000;
    }

    /// The events for the server
    #[derive(Copy, Clone, PartialEq, Eq)]
    pub struct ServerEvents: u32 {
        const Led0 = 0b0001_0000;
        const Led1 = 0b0010_0000;
        const Led2 = 0b0100_0000;
        const Led3 = 0b1000_0000;
        const Ticker = 0b0000_1000;
    }
}

/// This task sends an event to server through Pxros in a periodic manner
#[veecle_pxros::pxros_task(auto_spawn(core = 0, priority = 10))]
fn rust_task_a(_: PxMbx_t) -> PxResult<()> {
    // Fetch the server ID or panic if not found
    let server = SERVER_TASK.query(TaskAEvents::Ticker);

    // Get a signaller for LED0
    let mut signaller = Signaller::new(ServerEvents::Led0, server);

    // Create the ticker for ourselves
    let mut ticker = Ticker::every(TaskAEvents::Ticker, Duration::from_millis(250))?;

    // While the ticker works
    loop {
        ticker.wait();

        if let Err(e) = signaller.signal() {
            defmt::warn!("[TASK A] Toggle LED(0); signal error: {:?}", e)
        } else {
            defmt::trace!("[TASK A] Toggle LED(0); signalled")
        }
    }
}

/// This task sends a message to the server in a periodic manner
#[veecle_pxros::pxros_task(auto_spawn(core = 1, priority = 10))]
fn rust_task_b(_: PxMbx_t) -> PxResult<()> {
    // Fetch the names or panic if not found
    let server = SERVER_TASK.query(TaskBEvents::Ticker);
    let executor = ASYNC_EXECUTOR.query(TaskBEvents::Ticker);

    // Get a mailbox sender for LED1
    let mut sender = MailSender::new(server)?;
    let mut executor_sender = MailSender::new(executor)?;

    // Create the ticker for ourselves
    let mut ticker = Ticker::every(TaskBEvents::Ticker, Duration::from_millis(500))?;

    loop {
        ticker.wait();

        let message = ServerEvents::Led1.bits().to_le_bytes();

        if let Err(e) = sender.send_bytes(message.as_slice()) {
            defmt::warn!("[TASK B] Toggle LED(1); message error: {:?}", e)
        } else {
            defmt::trace!("[TASK B] Toggle LED(1); message sent")
        }

        let _ = executor_sender.send_bytes(message.as_slice());
    }
}

/// An async executor
///
/// This shows how to register a task name automatically via the macro.
#[veecle_pxros::pxros_task(task_name = ASYNC_EXECUTOR, auto_spawn(core = 1, priority = 5))]
fn async_executor(mailbox: PxMbx_t) -> PxResult<()> {
    // Prepare a ticker here to show that parameter passing works
    let ticker = AsyncTicker::every(AsyncEvent::Ticker, Duration::from_millis(1_500))?;

    // Return type
    let result = pxros_run!(
        // Mailbox & associated events
        mailbox,
        AsyncEvent,
        // Type of return
        PxResult<i64>,
        // List of tasks
        async_task_yield_1(),
        async_task_yield_2(),
        async_task_on_event(),
        async_task_on_ticker(ticker),
        async_task_on_message(),
    );

    defmt::warn!("Prototype executor code terminated, the program shall panic after printing results");
    for (index, result) in result.into_iter().enumerate() {
        defmt::warn!("Return for async task [{}]: {:?}", index, result);
    }

    Ok(())
}

const SERVER_REGIONS: [MemoryRegion; 1] = [MemoryRegion::new(MODULE_P00..MODULE_P34, PxProtectType_t::WRProtection)];

/// The server task receives messages and events from tasks A and B, toggling
/// the boards LEDs accordingly.
///
/// This shows how to register a task name manually.
#[veecle_pxros::pxros_task(auto_spawn(core = 0, priority = 8), regions = SERVER_REGIONS, privileges = DirectAccess)]
fn rust_task_server(mailbox: PxMbx_t) -> PxResult<()> {
    use BOARD_LED_t::*;

    // Register the name to the name server. Will panic on error.
    SERVER_TASK.register(PxGetId());

    // Fetch the server ID or panic if not found
    let async_task = ASYNC_EXECUTOR.query(ServerEvents::Ticker);

    // Get a signaller for LED0
    let mut async_signaller = Signaller::new(AsyncEvent::SERVER, async_task);

    // Obtain a receiver for *all* possible events
    let mut receiver = Receiver::new(mailbox, ServerEvents::all());

    loop {
        let (events, message) = receiver.receive();

        if !(events & ServerEvents::Led0).is_empty() {
            defmt::trace!("[TASK SERVER] Received LED (0) event");
            toggle_led(BOARD_LED_0);
        }
        if !(events & ServerEvents::Led1).is_empty() {
            defmt::trace!("[TASK SERVER] Received LED (1) event");
            toggle_led(BOARD_LED_1);
        }

        if let Some(mut message) = message {
            defmt::trace!("[TASK SERVER] Received message {:?}", message);

            // Signal the async task :)
            async_signaller.signal()?;

            // Hacked together for now :) Proper method later on
            let mut bytes = [0_u8; 4];
            bytes.copy_from_slice(&message.data().expect("Message should contain data.")[0..4]);

            let led = ServerEvents::from_bits(u32::from_le_bytes(bytes)).unwrap();
            match led {
                ServerEvents::Led0 => toggle_led(BOARD_LED_0),
                ServerEvents::Led1 => toggle_led(BOARD_LED_1),
                ServerEvents::Led2 => toggle_led(BOARD_LED_2),
                ServerEvents::Led3 => toggle_led(BOARD_LED_3),
                _ => defmt::panic!("Invalid message: {:?}", message),
            }
            if let Err(error) = message.release() {
                defmt::info!("Message could not be released: {}", error);
            }
        }
    }
}

// *************************
// Impl details for the app - ignore
// *************************

extern "C" {
    /// Temporary function to turn on/off the LEDs
    pub fn bsp_board_led_Set(led: BOARD_LED_t, cmd: BOARD_LED_SET_t);
}

fn toggle_led(led: BOARD_LED_t) {
    unsafe { bsp_board_led_Set(led, BOARD_LED_SET_t::BOARD_LED_SET_TOGGLE) };
}

#[repr(C)]
pub enum BOARD_LED_t {
    BOARD_LED_0 = 0,
    BOARD_LED_1,
    BOARD_LED_2,
    BOARD_LED_3,
    BOARD_LED_4,
    BOARD_LED_5,
    BOARD_LED_6,
    BOARD_LED_7,
    BOARD_NB_LEDS,
}

#[repr(C)]
pub enum BOARD_LED_SET_t {
    BOARD_LED_SET_NO_CHANGE = 0,
    BOARD_LED_SET_OFF = 1,
    BOARD_LED_SET_ON = 2,
    BOARD_LED_SET_TOGGLE = 3,
}
