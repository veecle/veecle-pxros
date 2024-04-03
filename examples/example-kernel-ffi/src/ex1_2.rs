//! Exercise 1.2
extern "Rust" {
    /// This FFI return the first half of the flag encoded as &'str.
    fn partial_secret_flag() -> &'static str;

    /// The second FFI return an HEX encoded digit.
    fn partial_flag_suffix() -> u32;
}

/// The function should call (via `unsafe`) the two FFIs and print the
/// flag by concatenating the `&str` with an HEX encoded representation
/// of the suffix.
///
/// For formatting rules see [defmt].
pub fn ex1_2_solution() {
    let partial_flag = unsafe { partial_secret_flag() };
    let flag_suffix = unsafe { partial_flag_suffix() };

    defmt::info!("The (1.2) flag is {} {:X}", partial_flag, flag_suffix);
}
