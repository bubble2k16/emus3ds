#include "VsUnisystem.h"

// デフォルトDIP-SW
VSDIPSWITCH	vsdip_default[] = {
//	Dipname				mask&value
	"Unknown",			0x0100,
	"Off",				0x00,
	"On",				0x01,
	NULL,				0xFF,
//	Dipname				mask&value
	"Unknown",			0x0200,
	"Off",				0x00,
	"On",				0x02,
	NULL,				0xFF,
//	Dipname				mask&value
	"Unknown",			0x0400,
	"Off",				0x00,
	"On",				0x04,
	NULL,				0xFF,
//	Dipname				mask&value
	"Unknown",			0x0800,
	"Off",				0x00,
	"On",				0x08,
	NULL,				0xFF,
//	Dipname				mask&value
	"Unknown",			0x1000,
	"Off",				0x00,
	"On",				0x10,
	NULL,				0xFF,
//	Dipname				mask&value
	"Unknown",			0x2000,
	"Off",				0x00,
	"On",				0x20,
	NULL,				0xFF,
//	Dipname				mask&value
	"Unknown",			0x4000,
	"Off",				0x00,
	"On",				0x40,
	NULL,				0xFF,
//	Dipname				mask&value
	"Unknown",			0x8000,
	"Off",				0x00,
	"On",				0x80,
	NULL,				0xFF,
// デリミタ
	NULL,				0,
};

// VS TKO Boxing
VSDIPSWITCH	vsdip_tkoboxing[] = {
//	Dipname				mask&value
	"Coin",				0x0300,
	"1 Coin  / 1 Credit",		0x00,
	"1 Coin  / 2 Credits",		0x01,
	"2 Coins / 1 Credit",		0x02,
	"3 Coins / 1 Credit",		0x03,
	NULL,				0xFF,
//	Dipname				mask&value
	"Unknown",			0x0400,
	"Off",				0x00,
	"On",				0x04,
	NULL,				0xFF,
//	Dipname				mask&value
	"Unknown",			0x0800,
	"Off",				0x00,
	"On",				0x08,
	NULL,				0xFF,
//	Dipname				mask&value
	"Unknown",			0x1000,
	"Off",				0x00,
	"On",				0x10,
	NULL,				0xFF,
//	Dipname				mask&value
	"Color Palette",		0x2020,
	"Normal",			0x20,
	"Wrong",			0x00,
	NULL,				0xFF,
//	Dipname				mask&value
	"Unknown",			0x4000,
	"Off",				0x00,
	"On",				0x40,
	NULL,				0xFF,
//	Dipname				mask&value
	"Unknown",			0x8000,
	"Off",				0x00,
	"On",				0x80,
	NULL,				0xFF,
// デリミタ
	NULL,				0,
};

// VS Atari RBI Baseball
VSDIPSWITCH	vsdip_rbibaseball[] = {
//	Dipname				mask&value
	"Coin",				0x0300,
	"1 Coin  / 1 Credit",		0x00,
	"1 Coin  / 2 Credits",		0x01,
	"2 Coins / 1 Credit",		0x02,
	"3 Coins / 1 Credit",		0x03,
	NULL,				0xFF,
//	Dipname				mask&value
	"Max. 1p/in, 2p/in, Min",	0x0C0C,
	"2, 1, 3",			0x04,
	"2, 2, 4",			0x0C,
	"3, 2, 6",			0x00,
	"4, 3, 7",			0x08,
	NULL,				0xFF,
//	Dipname				mask&value
	"Demo Sounds",			0x1000,
	"Off",				0x10,
	"On",				0x00,
	NULL,				0xFF,
//	Dipname				mask&value
	"Color Palette",		0xE080,
	"Normal",			0x80,
	"Wrong 1",			0x00,
	"Wrong 2",			0x40,
	"Wrong 3",			0x20,
	"Wrong 4",			0xC0,
	NULL,				0xFF,
// デリミタ
	NULL,				0,
};

// VS Duck Hunt
VSDIPSWITCH	vsdip_duckhunt[] = {
//	Dipname				mask&value
	"Coin",				0x0700,
	"1 Coin  / 1 Credit",		0x00,
	"1 Coin  / 2 Credits",		0x04,
	"1 Coin  / 3 Credits",		0x02,
	"2 Coins / 1 Credit",		0x06,
	"3 Coins / 1 Credit",		0x01,
	"4 Coins / 1 Credit",		0x05,
	"5 Coins / 1 Credit",		0x03,
	"Free Play",			0x07,
	NULL,				0xFF,
//	Dipname				mask&value
	"Difficulty",			0x1808,
	"Easy",				0x00,
	"Normal",			0x08,
	"Hard",				0x10,
	"Very Hard",			0x18,
	NULL,				0xFF,
//	Dipname				mask&value
	"Missed par Game",		0x2000,
	"3",				0x00,
	"5",				0x20,
	NULL,				0xFF,
//	Dipname				mask&value
	"Bonus Life",			0xC000,
	"30000pts",			0x00,
	"50000pts",			0x40,
	"80000pts",			0x80,
	"100000pts",			0xC0,
	NULL,				0xFF,
// デリミタ
	NULL,				0,
};

