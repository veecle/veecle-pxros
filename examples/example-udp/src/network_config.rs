//! Network configuration.

use smoltcp::wire::{EthernetAddress, IpAddress};

/// MAC Address of the device.
pub const MAC_ADDRESS: EthernetAddress = EthernetAddress([0x00, 0x11, 0x22, 0x33, 0x44, 0x55]);

/// IP Address of the device.
pub const IP_ADDRESS: IpAddress = IpAddress::v4(192, 168, 2, 10);

/// Port of the device.
pub const PORT: u16 = 5000;
