## Emus for 3DS (PicoDrive Sega Master System / Mega Drive core)

This is a port of notaz's PicoDrive emulator to the old 3DS / 2DS. Although PicoDrive is already highly optimized for ARM processors, if ported as is, it still doesn't run full speed for all Mega Drive games on the old 3DS / 2DS, as it's evident in the RetroArch's version. So this port heavily relies on the 3DS's 2nd core to emulate the FM synthesized music for the YM2612 FM chip to achieve 60 FPS. But the 2nd core on the old 3DS is unfortunately not fast enough to generate the FM synthesized music at a full 44100Hz, so sounds are generated at 30000Hz on an Old 3DS. It sounds ok for many games, except for those that use high-pitched notes or sound samples.

The screen rendering is done completely using the original PicoDrive's ARM processor-optimized renderer. 

You can play Master System and Mega Drive games, but not support CD or 32X games yet. Meanwhile use the Retroarch versions for Sega CD and 32X games. :)

The default maps for the controls are: 
1. 3DS' Y Button -> MD's A Button, 
2. 3DS' B Button -> MD's B Button,
3. 3DS' A Button -> MD's C Button,

This emulator uses the same user interface as VirtuaNES for 3DS, TemperPCE for 3DS, Snes9x for 3DS. It will run better on the New 3DS as usual, where all music and sound samples will be generated at 44100Hz.

https://github.com/bubble2k16/picodrive_3ds/releases

### Homebrew Launcher:

1. Copy picodrive_3ds.3dsx, picodrive_3ds.smdh and picodrive_3ds_top.png into the /3ds/picodrive_3ds folder on your SD card.
2. Place your ROMs inside any folder.
3. Go to your Homebrew Launcher (either via Cubic Ninja, Soundhax or other entry points) and launch the picodrive_3ds emulator.

### CIA Version:

1. Use your favorite CIA installer to install picodrive_3ds.cia into your CFW. You can also use FBI to download from TitleDB.
2. Place your ROMs inside any folder.
3. Copy picodrive_3ds_top.png to ROOT of your SD card.
4. Exit your CIA installer and go to your CFW's home screen to launch the app.

### When in-game,

1. Tap the bottom screen for the menu.
2. Use Up/Down to choose option, and A to confirm. (Hold down X while pressing Up/Down to Page Up/Page Down)
3. Use Left/Right to change between ROM selection and emulator options.
4. You can quit the emulator to your homebrew launcher / your CFW's home screen.

-------------------------------------------------------------------------------------------------------

## PicoDrive Screenshots

![alt tag](https://github.com/bubble2k16/emus3ds/blob/master/screenshots/PicoDrive01.bmp)

![alt tag](https://github.com/bubble2k16/emus3ds/blob/master/screenshots/PicoDrive02.bmp)

![alt tag](https://github.com/bubble2k16/emus3ds/blob/master/screenshots/PicoDrive03.bmp)

![alt tag](https://github.com/bubble2k16/emus3ds/blob/master/screenshots/PicoDrive04.bmp)

![alt tag](https://github.com/bubble2k16/emus3ds/blob/master/screenshots/PicoDrive05.bmp)

![alt tag](https://github.com/bubble2k16/emus3ds/blob/master/screenshots/PicoDrive06.bmp)


-------------------------------------------------------------------------------------------

## Change History

### v0.90
- First release.

-------------------------------------------------------------------------------------------------------

## How to Build

The current version can be built in two ways:

###  libctru v1.0.0

You will need:
- devkitARM r45
- libctru v1.0.0
- citro3d v1.0.0

Then build by using *make*.

-------------------------------------------------------------------------------------------------------

## Credits

1. Notaz for his well-optimized PicoDrive emulator
2. Authors of the Citra 3DS Emulator team. Without them, this project would have been extremely difficult.
3. Fellow forummers on GBATemp for the bug reports and suggestions for improvements.