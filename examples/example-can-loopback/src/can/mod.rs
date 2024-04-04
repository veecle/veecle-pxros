//! Task running the CAN driver.
mod constants;
pub mod mbx;

use core::mem::MaybeUninit;

use bw_r_drivers_tc37x::can::config::NodeInterruptConfig;
use bw_r_drivers_tc37x::can::msg::ReadFrom;
use bw_r_drivers_tc37x::can::*;
use bw_r_drivers_tc37x::cpu::Priority;
use bw_r_drivers_tc37x::gpio::GpioExt;
use bw_r_drivers_tc37x::pac;
use bw_r_drivers_tc37x::pac::can0::{Can0, N as Can0Node};
use constants::peripheral_addresses::{
    MODULE_P00,
    MODULE_P34,
    SCU_END,
    SCU_START,
    SFR_MCMCAN0_END,
    SRAM_MCMCAN0,
    SRC_END,
    SRC_START,
};
use bw_r_drivers_tc37x::embedded_can::ExtendedId;
use pxros::bindings::*;
use pxros::mem::MemoryRegion;
use pxros::PxResult;
use veecle_pxros::pxros::events::Receiver;
use veecle_pxros::pxros::name_server::TaskName;

use crate::can::mbx::CanMailbox;

/// Name of the server task
pub const CAN_DRV_TASK: TaskName = TaskName::new(6);
const CAN_DRV_TASK_PRIO: u32 = 6;

bitflags::bitflags! {
    /// Events used by the CAN task.
    #[derive(Copy, Clone)]
    pub struct CanDrvEvents: u32 {
        /// A CAN packet is ready to be transmitted.
        const TxEvent = 0b0001_0000;
        /// A CAN packet has been received.
        const RxEvent = 0b0000_1000;
        /// Ticker
        const Ticker = 0b0000_0100;
    }
}

const CAN_REGIONS: [MemoryRegion; 4] = [
    MemoryRegion::new(MODULE_P00..MODULE_P34, PxProtectType_t::WRProtection),
    MemoryRegion::new(SRAM_MCMCAN0..SFR_MCMCAN0_END, PxProtectType_t::WRProtection),
    MemoryRegion::new(SCU_START..SCU_END, PxProtectType_t::WRProtection),
    MemoryRegion::new(SRC_START..SRC_END, PxProtectType_t::WRProtection),
];

/// Initialize CAN Module 0 Node 0
fn setup_can0() -> Option<Node<Can0Node, Can0, Node0, Configured>> {
    let can_module = Module::new(Module0);
    let mut can_module = can_module.enable();

    let cfg = NodeConfig {
        baud_rate: BitTimingConfig::Auto(AutoBitTiming {
            baud_rate: 1_000_000,
            sample_point: 8_000,
            sync_jump_width: 3,
        }),
        ..Default::default()
    };

    let node = can_module.take_node(Node0, cfg)?;

    node.setup_tx(&TxConfig {
        mode: TxMode::DedicatedBuffers,
        dedicated_tx_buffers_number: 2,
        fifo_queue_size: 0,
        buffer_data_field_size: DataFieldSize::_8,
        event_fifo_size: 1,
        tx_event_fifo_start_address: 0x400,
        tx_buffers_start_address: 0x440,
    });

    node.setup_pins(None);

    Some(node.lock_configuration())
}

/// Initialize CAN Module 0 Node 1
fn setup_can1() -> Option<Node<Can0Node, Can0, Node1, Configured>> {
    let can_module = Module::new(Module0);
    let mut can_module = can_module.enable();

    let cfg = NodeConfig {
        baud_rate: BitTimingConfig::Auto(AutoBitTiming {
            baud_rate: 1_000_000,
            sample_point: 8_000,
            sync_jump_width: 3,
        }),
        ..Default::default()
    };

    let mut node = can_module.take_node(Node1, cfg)?;

    node.setup_rx(RxConfig {
        mode: RxMode::SharedFifo0,
        buffer_data_field_size: DataFieldSize::_8,
        fifo0_data_field_size: DataFieldSize::_8,
        fifo1_data_field_size: DataFieldSize::_8,
        fifo0_operating_mode: RxFifoMode::Blocking,
        fifo1_operating_mode: RxFifoMode::Blocking,
        fifo0_watermark_level: 0,
        fifo1_watermark_level: 0,
        fifo0_size: 4,
        fifo1_size: 0,
        rx_fifo0_start_address: 0x100,
        rx_fifo1_start_address: 0x200,
        rx_buffers_start_address: 0x300,
    });

    node.setup_pins(None);

    node.setup_interrupt(&NodeInterruptConfig {
        interrupt_group: InterruptGroup::Rxf0n,
        interrupt: Interrupt::RxFifo0newMessage,
        line: InterruptLine::Line0,
        priority: Priority::try_from(6).unwrap(),
        tos: Tos::Cpu0,
    });

    Some(node.lock_configuration())
}

/// Initialize the STB pin for the CAN transceiver.
fn init_can_stb_pin() {
    let gpio20 = pac::P20.split();
    let mut stb = gpio20.p20_6.into_push_pull_output();
    stb.set_low();
}

