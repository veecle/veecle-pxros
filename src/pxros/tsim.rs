//! Support for running binaries in the HighTec TSIM simulator.
//!
//! This provides access to `libos` which is implemented by HighTec.

/// Write data to the given file descriptor.
///
/// TODO: Document what the significance of the file descriptor actually is. Does
/// this map one to one to the hosts file descriptors?
pub fn write(file_descriptor: u32, data: &[u8]) -> u32 {
    extern "C" {
        fn write(fd: u32, buf: *const u8, len: u32) -> u32;
    }

    let buffer_pointer = data.as_ptr();
    let length = data.len() as u32;

    unsafe { write(file_descriptor, buffer_pointer, length) }
}

/// Checks whether the code is currently running in the simulator.
pub fn is_running_on_tsim() -> bool {
    extern "C" {
        fn run_on_tsim() -> u32;
    }

    unsafe { run_on_tsim() != 0 }
}
