u32 *render_bg_line(vdc_struct *vdc, u8 **bg_buffer_result)
{
  u8 *bg_buffer = vdc->bg_buffer + 512 + 512;
  u8 *dest = bg_buffer;

  u32 mwr = vdc->mwr;

  u32 bg_width = vdc->mwr >> 4;
  bg_width = bg_width & 0x3;

  u32 bg_height = vdc->mwr >> 6;
  bg_height = bg_height & 0x1;

  u32 screen_width = vdc->screen_width;
  u32 screen_width_tiles = screen_width >> 3;

  u32 screen_width_mask_blocks;

  u8 *transparency_mask_dest =
   ((u8 *)vdc->bg_mask_buffer) + ((512 + 512) / 8);
  u8 *transparency_mask_blocks = transparency_mask_dest;
  u8 *transparency_mask_buffer_result;

  u32 effective_byr = vdc->effective_byr;
  u32 y_tile_offset = effective_byr >> 3;
  u32 y_pixel_offset = effective_byr & 0x7;

  u8 *bat_offset;
  u8 *tile_cache = vdc->tile_cache;
  u8 *tile_base = tile_cache + (y_pixel_offset << 3);

  u8 *tile_cache_transparency_mask = vdc->tile_cache_transparency_mask; 
  u8 *transparency_mask_base =
   tile_cache_transparency_mask + y_pixel_offset;

  u32 bxr = vdc->bxr;
  u32 scroll_shift = vdc->scroll_shift;  
  u32 x_scroll = bxr + scroll_shift;

  u32 x_tile_offset, x_pixel_offset;
  // Scanline offset is the amount into the scanline the active
  // display starts at.
  s32 scanline_offset = vdc->scanline_offset;
  u32 tile_mask_shift;

  x_tile_offset = x_scroll >> 3;
  x_pixel_offset = x_scroll & 0x7;

  scanline_offset -= x_pixel_offset;
  u8 *bg_buffer_shifted = bg_buffer - scanline_offset;
  *bg_buffer_result = bg_buffer_shifted;

  if(x_pixel_offset)
    screen_width_tiles++;

  screen_width_mask_blocks = screen_width_tiles + 3;
  screen_width_mask_blocks = screen_width_mask_blocks >> 2;

  if(bg_width == 2)
    bg_width = 1;

  bg_width++;
  u32 bg_width_shift = bg_width + 5;
  bg_width = bg_width << 5;

  bg_height++; 
  bg_height = bg_height << 5;
  bg_height--;
                                     
  y_tile_offset = y_tile_offset & bg_height;

  u8 *vram = vdc->vram;
  bat_offset = vram;
  bat_offset = bat_offset + (y_tile_offset << bg_width_shift);

  bg_width--;

  x_tile_offset = x_tile_offset & bg_width;

  if(screen_width_tiles == 0)
    goto _1a;

  u8 *dirty_tiles = vdc->dirty_tiles;

  // Live variables:
  //  vdc (r0)
  //  bat_offset
  //  x_tile_offset
  //  dirty_tiles
  //  dest
  //  transparency_mask_base
  //  transparency_mask_dest
  //  screen_width_tiles
  //  bg_width

  // save + restore these for later half
  //  scanline_offset
  //  bg_buffer
  //  transparency_mask_blocks
  //  x_pixel_offset

  _0a:
  {
    u8 *tile_ptr;
    u32 bat_entry = *(u16 *)(bat_offset + (x_tile_offset << 1));
    u32 tile_number = bat_entry & 0xFFF;
    u32 current_palette = bat_entry >> 12;
    current_palette = current_palette << 4;
 
    current_palette = current_palette | (current_palette << 8);
    current_palette = current_palette | (current_palette << 16);

    u32 dirty_value = dirty_tiles[tile_number];

    if(dirty_value)
    {
      cache_tile(vdc, tile_number);
      dirty_tiles[tile_number] = 0;
    }

    tile_ptr = tile_base + (tile_number << 6);

    u32 half_a = tile_ptr[0];
    u32 half_b = tile_ptr[1];

    half_a |= current_palette;
    half_b |= current_palette;

    dest[0] = half_a;
    dest[1] = half_b;

    u8 transparency_mask_value =
     *(transparency_mask_base + (tile_number << 3));
    *transparency_mask_dest = transparency_mask_value;

    x_tile_offset = x_tile_offset + 1;
    x_tile_offset = x_tile_offset & bg_width;

    dest += 2;
    transparency_mask_dest++;

    screen_width_tiles--;

    if(screen_width_tiles > 0)
      goto 0a;
  }

 _1a:

  // Clamp off edges if necessary, by clearing at least 8 pixels around them.
  if(scanline_offset <= 0)
    goto _0b;

  {
    u32 left_pixel_offset = x_pixel_offset;
    u32 *bg_buffer_clear = bg_buffer;

    if(left_pixel_offset > 3)
    {
      *bg_buffer_clear = 0;
      bg_buffer_clear++;
      left_pixel_offset -= 4;
    }

    u32 mask = 0xFFFFFFFF;
    u32 shift = left_pixel_offset << 3;
    mask = mask << shift;

    u32 bg_buffer_clear_value = *bg_buffer_clear;
    bg_buffer_clear_value &= mask;
    *bg_buffer_clear = bg_buffer_clear_value;
  }

 _0b:

  u32 screen_end = vdc->screen_end;
  u32 screen_width = vce.screen_width;

  if(screen_end >= screen_width)
    goto _0c:

  {
    u32 *bg_buffer_clear = bg_buffer;
    u32 screen_width_shift = screen_width >> 2;
    bg_buffer_clear += screen_width_shift;

    if(x_pixel_offset >= 4)
    {
      bg_buffer_clear++;
      x_pixel_offset -= 4;
    }

    bg_buffer_clear[1] = 0;

    u32 mask = 0xFFFFFFFF;
    u32 shift = x_pixel_offset << 3;
    mask = mask << shift;

    u32 bg_buffer_clear_value = *bg_buffer_clear;
    bg_buffer_clear_value &= ~mask;
    *bg_buffer_clear = bg_buffer_clear_value;
  }

 _0c:

  if(scanline_offset >= 0)
    goto _3a;

  {
    if(screen_width_mask_blocks == 0)
      goto _3a;

    u32 tile_mask_shift = -scanline_offset;
    u32 tile_mask_shift_inverse = 32 - tile_mask_shift;

    transparency_mask_buffer_result = transparency_mask_blocks;

    _0d:
    {
      u32 transparency_mask = *((u32 *)transparency_mask_blocks);
      transparency_mask = transparency_mask >> tile_mask_shift;
      u32 transparency_mask_b = *((u32 *)(transparency_mask_blocks + 4));
      transparency_mask_b = transparency_mask_b << tile_mask_shift_inverse;

      transparency_mask = transparency_mask | transparency_mask_b;
      
      *transparency_mask_blocks = transparency_mask;
  
      transparency_mask_blocks++;
      screen_width_mask_blocks--;

      if(screen_width_mask_blocks > 0)
        goto _0d;
    }
  }
  goto _4a;

 _3a:
  {
    tile_mask_shift = (u32)scanline_offset % 32;

    if(tile_mask_shift == 0)
      goto _4a;

    u32 tile_mask_shift_inverse = 32 - tile_mask_shift;

    transparency_mask_buffer_result = transparency_mask_blocks;
    u32 scanline_offset32 = scanline_offset >> 5;
    transparency_mask_buffer_result =
     transparency_mask_buffer_result - ((scanline_offset32) << 2);

    {
      transparency_mask_blocks =
       transparency_mask_blocks + (screen_width_mask_blocks << 2);

      screen_width_mask_blocks = screen_width_mask_blocks + 1;

      _0e:
      {
        u32 transparency_mask = *((u32 *)transparency_mask_blocks);
        transparency_mask = transparency_mask << tile_mask_shift;
        u32 transparency_mask_b = *((u32 *)transparency_mask_blocks - 4);
        transparency_mask = 
         transparency_mask | (transparency_mask_b >> tile_mask_shift_inverse);
      
        *((u32 *)transparency_mask_blocks) = transparency_mask;
  
        transparency_mask_blocks--;
        screen_width_mask_blocks--;

        if(screen_width_mask_blocks == 0)
          goto _0e;
      }
    }
  }

 _4a:
  return transparency_mask_buffer_result;
}

