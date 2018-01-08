
#include "extern.h"
#include "common.h"

#include "3dsgpu.h"
#include "3dsimpl_gpu.h"
#include "3dsimpl_tilecache.h"
#include "vsect.h"


VerticalSections screenWidthVerticalSection;

vdc_hw_struct vdc_hw_a;
vdc_hw_struct vdc_hw_b;


extern "C" void cache_tile(vdc_struct *vdc, u32 tile_number);
extern "C" void cache_pattern(vdc_struct *vdc, u32 pattern_number);

extern SGPUTexture *emuTileCacheTexture;


void reset_video_hw()
{
    for (int t = 0; t < 2048; t++)
        for (int p = 0; p < 32; p++)
        {
            GPU3DSExt.VRAMPaletteFrame[t][p] = 0;
            GPU3DSExt.VRAMPaletteFrame[t][p] = 0;
        }
    
    for (int p = 0; p < 32; p++)
    {
        GPU3DSExt.PaletteFrame[p] = 1;
    }

    GPU3DSExt.newCacheTexturePosition = 2;

    memset(&vdc_hw_a, 0, sizeof(vdc_hw_a));
    memset(&vdc_hw_b, 0, sizeof(vdc_hw_b));
}

void update_palette_frame(int pal)
{
    GPU3DSExt.PaletteFrame[pal]++;
}

void copy_line_data(vdc_struct *vdc, vdc_hw_struct *vdc_hw)
{
    vdc_hw->prev_spr = vdc_hw->cur_spr;
    vdc_hw->prev_bg  = vdc_hw->cur_bg;

    u32 scanline_line = vce.frame_counter - 14;

    if (scanline_line >= 0 && scanline_line < 256)
    {
        if((vdc->display_counter >= vdc->start_line) &&
            (vdc->display_counter < vdc->vblank_line) &&
            (vdc->burst_mode == 0))
        {
            int y_scroll = vdc->effective_byr;
            int x_scroll = vdc->bxr + vdc->scroll_shift;

            vdc_hw->x_scroll[scanline_line] = x_scroll;
            vdc_hw->y_scroll[scanline_line] = y_scroll;

            vdc_hw->screen_width[scanline_line] = vdc->screen_width;

            vdc_hw->cur_spr = (vdc->cr & 0x40) > 0;
            vdc_hw->cur_bg = (vdc->cr & 0x80) > 0;
        }
        else
        {
            vdc_hw->cur_spr = 0;
            vdc_hw->cur_bg = 0;
        }

        //printf ("%d hd s:%d e:%d %d\n", vce.screen_width, vdc->hds, vdc->hde, vdc->hdw);

        int clock_width = vce.screen_width;
        int hwidth = (vdc->hdw + 1) * 8;

        // maxwidth is just the physical width of the TV.
        // (See Charles MacDonald's documentation)
        // The real console probably can't output more than that
        // anyway, so we use it to clamp hwidth.
        //
        int maxwidth = 256;         
        if (clock_width == 256)
            maxwidth = 280;
        else if (clock_width == 320)
            maxwidth = 376;
        else if (clock_width == 512)
            maxwidth = 536;

        if (hwidth > maxwidth)
            hwidth = maxwidth;

        vsectUpdateValue(&screenWidthVerticalSection, 
            scanline_line, hwidth);
    }
    
    
    /*
    if (vdc_hw->x_scroll[scanline_line] != vdc_hw->x_scroll[scanline_line-1] ||
        vdc_hw->y_scroll[scanline_line] != vdc_hw->y_scroll[scanline_line-1] + 1 ||
        vdc_hw->screen_width[scanline_line] != vdc_hw->screen_width[scanline_line-1])
    printf ("%3d: %d %d %d\n", scscanline_lineanline, vdc->screen_width, x_scroll, y_scroll);
    */
}


int bg_depth = 3 << 8;               // depth for BG (shifted left by 8)
int spr_depth[2] = {2 << 8, 5 << 8};      // depth for SPR (shifted left by 8)


