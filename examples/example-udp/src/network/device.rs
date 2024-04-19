//! Implementation of [phy::Device] based on PXROS message passing.
use pxros::bindings::*;
use pxros::PxResult;
use smoltcp::phy;
use smoltcp::time::Instant;
use veecle_pxros::pxros::messages::RawMessage;

use crate::hardcoded_bindings::{PxNetDrvMsg_t, PxNetMsgType_t, ETH_MTU, PXETH_MAX_FRM_LEN};

/// smoltcp compatible device for PXROS.
#[derive(Debug, Clone, Copy)]
pub struct PxDevice {
    rx_mailbox: PxMbx_t,
    tx_mailbox: PxMbx_t,
}

impl PxDevice {
    /// Initializes receive-messages, which are used by the Ethernet driver task to send received ETH frames to the
    /// Stack task.
    ///
    /// A limited number of messages is used. The messages are kept by the Ethernet driver task in a dedicated RX
    /// mailbox (RX pool). When the Ethernet driver receives network data, it takes a message from the RX mailbox and
    /// fills it with the incoming ETH frame. If no message is available in the RX mailbox, the ETH frame is
    /// dropped.
    ///
    /// The messages are created by the task calling this function and its mailbox is installed as release mailbox. The
    /// messages are sent to the Ethernet driver task, go to the RX pool and when the Ethernet driver task receives an
    /// ETH frame, it fills the message and releases it. The message is then sent to the release mailbox, so the
    /// Stack task can wait on message receive.
    pub fn init(count: usize, rx_mailbox: PxMbx_t, tx_mailbox: PxMbx_t) -> PxResult<Self> {
        let mut mailbox = Self { rx_mailbox, tx_mailbox };

        for _ in 0..count {
            let mut handle = RawMessage::request_no_wait(PXETH_MAX_FRM_LEN, PxMc_t::default(), PxOpool_t::default())?;

            // Install this mailbox as the release mailbox for a message.
            handle.install_release_mailbox(mailbox.rx_mailbox)?;

            // Prepare the message and send it to the driver.
            mailbox.release_message(handle)?;
        }

        Ok(mailbox)
    }

    /// Configures [PxNetDrvMsg_t] metadata in the message.
    fn set_metadata(&mut self, handle: &mut RawMessage, msg_type: PxNetMsgType_t, size: usize) -> PxResult<()> {
        let mut meta = PxNetDrvMsg_t { m: PxMsgMetadata_t(0) };

        meta.s.msg_type = msg_type;
        meta.s.instance = PxUInt8_t(PxUChar_t(0));
        meta.s.frame.buf_len = PxUInt16_t(PxUShort_t(PXETH_MAX_FRM_LEN as u16));
        meta.s.frame.actual_len = PxUInt16_t(PxUShort_t(size as u16));

        handle.set_metadata(unsafe { meta.m })
    }

    /// Releases a message back to the Ethernet pool.
    fn release_message(&mut self, mut handle: RawMessage) -> PxResult<()> {
        // Clear and prepare message to be used again by sending it to the
        // Ethernet driver task (RX pool).
        self.set_metadata(&mut handle, PxNetMsgType_t::PXNET_DRV_RXBUF, 0)?;

        // Release it to the driver; if failure free the memory.
        handle.send(self.tx_mailbox).map_err(|error| {
            defmt::warn!("Error {:?} while sending message: {:?}", error, handle);
            handle.release().expect("Could not release message.");

            error
        })
    }

    /// Tries to receive a [RxToken] from the Ethernet driver.
    ///
    /// This function does not block.
    fn try_receive(&mut self) -> PxResult<Option<RxToken>> {
        match RawMessage::receive_no_wait(self.rx_mailbox) {
            Ok(handle) => Ok(Some(RxToken { device: *self, handle })),
            Err(PxError_t::PXERR_MSG_NOMSG) => Ok(None),
            Err(e) => Err(e),
        }
    }

    /// Requests a message for transmission.
    fn request_tx(&mut self, size: usize) -> PxResult<RawMessage> {
        let mut handle = RawMessage::request(size as u32, PxMc_t::default(), PxOpool_t::default())?;

        // Prepare the metadata for transmission.
        self.set_metadata(&mut handle, PxNetMsgType_t::PXNET_DRV_OUTPKT, size)?;

        // Set to await-release.
        handle.set_await_release()?;

        Ok(handle)
    }