u32 *render_bg_line_blank(vdc_struct *vdc, u8 **bg_buffer_result)
{
  u32 screen_width_mask_blocks = (vce.screen_width + 31) / 32;
  u8 *bg_buffer = vdc->bg_buffer + 512 + 512;
  u32 *transparency_mask_buffer =
   vdc->bg_mask_buffer + ((512 + 512) / 32);

  memset(bg_buffer, 0, vdc->screen_width);

  while(screen_width_mask_blocks)
  {
    *transparency_mask_buffer = 0; 
    transparency_mask_buffer++;
    screen_width_mask_blocks--;
  }

  *bg_buffer_result = bg_buffer;
  return transparency_mask_buffer;
}

#define render_spr_row_pixels_no_flip()                                       \
  for(i2 = 0; i2 < 16; i2++)                                                  \
  {                                                                           \
    current_pixel = current_row_ptr[i2];                                      \
    if(current_pixel)                                                         \
      *dest = current_pixel | current_palette;                                \
    dest++;                                                                   \
  }                                                                           \

#define render_spr_row_pixels_flip()                                          \
  for(i2 = 15; i2 >= 0; i2--)                                                 \
  {                                                                           \
    current_pixel = current_row_ptr[i2];                                      \
    if(current_pixel)                                                         \
      *dest = current_pixel | current_palette;                                \
    dest++;                                                                   \
  }                                                                           \


#define render_spr_check_dirty(_pattern_offset)                               \
  if(vdc->dirty_patterns[_pattern_offset])                                    \
  {                                                                           \
    cache_pattern(vdc, _pattern_offset);                                      \
    vdc->dirty_patterns[_pattern_offset] = 0;                                 \
  }                                                                           \

#define render_spr_pattern_row(offset, flip_type)                             \
  current_row_ptr =                                                           \
   vdc->pattern_cache + ((pattern_line_offset + offset) * 16);                \
  render_spr_row_pixels_##flip_type()                                         \


#define render_spr_row_wide_a_flip()                                          \
  render_spr_pattern_row(16, flip)                                            \

#define render_spr_row_wide_b_flip()                                          \
  render_spr_pattern_row(0, flip)                                             \

#define render_spr_row_wide_a_no_flip()                                       \
  render_spr_pattern_row(0, no_flip)                                          \

