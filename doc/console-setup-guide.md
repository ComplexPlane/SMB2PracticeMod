# Console Setup Guide

1. Homebrew your Wii using [this guide](https://wii.guide), up to and including installing the Homebrew channel. I
   highly recommend following the Letterbomb homebrew method over other methods, it's super reliable and easy.
2. Install [Nintendont](https://github.com/FIX94/Nintendont) on your Wii. **Make sure your Nintendont version is from
   2019 or newer**, older versions may fail to load the practice mod. You can check your version in the Nintendont
   settings screen, or just upgrade to the latest version.
3. Download the `.zip` file containing the release (_not_ the source code). Releases are
   found [here](https://github.com/complexplane/apesphere/releases). Open up the `.zip` file (you don't necessarily need
   to extract it).
4. Plug in your SD card, flash drive, hard drive, etc. which has your homebrew files on it. If you intend to play a ISO
   file, the practice mod must go on the same SD card/flash drive/etc. as the ISO file.
5. On your homebrew SD card, if you have any game data on a virtual memory card in Slot B for Super Monkey Ball 2,
   you're going to want to back it up as the practice mod will replace it. It will be the file `saves/GM2E_B.raw`. If
   you don't have this file or don't know what I'm talking about, you're probably fine.
6. Drag the `codes` and `saves` folders from the `.zip` release you downloaded to your SD card. If asked to replace
   files, choose Yes.
7. If you plan to play a romhack, make sure you have a `<game_id>.gct` file in the `codes` folder with a `<game_id>` that matches the game ID of your romhack. The `codes` folder includes files for some popular romhacks (like `GM2EGD` for Gaiden), but if your romhack isn't included, make a copy of one of the `.gct` files (they're all identical) and name it appropriately. You can find the game ID of your romhack by opening the ISO file in a hex editor such as [hxd](https://mh-nexus.de/en/hxd/) - it's the first 6 characters of the file.
8. Plug your SD card back into your Wii and boot up Nintendont.
9. In Nintendont, press B to go to options. **Enable** Cheats, Unlock Read Speed, Memcard Emulation, and Native
   Control. **Disable** Memcard Multi.
10. Boot the game from disc or ISO file like usual.

If you see the "SMB2 Practice Mod" text in place of the normal "Press Start" text on the title screen when you start the
game, you've loaded the mod successfully. If you don't see this, the mod is definitely not loaded correctly, go back and check to make sure you didn't miss a step. If you can't figure it out, feel free to message me on
Discord (`ComplexPlane#3111`) or email (`complexplanegames@gmail.com`), or file a Github issue.

## Troubleshooting

One person reported having trouble loading the practice mod when there were other memory card savefiles in `saves/` besides `GM2E_B.raw`. If you're having trouble, try putting only `GM2E_B.raw` in your `saves/` folder. I'm not sure exactly what circumstances cause this issue, let me know if you run into it so I can take a closer look.
