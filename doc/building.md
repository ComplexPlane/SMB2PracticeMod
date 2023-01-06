# Building from Source

You will need a Linux environment. A native Linux install or Windows+WSL works. The following assumes Ubuntu 22.04 LTS.

## Install Linux Dependencies

```sh
sudo apt update
sudo apt install build-essential pkg-config cmake libboost-dev libboost-program-options-dev
```

## Install devkitPro

devkitPro is a gcc-based compiler suite for GameCube and other consoles.

To install it on Ubuntu 22.04:

```sh
wget https://apt.devkitpro.org/install-devkitpro-pacman
chmod +x ./install-devkitpro-pacman
sudo ./install-devkitpro-pacman
sudo dkp-pacman -S gamecube-dev
```

Then add `export DEVKITPPC=/opt/devkitpro/devkitPPC` to your `~/.bashrc`.

See [here](https://devkitpro.org/wiki/devkitPro_pacman) for full details.

## Clone and Build

```sh
git clone https://github.com/ComplexPlane/SMB2PracticeMod.git
cd SMB2PracticeMod
make -j
```

I recommend using a script to quickly build and copy the rels/gcis to your Dolphin directories. See `scripts/go.sh` for an example meant to be placed at the root of the repo as `go.sh`.

## VSCode Editor Setup

Here's how to set up VSCode with clangd. Clangd provides error checking, autocompletion, formatting, and more. It's faster and more accurate than the Microsoft C++ extension in my experience.

1. Run: `scripts/gen_compile_commands.py` to generate `compile_commands.json` which clangd needs.
    - You ought to run this every time you add a new `.cpp` file but it may work fine if you forget.
2. Install the clangd extension. If you see popups for disabling the conflicting Microsoft C++ Intellisense or downloading clangd, go ahead and comply.
3. Restart your editor and enjoy!

I also like to enable "autosave on focus change" and "format on save". Clangd is used for formatting with the style defined in `.clang-format`.
