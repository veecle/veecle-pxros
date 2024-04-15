//! Embedded world demo code - see README.md for details.
//!
//! Driver task <--> smoltcp task <--> ping task
#![no_std]
use core::time::Duration;

use pxros::bindings::*;
use pxros::PxResult;
use veecle_pxros::pxros::events::Event;
use veecle_pxros::pxros::ticker::Ticker;

mod app;
mod config;
mod hardcoded_bindings;
mod network;

/// Small semantic wrapper for declaring global PXROS services.
#[derive(Debug, Clone, Copy, PartialEq, Eq, defmt::Format)]
pub enum Service {
    /// The Ethernet driver service.
    Ethernet,
    /// The Network service.
    Network,
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
        };
        let core = PxCoreId_t(PxUChar_t(core as u8));

        // Up to one second.
        for _ in 0..10 {
            let err = unsafe { PxGetGlobalServerMbx(core, service) };
            if err == PxError_t::PXERR_NOERROR {
                return Ok(unsafe { PxMbxRequestMbx(service) });
            }

            ticker.wait();
        }

        Err(PxError_t::PXERR_REQUEST_FAILED)
    }
}
