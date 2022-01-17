# Building from Source

You will need a Linux environment. A native Linux install or Windows+WSL works. The following assumes Ubuntu 20.04 LTS.

## Install Linux Dependencies

```sh
sudo apt update
sudo apt install build-essential pkg-config cmake libboost-dev libboost-program-options-dev
```

## Install devkitPro

devkitPro is a gcc-based compiler suite for Gamecube and other consoles.

1. Download devkitPro-pacman from here: https://github.com/devkitPro/pacman/releases
2. Install the downloaded .deb: `sudo dpkg -i devkitpro-pacman.amd64.deb`
3. Install the Gamecube toolchain: `sudo dkp-pacman -S gamecube-dev`
4. Downgrade devkitPPC to r39-2 because the latest version (r40-3 as of this writing) is broken: download [here](https://drive.google.com/file/d/1tMrqjfqRkJNgBidvYJMj48_yuWM8pmUF/view?usp=sharing) and install it with `sudo dkp-pacman -U devkitPPC-r39-2-x86_64.pkg.tar.xz`
5. Source the devkitPro environment: `source /etc/profile.d/devkit-env.sh`

## Clone and Build

```sh
git clone https://github.com/ComplexPlane/ApeSphere.git
cd ApeSphere
make us -j$(nproc) # Builds for vanilla SMB2
# To build versions for all supported SMB2 romhacks as well:
# make -j$(nproc)
```

I recommend using a script to quickly build and copy the rels/gcis to your Dolphin directories. See the `script` directory for examples meant to be placed at the root of the repo and named `go.sh`.