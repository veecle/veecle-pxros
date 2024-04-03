//! Waker implementation.
use core::any::Any;
use core::task::{Context, Poll, RawWaker, RawWakerVTable, Waker};

pub(crate) mod storage;

use storage::WakerRef;

/// Allows access to the current task local data.
///
/// # Panics
/// This function will panic if not called from within the executor, or when
/// the type expected as local context was not correct
pub fn with_local_data<F, R, L: Any>(mut callback: F, cx: &Context<'_>) -> Poll<R>
where
    F: FnMut(&mut L) -> Poll<R>,
{
    let waker = cx.waker().as_raw();
    if waker.vtable() != &VTABLE {
        panic!("This function can only be called from within veecle-pxros")
    }

    let context = waker.data();
    // SAFETY: This is safe as our waker adheres to the rules of RawWaker, the reference
    // to the context is static and the data the reference points to is Sync.
    let context = unsafe { &*(context as *const WakerRef) };

    // SAFETY: The local function is only called in the context of a task
    // here, and since we are not calling an async function there can be no
    // reentrancy.
    let context = unsafe { context.local() };
    callback(context.downcast_mut().expect("Unexpected local context type"))
}

/// Create a new [Waker] from a [WakerRef].
pub(crate) fn new_waker(context: &'static WakerRef) -> Waker {
    // Keep this here to make sure nobody makes WakerRef non-Sync by accident
    fn assert_is_sync<T: Sync>(_: &T) {}
    assert_is_sync(context);

    // SAFETY: Our waker adheres to the rules of RawWaker, the reference to the context
    // is static and the data the reference points to is Sync
    unsafe { Waker::from_raw(RawWaker::new(context as *const WakerRef as *const (), &VTABLE)) }
}

const VTABLE: RawWakerVTable = RawWakerVTable::new(clone, wake, wake_by_ref, drop);

unsafe fn clone(data: *const ()) -> RawWaker {
    RawWaker::new(data, &VTABLE)
}

unsafe fn wake(data: *const ()) {
    // SAFETY: This is safe as our waker adheres to the rules of RawWaker, the reference
    // to the context is static and the data the reference points to is Sync.
    let data = unsafe { &*(data as *const WakerRef) };
    data.waker_context().mark_ready()
}

unsafe fn wake_by_ref(data_ref: *const ()) {
    // SAFETY: This is safe as our waker adheres to the rules of RawWaker, the reference
    // to the context is static and the data the reference points to is Sync.
    let data_ref = unsafe { &*(data_ref as *const WakerRef) };
    data_ref.waker_context().mark_ready()
}

unsafe fn drop(_: *const ()) {}