inline void render_bg_tile (
    vdc_struct *vdc, u32 *screen_ptr, 
    int x, int y, int offset_y,
    int current_palette,
    int tile_number, int tile_start_y, int tile_end_y)
{
    //printf ("R @%3d,%3d - %4x,%d,%d\n", x, y, tile_number, tile_start_y, tile_end_y);

    //printf ("R @  %d\n", current_palette);
    //screen_ptr[0] = tile_number + 1;
    u32 *tile_screen_ptr = &screen_ptr[y * 512 + x];

    // Cache the tile if it is marked dirty
    //
    if(vdc->dirty_tiles[tile_number])
    {
        cache_tile(vdc, tile_number);
        vdc->dirty_tiles[tile_number] = 0;
    
        for (int p = 0; p < 16; p++)
            GPU3DSExt.VRAMPaletteFrame[tile_number][p] = 0;
    }

    // Cached tile position
    //
    u8 *tile_cache = &vdc->tile_cache[tile_number * 64];
    int current_palette_idx = current_palette >> 4;

    // Cache the BG tile in our texture.
    //
    int texturePos = cache3dsGetTexturePositionFast(tile_number * 32, current_palette_idx);
    if (GPU3DSExt.VRAMPaletteFrame[tile_number][current_palette_idx] != 
        GPU3DSExt.PaletteFrame[current_palette_idx])
    {
        GPU3DSExt.VRAMPaletteFrame[tile_number][current_palette_idx] = GPU3DSExt.PaletteFrame[current_palette_idx];

        texturePos = cacheGetSwapTexturePositionForAltFrameFast(tile_number * 32, current_palette_idx);
        cache3dsCacheTGFX8x8TileToTexturePosition(tile_cache, &vce.palette_cache_5551[current_palette_idx * 16], texturePos);
    }

/*
    // Loop each line and render to screen
    //
    for (int tile_y = tile_start_y; tile_y <= tile_end_y; tile_y++)
    {
        u8 pixel;
        u8 *cache_tile_sliver = &tile_cache[tile_y * 8];

#define write_pixel(x)                                                          \
        pixel = cache_tile_sliver[x];                                           \
        if (pixel)                                                              \
        {                                                                       \
            if (!(tile_screen_ptr[x] & 0x80))                                   \
                tile_screen_ptr[x] = vce.palette_cache[pixel | current_palette];\
        }


        write_pixel(0)
        write_pixel(1)
        write_pixel(2)
        write_pixel(3)
        write_pixel(4)
        write_pixel(5)
        write_pixel(6)
        write_pixel(7)

        tile_screen_ptr = tile_screen_ptr + 512;
    }*/

	// Render tile
	//
    int height = tile_end_y - tile_start_y + 1;
	int x0 = x;
	int y0 = y + bg_depth;
	int x1 = x0 + 8;
	int y1 = y0 + height;

	int tx0 = 0;
	int ty0 = tile_start_y;
	int tx1 = 8;
	int ty1 = tile_end_y + 1;


	gpu3dsAddTileVertexes(
		x0, y0, x1, y1,
		tx0, ty0,
		tx1, ty1, texturePos);
    
}



void render_bg_block(int start_y, int end_y, int offset_y, vdc_struct *vdc, vdc_hw_struct *vdc_hw)
{
    u16 *bat_offset;

    u32 *screen_ptr = get_screen_ptr();
    
    u32 bg_width = (vdc->mwr >> 4) & 0x3;
    u32 bg_height = (vdc->mwr >> 6) & 0x1;
    
    u32 screen_width = vdc->screen_width;
    
    if(bg_width == 2)
        bg_width = 3;
    bg_width = (bg_width + 1) * 32;
    bg_height = ((bg_height + 1) * 32);

    //printf ("----------------- (%d, %d)\n", start_y, end_y);
    for (int y = start_y; y <= end_y; )
    {
        int x_scroll = vdc_hw->x_scroll[y];
        int y_scroll = vdc_hw->y_scroll[y];

        //printf ("line:%d scroll:%d,%d blank:%d\n", y, x_scroll, y_scroll, vdc_hw->blank_line[y]);

        int x_tile_offset = x_scroll / 8;
        int x_pixel_offset = x_scroll % 8;
        int y_tile_offset = y_scroll / 8;
        int y_pixel_offset = y_scroll % 8;
        int max_height = 8 - y_pixel_offset;

        int height = 1;
        for (; height < max_height; height++)
        {
            if (height == 8 ||
                vdc_hw->x_scroll[y + height - 1] != vdc_hw->x_scroll[y + height] ||
                vdc_hw->y_scroll[y + height - 1] + 1 != vdc_hw->y_scroll[y + height])
                break;
        }
        if (y + height - 1 > end_y)
            height = end_y - y + 1;
        //printf ("Render: %d %d-%d\n", y_scroll, y, y+height-1);

        // height - stores the height of the block we will render at y

        y_tile_offset &= (bg_height - 1);
        x_tile_offset &= (bg_width - 1);

        bat_offset = vdc->vram + (y_tile_offset * bg_width);
                                            
        u32 current_x = 32 - x_pixel_offset;

        // Loop through each tile in this horizontal block and
        // render them to the screen.
        //
        u32 screen_width_tiles = screen_width / 8;
        if(x_pixel_offset)
            screen_width_tiles++;

        while (screen_width_tiles)
        {
            u32 *tile_ptr;
            u32 bat_entry = bat_offset[x_tile_offset];
            u32 tile_number = bat_entry & 0xFFF;
            u32 current_palette = (bat_entry >> 12) << 4;

            // Render the tile to screen
            //
            render_bg_tile(
                vdc, screen_ptr, current_x, y, offset_y, 
                current_palette,
                tile_number, y_pixel_offset, y_pixel_offset + height - 1);
            
            x_tile_offset = (x_tile_offset + 1) & (bg_width - 1);
            current_x += 8;
            screen_width_tiles--;
        }

        y += height;
        //if (end_y == 239)
        //printf ("end %d\n", y);
        
    }
}


