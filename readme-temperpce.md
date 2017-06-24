## Emus for 3DS (TemperPCE TurboGrafx/PC-Engine core)

This is a port of Exophase's Temper (TurboGrafx/PC-Engine) emulator to the old 3DS and old 2DS. This port heavily relies on the 3DS's 3D GPU hardware to achieve 60 FPS (or close to 60 with frame skips). Since we are using the hardware, some games that utilise special palette effects may not work so well.

This emulator bears the same user interface as VirtuaNES for 3DS and Snes9x for 3DS. It should run better on the New 3DS as usual.

https://github.com/bubble2k16/temperpce_3ds/releases

### Homebrew Launcher:

1. Copy temperpce_3ds.3dsx, temperpce_3ds.smdh and temperpce_3ds_top.png into the /3ds/temperpce_3ds folder on your SD card.
2. Place your ROMs inside any folder.
3. Go to your Homebrew Launcher (either via Cubic Ninja, Soundhax or other entry points) and launch the temperpce_3ds emulator.

### CIA Version:

1. Use your favorite CIA installer to install temperpce_3ds.cia into your CFW. You can also use FBI to download from TitleDB.
2. Place your ROMs inside any folder.
3. Copy temperpce_3ds_top.png to ROOT of your SD card.
4. Exit your CIA installer and go to your CFW's home screen to launch the app.

### CD-ROM BIOS

1. If you have the CD ROM BIOS, place them in the /3ds/temperpce_3ds/syscards folder.
2. They must be named:
   - syscard1.pce (version 1),
   - syscard2.pce (version 2),
   - syscard3.pce (version 3), or
   - games_express.pce (for Games Express).
3. You can have all four in the /3ds/temperpce_3ds/syscards folder. In the settings screen, you can choose which ROM version you want to use.


### When in-game,

1. Tap the bottom screen for the menu.
2. Use Up/Down to choose option, and A to confirm. (Hold down X while pressing Up/Down to Page Up/Page Down)
3. Use Left/Right to change between ROM selection and emulator options.
4. You can quit the emulator to your homebrew launcher / your CFW's home screen.

-------------------------------------------------------------------------------------------------------

## TemperPCE Screenshots

![alt tag](https://github.com/bubble2k16/emus3ds/blob/master/screenshots/TemperPCE01.bmp)

![alt tag](https://github.com/bubble2k16/emus3ds/blob/master/screenshots/TemperPCE02.bmp)

![alt tag](https://github.com/bubble2k16/emus3ds/blob/master/screenshots/TemperPCE03.bmp)

![alt tag](https://github.com/bubble2k16/emus3ds/blob/master/screenshots/TemperPCE04.bmp)

![alt tag](https://github.com/bubble2k16/emus3ds/blob/master/screenshots/TemperPCE05.bmp)

![alt tag](https://github.com/bubble2k16/emus3ds/blob/master/screenshots/TemperPCE06.bmp)


-------------------------------------------------------------------------------------------

## Change History


### v0.91
- Fixed frame rate issues with CD-ROM games speeding up to 100-200 FPS.
- Fixed skipping CD music
- Improved overall CD/ADPCM synchronization.
- Added option to select optimized CPU core (less compatible)

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

1. Exophase for the well-optimized his Temper TG16/PCE emulator
2. Authors of the Citra 3DS Emulator team. Without them, this project would have been extremely difficult.
3. Fellow forummers on GBATemp for the bug reports and suggestions for improvements.