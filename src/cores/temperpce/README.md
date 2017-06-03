======================
Temper for GCW0
======================

Temper was originally a PC-Engine emulator by Exophase,
originally made for the GP2X and then ported to other devices.

Here's an improved version of Temper for the GCW0.
Compared to the Dingoo version, this one has :
- Improved sound quality
- Better color reproduction, compared to the old version.
- Triple-buffering support, which means no tearing when the game scrolls.
- Takes advantage of the more powerful processor.
- Auto-Detection for Supergrafx games.

And more...

To install the emulator, put the opk in /media/data/apps.
Alternatively, you can also put the syscards bios in /media/home/.temper/syscards.
Name it syscard3.pce and you're ready to play CD games !


============
FAQ
============

Q :  I can't play my CD games ? It throws me back to the menu !
A : First of all, make sure you pick up the cue file, not the iso file.
Check the cue file and see if the path to the iso is correct.
Check the case of the iso file as Temper is case-sensitive !

Make sure the default transfer type is set to 'Binary' if you use Filezilla.
If not, Filezilla will switch to 'ASCII' when transferring ISO files and you'll end up 
with a corrupted file...

Don't forget to put the syscard file in /media/home/.temper/syscards !
It needs to be called either syscard1.pce, syscard2.pce, syscard3.pce
or games_express.pce if you play Games Express games. 
(Only for some japanese porn games)

For Arcade CD games, you need to go to the menu.
Then Change Options -> CD-Rom System and select "acd".
You will then be able to play Arcade CD games.

Q: I want to customise Controls, is it possible ?
A : Yes, simply go to Configure pad in the menu and change the controls to your liking.
Note that rapid fire at this time is not yet suported.