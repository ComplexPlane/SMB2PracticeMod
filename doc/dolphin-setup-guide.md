# ApeSphere Dolphin Setup Guide

## Set up the GCI

* Download the `.zip` file containing the release (_not_ the source code). Releases are found [here](https://github.com/complexplane/apesphere/releases).
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
  * (For older revisions of Dolphin v5, instead go to Properties -> Edit Config and paste the text code into the Gecko codes section of the `GM28EP.ini` file that opens, marked by the text **[Gecko]**; if the section doesn't exist, add it.)
* In the Gecko Codes window, make sure the checkbox next to the Gecko code you just added is checked

## Launch the game

You should be able to launch the game like usual by double-clicking it in the games list. If you see "ApeSphere Practice Mod" on the titlescreen, you're good to go, otherwise the mod did not load correctly. You can use either Card A or Card B for your save data, it doesn't matter.

If the game does not progress past the Presented By Sega screen, try upgrading your version of Dolphin Emulator (download [here](https://dolphin-emu.org/download/)). Using at least the latest beta is recommended.
