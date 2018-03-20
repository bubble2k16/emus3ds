
#include "palette.h"

SNesPalette nesPalette[64] =
{
    // FCEUX palette
    { 0x74, 0x74, 0x74 }, /* 0 */
    { 0x24, 0x18, 0x8c }, /* 1 */
    { 0x00, 0x00, 0xa8 }, /* 2 */
    { 0x44, 0x00, 0x9c }, /* 3 */
    { 0x8c, 0x00, 0x74 }, /* 4 */
    { 0xa8, 0x00, 0x10 }, /* 5 */
    { 0xa4, 0x00, 0x00 }, /* 6 */
    { 0x7c, 0x08, 0x00 }, /* 7 */
    { 0x40, 0x2c, 0x00 }, /* 8 */
    { 0x00, 0x44, 0x00 }, /* 9 */
    { 0x00, 0x50, 0x00 }, /* 10 */
    { 0x00, 0x3c, 0x14 }, /* 11 */
    { 0x18, 0x3c, 0x5c }, /* 12 */
    { 0x00, 0x00, 0x00 }, /* 13 */
    { 0x00, 0x00, 0x00 }, /* 14 */
    { 0x00, 0x00, 0x00 }, /* 15 */

    { 0xbc, 0xbc, 0xbc }, /* 16 */
    { 0x00, 0x70, 0xec }, /* 17 */
    { 0x20, 0x38, 0xec }, /* 18 */
    { 0x80, 0x00, 0xf0 }, /* 19 */
    { 0xbc, 0x00, 0xbc }, /* 20 */
    { 0xe4, 0x00, 0x58 }, /* 21 */
    { 0xd8, 0x28, 0x00 }, /* 22 */
    { 0xc8, 0x4c, 0x0c }, /* 23 */
    { 0x88, 0x70, 0x00 }, /* 24 */
    { 0x00, 0x94, 0x00 }, /* 25 */
    { 0x00, 0xa8, 0x00 }, /* 26 */
    { 0x00, 0x90, 0x38 }, /* 27 */
    { 0x00, 0x80, 0x88 }, /* 28 */
    { 0x00, 0x00, 0x00 }, /* 29 */
    { 0x00, 0x00, 0x00 }, /* 30 */
    { 0x00, 0x00, 0x00 }, /* 31 */

    { 0xfc, 0xfc, 0xfc }, /* 32 */
    { 0x3c, 0xbc, 0xfc }, /* 33 */
    { 0x5c, 0x94, 0xfc }, /* 34 */
    { 0xcc, 0x88, 0xfc }, /* 35 */
    { 0xf4, 0x78, 0xfc }, /* 36 */
    { 0xfc, 0x74, 0xb4 }, /* 37 */
    { 0xfc, 0x74, 0x60 }, /* 38 */
    { 0xfc, 0x98, 0x38 }, /* 39 */
    { 0xf0, 0xbc, 0x3c }, /* 40 */
    { 0x80, 0xd0, 0x10 }, /* 41 */
    { 0x4c, 0xdc, 0x48 }, /* 42 */
    { 0x58, 0xf8, 0x98 }, /* 43 */
    { 0x00, 0xe8, 0xd8 }, /* 44 */
    { 0x78, 0x78, 0x78 }, /* 45 */
    { 0x00, 0x00, 0x00 }, /* 46 */
    { 0x00, 0x00, 0x00 }, /* 47 */

    { 0xfc, 0xfc, 0xfc }, /* 48 */
    { 0xa8, 0xe4, 0xfc }, /* 49 */
    { 0xc4, 0xd4, 0xfc }, /* 50 */
    { 0xd4, 0xc8, 0xfc }, /* 51 */
    { 0xfc, 0xc4, 0xfc }, /* 52 */
    { 0xfc, 0xc4, 0xd8 }, /* 53 */
    { 0xfc, 0xbc, 0xb0 }, /* 54 */
    { 0xfc, 0xd8, 0xa8 }, /* 55 */
    { 0xfc, 0xe4, 0xa0 }, /* 56 */
    { 0xe0, 0xfc, 0xa0 }, /* 57 */
    { 0xa8, 0xf0, 0xbc }, /* 58 */
    { 0xb0, 0xfc, 0xcc }, /* 59 */
    { 0x9c, 0xfc, 0xf0 }, /* 60 */
    { 0xc4, 0xc4, 0xc4 }, /* 61 */
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