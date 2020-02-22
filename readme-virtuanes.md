## VirtuaNES for 3DS

VirtuaNES for 3DS is a high compatibility NES emulator for your old 3DS or 2DS. It's not as accurate as FCEUX or Nestopia, but it runs at full 60 FPS for almost all games, and supports tonnes of mappers: MMC1,3,5,6; VRC1,2,3,4,6,7; and tonnes of other mappers. As a result, the library of games it supports are huge. 

This 3DS version also fixes a few bugs from VirtuaNES's MMC5 mappers and even plays Rockman 4 Minus Infinity and Zelda Legend of Link hacks.

It also runs on your New 3DS as well!

### Homebrew Launcher:

1. Copy virtuanes_3ds.3dsx, virtuanes_3ds.smdh and virtuanes_3ds_top.png into the /3ds/virtuanes_3ds folder on your SD card.
2. Place your ROMs inside any folder.
3. Go to your Homebrew Launcher (either via Cubic Ninja, Soundhax or other entry points) and launch the virtuanes_3ds emulator.

### CIA Version:

1. Use your favorite CIA installer to install virtuanes_3ds.cia into your CFW. You can also use FBI to download from TitleDB.
2. Place your ROMs inside any folder.
3. Copy virtuanes_3ds_top.png to ROOT of your SD card.
4. Exit your CIA installer and go to your CFW's home screen to launch the app.

### FDS BIOS

1. To load .FDS games, make sure that you get the FDS BIOS.
2. Rename the BIOS to disksys.rom and place it in /3ds/virtuanes_3ds/bios/disksys.rom

### When in-game,

1. Tap the bottom screen for the menu.
2. Use Up/Down to choose option, and A to confirm. (Hold down X while pressing Up/Down to Page Up/Page Down)
3. Use Left/Right to change between ROM selection and emulator options.
4. You can quit the emulator to your homebrew launcher / your CFW's home screen.

-------------------------------------------------------------------------------------------------------

## VirtuaNES Screenshots

