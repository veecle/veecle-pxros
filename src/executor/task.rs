//! Executor task storage.
//!
//! This module contains implementation details used to store executor-specific
//! data on a per-task fashion.
use core::future::Future;
use core::pin::Pin;
use core::ptr::NonNull;
use core::sync::atomic::{AtomicU32, Ordering};
use core::task::{Context, Poll};

use super::waker::storage::WakerStorage;
pub use super::waker::with_local_data;
use super::RawExecutor;

/// This is the abstraction between the generic executor and the task storage.
///
/// This trait is used to erase the concrete type of a [TaskStorage] so the executor
/// can work with futures of different concrete types.
pub(crate) trait Task<E: RawExecutor, R> {
    /// Poll the task in [Future] fashion.
    fn poll(self: Pin<&mut Self>) -> Poll<R>;

    /// Access the local data of the task. See [RawExecutor]
    /// for documentation.
    fn local_data(self: Pin<&mut Self>) -> &mut E::TaskLocalData;

    /// Provide access to the global context that is also available through the Waker.
    fn waker_context(&self) -> &GenericContext;
}

/// Generic context that must be available for every task storage, independent
/// of the specific executor.
#[derive(Default)]
pub(crate) struct GenericContext {
    is_ready: AtomicU32,
}

impl GenericContext {
    /// Mark this context as ready.
    pub fn mark_ready(&self) {
        self.is_ready.store(1, Ordering::Release);
    }

    /// Check whether this context is marked as ready.
    pub fn is_ready(&self) -> bool {
        self.is_ready.load(Ordering::Acquire) == 1
    }

    /// Clear and return the previous ready status.
    pub(crate) fn clear_ready(&self) -> bool {
        self.is_ready.swap(0, Ordering::AcqRel) == 1
    }
}

/// Storage structure for an async task & executor.
///
/// This structure must have `&'static mut` lifetime; this can be achieved
/// by wrapping it in a [static_cell::StaticCell].
#[pin_project::pin_project]
pub struct TaskStorage<F: Future, E: RawExecutor> {
    waker_data: WakerStorage<E>,
    was_initialized: bool,
    #[pin]
    future: F,
}

impl<E, F> TaskStorage<F, E>
where
    E: RawExecutor,
    F: Future,
{
    /// Create a new storage from a future.
    pub const fn new(future: F) -> Self {
        TaskStorage {
            waker_data: WakerStorage::new(),
            future,
            was_initialized: false,
        }
    }

    /// Initialize the static storage once.
    ///
    /// # Panics
    /// This will panic if called twice.
    pub fn init(&'static mut self, local_data: E::TaskLocalData) -> &'static mut Self {
        assert!(!self.was_initialized, "This function can only be called once");

        // SAFETY: We only call this function once, and the pointer has a static
        // lifetime.
        unsafe {
            WakerStorage::init(NonNull::from(&mut self.waker_data), local_data);
        }

        self
    }
}

impl<E, R, F> Task<E, R> for TaskStorage<F, E>
where
    E: RawExecutor,
    F: Future<Output = R>,
{
    fn local_data(self: Pin<&mut Self>) -> &mut E::TaskLocalData {
        let this = self.project();
        this.waker_data.local_data()
    }

    fn waker_context(&self) -> &GenericContext {
        self.waker_data.waker_context()
    }

    fn poll(self: Pin<&mut Self>) -> Poll<R> {
        let this = self.project();
        let mut context = Context::from_waker(this.waker_data.waker());

        Future::poll(this.future, &mut context)
    }
}

#[cfg(test)]
mod tests {
    use core::future::Future;
    use core::pin::Pin;
    use core::task::Poll;

    use super::TaskStorage;
    use crate::executor::RawExecutor;

    struct DummyFuture;

    impl Future for DummyFuture {
        type Output = ();

        fn poll(self: core::pin::Pin<&mut Self>, _cx: &mut core::task::Context<'_>) -> core::task::Poll<Self::Output> {
            core::task::Poll::Ready(())
        }
    }

    pub struct DummyExecutor;

    impl RawExecutor for DummyExecutor {
        type TaskLocalData = ();

        fn wait<C: crate::executor::TaskContext<Self>>(&mut self, _tasks: &mut [C], _may_block: bool) {
            todo!()
        }

        fn new_context(&self) -> Self::TaskLocalData {
            todo!()
        }
    }

    #[test]
    fn init() {
        let storage = Box::leak(Box::new(TaskStorage::<_, DummyExecutor>::new(DummyFuture)));

        storage.init(());
    }

    #[test]
    fn run_task_once() {
        use crate::executor::task::Task;

        let storage = Box::leak(Box::new(TaskStorage::<_, DummyExecutor>::new(DummyFuture)));
        let storage = storage.init(());
        let storage = Pin::static_mut(storage);

        assert_eq!(storage.poll(), Poll::Ready(()));
    }
}
