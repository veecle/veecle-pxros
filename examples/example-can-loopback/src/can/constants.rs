#![allow(dead_code)]

pub mod peripheral_addresses {
    //! Manually created addresses of peripheral block start addresses
    //!
    //! All constants starting with `MODULE` are GPIO control blocks
    pub const MODULE_P00: u32 = 0xF003A000;
    pub const MODULE_P01: u32 = 0xF003A100;
    pub const MODULE_P02: u32 = 0xF003A200;
    pub const MODULE_P10: u32 = 0xF003AA00;
    pub const MODULE_P11: u32 = 0xF003AB00;
    pub const MODULE_P12: u32 = 0xF003AC00;
    pub const MODULE_P13: u32 = 0xF003AD00;
    pub const MODULE_P14: u32 = 0xF003AE00;
    pub const MODULE_P15: u32 = 0xF003AF00;
    pub const MODULE_P20: u32 = 0xF003B400;
    pub const MODULE_P21: u32 = 0xF003B500;
    pub const MODULE_P22: u32 = 0xF003B600;
    pub const MODULE_P23: u32 = 0xF003B700;
    pub const MODULE_P32: u32 = 0xF003C000;
    pub const MODULE_P33: u32 = 0xF003C100;
    pub const MODULE_P34: u32 = 0xF003C200;
    pub const MODULE_P40: u32 = 0xF003C800;
    pub const SRAM_MCMCAN0: u32 = 0xF0200000;
    pub const SFR_MCMCAN0: u32 = 0xF0208000;
    pub const SFR_MCMCAN0_END: u32 = 0xF0209000;
    pub const SCU_START: u32 = 0xF0036000;
    pub const SCU_END: u32 = 0xF0036400;
    pub const SRC_START: u32 = 0xF0038000;
    pub const SRC_END: u32 = 0xF003A000;
}
