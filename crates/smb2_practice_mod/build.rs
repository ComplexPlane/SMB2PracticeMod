use std::process::Command;

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

    println!("cargo:rustc-env=GIT_HASH={git_hash}");
    // Rerun on every build so Git info is always accurate.
    // This does cost some warm rebuild time
    println!("cargo:rerun-if-changed=nonexistent_file");
}

fn main() {
    generate_git_hash();
}
