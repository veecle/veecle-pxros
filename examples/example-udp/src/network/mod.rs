//! Task running a TCP/IP network stack based on [smoltcp].
mod device;
mod net;
pub mod udp;

use device::*;
use pxros::bindings::*;
use pxros::PxResult;

use crate::network::udp::UdpMailbox;
use crate::Service;

bitflags::bitflags! {
    /// Events used by the network stack task.
    #[derive(Copy, Clone)]
    pub struct NetworkEvents: u32 {
        /// A Ethernet packet is ready to be received.
        const EthernetEvent = 0b0000_1000;
        /// Ticker
        const Ticker = 0b0000_0100;
    }
}

/// Handler to signal there is a new Ethernet packet to process.
extern "C" fn mbx_handler(arg1: PxMsg_t, _: PxMsgType_t, arg3: PxArg_t) -> PxMsg_t {
    let task = PxTask_t::from_raw(arg3.0 as u32);
    let evt = NetworkEvents::EthernetEvent.bits();

    unsafe { PxTaskSignalEvents_Hnd(task, PxEvents_t(evt)) };

    arg1
}

/// Network stack task.
///
/// This tasks runs an instance of the network stack. It requires exclusive access to the following global mailboxes:
/// * [PxMbxReq_t::_PxSrv1_ReqMbxId]: this is used to communicate with the Ethernet "C" driver.
/// * [PxMbxReq_t::_PxSrv2_ReqMbxId]: this is used by other tasks to send UDP packets (temporary and ugly solution).
#[veecle_pxros::pxros_task(auto_spawn(core = 1, priority = 10), stack_size = 16_000, interrupt_stack_size = 512)]
fn rust_task_tcp_ip(mailbox: PxMbx_t) -> PxResult<()> {
    // Wait for the Ethernet driver service task on PXCORE_0 to get initialized. Synchronization point.
    let tx_mailbox = Service::Ethernet
        .wait_for_service(PXCORE_0, NetworkEvents::Ticker)
        .expect("Failed to query ethernet service");

    // Attach callback to my mailbox to know when ETH arrives.
    let task = PxGetId().as_raw();
    let err = unsafe { PxMbxInstallHnd(mailbox, Some(mbx_handler), PxMsgType_t::PXMsgNormalMsg, PxArg_t(task as i32)) };
    PxResult::from(err).expect("Failed to install mailbox");

    // Create a new mailbox to receive UDP packets and register it as global mailbox.
    let udp_rx_mailbox = unsafe { PxMbxRequest(PxOpool_t::default()) }
        .checked()
        .expect("Could not create other mailbox");
    let err = unsafe { PxMbxRegisterMbx(PxMbxReq_t::_PxSrv2_ReqMbxId, udp_rx_mailbox) };
    PxResult::from(err).expect("Could not register to global mailbox");

    // Create the network device; this will initialize all the RX buffers.
    let device = PxDevice::init(4, mailbox, tx_mailbox).expect("Failed to initialize pool");
    defmt::info!("[TASK TCP/IP] RX pool initialized");

    // Create the UDP server mailbox.
    let udp_server = UdpMailbox::init(udp_rx_mailbox);

    // Run the network stack forever.
    net::run_network_stack(device, udp_server);
}
