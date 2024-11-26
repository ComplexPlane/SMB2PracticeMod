use std::io::Write;
use std::path::Path;
use std::{path::PathBuf, process::Command};

use anyhow::bail;
use anyhow::{anyhow, Context};
use gamecube_tools::elf2rel::RelVersion;

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

    let archive = project_root.join("target/powerpc-unknown-eabi/release/libsmb2_practice_mod.a");
    let linker_script = project_root.join("ogc.ld");
    let output_elf = project_root.join("target/SMB2PracticeMod.elf");

    let mut args = LINKER_FLAGS.to_vec();
    args.push(archive.to_str().unwrap());
    args.extend(["-T", linker_script.to_str().unwrap()]);
    args.extend(["-o", output_elf.to_str().unwrap()]);

    spawn(linker, &args, "")?;
    Ok(())
}

fn create_rel(project_root: &Path) -> anyhow::Result<()> {
    let input_elf = project_root.join("target/SMB2PracticeMod.elf");
    let elf_buf = std::fs::read(&input_elf).context("Failed to read ELF")?;

    let input_symbol_map = project_root.join("crates/smb2_practice_mod/src/mkb2.us.lst");
    let symbol_map_buf = std::fs::read(&input_symbol_map).context("Failed to read symbol map")?;

    let rel_buf = gamecube_tools::elf2rel::elf2rel(&elf_buf, &symbol_map_buf, 101, RelVersion::V2)?;
    let output_rel = project_root.join("target/SMB2PracticeMod.rel");
    std::fs::write(&output_rel, &rel_buf).context("Failed to write REL")?;

    Ok(())
}

fn create_gci(project_root: &Path) -> anyhow::Result<()> {
    let input_rel = project_root.join("target/SMB2PracticeMod.rel");
    let rel_buf = std::fs::read(&input_rel).context("Failed to read REL")?;

    let input_banner = project_root.join("images/banner_us.raw");
    let banner_buf = std::fs::read(&input_banner).context("Failed to read banner")?;

    let input_icon = project_root.join("images/icon_us.raw");
    let icon_buf = std::fs::read(&input_icon).context("Failed to read icon")?;

    let gci_buf = gamecube_tools::gcipack::gcipack(
        &rel_buf,
        "rel",
        "Super Monkey Ball 2",
        "SMB2 Practice Mod",
        &banner_buf,
        &icon_buf,
        "GM2E8P",
    )?;
    let output_gci = project_root.join("target/SMB2PracticeMod.gci");

    std::fs::write(output_gci, &gci_buf).context("Failed to write GCI")?;

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
    create_rel(&project_root)?;
    create_gci(&project_root)?;

    Ok(())
}
