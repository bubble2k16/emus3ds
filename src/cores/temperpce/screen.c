#include "common.h"
#include "font.h"

font_struct *current_font = &font_a;

#ifdef FRAMEBUFFER_PORTRAIT_ORIENTATION

#define calculate_screen_offset(_x, _y)                                        \
  (((_x) * pitch) + (_y))                                                      \

#define advance_offset_column()                                                \
  dest_offset += pitch                                                         \

#else

#define calculate_screen_offset(_x, _y)                                        \
  (((_y) * pitch) + _x)                                                        \

#define advance_offset_column()                                                \
  dest_offset++                                                                \

#endif

void print_string_ext(const char *str, u32 fg_color, u32 bg_color,
 u32 x, u32 y, u16 *_dest_ptr, u16 *_bg_ptr, u32 pitch, u32 pad,
 u32 screen_limit)
{
  u32 dest_offset;
  u8 current_char = str[0];
  u32 current_row;
  u32 glyph_offset;
  u32 i = 0, i2, i3;
  u32 str_index = 1;
  u32 current_x = x;

  while(current_char)
  {
    if(current_char == '\n')
    {
      y += current_font->height;
      current_x = x;
    }
    else
    {
      glyph_offset = current_font->_font_offset[current_char];

      if((current_x + current_font->width) > screen_limit)
        return;

      for(i2 = 0; i2 < current_font->height; i2++, glyph_offset++)
      {
        dest_offset = calculate_screen_offset(current_x, y + i2);
        current_row = current_font->_font_bits[glyph_offset];
        for(i3 = 0; i3 < current_font->width; i3++)
        {
          if((current_row >> (15 - i3)) & 0x01)
          {
            _dest_ptr[dest_offset] = fg_color;
          }
          else
          {
            if(bg_color & 0x10000)
            {
              if(_bg_ptr)
                _dest_ptr[dest_offset] = _bg_ptr[dest_offset];
            }
            else
            {
              _dest_ptr[dest_offset] = bg_color;
            }
          }
          advance_offset_column();
        }
      }
      current_x += current_font->width;
    }

    i++;

    current_char = str[str_index];

    if((i < pad) && (current_char == 0))
      current_char = ' ';
    else
      str_index++;
  }
}

void print_string(const char *str, u32 fg_color, u32 bg_color,
 u32 x, u32 y, u32 screen_limit)
{
  print_string_ext(str, fg_color, bg_color, x, y, get_screen_ptr(),
   NULL, get_screen_pitch(), 0, screen_limit);
}

void print_string_bg(const char *str, u32 fg_color, u16 *bg_ptr,
 u32 x, u32 y, u32 screen_limit)
{
  print_string_ext(str, fg_color, 0x10000, x, y, get_screen_ptr(),
   bg_ptr, get_screen_pitch(), 0, screen_limit);
}

void set_font_wide()
{
  current_font = &font_b;
}

void set_font_narrow()
{
  current_font = &font_a;
}

void copy_screen(u32 *dest)
{
  u32 i;
  u32 pitch = get_screen_pitch();
  u16 *pixels = get_screen_ptr();

  for(i = 0; i < 240; i++)
  {
    memcpy(dest, pixels, 320 * 2);
    pixels += pitch;
    dest += 320;
  }
}

void blit_screen(u32 *src)
{
  u32 i;
  u32 pitch = get_screen_pitch();
  u16 *pixels = get_screen_ptr();

  for(i = 0; i < 240; i++)
  {
    memcpy(pixels, src, 320 * 2);
    pixels += pitch;
    src += 320;
  }
}

// Pass it get_screen_ptr() to halve intensity on itself; should
// work okay.

void buffer_quarter_intensity(u16 *_dest, u16 *_pixels, u32 pitch)
{
  s32 i, i2;
  u32 current_pixel_pair;
  u32 intensity_mask = ((0x3 << 11) | (0x3 << 5) | 0x3);
  u32 *pixels = (u32 *)_pixels;
  u32 *dest = (u32 *)_dest;

  pitch /= 2;

  intensity_mask = ~(intensity_mask | (intensity_mask << 16));

  for(i = 0; i < 240; i++)
  {
    i2 = 159;
    do
    {
      current_pixel_pair = pixels[i2] & intensity_mask;
      dest[i2] = current_pixel_pair >> 2;

      i2--;
    } while(i2 >= 0);

    pixels += pitch;
    dest += 160;
  }
}

void buffer_half_intensity(u16 *_dest, u16 *_pixels, u32 pitch)
{
  s32 i, i2;
  u32 current_pixel_pair;
  u32 intensity_mask = ((0x1 << 11) | (0x1 << 5) | 0x1);
  u32 *pixels = (u32 *)_pixels;
  u32 *dest = (u32 *)_dest;

  pitch /= 2;

  intensity_mask = ~(intensity_mask | (intensity_mask << 16));

  for(i = 0; i < 240; i++)
  {
    i2 = 159;
    do
    {
      current_pixel_pair = pixels[i2] & intensity_mask;
      dest[i2] = current_pixel_pair >> 1;

      i2--;
    } while(i2 >= 0);

    pixels += pitch;
    dest += 160;
  }
}

void copy_screen_quarter_intensity(u16 *_dest)
{
  buffer_quarter_intensity(_dest, get_screen_ptr(), get_screen_pitch());
}

void copy_screen_half_intensity(u16 *_dest)
{
  buffer_half_intensity(_dest, get_screen_ptr(), get_screen_pitch());
}

