
#include "palette.h"

SNesPalette nesPalette[64] =
{
    // Smooth FBX palette
    { 0x6A, 0x6D, 0x6A }, /* 0 */
    { 0x00, 0x13, 0x80 }, /* 1 */
	{ 0x1E, 0x00, 0x8A }, /* 2 */
    { 0x39, 0x00, 0x7A }, /* 3 */
    { 0x55, 0x00, 0x56 }, /* 4 */
    { 0x5A, 0x00, 0x18 }, /* 5 */
    { 0x4F, 0x10, 0x00 }, /* 6 */
    { 0x3D, 0x1C, 0x00 }, /* 7 */
    { 0x25, 0x32, 0x00 }, /* 8 */
    { 0x00, 0x3D, 0x00 }, /* 9 */
    { 0x00, 0x40, 0x00 }, /* 10 */
    { 0x00, 0x39, 0x24 }, /* 11 */
    { 0x00, 0x2E, 0x55 }, /* 12 */
    { 0x00, 0x00, 0x00 }, /* 13 */
    { 0x00, 0x00, 0x00 }, /* 14 */
    { 0x00, 0x00, 0x00 }, /* 15 */

    { 0xB9, 0xBC, 0xB9 }, /* 16 */
    { 0x18, 0x50, 0xC7 }, /* 17 */
    { 0x4B, 0x30, 0xE3 }, /* 18 */
    { 0x73, 0x22, 0xD6 }, /* 19 */
    { 0x95, 0x1F, 0xA9 }, /* 20 */
    { 0x9D, 0x28, 0x5C }, /* 21 */
    { 0x98, 0x37, 0x00 }, /* 22 */
    { 0x7F, 0x4C, 0x00 }, /* 23 */
    { 0x5E, 0x64, 0x00 }, /* 24 */
    { 0x22, 0x77, 0x00 }, /* 25 */
    { 0x02, 0x7E, 0x02 }, /* 26 */
    { 0x00, 0x76, 0x45 }, /* 27 */
    { 0x00, 0x6E, 0x8A }, /* 28 */
    { 0x00, 0x00, 0x00 }, /* 29 */
    { 0x00, 0x00, 0x00 }, /* 30 */
    { 0x00, 0x00, 0x00 }, /* 31 */

    { 0xFF, 0xFF, 0xFF }, /* 32 */
    { 0x68, 0xA6, 0xFF }, /* 33 */
    { 0x8C, 0x9C, 0xFF }, /* 34 */
    { 0xB5, 0x86, 0xFF }, /* 35 */
    { 0xD9, 0x75, 0xFD }, /* 36 */
    { 0xE3, 0x77, 0xB9 }, /* 37 */
    { 0xE5, 0x8D, 0x68 }, /* 38 */
    { 0xD4, 0x9D, 0x29 }, /* 39 */
    { 0xB3, 0xAF, 0x0C }, /* 40 */
    { 0x7B, 0xC2, 0x11 }, /* 41 */
    { 0x55, 0xCA, 0x47 }, /* 42 */
    { 0x46, 0xCB, 0x81 }, /* 43 */
    { 0x47, 0xC1, 0xC5 }, /* 44 */
    { 0x4A, 0x4D, 0x4A }, /* 45 */
    { 0x00, 0x00, 0x00 }, /* 46 */
    { 0x00, 0x00, 0x00 }, /* 47 */

    { 0xFF, 0xFF, 0xFF }, /* 48 */
    { 0xCC, 0xEA, 0xFF }, /* 49 */
    { 0xDD, 0xDE, 0xFF }, /* 50 */
    { 0xEC, 0xDA, 0xFF }, /* 51 */
    { 0xF8, 0xD7, 0xFE }, /* 52 */
    { 0xFC, 0xD6, 0xF5 }, /* 53 */
    { 0xFD, 0xDB, 0xCF }, /* 54 */
    { 0xF9, 0xE7, 0xB5 }, /* 55 */
    { 0xF1, 0xF0, 0xAA }, /* 56 */
    { 0xDA, 0xFA, 0xA9 }, /* 57 */
    { 0xC9, 0xFF, 0xBC }, /* 58 */
    { 0xC3, 0xFB, 0xD7 }, /* 59 */
    { 0xC4, 0xF6, 0xF6 }, /* 60 */
    { 0xBE, 0xC1, 0xBE }, /* 61 */
    { 0x00, 0x00, 0x00 }, /* 62 */
    { 0x00, 0x00, 0x00 }, /* 63 */

};


static float redDeemphFactor[]={
	1.0,
	1.239,
	0.794,
	1.019,
	0.905,
	1.023,
	0.741,
	0.75
};
static float greenDeemphFactor[]={
	1.0,
	0.915,
	1.086,
	0.98,
	1.026,
	0.908,
	0.987,
	0.75
};
static float blueDeemphFactor[]={
	1.0,
	0.743,
	0.882,
	0.653,
	1.277,
	0.979,
	0.101,
	0.75
};

u16 rgbaPalette[16][64];


void nespalInitialize()
{
    //#define MAKE_COLOR(r,g,b,a) (a << 0) | (r << 24) | (g << 16) | (b << 8)
    #define MAKE_COLOR(r,g,b,a) ((r >> 3) << 11) | ((g >> 3) << 6) | ((b >> 3) << 0)

    for (int index = 0; index < 64; index++)
    {
        u8 alpha = 0;
        //if (index == 0)
        //	alpha = 0;

        u8 r = nesPalette[index].red;
        u8 g = nesPalette[index].green;
        u8 b = nesPalette[index].blue;

        rgbaPalette[0][index] = MAKE_COLOR(r,g,b,alpha);

        // compute deemph and grayscale
        //
        int nr, ng, nb;
        for (int i = 1; i < 16; i++)
        {
            if (i & 1)
            {
                // Gray scale
                nr = nesPalette[index & 0x30].red;
                ng = nesPalette[index & 0x30].green;
                nb = nesPalette[index & 0x30].blue;
            }
            else
            {
                // Default palette.
                nr = r;
                ng = g;
                nb = b;
            }

            int deemph = i >> 1;

            // Need r/g swap later for PAL games
            //
            int dr = (int) (nr * redDeemphFactor[deemph]);
            int dg = (int) (ng * greenDeemphFactor[deemph]);
            int db = (int) (nb * blueDeemphFactor[deemph]);
            if (dr > 0xff) dr = 0xff;
            if (dg > 0xff) dg = 0xff;
            if (db > 0xff) db = 0xff;

            rgbaPalette[i][index] = MAKE_COLOR(dr,dg,db,alpha);
        }
    }
}