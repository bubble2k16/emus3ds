
#include "3dsgpu.h"

#ifndef _3DSIMPL_H_
#define _3DSIMPL_H_

#define BTN3DS_A        0
#define BTN3DS_B        1
#define BTN3DS_X        2
#define BTN3DS_Y        3
#define BTN3DS_L        4
#define BTN3DS_R        5
#define BTN3DS_ZL       6
#define BTN3DS_ZR       7
#define BTN3DS_SELECT   8
#define BTN3DS_START    9


#define BTNNES_A 			0x1
#define BTNNES_B 			0x2
#define BTNNES_SELECT 		0x4
#define BTNNES_START 		0x8
#define BTNNES_UP 			0x10
#define BTNNES_DOWN 		0x20
#define BTNNES_LEFT 		0x40
#define BTNNES_RIGHT 		0x80

extern SGPUTexture *snesMainScreenTarget;
extern SGPUTexture *snesTileCacheTexture;
extern SGPUTexture *snesDepthForScreens;
extern SGPUTexture *snesDepthForOtherTextures;

#endif