// VS Ninja Jajamaru Kun (J)
VSDIPSWITCH	vsdip_jajamaru[] = {
//	Dipname				mask&value
	"Coin",				0x0700,
	"1 Coin  / 1 Credit",		0x00,
	"1 Coin  / 2 Credits",		0x04,
	"1 Coin  / 3 Credits",		0x02,
	"1 Coin  / 4 Credits",		0x06,
	"3 Coins / 1 Credit",		0x01,
	"4 Coins / 1 Credit",		0x05,
	"5 Coins / 1 Credit",		0x03,
	"Free Play",			0x07,
	NULL,				0xFF,
//	Dipname				mask&value
	"Lives",			0x1800,
	"3",				0x00,
	"4",				0x10,
	"5",				0x08,
	"6",				0x18,
	NULL,				0xFF,
//	Dipname				mask&value
	"Unknown",			0x4000,
	"Off",				0x00,
	"On",				0x40,
	NULL,				0xFF,
//	Dipname				mask&value
	"Demo Sounds",			0x8080,
	"Off",				0x00,
	"On",				0x80,
	NULL,				0xFF,
// デリミタ
	NULL,				0,
};

// VS Tetris
VSDIPSWITCH	vsdip_tetris[] = {
//	Dipname				mask&value
	"Coin",				0x0300,
	"1 Coin  / 1 Credit",		0x00,
	"1 Coin  / 2 Credits",		0x02,
	"2 Coins / 1 Credit",		0x01,
	"3 Coins / 1 Credit",		0x03,
	NULL,				0xFF,
//	Dipname				mask&value
	"Difficulty",			0x0C08,
	"Easy",				0x00,
	"Normal",			0x04,
	"Hard",				0x08,
	"Very Hard",			0x0C,
	NULL,				0xFF,
//	Dipname				mask&value
	"Unknown",			0x1000,
	"Off",				0x00,
	"On",				0x10,
	NULL,				0xFF,
//	Dipname				mask&value
	"Color Palette",		0x6060,
	"Normal",			0x60,
	"Wrong 1",			0x20,
	"Wrong 2",			0x40,
	"Wrong 3",			0x00,
	NULL,				0xFF,
//	Dipname				mask&value
	"Unknown",			0x8000,
	"Off",				0x00,
	"On",				0x80,
	NULL,				0xFF,
// デリミタ
	NULL,				0,
};

// VS Top Gun
VSDIPSWITCH	vsdip_topgun[] = {
//	Dipname				mask&value
	"Coin",				0x0700,
	"1 Coin  / 1 Credit",		0x00,
	"1 Coin  / 2 Credits",		0x04,
	"1 Coin  / 3 Credits",		0x02,
	"2 Coins / 1 Credit",		0x06,
	"3 Coins / 1 Credit",		0x01,
	"4 Coins / 1 Credit",		0x05,
	"5 Coins / 1 Credit",		0x03,
	"Free Play",			0x07,
	NULL,				0xFF,
//	Dipname				mask&value
	"Lives par Coin",		0x0800,
	"3 - 12 Max",			0x00,
	"2 - 9 Max",			0x08,
	NULL,				0xFF,
//	Dipname				mask&value
	"Bonus",			0x3000,
	"30k and every 50k",		0x00,
	"50k and every 100k",		0x20,
	"100k and every 150k",		0x10,
	"200k and every 200k",		0x30,
	NULL,				0xFF,
//	Dipname				mask&value
	"Difficulty",			0x4000,
	"Normal",			0x00,
	"Hard",				0x40,
	NULL,				0xFF,
//	Dipname				mask&value
	"Demo Sounds",			0x8080,
	"Off",				0x00,
	"On",				0x80,
	NULL,				0xFF,
// デリミタ
	NULL,				0,
};

// VS Slalom
VSDIPSWITCH	vsdip_slalom[] = {
//	Dipname				mask&value
	"Coin",				0x0700,
	"1 Coin  / 1 Credit",		0x00,
	"1 Coin  / 2 Credits",		0x04,
	"1 Coin  / 3 Credits",		0x02,
	"2 Coins / 1 Credit",		0x06,
	"3 Coins / 1 Credit",		0x01,
	"4 Coins / 1 Credit",		0x05,
	"5 Coins / 1 Credit",		0x03,
	"Free Play",			0x07,
	NULL,				0xFF,
//	Dipname				mask&value
	"Freestyle Points",		0x0800,
	"Left / Right",			0x00,
	"Hold Time",			0x08,
	NULL,				0xFF,
//	Dipname				mask&value
	"Difficulty",			0x3010,
	"Easy",				0x00,
	"Normal",			0x10,
	"Hard",				0x20,
	"Hardest",			0x30,
	NULL,				0xFF,
//	Dipname				mask&value
	"Allow Continue",		0x4000,
	"No",				0x40,
	"Yes",				0x00,
	NULL,				0xFF,
//	Dipname				mask&value
	"Inverted input",		0x8000,
	"Off",				0x00,
	"On",				0x80,
	NULL,				0xFF,
// デリミタ
	NULL,				0,
};

// VS Sky Kid
VSDIPSWITCH	vsdip_skykid[] = {
//	Dipname				mask&value
	"Unknown",			0x0100,
	"Off",				0x00,
	"On",				0x01,
	NULL,				0xFF,
//	Dipname				mask&value
	"Unknown",			0x0200,
	"Off",				0x00,
	"On",				0x02,
	NULL,				0xFF,
//	Dipname				mask&value
	"Lives",			0x0404,
	"2",				0x00,
	"3",				0x04,
	NULL,				0xFF,
//	Dipname				mask&value
	"Coin",				0x1800,
	"1 Coin  / 1 Credit",		0x00,
	"1 Coin  / 2 Credits",		0x08,
	"2 Coins / 1 Credit",		0x10,
	"3 Coins / 1 Credit",		0x18,
	NULL,				0xFF,
//	Dipname				mask&value
	"Color Palette",		0xE020,
	"Normal",			0x20,
	"Wrong 1",			0x00,
	"Wrong 2",			0x40,
	"Wrong 3",			0x80,
	"Wrong 4",			0xC0,
	NULL,				0xFF,
// デリミタ
	NULL,				0,
};

