.extern cache_tile


#define VDC_BG_BUFFER_OFFSET
#define VDC_MWR_OFFSET
#define VDC_SCREEN_WIDTH_OFFSET
#define VDC_EFFECTIVE_BYR_OFFSET
#define VDC_BG_MASK_BUFFER_OFFSET
#define VDC_TILE_CACHE_OFFSET
#define VDC_BXR_OFFSET
#define VDC_SCROLL_SHIFT_OFFSET
#define VDC_SCANLINE_OFFSET_OFFSET
#define VDC_VRAM_OFFSET
#define VDC_DIRTY_TILES_OFFSET

@ render_bg_line
@  r0: vdc_struct *vdc
@  r1: u8 **bg_buffer_result
@ returns u32 * pointer to bg transparency mask

.global ext_symbol(render_bg_line)

.align 8

render_bg_line:
  @ Temporaries: r2, r3, r14

  stmdb sp!, { r4 - r11, r14 }         @ save registers
  @ r8 = bg_buffer
  add r8, r0, #(VDC_BG_BUFFER_OFFSET + 512 + 512)

  sub r13, r13, #(4 * 6)               @ make room to save 6 registers
  add r2, r0, #VDC_MWR_OFFSET          @ r2 = vdc->mwr

  @ r10 = vdc->screen_width
  ldr r11, [r0, #VDC_SCREEN_WIDTH_OFFSET]  
  mov r12, r2, lsr #4                  @ r12 = vdc->mwr >> 4

  mov r2, r2, lsr #6                   @ r2 = vdc->mwr >> 6
  @ r12 = bg_width
  and r12, r12, #0x3                   @ r12 = (vdc->mwr >> 4) & 0x3

  @ r3 = vdc->effective_byr
  ldr r3, [r0, #VDC_EFFECTIVE_BYR_OFFSET]
  @ r11 = screen_width_tiles
  mov r11, r11, lsr #3                 @ r11 = (vdc->screen_width) / 8

  @ r2 = bg_height
  and r2, r2, #0x1                     @ r2 = (vdc->mwr >> 6) & 0x1
  @ r9 = (vdc->bg_mask_buffer + ((512 + 512) / 8)) = transparency_mask_dest
  add r9, r0, #(VDC_BG_MASK_BUFFER_OFFSET + ((512 + 512) / 8))

  @ r14 = y_tile_offset
  mov r14, r3, lsr #3                  @ r14 = vdc->effective_byr >> 3
  add r7, r0, #VDC_TILE_CACHE_OFFSET   @ r7 = vdc->tile_cache

  ldr r6, [r0, #VDC_BXR_OFFSET]        @ r6 = vdc->bxr
  @ r3 = y_pixel_offset
  and r3, r3, #0x7                     @ r3 = vdc->effective_byr & 0x7

  @ r5 = vdc->scroll_shift
  ldr r5, [r0, #VDC_SCROLL_SHIFT_OFFSET]
  @ r9 = vdc->tile_cache_transparency_mask
  add r9, r0, #VDC_TILE_CACHE_TRANSPARENCY_MASK

  @ r7 = tile_base
  add r7, r7, r3, lsl #3               @ r7 = tile_cache + (y_pixel_offset << 3)
  @ r9 = tile_cache_transparency_mask
  add r9, r9, r3                       @ r9 = tile_cache_tm + y_pixel_offset

  @ r3 = vdc->scanline_offset
  ldr r3, [r0, VDC_SCANLINE_OFFSET_OFFSET]
  @ r6 = x_scroll
  add r6, r6, r5                       @ r6 = vdc->bxr + vdc->scroll_shift

  @ r5 = x_tile_offset
  mov r5, r6, lsr #3                   @ r5 = x_scroll >> 3
  @ r6 = x_pixel_offset (set flags)
  ands r6, r6, #0x7                    @ r6 = x_scroll & 0x7

  add r2, r2, #1                       @ bg_height++
  sub r3, r3, r6                       @ scanline_offset -= x_pixel_offset
 
  addne r11, r11, #1                   @ if(x_pixel_offset) scr_wdth_tiles++
  @ save scanline_offset, x_pixel_offset, bg_buffer, transparency_mask_blocks
  stmda r13, { r0, r3, r6, r8, r10 }   @ save vdc, scanline_offset, x_pixel_offset

  sub r6, r8, r3                       @ r6 = bg_buffer - scanline_offset
  cmp r12, #2                          @ check if bg_width is 2

  str r6, [r1]                         @ write out bg_buffer_result
  addeq r12, r12, #1                   @ if(bg_width == 2) bg_width = 3

  @ r6 = screen_width_mask_blocks
  add r6, r11, #3                      @ r6 = screen_width_tiles + 3
  add r12, r12, #1                     @ bg_width++

  mov r6, r6, lsr #2                   @ r6 = scr_wdth_msk_blks >> 2
  mov r2, r2, lsl #5                   @ r2 = bg_height << 5

  str r6, [r13, #20]                   @ save screen_width_mask_blocks
  mov r12, r12, lsl #5                 @ bg_width <<= 5

  sub r2, r2, #1                       @ r2 = (bg_height << 5) - 1
  @ r4 = bat_offset
  add r4, r0, VDC_VRAM_OFFSET          @ r4 = vdc->vram 

  and r14, r14, r2                     @ r14 = y_tile_offset & bg_height
  add r3, r12, r12                     @ r3 = bg_width * 2
                                 
  mla r4, r14, r3, r4                  @ r4 = bat_off + (y_tile_off * bg_width)
  sub r12, r12, #1                     @ r0 = bg_width - 1

  add r6, r0, VDC_DIRTY_TILES_OFFSET   @ r6 = vdc->dirty_tiles
  add r12, r12, r12                    @ r12 = (bg_width - 1) * 2

  @ Live variables:
  @  r0: vdc
  @  r4: bat_offset
  @  r5: x_tile_offset
  @  r6: dirty_tiles
  @  r7: tile_base
  @  r8: dest
  @  r9: transparency_mask_base
  @  r10: transparency_mask_dest
  @  r11: screen_width_tiles
  @  r12: bg_width

  @ save + restore these for later half
  @  r1: scanline_offset
  @  r2: bg_buffer
  @  r3: transparency_mask_blocks
  @  r4: x_pixel_offset
  @  r5: screen_width_mask_blocks

 0:
  @ Temporaries: r0, r1, r2, r3, r14
  ldrh r0, [r4, r5]                    @ r0 = *(u16 *)(bat_off + x_tile_off)
  add r5, r5, #2                       @ x_tile_offset += 2
  and r5, r5, r12                      @ x_tile_offset &= bg_width
  mov r2, r0, lsr #12                  @ r2 = bat_entry >> 12 (current_palette)
  bic r1, r0, #0xF000                  @ r1 = bat_entry & 0xFFF (tile_number)
  ldrb r0, [r6, r1]                    @ r0 = dirty_tiles[tile_number]
  orr r2, r2, lsl #8                   @ duplicate current_palette 8b to 16b
  orr r2, r2, lsl #16                  @ duplicate current_palette 16b to 32b
  cmp r0, #0                           @ check if dirty value is set
  bne render_bg_line_cache_tile 

 render_bg_line_tile_cached:
  subs r11, r11, #1                    @ screen_width_tiles--
  add r2, r7, r1, lsl #6               @ r2 = tile_base + (tile_number << 6)
  ldrb r3, [r9, r1, lsl #3]            @ r3 = transparency_mask_value
  ldr r0, [r2]                         @ r0 = left tile half
  ldr r1, [r2, #4]                     @ r1 = right tile half
  orr r0, r0, r2                       @ set palette for left half
  orr r1, r0, r2                       @ set palette for right half
  str r0, [r8], #4                     @ write out left half
  str r1, [r8], #4                     @ write out right half
  strb r3, [r10], #1                   @ write out transparency mask

  bne 0b                               @ repeat loop if tiles remain

  // Clamp off edges if necessary, by clearing at least 8 pixels around them.
  if(scanline_offset <= 0)
    goto _0b;

  {
    u32 left_pixel_offset = x_pixel_offset;
    u8 *bg_buffer_clear = bg_buffer;

    if(left_pixel_offset > 3)
    {
      u32 zero = 0;
      *((u32 *)bg_buffer_clear) = zero;
      bg_buffer_clear += 4;
      left_pixel_offset -= 4;
    }

    u32 mask = 0xFFFFFFFF;
    u32 shift = left_pixel_offset << 3;
    mask = mask << shift;

    u32 bg_buffer_clear_value = *((u32 *)bg_buffer_clear);
    bg_buffer_clear_value &= mask;
    *((u32 *)bg_buffer_clear) = bg_buffer_clear_value;
  }

 _0b:;

  u32 screen_end = vdc->screen_end;
  u32 vce_screen_width = vce.screen_width;

  if(screen_end >= vce_screen_width)
    goto _0c;

  {
    u32 screen_width_shift = screen_width >> 2;

    u8 *bg_buffer_clear = bg_buffer + (screen_width_shift << 2);

    if(x_pixel_offset >= 4)
    {
      bg_buffer_clear += 4;
      x_pixel_offset -= 4;
    }

    u32 zero = 0;
    *((u32 *)(bg_buffer_clear + 4)) = zero;

    u32 mask = 0xFFFFFFFF;
    u32 shift = x_pixel_offset << 3;
    mask = mask << shift;

    u32 bg_buffer_clear_value = *((u32 *)bg_buffer_clear);
    bg_buffer_clear_value = bg_buffer_clear_value & ~mask;
    *((u32 *)bg_buffer_clear) = bg_buffer_clear_value;
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
      
      *((u32 *)transparency_mask_blocks) = transparency_mask;
  
      transparency_mask_blocks += 4;
      screen_width_mask_blocks--;

      if(screen_width_mask_blocks > 0)
        goto _0d;
    }
  }
  goto _4a;

 _3a:
  {
    u32 tile_mask_shift = scanline_offset & 0x1F;
    u32 tile_mask_shift_inverse = 32 - tile_mask_shift;

    transparency_mask_buffer_result = transparency_mask_blocks;
    u32 scanline_offset_div32 = (u32)scanline_offset >> 5;
    transparency_mask_buffer_result =
     transparency_mask_buffer_result - (scanline_offset_div32 << 2);

    if(tile_mask_shift == 0)
      goto _4a;

    {
      transparency_mask_blocks =
       transparency_mask_blocks + (screen_width_mask_blocks << 2);

      screen_width_mask_blocks = screen_width_mask_blocks + 1;

      _0e:
      {
        u32 transparency_mask = *((u32 *)transparency_mask_blocks);
        transparency_mask = transparency_mask << tile_mask_shift;
        u32 transparency_mask_b = *((u32 *)(transparency_mask_blocks - 4));
        transparency_mask = 
         transparency_mask | (transparency_mask_b >> tile_mask_shift_inverse);
     
        *((u32 *)transparency_mask_blocks) = transparency_mask;
  
        transparency_mask_blocks -= 4;
        screen_width_mask_blocks--;

        if(screen_width_mask_blocks > 0)
          goto _0e;
      }
    }
  }

 _4a:
  return (u32 *)transparency_mask_buffer_result;
}

render_bg_line_cache_tile:
  stmdb sp!, { r0 - r2, r12, r14 }     @ save modifiable registers
  bl cache_tile                        @ r0 = vdc, r1 = tile_number

  ldmia sp!, { r0 - r2, r12, r14 }     @ restore modifiable registers
  mov r3, #0                           @ r3 = 0

  strb r3, [r6, r1]                    @ dirty_tiles[tile_number] = 0
  b render_bg_line_tile_cached         @ return to render loop

#endif
