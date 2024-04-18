//! Utilities to use the Pxros asynchronous executor.

/// This macro will declare task storages and executor, pipe the tasks
/// in the executor and run it. The mailbox will be moved into the executor
/// at creation
///
/// This will not return unless all async tasks completes. Tasks can return
/// values which will be then collected in a vector and delivered to the
/// caller
///
/// ## Usage
///
/// ```
/// # #![feature(type_alias_impl_trait)]
/// # #![feature(macro_metavar_expr)]
/// # use pxros::PxResult;
/// # use pxros::bindings::PxMbx_t;
/// # use veecle_pxros::pxros::task::PxrosTask;
/// # use veecle_pxros::pxros_run;
/// /// Declare the superset of events supported by the executor
/// bitflags::bitflags! {
///     #[derive(Copy, Clone)]
///     pub struct AsyncEvent: u32 {
///         const A = 0b0001_0000;
///         const B = 0b0010_0000;
///     }
/// }
///
/// // Declare the tasks
/// async fn foo(x: i32) -> PxResult<i64> {
///     todo!()
/// }
/// async fn bar() -> PxResult<i64> {
///     todo!()
/// }
///
/// struct AsyncExecutor;
/// impl PxrosTask for AsyncExecutor {
///     fn task_main(mailbox: PxMbx_t) -> PxResult<()> {
///         let result = pxros_run!(mailbox, AsyncEvent, PxResult<i64>, foo(10), bar());
///
///         // All task completed
///         result
///             .iter()
///             .for_each(|result| defmt::info!("My result is {:?}", result));
///         Ok(())
///     }
/// }
/// ```
#[macro_export]
macro_rules! pxros_run {
    ($mailbox:ident, $event:ty, $ret:ty, $($future:expr),* $(,)?) => {{
        use $crate::executor::executor::Executor;
        use $crate::pxros::executor::PxrosExecutor;
        use pxros::PxResult;

        type SpecificExecutor = Executor<'static, PxrosExecutor<$event>, $ret, ${count(future)}>;
        let mut executor: SpecificExecutor = Executor::new(PxrosExecutor::new($mailbox));

        $(
            {
                use $crate::StaticCell;
                use $crate::executor::task::TaskStorage;

                type F = impl ::core::future::Future<Output = $ret>;
                static TASK: StaticCell<TaskStorage<F, PxrosExecutor<$event>>> = StaticCell::new();

                let task = TASK.init(TaskStorage::new($future));
                executor.add(task);
            }
        )*

        executor.run()
    }};
}
