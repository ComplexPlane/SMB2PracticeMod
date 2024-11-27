use std::io::Write;
use std::path::Path;
use std::{path::PathBuf, process::Command};

use anyhow::bail;
use anyhow::{anyhow, Context};
use gamecube_tools::elf2rel::RelVersion;

// We don't read from the CARGO environment variable because we want to use the one in PATH. It
// allows selecting a different toolchain with +nightly
const CARGO_PATH: &str = "cargo";

const CARGO_COMMON: &[&str] = &["+nightly", "build", "-p", "smb2_practice_mod", "--release"];

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

struct BuildPaths {
    linker: PathBuf,
    target_config: PathBuf,
    archive: PathBuf,
    linker_script: PathBuf,
    elf: PathBuf,
    symbol_map: PathBuf,
    rel: PathBuf,
    banner: PathBuf,
    icon: PathBuf,
    gci: PathBuf,
}

fn get_build_paths() -> anyhow::Result<BuildPaths> {
    let home_dir = dirs::home_dir().ok_or(anyhow!("Failed to determine home directory"))?;
    let project_root = get_project_root()?;
    let rust_host = get_rust_host()?;

    let linker = home_dir.join(format!(
        ".rustup/toolchains/stable-{rust_host}/lib/rustlib/{rust_host}/bin/gcc-ld/ld.lld"
    ));

    let target_config = project_root.join("powerpc-unknown-eabi.json");
    let archive = project_root.join("target/powerpc-unknown-eabi/release/libsmb2_practice_mod.a");
    let linker_script = project_root.join("ogc.ld");
    let elf = project_root.join("target/SMB2PracticeMod.elf");
    let symbol_map = project_root.join("crates/smb2_practice_mod/src/mkb2.us.lst");
    let rel = project_root.join("target/SMB2PracticeMod.rel");
    let banner = project_root.join("images/banner_us.raw");
    let icon = project_root.join("images/icon_us.raw");
    let gci = project_root.join("target/SMB2PracticeMod.gci");

    Ok(BuildPaths {
        linker,
        target_config,
        archive,
        linker_script,
        elf,
        symbol_map,
        rel,
        banner,
        icon,
        gci,
    })
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

fn build_archive(
    cargo_base: &[&str],
    rustflags: &str,
    build_paths: &BuildPaths,
) -> anyhow::Result<()> {
    let mut cmd = Command::new(CARGO_PATH)
        .args(CARGO_COMMON)
        .args(cargo_base)
        .args(["--target", build_paths.target_config.to_str().unwrap()])
        .env("RUSTFLAGS", rustflags)
        .spawn()?;
    if !cmd.wait()?.success() {
        bail!("Failed to build archive");
    }
    Ok(())
}

fn create_elf(build_paths: &BuildPaths) -> anyhow::Result<()> {
    let mut cmd = Command::new(&build_paths.linker)
        .args(LINKER_FLAGS)
        .arg(&build_paths.archive)
        .args(["-T", build_paths.linker_script.to_str().unwrap()])
        .args(["-o", build_paths.elf.to_str().unwrap()])
        .spawn()?;
    if !cmd.wait()?.success() {
        bail!("Linker failed");
    }

    Ok(())
}

fn create_rel(build_paths: &BuildPaths) -> anyhow::Result<()> {
    let elf_buf = std::fs::read(&build_paths.elf).context("Failed to read ELF")?;

    let symbol_map_buf =
        std::fs::read(&build_paths.symbol_map).context("Failed to read symbol map")?;

    let rel_buf = gamecube_tools::elf2rel::elf2rel(&elf_buf, &symbol_map_buf, 101, RelVersion::V2)?;
    std::fs::write(&build_paths.rel, &rel_buf).context("Failed to write REL")?;

    Ok(())
}

fn create_gci(build_paths: &BuildPaths) -> anyhow::Result<()> {
    let rel_buf = std::fs::read(&build_paths.rel).context("Failed to read REL")?;
    let banner_buf = std::fs::read(&build_paths.banner).context("Failed to read banner")?;
    let icon_buf = std::fs::read(&build_paths.icon).context("Failed to read icon")?;

    let gci_buf = gamecube_tools::gcipack::gcipack(
        &rel_buf,
        "rel",
        "Super Monkey Ball 2",
        "SMB2 Practice Mod",
        &banner_buf,
        &icon_buf,
        "GM2E8P",
    )?;

    std::fs::write(&build_paths.gci, &gci_buf).context("Failed to write GCI")?;

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

    let build_paths = get_build_paths()?;

    match profile {
        Profile::Release => build_archive(CARGO_BASE_RELEASE, RUSTFLAGS_RELEASE, &build_paths)?,
        Profile::Debug => build_archive(CARGO_BASE_DEBUG, RUSTFLAGS_DEBUG, &build_paths)?,
    }

    create_elf(&build_paths)?;
    create_rel(&build_paths)?;
    create_gci(&build_paths)?;

    Ok(())
}
