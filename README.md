# :confetti_ball: ApeSphere Practice Mod

A Super Monkey Ball 2 mod for speedrun practice and general experimentation. It supports vanilla SMB2 as well as most romhacks, including Gaiden, Deluxe in 2, SMB651, Monkeyed Ball 2, and Community Pack 2020.

Features include:

* Console savestates
* An RTA timer that tracks time spent paused
* An individual-world practice mode
* Frame advance

...and more to come!

# Setup

To set up the mod on a **Wii console** (recommended), either follow this [video guide](https://www.youtube.com/watch?v=BwI_HtiMUiQ&list=PL7BL-I_VX5tNeBwc7jlweIXvf59QG_dMG&index=1) or this [text guide](/doc/console-setup-guide.md). _Note:_ you only need to copy the `codes` and `saves` folders in the `.zip` to your SD card, not the other files.

To set up the mod on **Dolphin Emulator**, follow [this text guide](/doc/dolphin-setup-guide.md). 

# Usage

Press L+R at the same time on your controller to toggle the in-game help menu. For individual-world practice, head to Story Mode and press Up/Down on a savefile to change world.

For a more in-depth guide, check out this [basic usage guide video](https://www.youtube.com/watch?v=dfeA8ruJx18&list=PL7BL-I_VX5tNeBwc7jlweIXvf59QG_dMG&index=2).

If you have any questions, message me on Discord (`ComplexPlane#3111`).

Be sure to look for the Easter egg!

# Credits

Thank you to:

**Zephiles** for their numerous code contributions. These include:

* The initial [TTYD-Practice-Codes](https://github.com/Zephiles/TTYD-Practice-Codes) framework of which this mod is based on
* The current REL loader Gecko code
* A custom heap implementation for the mod
* A revised REL loader which serves as both a Gecko and ISO loader (not yet included)
* A Super Monkey Ball 1 port of the REL loader (not yet included)

**PistonMiner** for their [ttyd-tools](https://github.com/PistonMiner/ttyd-tools) framework, which provides everything you need to turn C++ into a .GCI savefile which can be loaded into a Gamecube game at runtime. In addition, thanks very much for the detailed answers to my romhacking-related questions.

**CraftedCart** for their substantial contributions to the Monkey Ball 2 decompile project, as well as the many large contributions to the Monkey Ball hacking and custom level scene on the whole.

**Yoshimaster96** for labelling hundreds of functions and data in Super Monkey Ball 1, many of which were ported to Super Monkey Ball 2.

**TheBombSquad** for their decompile contributions as well, and for writing the current version of the ISO REL loader.

**Alist** and **PetresInc** for providing insightful feedback on a pre-release alpha needed before publishing an initial beta version of the mod.

**The Monkey Ball Speedrunning Scene** for inspiring this mod! Thank you for being so fun to be a part of.
