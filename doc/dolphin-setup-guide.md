# ApeSphere Dolphin Setup Guide

## Set up the GCI

* Download the file ending in `.gci` from the most recent release (found [here](https://github.com/complexplane/apesphere/releases)
* Open up Dolphin and go to Config -> Gamecube. Under "Device Settings" for _Slot B_ choose _GCI Folder_.
* Copy the `.gci` file you downloaded into the appropriate folder, which you may need to create yourself if it doesn't exist
  * On Windows the folder is `Documents\Dolphin Emulator\GC\USA\Slot B\`
  * On Linux/macOS the folder is `~/.local/share/dolphin-emu/GC/USA/Slot B/`

## Set up the Gecko code

* In Dolphin, under Config -> General, make sure cheats are enabled
* Copy the contents of `REL_Loader_US_Gecko.txt` to your clipboard, found under the latest release [here](https://github.com/complexplane/apesphere/releases)
* Right-click your Super Monkey Ball 2 game in the games list and go to Properties -> Gecko Codes
* At the bottom click "Add New Code", paste the Gecko code into the "Code" box, then click Save
  * (For older revisions of Dolphin v5, instead go to Properties -> Edit Config and paste the text code into the Gecko codes section of the "GM28EP.ini" file that opens, marked by the text **[Gecko]**; if the section doesn't exist, add it.)
* In the Gecko Codes window, make sure the checkbox next to the Gecko code you just added is checked

## Launch the game

You should be able to launch the game like usual by double-clicking it in the games list. If you see "ApeSphere Practice Mode" on the titlescreen, you're good to go, otherwise the mod did not load correctly.