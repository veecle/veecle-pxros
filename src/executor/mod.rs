//! Veecle-pxros asynchronous runtime.
//!
//! This module defines the core utilities to provide an asynchronous runtime.
//!
//! The [generic executor](executor::Executor) is the core of the asynchronous runtime.
//! The hardware/OS must provide an implementation of [RawExecutor].
#![allow(clippy::module_inception)]

pub mod executor;
pub mod task;
mod waker;

/// Defines when tasks are ready and provides an implementation to wait for a
/// task to become ready.
///
/// An executor strategy is defined by waiting on a list of [TaskContext].
/// These contexts are associated to a task and only available locally. The task
/// may manipulate the context to communicate with the strategy on what the task
/// is waiting for.
pub trait RawExecutor: Sized {
    /// The data available locally to a task.
    ///
    /// This data is provided via contexts; this is where the executor shall store
    /// any specific implementation details
    type TaskLocalData: 'static;

    /// Checks whether any task is ready.
    ///
    /// If indicated by its argument, this function is allowed to block.
    ///
    /// The implementation communicates to the callee that a task is ready to be
    /// executed via [TaskContext::mark_ready].
    fn wait<C: TaskContext<Self>>(&mut self, tasks: &mut [C], may_block: bool);

    /// Generate a new context to associate with a new task.
    fn new_context(&self) -> Self::TaskLocalData;
}

/// Provides generic access to a tasks context.
pub trait TaskContext<E: RawExecutor> {
    /// Provide access to the task local data.
    fn local_data(&mut self) -> &mut E::TaskLocalData;

    /// Mark the task as ready to be processed.
    fn mark_ready(&self);
}