void render_spr_smalltile_clipy(
    int start_y, int end_y, int depth,
    int x0, int y0, int x1, int y1, int texturePos)
{
    int tx0 = 0, ty0 = 0;
    int tx1 = 8, ty1 = 8;

    if (y1 < start_y) return;
    if (y0 > end_y) return;

    if (y0 < start_y)
    {
        int d = start_y - y0;
        y0 = start_y;
        ty0 += d;
    }
    end_y++;
    if (y1 > end_y)
    {
        int d = y1 - end_y;
        y1 = end_y;
        ty1 -= d;
    }

    gpu3dsAddTileVertexes(x0, y0+depth, x1, y1+depth, tx0, ty0, tx1, ty1, texturePos);
}

void render_spr_tile (
    vdc_struct *vdc, u32 *screen_ptr, 
    int x, int y, int offset_y,
    int current_palette,
    int pattern_number, 
    int start_y, int end_y,
    int flip_x, int flip_y,
    u32 prio)
{
    //printf ("R @%3d,%3d - %4x,%d,%d\n", x, y, tile_number, tile_start_y, tile_end_y);

    //printf ("R @  %d\n", current_palette);
    //screen_ptr[0] = tile_number + 1;

    // Cache the sprite tile if it is marked dirty
    //
    if(vdc->dirty_patterns[pattern_number])
    {
        cache_pattern(vdc, pattern_number);
        vdc->dirty_patterns[pattern_number] = 0;

        for (int p = 16; p < 32; p++)
        {
            GPU3DSExt.VRAMPaletteFrame[pattern_number][p] = 0;
        }
        
    }

    // Cached tile position
    //
    u8 *pattern_cache = &vdc->pattern_cache[pattern_number * 256];
    int current_palette_idx = (current_palette >> 4) + 16;

    // Cache the BG tile in our texture.
    //
    int texturePos0 = cache3dsGetTexturePositionFast(pattern_number * 128 + 0,  current_palette_idx);
    int texturePos1 = cache3dsGetTexturePositionFast(pattern_number * 128 + 32, current_palette_idx);
    int texturePos2 = cache3dsGetTexturePositionFast(pattern_number * 128 + 64, current_palette_idx);
    int texturePos3 = cache3dsGetTexturePositionFast(pattern_number * 128 + 96, current_palette_idx);
    if (GPU3DSExt.VRAMPaletteFrame[pattern_number][current_palette_idx] != 
        GPU3DSExt.PaletteFrame[current_palette_idx])
    {
        GPU3DSExt.VRAMPaletteFrame[pattern_number][current_palette_idx] = GPU3DSExt.PaletteFrame[current_palette_idx];

        texturePos0 = cacheGetSwapTexturePositionForAltFrameFast(pattern_number * 128 + 0,  current_palette_idx);
        texturePos1 = cacheGetSwapTexturePositionForAltFrameFast(pattern_number * 128 + 32, current_palette_idx);
        texturePos2 = cacheGetSwapTexturePositionForAltFrameFast(pattern_number * 128 + 64, current_palette_idx);
        texturePos3 = cacheGetSwapTexturePositionForAltFrameFast(pattern_number * 128 + 96, current_palette_idx);

        u16 *palette_cache_5551_spr = &vce.palette_cache_5551[current_palette_idx * 16];
        cache3dsCacheTGFX16x16TileToTexturePosition(pattern_cache, palette_cache_5551_spr, 0, texturePos0);
        cache3dsCacheTGFX16x16TileToTexturePosition(pattern_cache, palette_cache_5551_spr, 1, texturePos1);
        cache3dsCacheTGFX16x16TileToTexturePosition(pattern_cache, palette_cache_5551_spr, 2, texturePos2);
        cache3dsCacheTGFX16x16TileToTexturePosition(pattern_cache, palette_cache_5551_spr, 3, texturePos3);
    }

/*
    // Clip y
    //
    int height = 8;
    int tile_start_y = 0;
    int tile_end_y = 15;
    if (y < start_y)
    {
        tile_start_y += (start_y - y);
        height -= (start_y - y);
        y = start_y;
    }
    if (height > 0 && y + height > end_y)
    {
        tile_end_y -= (y + height - end_y);
        height -= (y + height - end_y);
    }
*/
    /*
    u32 *tile_screen_ptr = &screen_ptr[y * 512 + x];

    // Loop each line and render to screen
    //
    if (!flip_x)
    {
        for (int tile_y = tile_start_y; tile_y <= tile_end_y; tile_y++)
        {
            u8 pixel;
            int ty = tile_y;
            if (flip_y)
                ty = 15 - ty;
            u8 *cache_pattern_sliver = &pattern_cache[ty * 16];

    #define write_pixel(x)                                                  \
            pixel = cache_pattern_sliver[x];                                \
            if (pixel)                                                      \
                tile_screen_ptr[x] = vce.palette_cache[pixel | current_palette | 0x100] | prio; 
            
            write_pixel(0)
            write_pixel(1)
            write_pixel(2)
            write_pixel(3)
            write_pixel(4)
            write_pixel(5)
            write_pixel(6)
            write_pixel(7)
            write_pixel(8)
            write_pixel(9)
            write_pixel(10)
            write_pixel(11)
            write_pixel(12)
            write_pixel(13)
            write_pixel(14)
            write_pixel(15)

            tile_screen_ptr = tile_screen_ptr + 512;
        }
    }
    else
    {
        for (int tile_y = tile_start_y; tile_y <= tile_end_y; tile_y++)
        {
            u8 pixel;
            int ty = tile_y;
            if (flip_y)
                ty = 15 - ty;
            u8 *cache_pattern_sliver = &pattern_cache[ty * 16];

    #define write_pixel(x)                                                  \
            pixel = cache_pattern_sliver[15-x];                             \
            if (pixel)                                                      \
                tile_screen_ptr[x] = vce.palette_cache[pixel | current_palette | 0x100] | prio; 
            
            write_pixel(0)
            write_pixel(1)
            write_pixel(2)
            write_pixel(3)
            write_pixel(4)
            write_pixel(5)
            write_pixel(6)
            write_pixel(7)
            write_pixel(8)
            write_pixel(9)
            write_pixel(10)
            write_pixel(11)
            write_pixel(12)
            write_pixel(13)
            write_pixel(14)
            write_pixel(15)

            tile_screen_ptr = tile_screen_ptr + 512;
        }
    }
    */

	// Render tile
	//
    int height = 8;
	int x0 = x;
	int y0 = y;
	int x1 = x0 + 8;
	int y1 = y0 + 8;

	int tx0 = 0;
	int ty0 = 0;
	int tx1 = 8;
	int ty1 = 8;

    int flip = (flip_x | (flip_y << 1));
    int flipshl14 = flip << 14;

    switch (flip)
    {
        case 0: // no flip
            render_spr_smalltile_clipy(start_y, end_y, spr_depth[prio], x0+0, y0+0, x1+0, y1+0, texturePos0 + flipshl14);
            render_spr_smalltile_clipy(start_y, end_y, spr_depth[prio], x0+8, y0+0, x1+8, y1+0, texturePos1 + flipshl14);
            render_spr_smalltile_clipy(start_y, end_y, spr_depth[prio], x0+0, y0+8, x1+0, y1+8, texturePos2 + flipshl14);
            render_spr_smalltile_clipy(start_y, end_y, spr_depth[prio], x0+8, y0+8, x1+8, y1+8, texturePos3 + flipshl14);
            break;
        case 1: // flip x
            render_spr_smalltile_clipy(start_y, end_y, spr_depth[prio], x0+0, y0+0, x1+0, y1+0, texturePos1 + flipshl14);
            render_spr_smalltile_clipy(start_y, end_y, spr_depth[prio], x0+8, y0+0, x1+8, y1+0, texturePos0 + flipshl14);
            render_spr_smalltile_clipy(start_y, end_y, spr_depth[prio], x0+0, y0+8, x1+0, y1+8, texturePos3 + flipshl14);
            render_spr_smalltile_clipy(start_y, end_y, spr_depth[prio], x0+8, y0+8, x1+8, y1+8, texturePos2 + flipshl14);
            break;
        case 2: // flip y
            render_spr_smalltile_clipy(start_y, end_y, spr_depth[prio], x0+0, y0+0, x1+0, y1+0, texturePos2 + flipshl14);
            render_spr_smalltile_clipy(start_y, end_y, spr_depth[prio], x0+8, y0+0, x1+8, y1+0, texturePos3 + flipshl14);
            render_spr_smalltile_clipy(start_y, end_y, spr_depth[prio], x0+0, y0+8, x1+0, y1+8, texturePos0 + flipshl14);
            render_spr_smalltile_clipy(start_y, end_y, spr_depth[prio], x0+8, y0+8, x1+8, y1+8, texturePos1 + flipshl14);
            break;
        case 3: // flip x+y
            render_spr_smalltile_clipy(start_y, end_y, spr_depth[prio], x0+0, y0+0, x1+0, y1+0, texturePos3 + flipshl14);
            render_spr_smalltile_clipy(start_y, end_y, spr_depth[prio], x0+8, y0+0, x1+8, y1+0, texturePos2 + flipshl14);
            render_spr_smalltile_clipy(start_y, end_y, spr_depth[prio], x0+0, y0+8, x1+0, y1+8, texturePos1 + flipshl14);
            render_spr_smalltile_clipy(start_y, end_y, spr_depth[prio], x0+8, y0+8, x1+8, y1+8, texturePos0 + flipshl14);
            break;
    }
}

