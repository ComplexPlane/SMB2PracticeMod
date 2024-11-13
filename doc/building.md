# Building from Source

You will need a Unix environment, such as macOS, Linux, or Windows+WSL2.

## Install Dependencies

### macOS

Install Xcode command line tools:

```sh
xcode-select --install
```

[Install Homebrew](https://brew.sh/), then install some dependencies:

```sh
brew install cmake boost python3
exec zsh
```

### Ubuntu 24.04

Install some dependency packages:

```sh
sudo apt update
sudo apt install build-essential pkg-config cmake libboost-dev libboost-program-options-dev
```

## Install devkitPro

devkitPro is a gcc-based compiler suite for GameCube and other consoles. We currently only use it for linking.

Install devkitPro by following the [devkitPro Getting Started](https://devkitpro.org/wiki/Getting_Started) page.

Once `dkp-pacman` is installed, install the GameCube devkitPPC compiler suite:

```sh
sudo dkp-pacman -S gamecube-dev
```

Then add `export DEVKITPPC=/opt/devkitpro/devkitPPC` to your `~/.bashrc` (Ubuntu) or `~/.zshrc` (macOS). Restart your shell with `exec bash` / `exec zsh`.

## Install Rust

[Install the base Rust toolchain](https://www.rust-lang.org/tools/install).

Next, install Rust Nightly and Rust sources:

```sh
rustup toolchain install nightly
rustup component add rust-src --toolchain nightly
```

## Clone Source

```sh
git clone https://github.com/ComplexPlane/SMB2PracticeMod.git --recursive
cd SMB2PracticeMod
```

## Build Main Binary

Run one of:

- `make`: Standard build that produces `SMB2PracticeMod.gci`.
- `make debug`: Build that includes file/line/column panic debug information. This is very helpful for debugging crashes, but isn't the default as it significantly increases the binary size.
- `make fix`: Auto-fix problems in the code like unused imports.

## Build Gecko Codes

Building the Gecko codes from source requires using some Windows programs. Please tell me if you know of another way.

To generate the Gecko code text:

- Get CodeWrite: https://github.com/TheGag96/CodeWrite
- Use the insertion address noted at the top of the Gecko source
- Paste in the assembly and generate Gecko code
- Prepend this line to generated code: `040044C0 72656C00`

To turn the Gecko code text into a .gct for use with Nintendont:

- Get Gecko Code Manager: https://www.zeldacodes.org/downloads/code-manager
- Use GMBE8P as the gamecode for SMB1, and GM2E8P for SMB2
- Add a code, paste in the Gecko code,
- Save modifications
- Export to GCT
