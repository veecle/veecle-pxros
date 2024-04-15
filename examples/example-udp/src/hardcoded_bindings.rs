use pxros::bindings::{PxMsgMetadata_t, PxUInt16_t, PxUInt8_t};

// Hardcoded from a bindgen invocation for an easier setup
pub const PXETH_MAX_FRM_LEN: u32 = 1522;
pub const ETH_MTU: u32 = 1500;

#[allow(non_camel_case_types)]
#[repr(C)]
#[derive(Copy, Clone)]
pub union PxNetDrvMsg_t {
    pub s: PxNetDrvMsg_t__bindgen_ty_1,
    pub m: PxMsgMetadata_t,
}
#[allow(dead_code)]
#[allow(non_camel_case_types)]
#[repr(u8)]
#[derive(Debug, Copy, Clone, Hash, PartialEq, Eq)]
pub enum PxNetMsgType_t {
    PXNET_DRV_INPKT = 0,
    PXNET_DRV_OUTPKT = 1,
    PXNET_DRV_RXBUF = 2,
    PXNET_DRV_TXDONE = 3,
}

#[allow(non_camel_case_types)]
#[repr(C, packed)]
#[derive(Debug, Copy, Clone)]
pub struct PxNetDrvMsg_t__bindgen_ty_1 {
    pub msg_type: PxNetMsgType_t,
    pub instance: PxUInt8_t,
    pub frame: PxNetDrvMsg_t__bindgen_ty_1__bindgen_ty_1,
}
#[allow(non_camel_case_types)]
#[repr(C, packed)]
#[derive(Debug, Copy, Clone)]
pub struct PxNetDrvMsg_t__bindgen_ty_1__bindgen_ty_1 {
    pub buf_len: PxUInt16_t,
    pub actual_len: PxUInt16_t,
}