// VS Star Luster
VSDIPSWITCH	vsdip_starluster[] = {
//	Dipname				mask&value
	"Coin",				0x0300,
	"1 Coin  / 1 Credit",		0x00,
	"1 Coin  / 2 Credits",		0x02,
	"2 Coins / 1 Credit",		0x01,
	"3 Coins / 1 Credit",		0x03,
	NULL,				0xFF,
//	Dipname				mask&value
	"Unknown",			0x0400,
	"Off",				0x00,
	"On",				0x04,
	NULL,				0xFF,
//	Dipname				mask&value
	"Unknown",			0x0800,
	"Off",				0x00,
	"On",				0x08,
	NULL,				0xFF,
//	Dipname				mask&value
	"Unknown",			0x1000,
	"Off",				0x00,
	"On",				0x10,
	NULL,				0xFF,
//	Dipname				mask&value
	"Color Palette",		0x6000,
	"Normal",			0x00,
	"Wrong 1",			0x20,
	"Wrong 2",			0x40,
	"Wrong 3",			0x60,
	NULL,				0xFF,
//	Dipname				mask&value
	"Unknown",			0x8000,
	"Off",				0x00,
	"On",				0x80,
	NULL,				0xFF,
// デリミタ
	NULL,				0,
};

// VS Dr. Mario
VSDIPSWITCH	vsdip_drmario[] = {
//	Dipname				mask&value
	"Drop Rate Increases After",	0x0300,
	"7 Pills",			0x00,
	"8 Pills",			0x01,
	"9 Pills",			0x02,
	"10 Pills",			0x03,
	NULL,				0xFF,
//	Dipname				mask&value
	"Virus Level",			0x0C00,
	"1",				0x00,
	"3",				0x04,
	"5",				0x08,
	"7",				0x0C,
	NULL,				0xFF,
//	Dipname				mask&value
	"Drop Speed Up",		0x3000,
	"Slow",				0x00,
	"Midium",			0x10,
	"Fast",				0x20,
	"Fastest",			0x30,
	NULL,				0xFF,
//	Dipname				mask&value
	"Free Play",			0x4000,
	"Off",				0x00,
	"On",				0x40,
	NULL,				0xFF,
//	Dipname				mask&value
	"Demo Sounds",			0x8080,
	"Off",				0x00,
	"On",				0x80,
	NULL,				0xFF,
// デリミタ
	NULL,				0,
};

// VS Castlevania
VSDIPSWITCH	vsdip_castlevania[] = {
//	Dipname				mask&value
	"Coin",				0x0700,
	"1 Coin  / 1 Credit",		0x00,
	"1 Coin  / 2 Credits",		0x04,
	"1 Coin  / 3 Credits",		0x02,
	"2 Coins / 1 Credit",		0x06,
	"3 Coins / 1 Credit",		0x01,
	"4 Coins / 1 Credit",		0x05,
	"5 Coins / 1 Credit",		0x03,
	"Free Play",			0x07,
	NULL,				0xFF,
//	Dipname				mask&value
	"Lives",			0x0808,
	"2",				0x00,
	"3",				0x08,
	NULL,				0xFF,
//	Dipname				mask&value
	"Bonus",			0x3000,
	"100k",				0x00,
	"200k",				0x10,
	"300k",				0x20,
	"400k",				0x30,
	NULL,				0xFF,
//	Dipname				mask&value
	"Difficulty",			0x4000,
	"Normal",			0x00,
	"Hard",				0x40,
	NULL,				0xFF,
//	Dipname				mask&value
	"Unknown",			0x8000,
	"Off",				0x00,
	"On",				0x80,
	NULL,				0xFF,
// デリミタ
	NULL,				0,
};

// VS Platoon
VSDIPSWITCH	vsdip_platoon[] = {
//	Dipname				mask&value
	"Unknown",			0x0100,
	"Off",				0x00,
	"On",				0x01,
	NULL,				0xFF,
//	Dipname				mask&value
	"Unknown",			0x0200,
	"Off",				0x00,
	"On",				0x02,
	NULL,				0xFF,
//	Dipname				mask&value
	"Demo Sounds",			0x0404,
	"Off",				0x00,
	"On",				0x04,
	NULL,				0xFF,
//	Dipname				mask&value
	"Unknown",			0x0800,
	"Off",				0x00,
	"On",				0x08,
	NULL,				0xFF,
//	Dipname				mask&value
	"Unknown",			0x1000,
	"Off",				0x00,
	"On",				0x10,
	NULL,				0xFF,
//	Dipname				mask&value
	"Coin",				0xE000,
	"1 Coin  / 1 Credit",		0x00,
	"1 Coin  / 2 Credits",		0x20,
	"1 Coin  / 3 Credits",		0x40,
	"2 Coins / 1 Credit",		0x60,
	"3 Coins / 1 Credit",		0x80,
	"4 Coins / 1 Credit",		0xA0,
	"5 Coins / 1 Credit",		0xC0,
	"Free Play",			0xE0,
	NULL,				0xFF,
// デリミタ
	NULL,				0,
};