int spr_x1[2][2] = { 
    { 00, 00 },         // width = 16 
    { 00, 16 }          // width = 32
};
int spr_x2[2][2] = {
    { -1, -1 },         // width = 16 
    { 16, 00 }          // width = 32
};
int spr_y1[4][2] = { 
    { 00, 00 },         // height = 16 
    { 00, 16 },         // height = 32
    { 00, 48 },         // height = 64
    { 00, 48 }          // height = 64
};
int spr_y2[4][2] = { 
    { -1, -1 },         // height = 16 
    { 16, 00 },         // height = 32
    { 16, 32 },         // height = 64
    { 16, 32 }          // height = 64
};
int spr_y3[4][2] = { 
    { -1, -1 },         // height = 16 
    { -1, -1 },         // height = 32
    { 32, 16 },         // height = 64
    { 32, 16 }          // height = 64
};
int spr_y4[4][2] = { 
    { -1, -1 },         // height = 16 
    { -1, -1 },         // height = 32
    { 48, 00 },         // height = 64
    { 48, 00 }          // height = 64
};

void render_spr_block(int start_y, int end_y, int offset_y, vdc_struct *vdc, vdc_hw_struct *vdc_hw)
{
    u32 *screen_ptr = get_screen_ptr();
    u32 screen_width = vdc->screen_width;

    u16 *satb_location = vdc->sat;
    u16 *current_sprite = satb_location;

    for(int i = 63; i >= 0; i--)
    {
        current_sprite = &satb_location[4 * i];
        int y = (current_sprite[0] & 0x3FF) - 64 + offset_y;
        int x = (current_sprite[1] & 0x3FF);
        int attributes = current_sprite[3] & 0xB98F;
        int cgx = ((attributes >> 8) & 0x1);
        int cgy = ((attributes >> 12) & 0x3);
        int flip_x = (attributes >> 11) & 1;
        int flip_y = (attributes >> 15) & 1;
        int current_palette = (attributes & 0xF) << 4;
        u32 prio = (attributes & 0x80) >> 7;

        /* cgy 2 is not supposed to be valid, but really it's like 3 */
        if(cgy == 2)
            cgy = 3;
        
        int width = (cgx + 1) * 16;
        int height = (cgy + 1) * 16;

        if (y + height - 1 < start_y || y > end_y)
            continue;

        int tile_hw = cgy * 2 + cgx;

        int pattern_number = (current_sprite[2] >> 1) & 0x3FF;
        pattern_number &= ~cgx;
        if(cgy > 0)
          pattern_number &= ~0x2;
        if(cgy > 1)
          pattern_number &= ~0x4;
        if(pattern_number >= 0x200)
          continue; 

#define render_spr_tile_16x16(xa, ya, p)                                                \
    render_spr_tile(vdc, screen_ptr,                                                    \
        x + spr_##xa[cgx][flip_x], y + spr_##ya[cgy][flip_y], 0,                        \  
        current_palette, pattern_number + p, start_y, end_y, flip_x, flip_y, prio);     \

        switch (tile_hw)
        {
            case 0x00:
                // 16x16
                render_spr_tile_16x16(x1, y1, 0);
                break;
            case 0x01:
                // 32x16
                render_spr_tile_16x16(x1, y1, 0);
                render_spr_tile_16x16(x2, y1, 1);
                break;
            case 0x02:
                // 16x32
                render_spr_tile_16x16(x1, y1, 0);
                render_spr_tile_16x16(x1, y2, 2);
                break;
            case 0x03:
                // 32x32
                render_spr_tile_16x16(x1, y1, 0);
                render_spr_tile_16x16(x2, y1, 1);
                render_spr_tile_16x16(x1, y2, 2);
                render_spr_tile_16x16(x2, y2, 3);
                break;
            case 0x04:
            case 0x06:
                // 16x64
                render_spr_tile_16x16(x1, y1, 0);
                render_spr_tile_16x16(x1, y2, 2);
                render_spr_tile_16x16(x1, y3, 4);
                render_spr_tile_16x16(x1, y4, 6);
                break;
            case 0x05:
            case 0x07:
                // 32x64
                render_spr_tile_16x16(x1, y1, 0);
                render_spr_tile_16x16(x2, y1, 1);
                render_spr_tile_16x16(x1, y2, 2);
                render_spr_tile_16x16(x2, y2, 3);
                render_spr_tile_16x16(x1, y3, 4);
                render_spr_tile_16x16(x2, y3, 5);
                render_spr_tile_16x16(x1, y4, 6);
                render_spr_tile_16x16(x2, y4, 7);
                break;
        }
    }
}


