// Make sure these values are correct. There's an area in main.c that will
// print them out which is normally commented out - use that to check. Be
// careful if things are changed...


#ifdef IPHONE_BUILD

#define ext_symbol(symbol) _##symbol

#define call_c(function, offset)                                              \
  ldr r9, [sp, #((offset) * 4)];                                              \
  bl _##function                                                              \

#else

#define ext_symbol(symbol) symbol

#define call_c(function, offset)                                              \
  bl function                                                                 \

#endif


// Any increment or offset to the framebuffer is abstracted so that either
// rotated portrait or normal landscape style framebuffers may be supported.
// Value should be given in bytes.

#ifdef FRAMEBUFFER_PORTRAIT_ORIENTATION

#define pixel_pitch(value)                                                    \
  #(value * 240)                                                              \

#define add_pixel_pitch_variable_direct(reg_dest, reg_base, reg_value)        \
  mov reg_dest, #480;                                                         \
  mul reg_dest, reg_value, reg_dest;                                          \
  add reg_dest, reg_base, reg_dest                                            \

#else

#define pixel_pitch(value)                                                    \
  #(value)                                                                    \

#define add_pixel_pitch_variable_direct(reg_dest, reg_base, reg_value)        \
  add reg_dest, reg_base, reg_value, lsl #1                                   \

#endif


#define add_pixel_pitch_variable_indirect(reg_dest, reg_base, reg_value)      \
  add reg_dest, reg_base, reg_value, lsl #1                                   \


#define VCE_PALETTE_CACHE_OFFSET  (ext_symbol(vce) + 0x00400)
#define VCE_SPALETTE_CACHE_OFFSET (ext_symbol(vce) + 0x00400 + 0x200)

#define VDC_VRAM_OFFSET           (ext_symbol(vdc) + 0x00000)
#define VDC_STATUS_OFFSET         (ext_symbol(vdc) + 0x10200)
#define VDC_CR_OFFSET             (ext_symbol(vdc) + 0x10210)
#define VDC_MWR_OFFSET            (ext_symbol(vdc) + 0x10218)
#define VDC_BXR_OFFSET            (ext_symbol(vdc) + 0x1021C)
#define VDC_EFFECTIVE_BYR_OFFSET  (ext_symbol(vdc) + 0x10264)
#define VDC_SCREEN_WIDTH_OFFSET   (ext_symbol(vdc) + 0x1026c)
#define VDC_OVERDRAW_WIDTH_OFFSET (ext_symbol(vdc) + 0x1027c)
#define VDC_TILE_CACHE_OFFSET     (ext_symbol(vdc) + 0x102a4)
#define VDC_PATTERN_CACHE_OFFSET  (ext_symbol(vdc) + 0x202a4)
#define VDC_DIRTY_TILES_OFFSET    (ext_symbol(vdc) + 0x302a4)
#define VDC_DIRTY_PATTERNS_OFFSET (ext_symbol(vdc) + 0x30aa4)

#define SAT_CACHE_OFFSET          (ext_symbol(sat_cache))

#define IRQ_VDC                   0x2
#define VDC_STATUS_OVERFLOW_IRQ   0x2

@ r0: u16 *dest
@ r1: u16 *src

0:
  .long VCE_PALETTE_CACHE_OFFSET  @ 0x0
  .long VDC_SCREEN_WIDTH_OFFSET   @ 0x4
  .long 0x0000FFFF                @ 0x8
  .long ext_symbol(sp_storage)    @ 0xC
  .long 0x07E0F81F                @ 0x10

.globl ext_symbol(render_line_expand)


#define expand_pair(src)                                                      \
  and r7, src, r14                ;/* r7 = bottom half                      */\
                                                                              \
  tst r7, #0xFF00                 ;/* if(r7 & 0xFF00)                       */\
  addne r7, r12, r7, lsr #8       ;/*   r7 = (0x100 + (r7 >> 8))            */\
  ldr r7, [r9, r7, lsl #1]        ;/* r7 = vce.palette_cache[r7]            */\
  mov src, src, lsr #16           ;/* src = top half of pixel pair          */\
  tst src, #0xFF00                ;/* if(src & 0xFF00)                      */\
  addne src, r12, src, lsr #8     ;/*   src = (0x100 + (src >> 8))          */\
  ldr src, [r9, src, lsl #1]      ;/* src = vce.palette_cache[src]          */\
  and r7, r7, r14                 ;/* isolate lower 16 bits of r7           */\
                                                                              \
  orr src, r7, src, lsl #16        /* combine new bits into src             */\


@ This is really reaching, but it fills the tiny potential load-use penalty.

#define expand_pair_sub(src)                                                  \
  and r7, src, r14                ;/* r7 = bottom half                      */\
                                                                              \
  tst r7, #0xFF00                 ;/* if(r7 & 0xFF00)                       */\
  addne r7, r12, r7, lsr #8       ;/*   r7 = (0x100 + (r7 >> 8))            */\
  ldr r7, [r9, r7, lsl #1]        ;/* r7 = vce.palette_cache[r7]            */\
  mov src, src, lsr #16           ;/* src = top half of pixel pair          */\
  tst src, #0xFF00                ;/* if(src & 0xFF00)                      */\
  addne src, r12, src, lsr #8     ;/*   src = (0x100 + (src >> 8))          */\
  ldr src, [r9, src, lsl #1]      ;/* src = vce.palette_cache[src]          */\
  and r7, r7, r14                 ;/* isolate lower 16 bits of r7           */\
  subs r2, r2, #1                 ;/* go to next 16 pixels                  */\
  orr src, r7, src, lsl #16        /* combine new bits into src             */\


ext_symbol(render_line_expand):
  stmdb sp!, { r4 - r11, r14 }

  ldr r9, 0b + 0xC                @ tuck away sp in sp_storage
  str sp, [r9]
  ldr r2, 0b + 0x4                @ r2 = &vdc.screen_width
  ldr r9, 0b                      @ r9 = vce.palette_cache
  ldr r2, [r2]                    @ r2 = vdc.screen_width
  mov r12, #0x100                 @ r12 = 0x100 (useful constant)
  ldr r14, 0b + 0x8               @ r14 = 0xFFFF, another useful constant

  @ screen.width should be a multiple of 8
  movs r2, r2, lsr #4
  bcc 1f

  @ load 8 pixels
  ldmia r1!, { r3 - r6 }

  expand_pair(r3)                 @ expand pixel pairs
  expand_pair(r4)
  expand_pair(r5)
  expand_pair(r6)

  @ store 8 pixels
#ifdef FRAMEBUFFER_PORTRAIT_ORIENTATION

#else
  stmia r0!, { r3 - r6 }
#endif

 1:
  @ load 16 pixels
  ldmia r1!, { r3 - r6, r8, r10 - r11, r13 }

  expand_pair(r3)                 @ expand pixel pairs
  expand_pair(r4)
  expand_pair(r5)
  expand_pair(r6)
  expand_pair(r8)
  expand_pair(r10)
  expand_pair(r11)
  expand_pair_sub(r13)

  @ store 16 pixels
  stmia r0!, { r3 - r6, r8, r10 - r11, r13 }

  bne 1b

  ldr r9, 0b + 0xC                @ bring back sp
  ldr sp, [r9]

  ldmia sp!, { r4 - r11, pc }

@ dest may be _b, but not _a

#define color_blend_25_75(dest, _a, _b)                                       \
  add dest, _b, _b, lsl #1;                                                   \
  add dest, _a, dest;                                                         \
  and dest, r12, dest, lsr #2;                                                \
  orr dest, dest, dest, lsr #16                                               \

@ dest may be _a or _b

#define color_blend_50_50(dest, _a, _b)                                       \
  add dest, _a, _b;                                                           \
  and dest, r12, dest, lsr #1;                                                \
  orr dest, dest, dest, lsr #16                                               \

#define pixel_extend(dest, src)                                               \
  orr dest, src, src, lsl #16;                                                \
  and dest, dest, r12                                                         \

@ dest may be _a, but not _b

#define pack_pixels(dest, _a, _b)                                             \
  and dest, _a, r14;                                                          \
  orr dest, dest, _b, lsl #16                                                 \


.globl ext_symbol(render_line_scale_width_256)

ext_symbol(render_line_scale_width_256):
  stmdb sp!, { r4 - r11, lr }

  ldr r2, 0b + 0x4                @ r2 = &vdc.screen_width
  ldr r12, 0b + 0x10              @ r12 = g_rb_mask
  ldr r2, [r2]                    @ r2 = vdc.screen_width
  ldr r14, 0b + 0x8               @ r14 = 0xFFFF, another useful constant

 1:
  @ get first 4 pixels
  ldrh r3, [r1], #2
  ldrh r4, [r1], #2
  ldrh r6, [r1], #2
  ldrh r5, [r1], #2

  @ extend
  pixel_extend(r7, r3)
  pixel_extend(r4, r4)
  pixel_extend(r8, r6)
  pixel_extend(r9, r5)

  @ get first two colors in r3 and r10, pack to r7
  color_blend_25_75(r10, r7, r4)
  pack_pixels(r3, r3, r10)

  @ get next two colors in r10 and r11, pack to r4
  color_blend_50_50(r10, r4, r8)
  color_blend_25_75(r11, r9, r8)
  pack_pixels(r4, r10, r11)

  @ next color is in r5

  @ get next 4 pixels
  ldrh r6, [r1], #2
  ldrh r7, [r1], #2
  ldrh r8, [r1], #2
  ldrh r9, [r1], #2

  @ combine first new pixel with last old one
  pack_pixels(r5, r5, r6)

  @ extend
  pixel_extend(r6, r6)
  pixel_extend(r7, r7)
  pixel_extend(r8, r8)

  @ get next two colors in r10 and r11, pack to r6
  color_blend_25_75(r10, r6, r7)
  color_blend_50_50(r11, r7, r8)
  pack_pixels(r6, r10, r11)

  @ get last colors in r10 and r9, pack to r7
  pixel_extend(r11, r9)
  color_blend_25_75(r10, r11, r8)
  pack_pixels(r7, r10, r9)

  @ store 10 pixels
  stmia r0!, { r3 - r7 }

  subs r2, r2, #8
  bne 1b

  ldmia sp!, { r4 - r11, pc }


#define expand_pixel(src)                                                     \
  tst src, #0xFF00;                                                           \
  addne src, r11, src, lsr #8;                                                \
  mov src, src, lsl #1;                                                       \
  ldrh src, [r10, src]                                                        \

.globl ext_symbol(render_line_expand_scale_width_256)

ext_symbol(render_line_expand_scale_width_256):
  stmdb sp!, { r4 - r11, lr }

  ldr r2, 0b + 0x4                @ r2 = &vdc.screen_width
  ldr r12, 0b + 0x10              @ r12 = g_rb_mask
  ldr r2, [r2]                    @ r2 = vdc.screen_width
  ldr r14, 0b + 0x8               @ r14 = 0xFFFF, another useful constant

 1:
  @ Unfortunately, there are no registers left over for these, so they have to
  @ be reloaded twice inside here
  ldr r10, 0b                     @ r10 = vce.palette_cache
  mov r11, #0x100                 @ r11 = 0x100

  @ get first 4 pixels
  ldrh r3, [r1], #2
  ldrh r4, [r1], #2
  ldrh r6, [r1], #2
  ldrh r5, [r1], #2

  @ expand
  expand_pixel(r3)
  expand_pixel(r4)
  expand_pixel(r6)
  expand_pixel(r5)

  @ extend
  pixel_extend(r7, r3)
  pixel_extend(r4, r4)
  pixel_extend(r8, r6)
  pixel_extend(r9, r5)

  @ get first two colors in r3 and r10, pack to r7
  color_blend_25_75(r10, r7, r4)
  pack_pixels(r3, r3, r10)

  @ get next two colors in r10 and r11, pack to r4
  color_blend_50_50(r10, r4, r8)
  color_blend_25_75(r11, r9, r8)
  pack_pixels(r4, r10, r11)

  @ next color is in r5

  @ get next 4 pixels
  ldrh r6, [r1], #2
  ldrh r7, [r1], #2
  ldrh r8, [r1], #2
  ldrh r9, [r1], #2

  ldr r10, 0b                     @ r10 = vce.palette_cache
  mov r11, #0x100                 @ r11 = 0x100

  @ expand
  expand_pixel(r6)
  expand_pixel(r7)
  expand_pixel(r8)
  expand_pixel(r9)

  @ combine first new pixel with last old one
  pack_pixels(r5, r5, r6)

  @ extend
  pixel_extend(r6, r6)
  pixel_extend(r7, r7)
  pixel_extend(r8, r8)

  @ get next two colors in r10 and r11, pack to r6
  color_blend_25_75(r10, r6, r7)
  color_blend_50_50(r11, r7, r8)
  pack_pixels(r6, r10, r11)

  @ get last colors in r10 and r9, pack to r7
  pixel_extend(r11, r9)
  color_blend_25_75(r10, r11, r8)
  pack_pixels(r7, r10, r9)

  @ store 10 pixels
  stmia r0!, { r3 - r7 }

  subs r2, r2, #8
  bne 1b

  ldmia sp!, { r4 - r11, pc }



@ live: bat_offset (r1), x_tile_offset (r5), bg_width (r3),
@       vdc.dirty_tiles (r4), tile_base (r8), x_pixel_offset (r6),
@       screen_width (r2), dest (r0), vce.palette_cache (r7),
@       tile_number (r11)
@ reload: current_row (r9)


__cache_tile:
  @ save the living from being smashed by the function
  stmdb sp!, { r0 - r3, r14 }

  mov r0, r11                     @ load parameter
  call_c(cache_tile, 5 + 5)       @ cache_tile(tile_number)
  mov r0, #0                      @ constant 0
  strb r0, [r4, r11]              @ vdc.dirty_tiles[tile_number] = 0
  ldr r9, [r8, r11, lsl #5]       @ reload pixel row

  @ restore the living and go back
  ldmia sp!, { r0 - r3, pc }


@ These are special versions that interleave some operation to relieve
@ ldr stalls from the palette load.

#define render_pixel_sha_direct(sh_offset, src)                               \
  and r11, src, #0xF              ;/* mask out pixel                        */\
  ldr r11, [r10, r11, lsl #1]     ;/* load palette cache entry              */\
  mov r12, r9, lsr #(sh_offset)   ;/* load next pixel                       */\
                                  ;/* unalignment stall 50% of the time     */\
  strh r11, [r0], pixel_pitch(2)   /* write out to dest                     */\

#define render_pixel_sha_indirect(sh_offset)                                  \
  and r11, r14, r9, lsr #(sh_offset);                                         \
  orr r11, r11, r10, lsl #4       ;/* combine pixel with palette            */\
  strh r11, [r0], #2               /* write out to dest                     */\

#define render_pixel_and_indirect()                                           \
  and r11, r14, r9;                                                           \
  orr r11, r11, r10, lsl #4       ;/* combine pixel with palette            */\
  strh r11, [r0], #2               /* write out to dest                     */\


#define render_pixel_sub_direct(count, src)                                   \
  ldr r11, [r10, src, lsl #1]     ;/* load palette cache entry              */\
  subs count, count, #1           ;/* decrement counter                     */\
                                  ;/* unalignment stall 50% of the time     */\
  strh r11, [r0], pixel_pitch(2)   /* write out to dest                     */\

#define render_pixel_sub_indirect(count, src)                                \
  orr r11, src, r10, lsl #4       ;/* combine pixel with palette            */\
  subs count, count, #1           ;/* decrement counter                     */\
  strh r11, [r0], #2               /* write out to dest                     */\

#define render_pixel_sub2_indirect()                                          \
  and r11, r14, r9, lsr #28;      ;/* load/mask pixel                       */\
  orr r11, r11, r10, lsl #4       ;/* combine pixel with palette            */\
  subs r2, r2, #1;                ;/* decrement loop counter                */\
  strh r11, [r0], #2               /* write out to dest                     */\


#define render_bg_prepare_direct()                                            \
  add r10, r7, r10, lsl #5         /* offset palette                        */\

#define render_bg_prepare_indirect()                                          \
  mov r14, #0xF                    /* load mask to r14                      */\


@ live: bat_offset (r1), x_tile_offset (r5), bg_width (r3),
@       vdc.dirty_tiles (r4), tile_base (r8), x_pixel_offset (r6),
@       screen_width (r2), dest (r0), vce.palette_cache (r7) (direct only)
@ next: current_row (r9), palette_offset/current_palette (r10),
@       current_pixel (r11)
@ free: r12, r14

#define render_bg_get_bat_entry(type)                                         \
  ldrh r12, [r1, r5]              ;/* r12 = bat_offset[x_tile_offset];      */\
  add r5, r5, #2                  ;/* x_tile_offset++ (word address)        */\
  and r5, r5, r3                  ;/* x_tile_offset &= bg_width             */\
  bic r11, r12, #0xF800           ;/* isolate tile number from BAT entry    */\
  ldrb r14, [r4, r11]             ;/* r14 = vdc.dirty_tiles[tile_number]    */\
  mov r10, r12, lsr #12           ;/* isolate palette from BAT entry        */\
  ldr r9, [r8, r11, lsl #5]       ;/* load pixel row                        */\
  cmp r14, #0                     ;/* is the tile dirty?                    */\
  blne __cache_tile               ;/* if so, update it                      */\
  render_bg_prepare_##type()       /* setup additional reg                  */\

#define render_bg_row_pixels_loop(c, r_t)                                     \
 1:;                                                                          \
  and r11, r9, #0xF               ;/* current_pixel = current_row & 0xF     */\
  render_pixel_sub_##r_t(c, r11)  ;/* render the pixel, decrement counter   */\
  mov r9, r9, lsr #4              ;/* switch to next pixel                  */\
  bne 1b                           /* go to next pixel                      */\


@ This is like the above but with the loop expanded to 8.

#define render_bg_row_pixels_direct(r_t)                                      \
  render_pixel_sha_direct(4, r9)  ;/* mask/render pixel 0, load pixel 1     */\
  render_pixel_sha_direct(8, r12) ;/* mask/render pixel 1, load pixel 2     */\
  render_pixel_sha_direct(12, r12);/* mask/render pixel 2, load pixel 3     */\
  render_pixel_sha_direct(16, r12);/* mask/render pixel 3, load pixel 4     */\
  render_pixel_sha_direct(20, r12);/* mask/render pixel 4, load pixel 5     */\
  render_pixel_sha_direct(24, r12);/* mask/render pixel 5, load pixel 6     */\
  render_pixel_sha_direct(28, r12);/* mask/render pixel 6, load pixel 7     */\
  render_pixel_sub_direct(r2, r12) /* render pixel 7, sub counter           */\

#define render_bg_row_pixels_indirect()                                       \
  render_pixel_and_indirect()     ;/* mask/render pixel 0, load pixel 1     */\
  render_pixel_sha_indirect(4)    ;/* mask/render pixel 1, load pixel 2     */\
  render_pixel_sha_indirect(8)    ;/* mask/render pixel 2, load pixel 3     */\
  render_pixel_sha_indirect(12)   ;/* mask/render pixel 3, load pixel 4     */\
  render_pixel_sha_indirect(16)   ;/* mask/render pixel 4, load pixel 5     */\
  render_pixel_sha_indirect(20)   ;/* mask/render pixel 5, load pixel 6     */\
  render_pixel_sha_indirect(24)   ;/* mask/render pixel 6, load pixel 7     */\
  render_pixel_sub2_indirect()     /* render pixel 7, sub counter           */\


#define load_palette_cache_ptr_indirect(dest)                                 \

#define load_palette_cache_ptr_direct(dest)                                   \
  ldr dest, 0b + 0x20                                                         \

0:
  .long VDC_VRAM_OFFSET           @ 0x0
  .long VDC_MWR_OFFSET            @ 0x4
  .long VDC_BXR_OFFSET            @ 0x8
  .long VDC_EFFECTIVE_BYR_OFFSET  @ 0xC
  .long VDC_SCREEN_WIDTH_OFFSET   @ 0x10
  .long VDC_TILE_CACHE_OFFSET     @ 0x14
  .long VDC_DIRTY_TILES_OFFSET    @ 0x18
  .long VDC_OVERDRAW_WIDTH_OFFSET @ 0x1C
  .long VCE_PALETTE_CACHE_OFFSET  @ 0x20


#define render_bg_line_maker(render_type)                                     \
.globl ext_symbol(render_bg_line_##render_type);                             \
                                                                              \
ext_symbol(render_bg_line_##render_type):;                                    \
  /* Going to need all the registers for this one.                          */\
  stmdb sp!, { r4 - r11, r14 };                                               \
                                                                              \
  ldr r1, 0b + 0x4                ;/* r1 = &vdc.mwr                         */\
  ldr r2, 0b + 0x10               ;/* r2 = &vdc.screen_width                */\
  ldr r1, [r1]                    ;/* r1 = vdc.mwr                          */\
  mov r3, r1, lsr #4              ;/* r3 = vdc.mwr >> 4                     */\
  and r3, r3, #0x3                ;/* r3 = (vdc.mwr >> 4) & 0x3 (bg_width)  */\
  mov r1, r1, lsr #6              ;/* r1 = vdc.mwr >> 6                     */\
  and r1, r1, #0x1                ;/* r1 = (vdc.mwr >> 6) & 0x1 (bg_height) */\
                                                                              \
  ldr r2, [r2]                    ;/* r2 = vdc.screen_width                 */\
  ldr r5, 0b + 0x8                ;/* r5 = &vdc.bxr                         */\
  mov r2, r2, lsr #3              ;/* r2 = vdc.screen_width / 8 (screen_w)  */\
  ldr r9, 0b + 0x1C               ;/* r9 = &vdc.overdraw_width_offset       */\
  ldr r5, [r5]                    ;/* r5 = vdc.bxr                          */\
  ldr r9, [r9]                    ;/* r9 = vdc.overdraw_width_offset        */\
  ldr r7, 0b + 0xC                ;/* r7 = &vdc.effective_byr               */\
  add r5, r5, r9                  ;/* r9 = vdc.bxr + overdraw_width_offset  */\
  and r6, r5, #0x7                ;/* r6 = vdc.bxr % 8 (x_pixel_offset)     */\
  ldr r7, [r7]                    ;/* r7 = vdc.effective_byr                */\
  mov r5, r5, lsr #3              ;/* r5 = vdc.bxr / 8 (x_tile_offset)      */\
  and r8, r7, #0x7                ;/* r8 = vdc.eff_byr % 8 (y_pixel_offset) */\
  ldr r9, 0b + 0x14               ;/* r9 = vdc.tile_cache                   */\
  mov r7, r7, lsr #3              ;/* r7 = vdc.eff_byr / 8 (y_tile_offset)  */\
                                                                              \
  add r8, r9, r8, lsl #2          ;/* r8 = vdc.tcache + y_p_off (tile_base) */\
                                                                              \
  cmp r3, #2                      ;/* if(bg_width == 2)                     */\
  moveq r3, #3                    ;/*   bg_width = 3                        */\
  add r9, r3, #1                  ;/* r9 = bg_width + 1                     */\
  mov r3, r9, lsl #5              ;/* bg_width = (bg_width + 1) * 32        */\
                                                                              \
  add r9, r1, #1                  ;/* r9 = bg_height + 1                    */\
  mov r1, r9, lsl #5              ;/* bg_height = (bg_height + 1) * 32      */\
  sub r1, r1, #1                  ;/* bg_height--                           */\
  and r7, r7, r1                  ;/* y_tile_offset &= bg_height            */\
                                                                              \
  ldr r9, 0b + 0x0                ;/* r9 = vdc.vram                         */\
  mov r3, r3, lsl #1              ;/* adjust bg_width for halfword access   */\
  mla r1, r7, r3, r9              ;/* y_tile_offset *= bg_width             */\
                                                                              \
  sub r3, r3, #1                  ;/* bg_width--                            */\
                                                                              \
  ldr r4, 0b + 0x18               ;/* r4 = vdc.dirty_tiles                  */\
  and r5, r3, r5, lsl #1          ;/* x_t_off = (x_t_off * 2) & bg_width    */\
                                                                              \
  /* Load palette cache pointer for direct render type                      */\
  load_palette_cache_ptr_##render_type(r7);                                   \
                                                                              \
  /* live: bat_offset (r1), x_tile_offset (r5), bg_width (r3),              */\
  /*       vdc.dirty_tiles (r4), tile_base (r8), x_pixel_offset (r6),       */\
  /*       screen_width (r2), dest (r0), vce.palette_cache (r7)             */\
  /* next: current_row (r9), palette_offset/current_palette (r10),          */\
  /*       current_pixel (r11)                                              */\
  /* free: r12, r14                                                         */\
                                                                              \
  cmp r6, #0                      ;/* is x_pixel_offset non-zero?           */\
  beq 2f                          ;/* if not skip left hand trailing        */\
                                                                              \
  render_bg_get_bat_entry(render_type);                                       \
  mov r14, r6, lsl #2             ;/* r14 = x_pixel_offset * 4              */\
  mov r9, r9, lsr r14             ;/* current_row >>= (4 * x_pixel_offset)  */\
  rsb r14, r6, #8                 ;/* r14 = 8 - x_pixel_offset              */\
  render_bg_row_pixels_loop(r14, render_type);                                \
  subs r2, r2, #1                 ;/* take off from screen width            */\
  beq 3f                          ;/* if it's zero just do the other side   */\
                                                                              \
 2:;                                                                          \
  render_bg_get_bat_entry(render_type);                                       \
                                                                              \
  /* This will render and decrement the counter (interleaved in stall)      */\
  render_bg_row_pixels_##render_type();                                       \
  bne 2b                          ;/* do another tile                       */\
                                                                              \
  cmp r6, #0                      ;/* is x_pixel_offset non-zero?           */\
  beq 4f                          ;/* if not skip right hand trailing       */\
                                                                              \
 3:;                                                                          \
  render_bg_get_bat_entry(render_type);                                       \
  render_bg_row_pixels_loop(r6, render_type);                                 \
                                                                              \
 4:;                                                                          \
  ldmia sp!, { r4 - r11, pc }                                                 \

render_bg_line_maker(direct);
render_bg_line_maker(indirect);


#define SPRITE_ATTRIBUTE_CLIP_LEFT   0x0010
#define SPRITE_ATTRIBUTE_CLIP_RIGHT  0x0020
#define SPRITE_ATTRIBUTE_PRIORITY    0x0080
#define SPRITE_ATTRIBUTE_WIDE        0x0100
#define SPRITE_ATTRIBUTE_CG          0x0200
#define SPRITE_ATTRIBUTE_HFLIP       0x0800
#define SPRITE_ATTRIBUTE_VFLIP       0x8000


#define reg_base                   r0
#define reg_dest                   r1
#define reg_sprite_count           r2
#define reg_dirty_patterns         r3
#define reg_pattern_index          r4
#define reg_pattern_cache_base     r5
#define reg_current_half_row       r6
#define reg_x                      r7
#define reg_partial_x              r8
#define reg_t1                     r8
#define reg_attributes             r9
#define reg_palette_offset         r9
#define reg_screen_width           r10
#define reg_sat_cache_entry        r11
#define reg_palette_base           r12
#define reg_t2                     r12
#define reg_t0                     r14
#define reg_current_pixel          r14


__cache_pattern_a:
  @ save the living from being smashed by the function
  stmdb sp!, { r0 - r3, r12, r14 }

  mov r1, #0                      @ constant 0
  @ vdc.dirty_patterns[pattern_offset / 32] = 0
  strb r1, [reg_dirty_patterns, reg_pattern_index, lsr #5]
  mov r0, reg_pattern_index, lsr #5
  call_c(cache_pattern, 5 + 6)    @ cache_pattern(pattern_offset)

  @ restore the living and go back
  ldmia sp!, { r0 - r3, r12, pc }

__cache_pattern_b:
  @ save the living from being smashed by the function
  stmdb sp!, { r0 - r3, r12, r14 }

  mov r1, #0                      @ constant 0
  @ vdc.dirty_patterns[pattern_offset / 32] = 0
  add reg_t0, reg_pattern_index, #32;
  strb r1, [reg_dirty_patterns, reg_t0, lsr #5]
  mov r0, reg_t0, lsr #5          @ load parameter
  call_c(cache_pattern, 5 + 6)    @ cache_pattern(pattern_offset)

  @ restore the living and go back
  ldmia sp!, { r0 - r3, r12, pc }


0:
  .long VDC_STATUS_OFFSET

check_overflow_irq:
  ldrb reg_t1, [reg_sat_cache_entry, #0x183]
  cmp reg_t1, #0
  bxeq lr

  stmdb sp!, { r0 - r3, r12, lr }

  ldr r1, 0b                     @ r1 = &vdc.status
  mov r0, #IRQ_VDC
  ldr r2, [r1]                   @ r1 = vdc.status
  @ Trigger overflow IRQ status
  orr r2, r2, #VDC_STATUS_OVERFLOW_IRQ;
  str r2, [r1]
  call_c(raise_interrupt, 5 + 6)
  ldmia sp!, { r0 - r3, r12, pc }


#define render_pixel_spr_partial_direct_high(test_region, shift_dir)          \
  ldr reg_current_pixel, [reg_palette_offset, reg_current_pixel, lsl #1];     \
  tst reg_current_half_row, #(test_region);                                   \
  mov reg_current_half_row, reg_current_half_row, shift_dir #4;               \
  strneh reg_current_pixel, [reg_dest];                                       \
  add reg_dest, reg_dest, pixel_pitch(2);                                     \
  subs reg_partial_x, reg_partial_x, #1                                       \

#define render_pixel_spr_partial_indirect_high(test_region, shift_dir)        \
  ldrh reg_t2, [reg_dest];                                                    \
  orr reg_current_pixel, reg_current_pixel, reg_palette_offset, lsl #4;       \
  tst reg_current_half_row, #(test_region);                                   \
  mov reg_current_half_row, reg_current_half_row, shift_dir #4;               \
  and reg_t2, reg_t2, #0xFF;                                                  \
  orr reg_t2, reg_t2, reg_current_pixel, lsl #8;                              \
  strneh reg_t2, [reg_dest];                                                  \
  add reg_dest, reg_dest, #2;                                                 \
  subs reg_partial_x, reg_partial_x, #1                                       \


#define render_pixel_spr_partial_indirect_low(test_region, shift_dir)         \
  tst reg_current_half_row, #(test_region);                                   \
  mov reg_current_half_row, reg_current_half_row, shift_dir #4;               \
  beq 0f;                                                                     \
  ldrh reg_t2, [reg_dest];                                                    \
  orr reg_current_pixel, reg_current_pixel, reg_palette_offset, lsl #4;       \
  and reg_t2, reg_t2, #0xFF;                                                  \
  tst reg_t2, #0xF;                                                           \
  orr reg_t2, reg_t2, reg_current_pixel, lsl #8;                              \
  streqh reg_t2, [reg_dest];                                                  \
                                                                              \
0:;                                                                           \
  add reg_dest, reg_dest, #2;                                                 \
  subs reg_partial_x, reg_partial_x, #1                                       \


#define render_spr_half_row_pixels_partial_no_flip(render_type, priority)     \
 1:                                                                           \
  and reg_current_pixel, reg_current_half_row, #0xF;                          \
  render_pixel_spr_partial_##render_type##_##priority(0x0F, lsr);             \
  bne 1b                                                                      \


#define render_spr_half_row_pixels_partial_flip(render_type, priority)        \
 1:                                                                           \
  mov reg_current_pixel, reg_current_half_row, lsr #28;                       \
  render_pixel_spr_partial_##render_type##_##priority(0x0F << 28, lsl);       \
  bne 1b                                                                      \


#define render_pixel_spr_sha_direct_high(sh_offset, src, pixel_offset)        \
  and reg_current_pixel, src, #0xF;                                           \
  ldr reg_current_pixel, [reg_palette_offset, reg_current_pixel, lsl #1];     \
  tst reg_current_half_row, #(0x0F << sh_offset);                             \
  mov reg_t1, reg_current_half_row, lsr #(sh_offset + 4);                     \
  strneh reg_current_pixel, [reg_dest, pixel_pitch(pixel_offset)]             \

#define render_pixel_spr_sha_direct_high_edge(flip)                           \
  and reg_current_pixel, reg_t1, #0xF;                                        \
  ldr reg_current_pixel, [reg_palette_offset, reg_current_pixel, lsl #1];     \
  tst reg_current_half_row, #(0x0F << 28);                                    \
  add reg_dest, reg_dest, pixel_pitch(16);                                    \
  strneh reg_current_pixel, [reg_dest, pixel_pitch(spr_##flip##_edge_offset)] \


#define render_pixel_spr_sha_indirect_high(sh_offset, src, pixel_offset)      \
  ldrh reg_t2, [reg_dest, #(pixel_offset)];                                   \
  and reg_current_pixel, src, #0xF;                                           \
  orr reg_current_pixel, reg_current_pixel, reg_palette_offset, lsl #4;       \
  tst reg_current_half_row, #(0x0F << sh_offset);                             \
  mov reg_t1, reg_current_half_row, lsr #(sh_offset + 4);                     \
  and reg_t2, reg_t2, #0xFF;                                                  \
  orr reg_t2, reg_t2, reg_current_pixel, lsl #8;                              \
  strneh reg_t2, [reg_dest, #(pixel_offset)]                                  \

#define render_pixel_spr_sha_indirect_high_edge(flip)                         \
  ldrh reg_t2, [reg_dest, #spr_##flip##_offset(14)];                          \
  and reg_current_pixel, reg_t1, #0xF;                                        \
  orr reg_current_pixel, reg_current_pixel, reg_palette_offset, lsl #4;       \
  tst reg_current_half_row, #(0x0F << 28);                                    \
  add reg_dest, reg_dest, #16;                                                \
  and reg_t2, reg_t2, #0xFF;                                                  \
  orr reg_t2, reg_t2, reg_current_pixel, lsl #8;                              \
  strneh reg_t2, [reg_dest, #spr_##flip##_edge_offset]                        \


#define render_pixel_spr_sha_indirect_low(sh_offset, src, pixel_offset)       \
  tst reg_current_half_row, #(0x0F << sh_offset);                             \
  mov reg_t1, reg_current_half_row, lsr #(sh_offset + 4);                     \
  beq 0f;                                                                     \
  ldrh reg_t2, [reg_dest, #(pixel_offset)];                                   \
  and reg_current_pixel, src, #0xF;                                           \
  orr reg_current_pixel, reg_current_pixel, reg_palette_offset, lsl #4;       \
  and reg_t2, reg_t2, #0xFF;                                                  \
  tst reg_t2, #0xF;                                                           \
  orreq reg_t2, reg_t2, reg_current_pixel, lsl #8;                            \
  strh reg_t2, [reg_dest, #(pixel_offset)];                                   \
                                                                              \
 0:                                                                           \

#define render_pixel_spr_sha_indirect_low_edge(flip)                          \
  tst reg_current_half_row, #(0x0F << 28);                                    \
  add reg_dest, reg_dest, #16;                                                \
  beq 0f;                                                                     \
  ldrh reg_t2, [reg_dest, #spr_##flip##_edge_offset];                         \
  and reg_current_pixel, reg_t1, #0xF;                                        \
  orr reg_current_pixel, reg_current_pixel, reg_palette_offset, lsl #4;       \
  and reg_t2, reg_t2, #0xFF;                                                  \
  tst reg_t2, #0xF;                                                           \
  orreq reg_t2, reg_t2, reg_current_pixel, lsl #8;                            \
  strh reg_t2, [reg_dest, #spr_##flip##_edge_offset];                         \
                                                                              \
 0:                                                                           \



#define spr_flip_offset(offset)     (14 - offset)
#define spr_no_flip_offset(offset)  offset
#define spr_flip_edge_offset        -16
#define spr_no_flip_edge_offset     -2


#define render_spr_half_row_pixels(r_type, p, flip)                           \
  render_pixel_spr_sha_##r_type##_##p(0, reg_current_half_row,                \
   spr_##flip##_offset(0));                                                   \
  render_pixel_spr_sha_##r_type##_##p(4, reg_t1, spr_##flip##_offset(2));     \
  render_pixel_spr_sha_##r_type##_##p(8, reg_t1, spr_##flip##_offset(4));     \
  render_pixel_spr_sha_##r_type##_##p(12, reg_t1, spr_##flip##_offset(6));    \
  render_pixel_spr_sha_##r_type##_##p(16, reg_t1, spr_##flip##_offset(8));    \
  render_pixel_spr_sha_##r_type##_##p(20, reg_t1, spr_##flip##_offset(10));   \
  render_pixel_spr_sha_##r_type##_##p(24, reg_t1, spr_##flip##_offset(12));   \
  render_pixel_spr_sha_##r_type##_##p##_edge(flip)                            \


#define render_spr_check_dirty_a(offset)                                      \
  ldrb reg_t0, [reg_dirty_patterns, reg_pattern_index, lsr #5];               \
  cmp reg_t0, #0;                                                             \
  blne __cache_pattern_a                                                      \


#define render_spr_check_dirty_b(offset)                                      \
  add reg_t0, reg_pattern_index, #32;                                         \
  ldrb reg_t0, [reg_dirty_patterns, reg_t0, lsr #5];                          \
  cmp reg_t0, #0;                                                             \
  blne __cache_pattern_b                                                      \

#define render_spr_check_dirty_0(offset)                                      \
  render_spr_check_dirty_a()                                                  \

#define render_spr_check_dirty_1(offset)                                      \
  render_spr_check_dirty_a()                                                  \

#define render_spr_check_dirty_32(offset)                                     \
  render_spr_check_dirty_b()                                                  \

#define render_spr_check_dirty_33(offset)                                     \
  render_spr_check_dirty_b()                                                  \


#define render_spr_ordered_load_index_0()                                     \
  ldr reg_current_half_row, [reg_pattern_cache_base, reg_pattern_index,       \
   lsl #2]                                                                    \

#define render_spr_ordered_load_index_offset(offset)                          \
  add reg_t0, reg_pattern_index, #(offset);                                   \
  ldr reg_current_half_row, [reg_pattern_cache_base, reg_t0, lsl #2]          \

#define render_spr_ordered_load_index_1()                                     \
  render_spr_ordered_load_index_offset(1)                                     \

#define render_spr_ordered_load_index_32()                                    \
  render_spr_ordered_load_index_offset(32)                                    \

#define render_spr_ordered_load_index_33()                                    \
  render_spr_ordered_load_index_offset(33)                                    \

#define render_spr_row_ordered_normal(offset_a, offset_b, render_type,        \
 priority, flip_type)                                                         \
  render_spr_check_dirty_##offset_a();                                        \
  render_spr_ordered_load_index_##offset_a();                                 \
  render_spr_half_row_pixels(render_type, priority, flip_type);               \
  render_spr_ordered_load_index_##offset_b();                                 \
  render_spr_half_row_pixels(render_type, priority, flip_type)                \


#define render_spr_row_partial_shift_left_no_flip()                           \
  rsb reg_t0, reg_partial_x, #8;                                              \
  mov reg_t0, reg_t0, lsl #2;                                                 \
  mov reg_current_half_row, reg_current_half_row, lsr reg_t0                  \

#define render_spr_row_partial_shift_left_flip()                              \
  rsb reg_t0, reg_partial_x, #8;                                              \
  mov reg_t0, reg_t0, lsl #2;                                                 \
  mov reg_current_half_row, reg_current_half_row, lsl reg_t0                  \


#define render_spr_row_ordered_partial_left(off_a, off_b, r_type, p, f_type)  \
  add reg_partial_x, reg_x, #16;                                              \
  render_spr_check_dirty_##off_a();                                           \
  cmp reg_partial_x, #8;                                                      \
  bgt 2f;                                                                     \
                                                                              \
  render_spr_ordered_load_index_##off_b();                                    \
  render_spr_row_partial_shift_left_##f_type();                               \
  render_spr_half_row_pixels_partial_##f_type(r_type, p);                     \
  b 3f;                                                                       \
                                                                              \
 2:;                                                                          \
  sub reg_partial_x, reg_partial_x, #8;                                       \
  render_spr_ordered_load_index_##off_a();                                    \
  render_spr_row_partial_shift_left_##f_type();                               \
  render_spr_half_row_pixels_partial_##f_type(r_type, p);                     \
  render_spr_ordered_load_index_##off_b();                                    \
  render_spr_half_row_pixels(r_type, p, f_type);                              \
                                                                              \
 3:                                                                           \


#define render_spr_row_ordered_partial_right(off_a, off_b, r_type, p, f_type) \
  sub reg_partial_x, reg_screen_width, reg_x;                                 \
  render_spr_check_dirty_##off_a();                                           \
  render_spr_ordered_load_index_##off_a();                                    \
                                                                              \
  cmp reg_partial_x, #8;                                                      \
  bgt 2f;                                                                     \
  render_spr_half_row_pixels_partial_##f_type(r_type, p);                     \
  b 3f;                                                                       \
                                                                              \
 2:                                                                           \
  render_spr_half_row_pixels(r_type, p, f_type);                              \
  render_spr_ordered_load_index_##off_b();                                    \
  sub reg_partial_x, reg_screen_width, reg_x;                                 \
  sub reg_partial_x, reg_partial_x, #8;                                       \
  render_spr_half_row_pixels_partial_##f_type(r_type, p);                     \
                                                                              \
 3:                                                                           \

#define render_spr_row_wide_a_flip(type, render_type, priority)               \
  render_spr_row_ordered_##type(33, 32, render_type, priority, flip)          \

#define render_spr_row_wide_b_flip(type, render_type, priority)               \
  render_spr_row_ordered_##type(1, 0, render_type, priority, flip)            \

#define render_spr_row_wide_a_no_flip(type, render_type, priority)            \
  render_spr_row_ordered_##type(0, 1, render_type, priority, no_flip)         \

#define render_spr_row_wide_b_no_flip(type, render_type, priority)            \
  render_spr_row_ordered_##type(32, 33, render_type, priority,                \
   no_flip)                                                                   \


#define render_spr_wide_normal(render_type, priority, flip_type)              \
  render_spr_row_wide_a_##flip_type(normal, render_type, priority);           \
  render_spr_row_wide_b_##flip_type(normal, render_type, priority)            \

#define render_spr_wide_partial_left(render_type, priority, flip_type)        \
  render_spr_row_wide_a_##flip_type(partial_left, render_type, priority);     \
  render_spr_row_wide_b_##flip_type(normal, render_type, priority)            \

#define render_spr_wide_partial_right(render_type, priority, flip_type)       \
  render_spr_row_wide_a_##flip_type(normal, render_type, priority);           \
  add reg_x, reg_x, #16;                                                      \
  render_spr_row_wide_b_##flip_type(partial_right, render_type, priority)     \

#define render_spr_prepare_palette_direct()                                   \
  and reg_palette_offset, reg_attributes, #0xF;                               \
  add reg_palette_offset, reg_palette_base, reg_palette_offset, lsl #5        \

#define render_spr_prepare_palette_indirect()                                 \
  and reg_palette_offset, reg_attributes, #0xF                                \


#define render_spr_row_ordered(type, render_type, priority)                   \
  tst reg_attributes, #SPRITE_ATTRIBUTE_HFLIP;                                \
  beq render_spr_row_ordered_##type##_##render_type##_##priority##_no_flip;   \
                                                                              \
  tst reg_attributes, #SPRITE_ATTRIBUTE_WIDE;                                 \
  beq render_spr_row_ordered_##type##_##render_type##_##priority##_flip_s;    \
  render_spr_prepare_palette_##render_type();                                 \
  render_spr_wide_##type(render_type, priority, flip);                        \
  b render_spr_row_ordered_##type##_##render_type##_##priority##_join;        \
                                                                              \
 render_spr_row_ordered_##type##_##render_type##_##priority##_flip_s:;        \
  render_spr_prepare_palette_##render_type();                                 \
  render_spr_row_ordered_##type(1, 0, render_type, priority, flip);           \
  b render_spr_row_ordered_##type##_##render_type##_##priority##_join;        \
                                                                              \
 render_spr_row_ordered_##type##_##render_type##_##priority##_no_flip:;       \
  tst reg_attributes, #SPRITE_ATTRIBUTE_WIDE;                                 \
  beq render_spr_row_ordered_##type##_##render_type##_##priority##_no_flip_s; \
  render_spr_prepare_palette_##render_type();                                 \
  render_spr_wide_##type(render_type, priority, no_flip);                     \
  b render_spr_row_ordered_##type##_##render_type##_##priority##_join;        \
                                                                              \
 render_spr_row_ordered_##type##_##render_type##_##priority##_no_flip_s:;     \
  render_spr_prepare_palette_##render_type();                                 \
  render_spr_row_ordered_##type(0, 1, render_type, priority, no_flip);        \
                                                                              \
 render_spr_row_ordered_##type##_##render_type##_##priority##_join:           \


#define render_spr_row(render_type, priority)                                 \
  tst reg_attributes,                                                         \
   #(SPRITE_ATTRIBUTE_CLIP_LEFT | SPRITE_ATTRIBUTE_CLIP_RIGHT);               \
  beq render_spr_row_##render_type##_##priority##_no_clip;                    \
                                                                              \
  tst reg_attributes, #SPRITE_ATTRIBUTE_CLIP_LEFT;                            \
  beq render_spr_row_##render_type##_##priority##_right_clip;                 \
                                                                              \
  mov reg_dest, reg_base;                                                     \
  render_spr_row_ordered(partial_left, render_type, priority);                \
  b render_spr_row_##render_type##_##priority##_join;                         \
                                                                              \
 render_spr_row_##render_type##_##priority##_right_clip:;                     \
  add_pixel_pitch_variable_##render_type(reg_dest, reg_base, reg_x);          \
  render_spr_row_ordered(partial_right, render_type, priority);               \
  b render_spr_row_##render_type##_##priority##_join;                         \
                                                                              \
 render_spr_row_##render_type##_##priority##_no_clip:;                        \
  add_pixel_pitch_variable_##render_type(reg_dest, reg_base, reg_x);          \
  render_spr_row_ordered(normal, render_type, priority);                      \
                                                                              \
 render_spr_row_##render_type##_##priority##_join:                            \


#define render_spr_row_decision_direct()                                      \
  render_spr_row(direct, high)                                                \

#define render_spr_row_decision(type)                                         \
  tst reg_attributes, #SPRITE_ATTRIBUTE_PRIORITY;                             \
  beq render_spr_row_decision_hp;                                             \
                                                                              \
  render_spr_row(type, high);                                                 \
  b render_spr_row_decision_join;                                             \
                                                                              \
 render_spr_row_decision_hp:                                                  \
  render_spr_row(type, low);                                                  \
                                                                              \
 render_spr_row_decision_join:                                                \

#define render_spr_row_decision_indirect()                                    \
  render_spr_row_decision(indirect)                                           \


#define render_spr_line_body_load_palette_base_direct()                       \
  ldr reg_palette_base, 0b + 0x10                                             \

#define render_spr_line_body_load_palette_base_indirect()                     \


#define render_spr_line_body(render_type)                                     \
  stmdb sp!, { r4 - r11, lr };                                                \
  ldr reg_pattern_cache_base, 0b + 0x4;                                       \
  ldr reg_dirty_patterns, 0b + 0x8;                                           \
  ldr reg_screen_width, 0b + 0xC;                                             \
  render_spr_line_body_load_palette_base_##render_type();                     \
  ldr reg_sat_cache_entry, 0b + 0x14;                                         \
  ldr reg_screen_width, [reg_screen_width];                                   \
  ldr reg_t0, 0b;                                                             \
  mov reg_t1, #0x184;                                                         \
  ldr reg_t0, [reg_t0];           ;/* reg_t0 = vdc.cr                       */\
  /* reg_sat_cache_entry = &sat_cache.lines[line]                           */\
  mla reg_sat_cache_entry, r1, reg_t1, reg_sat_cache_entry;                   \
  tst reg_t0, #0x2                ;/* see if spr overflows are enabled      */\
  blne check_overflow_irq         ;/* do a more indepth check if so         */\
                                                                              \
  /* reg_sprite_count = sat_cache.lines[line].num_active                    */\
  ldrb reg_sprite_count, [reg_sat_cache_entry, #0x181];                       \
  subs reg_sprite_count, reg_sprite_count, #1;                                \
  bmi render_spr_line_##render_type##_done;                                   \
  /* reg_t0 = reg_sprite_count * 3                                          */\
  add reg_t0, reg_sprite_count, reg_sprite_count, lsl #1;                     \
  /* reg_sat_cache_entry = &sat_cache.lines[line].entries[spr_count]        */\
  add reg_sat_cache_entry, reg_sat_cache_entry, reg_t0, lsl #1;               \
                                                                              \
render_spr_line_main_loop_##render_type:                                      \
  ldrh reg_pattern_index, [reg_sat_cache_entry, #0];                          \
  ldrsh reg_x, [reg_sat_cache_entry, #2];                                     \
  ldrh reg_attributes, [reg_sat_cache_entry, #4];                             \
  /* Go to the previous SAT cache entry                                     */\
  sub reg_sat_cache_entry, reg_sat_cache_entry, #6;                           \
  render_spr_row_decision_##render_type();                                    \
  subs reg_sprite_count, reg_sprite_count, #1;                                \
  bpl render_spr_line_main_loop_##render_type;                                \
                                                                              \
render_spr_line_##render_type##_done:                                         \
  ldmia sp!, { r4 - r11, pc }                                                 \


#define render_spr_line_maker(render_type)                                    \
0:                                                                            \
  .long VDC_CR_OFFSET             ;/* 0x0                                   */\
  .long VDC_PATTERN_CACHE_OFFSET  ;/* 0x4                                   */\
  .long VDC_DIRTY_PATTERNS_OFFSET ;/* 0x8                                   */\
  .long VDC_SCREEN_WIDTH_OFFSET   ;/* 0xC                                   */\
  .long VCE_SPALETTE_CACHE_OFFSET ;/* 0x10                                  */\
  .long SAT_CACHE_OFFSET          ;/* 0x14                                  */\
                                                                              \
.globl ext_symbol(render_spr_line_##render_type);                            \
                                                                              \
ext_symbol(render_spr_line_##render_type):;                                   \
  render_spr_line_body(render_type)                                           \

render_spr_line_maker(direct)
render_spr_line_maker(indirect)


.globl ext_symbol(render_line_fill)

0:
  .long VDC_SCREEN_WIDTH_OFFSET

ext_symbol(render_line_fill):
  stmdb sp!, { r4 - r7, r14 }

  orr r2, r1, r1, lsl #16         @ expand to 32bits
  ldr r1, 0b                      @ r1 = &vdc.screen_width
  mov r3, r2                      @ expand to 2 words
  ldr r1, [r1]                    @ r1 = vdc.screen_width
  mov r4, r2                      @ expand to 3 words
  mov r5, r2                      @ expand to 4 words
  mov r6, r2                      @ expand to 5 words
  mov r7, r2                      @ expand to 6 words
  mov r12, r2                     @ expand to 7 words
  mov r14, r2                     @ expand to 8 words

  @ screen.width should be a multiple of 8

  movs r1, r1, lsr #4             @ count 16 pixel blocks

  stmcsia r0!, { r2 - r5 }        @ store 4 words (8 pixels)

 1:
  @ store 8 words
  stmia r0!, { r2 - r7, r12, r14 }

  subs r1, r1, #1
  bne 1b

  ldmia sp!, { r4 - r7, pc }