// VS Excitebike
VSDIPSWITCH	vsdip_excitebike[] = {
//	Dipname				mask&value
	"Coin",				0x0700,
	"1 Coin  / 1 Credit",		0x00,
	"1 Coin  / 2 Credits",		0x04,
	"1 Coin  / 3 Credits",		0x02,
	"1 Coin  / 4 Credits",		0x06,
	"2 Coins / 1 Credit",		0x01,
	"3 Coins / 1 Credit",		0x05,
	"4 Coins / 1 Credit",		0x03,
	"Free Play",			0x07,
	NULL,				0xFF,
//	Dipname				mask&value
	"Bonus",			0x1800,
	"100k and Every 50k",		0x00,
	"Every 100k",			0x10,
	"100k Only",			0x08,
	"None",				0x18,
	NULL,				0xFF,
//	Dipname				mask&value
	"1st Half Qualifying Time",	0x2000,
	"Normal",			0x00,
	"Hard",				0x20,
	NULL,				0xFF,
//	Dipname				mask&value
	"2nd Half Qualifying Time",	0x4000,
	"Normal",			0x00,
	"Hard",				0x40,
	NULL,				0xFF,
//	Dipname				mask&value
	"Unknown",			0x8000,
	"Off",				0x00,
	"On",				0x80,
	NULL,				0xFF,
// デリミタ
	NULL,				0,
};

// VS Clu Clu Land
VSDIPSWITCH	vsdip_clucluland[] = {
//	Dipname				mask&value
	"Coin",				0x0700,
	"1 Coin  / 1 Credit",		0x00,
	"1 Coin  / 2 Credits",		0x04,
	"1 Coin  / 3 Credits",		0x02,
	"1 Coin  / 4 Credits",		0x06,
	"2 Coins / 1 Credit",		0x01,
	"3 Coins / 1 Credit",		0x05,
	"4 Coins / 1 Credit",		0x03,
	"Free Play",			0x07,
	NULL,				0xFF,
//	Dipname				mask&value
	"Unknown",			0x0800,
	"Off",				0x00,
	"On",				0x08,
	NULL,				0xFF,
//	Dipname				mask&value
	"Unknown",			0x1000,
	"Off",				0x00,
	"On",				0x10,
	NULL,				0xFF,
//	Dipname				mask&value
	"Lives",			0x6000,
	"2",				0x60,
	"3",				0x00,
	"4",				0x40,
	"5",				0x20,
	NULL,				0xFF,
//	Dipname				mask&value
	"Unknown",			0x8000,
	"Off",				0x00,
	"On",				0x80,
	NULL,				0xFF,
// デリミタ
	NULL,				0,
};

// VS Ice Climber
VSDIPSWITCH	vsdip_iceclimber[] = {
//	Dipname				mask&value
	"Coin",				0x0700,
	"1 Coin  / 1 Credit",		0x00,
	"1 Coin  / 2 Credits",		0x04,
	"1 Coin  / 3 Credits",		0x02,
	"1 Coin  / 4 Credits",		0x06,
	"2 Coins / 1 Credit",		0x01,
	"3 Coins / 1 Credit",		0x05,
	"4 Coins / 1 Credit",		0x03,
	"Free Play",			0x07,
	NULL,				0xFF,
//	Dipname				mask&value
	"Lives",			0x1800,
	"3",				0x00,
	"4",				0x10,
	"5",				0x08,
	"7",				0x18,
	NULL,				0xFF,
//	Dipname				mask&value
	"Difficulty",			0x2000,
	"Normal",			0x00,
	"Hard",				0x20,
	NULL,				0xFF,
//	Dipname				mask&value
	"Time before the bear",		0x4000,
	"Long",				0x00,
	"Short",			0x40,
	NULL,				0xFF,
//	Dipname				mask&value
	"Unknown",			0x8000,
	"Off",				0x00,
	"On",				0x80,
	NULL,				0xFF,
// デリミタ
	NULL,				0,
};

// VS Super Mario Bros
VSDIPSWITCH	vsdip_supermariobros[] = {
//	Dipname				mask&value
	"Coin",				0x0700,
	"1 Coin  / 1 Credit",		0x00,
	"1 Coin  / 2 Credits",		0x06,
	"1 Coin  / 3 Credits",		0x01,
	"1 Coin  / 4 Credits",		0x05,
	"1 Coin  / 5 Credits",		0x03,
	"2 Coins / 1 Credit",		0x04,
	"3 Coins / 1 Credit",		0x02,
	"Free Play",			0x07,
	NULL,				0xFF,
//	Dipname				mask&value
	"Lives",			0x0800,
	"2",				0x08,
	"3",				0x00,
	NULL,				0xFF,
//	Dipname				mask&value
	"Bonus Life",			0x3000,
	"100",				0x00,
	"150",				0x20,
	"200",				0x10,
	"250",				0x30,
	NULL,				0xFF,
//	Dipname				mask&value
	"Timer",			0x4000,
	"Normal",			0x00,
	"Fast",				0x40,
	NULL,				0xFF,
//	Dipname				mask&value
	"Continue Lives",		0x8000,
	"3",				0x80,
	"4",				0x00,
	NULL,				0xFF,
// デリミタ
	NULL,				0,
};

