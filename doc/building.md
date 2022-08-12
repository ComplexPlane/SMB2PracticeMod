# Building from Source

You will need a Linux environment. A native Linux install or Windows+WSL works. The following assumes Ubuntu 22.04 LTS.

## Install Linux Dependencies

```sh
sudo apt update
sudo apt install build-essential pkg-config cmake libboost-dev libboost-program-options-dev
```

## Install devkitPro

devkitPro is a gcc-based compiler suite for GameCube and other consoles. Follow [these instructions](https://devkitpro.org/wiki/devkitPro_pacman) to install devkitPro pacman, followed by the `gamecube-dev` package group.

## Clone and Build

```sh
git clone https://github.com/ComplexPlane/ApeSphere.git
cd ApeSphere
make us -j # Builds for vanilla SMB2
# To build versions for all supported SMB2 romhacks as well:
# make -j
```

I recommend using a script to quickly build and copy the rels/gcis to your Dolphin directories. See the `script` directory for examples meant to be placed at the root of the repo and named `go.sh`.