    /// Sends message to Ethernet driver task containing ETH frame to transmit.
    ///
    /// The message waits for message via await-release mechanism to know that the ETH f
    /// frame was sent by the Ethernet driver.
    fn send(&mut self, mut handle: RawMessage) -> PxResult<()> {
        // Send it to the mailbox
        handle.send(self.tx_mailbox).map_err(|error| {
            defmt::warn!("Error {:?} while sending message: {:?}", error, handle);
            handle.release().expect("Could not release message.");

            error
        })?;

        // Wait for until the driver has processed the buffer of TxMsg.
        handle.await_release().map_err(|error| {
            defmt::warn!("Error {:?} while await-release message: {:?}", error, handle);
            handle.release().expect("Could not release message.");

            error
        })?;

        // Get message metadata and validate the output for consistency.
        let meta = PxNetDrvMsg_t { m: handle.metadata()? };
        if unsafe { meta.s.msg_type } != PxNetMsgType_t::PXNET_DRV_TXDONE {
            defmt::panic!("BUG: Message not released correctly.");
        }

        handle.release().expect("Could not release message");

        Ok(())
    }
}

impl phy::Device for PxDevice {
    type RxToken<'a> = RxToken where Self: 'a;
    type TxToken<'a> = TxToken where Self: 'a;

    fn receive(&mut self, _: Instant) -> Option<(Self::RxToken<'_>, Self::TxToken<'_>)> {
        match self.try_receive() {
            Ok(rx_token) => rx_token.map(|rx_token| (rx_token, TxToken { device: *self })),
            Err(e) => {
                defmt::error!("Error: {:?}, while receiving frame.", e);
                None
            },
        }
    }

    fn transmit(&mut self, _: Instant) -> Option<Self::TxToken<'_>> {
        Some(TxToken { device: *self })
    }

    fn capabilities(&self) -> phy::DeviceCapabilities {
        let mut caps = phy::DeviceCapabilities::default();
        caps.medium = phy::Medium::Ethernet;
        caps.max_transmission_unit = ETH_MTU as usize;
        caps
    }
}

/// A reception token to process one Ethernet frame.
pub struct RxToken {
    device: PxDevice,
    handle: RawMessage,
}

impl RxToken {
    /// Inner helper to consume a message into a smolTCP callback.
    fn consume_inner<R, F>(mut self, f: F) -> PxResult<R>
    where
        F: FnOnce(&mut [u8]) -> R,
    {
        // Data ETH_MTU long - metadata describes the actual size.
        let meta = PxNetDrvMsg_t {
            m: self.handle.metadata()?,
        };

        // Safety: access to union is unsafe in Rust - let's hope bindgen is correct.
        let size = unsafe { meta.s.frame.actual_len };

        // Extract the inner u16 from pxros wrapper type.
        let size = size.0.0 as usize;
        assert!(size <= PXETH_MAX_FRM_LEN as usize);

        // Actual payload length
        let payload = &mut self.handle.data_mut()?[0..size];
        defmt::debug!("RxToken: {:?}", payload);

        // Process the payload through the smolTCP stack.
        let ret = f(payload);

        // Finally, release the message back to the driver.
        let _ = self.device.release_message(self.handle);
        Ok(ret)
    }
}

impl phy::RxToken for RxToken {
    fn consume<R, F>(self, f: F) -> R
    where
        F: FnOnce(&mut [u8]) -> R,
    {
        self.consume_inner(f).expect("Failed to process RxToken.")
    }
}

/// A transmission token to transmit a single Ethernet frame.
pub struct TxToken {
    device: PxDevice,
}

impl phy::TxToken for TxToken {
    fn consume<R, F>(mut self, len: usize, f: F) -> R
    where
        F: FnOnce(&mut [u8]) -> R,
    {
        let mut handle = self.device.request_tx(len).expect("Failed to allocate message.");

        // Obtain the payload and apply the callback.
        let payload: &mut [u8] = handle.data_mut().expect("Could not get handle to message data.");
        let ret = f(payload);

        defmt::debug!("TxToken: {:?}", payload);

        // Finally, send the message and panic on error for now.
        self.device.send(handle).expect("Failed to send message.");

        ret
    }
}