// VS Pinball
VSDIPSWITCH	vsdip_pinball[] = {
//	Dipname				mask&value
	"Coin",				0x0701,
	"1 Coin  / 1 Credit",		0x01,
	"1 Coin  / 2 Credits",		0x06,
	"1 Coin  / 3 Credits",		0x02,
	"1 Coin  / 4 Credits",		0x04,
	"2 Coins / 1 Credit",		0x05,
	"3 Coins / 1 Credit",		0x03,
	"4 Coins / 1 Credit",		0x07,
	"Free Play",			0x00,
	NULL,				0xFF,
//	Dipname				mask&value
	"Unknown",			0x0800,
	"Off",				0x00,
	"On",				0x08,
	NULL,				0xFF,
//	Dipname				mask&value
	"Unknown",			0x1000,
	"Off",				0x00,
	"On",				0x10,
	NULL,				0xFF,
//	Dipname				mask&value
	"Balls",			0x6000,
	"2",				0x60,
	"3",				0x00,
	"4",				0x40,
	"5",				0x20,
	NULL,				0xFF,
//	Dipname				mask&value
	"Ball Speed",			0x8000,
	"Normal",			0x00,
	"Fast",				0x80,
	NULL,				0xFF,
// デリミタ
	NULL,				0,
};

// VS Mach Rider(Endurance Course)
VSDIPSWITCH	vsdip_machrider[] = {
//	Dipname				mask&value
	"Coin",				0x0700,
	"1 Coin  / 1 Credit",		0x00,
	"1 Coin  / 2 Credits",		0x04,
	"1 Coin  / 3 Credits",		0x02,
	"1 Coin  / 4 Credits",		0x06,
	"2 Coins / 1 Credit",		0x01,
	"3 Coins / 1 Credit",		0x05,
	"4 Coins / 1 Credit",		0x03,
	"Free Play",			0x07,
	NULL,				0xFF,
//	Dipname				mask&value
	"Time",				0x1800,
	"280",				0x00,
	"250",				0x10,
	"220",				0x08,
	"200",				0x18,
	NULL,				0xFF,
//	Dipname				mask&value
	"Unknown",			0x2000,
	"Off",				0x00,
	"On",				0x20,
	NULL,				0xFF,
//	Dipname				mask&value
	"Unknown",			0x4000,
	"Off",				0x00,
	"On",				0x40,
	NULL,				0xFF,
//	Dipname				mask&value
	"Unknown",			0x8000,
	"Off",				0x00,
	"On",				0x80,
	NULL,				0xFF,
// デリミタ
	NULL,				0,
};

// VS Mach Rider (Japan, Fighting Course)
VSDIPSWITCH	vsdip_machriderj[] = {
//	Dipname				mask&value
	"Coin",				0x0700,
	"1 Coin  / 1 Credit",		0x00,
	"1 Coin  / 2 Credits",		0x04,
	"1 Coin  / 3 Credits",		0x02,
	"1 Coin  / 4 Credits",		0x06,
	"2 Coins / 1 Credit",		0x01,
	"3 Coins / 1 Credit",		0x05,
	"4 Coins / 1 Credit",		0x03,
	"Free Play",			0x07,
	NULL,				0xFF,
//	Dipname				mask&value
	"1st Race Distance",		0x1000,
	"12",				0x00,
	"15",				0x10,
	NULL,				0xFF,
//	Dipname				mask&value
	"Unknown",			0x1000,
	"Off",				0x00,
	"On",				0x20,
	NULL,				0xFF,
//	Dipname				mask&value
	"Unknown",			0x2000,
	"Off",				0x00,
	"On",				0x20,
	NULL,				0xFF,
//	Dipname				mask&value
	"Unknown",			0x4000,
	"Off",				0x00,
	"On",				0x40,
	NULL,				0xFF,
//	Dipname				mask&value
	"Unknown",			0x8000,
	"Off",				0x00,
	"On",				0x80,
	NULL,				0xFF,
// デリミタ
	NULL,				0,
};

// VS Soccer
VSDIPSWITCH	vsdip_soccer[] = {
//	Dipname				mask&value
	"Coin",				0x0700,
	"1 Coin  / 1 Credit",		0x00,
	"1 Coin  / 2 Credits",		0x04,
	"1 Coin  / 3 Credits",		0x02,
	"1 Coin  / 4 Credits",		0x06,
	"2 Coins / 1 Credit",		0x01,
	"3 Coins / 1 Credit",		0x05,
	"4 Coins / 1 Credit",		0x03,
	"Free Play",			0x07,
	NULL,				0xFF,
//	Dipname				mask&value
	"Points Timer",			0x1808,
	"600pts",			0x00,
	"800pts",			0x10,
	"1000pts",			0x08,
	"1200pts",			0x18,
	NULL,				0xFF,
//	Dipname				mask&value
	"Difficulty",			0x6040,
	"Easy",				0x00,
	"Normal",			0x40,
	"Hard",				0x20,
	"Hardest",			0x60,
	NULL,				0xFF,
//	Dipname				mask&value
	"Unknown",			0x8000,
	"Off",				0x00,
	"On",				0x80,
	NULL,				0xFF,
// デリミタ
	NULL,				0,
};

// VS Battle City
VSDIPSWITCH	vsdip_battlecity[] = {
//	Dipname				mask&value
	"Credits for 2 Players",	0x0101,
	"1",				0x00,
	"2",				0x01,
	NULL,				0xFF,
//	Dipname				mask&value
	"Lives",			0x0200,
	"3",				0x00,
	"5",				0x02,
	NULL,				0xFF,
//	Dipname				mask&value
	"Demo Sounds",			0x0404,
	"Off",				0x00,
	"On",				0x04,
	NULL,				0xFF,
//	Dipname				mask&value
	"Unknown",			0x1000,
	"Off",				0x00,
	"On",				0x10,
	NULL,				0xFF,
//	Dipname				mask&value
	"Unknown",			0x2000,
	"Off",				0x00,
	"On",				0x20,
	NULL,				0xFF,
//	Dipname				mask&value
	"Color Palette",		0xC080,
	"Normal",			0x80,
	"Wrong 1",			0x00,
	"Wrong 2",			0x40,
	"Wrong 3",			0xC0,
	NULL,				0xFF,
// デリミタ
	NULL,				0,
};