#define render_spr_row_wide_b_no_flip()                                       \
  render_spr_pattern_row(16, no_flip)                                         \


#define render_spr_load_transparency_mask_wide_no_flip()                      \
  transparency_mask =                                                         \
   vdc->pattern_cache_transparency_mask[pattern_line_offset * 2];             \
  transparency_mask |=                                                        \
   vdc->pattern_cache_transparency_mask[(pattern_line_offset + 16) * 2] << 16 \

#define render_spr_load_transparency_mask_wide_flip()                         \
  transparency_mask =                                                         \
   vdc->pattern_cache_transparency_mask[((pattern_line_offset + 16) * 2) + 1];\
  transparency_mask |=                                                        \
   vdc->pattern_cache_transparency_mask[(pattern_line_offset * 2) + 1] << 16  \


#define render_spr_collision_check_yes(_transparency_mask)                    \
  spr0_collision |= (obj_low_transparency_mask_dest[block_offset] |           \
   obj_high_transparency_mask_dest[block_offset]) & _transparency_mask        \

#define render_spr_collision_check_no(_transparency_mask)


#define render_spr_mark_transparency_low(_transparency_mask, collision_check) \
  /* A low priority sprite that's drawn after high priority sprites will    */\
  /* mask off those sprites.                                                */\
  obj_low_transparency_mask_dest[block_offset] |= _transparency_mask;         \
  obj_high_transparency_mask_dest[block_offset] &= ~_transparency_mask;       \
  render_spr_collision_check_##collision_check(_transparency_mask)            \

#define render_spr_mark_transparency_high(_transparency_mask,                 \
 collision_check)                                                             \
  obj_high_transparency_mask_dest[block_offset] |= _transparency_mask;        \
  render_spr_collision_check_##collision_check(_transparency_mask)            \


#define render_spr_row_wide(priority, flip_type, collision_check)             \
do                                                                            \
{                                                                             \
  u32 pattern_offset_a = pattern_line_offset / 16;                            \
  u32 pattern_offset_b = pattern_offset_a + 1;                                \
  render_spr_check_dirty(pattern_offset_a);                                   \
  render_spr_check_dirty(pattern_offset_b);                                   \
                                                                              \
  render_spr_load_transparency_mask_wide_##flip_type();                       \
  /* The transparency mask spans either 1 or 2 32-pixel blocks, but almost  */\
  /* always 2 unless the sprite is aligned with a 32-pixel boundary.        */\
  u32 transparency_mask_a = transparency_mask << block_pixel_offset;          \
  render_spr_mark_transparency_##priority(transparency_mask_a,                \
   collision_check);                                                          \
  if(block_pixel_offset)                                                      \
  {                                                                           \
    u32 transparency_mask_b = transparency_mask >> (32 - block_pixel_offset); \
    block_offset++;                                                           \
    render_spr_mark_transparency_##priority(transparency_mask_b,              \
     collision_check);                                                        \
  }                                                                           \
  render_spr_row_wide_a_##flip_type();                                        \
  render_spr_row_wide_b_##flip_type();                                        \
} while(0)                                                                    \


#define render_spr_load_transparency_mask_no_flip()                           \
  transparency_mask =                                                         \
   vdc->pattern_cache_transparency_mask[pattern_line_offset * 2]              \

#define render_spr_load_transparency_mask_flip()                              \
  transparency_mask =                                                         \
   vdc->pattern_cache_transparency_mask[(pattern_line_offset * 2) + 1]        \

#define render_spr_row_single(priority, flip_type, collision_check)           \
do                                                                            \
{                                                                             \
  u32 pattern_offset = pattern_line_offset / 16;                              \
  render_spr_check_dirty(pattern_offset);                                     \
                                                                              \
  render_spr_load_transparency_mask_##flip_type();                            \
  /* The transparency mask spans either 1 or 2 32-pixel blocks.             */\
  /* It spans two if the block pixel offset passes the second half of the   */\
  /* pixel block, so half of the time.                                      */\
  if(block_pixel_offset > 16)                                                 \
  {                                                                           \
    u32 transparency_mask_a;                                                  \
    u32 transparency_mask_b;                                                  \
                                                                              \
    transparency_mask_a = transparency_mask << block_pixel_offset;            \
    transparency_mask_b = transparency_mask >> (32 - block_pixel_offset);     \
                                                                              \
    render_spr_mark_transparency_##priority(transparency_mask_a,              \
     collision_check);                                                        \
    block_offset++;                                                           \
    render_spr_mark_transparency_##priority(transparency_mask_b,              \
     collision_check);                                                        \
  }                                                                           \
  else                                                                        \
  {                                                                           \
    transparency_mask <<= block_pixel_offset;                                 \
    render_spr_mark_transparency_##priority(transparency_mask,                \
     collision_check);                                                        \
  }                                                                           \
  render_spr_pattern_row(0, flip_type);                                       \
} while(0)                                                                    \


#define render_spr_row(priority, collision_check)                             \
do                                                                            \
{                                                                             \
  if(attributes & SPRITE_ATTRIBUTE_HFLIP_CHECK)                               \
  {                                                                           \
    if(attributes & SPRITE_ATTRIBUTE_WIDE)                                    \
      render_spr_row_wide(priority, flip, collision_check);                   \
    else                                                                      \
      render_spr_row_single(priority, flip, collision_check);                 \
  }                                                                           \
  else                                                                        \
  {                                                                           \
    if(attributes & SPRITE_ATTRIBUTE_WIDE)                                    \
      render_spr_row_wide(priority, no_flip, collision_check);                \
    else                                                                      \
      render_spr_row_single(priority, no_flip, collision_check);              \
  }                                                                           \
} while(0)                                                                    \