/// CAN RX interrupt handler
#[no_mangle]
pub extern "C" fn can_rx_handler(arg1: PxArg_t) {
    let task = PxTask_t::from_raw(arg1.0 as u32);
    let evt = CanDrvEvents::RxEvent.bits();

    unsafe { PxTaskSignalEvents_Hnd(task, PxEvents_t(evt)) };
}

/// Naive task that communicates over CAN.
#[veecle_pxros::pxros_task(auto_spawn(core = 0, priority = 8), regions = CAN_REGIONS, privileges = DirectAccess, stack_size = 24_000)]
fn rust_task_can_drv(mailbox: PxMbx_t) -> PxResult<()> {
    // Register the name to the name server. Will panic on error.
    CAN_DRV_TASK.register(PxGetId());

    // Initialize CAN transceiver
    init_can_stb_pin();

    // Create CAN module (needs supervisor privileges)
    let mut can0 = MaybeUninit::<Node<Can0Node, Can0, Node0, Configured>>::uninit();
    let can_ptr: *mut Node<Can0Node, Can0, Node0, Configured> = can0.as_mut_ptr();
    unsafe { _PxHndcallPTR_F(Some(init_can0), can_ptr as *mut c_void) };
    let can0 = unsafe { can0.assume_init() };

    let mut can1 = MaybeUninit::<Node<Can0Node, Can0, Node0, Configured>>::uninit();
    let can_ptr: *mut Node<Can0Node, Can0, Node0, Configured> = can1.as_mut_ptr();
    unsafe { _PxHndcallPTR_F(Some(init_can1), can_ptr as *mut c_void) };
    let can1 = unsafe { can1.assume_init() };

    // Attach callback to interrupt handler to know when CAN packets are received.
    let task = PxGetId().as_raw();
    let err = unsafe { PxIntInstallFastContextHandler(CAN_DRV_TASK_PRIO, PxIntHandler_t(Some(can_rx_handler)), PxArg_t(task as i32)) };
    PxResult::from(err).expect("Could not install fast context handler");

    // Create a new mailbox to expose received CAN packets and register it as global mailbox.
    let rx_mailbox = unsafe { PxMbxRequest(PxOpool_t::default()) }
        .checked()
        .expect("Could not create other mailbox");
    let err = unsafe { PxMbxRegisterMbx(PxMbxReq_t::_PxSrv3_ReqMbxId, rx_mailbox) };
    PxResult::from(err).expect("Could not register to global mailbox");

    // Create the CAN RX mailbox.
    let mut can_rx = CanMailbox::init(rx_mailbox);

    // TODO: hardcoded ID
    let tx_msg_id: MessageId = {
        let id = 0x0CFE6E00;
        let id: ExtendedId = ExtendedId::new(id).unwrap().into();
        id.into()
    };

    // Allocate buffers for message data
    let mut tx_msg_data: [u8; 8] = Default::default();
    let mut rx_msg_data: [u8; 8] = Default::default();

    // Obtain a receiver for *all* possible events
    let mut receiver = Receiver::new(mailbox, CanDrvEvents::all());

    loop {
        let (events, message) = receiver.receive();

        if !(events & CanDrvEvents::RxEvent).is_empty() {
            defmt::trace!("CAN message received");
            can1.receive(ReadFrom::RxFifo0, &mut rx_msg_data);
            can1.clear_interrupt_flag(Interrupt::RxFifo0newMessage);

            // Send received message to global mailbox
            can_rx.send(&rx_msg_data, rx_mailbox)?;
        }

        if let Some(mut message) = message {
            // Copy eight bytes from received private mailbox message
            tx_msg_data.copy_from_slice(&message.data().expect("Message should contain data.")[0..8]);

            // Create and transmit CAN frame
            let tx_frame = Frame::new(tx_msg_id, tx_msg_data.as_slice()).unwrap();
            if can0.transmit(&tx_frame).is_err() {
                defmt::info!("Cannot send frame");
            }

            // Finally, release the message.
            message.release().expect("Failed to release message.");
        }
    }
}

use core::ffi::c_void;

/// Wrapper for calling setup_can0 with supervisor privileges
extern "C" fn init_can0(ptr: *mut c_void) -> i32 {
    let ptr = ptr as *mut Node<Can0Node, Can0, Node0, Configured>;
    let can: &mut Node<Can0Node, Can0, Node0, Configured> = unsafe { &mut *ptr };
    *can = match setup_can0() {
        Some(can) => can,
        None => {
            defmt::info!("Can initialization error");
            loop {}
        },
    };

    return 0;
}

/// Wrapper for calling setup_can0 with supervisor privileges
extern "C" fn init_can1(ptr: *mut c_void) -> i32 {
    let ptr = ptr as *mut Node<Can0Node, Can0, Node1, Configured>;
    let can: &mut Node<Can0Node, Can0, Node1, Configured> = unsafe { &mut *ptr };
    *can = match setup_can1() {
        Some(can) => can,
        None => {
            defmt::info!("Can initialization error");
            loop {}
        },
    };

    return 0;
}