// VS Gradius
VSDIPSWITCH	vsdip_gradius[] = {
//	Dipname				mask&value
	"Coin",				0x0700,
	"1 Coin  / 1 Credit",		0x00,
	"1 Coin  / 2 Credits",		0x04,
	"1 Coin  / 3 Credits",		0x02,
	"2 Coins / 1 Credit",		0x06,
	"3 Coins / 1 Credit",		0x01,
	"4 Coins / 1 Credit",		0x05,
	"5 Coins / 1 Credit",		0x03,
	"Free Play",			0x07,
	NULL,				0xFF,
//	Dipname				mask&value
	"Lives",			0x0808,
	"3",				0x08,
	"4",				0x00,
	NULL,				0xFF,
//	Dipname				mask&value
	"Bonus",			0x3000,
	"100k",				0x00,
	"200k",				0x20,
	"300k",				0x10,
	"400k",				0x30,
	NULL,				0xFF,
//	Dipname				mask&value
	"Difficulty",			0x4000,
	"Normal",			0x00,
	"Hard",				0x40,
	NULL,				0xFF,
//	Dipname				mask&value
	"Demo Sounds",			0x8080,
	"Off",				0x00,
	"On",				0x80,
	NULL,				0xFF,
// デリミタ
	NULL,				0,
};

// VS Goonies
VSDIPSWITCH	vsdip_goonies[] = {
//	Dipname				mask&value
	"Coin",				0x0700,
	"1 Coin  / 1 Credit",		0x00,
	"1 Coin  / 2 Credits",		0x04,
	"1 Coin  / 3 Credits",		0x02,
	"2 Coins / 1 Credit",		0x06,
	"3 Coins / 1 Credit",		0x01,
	"4 Coins / 1 Credit",		0x05,
	"5 Coins / 1 Credit",		0x03,
	"Free Play",			0x07,
	NULL,				0xFF,
//	Dipname				mask&value
	"Lives",			0x0800,
	"3",				0x00,
	"2",				0x08,
	NULL,				0xFF,
//	Dipname				mask&value
	"Unknown",			0x1000,
	"Off",				0x00,
	"On",				0x10,
	NULL,				0xFF,
//	Dipname				mask&value
	"Unknown",			0x2000,
	"Off",				0x00,
	"On",				0x20,
	NULL,				0xFF,
//	Dipname				mask&value
	"Timer",			0x4000,
	"Normal",			0x00,
	"Fast",				0x40,
	NULL,				0xFF,
//	Dipname				mask&value
	"Demo Sounds",			0x8080,
	"Off",				0x00,
	"On",				0x80,
	NULL,				0xFF,
// デリミタ
	NULL,				0,
};

// VS Hogan's Alley
VSDIPSWITCH	vsdip_hogansalley[] = {
//	Dipname				mask&value
	"Coin",				0x0700,
	"5 Coins / 1 Credit",		0x03,
	"4 Coins / 1 Credit",		0x05,
	"3 Coins / 1 Credit",		0x01,
	"2 Coins / 1 Credit",		0x06,
	"1 Coin  / 1 Credit",		0x00,
	"1 Coin  / 2 Credits",		0x04,
	"1 Coin  / 3 Credits",		0x02,
	"Free Play",			0x07,
	NULL,				0xFF,
//	Dipname				mask&value
	"Difficulty",			0x1808,
	"Easy",				0x00,
	"Normal",			0x08,
	"Hard",				0x10,
	"Very Hard",			0x18,
	NULL,				0xFF,
//	Dipname				mask&value
	"Misses par Game",		0x2000,
	"3",				0x00,
	"5",				0x20,
	NULL,				0xFF,
//	Dipname				mask&value
	"Bonus Life",			0xC000,
	"30000pts",			0x00,
	"50000pts",			0x40,
	"80000pts",			0x80,
	"100000pts",			0xC0,
	NULL,				0xFF,
// デリミタ
	NULL,				0,
};

// VS Freedom Force
VSDIPSWITCH	vsdip_freedomforce[] = {
//	Dipname				mask&value
	"Unknown",			0x0100,
	"Off",				0x00,
	"On",				0x01,
	NULL,				0xFF,
//	Dipname				mask&value
	"Unknown",			0x0200,
	"Off",				0x00,
	"On",				0x02,
	NULL,				0xFF,
//	Dipname				mask&value
	"Unknown",			0x0400,
	"Off",				0x00,
	"On",				0x04,
	NULL,				0xFF,
//	Dipname				mask&value
	"Unknown",			0x0800,
	"Off",				0x00,
	"On",				0x08,
	NULL,				0xFF,
//	Dipname				mask&value
	"Unknown",			0x1000,
	"Off",				0x00,
	"On",				0x10,
	NULL,				0xFF,
//	Dipname				mask&value
	"Unknown",			0x2000,
	"Off",				0x00,
	"On",				0x20,
	NULL,				0xFF,
//	Dipname				mask&value
	"Unknown",			0x4000,
	"Off",				0x00,
	"On",				0x40,
	NULL,				0xFF,
//	Dipname				mask&value
	"Unknown",			0x8000,
	"Off",				0x00,
	"On",				0x80,
	NULL,				0xFF,
// デリミタ
	NULL,				0,
};