#define render_spr_line_entry(entry_index, collision_check)                   \
  sat_cache_entry = &(sat_cache_line->entries[entry_index]);                  \
  attributes = sat_cache_entry->attributes;                                   \
  current_palette = (attributes & 0xF) << 4;                                  \
  x = sat_cache_entry->x;                                                     \
  pattern_line_offset = sat_cache_entry->pattern_line_offset;                 \
                                                                              \
  block_offset = x >> 5;                                                      \
  block_pixel_offset = x & 0x1F;                                              \
                                                                              \
  dest = obj_dest + x;                                                        \
                                                                              \
  switch((attributes >> 6) & 0x7)                                             \
  {                                                                           \
    case 0x0:                                                                 \
      /* No hflip, low priority, narrow                                     */\
      render_spr_row_single(low, no_flip, collision_check);                   \
      break;                                                                  \
                                                                              \
    case 0x1:                                                                 \
      /* hflip, low priority, narrow                                        */\
      render_spr_row_single(low, flip, collision_check);                      \
      break;                                                                  \
                                                                              \
    case 0x2:                                                                 \
      /* No hflip, high priority, narrow                                    */\
      render_spr_row_single(high, no_flip, collision_check);                  \
      break;                                                                  \
                                                                              \
    case 0x3:                                                                 \
      /* hflip, high priority, narrow                                       */\
      render_spr_row_single(high, flip, collision_check);                     \
      break;                                                                  \
                                                                              \
    case 0x4:                                                                 \
      /* No hflip, low priority, wide                                       */\
      render_spr_row_wide(low, no_flip, collision_check);                     \
      break;                                                                  \
                                                                              \
    case 0x5:                                                                 \
      /* hflip, low priority, wide                                          */\
      render_spr_row_wide(low, flip, collision_check);                        \
      break;                                                                  \
                                                                              \
    case 0x6:                                                                 \
      /* No hflip, high priority, wide                                      */\
      render_spr_row_wide(high, no_flip, collision_check);                    \
      break;                                                                  \
                                                                              \
    case 0x7:                                                                 \
      /* hflip, high priority, wide                                         */\
      render_spr_row_wide(high, flip, collision_check);                       \
      break;                                                                  \
  }                                                                           \

#define render_spr_line_sprite_loop(end)                                      \
  sat_cache_line_struct *sat_cache_line = &(vdc->sat_cache.lines[line]);      \
  sat_cache_line_entry_struct *sat_cache_entry;                               \
  u32 pattern_line_offset;                                                    \
  u32 attributes;                                                             \
  u32 current_palette;                                                        \
  u32 current_pixel;                                                          \
  u32 transparency_mask;                                                      \
  s32 block_offset;                                                           \
  u32 block_pixel_offset;                                                     \
  s32 scanline_offset = vdc->scanline_offset;                                 \
  s32 x;                                                                      \
  s32 i, i2;                                                                  \
                                                                              \
  u8 *current_row_ptr;                                                        \
  u8 *dest;                                                                   \
                                                                              \
  for(i = sat_cache_line->num_active - 1; i >= end; i--)                      \
  {                                                                           \
    render_spr_line_entry(i, no);                                             \
  }                                                                           \


#define render_spr_line_clamp_edges()                                         \
  if(vdc->scanline_offset > 0)                                                \
  {                                                                           \
    u32 left_offset_block = vdc->scanline_offset / 32;                        \
    u32 left_offset_pixels = vdc->scanline_offset % 32;                       \
                                                                              \
    obj_high_transparency_mask_dest[left_offset_block - 1] = 0;               \
    obj_low_transparency_mask_dest[left_offset_block - 1] = 0;                \
    if(left_offset_pixels)                                                    \
    {                                                                         \
      u32 left_offset_mask = 0xFFFFFFFF << left_offset_pixels;                \
                                                                              \
      obj_high_transparency_mask_dest[left_offset_block] &= left_offset_mask; \
      obj_low_transparency_mask_dest[left_offset_block] &= left_offset_mask;  \
    }                                                                         \
  }                                                                           \
                                                                              \
  if(vdc->screen_end < vce.screen_width)                                      \
  {                                                                           \
    u32 right_offset_block = vdc->screen_end / 32;                            \
    u32 right_offset_pixels = vdc->screen_end % 32;                           \
                                                                              \
    if(right_offset_pixels)                                                   \
    {                                                                         \
      u32 right_offset_mask = 0xFFFFFFFF >> (32 - right_offset_pixels);       \
                                                                              \
      obj_high_transparency_mask_dest[right_offset_block] &=                  \
       right_offset_mask;                                                     \
      obj_low_transparency_mask_dest[right_offset_block] &=                   \
       right_offset_mask;                                                     \
    }                                                                         \
    obj_high_transparency_mask_dest[right_offset_block + 1] = 0;              \
    obj_low_transparency_mask_dest[right_offset_block + 1] = 0;               \
  }                                                                           \