void render_clear_screen_hw(int start_y, int end_y)
{
    u32 back_color = vce.palette_cache[0] | 0xff;
    
    gpu3dsDisableAlphaTest();
    gpu3dsDisableDepthTest();
    gpu3dsSetTextureEnvironmentReplaceColor();
    gpu3dsDrawRectangle(0, start_y, 512, end_y + 1, 0, back_color);

}


void render_flush(vdc_struct *vdc, vdc_hw_struct *vdc_hw)
{
    int scanline_line = vce.frame_counter - 14 - 1;
    if (scanline_line < 0)
        return;
    if (scanline_line > 239)
        scanline_line = 239;

    int offset_y = vdc->start_line - 14;

    //printf ("flush @ %3d: %3d-%3d %3d sp:%d bg:%d\n", vce.frame_counter, vdc_hw->start_render_line, scanline_line, vdc_a.start_line, vdc_hw->prev_spr, vdc_hw->prev_bg);

    
    render_clear_screen_hw(vdc_hw->start_render_line, scanline_line);

    gpu3dsSetTextureEnvironmentReplaceTexture0();
    gpu3dsBindTexture(emuTileCacheTexture, GPU_TEXUNIT0);

    gpu3dsDisableAlphaTest();
    gpu3dsDisableDepthTest();

    if (vdc_hw->prev_spr) 
    {
        gpu3dsEnableAlphaTestNotEqualsZero();
        gpu3dsDisableDepthTest();
        render_spr_block(vdc_hw->start_render_line, scanline_line, offset_y, vdc, vdc_hw);
        gpu3dsDrawVertexes();
    }

    if (vdc_hw->prev_bg) 
    {
        gpu3dsEnableAlphaTestNotEqualsZero();
        gpu3dsEnableDepthTest();
        render_bg_block(vdc_hw->start_render_line, scanline_line, 0, vdc, vdc_hw);
        gpu3dsDrawVertexes();
    }
   
    
    // For debugging only - Tells us where is the last scanline
    // of this refresh.
    //
    gpu3dsDisableAlphaTest();
    gpu3dsDisableDepthTest();
    gpu3dsSetTextureEnvironmentReplaceColor();
    gpu3dsDrawRectangle(0, scanline_line, 200, scanline_line + 1, 0, 0xffffffff);
    
    vdc_hw->start_render_line = scanline_line + 1;
}