// VS Super Xevious
VSDIPSWITCH	vsdip_superxevious[] = {
//	Dipname				mask&value
	"Unknown",			0x0100,
	"Off",				0x00,
	"On",				0x01,
	NULL,				0xFF,
//	Dipname				mask&value
	"Unknown",			0x0200,
	"Off",				0x00,
	"On",				0x02,
	NULL,				0xFF,
//	Dipname				mask&value
	"Unknown",			0x0400,
	"Off",				0x00,
	"On",				0x04,
	NULL,				0xFF,
//	Dipname				mask&value
	"Unknown",			0x0800,
	"Off",				0x00,
	"On",				0x08,
	NULL,				0xFF,
//	Dipname				mask&value
	"Unknown",			0x1000,
	"Off",				0x00,
	"On",				0x10,
	NULL,				0xFF,
//	Dipname				mask&value
	"Unknown",			0x2000,
	"Off",				0x00,
	"On",				0x20,
	NULL,				0xFF,
//	Dipname				mask&value
	"Unknown",			0x4000,
	"Off",				0x00,
	"On",				0x40,
	NULL,				0xFF,
//	Dipname				mask&value
	"Unknown",			0x8000,
	"Off",				0x00,
	"On",				0x80,
	NULL,				0xFF,
// デリミタ
	NULL,				0,
};

// VS Golf/Lady Golf
VSDIPSWITCH	vsdip_golf[] = {
//	Dipname				mask&value
	"Coin",				0x0701,
	"1 Coin  / 1 Credit",		0x01,
	"1 Coin  / 2 Credits",		0x06,
	"1 Coin  / 3 Credits",		0x02,
	"1 Coin  / 4 Credits",		0x04,
	"2 Coins / 1 Credit",		0x05,
	"3 Coins / 1 Credit",		0x03,
	"4 Coins / 1 Credit",		0x07,
	"Free Play",			0x00,
	NULL,				0xFF,
//	Dipname				mask&value
	"Hole Size",			0x0800,
	"Large",			0x00,
	"Small",			0x08,
	NULL,				0xFF,
//	Dipname				mask&value
	"Points par Stroke",		0x1000,
	"Easier",			0x00,
	"Harder",			0x10,
	NULL,				0xFF,
//	Dipname				mask&value
	"Starting Points",		0x6000,
	"10",				0x00,
	"13",				0x40,
	"16",				0x20,
	"20",				0x60,
	NULL,				0xFF,
//	Dipname				mask&value
	"Difficulty Vs. Computer",	0x8000,
	"Easy",				0x00,
	"Hard",				0x80,
	NULL,				0xFF,
// デリミタ
	NULL,				0,
};

// VS Stroke and Match Golf
VSDIPSWITCH	vsdip_strokandmatchegolf[] = {
//	Dipname				mask&value
	"Coin",				0x0701,
	"1 Coin  / 1 Credit",		0x01,
	"1 Coin  / 2 Credits",		0x06,
	"1 Coin  / 3 Credits",		0x02,
	"1 Coin  / 4 Credits",		0x04,
	"2 Coins / 1 Credit",		0x05,
	"3 Coins / 1 Credit",		0x03,
	"4 Coins / 1 Credit",		0x07,
	"Free Play",			0x00,
	NULL,				0xFF,
//	Dipname				mask&value
	"Hole Size",			0x0800,
	"Large",			0x00,
	"Small",			0x08,
	NULL,				0xFF,
//	Dipname				mask&value
	"Points par Stroke",		0x1000,
	"Easier",			0x00,
	"Harder",			0x10,
	NULL,				0xFF,
//	Dipname				mask&value
	"Starting Points",		0x6000,
	"300",				0x00,
	"400",				0x40,
	"500",				0x20,
	"600",				0x60,
	NULL,				0xFF,
//	Dipname				mask&value
	"Difficulty Vs. Computer",	0x8000,
	"Easy",				0x00,
	"Hard",				0x80,
	NULL,				0xFF,
// デリミタ
	NULL,				0,
};

// VS Raid on Bungeling Bay(J)
VSDIPSWITCH	vsdip_raidonbungelingbay[] = {
//	Dipname				mask&value
	"Coin",				0x0700,
	"1 Coin  / 1 Credit",		0x00,
	"1 Coin  / 2 Credits",		0x04,
	"1 Coin  / 3 Credits",		0x02,
	"1 Coin  / 4 Credits",		0x06,
	"2 Coins / 1 Credit",		0x01,
	"3 Coins / 1 Credit",		0x05,
	"4 Coins / 1 Credit",		0x03,
	"Free Play",			0x07,
	NULL,				0xFF,
//	Dipname				mask&value
	"Lives",			0x0808,
	"2",				0x00,
	"3",				0x08,
	NULL,				0xFF,
//	Dipname				mask&value
	"Unknown",			0x1000,
	"Off",				0x00,
	"On",				0x10,
	NULL,				0xFF,
//	Dipname				mask&value
	"Unknown",			0x2000,
	"Off",				0x00,
	"On",				0x20,
	NULL,				0xFF,
//	Dipname				mask&value
	"Unknown",			0x4000,
	"Off",				0x00,
	"On",				0x40,
	NULL,				0xFF,
//	Dipname				mask&value
	"Unknown",			0x8000,
	"Off",				0x00,
	"On",				0x80,
	NULL,				0xFF,
// デリミタ
	NULL,				0,
};