void render_spr_line(vdc_struct *vdc, u8 *obj_dest,
 u32 *obj_low_transparency_mask_dest, u32 *obj_high_transparency_mask_dest,
 u32 line)
{
  render_spr_line_sprite_loop(0);
  render_spr_line_clamp_edges();
}


u32 render_spr_line_check_collision(vdc_struct *vdc,
 u8 *obj_dest, u32 *obj_low_transparency_mask_dest,
 u32 *obj_high_transparency_mask_dest, u32 line)
{
  u32 spr0_collision = 0;

  render_spr_line_sprite_loop(1);
  render_spr_line_entry(0, yes);
  render_spr_line_clamp_edges();

  return spr0_collision;
}


void render_line_fill(u32 *dest, u32 color)
{
  u32 i;

  for(i = 0; i < vce.screen_width; i++)
  {
    *dest = color;
    dest++;
  }
}

#define render_line_expand_pixel_bg(_dest, offset)                           \
  _dest = vce.palette_cache[bg_buffer[pixel_offset + offset]]                \

#define render_line_expand_pixel_obj(_dest, offset)                          \
  _dest = vce.palette_cache[obj_buffer[pixel_offset + offset] + 256]         \

#define render_line_expand_pixel_case_select_four(pre, index, type_a,        \
 type_b, type_c, type_d)                                                     \
  case index:                                                                \
    render_line_expand_pixel_##type_a(pre##00, 0);                           \
    render_line_expand_pixel_##type_b(pre##01, 1);                           \
    render_line_expand_pixel_##type_c(pre##10, 2);                           \
    render_line_expand_pixel_##type_d(pre##11, 3);                           \
    break                                                                    \


#define render_line_expand_pixel(pixel)                                      \
  u32 pixel##_expand = (pixel | (pixel << 16)) & g_rb_mask                   \


#define render_line_collapse_pixel(_dest)                                    \
  current_color &= g_rb_mask;                                                \
  _dest = current_color | (current_color >> 16)                              \

#define render_line_expand_blend_25_75(_dest, _a, _b)                        \
  current_color = (_a + (_b * 3)) >> 2;                                      \
  render_line_collapse_pixel(_dest)                                          \

#define render_line_expand_blend_50_50(_dest, _a, _b)                        \
  current_color = (_a + _b) >> 1;                                            \
  render_line_collapse_pixel(_dest)                                          \

#define render_line_expand_blend_125_125_375_375(_dest, _a, _b, _c, _d)      \
  current_color = (_a + _b + ((_c + _d) * 3)) >> 3;                          \
  render_line_collapse_pixel(_dest)                                          \

#define render_line_expand_blend_25_25_25_25(_dest, _a, _b, _c, _d)          \
  current_color = (_a + _b + _c + _d) >> 2;                                  \
  render_line_collapse_pixel(_dest)                                          \


#define render_line_expand_dest_scale_4_to_5()                               \
{                                                                            \
  u32 rb_mask = 0x1F | (0x1F << 11);                                         \
  u32 g_mask = 0x3F << 5;                                                    \
  u32 g_rb_mask = (g_mask << 16) | rb_mask;                                  \
  u32 current_color;                                                         \
                                                                             \
  render_line_expand_pixel(pixel00);                                         \
  render_line_expand_pixel(pixel01);                                         \
  render_line_expand_pixel(pixel10);                                         \
  render_line_expand_pixel(pixel11);                                         \
                                                                             \
  dest[0] = pixel00;                                                         \
  render_line_expand_blend_25_75(dest[1], pixel00_expand, pixel01_expand);   \
  render_line_expand_blend_50_50(dest[2], pixel01_expand, pixel10_expand);   \
  render_line_expand_blend_25_75(dest[3], pixel11_expand, pixel10_expand);   \
  dest[4] = pixel11;                                                         \
                                                                             \
  dest += 5;                                                                 \
}                                                                            \

#define render_line_expand_dest_scale_8_to_5()                               \
{                                                                            \
  u32 rb_mask = 0x1F | (0x1F << 11);                                         \
  u32 g_mask = 0x3F << 5;                                                    \
  u32 g_rb_mask = (g_mask << 16) | rb_mask;                                  \
  u32 current_color;                                                         \
                                                                             \
  render_line_expand_pixel(pixel000);                                        \
  render_line_expand_pixel(pixel001);                                        \
  render_line_expand_pixel(pixel010);                                        \
  render_line_expand_pixel(pixel011);                                        \
  render_line_expand_pixel(pixel100);                                        \
  render_line_expand_pixel(pixel101);                                        \
  render_line_expand_pixel(pixel110);                                        \
  render_line_expand_pixel(pixel111);                                        \
                                                                             \
  render_line_expand_blend_50_50(dest[0], pixel000_expand, pixel001_expand); \
  render_line_expand_blend_125_125_375_375(dest[1], pixel000_expand,         \
   pixel001_expand, pixel010_expand, pixel011_expand);                       \
  render_line_expand_blend_25_25_25_25(dest[2], pixel010_expand,             \
   pixel011_expand, pixel100_expand, pixel101_expand);                       \
  render_line_expand_blend_125_125_375_375(dest[3], pixel110_expand,         \
   pixel111_expand, pixel100_expand, pixel101_expand);                       \
  render_line_expand_blend_50_50(dest[4], pixel110_expand, pixel111_expand); \
                                                                             \
  dest += 5;                                                                 \
}                                                                            \