void render_flush_on_state_changed(vdc_struct *vdc, vdc_hw_struct *vdc_hw)
{
    if ((vdc_hw->cur_spr != vdc_hw->prev_spr) ||
        (vdc_hw->cur_bg != vdc_hw->prev_bg))
    {
        render_flush(vdc, vdc_hw);
    }
}


u32 reverse(register u32 x)
{
    x = (((x & 0xaaaaaaaa) >> 1) | ((x & 0x55555555) << 1));
    x = (((x & 0xcccccccc) >> 2) | ((x & 0x33333333) << 2));
    x = (((x & 0xf0f0f0f0) >> 4) | ((x & 0x0f0f0f0f) << 4));
    x = (((x & 0xff00ff00) >> 8) | ((x & 0x00ff00ff) << 8));
    return((x >> 16) | (x << 16));

}

// Forms a sprite's bit mask.
//
u32 vdc_check_collision_form_bitmask(vdc_struct *vdc, sat_cache_line_entry_struct* sat_cache_entry)
{
    int pattern_number = sat_cache_entry->pattern_line_offset / 16;
    int pattern_y = sat_cache_entry->pattern_line_offset % 16;

    // cache sprite patterns if necessary
    //
    if (vdc->dirty_patterns[pattern_number])
    {
        cache_pattern(vdc, pattern_number);
        vdc->dirty_patterns[pattern_number] = 0;
    }

    u16 *pattern_cache_transparency_mask_offset =
        vdc->pattern_cache_transparency_mask + ((pattern_number * 16 + pattern_y) * 2);

    u16 *vram_offset = vdc->vram + (pattern_number * 64 + pattern_y);

    u32 bits =
        (vram_offset[0] |
        vram_offset[16] |
        vram_offset[32] |
        vram_offset[48]) << 16;
    
    if (sat_cache_entry->attributes & SPRITE_ATTRIBUTE_WIDE)
    {
        // cache sprite patterns if necessary
        //
        if (vdc->dirty_patterns[pattern_number + 1])
        {
            cache_pattern(vdc, pattern_number + 1);
            vdc->dirty_patterns[pattern_number + 1] = 0;

            for (int p = 16; p < 32; p++)
            {
                GPU3DSExt.VRAMPaletteFrame[pattern_number][p] = 0;
            }
        }

        vram_offset = vram_offset + 64;
        bits |= 
            (vram_offset[0] |
            vram_offset[16] |
            vram_offset[32] |
            vram_offset[48]);
    }

    if (sat_cache_entry->attributes & SPRITE_ATTRIBUTE_HFLIP)
    {
       bits = reverse(bits);
        if (!(sat_cache_entry->attributes & SPRITE_ATTRIBUTE_WIDE))
            bits = bits << 16;
    }
    return bits;
}


