//! Generic executor.
//!
//! This module defines the high-level executor logic. Independent from hardware
//! specific details the executor actually decides when to poll which tasks.
use core::future::Future;
use core::pin::Pin;
use core::task::{Context, Poll};

use heapless::Vec;

use super::task::{Task, TaskStorage};
use super::{RawExecutor, TaskContext};

/// A type that can be processed as result from any of the [Executor]
/// tasks.
///
/// This lets the executor know if it should stop execution early.
pub trait TaskResult {
    /// Indicates a fatal result that should stop execution.
    fn bail(&self) -> bool {
        false
    }
}

impl<T, E> TaskResult for Result<T, E> {
    fn bail(&self) -> bool {
        self.is_err()
    }
}

/// A generic executor able to run a fixed number of tasks.
///
/// This structure provides wakers to the tasks and defines execution order and
/// priority.
///
/// The trait [RawExecutor] allows upstream users to define efficient logic
/// when a task is actually ready to be run.
pub struct Executor<'a, E: RawExecutor, R, const TASK_COUNT: usize> {
    #[allow(clippy::type_complexity)]
    tasks: Vec<(usize, Pin<&'a mut dyn Task<E, R>>), TASK_COUNT>,
    executor: E,
}

impl<'a, E: RawExecutor, R: 'static, const TASK_COUNT: usize> Executor<'a, E, R, TASK_COUNT>
where
    R: 'static,
    R: TaskResult,
{
    /// Allows us to create an array of unset values even
    /// if no value of R is known and R is not clone.
    const EMPTY: Option<R> = None;

    /// Creates a new instance.
    pub fn new(executor: E) -> Self {
        Executor {
            tasks: Vec::new(),
            executor,
        }
    }

    /// Add a task to be run in this executor.
    ///
    /// # Panics
    /// This function panics if the executor is full.
    pub fn add<F>(&mut self, task: &'static mut TaskStorage<F, E>)
    where
        F: Future<Output = R>,
    {
        let local_context = self.executor.new_context();
        let task = task.init(local_context);

        task.waker_context().mark_ready();

        self.tasks
            .push((self.tasks.len(), Pin::static_mut(task)))
            .map_err(|_| ())
            .expect("Executor has no space left for a new task")
    }

    /// Run until either all tasks in the executor have finished or return early
    /// if one of the tasks finishes with a result where [TaskResult::bail]
    /// returns true.
    ///
    /// This method relies on the implementation of [RawExecutor::wait].
    pub fn run(mut self) -> [Option<R>; TASK_COUNT] {
        // Prepare a mutable array where to store the return types; could
        // be none if the task *fails* without returning
        let mut results = [Self::EMPTY; TASK_COUNT];

        loop {
            self.tasks.retain_mut(|(index, task)| {
                if task.waker_context().clear_ready() {
                    // In case the task completes, store in the correct array index
                    // it's result
                    match task.as_mut().poll() {
                        Poll::Pending => true,
                        Poll::Ready(r) => {
                            results[*index] = Some(r);
                            false
                        },
                    }
                } else {
                    true
                }
            });

            // Check if any task failed and we need to bail or we are done
            if results.iter().flatten().any(|r| r.bail()) || self.tasks.is_empty() {
                break;
            }

            let some_task_ready = self.tasks.iter().any(|task| task.1.waker_context().is_ready());

            self.executor.wait(self.tasks.as_mut(), !some_task_ready);
        }

        results
    }

    /// Allows access to the current task local data.
    ///
    /// # Panics
    /// This function will panic if not called from within this executor.
    pub fn with_local_data<F, T>(callback: F, cx: &Context<'_>) -> Poll<T>
    where
        F: FnMut(&mut E::TaskLocalData) -> Poll<T>,
    {
        super::waker::with_local_data(callback, cx)
    }
}

impl<'a, E: RawExecutor, R> TaskContext<E> for (usize, Pin<&'a mut dyn Task<E, R>>) {
    fn local_data(&mut self) -> &mut E::TaskLocalData {
        self.1.as_mut().local_data()
    }

    fn mark_ready(&self) {
        self.1.waker_context().mark_ready()
    }
}

#[cfg(test)]
mod tests {
    use core::future::{poll_fn, Future};
    use core::pin::Pin;
    use core::sync::atomic::{AtomicBool, Ordering};
    use core::task::{Context, Poll};

    use super::{Executor, RawExecutor};
    use crate::executor::task::TaskStorage;

    struct SimpleExecutor {}

    impl SimpleExecutor {
        pub fn new() -> Self {
            SimpleExecutor {}
        }
    }

    impl RawExecutor for SimpleExecutor {
        type TaskLocalData = ();

        fn wait<C: super::TaskContext<Self>>(&mut self, _: &mut [C], _may_block: bool) {}

        fn new_context(&self) -> Self::TaskLocalData {}
    }

    /// Implements [Future], resolving when called as many times as the
    /// target specifies
    struct CountToTarget {
        target: u8,
        current: u8,
        callback: fn(),
    }

    impl CountToTarget {
        /// The callback is invoked when the target is reached
        pub fn count_to(target: u8, callback: fn()) -> Self {
            CountToTarget {
                target,
                current: 0,
                callback,
            }
        }
    }

    impl Future for CountToTarget {
        type Output = Result<(), ()>;

        fn poll(mut self: Pin<&mut Self>, cx: &mut Context<'_>) -> Poll<Self::Output> {
            match self.current.cmp(&self.target) {
                core::cmp::Ordering::Less => {
                    self.current += 1;
                    cx.waker().wake_by_ref();
                    Poll::Pending
                },
                core::cmp::Ordering::Equal => {
                    (self.callback)();
                    Poll::Ready(Ok(()))
                },
                core::cmp::Ordering::Greater => {
                    panic!("poll() called one to many times")
                },
            }
        }
    }

    macro_rules! static_storage {
        ($future:ident, $executor_type:ty) => {{
            use $crate::executor::task::TaskStorage;
            use $crate::StaticCell;

            type F = impl ::core::future::Future<Output = Result<(), ()>>;
            static TASK: StaticCell<TaskStorage<F, $executor_type>> = StaticCell::new();

            TASK.init(TaskStorage::new($future))
        }};
    }

    #[test]
    fn single_task() {
        static DID_FINISH: AtomicBool = AtomicBool::new(false);

        let future = CountToTarget::count_to(5, || DID_FINISH.store(true, Ordering::Relaxed));
        let task = static_storage!(future, SimpleExecutor);

        let mut executor: Executor<'_, SimpleExecutor, Result<(), ()>, 1> = Executor::new(SimpleExecutor::new());

        executor.add(task);
        executor.run();

        assert!(DID_FINISH.load(Ordering::Relaxed));
    }

    #[test]
    fn multiple_tasks() {
        static DID_FINISH_A: AtomicBool = AtomicBool::new(false);
        static DID_FINISH_B: AtomicBool = AtomicBool::new(false);

        let future_a = CountToTarget::count_to(5, || DID_FINISH_A.store(true, Ordering::Relaxed));
        let task_a = static_storage!(future_a, SimpleExecutor);
        let future_b = CountToTarget::count_to(42, || DID_FINISH_B.store(true, Ordering::Relaxed));
        let task_b = static_storage!(future_b, SimpleExecutor);

        let mut executor: Executor<'_, SimpleExecutor, Result<(), ()>, 2> = Executor::new(SimpleExecutor::new());

        executor.add(task_a);
        executor.add(task_b);
        executor.run();

        assert!(DID_FINISH_A.load(Ordering::Relaxed));
        assert!(DID_FINISH_B.load(Ordering::Relaxed));
    }

    #[test]
    fn async_function() {
        static DID_FINISH: AtomicBool = AtomicBool::new(false);

        async fn counter(target: u8) -> Result<(), ()> {
            for _ in 0..target {
                poll_fn(|_| Poll::Ready(())).await
            }

            DID_FINISH.store(true, Ordering::Relaxed);

            Ok(())
        }

        let task = Box::leak(Box::new(TaskStorage::new(counter(3))));

        let mut executor: Executor<'_, SimpleExecutor, Result<(), ()>, 1> = Executor::new(SimpleExecutor::new());

        executor.add(task);
        executor.run();

        assert!(DID_FINISH.load(Ordering::Relaxed));
    }
}
