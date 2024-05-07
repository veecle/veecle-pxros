//! Task running a TCP/IP network stack based on [smoltcp].
mod device;
mod net;
pub mod udp;

use core::ffi::CStr;

use device::*;
use pxros::bindings::*;
use pxros::PxResult;
use veecle_pxros::pxros::task::{PxAccess, PxrosTask};

use crate::network::udp::UdpMailbox;
use crate::service::Service;

bitflags::bitflags! {
    /// Events used by the network stack task.
    #[derive(Copy, Clone)]
    pub struct NetworkEvents: u32 {
        /// An Ethernet packet is ready to be received.
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
/// This task runs an instance of the network stack. It requires exclusive access to the following global mailboxes:
/// * [PxMbxReq_t::_PxSrv1_ReqMbxId]: this is used to communicate with the Ethernet "C" driver.
/// * [PxMbxReq_t::_PxSrv2_ReqMbxId]: this is used by other tasks to send UDP packets (temporary and ugly solution).
pub(crate) struct NetworkStackTask;

impl PxrosTask for NetworkStackTask {
    fn task_main(mailbox: PxMbx_t) -> PxResult<()> {
        // Wait for the Ethernet driver service task on PXCORE_0 to get initialized. Synchronization point.
        let tx_mailbox = Service::Ethernet
            .wait_for_service(PXCORE_0, NetworkEvents::Ticker)
            .expect("Failed to query Ethernet service");

        // Attach callback to this tasks mailbox to know when ETH arrives.
        let task = PxGetId().as_raw();
        let err =
            unsafe { PxMbxInstallHnd(mailbox, Some(mbx_handler), PxMsgType_t::PXMsgNormalMsg, PxArg_t(task as i32)) };
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

    fn debug_name() -> &'static CStr {
        CStr::from_bytes_with_nul("TCP_IP_Task\0".as_bytes())
            .expect("The debug name should be a valid, zero-terminated C string.")
    }

    fn priority() -> PxPrio_t {
        PxPrio_t(10)
    }

    fn core() -> u32 {
        1
    }

    fn access_rights() -> PxAccess {
        PxAccess::INSTALL_HANDLERS
    }

    fn task_stack_size() -> u32 {
        16_000
    }

    fn interrupt_stack_size() -> u32 {
        512
    }
}
