use std::io::Write;
use std::path::Path;
use std::{path::PathBuf, process::Command};

use anyhow::anyhow;
use anyhow::bail;

// We don't read from the CARGO environment variable because we want to use the one in PATH. It
// allows selecting a different toolchain with +nightly
const CARGO_PATH: &str = "cargo";

const CARGO_COMMON: &[&str] = &[
    "+nightly",
    "build",
    "-p",
    "smb2_practice_mod",
    "--target",
    "powerpc-unknown-eabi.json",
    "--release",
];

const CARGO_BASE_RELEASE: &[&str] = &[
    "-Z",
    "build-std=core,alloc,panic_abort",
    "-Z",
    "build-std-features=panic_immediate_abort",
];
const CARGO_BASE_DEBUG: &[&str] = &["-Z", "build-std=core,alloc"];

const RUSTFLAGS_RELEASE: &str = "-Zlocation-detail=none -Zfmt-debug=none";
const RUSTFLAGS_DEBUG: &str = "";

const LINKER_FLAGS: &[&str] = &[
    "-r",
    "-e",
    "_prolog",
    "-u",
    "_prolog",
    "-u",
    "_epilog",
    "-u",
    "_unresolved",
    "--gc-sections",
    "-nostdlib",
    "-m",
    "elf32ppc",
];

enum Profile {
    Debug,
    Release,
}

fn spawn<P: AsRef<Path>>(bin: P, args: &[&str], stdin: &str) -> anyhow::Result<()> {
    let mut cmd = Command::new(bin.as_ref())
        .args(args)
        .stdin(std::process::Stdio::piped())
        .spawn()?;
    if let Some(mut stdin_handle) = cmd.stdin.take() {
        stdin_handle.write_all(stdin.as_bytes())?;
    }
    let status = cmd.wait()?;
    if !status.success() {
        bail!("Build failed");
    }
    Ok(())
}

fn get_rust_host() -> anyhow::Result<String> {
    let output = Command::new(CARGO_PATH)
        .args(["--version", "--verbose"])
        .output()?;
    let version_info = String::from_utf8(output.stdout)?;

    const HOST_PREFIX: &str = "host: ";
    for line in version_info.lines() {
        if let Some(rest) = line.strip_prefix(HOST_PREFIX) {
            return Ok(rest.into());
        }
    }
    bail!("Failed to parse host");
}

fn get_project_root() -> anyhow::Result<PathBuf> {
    let output = Command::new(CARGO_PATH)
        .args(["locate-project", "--workspace"])
        .output()?;
    let output_str = String::from_utf8(output.stdout)?;
    let splits = output_str.split("\"").collect::<Vec<_>>();
    let root_cargo_toml = *splits
        .get(3)
        .ok_or(anyhow!("Could not index project root"))?;
    let toml_path = Path::new(root_cargo_toml);
    let project_path = toml_path.parent().ok_or(anyhow!("Bad project root path"))?;
    Ok(project_path.to_str().unwrap().into())
}

fn build_archive(cargo_base: &[&str], rustflags: &str) -> anyhow::Result<()> {
    let mut args = CARGO_COMMON.to_vec();
    args.extend_from_slice(cargo_base);
    spawn(CARGO_PATH, &args, rustflags)?;
    Ok(())
}

fn create_elf(project_root: &Path) -> anyhow::Result<()> {
    let mut linker = dirs::home_dir().ok_or(anyhow!("Could not locate home directory"))?;
    let rust_host = get_rust_host()?;
    linker.push(format!(
        ".rustup/toolchains/stable-{rust_host}/lib/rustlib/{rust_host}/bin/gcc-ld/ld.lld"
    ));

    let mut archive = project_root.to_path_buf();
    archive.push("target/powerpc-unknown-eabi/release/libsmb2_practice_mod.a");

    let mut linker_script = project_root.to_path_buf();
    linker_script.push("ogc.ld");

    let mut output_elf = project_root.to_path_buf();
    output_elf.push("target/SMB2PracticeMod.elf");

    let mut args = LINKER_FLAGS.to_vec();
    args.push(archive.to_str().unwrap());
    args.extend(["-T", linker_script.to_str().unwrap()]);
    args.extend(["-o", output_elf.to_str().unwrap()]);

    spawn(linker, &args, "")?;
    Ok(())
}

fn main() -> anyhow::Result<()> {
    let mut args = std::env::args();
    let (_, profile) = (args.next(), args.next());
    let profile = match profile.as_deref() {
        None | Some("release") => Profile::Release,
        Some("debug") => Profile::Debug,
        Some(p) => bail!("Invalid profile: {p}"),
    };

    match profile {
        Profile::Release => build_archive(CARGO_BASE_RELEASE, RUSTFLAGS_RELEASE)?,
        Profile::Debug => build_archive(CARGO_BASE_DEBUG, RUSTFLAGS_DEBUG)?,
    }

    let project_root = get_project_root()?;
    create_elf(&project_root)?;

    Ok(())
}
