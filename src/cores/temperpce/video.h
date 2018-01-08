#ifndef VIDEO_H
#define VIDEO_H

#define RESOLUTION_WIDTH  320
#define RESOLUTION_HEIGHT 240

typedef enum
{
  SPRITE_ATTRIBUTE_HFLIP_CHECK    = 0x0040,
  SPRITE_ATTRIBUTE_PRIORITY       = 0x0080,
  SPRITE_ATTRIBUTE_WIDE           = 0x0100,
  SPRITE_ATTRIBUTE_CG             = 0x0200,
  SPRITE_ATTRIBUTE_HFLIP          = 0x0800,
  SPRITE_ATTRIBUTE_VFLIP          = 0x8000
} sprite_attribute_enum;

typedef enum
{
  NO_DEBUG,
  DEBUG_TILES,
  DEBUG_PATTERNS
} video_debug_mode_enum;

typedef enum
{
  VDC_STATUS_SPRITE_COLLISION_IRQ = 0x01,
  VDC_STATUS_OVERFLOW_IRQ         = 0x02,
  VDC_STATUS_RASTER_IRQ           = 0x04,
  VDC_STATUS_SATB_IRQ             = 0x08,
  VDC_STATUS_DMA_IRQ              = 0x10,
  VDC_STATUS_VBLANK_IRQ           = 0x20,
  VDC_STATUS_BUSY                 = 0x40
} vdc_status_enum;

typedef enum
{
  LINE_CACHE_FLAGS_LOW_PRIORITY = 0x1,
  LINE_CACHE_FLAGS_SPR0         = 0x2
} line_cache_flags_enum;

// 6 bytes, if we make it so with GCC

typedef struct __attribute__ ((__packed__))
{
  u16 pattern_line_offset;
  s16 x;
  u16 attributes;
} sat_cache_line_entry_struct;

// 388 (0x184) bytes
typedef struct __attribute__ ((__packed__))
{
  // entries: 384 (0x180) bytes
  sat_cache_line_entry_struct entries[64];
  u8 flags;
  u8 num_active;
  u8 num_present;
  u8 overflow;
} sat_cache_line_struct;

typedef struct
{
  sat_cache_line_struct lines[263];
} sat_cache_struct;

// 41B + 64KB + 512B + 128KB + 3040B
// 192KB + 3593B

typedef struct
{
  u16 vram[1024 * 32];
  u16 sat[64 * 4];

  u32 status;
  u32 register_select;
  u32 mawr;
  u32 marr;
  u32 cr;
  u32 rcr;
  u32 mwr;
  u32 bxr;
  u32 byr;
  u32 hds;
  u32 hsw;
  u32 hde;
  u32 hdw;
  u32 vds;
  u32 vsw;
  u32 vdw;
  u32 dcr;
  u32 vcr;
  u32 sour;
  u32 desr;
  u32 lenr;
  u32 satb;

  u32 write_latch;
  u32 read_latch;

  u32 raster_line;
  u32 effective_byr;
  u32 latched_mwr;

  u32 increment_value;

  u32 satb_dma_trigger;
  s32 satb_dma_irq_lines;

  u32 vram_dma_trigger;
  s32 vram_dma_irq_lines;

  u32 display_counter;
  u32 burst_mode;

  // Don't save these in a savestate.

  // This is a bunch of extra space for storing leading zeros and stuff.
  // It's here so that those zeros can be there persistently without having to
  // write them over and over.

  u8 bg_buffer[512 * 4];
  u32 bg_mask_buffer[512 * 4 / 32];

  sat_cache_struct sat_cache;

  u32 latch_vds;
  u32 latch_vsw;
  u32 latch_vdw;
  u32 latch_vcr;
  u32 vblank_line;
  u32 start_line;

  u32 vblank_active;

  u32 screen_width;
  u32 scanline_offset;
  u32 scroll_shift;
  u32 screen_end;

  u32 hds_cycles;

  // For 4bpp tiles over 64KB of VDC RAM, there's room for 1024 * 64 * 2 pixels.

  u8 tile_cache[(1024 * 64 * 2)];
  u8 pattern_cache[(1024 * 64 * 2)];

  // For 1024 * 64 * 2 pixels, an 8-bit mask stores transparency values for
  // 8 pixels.
  u8 tile_cache_transparency_mask[(1024 * 64 * 2) / 8];
  // Two times the masks are stored so that flipped masks can be as well.
  u16 pattern_cache_transparency_mask[((1024 * 64 * 2) / 16) * 2];

  // At 8x8x4bpp, each tile takes 32 bytes.
  u8 dirty_tiles[1024 * 64 / 32];
  // At 16x16x4bpp, each pattern takes 128 bytes
  u8 dirty_patterns[1024 * 64 / 128];
} vdc_struct;

