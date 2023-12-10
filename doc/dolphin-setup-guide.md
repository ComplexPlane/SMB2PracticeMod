# Dolphin Setup Guide

## Install Dolphin

Using the latest **Beta** version of [Dolphin Emulator](https://dolphin-emu.org/download/) is highly recommended. Dolphin 5.0 (legacy release) is many years out of date, and _Development_ versions are less tested than Beta releases.

## Set up the GCI

* Download the `.zip` file containing the release (_not_ the source code). Releases are found [here](https://github.com/ComplexPlane/SMB2PracticeMod/releases).
* Open up Dolphin and go to _Config -> Gamecube_. Under _Device Settings_ for _Slot B_ choose _GCI Folder_.
* Copy the `.gci` files in the release into the appropriate folder, which you may need to create yourself if it doesn't exist
  * On Windows the folder is `Documents\Dolphin Emulator\GC\USA\Card B\`
  * On Linux/macOS the folder is `~/.local/share/dolphin-emu/GC/USA/Card B/`

## Set up the Gecko code

* In Dolphin, under _Config -> General_, make sure cheats are enabled
* Copy the contents of `REL_Loader_US_Gecko.txt` to your clipboard (found in the `.zip` release you downloaded)
  * Don't copy the first line that starts with `$`
* Right-click your Super Monkey Ball 2 game in the games list and go to _Properties -> Gecko Codes_
* At the bottom click _Add New Code_, paste the Gecko code into the _Code_ box, then click _Save_
  * Make sure the first line that starts with `$` isn't included
  * (For older revisions of Dolphin v5, instead go to Properties -> Edit Config and paste the text code into the Gecko
    codes section of the `GM28EP.ini` file that opens, marked by the text **[Gecko]**; if the section doesn't exist, add
    it.)
* In the Gecko Codes window, make sure the checkbox next to the Gecko code you just added is checked

## Launch the game

You should be able to launch the game like usual by double-clicking it in the games list. If you see "SMB2 Practice Mod"
on the titlescreen, you're good to go, otherwise the mod did not load correctly. You can use either Card A or Card B for
your save data, it doesn't matter.

## Troubleshooting

If the game does not progress past the Presented By Sega screen, try upgrading your version of [Dolphin Emulator](https://dolphin-emu.org/download/). Using the latest **Beta** version is recommended.

If the game doesn't boot or crashes, it may be worth removing your Dolphin data directory (`Documents\Dolphin Emulator` or equivalent) and retrying to see if bad Dolphin data/settings is involved.

If you still can't figure it out, feel free to message me on Discord (`complexplane`) or
email (`complexplanegames@gmail.com`), or file a GitHub issue.
