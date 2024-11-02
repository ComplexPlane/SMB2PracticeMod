# Building from Source

You will need a Unix environment, such as macOS, Linux, or Windows+WSL2.

## Install Dependencies

### macOS

Install Xcode command line tools:

```sh
xcode-select --install
```

[Install homebrew](https://brew.sh/), then install some dependencies:

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

Install Rust Nightly and Rust sources:

```sh
rustup toolchain install nightly
rustup component add rust-src --toolchain nightly
```

## Clone Source

```sh
git clone https://github.com/ComplexPlane/SMB2PracticeMod.git
cd SMB2PracticeMod
```

## Build It

Run `make` to produce `SMB2PracticeMod.gci`.

For a build that includes file/line/column panic debug information, run `make debug`. This info isn't included by default as it significantly increases the binary size.