![alt tag](https://github.com/bubble2k16/emus3ds/blob/master/screenshots/VirtuaNES%20-%20Bionic%20Commando.bmp)

![alt tag](https://github.com/bubble2k16/emus3ds/blob/master/screenshots/VirtuaNES%20-%20Gradius%20II.bmp)

![alt tag](https://github.com/bubble2k16/emus3ds/blob/master/screenshots/VirtuaNES%20-%20Kirby's%20Adventure.bmp)

![alt tag](https://github.com/bubble2k16/emus3ds/blob/master/screenshots/VirtuaNES%20-%20Legend%20of%20Link.bmp)

![alt tag](https://github.com/bubble2k16/emus3ds/blob/master/screenshots/VirtuaNES%20-%20Rockman%204%20Minus%20Infinity.bmp)

![alt tag](https://github.com/bubble2k16/emus3ds/blob/master/screenshots/VirtuaNES%20-%20Super%20Mario%20Bros%203.bmp)

-------------------------------------------------------------------------------------------

## Change History

*VirtuaNES:*

### v1.02
- Fixed bug in MMC5 mapper that was causing Castlevania 3's graphics to corrupt.
- Optimized rendering to 16-bit buffer to reduce cache misses, and minor optimizations for MMC5 rendering.
- Fixed occassional crashing bug when loading a ROM.

### v1.01
- Fixed cheats (previously cheats couldn't be activated whatever you did)
- Fixed mapper problem in Kabuki Quantum Fighter

### v1.00
- Added key mappings to fast forward (capped at 180 FPS), and open the emulator menu.
- Updated configuration menu
- Added feature to auto-save state
- Migrated and fixed some mappers from VirtuaNESEx (thanks to the authors of VirtualNESEx) 
- The following mappers are fixed/added: 4, 7, 16, 23, 34, 47, 49, 52, 65, 69, 71, 73, 74, 86, 91, 92, 121, 132, 141, 148, 150, 162, 163, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 189, 190, 194, 198, 199, 212, 235, 237, 240, 241, 252
- Fixed VS mapper for Super Mario Bros
- Added ability to map any 3DS key to insert coin for VS games
- Fixed Lagrange Point VRC7 emulation for the Old 3DS by playing at 20KHz. The old 3DS was too slow to render sounds at 32KHz or even at 22KHz.
- Minor optimizations for sound generation.
- A number of games are fixed due to the mapper updates. Examples are: Crisis Force (Japan), Feng Yun (Ch), San Guo Zhi (Ch), and many others.

### v0.92

- Migrated some mappers from VirtuaNES Up (thank you dragon2snow, and authors of VirtualNES Plus)
- The following mappers are fixed: 0, 15, 16, 25, 27, 58, 62, 74, 83, 111, 163, 187, 193, 194, 199, 241
- Added Mapper 28, 35, 36, 120, 156, 162, 170, 175, 206, 209, 211, 216, 220, 253
- Fixed rendering problem with Mother (J) and Marble Madness
- Detect and auto-patch correct header numbers for certain ROMs (database of corrected mapping for ROMs taken from FCEUX)
- Games that are now working on this version:
  > Final Fantasy VII Demake (both English / Chinese versions)
  > DBZ RPG 2/3 (English)
  > Donkey Kong Country 4
  > Super Contra 3
  > Super Samurai Spirits 2
  > Action 53
  > Probably a few others.
- Added settings to allow players to switch between global/local config for audio, button mapping and rapid fire.

### v0.91

- Enabled support of FDS ROMS without the standard FDS headers.
- Added feature to allow players to switch disks for FDS games.
- Fixed faint vertical lines appearing at the left/right edges of the screen.
- Enabled APU frame counter IRQ (fixes Qix, and probably many others)
- Used 64-bit integer for total cycle counter. This should fix sound problems after running a game for too long.
- Improved sound synchronization - minimizes occurrence of songs going out of tune or just behaving oddly.
- Improved digital sample playback synchronisation. Fixes Skate of Die 2’s intro song.
- Allow mapping of 3DS buttons to the NES Start/Select buttons.
- Added cropped 4:3 Fit and cropped fullscreen modes.
- Fixed some issues with MMC5 mappers when powering up.
- Fixed issues handling of CHR-RAM. 
- Updated MMC5 mapper to handle register $5130 correctly.
- Updated MMC5 mapper to write to XRAM so that it can be executed from CPU.
- Updated MMC5 mapper to save /load state of the updated internal registers.
- Fixes Zelda - Legend of Link and Rockman 4 Minus Infinity MMC5 build.
- Minor optimizations by removing checks for clock tests.
- Fixed Smash TV D-pad problem
- Fixed bug with the saving of the auto-fire rate.
- Fixed bug where the game-specific settings never seemed to get saved.
- Fixed bug where the emulator freezes when you select a ROM after selecting one that fails to load.
- Fixed problem of a full black screen for right-eye when using a New 3DS.
- Fixed bug that causes crashing on certain 3DSes.

### v0.90
- First release.

-------------------------------------------------------------------------------------------------------

## .CHX Cheat File format

The .CHX is a cheat file format that you can create with any text editor. Each line in the file corresponds to one cheat, and is of the following format:

     [Y/N],[CheatCode],[Name]

1. [Y/N] represents whether the cheat is enabled. Whenever you enable/disable it in the emulator, the .CHX cheat file will be modified to save your changes.
2. [CheatCode] must be an NES Game Genie 6- or 8-character cheat code. An NES Game-Genie code looks like this: **GXOUSUSE**.
3. [Name] is a short name that represents this cheat. Since this will appear in the emulator, keep it short (< 30 characters). 

The .CHX must have the same name as your ROM. This is an example of a .CHX file:

Filename: **8x8 Eyes.CHX**
```
Y,GXOUSUSE,Most attacks won't damage Orin
Y,GXNGNOSE,Most attacks won't damage Cutrus
```

NOTE: If you enable a cheat, and then disable it, you may need to reset the game for the disabling to take effect.

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

1. Norix team for the fantastic well-optimized VirtuaNES emulator
2. Authors of the Citra 3DS Emulator team. Without them, this project would have been extremely difficult.
5. SG6000 for helping with debugging the nasty crashing bug.
6. Fellow forummers on GBATemp for the bug reports and suggestions for improvements.