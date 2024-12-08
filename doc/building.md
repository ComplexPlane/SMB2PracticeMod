# Building from Source

Building is supported on macOS, Windows, and Linux/WSL.

## Install Dependencies

You will need:

1) Git
2) System-native C++ toolchain (needed for linking Rust binaries)
3) LLVM (needed for generating Rust -> C bindings for SMB2)

### macOS

Install Xcode command line tools:

```sh
xcode-select --install
```

Follow the [`bindgen` instructions for installing LLVM](https://rust-lang.github.io/rust-bindgen/requirements.html).

### Windows

Install [Git for Windows](https://git-scm.com/downloads/win).

Follow the [`bindgen` instructions for installing LLVM](https://rust-lang.github.io/rust-bindgen/requirements.html). On Windows on ARM, you may need to use the installer directly instead of `winget`.

Set the `LIBCLANG_PATH` [environment variable](https://www.alphr.com/environment-variables-windows-10/) to `C:\Program Files\LLVM\lib` (or to wherever you installed LLVM).

### Ubuntu 24.04

Install GCC and LLVM:

```sh
sudo apt update
sudo apt install build-essential libclang-dev
```

## Install Rust

[Install the base Rust toolchain](https://www.rust-lang.org/tools/install).

On Windows, follow the installer's instructions for installing Visual Studio correctly.

## Clone Source

```sh
git clone https://github.com/ComplexPlane/SMB2PracticeMod.git
cd SMB2PracticeMod
```

## Build It

Run one of:

- `cargo xtask`: Standard build that produces `target/SMB2PracticeMod.gci`.
- `cargo xtask debug`: Build that includes file/line/column panic debug information. This is very helpful for debugging crashes, but isn't the default as it significantly increases the binary size.

If you get an error when the nightly toolchain is installing, you may need to remove some conflicting nightly toolchains and try again. Use `rustup toolchain list` to list the toolchains, and `rustup toolchain uninstall <toolchain>` to remove them.