#define render_line_expand_dest_no_scale()                                   \
  dest[0] = pixel00;                                                         \
  dest[1] = pixel01;                                                         \
  dest[2] = pixel10;                                                         \
  dest[3] = pixel11;                                                         \
                                                                             \
  dest += 4                                                                  \


#define render_line_expand_source_switch_four(pre)                           \
  switch(select_pixels & 0xF)                                                \
  {                                                                          \
    render_line_expand_pixel_case_select_four(pre, 0x0, bg,  bg,  bg,  bg);  \
    render_line_expand_pixel_case_select_four(pre, 0x1, obj, bg,  bg,  bg);  \
    render_line_expand_pixel_case_select_four(pre, 0x2, bg,  obj, bg,  bg);  \
    render_line_expand_pixel_case_select_four(pre, 0x3, obj, obj, bg,  bg);  \
    render_line_expand_pixel_case_select_four(pre, 0x4, bg,  bg,  obj, bg);  \
    render_line_expand_pixel_case_select_four(pre, 0x5, obj, bg,  obj, bg);  \
    render_line_expand_pixel_case_select_four(pre, 0x6, bg,  obj, obj, bg);  \
    render_line_expand_pixel_case_select_four(pre, 0x7, obj, obj, obj, bg);  \
    render_line_expand_pixel_case_select_four(pre, 0x8, bg,  bg,  bg,  obj); \
    render_line_expand_pixel_case_select_four(pre, 0x9, obj, bg,  bg,  obj); \
    render_line_expand_pixel_case_select_four(pre, 0xA, bg,  obj, bg,  obj); \
    render_line_expand_pixel_case_select_four(pre, 0xB, obj, obj, bg,  obj); \
    render_line_expand_pixel_case_select_four(pre, 0xC, bg,  bg,  obj, obj); \
    render_line_expand_pixel_case_select_four(pre, 0xD, obj, bg,  obj, obj); \
    render_line_expand_pixel_case_select_four(pre, 0xE, bg,  obj, obj, obj); \
    render_line_expand_pixel_case_select_four(pre, 0xF, obj, obj, obj, obj); \
  }                                                                          \
  pixel_offset += 4;                                                         \
  select_pixels >>= 4                                                        \


#define render_line_expand_source_four()                                     \
  /* Select four pixels at once.                                           */\
  u32 pixel00, pixel01, pixel10, pixel11;                                    \
  render_line_expand_source_switch_four(pixel)                               \


#define render_line_expand_source_eight()                                    \
  /* Select eight pixels at once.                                          */\
  u32 pixel000, pixel001, pixel010, pixel011;                                \
  u32 pixel100, pixel101, pixel110, pixel111;                                \
  render_line_expand_source_switch_four(pixel0);                             \
  render_line_expand_source_switch_four(pixel1)                              \


#define render_line_expand_builder(type, source_type, source_increment)      \
void render_line_expand_##type(u32 *dest, u8 *bg_buffer, u8 *obj_buffer,     \
 u32 *bg_mask_buffer, u32 *obj_low_mask_buffer, u32 *obj_high_mask_buffer)   \
{                                                                            \
  /* Number of 32 pixel blocks                                             */\
  u32 screen_width_blocks = vce.screen_width / 32;                           \
  u32 pixel_offset = 0;                                                      \
  u32 i, i2;                                                                 \
  u32 select_pixels;                                                         \
  for(i = 0; i < screen_width_blocks; i++)                                   \
  {                                                                          \
    /* 0 selects BG, 1 selects OBJ                                         */\
    /* BG OBJ-H OBJ-L Select                                               */\
    /*  0  0     0     0                                                   */\
    /*  0  0     1     1                                                   */\
    /*  0  1     0     1                                                   */\
    /*  0  1     1     1                                                   */\
    /*  1  0     0     0                                                   */\
    /*  1  0     1     0                                                   */\
    /*  1  1     0     1                                                   */\
    /*  1  1     1     1                                                   */\
    /* Select = (OBJ-L & ~BG) | OBJ-H                                      */\
    select_pixels = (obj_low_mask_buffer[i] & ~(bg_mask_buffer[i])) |        \
     obj_high_mask_buffer[i];                                                \
                                                                             \
    for(i2 = 0; i2 < 32; i2 += source_increment)                             \
    {                                                                        \
      render_line_expand_source_##source_type();                             \
      render_line_expand_dest_##type();                                      \
    }                                                                        \
  }                                                                          \
}                                                                            \

render_line_expand_builder(scale_4_to_5, four, 4);
render_line_expand_builder(scale_8_to_5, eight, 8);
render_line_expand_builder(no_scale, four, 4);


#define render_line_expand_sgx_select_priority_a()                           \

// VDC2 spr chooses VDC2 when VDC1 is chosen by sprite
#define render_line_expand_sgx_select_priority_b()                           \
  select_vdc |= select_obj_vdc_b & ~select_obj_vdc_a                         \

#define render_line_expand_sgx_select_priority_c()                           \
  select_vdc |= select_bg_vdc_b & ~select_bg_vdc_a                           \


// This is done in order to reduce the amount of variables floating around.
// It turns the number of select bitmaps from 6 to 1.