// VS Mighty Bomb Jack(J)
VSDIPSWITCH	vsdip_mightybombjack[] = {
//	Dipname				mask&value
	"Coin",				0x0700,
	"5 Coins / 1 Credit",		0x07,
	"4 Coins / 1 Credit",		0x03,
	"3 Coins / 1 Credit",		0x05,
	"2 Coins / 1 Credit",		0x01,
	"1 Coin  / 1 Credit",		0x00,
	"1 Coin  / 2 Credits",		0x04,
	"1 Coin  / 3 Credits",		0x02,
	"1 Coin  / 4 Credits",		0x06,
	NULL,				0xFF,
//	Dipname				mask&value
	"Lives",			0x1800,
	"2",				0x10,
	"3",				0x00,
	"4",				0x08,
	"5",				0x18,
	NULL,				0xFF,
//	Dipname				mask&value
	"Unknown",			0x2000,
	"Off",				0x00,
	"On",				0x20,
	NULL,				0xFF,
//	Dipname				mask&value
	"Unknown",			0x4000,
	"Off",				0x00,
	"On",				0x40,
	NULL,				0xFF,
//	Dipname				mask&value
	"Unknown",			0x8000,
	"Off",				0x00,
	"On",				0x80,
	NULL,				0xFF,
// デリミタ
	NULL,				0,
};


VSDIPSWITCH*	FindVSDipSwitchTable( DWORD crc )
{
VSDIPSWITCH*	dip = vsdip_default;

	switch( crc ) {
		case	0xeb2dba63:	// VS TKO Boxing
		case	0x98cfe016:
		case	0x9818f656:
			dip = vsdip_tkoboxing;
			break;
		case	0xb90497aa:	// VS Tennis
			break;
		case	0x8c0c2df5:	// VS Top Gun
			dip = vsdip_topgun;
			break;
		case	0x16d3f469:	// VS Ninja Jajamaru Kun (J)
			dip = vsdip_jajamaru;
			break;
		case	0x8850924b:	// VS Tetris
			dip = vsdip_tetris;
			break;
		case	0xcf36261e:	// VS Sky Kid
			dip = vsdip_skykid;
			break;
		case	0xe1aa8214:	// VS Star Luster
			dip = vsdip_starluster;
			break;
		case	0xec461db9:	// VS Pinball
		case	0xe528f651:	// VS Pinball (alt)
			dip = vsdip_pinball;
			break;
		case	0x17ae56be:	// VS Freedom Force
			dip = vsdip_freedomforce;
			break;
		case	0xe2c0a2be:	// VS Platoon
			dip = vsdip_platoon;
			break;
		case	0x70901b25:	// VS Slalom
			dip = vsdip_slalom;
			break;
		case	0x0b65a917:	// VS Mach Rider(Endurance Course)
		case	0x8a6a9848:
			dip = vsdip_machrider;
			break;
		case	0xae8063ef:	// VS Mach Rider(Japan, Fighting Course)
			dip = vsdip_machriderj;
			break;
		case	0xffbef374:	// VS Castlevania
			dip = vsdip_castlevania;
			break;

		case	0xcc2c4b5d:	// VS Golf
		case	0x86167220:	// VS Lady Golf
			dip = vsdip_golf;
			break;

		case	0xa93a5aee:	// VS Stroke and Match Golf
			dip = vsdip_strokandmatchegolf;
			break;

		case	0x135adf7c:	// VS Atari RBI Baseball
			dip = vsdip_rbibaseball;
			break;
		case	0xd5d7eac4:	// VS Dr. Mario
			dip = vsdip_drmario;
			break;
		case	0x46914e3e:	// VS Soccer
			dip = vsdip_soccer;
			break;
		case	0x70433f2c:	// VS Battle City
		case	0x8d15a6e6:	// VS bad .nes
			dip = vsdip_battlecity;
			break;
		case	0x1e438d52:	// VS Goonies
			dip = vsdip_goonies;
			break;

		case	0xcbe85490:	// VS Excitebike
		case	0x29155e0c:	// VS Excitebike (alt)
			dip = vsdip_excitebike;
			break;

		case	0x07138c06:	// VS Clu Clu Land
			dip = vsdip_clucluland;
			break;
		case	0x43a357ef:	// VS Ice Climber
			dip = vsdip_iceclimber;
			break;
		case	0x737dd1bf:	// VS Super Mario Bros
		case	0x4bf3972d:
		case	0x8b60cc58:
		case	0x8192c804:
			dip = vsdip_supermariobros;
			break;

		case	0xd99a2087:	// VS Gradius
			dip = vsdip_gradius;
			break;
		case	0xf9d3b0a3:	// VS Super Xevious
		case	0x9924980a:	// VS Super Xevious
		case	0x66bb838f:	// VS Super Xevious
			dip = vsdip_superxevious;
			break;

		case	0xff5135a3:	// VS Hogan's Alley
			dip = vsdip_hogansalley;
			break;
		case	0xed588f00:	// VS Duck Hunt
			dip = vsdip_duckhunt;
			break;

		case	0xc99ec059:	// VS Raid on Bungeling Bay(J)
			dip = vsdip_raidonbungelingbay;
			break;
		case	0xca85e56d:	// VS Mighty Bomb Jack(J)
			dip = vsdip_mightybombjack;
			break;
	}

	return	dip;
}

BYTE	GetVSDefaultDipSwitchValue( DWORD crc )
{
VSDIPSWITCH*	dip = FindVSDipSwitchTable( crc );

	BYTE	value = 0x00;
	BYTE	flag = 0;
	for(;;) {
		if( dip->name == NULL && dip->value == 0x0000 ) {
			break;
		} else if( dip->value == 0x00FF ) {
			flag = 0;
		} else if( !flag ) {
			flag = 1;
			value |= dip->value;
		}
		dip++;
	}

	return	value;
}

