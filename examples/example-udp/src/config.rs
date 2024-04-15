use smoltcp::wire::{EthernetAddress, IpAddress};

/// MAC Address of the device (hardcoded as we cannot query it)
pub const ETH_ADDRESS: EthernetAddress = EthernetAddress([0x00, 0x11, 0x22, 0x33, 0x44, 0x55]);

/// Maximum supported UDP MTU
pub const UDP_MTU: usize = 256;

/// My IP.
pub const MY_IP: IpAddress = IpAddress::v4(192, 168, 2, 10);
/// My Port.
pub const MY_PORT: u16 = 5000;
