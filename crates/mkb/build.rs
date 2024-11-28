fn generate_bindings() {
    // The bindgen::Builder is the main entry point
    // to bindgen, and lets you build up options for
    // the resulting bindings.
    let bindings = bindgen::Builder::default()
        // The input header we would like to generate
        // bindings for.
        .header("src/mkb2_ghidra.h")
        // Tell cargo to invalidate the built crate whenever any of the
        // included header files changed.
        .parse_callbacks(Box::new(bindgen::CargoCallbacks::new()))
        // Use core instead of std, so no-std works.
        .use_core()
        // Derive Default trait for types.
        .derive_default(true)
        // Don't generate static size assertions - these break rust-analyzer which I don't know how
        // to configure to a 32-bit architecture.
        .layout_tests(false)
        // Clean up some redundant noise in the output.
        .merge_extern_blocks(true)
        // Finish the builder and generate the bindings.
        .generate()
        // Unwrap the Result and panic on failure.
        .expect("Unable to generate bindings");

    // Write the bindings to the $OUT_DIR/bindings.rs file.
    bindings
        .write_to_file("src/mkb.rs")
        .expect("Couldn't write bindings");
}

fn main() {
    generate_bindings();
}
