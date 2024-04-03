//! Hidden backend code for the exercise 1 to validate flags
use pxros::bindings::{PxGetId, PxTask_t};

#[no_mangle]
fn secret_flag() -> &'static str {
    "5SjZ9CVsOV"
}

#[no_mangle]
fn partial_secret_flag() -> &'static str {
    "uUGnnL45"
}

#[no_mangle]
fn partial_flag_suffix() -> u32 {
    0x47
}

#[no_mangle]
fn secret_flag_from_task_id(task_id: PxTask_t) -> &'static str {
    match PxGetId().id() == task_id.id() {
        true => "OWKuUC0XVJ",
        false => panic!("You called me with an incorrect task ID"),
    }
}
