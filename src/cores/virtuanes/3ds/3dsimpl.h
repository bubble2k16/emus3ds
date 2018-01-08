
#include "3dsgpu.h"

#ifndef _3DSIMPL_H_
#define _3DSIMPL_H_

#define BTN3DS_A        0
#define BTN3DS_B        1
#define BTN3DS_X        2
#define BTN3DS_Y        3
#define BTN3DS_L        4
#define BTN3DS_R        5


#define BTNNES_A 			0x1
#define BTNNES_B 			0x2
#define BTNNES_SELECT 		0x4
#define BTNNES_START 		0x8
#define BTNNES_UP 			0x10
#define BTNNES_DOWN 		0x20
#define BTNNES_LEFT 		0x40
#define BTNNES_RIGHT 		0x80

#define BTNNES_2A 			0x0100
#define BTNNES_2B 			0x0200
#define BTNNES_2SELECT 		0x0400
#define BTNNES_2START 		0x0800
#define BTNNES_2UP 			0x1000
#define BTNNES_2DOWN 		0x2000
#define BTNNES_2LEFT 		0x4000
#define BTNNES_2RIGHT 		0x8000

extern SGPUTexture *snesMainScreenTarget;
extern SGPUTexture *snesTileCacheTexture;
extern SGPUTexture *snesDepthForScreens;
extern SGPUTexture *snesDepthForOtherTextures;

#endif