void vdc_check_collision(vdc_struct *vdc, int screen_line)
{
    sat_cache_line_struct *sat_cache_line = &vdc->sat_cache.lines[screen_line];
    sat_cache_line_entry_struct *sat_cache_entry;
    
    // Sprite 0 is not available in this line.
    // So we return.
    //
    if (!(sat_cache_line->flags & LINE_CACHE_FLAGS_SPR0))
        return false;

    // No sprites, or only sprite 0 is available in this line.
    // So we return.
    //
    if (sat_cache_line->num_active <= 1)
        return false;

    // The first sprite is sprite 0. Form the bit mask at this
    // line.
    //
    int spr0_width = 0;
    u32 spr0_bits = 0;
    int spr0_x1 = 0;
    int spr0_x2 = 0;
    for (int i = 0; i < 1; i++)
    {
        sat_cache_entry = &(sat_cache_line->entries[i]);

        if (sat_cache_entry->attributes & SPRITE_ATTRIBUTE_WIDE)
            spr0_width = 32;
        else
            spr0_width = 16;

        spr0_x1 = sat_cache_entry->x;
        spr0_x2 = sat_cache_entry->x + spr0_width - 1;
        
        spr0_bits = vdc_check_collision_form_bitmask(vdc, sat_cache_entry);
        
        /*
        printf ("%2x,%2x:",sat_cache_entry->x, screen_line);
        for (int b = 0; b < 32; b++)
            if ((spr0_bits >> (31-b)) & 0x1)
                printf ("1");
            else
                printf (".");
        printf ("\n");
        */
    }

    // If sprite 0 at this line has no non-zero pixels,
    // it's not going to collide with anything, so we will
    // skip the collision check.
    //
    if (spr0_bits == 0)
        return false;

    // Here we do our collision test
    //
    bool spr0_collision = false;
    int testspr_x1 = 0;
    int testspr_x2 = 0;
    int testspr_width = 0;
    u32 testspr_bits = 0;
    for (int i = 1; i < sat_cache_line->num_active; i++)
    {
        sat_cache_entry = &(sat_cache_line->entries[i]);

        if (sat_cache_entry->attributes & SPRITE_ATTRIBUTE_WIDE)
            testspr_width = 32;
        else
            testspr_width = 16;

        testspr_x1 = sat_cache_entry->x;
        testspr_x2 = sat_cache_entry->x + testspr_width - 1;

        // First we test if the test sprite's rectangle hits
        // the sprite 0's rectangle. If the test sprites falls
        // outside, then skip the test.
        //
        if (testspr_x2 < spr0_x1 || testspr_x1 > spr0_x2)
            continue;

        // Construct the test sprite's bits.
        //
        testspr_bits = vdc_check_collision_form_bitmask(vdc, sat_cache_entry);
        if (testspr_x1 < spr0_x1)
            testspr_bits = testspr_bits << (spr0_x1 - testspr_x1);
        else
            testspr_bits = testspr_bits >> (spr0_x1 - testspr_x1);

        if (testspr_bits & spr0_bits)
        {
            /*
            printf ("collide:\n");
            printf ("%2x,%2x:",sat_cache_entry->x, screen_line);
            for (int b = 0; b < 32; b++)
                if ((spr0_bits >> (31-b)) & 0x1)
                    printf ("1");
                else
                    printf (".");
            printf ("\n");
            printf ("      ");
            for (int b = 0; b < 32; b++)
                if ((testspr_bits >> (31-b)) & 0x1)
                    printf ("1");
                else
                    printf (".");
            printf ("\n");
            */
            spr0_collision = true;
            break;
        }
    }

    if (spr0_collision)
    {
        vdc->status |= VDC_STATUS_SPRITE_COLLISION_IRQ;
        raise_interrupt(IRQ_VDC);
    }
    return false;
}

