//! Waker storage.
use core::any::Any;
use core::ptr::NonNull;
use core::task::Waker;

use super::new_waker;
use crate::executor::task::GenericContext;
use crate::executor::RawExecutor;

/// This structure can be used to generate a waker that provides access to the given
/// [executor specific local context](RawExecutor::TaskLocalData) and the [generic context](GenericContext).
///
/// # Note
/// The internal fields are all optional, for performance reasons this can be changed to
/// a [core::mem::MaybeUninit] since any panicking methods are not supposed to be called
/// before the fields are initialized.
pub(crate) struct WakerStorage<E: RawExecutor> {
    local_data: Option<E::TaskLocalData>,
    waker_context: Option<GenericContext>,
    waker_ref: Option<WakerRef>,
    waker: Option<Waker>,
}

impl<E: RawExecutor> WakerStorage<E> {
    /// Create a new context.
    ///
    /// Before doing anything with this structure, [WakerStorage::init] should be
    /// called for it.
    pub const fn new() -> Self {
        WakerStorage {
            local_data: None,
            waker_context: None,
            waker: None,
            waker_ref: None,
        }
    }

    /// Initialize a waker from a context and local data.
    ///
    /// # Safety
    /// The pointer passed must be aligned, point to a initialized instance of
    /// a [WakerStorage] and not be aliased when calling this function. It captures
    /// a `'static`, immutable reference to that structure. Stated explicitely:
    /// The pointer must have a `'static` lifetime.
    ///
    /// Effectively the user must consider the reference to self to be a `&mut self`,
    /// while after that only methods with `&self` as a receiver must be called.
    /// In particular, calling [WakerStorage::waker()] is safe.
    ///
    /// This implies that this function is only called once, and that the pointer
    /// can safely dereferenced to a `&mut self` in this function
    pub unsafe fn init(mut this: NonNull<WakerStorage<E>>, local_data: E::TaskLocalData)
    where
        E: 'static,
    {
        // SAFETY: See function documentation
        let this = unsafe { this.as_mut() };

        this.local_data = Some(local_data);
        this.waker_context = Some(GenericContext::default());

        // SAFETY: We never access the fields local_data and global_context
        // after this
        let waker_data = unsafe {
            WakerRef::new(
                NonNull::from(this.local_data.as_ref().unwrap()),
                NonNull::from(this.waker_context.as_ref().unwrap()),
            )
        };
        this.waker_ref = Some(waker_data);

        let waker = new_waker(this.waker_ref.as_ref().unwrap());

        this.waker = Some(waker);
    }

    /// Reference to the waker for usage in an asynchronous executor.
    ///
    /// # Panics
    /// This will panic if called before [WakerStorage::init].
    pub fn waker(&self) -> &Waker {
        self.waker.as_ref().unwrap()
    }

    /// Access to the context that is also available to the Waker.
    ///
    /// # Panics
    /// This will panic if called before [WakerStorage::init].
    pub fn waker_context(&self) -> &GenericContext {
        self.waker_context.as_ref().unwrap()
    }

    /// Mutably access the local data.
    ///
    /// # Panics
    /// This will panic if called before [WakerStorage::init].
    pub fn local_data(&mut self) -> &mut E::TaskLocalData {
        unsafe {
            self.waker_ref
                .as_ref()
                .expect("No context registered")
                .local()
                .downcast_mut()
                .expect("Inconsistent state, waker not as expected")
        }
    }
}

/// This is the structure which is pointed to by the waker.
///
/// It allows to safely erase the types and access them from the context of a
/// waker.
///
/// This is an intermediate step because `*mut dyn Any` is a fat pointer which
/// can not be stored in the data field for the waker.
pub(crate) struct WakerRef {
    local: NonNull<dyn Any>,
    flag: NonNull<GenericContext>,
}

impl WakerRef {
    /// Create a new instance from a context.
    ///
    /// # Safety
    /// The local pointer must be not aliased and have a static lifetime
    /// The pointer to the generic context may be aliased, but also must have a static lifetime
    pub(crate) unsafe fn new(local: NonNull<dyn Any>, flag: NonNull<GenericContext>) -> Self {
        WakerRef { local, flag }
    }

    /// Return a reference to the generic context.
    pub(crate) fn waker_context(&self) -> &GenericContext {
        // SAFETY: The pointer may always be dereferenced since it has a static lifetime
        // Check the `new` constructor
        unsafe { self.flag.as_ref() }
    }

    /// Provide access to the local waker data
    ///
    /// # Safety
    /// The user must guarantee mutable access to the `local` part of this struct,
    /// In specific:
    /// - This function must not be called re-entrantely on the same waker data structure
    /// - This function must be called from the same context where WakerData was created. This is because the `local`
    ///   part of this structure is actually not [Send]/[Sync]
    #[allow(clippy::mut_from_ref)]
    pub unsafe fn local(&self) -> &mut dyn Any {
        // SAFETY: The pointer is always valid, and we can assume this function
        // not be called reentrantely
        unsafe { &mut *self.local.as_ptr() }
    }
}

// SAFETY: The `global` part is Send and Sync, and the `local` part can only be
// accessed in a non Send/Sync manner
unsafe impl Send for WakerRef {}
// SAFETY: The `global` part is Send and Sync, and the `local` part can only be
// accessed in a non Send/Sync manner
unsafe impl Sync for WakerRef {}
