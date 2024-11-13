use std::path::Path;
use std::process::Command;

use std::fs::File;
use std::io::{BufRead, BufReader, BufWriter, Write};

const SYMBOL_SECTIONS: [&str; 8] = [
    ".text", ".rodata", ".data", ".bss", ".sdata", ".sbss", ".sdata2", ".sbss2",
];

const MKB1_MAP: &str = "src/mkb1/supermonkeyball.map";

fn convert_smb1_symbol_map(in_path: &Path, out_path: &Path) {
    let in_file = File::open(in_path).unwrap();
    let mut in_file = BufReader::new(in_file);
    let out_file = File::create(out_path).unwrap();
    let mut out_file = BufWriter::new(out_file);

    let mut line = String::new();
    let mut last_section_line = String::new();
    while {
        line.clear();
        in_file.read_line(&mut line).unwrap()
    } > 0
    {
        if line.starts_with(".") {
            last_section_line.clone_from(&line);
        }

        let first_word = last_section_line.split(" ").next().unwrap_or("");
        if !SYMBOL_SECTIONS.contains(&first_word) {
            continue;
        }
        let splits: Vec<&str> = line.split_whitespace().collect();
        if splits.len() < 5 {
            continue;
        }
        let addr = splits[2];
        let mut symbol = splits[3];
        if symbol.chars().all(|c| c.is_numeric()) {
            symbol = splits[4];
        }
        if symbol.starts_with('@') || symbol.starts_with('.') {
            continue;
        }

        writeln!(out_file, "{}:{}", addr, symbol).unwrap();
    }
}

fn generate_git_hash() {
    let output = Command::new("git")
        .args(["rev-parse", "@"])
        .output()
        .unwrap();
    let mut git_hash = String::from_utf8(output.stdout).unwrap();
    git_hash.truncate(8);

    let status = Command::new("git")
        .args(["status", "--porcelain"])
        .output()
        .unwrap();
    let status = String::from_utf8(status.stdout).unwrap();
    if !status.trim().is_empty() {
        git_hash.push_str("-dirty");
    }

    println!("cargo:rustc-env=GIT_HASH={}", git_hash);
}

fn get_base_bindgen_builder() -> bindgen::Builder {
    // The bindgen::Builder is the main entry point
    // to bindgen, and lets you build up options for
    // the resulting bindings.
    bindgen::Builder::default()
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
}

fn generate_mkb1_bindings() {
    let mut builder = get_base_bindgen_builder();

    // Find all .h files
    for entry in std::fs::read_dir("3rdparty/smb-decomp/src").unwrap() {
        let entry = entry.unwrap();
        let path = entry.path();
        if path
            .file_name()
            .map_or(true, |n| n == "rel_sample.c" || n == "sel_stage_rel.c")
        {
            continue;
        }
        if path
            .extension()
            .map_or(true, |ext| ext != "h" && ext != "c")
        {
            continue;
        }
        builder = builder.header(path.to_str().unwrap());
    }

    let bindings = builder
        .clang_args([
            "-I3rdparty/smb-decomp/include",
            "-DC_ONLY",
            "-DNONMATCHING",
            // Treat all .c files as header files so their definitions are included in bindings
            "-x",
            "c-header",
        ])
        // Finish the builder and generate the bindings.
        .generate()
        // Unwrap the Result and panic on failure.
        .expect("Unable to generate bindings");

    // Write the bindings to the $OUT_DIR/bindings.rs file.
    bindings
        .write_to_file("src/mkb1/mkb1.rs")
        .expect("Couldn't write bindings!");
}

fn generate_mkb2_bindings() {
    let bindings = get_base_bindgen_builder()
        .header("src/mkb2/mkb2_ghidra.h")
        .generate()
        // Unwrap the Result and panic on failure.
        .expect("Unable to generate bindings");

    // Write the bindings to the $OUT_DIR/bindings.rs file.
    bindings
        .write_to_file("src/mkb2/mkb2.rs")
        .expect("Couldn't write bindings!");
}

fn main() {
    generate_git_hash();
    generate_mkb1_bindings();
    generate_mkb2_bindings();

    println!("cargo::rerun-if-changed={}", MKB1_MAP);
    let map_path = Path::new(MKB1_MAP);
    let lst_path = map_path.with_extension("lst");
    convert_smb1_symbol_map(map_path, &lst_path);
}