void vdc_check_line_hw(vdc_struct *vdc)
{
    //printf ("cd:%x\n", cd.scsi_bus_state);
    if((vdc->display_counter >= vdc->start_line) &&
        (vdc->display_counter < vdc->vblank_line) &&
        (vdc->burst_mode == 0))
    {    
        u32 screen_line = vdc->raster_line;

        // Check for overflow
        //
        if((vdc->cr & 0x40))
            vdc_check_spr_limit(vdc, screen_line);

        // Check for collision
        if((vdc->cr & 0x1))
        {
            vdc_check_collision(vdc, screen_line);
        }

    } 
}

void render_line_hw(void)
{
    //printf ("%3d: %3d %3d %02x\n", vce.frame_counter, vdc_a.raster_line, vdc_a.start_line, vdc_a.cr);
    if (vce.frame_counter == 0)
    {
        vdc_hw_a.start_render_line = 0; 

    }

    if ((vce.frame_counter >= 14) &&
        (vce.frame_counter < (14 + RESOLUTION_HEIGHT)))
    {
        copy_line_data(&vdc_a, &vdc_hw_a);
        render_flush_on_state_changed(&vdc_a, &vdc_hw_a);
        vdc_check_line_hw (&vdc_a);
    }

    if (vce.frame_counter == (14 + RESOLUTION_HEIGHT))
    {
        vsectCommit(&screenWidthVerticalSection, RESOLUTION_HEIGHT);
        render_flush(&vdc_a, &vdc_hw_a);
    }

}
