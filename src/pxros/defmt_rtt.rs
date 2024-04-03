// The following section is licensed under MIT license from the `defmt` project
// (https://github.com/knurling-rs/defmt):
//
// Permission is hereby granted, free of charge, to any
// person obtaining a copy of this software and associated
// documentation files (the "Software"), to deal in the
// Software without restriction, including without
// limitation the rights to use, copy, modify, merge,
// publish, distribute, sublicense, and/or sell copies of
// the Software, and to permit persons to whom the Software
// is furnished to do so, subject to the following
// conditions:
//
// The above copyright notice and this permission notice
// shall be included in all copies or substantial portions
// of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF
// ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED
// TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
// PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT
// SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
// OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR
// IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//! Logger runtime based on defmt
//! This code has been forked from [`defmt-rtt`](https://docs.rs/defmt-rtt/0.4.0/defmt_rtt/).
//!
//! NOTE when using this crate it's not possible to use (link to) the `rtt-target` crate
//!
//! ## Blocking/Non-blocking
//! `probe-run` puts RTT into blocking-mode, to avoid losing data.
//!
//! As an effect this implementation may block forever if `probe-run` disconnects on runtime. This
//! is because the RTT buffer will fill up and writing will eventually halt the program execution.
//!
//! `defmt::flush` would also block forever in that case.
use core::sync::atomic::{AtomicU16, AtomicUsize, Ordering};
use core::time::Duration;

use pxros::bindings::{PxAbort, PxError_t, PxGetId};

use super::tsim;
use crate::pxros::ticker::Ticker;
use crate::pxros::time::time_since_boot;

bitflags::bitflags! {
    /// Used for waiting on the defmt global logger lock.
    ///
    /// This occupies the 32nd bit/event slot in the available event slots.
    /// For now, this is a limitation of the Rust logging implementation to prevent deadlocks.
    #[derive(Copy, Clone)]
    pub struct LoggerEvent: u32 {
        const LoggerDelay = 1<<31;
    }
}

static LOGGER_TAKEN_BY_TASK_ID: AtomicU16 = AtomicU16::new(0);

/// This is the default size for buffers in `defmt-rtt`.
/// If you don't like it, just change it.
const DEFMT_BUF_SIZE: usize = 1024;

// Configure the timestamp
defmt::timestamp!("{=u64:us}", { time_since_boot().as_micros() as u64 });

/// Write log data into the output buffer.
#[inline]
fn do_write(bytes: &[u8]) {
    if tsim::is_running_on_tsim() {
        tsim::write(1, bytes);
    } else {
        unsafe { handle() }.write_all(bytes);
    }
}

/// Current encoder metadata
static mut ENCODER: defmt::Encoder = defmt::Encoder::new();

/// Defmt global logger
#[defmt::global_logger]
struct PxrosLogger;

unsafe impl defmt::Logger for PxrosLogger {
    fn acquire() {
        let current_task_id = PxGetId().id();

        while let Err(logger_task_id) =
            LOGGER_TAKEN_BY_TASK_ID.compare_exchange(0, current_task_id, Ordering::AcqRel, Ordering::Relaxed)
        {
            // Abort on reentrancy.
            // defmt does not support reentrancy: https://defmt.ferrous-systems.com/re-entrancy
            if current_task_id == logger_task_id {
                unsafe { PxAbort(PxError_t::PXERR_ACCESS_RIGHT) };
            }

            // Deschedules the task for a period of time to allow the task holding the lock to complete.
            Ticker::after(LoggerEvent::LoggerDelay, Duration::from_millis(5)).unwrap();
        }

        // SAFETY
        // We synchronized memory based on the atomic variable which also guarantees
        // unique access of the variable based on this trait.
        unsafe { ENCODER.start_frame(do_write) }
    }

    unsafe fn flush() {
        // SAFETY
        // Unique access guaranteed by Logger trait
        unsafe { handle() }.flush();
    }

    unsafe fn release() {
        // SAFETY
        // Unique access guaranteed by Logger trait
        unsafe {
            ENCODER.end_frame(do_write);
        }

        LOGGER_TAKEN_BY_TASK_ID.store(0, Ordering::Release);
    }

    unsafe fn write(bytes: &[u8]) {
        // SAFETY
        // Unique access guaranteed by Logger trait
        unsafe {
            ENCODER.write(bytes, do_write);
        }
    }
}

#[repr(C)]
struct Header {
    id: [u8; 16],
    max_up_channels: usize,
    max_down_channels: usize,
    up_channel: Channel,
}

const MODE_MASK: usize = 0b11;
/// Block the application if the RTT buffer is full, wait for the host to read data.
const MODE_BLOCK_IF_FULL: usize = 2;
/// Don't block if the RTT buffer is full. Truncate data to output as much as fits.
const MODE_NON_BLOCKING_TRIM: usize = 1;

