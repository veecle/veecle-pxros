//! Embedded world demo code - see README.md for details
#![no_std]
mod can;

use core::time::Duration;

use pxros::bindings::*;
use pxros::PxResult;
use veecle_pxros::pxros::events::Event;
use veecle_pxros::pxros::messages::MailSender;
use veecle_pxros::pxros::ticker::Ticker;

use crate::can::mbx::CanMailbox;
use crate::can::{CanDrvEvents, CAN_DRV_TASK};

bitflags::bitflags! {
    /// Events for the CAN.
    #[derive(Copy, Clone)]
    pub struct CanEvents: u32 {
        const Ticker = 0b0001_0000;
    }
}

/// Entry point
#[veecle_pxros::pxros_task(auto_spawn(core = 0, priority = 12))]
fn rust_task_can(_mailbox: PxMbx_t) -> PxResult<()> {

    let can_drv_task = CAN_DRV_TASK.query(CanDrvEvents::TxEvent);
    
    // Get a mailbox sender for CAN Tx
    let mut sender = MailSender::new(can_drv_task)?;

    // Wait for the CAN service to initialize.
    let rx_mailbox = Service::Can
        .wait_for_service(PXCORE_0, CanEvents::Ticker)
        .expect("The CAN service should be queryable within ten tries.");

    let _ticker = Ticker::every(CanEvents::Ticker, Duration::from_millis(1000)).unwrap();

    // This transmits over private mailbox and receives over service mailbox.
    let mut can = CanMailbox::init(rx_mailbox);

    let mut tx_msg_data: [u8; 8] = [0, 1, 2, 3, 4, 5, 6, 7];

    loop {
        match can.receive_or_event(CanEvents::Ticker) {
            Ok(None) => {
                // Transmit a different message each time (changing the first byte)
                tx_msg_data[0] = tx_msg_data[0].wrapping_add(1);

                // Ticker event. Trigger the sending of CAN packet.
                if let Err(e) = sender.send_bytes(tx_msg_data.as_slice()) {
                    defmt::warn!("Message error: {:?}", e)
                } else {
                    defmt::trace!("Message sent")
                }
            },
            Ok(Some(mut message)) => {
                // Message received. Deserialize payload and work with it.
                defmt::info!("Received CAN message: {:?}", message);

                // Copy received message to output
                tx_msg_data.copy_from_slice(&message.data().expect("Message should contain data.")[0..8]);

                message.release()?;
            },
            Err(error) => {
                // Just print the error.
                defmt::info!("Error while receiving CAN message: {:?}", error);
            },
        }
    }
}

/// Small semantic wrapper for declaring global PXROS services.
#[derive(Debug, Clone, Copy, PartialEq, Eq, defmt::Format)]
pub enum Service {
    /// The Ethernet driver service.
    Ethernet,
    /// The Network service.
    Network,
    /// The CAN serive.
    Can,
}

impl Service {
    /// Queries for a service up to ten time until available; returns error otherwise.
    ///
    /// This requires a *ticker* event for the duration of the query to support
    /// delays: this is needed to allow for other work on the same core to complete.
    pub fn wait_for_service<E: Event>(&self, core: u32, event: E) -> PxResult<PxMbx_t> {
        let timeout = Duration::from_millis(100);
        let mut ticker = Ticker::every(event, timeout)?;

        let service = match self {
            Service::Ethernet => PxMbxReq_t::_PxSrv1_ReqMbxId,
            Service::Network => PxMbxReq_t::_PxSrv2_ReqMbxId,
            Service::Can => PxMbxReq_t::_PxSrv3_ReqMbxId,
        };
        let core = PxCoreId_t(PxUChar_t(core as u8));

        // Up to one second.
        for _ in 0..10 {
            let err = unsafe { PxGetGlobalServerMbx(core, service) };
            match err {
                PxError_t::PXERR_REQUEST_FAILED => break,
                PxError_t::PXERR_NOERROR => {
                    return Ok(unsafe { PxMbxRequestMbx(service) });
                },
                _ => {},
            }

            ticker.wait();
        }

        Err(PxError_t::PXERR_REQUEST_FAILED)
    }
}