#define render_line_expand_sgx_select_pixels(priority_type)                  \
  for(i = 0; i < screen_width_blocks; i++)                                   \
  {                                                                          \
    /* 0 selects BG, 1 selects OBJ                                         */\
    /* BG OBJ-H OBJ-L Select                                               */\
    /*  0  0     0     0                                                   */\
    /*  0  0     1     1                                                   */\
    /*  0  1     0     1                                                   */\
    /*  0  1     1     1                                                   */\
    /*  1  0     0     0                                                   */\
    /*  1  0     1     0                                                   */\
    /*  1  1     0     1                                                   */\
    /*  1  1     1     1                                                   */\
    /* Select = (OBJ-L & ~BG) | OBJ-H                                      */\
    u32 select_bg_vdc_a = bg_mask_buffer_a[i];                               \
    u32 select_obj_vdc_a = (obj_low_mask_buffer_a[i] & ~select_bg_vdc_a) |   \
     obj_high_mask_buffer_a[i];                                              \
    u32 select_opaque_vdc_a = select_bg_vdc_a | select_obj_vdc_a;            \
    u32 select_bg_vdc_b = bg_mask_buffer_b[i];                               \
    u32 select_obj_vdc_b = (obj_low_mask_buffer_b[i] & ~select_bg_vdc_b) |   \
     obj_high_mask_buffer_b[i];                                              \
    u32 select_opaque_vdc_b = select_bg_vdc_b | select_obj_vdc_b;            \
    u32 select_vdc = ~select_opaque_vdc_a;                                   \
    u32 select_obj;                                                          \
                                                                             \
    render_line_expand_sgx_select_##priority_type();                         \
    select_obj = (select_vdc & select_obj_vdc_b) |                           \
     (~select_vdc & select_obj_vdc_a);                                       \
                                                                             \
    select_pixels[i * 2] =                                                   \
     (select_obj & 0x55555555) | ((select_vdc & 0x55555555) << 1);           \
    select_pixels[(i * 2) + 1] =                                             \
     ((select_obj >> 1) & 0x55555555) | (select_vdc & 0xAAAAAAAA);           \
  }                                                                          \


#define render_line_expand_pixel_sgx_bg_a(_dest, offset)                     \
  _dest = vce.palette_cache[bg_buffer_a[pixel_offset + offset]]              \

#define render_line_expand_pixel_sgx_obj_a(_dest, offset)                    \
  _dest = vce.palette_cache[obj_buffer_a[pixel_offset + offset] + 256]       \

#define render_line_expand_pixel_sgx_bg_b(_dest, offset)                     \
  _dest = vce.palette_cache[bg_buffer_b[pixel_offset + offset]]              \

#define render_line_expand_pixel_sgx_obj_b(_dest, offset)                    \
  _dest = vce.palette_cache[obj_buffer_b[pixel_offset + offset] + 256]       \



#define render_line_expand_sgx_case_select_two(off, _a, _b, index, type_a,   \
 type_b)                                                                     \
  case index:                                                                \
    render_line_expand_pixel_sgx_##type_a(_a, off + 0);                      \
    render_line_expand_pixel_sgx_##type_b(_b, off + 2);                      \
    break                                                                    \


#define render_line_expand_source_sgx_two(off, _select_pixels, _a, _b)       \
  switch(_select_pixels & 0xF)                                               \
  {                                                                          \
    render_line_expand_sgx_case_select_two(off, _a, _b, 0x0, bg_a,  bg_a);   \
    render_line_expand_sgx_case_select_two(off, _a, _b, 0x1, obj_a, bg_a);   \
    render_line_expand_sgx_case_select_two(off, _a, _b, 0x2, bg_b,  bg_a);   \
    render_line_expand_sgx_case_select_two(off, _a, _b, 0x3, obj_b, bg_a);   \
    render_line_expand_sgx_case_select_two(off, _a, _b, 0x4, bg_a,  obj_a);  \
    render_line_expand_sgx_case_select_two(off, _a, _b, 0x5, obj_a, obj_a);  \
    render_line_expand_sgx_case_select_two(off, _a, _b, 0x6, bg_b,  obj_a);  \
    render_line_expand_sgx_case_select_two(off, _a, _b, 0x7, obj_b, obj_a);  \
    render_line_expand_sgx_case_select_two(off, _a, _b, 0x8, bg_a,  bg_b);   \
    render_line_expand_sgx_case_select_two(off, _a, _b, 0x9, obj_a, bg_b);   \
    render_line_expand_sgx_case_select_two(off, _a, _b, 0xA, bg_b,  bg_b);   \
    render_line_expand_sgx_case_select_two(off, _a, _b, 0xB, obj_b, bg_b);   \
    render_line_expand_sgx_case_select_two(off, _a, _b, 0xC, bg_a,  obj_b);  \
    render_line_expand_sgx_case_select_two(off, _a, _b, 0xD, obj_a, obj_b);  \
    render_line_expand_sgx_case_select_two(off, _a, _b, 0xE, bg_b,  obj_b);  \
    render_line_expand_sgx_case_select_two(off, _a, _b, 0xF, obj_b, obj_b);  \
  }                                                                          \
  _select_pixels >>= 4                                                       \


#define render_line_expand_source_sgx_four()                                 \
  /* Select four pixels at once.                                           */\
  u32 pixel00, pixel01, pixel10, pixel11;                                    \
  render_line_expand_source_sgx_two(0, select_even, pixel00, pixel10);       \
  render_line_expand_source_sgx_two(1, select_odd, pixel01, pixel11);        \
  pixel_offset += 4                                                          \


