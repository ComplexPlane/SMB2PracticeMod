# Dolphin Setup Guide

## Install Dolphin

Using the latest stable release of [Dolphin Emulator](https://dolphin-emu.org/download/) is highly recommended.

## Set up the GCI

* From the [releases](https://github.com/ComplexPlane/SMB2PracticeMod/releases) page, download the `.zip` file containing the release (_not_ the source code).
* Open up Dolphin and go to _Config -> Gamecube_. Under _Device Settings_ for _Slot B_ choose _GCI Folder_
* Inside the `Dolphin Emulator` folder from the release, locate the `SMB2PracticeMod.gci` file. Copy it into the following folder, which you may need to create yourself if it doesn't exist:
  * **Windows:** `AppData\Roaming\Dolphin Emulator\GC\USA\Card B\`
  * **macOS:** `~/Library/Application Support/Dolphin/GC/USA/Card B/`
  * **Linux:** `~/.local/share/dolphin-emu/GC/USA/Card B/`

## Set up the Gecko code

* In Dolphin, under _Config -> General_, make sure cheats are enabled
* Copy the contents of `Dolphin Emulator/REL_Loader_US_Gecko.txt` from the release to your clipboard
* Right-click your Super Monkey Ball 2 game in the games list and go to _Properties -> Gecko Codes_
* At the bottom click _Add New Code_, paste the Gecko code into the _Code_ box, then click _Save_
* In the Gecko Codes window, make sure the checkbox next to the Gecko code you just added is checked

## Launch the game

You should be able to launch the game like usual by double-clicking it in the games list. If you see "SMB2 Practice Mod"
on the titlescreen, you're good to go, otherwise the mod did not load correctly. You can use either Card A or Card B for
your game save data, it doesn't matter.

## Troubleshooting

If the game does not progress past the Presented By Sega screen, try upgrading your version of [Dolphin Emulator](https://dolphin-emu.org/download/). Using the latest stable release is recommended.

If the game doesn't boot or crashes, it may be worth removing your Dolphin data directory (`AppData\Roaming\Dolphin Emulator` or equivalent) and retrying to see if bad Dolphin data/settings is involved.

If you still can't figure it out, feel free to message me on Discord (`complexplane`) or
email (`complexplanegames@gmail.com`), or file a GitHub issue.