// make sure we only get shared references to the header/channel (avoid UB)
/// # Safety
/// `Channel` API is not re-entrant; this handle should not be held from different execution
/// contexts (e.g. thread-mode, interrupt context)
pub(super) unsafe fn handle() -> &'static Channel {
    // NOTE the `rtt-target` API is too permissive. It allows writing arbitrary data to any
    // channel (`set_print_channel` + `rprint*`) and that can corrupt defmt log frames.
    // So we declare the RTT control block here and make it impossible to use `rtt-target` together
    // with this crate.
    #[no_mangle]
    static mut _SEGGER_RTT: Header = Header {
        id: *b"SEGGER RTT\0\0\0\0\0\0",
        max_up_channels: 1,
        max_down_channels: 0,
        up_channel: Channel {
            name: &NAME as *const _ as *const u8,
            buffer: unsafe { &mut BUFFER as *mut u8 },
            size: DEFMT_BUF_SIZE,
            write: AtomicUsize::new(0),
            read: AtomicUsize::new(0),
            flags: AtomicUsize::new(MODE_NON_BLOCKING_TRIM),
        },
    };

    static mut BUFFER: [u8; DEFMT_BUF_SIZE] = [0; DEFMT_BUF_SIZE];

    // Place NAME in data section, so the whole RTT header can be read from RAM.
    // This is useful if flash access gets disabled by the firmware at runtime.
    static NAME: [u8; 6] = *b"defmt\0";

    unsafe { &_SEGGER_RTT.up_channel }
}

/// RTT Up channel
#[repr(C)]
pub struct Channel {
    pub name: *const u8,
    /// Pointer to the RTT buffer.
    pub buffer: *mut u8,
    pub size: usize,
    /// Written by the target.
    pub write: AtomicUsize,
    /// Written by the host.
    pub read: AtomicUsize,
    /// Channel properties.
    ///
    /// Currently, only the lowest 2 bits are used to set the channel mode (see constants below).
    pub flags: AtomicUsize,
}

impl Channel {
    pub fn write_all(&self, mut bytes: &[u8]) {
        // the host-connection-status is only modified after RAM initialization while the device is
        // halted, so we only need to check it once before the write-loop
        let write = match self.host_is_connected() {
            true => Self::blocking_write,
            false => Self::nonblocking_write,
        };

        while !bytes.is_empty() {
            let consumed = write(self, bytes);
            if consumed != 0 {
                bytes = &bytes[consumed..];
            }
        }
    }

    fn blocking_write(&self, bytes: &[u8]) -> usize {
        if bytes.is_empty() {
            return 0;
        }

        // calculate how much space is left in the buffer
        let read = self.read.load(Ordering::Relaxed);
        let write = self.write.load(Ordering::Acquire);
        let available = available_buffer_size(read, write);

        // abort if buffer is full
        if available == 0 {
            return 0;
        }

        self.write_impl(bytes, write, available)
    }

    fn nonblocking_write(&self, bytes: &[u8]) -> usize {
        let write = self.write.load(Ordering::Acquire);

        // NOTE truncate at BUF_SIZE to avoid more than one "wrap-around" in a single `write` call
        self.write_impl(bytes, write, DEFMT_BUF_SIZE)
    }

    fn write_impl(&self, bytes: &[u8], cursor: usize, available: usize) -> usize {
        let len = bytes.len().min(available);

        // copy `bytes[..len]` to the RTT buffer
        unsafe {
            if cursor + len > DEFMT_BUF_SIZE {
                // split memcpy
                let pivot = DEFMT_BUF_SIZE - cursor;
                core::ptr::copy_nonoverlapping(bytes.as_ptr(), self.buffer.add(cursor), pivot);
                core::ptr::copy_nonoverlapping(bytes.as_ptr().add(pivot), self.buffer, len - pivot);
            } else {
                // single memcpy
                core::ptr::copy_nonoverlapping(bytes.as_ptr(), self.buffer.add(cursor), len);
            }
        }

        // adjust the write pointer, so the host knows that there is new data
        self.write
            .store(cursor.wrapping_add(len) % DEFMT_BUF_SIZE, Ordering::Release);

        // return the number of bytes written
        len
    }

    pub fn flush(&self) {
        // return early, if host is disconnected
        if !self.host_is_connected() {
            return;
        }

        // busy wait, until the read- catches up with the write-pointer
        let read = || self.read.load(Ordering::Relaxed);
        let write = || self.write.load(Ordering::Relaxed);
        while read() != write() {}
    }

    fn host_is_connected(&self) -> bool {
        // we assume that a host is connected if we are in blocking-mode. this is what probe-run does.
        self.flags.load(Ordering::Relaxed) & MODE_MASK == MODE_BLOCK_IF_FULL
    }
}

/// How much space is left in the buffer?
fn available_buffer_size(read_cursor: usize, write_cursor: usize) -> usize {
    if read_cursor > write_cursor {
        read_cursor - write_cursor - 1
    } else if read_cursor == 0 {
        DEFMT_BUF_SIZE - write_cursor - 1
    } else {
        DEFMT_BUF_SIZE - write_cursor
    }
}
