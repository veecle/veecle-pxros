//! Interface to the Pxros name server extension.
//!
//! This requires the name server to be run in a dedicated task.

use core::time::Duration;

use pxros::bindings::{PxError_t, PxNameId_t, PxNameQuery, PxNameRegister, PxTask_t};
use pxros::PxResult;

use super::events::Event;
use super::ticker::Ticker;

/// Wrapper around a [PxNameId_t].
///
/// # Note
/// This interface is incomplete. Future PRs shall ensure this is
/// generated at compile time to avoid mistakes.
pub struct TaskName(PxNameId_t);

impl TaskName {
    /// Create a new TaskName.
    pub const fn new(id: u32) -> Self {
        TaskName(PxNameId_t { id })
    }

    /// Query the runtime [PxTask_t] identifier of this task via [NameServer].
    ///
    /// Shortcut for [NameServer::query], see that for details.
    ///
    /// # Panics
    /// This shall panic if the query fails.
    pub fn query<E: Event>(&self, event: E) -> PxTask_t {
        NameServer::query(self, event).expect("Failed to query task name")
    }

    /// Register the name via the [NameServer].
    ///
    /// Shortcut for [NameServer::register], see that for details.
    ///
    /// # Panics
    /// This shall panic if the registration fails.
    pub fn register(&self, id: PxTask_t) {
        NameServer::register(self, id).expect("Failed to register name in NameServer");
    }
}

/// Rusty interface to PXROS name server.
///
/// The name server allows for tasks to query the runtime [PxTask_t] ID
/// of other tasks through a statically defined [TaskName].
///
/// Tasks **must** register themselves via [NameServer::register] in order to
/// be discoverable: This may not happen instantly; delays should be accounted
/// for.
pub struct NameServer;

impl NameServer {
    /// Delay job duration
    const DELAY: Duration = Duration::from_millis(100);
    /// Maximum number of tries the `Query` method will perform before
    /// returning error
    const TRIES: usize = 10;

    /// Queries the [PxTask_t] associated with a given [TaskName].
    ///
    /// This tries the operation once via [PxNameQuery].
    pub fn try_query(name: &TaskName) -> PxResult<PxTask_t> {
        let size = core::mem::size_of::<PxTask_t>() as u32;
        let mut task_id = PxTask_t::invalid();

        // Safety: pointer to task_id is valid and arguments are correct
        let result = unsafe { PxNameQuery(name.0, size, &mut task_id as *mut PxTask_t as _, core::ptr::null_mut()) };

        PxResult::from(result).map(|_| task_id)
    }

    /// Query the [PxTask_t] up to `NameServer::TRIES` time until available.
    ///
    /// This requires a *ticker* event for the duration of the query to support
    /// delays: this is needed to yield execution back to the kernel while waiting.
    /// This is required as some tasks may be spawning after this or may still
    /// have to be registered in the name server.
    ///
    /// See [NameServer::try_query] for details.
    pub fn query<E: Event>(name: &TaskName, event: E) -> PxResult<PxTask_t> {
        let mut ticker = Ticker::every(event, Self::DELAY)?;

        for index in 0..Self::TRIES {
            match NameServer::try_query(name) {
                Ok(task) => return Ok(task),
                Err(e) => {
                    defmt::debug!("[NameServer] error: {:?}. Tries: {}/{}", e, index, Self::TRIES);
                },
            }

            ticker.wait();
        }

        Err(PxError_t::PXERR_TASK_ILLTASK)
    }

    /// Registers a name via the name server.
    ///
    /// This operation mail fail, in which case the task won't be registered
    /// and reachable by other tasks.
    ///
    /// See [PxNameRegister] for details.
    pub fn register(name: &TaskName, id: PxTask_t) -> PxResult<()> {
        let px_task_size = core::mem::size_of::<PxTask_t>() as u32;
        // Safety: pointer to task_id is valid and arguments are correct
        let result = unsafe { PxNameRegister(name.0, px_task_size, &id as *const PxTask_t as _) };

        PxResult::from(result)
    }
}