typedef enum
{
  VPC_SELECT_VDC_A,
  VPC_SELECT_VDC_B
} vpc_select_enum;

typedef struct
{
  u16 window1_value;
  u16 window2_value;

  u16 window_status;

  u8 select;

  // Not saved in savestates
  u16 window_span_lengths[4];
  u8 window_span_status[4];
  u8 window_spans;
  u8 windows_dirty;
} vpc_struct;

typedef struct
{
  u32 palette[512];

  // Can be recalculated, don't save in save state.
  u32 palette_cache[512];
  u16 palette_cache_5551[512];

  u16 pixels_drawn[242];

  u32 control;
  u32 palette_offset;
  u32 num_lines;

  u32 frame_counter;

  // Inpersistent/debug info, don't save in save state.
  u32 screen_width;
  u32 screen_overdraw_offset;
  s32 screen_center_offset;

  u32 clear_edges;
  u32 frames_rendered;
  u32 scanline_cycles;
} vce_struct;

extern video_debug_mode_enum video_debug_mode;

extern vdc_struct vdc_a;
extern vdc_struct vdc_b;
extern vpc_struct vdp;
extern vce_struct vce;


#ifdef EXTERN_C_START
EXTERN_C_START
#endif 

u32 vdc_status_vdc_a(void);
u32 vdc_data_read_low_vdc_a(void);
u32 vdc_data_read_high_vdc_a(void);

void vdc_register_select_vdc_a(u32 value);
void vdc_data_write_low_vdc_a(u32 value);
void vdc_data_write_high_vdc_a(u32 value);

u32 vdc_status_vdc_b(void);
u32 vdc_data_read_low_vdc_b(void);
u32 vdc_data_read_high_vdc_b(void);

void vdc_register_select_vdc_b(u32 value);
void vdc_data_write_low_vdc_b(u32 value);
void vdc_data_write_high_vdc_b(u32 value);

void vdc_register_select(u32 value);
void vdc_data_write_low(u32 value);
void vdc_data_write_high(u32 value);

u32 vpc_read_window_status_low(void);
u32 vpc_read_window_status_high(void);
u32 vpc_read_window1_low(void);
u32 vpc_read_window1_high(void);
u32 vpc_read_window2_low(void);
u32 vpc_read_window2_high(void);

void vpc_write_window_status_low(u32 value);
void vpc_write_window_status_high(u32 value);
void vpc_write_window1_low(u32 value);
void vpc_write_window1_high(u32 value);
void vpc_write_window2_low(u32 value);
void vpc_write_window2_high(u32 value);
void vpc_select(u32 value);

void reset_video();
void initialize_video();

void vce_control_write(u32 value);
void vce_address_write_low(u32 value);
void vce_address_write_high(u32 value);
u32 vce_data_read_low();
u32 vce_data_read_high();
void vce_data_write_low(u32 value);
void vce_data_write_high(u32 value);
void initialize_vce();

void vdc_check_spr_limit(vdc_struct *vdc, u32 line);
void update_status_message();
void update_frame(u32 skip);

void render_bg_lines(u8 *dest, u32 start_y, u32 end_y);

void dump_vram(u32 start, u32 size);
void dump_palette(u32 start, u32 size);
void dump_video_status();

void dump_spr(u32 start, u32 size);
void dump_sprites_per_line(u32 line);

void video_load_savestate(savestate_load_type savestate_file);
void video_store_savestate(savestate_store_type savestate_file);

#ifdef EXTERN_C_END
EXTERN_C_END
#endif

#endif