#define render_line_expand_source_sgx_eight()                                \
  /* Select eight pixels at once.                                          */\
  u32 pixel000, pixel001, pixel010, pixel011;                                \
  u32 pixel100, pixel101, pixel110, pixel111;                                \
  render_line_expand_source_sgx_two(0, select_even, pixel000, pixel010);     \
  render_line_expand_source_sgx_two(1, select_odd, pixel001, pixel011);      \
  render_line_expand_source_sgx_two(4, select_even, pixel100, pixel110);     \
  render_line_expand_source_sgx_two(5, select_odd, pixel101, pixel111);      \
  pixel_offset += 8                                                          \


#define render_line_expand_sgx_combine(type, source_type, source_increment,  \
 priority_type)                                                              \
  render_line_expand_sgx_select_pixels(priority_type);                       \
  for(i = 0; i < screen_width_blocks * 2; i += 2)                            \
  {                                                                          \
    select_even = select_pixels[i];                                          \
    select_odd = select_pixels[i + 1];                                       \
    for(i2 = 0; i2 < 32; i2 += source_increment)                             \
    {                                                                        \
      render_line_expand_source_sgx_##source_type();                         \
      render_line_expand_dest_##type();                                      \
    }                                                                        \
  }                                                                          \


#define render_line_expand_blank_no_scale()                                  \
  for(i = 0; i < vce.screen_width; i++)                                      \
  {                                                                          \
    dest[i] = vce.palette_cache[0];                                          \
  }                                                                          \

#define render_line_expand_blank_scale_4_to_5()                              \
  for(i = 0; i < 320; i++)                                                   \
  {                                                                          \
    dest[i] = vce.palette_cache[0];                                          \
  }                                                                          \

#define render_line_expand_blank_scale_8_to_5()                              \
  for(i = 0; i < 320; i++)                                                   \
  {                                                                          \
    dest[i] = vce.palette_cache[0];                                          \
  }                                                                          \


#define render_line_expand_sgx_builder(type, source_type, source_increment)  \
void render_line_expand_sgx_##type(u32 *dest, u8 *bg_buffer_a,               \
 u8 *obj_buffer_a, u32 *bg_mask_buffer_a, u32 *obj_low_mask_buffer_a,        \
 u32 *obj_high_mask_buffer_a, u8 *bg_buffer_b, u8 *obj_buffer_b,             \
 u32 *bg_mask_buffer_b, u32 *obj_low_mask_buffer_b,                          \
 u32 *obj_high_mask_buffer_b)                                                \
{                                                                            \
  /* Number of 32 pixel blocks                                             */\
  u32 screen_width_blocks = vce.screen_width / 32;                           \
  u32 pixel_offset = 0;                                                      \
  u32 span, i, i2;                                                           \
  u32 select_pixels[512 * 2 / 32];                                           \
  u32 select_even, select_odd;                                               \
                                                                             \
  /* Almost no emulation of windows is being done here; the first span is  */\
  /* being used for the whole thing. Right now it appears that this is     */\
  /* sufficient for the 7 SGX titles, but this will possibly have to be    */\
  /* revisited.                                                            */\
  switch(vpc.window_span_status[0])                                          \
  {                                                                          \
    case 0x0:                                                                \
    case 0x4:                                                                \
    case 0x8:                                                                \
    case 0xC:                                                                \
      /* All disabled - show background.                                   */\
      render_line_expand_blank_##type();                                     \
      break;                                                                 \
                                                                             \
    case 0x1:                                                                \
    case 0x5:                                                                \
    case 0x9:                                                                \
    case 0xD:                                                                \
      /* VDC1 enabled only                                                 */\
      render_line_expand_##type(dest, bg_buffer_a, obj_buffer_a,             \
       bg_mask_buffer_a, obj_low_mask_buffer_a, obj_high_mask_buffer_a);     \
      break;                                                                 \
                                                                             \
    case 0x2:                                                                \
    case 0x6:                                                                \
    case 0xA:                                                                \
    case 0xE:                                                                \
      /* VDC2 enabled only                                                 */\
      render_line_expand_##type(dest, bg_buffer_b, obj_buffer_b,             \
       bg_mask_buffer_b, obj_low_mask_buffer_b, obj_high_mask_buffer_b);     \
      break;                                                                 \
                                                                             \
    case 0x3:                                                                \
    case 0xF:                                                                \
      /* Priority type a                                                   */\
      render_line_expand_sgx_combine(type, source_type, source_increment,    \
       priority_a);                                                          \
      break;                                                                 \
                                                                             \
    case 0x7:                                                                \
      /* Priority type b                                                   */\
      render_line_expand_sgx_combine(type, source_type, source_increment,    \
       priority_b);                                                          \
      break;                                                                 \
                                                                             \
    case 0xB:                                                                \
      /* Priority type c                                                   */\
      render_line_expand_sgx_combine(type, source_type, source_increment,    \
       priority_c);                                                          \
      break;                                                                 \
  }                                                                          \
}                                                                            \


render_line_expand_sgx_builder(scale_4_to_5, four, 4);
render_line_expand_sgx_builder(scale_8_to_5, eight, 8);
render_line_expand_sgx_builder(no_scale, four, 4);

#endif
