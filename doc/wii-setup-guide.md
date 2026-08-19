# Wii Setup Guide

## Install Homebrew Software

You will need a homebrewed Wii with **up-to-date** Nintendont installed.

* Homebrew your Wii using [this guide](https://wii.guide), up to and including installing the Homebrew channel. I
   highly recommend following the Letterbomb homebrew method over other methods, it's super reliable and easy.
* Install [Nintendont](https://github.com/FIX94/Nintendont) on your Wii. **Make sure your Nintendont version is from
   2019 or newer**, older versions may fail to load the practice mod. You can check your version in the Nintendont
   settings screen, or just upgrade to the latest version.
   
## Install the Practice Mod

* From the [releases](https://github.com/ComplexPlane/SMB2PracticeMod/releases) page, download the `.zip` file containing the release (_not_ the source code).
* Plug your homebrew flash drive into your computer (or SD card, hard drive, etc.). Note that if you intend to play ISO files (romhacks etc.), the practice mod must go on the same flash drive as them.
* On your homebrew flash drive, if you have any game data on a virtual memory card in Slot B for Super Monkey Ball 2,
   you're going to want to back it up as the practice mod will replace it. It will be the file `saves/GM2E_B.raw`. If
   you don't have this file or don't know what I'm talking about, you're probably fine.
* Copy everything inside the `Wii Console` folder (but not the `Wii Console` folder itself) onto your flash drive. If asked to replace files, choose Yes.

## Playing Custom Level Packs / Romhacks

To use SMB2 Practice Mod with custom level packs, you need a corresponding `<game_id>.gct` file in the `Wii Console/codes` folder for the pack. The following are included by default:

| Romhack                         | File Name                    |
| ------------------------------- | ---------------------------- |
| Deluxe in SMB2                  | GM2EDX.gct                   |
| Gaiden                          | GM2EGD.gct                   |
| Invasion                        | GM2EII.gct                   |
| Kaizo Monkey Ball               | GM2EKZ.gct                   |
| Launch!                         | GM2ELD.gct                   |
| Monkeyed Ball 2: Witty Subtitle | GM2E8P.gct (same as vanilla) |
| SMB651                          | GM2E8P.gct (same as vanilla) |
| Stardust                        | GM2ESD.gct                   |
| Touch & Roll Remake             | GM2ETR.gct                   |

If your pack isn't in this list, simply copy one of the pre-existing .gct files and rename it to the new pack's ID. If the pack's ID is `GM2EKZ`, you'll want to name the copied file `GM2EKZ.gct`. You can check a pack's ID in Dolphin Emulator or Nintendont (or you can ask in the [workshop server](https://discord.gg/CEYjvDj)!)

## Boot the Game

* Plug your SD card back into your Wii and boot up Nintendont.
* In Nintendont, press B to go to options. **Enable** Cheats, Unlock Read Speed, Memcard Emulation, and Native
   Control. **Disable** Memcard Multi.
* Boot the game from disc or ISO file like usual.

If you see the "SMB2 Practice Mod" text in place of the normal "Press Start" text on the title screen when you start the
game, you've loaded the mod successfully.

## Troubleshooting

If you don't see the "SMB2 Practice Mod" text, the mod is definitely not loaded correctly, go back and check to make sure you didn't miss a step. If you can't figure it out, feel free to message me on
Discord (`complexplane`) or email (`complexplanegames@gmail.com`), or [file a GitHub issue](https://github.com/ComplexPlane/SMB2PracticeMod/issues).

One person reported having trouble loading the practice mod when there were other memory card savefiles in `saves/` besides `GM2E_B.raw`. If you're having trouble, try putting only `GM2E_B.raw` in your `saves/` folder. I'm not sure exactly what circumstances cause this issue, let me know if you run into it so I can take a closer look.
