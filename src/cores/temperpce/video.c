#include "common.h"
#include "palette.h"

// Put temp debug vars here

#define	RGB(r, g, b)	((((r) >> 3) << 11) | (((g) >> 2) << 5) | ((b) >> 3))

//u64 scanline_cycle_counter;
u64 scanline_cycle_counter;

// ARM versions are using this, but it doesn't really hurt to
// have it here like this
u32 sp_storage;

video_debug_mode_enum video_debug_mode = NO_DEBUG;

#ifdef ARM_ARCH
  #define scanline_buffer_align __attribute__((aligned(32)))
#else
  #define scanline_buffer_align __attribute__((aligned(16)))
#endif

vdc_struct vdc_a;
vdc_struct vdc_b;

vpc_struct vpc;

void update_satb(vdc_struct *vdc);
void vdc_vram_dma_line(vdc_struct *vdc);

static void vdc_update_width(vdc_struct *vdc)
{
  s32 screen_width = (vdc->hdw + 1) * 8;
  s32 scanline_offset = ((vdc->hsw + vdc->hds) * 8) -
   vce.screen_overdraw_offset;
  u32 screen_end;
  s32 hds_cycles = 0;

  if(scanline_offset > (s32)vce.screen_width)
    scanline_offset = vce.screen_width;

  if(scanline_offset < 0)
  {
    if(screen_width < 0)
      screen_width = 0;

    screen_end = screen_width;

    vdc->scroll_shift = -scanline_offset;
    scanline_offset = 0;
  }
  else
  {
    screen_end = screen_width + scanline_offset;
    vdc->scroll_shift = 0;
  }

  if(screen_end >= vce.screen_width)
  {
    screen_width = vce.screen_width - scanline_offset;
  }
  else
  {
    memset(vdc->bg_buffer + 512 + 512 + screen_width, 0,
     vce.screen_width - screen_end);
    memset(vdc->bg_mask_buffer + ((512 + 512 + screen_width) / 32), 0,
     (vce.screen_width - screen_end) / 8);
  }

  vdc->screen_width = screen_width;
  vdc->scanline_offset = scanline_offset;
  vdc->screen_end = screen_end;

  switch(vce.control & 0x3)
  {
    case 0:
      hds_cycles = ((0x27 - vdc->hdw) * 8) * 4;

      if(vdc->hds >= 0x9)
        hds_cycles += ((vdc->hds - 0x8) * 8) * 4;
      else

      if(vdc->hds >= 0x6)
        hds_cycles -= 0x15;
      break;

    case 1:
      hds_cycles = ((0x35 - vdc->hdw) * 8) * 3;
      if(vdc->hds >= 0xC)
        hds_cycles += (((vdc_a.hds - 0xB) * 8) - 0x7) * 3;
      else

      if(vdc->hds >= 0x7)
        hds_cycles -= 0x15;

      if(vdc->hds >= 0x12)
        hds_cycles += ((vdc->hds - 0x11) * 8) * 4;
      break;

    case 2:
      hds_cycles = ((0x4E - vdc->hdw) * 8) * 2;
      if(vdc->hds >= 0x12)
        hds_cycles += ((vdc->hds - 0x11) * 8) * 4;
      else

      if(vdc->hds >= 0x18)
        hds_cycles -= 0x15;
      break;

    case 3:
      hds_cycles = ((0x4E - vdc->hdw) * 8) * 2;
      if(vdc->hds >= 0x12)
        hds_cycles += ((vdc->hds - 0x11) * 8) * 4;
      else

      if(vdc->hds >= 0x18)
        hds_cycles -= 0x15;
      break;
  } 

  if(hds_cycles < 0)
    hds_cycles = 0;

  vdc->hds_cycles = hds_cycles;

  update_satb(vdc);
}

#define vdc_interface_functions_builder(_vdc)                                 \
                                                                              \
u32 vdc_status_##_vdc()                                                       \
{                                                                             \
  memory.io_buffer = _vdc.status;                                             \
  irq.status &= ~IRQ_VDC;                                                     \
  _vdc.status &= ~0x3F;                                                       \
                                                                              \
  return memory.io_buffer;                                                    \
}                                                                             \
                                                                              \
void vdc_register_select_##_vdc(u32 value)                                    \
{                                                                             \
  _vdc.register_select = value & 0x1F;                                        \
}                                                                             \
                                                                              \
u32 vdc_data_read_low_##_vdc()                                                \
{                                                                             \
  return (_vdc.read_latch & 0xFF);                                            \
}                                                                             \
                                                                              \
u32 vdc_data_read_high_##_vdc()                                               \
{                                                                             \
  u32 read_value = _vdc.read_latch >> 8;                                      \
  if(_vdc.register_select == 0x2)                                             \
  {                                                                           \
    _vdc.marr = (_vdc.marr + _vdc.increment_value) & 0xFFFF;                  \
                                                                              \
    if(_vdc.marr < 0x8000)                                                    \
      _vdc.read_latch = _vdc.vram[_vdc.marr];                                 \
  }                                                                           \
                                                                              \
  return read_value;                                                          \
}                                                                             \
                                                                              \
void vdc_data_write_low_##_vdc(u32 value)                                     \
{                                                                             \
  switch(_vdc.register_select)                                                \
  {                                                                           \
    case 0x0:                                                                 \
      _vdc.mawr = (_vdc.mawr & 0xFF00) | value;                               \
      break;                                                                  \
                                                                              \
    case 0x1:                                                                 \
      _vdc.marr = (_vdc.marr & 0xFF00) | value;                               \
      break;                                                                  \
                                                                              \
    case 0x2:                                                                 \
      _vdc.write_latch = value;                                               \
      break;                                                                  \
                                                                              \
    case 0x5:                                                                 \
      /* control register, low byte                                         */\
      /* D7 : 1= Background enable                                          */\
      /* D6 : 1= Sprite enable                                              */\
      /* D5 : External sync (bit 1)                                         */\
      /* D4 : External sync (bit 0)                                         */\
      /* D3 : 1= Enable interrupt for vertical blanking                     */\
      /* D2 : 1= Enable interrupt for raster compare                        */\
      /* D1 : 1= Enable interrupt for sprite overflow                       */\
      /* D0 : 1= Enable interrupt for sprite #0 collision                   */\
      _vdc.cr = (_vdc.cr & 0xFF00) | value;                                   \
      break;                                                                  \
                                                                              \
    case 0x6:                                                                 \
      _vdc.rcr = (_vdc.rcr & 0xFF00) | value;                                 \
      break;                                                                  \
                                                                              \
    case 0x7:                                                                 \
      _vdc.bxr = (_vdc.bxr & 0xFF00) | value;                                 \
      break;                                                                  \
                                                                              \
    case 0x8:                                                                 \
      _vdc.byr = (_vdc.byr & 0xFF00) | value;                                 \
      _vdc.effective_byr = _vdc.byr;                                          \
      break;                                                                  \
                                                                              \
    case 0x9:                                                                 \
      /* mwr                                                                */\
      /* D7 : CG mode                                                       */\
      /* D6 : Virtual screen height                                         */\
      /* D5 : Virtual screen width (bit 1)                                  */\
      /* D4 : Virtual screen width (bit 0)                                  */\
      /* D3 : Sprite dot period (bit 1)                                     */\
      /* D2 : Sprite dot period (bit 0)                                     */\
      /* D1 : VRAM dot width (bit 1)                                        */\
      /* D0 : VRAM dot width (bit 0)                                        */\
      _vdc.latched_mwr = value;                                               \
      break;                                                                  \
                                                                              \
    case 0xA:                                                                 \
      _vdc.hsw = value & 0x1F;                                                \
      vdc_update_width(&_vdc);                                                \
      break;                                                                  \
                                                                              \
    case 0xB:                                                                 \
      _vdc.hdw = value & 0x7F;                                                \
      vdc_update_width(&_vdc);                                                \
      break;                                                                  \
                                                                              \
    case 0xC:                                                                 \
      _vdc.vsw = value & 0x1F;                                                \
      break;                                                                  \
                                                                              \
    case 0xD:                                                                 \
      _vdc.vdw = (_vdc.vdw & 0xFF00) | value;                                 \
      break;                                                                  \
                                                                              \
    case 0xE:                                                                 \
      _vdc.vcr = value;                                                       \
      break;                                                                  \
                                                                              \
    case 0xF:                                                                 \
      _vdc.dcr = value;                                                       \
      break;                                                                  \
                                                                              \
    case 0x10:                                                                \
      _vdc.sour = (_vdc.sour & 0xFF00) | value;                               \
      break;                                                                  \
                                                                              \
    case 0x11:                                                                \
      _vdc.desr = (_vdc.desr & 0xFF00) | value;                               \
      break;                                                                  \
                                                                              \
    case 0x12:                                                                \
      _vdc.lenr = (_vdc.lenr & 0xFF00) | value;                               \
      break;                                                                  \
                                                                              \
    case 0x13:                                                                \
      _vdc.satb = (_vdc.satb & 0xFF00) | value;                               \
      _vdc.satb_dma_trigger = 1;                                              \
      break;                                                                  \
                                                                              \
    case 0x3:                                                                 \
    case 0x4:                                                                 \
      break;                                                                  \
  }                                                                           \
}                                                                             \
                                                                              \
void vdc_data_write_high_##_vdc(u32 value)                                    \
{                                                                             \
  switch(_vdc.register_select)                                                \
  {                                                                           \
    case 0x0:                                                                 \
      _vdc.mawr = (_vdc.mawr & 0xFF) | (value << 8);                          \
      break;                                                                  \
                                                                              \
    case 0x1:                                                                 \
      _vdc.marr = (_vdc.marr & 0xFF) | (value << 8);                          \
      _vdc.read_latch = _vdc.vram[_vdc.marr];                                 \
      break;                                                                  \
                                                                              \
    case 0x2:                                                                 \
    {                                                                         \
      if(_vdc.mawr < 0x8000)                                                  \
      {                                                                       \
        u32 write_value = (value << 8) | _vdc.write_latch;                    \
        _vdc.vram[_vdc.mawr] = write_value;                                   \
        _vdc.dirty_tiles[_vdc.mawr / 16] = 1;                                 \
        _vdc.dirty_patterns[_vdc.mawr / 64] = 1;                              \
                                                                              \
        if(_vdc.satb_dma_irq_lines)                                           \
        {                                                                     \
          cpu.alert = 1;                                                      \
          cpu.vdc_stalled = 1;                                                \
        }                                                                     \
      }                                                                       \
                                                                              \
      _vdc.mawr = ((_vdc.mawr + _vdc.increment_value) & 0xFFFF);              \
                                                                              \
      break;                                                                  \
    }                                                                         \
                                                                              \
    case 0x5:                                                                 \
      /* control register, high byte                                        */\
      /* D15-D13 : Unused                                                   */\
      /* D12 : Increment width select (bit 1)                               */\
      /* D11 : Increment width select (bit 0)                               */\
      /* D10 : 1= DRAM refresh enable                                       */\
      /* D9 : DISP terminal output mode (bit 1)                             */\
      /* D8 : DISP terminal output mode (bit 0)                             */\
      value &= 0x1F;                                                          \
      _vdc.cr = (_vdc.cr & 0xFF) | (value << 8);                              \
      switch(value >> 3)                                                      \
      {                                                                       \
        case 0x0:                                                             \
          _vdc.increment_value = 0x01;                                        \
          break;                                                              \
                                                                              \
        case 0x1:                                                             \
          _vdc.increment_value = 0x20;                                        \
          break;                                                              \
                                                                              \
        case 0x2:                                                             \
          _vdc.increment_value = 0x40;                                        \
          break;                                                              \
                                                                              \
        case 0x3:                                                             \
          _vdc.increment_value = 0x80;                                        \
          break;                                                              \
      }                                                                       \
      break;                                                                  \
                                                                              \
    case 0x6:                                                                 \
      _vdc.rcr = (_vdc.rcr & 0xFF) | ((value & 0x3) << 8);                    \
      break;                                                                  \
                                                                              \
    case 0x7:                                                                 \
      _vdc.bxr = (_vdc.bxr & 0xFF) | ((value & 0x3) << 8);                    \
      break;                                                                  \
                                                                              \
    case 0x8:                                                                 \
    {                                                                         \
      _vdc.byr = (_vdc.byr & 0xFF) | ((value & 0x1) << 8);                    \
      _vdc.effective_byr = _vdc.byr;                                          \
      break;                                                                  \
    }                                                                         \
                                                                              \
    case 0xA:                                                                 \
      _vdc.hds = value & 0x7F;                                                \
      vdc_update_width(&_vdc);                                                \
      break;                                                                  \
                                                                              \
    case 0xB:                                                                 \
      _vdc.hde = value & 0x7F;                                                \
      break;                                                                  \
                                                                              \
    case 0xC:                                                                 \
      _vdc.vds = value;                                                       \
      break;                                                                  \
                                                                              \
    case 0xD:                                                                 \
      _vdc.vdw = (_vdc.vdw & 0xFF) | ((value & 0x1) << 8);                    \
      break;                                                                  \
                                                                              \
    case 0x10:                                                                \
      _vdc.sour = (_vdc.sour & 0xFF) | (value << 8);                          \
      break;                                                                  \
                                                                              \
    case 0x11:                                                                \
      _vdc.desr = (_vdc.desr & 0xFF) | (value << 8);                          \
      break;                                                                  \
                                                                              \
    case 0x12:                                                                \
      _vdc.lenr = (_vdc.lenr & 0xFF) | (value << 8);                          \
      _vdc.vram_dma_trigger = 1;                                              \
                                                                              \
      /* This is a hack.                                                    */\
      if(_vdc.burst_mode && ((_vdc.dcr & 0x02) == 0))                         \
        vdc_vram_dma_line(&_vdc);                                             \
      break;                                                                  \
                                                                              \
    case 0x13:                                                                \
      _vdc.satb = (_vdc.satb & 0xFF) | (value << 8);                          \
      _vdc.satb_dma_trigger = 1;                                              \
      break;                                                                  \
                                                                              \
    default:                                                                  \
      break;                                                                  \
  }                                                                           \
}                                                                             \


#define vdc_select_function_builder(name)                                     \
void name(u32 value)                                                          \
{                                                                             \
  if(vpc.select == VPC_SELECT_VDC_A)                                          \
    name##_vdc_a(value);                                                      \
  else                                                                        \
    name##_vdc_b(value);                                                      \
}                                                                             \

vdc_interface_functions_builder(vdc_a);
vdc_interface_functions_builder(vdc_b);

vdc_select_function_builder(vdc_register_select);
vdc_select_function_builder(vdc_data_write_low);
vdc_select_function_builder(vdc_data_write_high);


void vpc_update_windows(void)
{
  s32 first_point = vpc.window1_value;
  s32 second_point = vpc.window2_value;
  s32 segment_points[3] =
  {
    0, 0, vce.screen_width
  };

  s32 previous_segment_point;
  u32 i;

  u32 segment_statuses[3] =
  {
    // Window 1 + Window 2
    vpc.window_status & 0xF,
    // Window 2
    (vpc.window_status >> 4) & 0xF,
    // No Windows
    vpc.window_status >> 12
  };

  if(first_point > second_point)
  {
    second_point = first_point;
    first_point = vpc.window2_value;
    // Set middle segment to window 1.
    segment_statuses[1] = (vpc.window_status >> 8) & 0xF;
  }

  first_point -= 64 + vce.screen_overdraw_offset;
  second_point -= 64 + vce.screen_overdraw_offset;

  if(first_point < 0)
    first_point = 0;

  if(second_point < 0)
    second_point = 0;

  if(first_point > vce.screen_width)
    first_point = vce.screen_width;

  if(second_point > vce.screen_width)
    second_point = vce.screen_width;

  segment_points[0] = first_point;
  segment_points[1] = second_point;

  previous_segment_point = 0;

  vpc.window_spans = 0;

  for(i = 0; i < 3; i++)
  {
    if(segment_points[i] != previous_segment_point)
    {
      vpc.window_span_lengths[vpc.window_spans] =
       segment_points[i] - previous_segment_point;
      vpc.window_span_status[vpc.window_spans] = segment_statuses[i];

      (vpc.window_spans)++;
      previous_segment_point = segment_points[i];
    }
  }
}


u32 vpc_read_window_status_low(void)
{
  return vpc.window_status & 0xFF;
}

u32 vpc_read_window_status_high(void)
{
  return vpc.window_status >> 8;
}

u32 vpc_read_window1_low(void)
{
  return vpc.window1_value & 0xFF;
}

u32 vpc_read_window1_high(void)
{
  return vpc.window1_value >> 8;
}

u32 vpc_read_window2_low(void)
{
  return vpc.window2_value & 0xFF;
}

u32 vpc_read_window2_high(void)
{
  return vpc.window2_value >> 8;
}


void vpc_write_window_status_low(u32 value)
{
  vpc.window_status = (vpc.window_status & 0xFF00) | value;
  vpc.windows_dirty = 1;
}

void vpc_write_window_status_high(u32 value)
{
  vpc.window_status = (vpc.window_status & 0xFF) | (value << 8);
  vpc.windows_dirty = 1;
}

void vpc_write_window1_low(u32 value)
{
  vpc.window1_value = (vpc.window1_value & 0xFF00) | value;
  vpc.windows_dirty = 1;
}

void vpc_write_window1_high(u32 value)
{
  vpc.window1_value = (vpc.window1_value & 0xFF) | (value << 8);
  vpc.windows_dirty = 1;
}

void vpc_write_window2_low(u32 value)
{
  vpc.window2_value = (vpc.window2_value & 0xFF00) | value;
  vpc.windows_dirty = 1;
}

void vpc_write_window2_high(u32 value)
{
  vpc.window2_value = (vpc.window2_value & 0xFF) | (value << 8);
  vpc.windows_dirty = 1;
}

void vpc_select(u32 value)
{
  vpc.select = value & 0x1;
}


void initialize_vdc(vdc_struct *vdc)
{
  memset(vdc->bg_buffer, 0, sizeof(vdc->bg_buffer));
  memset(vdc->bg_mask_buffer, 0, sizeof(vdc->bg_mask_buffer));
}

void reset_vdc(vdc_struct *vdc)
{
  u32 i;

  vdc->status = 0;
  vdc->register_select = 0;
  vdc->mawr = 0;
  vdc->marr = 0;
  vdc->cr = 0;
  vdc->rcr = 0;
  vdc->mwr = 0;
  vdc->bxr = 0;
  vdc->byr = 0;
  vdc->dcr = 0;
  vdc->sour = 0;
  vdc->desr = 0;
  vdc->lenr = 0;
  vdc->satb = 0;

  vdc->write_latch = 0xFF;
  vdc->read_latch = 0xFF;

  vdc->raster_line = 0;
  vdc->effective_byr = 0;

  vdc->increment_value = 1;

  vdc->satb_dma_trigger = 0;
  vdc->satb_dma_irq_lines = 0;
  vdc->vram_dma_trigger = 0;
  vdc->vram_dma_irq_lines = 0;

  vdc->hsw = 0x1F;
  vdc->hds = 0x00;
  vdc->hdw = 0x7F;
  vdc->hde = 0x00;

  vdc->vsw = 0x02;
  vdc->vds = 0x0F;
  vdc->vdw = 0xFF;
  vdc->vcr = 0xFF;

  vdc->display_counter = 0;
  vdc->burst_mode = 0;

  memset(vdc->dirty_tiles, 0, sizeof(vdc->dirty_tiles));
  memset(vdc->dirty_patterns, 0, sizeof(vdc->dirty_patterns));
  memset(vdc->vram, 0, 64 * 1024);
  memset(vdc->tile_cache, 0, sizeof(vdc->tile_cache));
  memset(vdc->pattern_cache, 0, sizeof(vdc->pattern_cache));
  memset(vdc->tile_cache_transparency_mask, 0,
   sizeof(vdc->tile_cache_transparency_mask));
  memset(vdc->pattern_cache_transparency_mask, 0,
   sizeof(vdc->pattern_cache_transparency_mask));

  memset(vdc->sat, 0xFF, 64 * 8);
  update_satb(vdc);

  for(i = 0; i < 243; i++)
  {
    vdc->sat_cache.lines[i].num_active = 0;
  }

  vdc_update_width(vdc);
}

vce_struct vce;


void initialize_palette_convert()
{
    #define MAKE_COLOR(r,g,b,a) \
      (a << 0) | (r << 24) | (g << 16) | (b << 8)
    #define MAKE_COLOR_5551(r,g,b,a) \
      (a << 0) | (r << 11) | (g << 6) | (b << 1)
  
    s32 color, r, g, b;
    for(color = 0; color < 512; color++)
    {
        r = ((color >> 3) & 0x7) * 255 / 7;
        g = ((color >> 6) & 0x7) * 255 / 7;
        b = (color & 0x7) * 255 / 7;
        palette_convert[color] = MAKE_COLOR(r, g, b, 0x00);

        r = ((color >> 3) & 0x7) * 31 / 7;
        g = ((color >> 6) & 0x7) * 31 / 7;
        b = (color & 0x7) * 31 / 7;
        palette_convert_5551[color] = MAKE_COLOR_5551(r, g, b, 0x01);
    }
  
	/*FILE* fp;
	s32 color, r, g, b;
	s32 y, ry, by;
	double f_y, f_ry, f_by;
	double f_r, f_g, f_b;
  
  for(color = 0; color < 512; color++)
  {
	
    g = ((color >> 6) & 0x7);
    r = ((color >> 3) & 0x7);
    b = (color & 0x7);

    y = (r * 133) + (g * 261) + (b * 49);
    ry = (r * 214) + (g * -180) + (b * -34);
    by = (r * -72) + (g * -142) + (b * 214);

    y += 50;

    if(ry < 0)
      ry -= 50;
    else
      ry += 50;

    if(by < 0)
      by -= 50;
    else
      by += 50;

    y /= 100;
    ry /= 100;
    by /= 100;

    f_y = y / 31.0;
    f_ry = (ry / 15.0) * 0.701 * 0.75;
    f_by = (by / 15.0) * 0.886 * 0.75;

    f_r = f_y + f_ry;
    f_g = f_y - ((0.114 / 0.587) * f_by) - ((0.229 / 0.587) * f_ry);
    f_b = f_y + f_by;

    r = (u32)(floor(f_r * 31.0) + 0.5);
    b = (u32)(floor(f_b * 31.0) + 0.5);

    if(r < 0)
      r = 0;

    if(b < 0)
      b = 0;

    if(r > 31)
      r = 31;

    if(b > 31)
      b = 31;

#ifdef COLOR_RGB_555
    g = (u32)(floor(f_g * 31.0) + 0.5);

    if(g > 31)
      g = 31;
#else
    g = (u32)(floor(f_g * 63.0) + 0.5);

    if(g > 63)
      g = 63;
#endif

    if(g < 0)
      g = 0;

#ifdef COLOR_BGR_565
    palette_convert[color] = (b << 11) | (g << 5) | r;
#endif

#ifdef COLOR_RGB_565
    palette_convert[color] = (r << 11) | (g << 5) | b;
#endif

#ifdef COLOR_RGB_555
    palette_convert[color] = (r << 10) | (g << 5) | b;
#endif
  }*/
}

void vce_control_write(u32 value)
{
  vce.control = value;

  // Pixel clock mode
  switch(value & 0x3)
  {
    case 0:
      vce.screen_width = 256;
      vce.screen_overdraw_offset = 32;
      vce.scanline_cycles = 1364;
      break;

    case 1:
      // This is cut off, but fits best into all target resolutions, so we're
      // going with it for now.
      vce.screen_width = 320;
      vce.screen_overdraw_offset = 64;
      vce.scanline_cycles = 1365;
      break;

    case 2:
      vce.screen_width = 512;
      vce.screen_overdraw_offset = 104;
      vce.scanline_cycles = 1364;
      break;

    case 3:
      vce.screen_width = 512;
      vce.screen_overdraw_offset = 104;
      vce.scanline_cycles = 1364;

      if(value & 0x4)
        vce.scanline_cycles += 2;
      break;
  }

  vce.screen_center_offset = (RESOLUTION_WIDTH - vce.screen_width) / 2;

  vdc_update_width(&vdc_a);

  if(config.sgx_mode)
    vdc_update_width(&vdc_b);

  // Interlace bit determines how many scanlines there are.
  if(value & 0x4)
    vce.num_lines = 263;
  else
    vce.num_lines = 262;
}

void vce_address_write_low(u32 value)
{
  vce.palette_offset = (vce.palette_offset & 0xFF00) | value;
}

void vce_address_write_high(u32 value)
{
  vce.palette_offset = (vce.palette_offset & 0xFF) | ((value & 0x1) << 8);
}

u32 vce_data_read_low()
{
  return vce.palette[vce.palette_offset] & 0xFF;
}

u32 vce_data_read_high()
{
  u32 palette_entry = (vce.palette[vce.palette_offset] >> 8) | 0xFE;
  vce.palette_offset = (vce.palette_offset + 1) & 0x1FF;

  return palette_entry;
}

void vce_data_write_low(u32 value)
{
  u32 palette_entry = vce.palette[vce.palette_offset];
  palette_entry = (palette_entry & 0xFF00) | value;

  if (vce.palette[vce.palette_offset] != palette_entry)
  {
    vce.palette[vce.palette_offset] = palette_entry;

    if(((vce.palette_offset & 0x0F) == 0) && (vce.palette_offset < 0x100))
    {
      if(vce.palette_offset == 0)
      {
        u32 i;
        u32 *palette_ptr = vce.palette_cache +
          (vce.palette_offset & 0x100);
        u16 *palette_5551_ptr = vce.palette_cache_5551 +
          (vce.palette_offset & 0x100);

        palette_entry = palette_convert[palette_entry];
        u16 palette_entry_5551 = palette_convert_5551[palette_entry];

        for(i = 0; i < 16; i++)
        {
          *palette_ptr = palette_entry;
          //*palette_5551_ptr = palette_entry_5551;
          
          palette_ptr += 16;
          //palette_5551_ptr += 16;
        }
      }
    }
    else
    {
      if(vce.palette_offset == 0x100)
        vce.clear_edges = 1;

      vce.palette_cache[vce.palette_offset] = palette_convert[palette_entry];
      vce.palette_cache_5551[vce.palette_offset] = palette_convert_5551[palette_entry];

      update_palette_frame(vce.palette_offset / 16);
    }
   
  }
}

void vce_data_write_high(u32 value)
{
  u32 palette_entry = vce.palette[vce.palette_offset];
  palette_entry = (palette_entry & 0xFF) | ((value & 0x1) << 8);

  if (vce.palette[vce.palette_offset] != palette_entry)
  {
    vce.palette[vce.palette_offset] = palette_entry;

    if(((vce.palette_offset & 0x0F) == 0) && (vce.palette_offset < 0x100))
    {
      if(vce.palette_offset == 0)
      {
        u32 i;
        u32 *palette_ptr = vce.palette_cache +
          (vce.palette_offset & 0x100);
        u16 *palette_5551_ptr = vce.palette_cache_5551 +
          (vce.palette_offset & 0x100);
        
        palette_entry = palette_convert[palette_entry];
        u16 palette_entry_5551 = palette_convert_5551[palette_entry];

        for(i = 0; i < 16; i++)
        {
          *palette_ptr = palette_entry;
          //*palette_5551_ptr = palette_entry_5551;

          palette_ptr += 16;
          //palette_5551_ptr += 16;
        }
      }
    }
    else
    {
      vce.palette_cache[vce.palette_offset] = palette_convert[palette_entry];
      vce.palette_cache_5551[vce.palette_offset] = palette_convert_5551[palette_entry];
    
      update_palette_frame(vce.palette_offset / 16);
    }
  }

  vce.palette_offset = (vce.palette_offset + 1) & 0x1FF;
}

void reset_vce()
{
  u32 i;

  vce.palette_offset = 0;
  vce.num_lines = 262;

  vce.frame_counter = 0;

  vce_control_write(0);

  memset(vce.palette, 0, 512 * 4);
  memset(vce.palette_cache, 0, 512 * 4);
  for (int i = 0; i < 512; i++)
    vce.palette_cache_5551[i] = 0x1;  // black with alpha = 1;

  for(i = 0; i < 242; i++)
  {
    vce.pixels_drawn[i] = RESOLUTION_WIDTH;
  }

  vce.frames_rendered = 0;
}

void reset_vpc()
{
  vpc.window_status = 0x1111;
  vpc.window1_value = 0;
  vpc.window2_value = 0;
  vpc.select = 0;

  vpc.windows_dirty = 1;
}

void initialize_vce()
{
}

void initialize_vpc()
{
}


void reset_video()
{
  reset_vdc(&vdc_a);
  if(config.sgx_mode)
  {
    reset_vdc(&vdc_b);
    reset_vpc();
  }

  reset_vce();
}


void initialize_video()
{
  initialize_palette_convert();
  initialize_vdc(&vdc_a);
  initialize_vdc(&vdc_b);
  initialize_vce();
  initialize_vpc();

  set_screen_resolution(RESOLUTION_WIDTH, RESOLUTION_HEIGHT);
  clear_screen();
}

void cache_tile(vdc_struct *vdc, u32 tile_number)
{
  // Tiles consist of 2 16byte half-tiles contiguously in VRAM.
  // Each half-tile consists of 2 planes interleaved in 8 rows.
  // The tile cache consists of 64 8-bit pixels consecutively.

  u16 *vram_offset = vdc->vram + (tile_number * 16);
  u8 *tile_cache_offset = vdc->tile_cache + (tile_number * 64);
  u8 *tile_cache_transparency_mask_offset =
   vdc->tile_cache_transparency_mask + (tile_number * 8);
  u32 transparency_mask;
  u32 plane_row_a, plane_row_b;
  u32 x, y;

  for(y = 0; y < 8; y++)
  {
    plane_row_a = vram_offset[y];
    plane_row_b = vram_offset[y + 8];
    // 16bits -> 8 pixels (32bits packed 4bpp)
    for(x = 0; x < 8; x++)
    {
      tile_cache_offset[x] =
       ((plane_row_a >> (7 - x)) & 0x1) |
       (((plane_row_a >> ((7 - x) + 8)) & 0x1) << 1) |
       (((plane_row_b >> (7 - x)) & 0x1) << 2) |
       (((plane_row_b >> ((7 - x) + 8)) & 0x1) << 3);
    }
    transparency_mask =
     plane_row_a | (plane_row_a >> 8) | plane_row_b | (plane_row_b >> 8);

    // The bits need to be reversed in this. On archs with a bit-reversal
    // instruction it'll possibly be good to use it.
    transparency_mask =
     ((transparency_mask & 0x55) << 1) | ((transparency_mask & 0xAA) >> 1);
    transparency_mask =
     ((transparency_mask & 0x33) << 2) | ((transparency_mask & 0xCC) >> 2);
    transparency_mask =
     ((transparency_mask & 0x0F) << 4) | ((transparency_mask & 0xF0) >> 4);

    *tile_cache_transparency_mask_offset = transparency_mask;

    tile_cache_offset += 8;
    tile_cache_transparency_mask_offset++;
  }
}

void cache_pattern(vdc_struct *vdc, u32 pattern_number)
{
  // Patterns consist of 4 32byte quarter-patterns contiguously in VRAM.
  // Each quarter-tile consists of 1 plane in 16 rows.
  u16 *vram_offset = vdc->vram + (pattern_number * 64);
  u8 *pattern_cache_offset = vdc->pattern_cache + (pattern_number * 256);
  u16 *pattern_cache_transparency_mask_offset =
   vdc->pattern_cache_transparency_mask + ((pattern_number * 16) * 2);
  u32 transparency_mask;
  u16 plane_row_a, plane_row_b, plane_row_c, plane_row_d;
  u32 x, y;

  for(y = 0; y < 16; y++)
  {
    plane_row_a = vram_offset[y];
    plane_row_b = vram_offset[y + 16];
    plane_row_c = vram_offset[y + 32];
    plane_row_d = vram_offset[y + 48];

    // 16bits -> 16 pixels (32bits packed 4bpp)
    for(x = 0; x < 16; x++)
    {
      pattern_cache_offset[x] = 
       ((plane_row_a >> (15 - x)) & 0x1) |
       (((plane_row_b >> (15 - x)) & 0x1) << 1) |
       (((plane_row_c >> (15 - x)) & 0x1) << 2) |
       (((plane_row_d >> (15 - x)) & 0x1) << 3);
    }

    transparency_mask =
     plane_row_a | plane_row_b | plane_row_c | plane_row_d;

    // Store the bit-reversed mask for horizontal flips
    pattern_cache_transparency_mask_offset[1] = transparency_mask;

    // The bits need to be reversed in this. On archs with a bit-reversal
    // instruction it'll possibly be good to use it.
    transparency_mask =
     ((transparency_mask & 0x5555) << 1) | ((transparency_mask & 0xAAAA) >> 1);
    transparency_mask =
     ((transparency_mask & 0x3333) << 2) | ((transparency_mask & 0xCCCC) >> 2);
    transparency_mask =
     ((transparency_mask & 0x0F0F) << 4) | ((transparency_mask & 0xF0F0) >> 4);
    transparency_mask =
     ((transparency_mask & 0x00FF) << 8) | ((transparency_mask & 0xFF00) >> 8);

    pattern_cache_transparency_mask_offset[0] = transparency_mask;

    pattern_cache_offset += 16;
    pattern_cache_transparency_mask_offset += 2;
  }
}


#define rgb16(r, g, b)                                                        \
  ((r << 11) | (g << 5) | b)                                                  \

#define grey16(c)                                                             \
  rgb16(c, (c * 2), c)                                                        \

u16 debug_palette[16] =
{
  grey16(0), grey16(1), grey16(2), grey16(3), grey16(4), grey16(5),
  grey16(6), grey16(7), grey16(8), grey16(9), grey16(10), grey16(11),
  grey16(12), grey16(13), grey16(14), grey16(15),
};

void display_tileset(u32 palette_number)
{
/*  u32 tile_number;
  u32 x, y;
  u32 x2, y2;
  u16 *screen_ptr;
  u32 screen_pitch;
  u32 *tile_ptr;
  u32 tile_row;

  set_screen_resolution(512, 256);
  screen_ptr = get_screen_ptr();
  screen_pitch = get_screen_pitch();

  palette_number <<= 4;

  for(y = 0, tile_number = 0; y < 32; y++)
  {
    for(x = 0; x < 64; x++, tile_number++)
    {
      if(vdc.dirty_tiles[tile_number])
      {
        cache_tile(tile_number);
        vdc.dirty_tiles[tile_number] = 0;
      }
      tile_ptr = vdc.tile_cache + (tile_number * 8);
      for(y2 = 0; y2 < 8; y2++)
      {
        tile_row = tile_ptr[y2];
        for(x2 = 0; x2 < 8; x2++)
        {
          screen_ptr[x2] =
           debug_palette[tile_row & 0xF];
           //vce.palette_cache[palette_number | (tile_row & 0xF)];
          tile_row >>= 4;
        }
        screen_ptr += screen_pitch;
      }
      screen_ptr -= (screen_pitch * 8);
      screen_ptr += 8;
    }
    screen_ptr -= (8 * 64);
    screen_ptr += (screen_pitch * 8);
  } */
}

void display_tilemap(s32 palette_number_override)
{
/*  u32 tile_number, tile_offset;
  u32 palette_number = palette_number_override << 4;
  u32 bg_width = (((vdc.mwr >> 4) & 0x3) + 1) << 5;
  u32 bg_height = (((vdc.mwr >> 6) & 0x1) + 1) << 5;

  u32 x, y;
  u32 x2, y2;
  u16 *screen_ptr;
  u32 screen_pitch;
  u32 *tile_ptr;
  u32 tile_row;

  set_screen_resolution(bg_width * 8, bg_height * 8);
  screen_ptr = get_screen_ptr();
  screen_pitch = get_screen_pitch();

  for(y = 0, tile_offset = 0; y < bg_height; y++)
  {
    for(x = 0; x < bg_width; x++, tile_offset++)
    {
      tile_number = vdc.vram[tile_offset] & 0x7FF;

      if(palette_number_override == -1)
        palette_number = (vdc.vram[tile_offset] >> 12) << 4;

      if(vdc.dirty_tiles[tile_number])
      {
        cache_tile(tile_number);
        vdc.dirty_tiles[tile_number] = 0;
      }
      tile_ptr = vdc.tile_cache + (tile_number * 8);
      for(y2 = 0; y2 < 8; y2++)
      {
        tile_row = tile_ptr[y2];
        for(x2 = 0; x2 < 8; x2++)
        {
          screen_ptr[x2] =
           vce.palette_cache[palette_number | (tile_row & 0xF)];
          tile_row >>= 4;
        }
        screen_ptr += screen_pitch;
      }
      screen_ptr -= (screen_pitch * 8);
      screen_ptr += 8;
    }
    screen_ptr -= (8 * bg_width);
    screen_ptr += (screen_pitch * 8);
  } */
}


void display_patternset(u32 palette_number)
{
/*  u32 pattern_number;
  u32 x, y;
  u32 x2, y2;
  u16 *screen_ptr;
  u32 screen_pitch;
  u32 *pattern_ptr;
  u32 pattern_half_row;

  set_screen_resolution(512, 256);
  screen_ptr = get_screen_ptr();
  screen_pitch = get_screen_pitch();

  palette_number <<= 4;

  for(y = 0, pattern_number = 0; y < 16; y++)
  {
    for(x = 0; x < 32; x++, pattern_number++)
    {
      if(vdc.dirty_patterns[pattern_number])
      {
        cache_pattern(pattern_number);
        vdc.dirty_patterns[pattern_number] = 0;
      }
      pattern_ptr = vdc.pattern_cache + (pattern_number * 32);
      for(y2 = 0; y2 < 16; y2++)
      {
        pattern_half_row = pattern_ptr[y2 * 2];
        for(x2 = 0; x2 < 8; x2++)
        {
          screen_ptr[x2] =
           vce.palette_cache[256 + (palette_number |
            (pattern_half_row & 0xF))];
          pattern_half_row >>= 4;
        }

        pattern_half_row = pattern_ptr[(y2 * 2) + 1];
        for(x2 = 0; x2 < 8; x2++)
        {
          screen_ptr[x2 + 8] =
           vce.palette_cache[256 + (palette_number |
           (pattern_half_row & 0xF))];
          pattern_half_row >>= 4;
        }

        screen_ptr += screen_pitch;
      }
      screen_ptr -= (screen_pitch * 16);
      screen_ptr += 16;
    }
    screen_ptr -= (16 * 32);
    screen_ptr += (screen_pitch * 16);
  } */
}

//#ifdef ARM_ARCH

//#else

#if 1

u32 *render_bg_line(vdc_struct *vdc, u8 **bg_buffer_result)
{
  u32 *bg_buffer = (u32 *)(vdc->bg_buffer + 512 + 512);
  u32 *dest = (u32 *)bg_buffer;
  u32 bg_width = (vdc->mwr >> 4) & 0x3;
  u32 bg_height = (vdc->mwr >> 6) & 0x1;

  // Include one extra to account for shifting. It'll be masked off again if
  // necessary.
  u32 screen_width = vdc->screen_width;
  u32 screen_width_tiles = screen_width / 8;
  u32 screen_width_mask_blocks;

  u8 *transparency_mask_dest =
   (u8 *)(vdc->bg_mask_buffer + ((512 + 512) / 32));
  u32 *transparency_mask_blocks = (u32 *)transparency_mask_dest;
  u32 *transparency_mask_buffer_result;

  u32 y_tile_offset = vdc->effective_byr / 8;
  u32 y_pixel_offset = vdc->effective_byr % 8;
  u16 *bat_offset;
  u8 *tile_base = vdc->tile_cache + (y_pixel_offset * 8);
  u8 *transparency_mask_base =
   vdc->tile_cache_transparency_mask + y_pixel_offset;

  u32 x_scroll = vdc->bxr + vdc->scroll_shift;
  u32 x_tile_offset, x_pixel_offset;
  // Scanline offset is the amount into the scanline the active
  // display starts at.
  s32 scanline_offset = vdc->scanline_offset;

  x_tile_offset = x_scroll / 8;
  x_pixel_offset = x_scroll % 8;

  scanline_offset -= x_pixel_offset;
  *bg_buffer_result = (u8 *)bg_buffer - scanline_offset;

  if(x_pixel_offset)
    screen_width_tiles++;

  screen_width_mask_blocks = (screen_width_tiles + 3) / 4;

  if(bg_width == 2)
    bg_width = 3;

  bg_width = (bg_width + 1) * 32;
  bg_height = ((bg_height + 1) * 32) - 1;
                                     
  y_tile_offset &= bg_height;

  bat_offset = vdc->vram + (y_tile_offset * bg_width);
  bg_width--;

  x_tile_offset &= bg_width;

  while(screen_width_tiles)
  {
    u32 *tile_ptr;
    u32 bat_entry = bat_offset[x_tile_offset];
    u32 tile_number = bat_entry & 0xFFF;
    u32 current_palette = (bat_entry >> 12) << 4;
 
    current_palette = current_palette | (current_palette << 8);
    current_palette = current_palette | (current_palette << 16);

    if(vdc->dirty_tiles[tile_number])
    {
      cache_tile(vdc, tile_number);
      vdc->dirty_tiles[tile_number] = 0;
    }

    tile_ptr = (u32 *)(tile_base + (tile_number * 64));

    dest[0] = tile_ptr[0] | current_palette;
    dest[1] = tile_ptr[1] | current_palette;

    *transparency_mask_dest = transparency_mask_base[tile_number * 8];

    x_tile_offset = (x_tile_offset + 1) & bg_width;
    dest += 2;
    transparency_mask_dest++;

    screen_width_tiles--;
  }

  // Clamp off edges if necessary, by clearing at least 8 pixels around them.
  if(scanline_offset > 0)
  {
    u32 left_pixel_offset = x_pixel_offset;
    u32 *bg_buffer_clear = bg_buffer;

    if(left_pixel_offset > 3)
    {
      *bg_buffer_clear = 0;
      bg_buffer_clear++;
      left_pixel_offset -= 4;
    }

    *bg_buffer_clear &= 0xFFFFFFFF << (left_pixel_offset * 8);
  }

  if(vdc->screen_end < vce.screen_width)
  {
    u32 *bg_buffer_clear = bg_buffer + (screen_width / 4);

    if(x_pixel_offset >= 4)
    {
      bg_buffer_clear++;
      x_pixel_offset -= 4;
    }

    bg_buffer_clear[1] = 0;
    *bg_buffer_clear &= ~(0xFFFFFFFF << (x_pixel_offset * 8));
  }

  // Each transparency mask byte has:
  // b7 b6 b5 b4 b3 b2 b1 b0
  // msb                 lsb

  // When loading 4 at a time we get, on little endian:
  // b31 b30 b29 b28 b27 ... b5 b4 b3 b2 b1 b0
  // So it's safe to shift through like this.

  // We shift the transparency mask over like a giant shift register.
  // What we want is to shift it to the left in order to undo scanline offset,
  // but shift it to the right to undo tile scrolling. The current
  // scanline_offset value has the two merged, meaning shift left if it's
  // positive, right if it's negative.

  if(scanline_offset < 0)
  {
    u32 tile_mask_shift = -scanline_offset;
    transparency_mask_buffer_result = transparency_mask_blocks;

    while(screen_width_mask_blocks)
    {
      u32 transparency_mask =
       transparency_mask_blocks[0] >> tile_mask_shift;
      transparency_mask |=
       transparency_mask_blocks[1] << (32 - tile_mask_shift);
      
      *transparency_mask_blocks = transparency_mask;
  
      transparency_mask_blocks++;
      screen_width_mask_blocks--;
    }
  }
  else
  {
    u32 tile_mask_shift = (u32)scanline_offset % 32;
    transparency_mask_buffer_result =
     transparency_mask_blocks - ((u32)scanline_offset / 32);

    if(tile_mask_shift)
    {
      transparency_mask_blocks += screen_width_mask_blocks;
      screen_width_mask_blocks++;
      while(screen_width_mask_blocks)
      {
        u32 transparency_mask =
         transparency_mask_blocks[0] << tile_mask_shift;
        transparency_mask |=
         transparency_mask_blocks[-1] >> (32 - tile_mask_shift);
      
        *transparency_mask_blocks = transparency_mask;
  
        transparency_mask_blocks--;
        screen_width_mask_blocks--;
      }
    }
  }

  return transparency_mask_buffer_result;
}

#else

u32 *render_bg_line(vdc_struct *vdc, u8 **bg_buffer_result);

#endif

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
  u32 i, i2;                                                                 \
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

//#endif

void vdc_check_spr_limit(vdc_struct *vdc, u32 line)
{
  //printf ("spr-limit: %d\n", line);
  if((vdc->cr & 0x2) && (vdc->sat_cache.lines[line].overflow))
  {
    vdc->status |= VDC_STATUS_OVERFLOW_IRQ;
    raise_interrupt(IRQ_VDC);
  }
}

void vdc_render_line(vdc_struct *vdc, u8 **bg_buffer, u32 **bg_mask_buffer,
 u8 **obj_buffer, u32 *obj_low_mask_buffer, u32 *obj_high_mask_buffer)
{
  u32 i;

  if((vdc->display_counter >= vdc->start_line) &&
   (vdc->display_counter < vdc->vblank_line) &&
   (vdc->burst_mode == 0))
  {
    u32 screen_line = vdc->raster_line;

    // Render the BG
    if((vdc->cr & 0x80) && (config.bg_off == 0))
      *bg_mask_buffer = render_bg_line(vdc, bg_buffer);
    else
      *bg_mask_buffer = render_bg_line_blank(vdc, bg_buffer);

    for(i = 0; i < vce.screen_width / 32; i++)
    {
      obj_low_mask_buffer[i] = 0;
      obj_high_mask_buffer[i] = 0;
    }
                                                
    if((vdc->cr & 0x40) && (config.spr_off == 0))
    {
      if((vdc->cr & 0x1) &&
       (vdc->sat_cache.lines[screen_line].flags & LINE_CACHE_FLAGS_SPR0))
      {
        vdc_check_spr_limit(vdc, screen_line);
        if(render_spr_line_check_collision(vdc, *obj_buffer,
         obj_low_mask_buffer, obj_high_mask_buffer, screen_line))
        {
          vdc->status |= VDC_STATUS_SPRITE_COLLISION_IRQ;
          raise_interrupt(IRQ_VDC);
        }
      }
      else
      {
        // Render sprites
        vdc_check_spr_limit(vdc, screen_line);
        render_spr_line(vdc, *obj_buffer, obj_low_mask_buffer,
         obj_high_mask_buffer, screen_line);
      }
    }
  }
  else
  {
    // Line is overscan, which is like OBJ full with the buffer full of
    // color 0. The beginning of the BG buffer will be full of this.
    *obj_buffer = vdc->bg_buffer;
    *bg_buffer = vdc->bg_buffer;
    *bg_mask_buffer = vdc->bg_mask_buffer;
    for(i = 0; i < vce.screen_width / 32; i++)
    {
      obj_high_mask_buffer[i] = 0xFFFFFFFF;
    }
  }
}
 

void render_line(void)
{
  if((vce.frame_counter >= 14) &&
   (vce.frame_counter < (14 + RESOLUTION_HEIGHT)))
  {
    u32 scanline_line = vce.frame_counter - 14;

    // Pad by 32 on either side to allow rendering off the edges. In reality,
    // only 16 pixels are needed, but 32 are given so that everything stays aligned
    // with the 32 bit masks.
    u8 *bg_buffer;
    u8 _obj_buffer[512 + 64] scanline_buffer_align;
    u8 *obj_buffer = _obj_buffer + 32;

    u32 *bg_mask_buffer;
    u32 obj_low_mask_buffer[(512 + 64) / 32];
    u32 obj_high_mask_buffer[(512 + 64) / 32];

    u32 *dest = get_screen_ptr();

    dest += scanline_line * get_screen_pitch();

    vdc_render_line(&vdc_a, &bg_buffer, &bg_mask_buffer,
     &obj_buffer, obj_low_mask_buffer + 1, obj_high_mask_buffer + 1);
     //printf ("%d %d %d start = %d\n", vce.frame_counter, vdc_a.display_counter, vdc_a.raster_line, vdc_a.start_line );

    if(config.scale_width)
    {
      switch(vce.screen_width)
      {
        case 256:
          render_line_expand_scale_4_to_5(dest, bg_buffer, obj_buffer,
           bg_mask_buffer, obj_low_mask_buffer + 1, obj_high_mask_buffer + 1);
          break;

        case 320:
          render_line_expand_no_scale(dest, bg_buffer, obj_buffer,
           bg_mask_buffer, obj_low_mask_buffer + 1, obj_high_mask_buffer + 1);
          break;

        case 512:
          render_line_expand_scale_8_to_5(dest, bg_buffer, obj_buffer,
           bg_mask_buffer, obj_low_mask_buffer + 1, obj_high_mask_buffer + 1);
          break;
      }
      vce.pixels_drawn[scanline_line] = RESOLUTION_WIDTH;
    }
    else
    {
      if(vce.screen_width < vce.pixels_drawn[scanline_line])
      {
        clear_line_edges(scanline_line, 0, vce.screen_center_offset,
         vce.screen_width);
      }
      vce.pixels_drawn[scanline_line] = vce.screen_width;
      dest += vce.screen_center_offset;

      render_line_expand_no_scale(dest, bg_buffer, obj_buffer,
       bg_mask_buffer, obj_low_mask_buffer + 1, obj_high_mask_buffer + 1);
    }
  }
}


void render_line_sgx(void)
{
  if(vpc.windows_dirty)
  {
    vpc_update_windows();
    vpc.windows_dirty = 0;
  }

  if((vce.frame_counter >= 14) &&
   (vce.frame_counter < (14 + RESOLUTION_HEIGHT)))
  {
    u32 scanline_line = vce.frame_counter - 14;

    // Pad by 32 on either side to allow rendering off the edges. In reality,
    // only 16 pixels are needed, but 32 are given so that everything stays aligned
    // with the 32 bit masks.
    u8 *bg_buffer_a;
    u8 *bg_buffer_b;
    u8 _obj_buffer_a[512 + 64] scanline_buffer_align;
    u8 _obj_buffer_b[512 + 64] scanline_buffer_align;
    u8 *obj_buffer_a = _obj_buffer_a + 32;
    u8 *obj_buffer_b = _obj_buffer_b + 32;

    u32 *bg_mask_buffer_a;
    u32 *bg_mask_buffer_b;

    u32 obj_low_mask_buffer_a[(512 + 64) / 32];
    u32 obj_high_mask_buffer_a[(512 + 64) / 32];
    u32 obj_low_mask_buffer_b[(512 + 64) / 32];
    u32 obj_high_mask_buffer_b[(512 + 64) / 32];

    u32 *dest = get_screen_ptr();

    dest += scanline_line * get_screen_pitch();

    vdc_render_line(&vdc_a, &bg_buffer_a, &bg_mask_buffer_a,
     &obj_buffer_a, obj_low_mask_buffer_a + 1, obj_high_mask_buffer_a + 1);
    vdc_render_line(&vdc_b, &bg_buffer_b, &bg_mask_buffer_b,
     &obj_buffer_b, obj_low_mask_buffer_b + 1, obj_high_mask_buffer_b + 1);

    if(config.scale_width)
    {
      switch(vce.screen_width)
      {
        case 256:
          render_line_expand_sgx_scale_4_to_5(dest, bg_buffer_a, obj_buffer_a,
           bg_mask_buffer_a, obj_low_mask_buffer_a + 1,
           obj_high_mask_buffer_a + 1, bg_buffer_b, obj_buffer_b,
           bg_mask_buffer_b, obj_low_mask_buffer_b + 1,
           obj_high_mask_buffer_b + 1);
          break;

        case 320:
          render_line_expand_sgx_no_scale(dest, bg_buffer_a, obj_buffer_a,
           bg_mask_buffer_a, obj_low_mask_buffer_a + 1,
           obj_high_mask_buffer_a + 1, bg_buffer_b, obj_buffer_b,
           bg_mask_buffer_b, obj_low_mask_buffer_b + 1,
           obj_high_mask_buffer_b + 1);
          break;

        case 512:
          render_line_expand_sgx_scale_8_to_5(dest, bg_buffer_a, obj_buffer_a,
           bg_mask_buffer_a, obj_low_mask_buffer_a + 1,
           obj_high_mask_buffer_a + 1, bg_buffer_b, obj_buffer_b,
           bg_mask_buffer_b, obj_low_mask_buffer_b + 1,
           obj_high_mask_buffer_b + 1);
          break;
      }
      vce.pixels_drawn[scanline_line] = RESOLUTION_WIDTH;
    }
    else
    {
      if(vce.screen_width < vce.pixels_drawn[scanline_line])
      {
        clear_line_edges(scanline_line, 0,
         vce.screen_center_offset, vce.screen_width);
      }
      vce.pixels_drawn[scanline_line] = vce.screen_width;
      dest += vce.screen_center_offset;

      render_line_expand_sgx_no_scale(dest, bg_buffer_a, obj_buffer_a,
       bg_mask_buffer_a, obj_low_mask_buffer_a + 1,
       obj_high_mask_buffer_a + 1, bg_buffer_b, obj_buffer_b,
       bg_mask_buffer_b, obj_low_mask_buffer_b + 1,
       obj_high_mask_buffer_b + 1);
    }
  }
}


#define update_satb_add_entry_limit(line, offset, _x, _attributes)            \
{                                                                             \
  sat_cache_line_struct *sat_cache_line = &(sat_cache->lines[line]);          \
  if(sat_cache_line->num_present < 16)                                        \
  {                                                                           \
    line_entry = &(sat_cache_line->entries[sat_cache_line->num_active]);      \
    line_entry->x = _x;                                                       \
    line_entry->attributes = _attributes;                                     \
    line_entry->pattern_line_offset = (offset);                               \
    (sat_cache_line->num_active)++;                                           \
    (sat_cache_line->num_present)++;                                          \
    sat_cache_line->flags |= flags;                                           \
  }                                                                           \
  else                                                                        \
  {                                                                           \
    sat_cache_line->overflow = 1;                                             \
  }                                                                           \
}                                                                             \


#define _update_satb_add_entry_wide_limit(line, offset, offset_b, _x)         \
{                                                                             \
  sat_cache_line_struct *sat_cache_line = &(sat_cache->lines[line]);          \
  if(sat_cache_line->num_present < 15)                                        \
  {                                                                           \
    line_entry = &(sat_cache_line->entries[sat_cache_line->num_active]);      \
    line_entry->x = _x;                                                       \
    line_entry->attributes = attributes_wide;                                 \
    line_entry->pattern_line_offset = (offset);                               \
    (sat_cache_line->num_active)++;                                           \
    sat_cache_line->num_present += 2;                                         \
    sat_cache_line->flags |= flags;                                           \
  }                                                                           \
  else                                                                        \
  {                                                                           \
    if(sat_cache_line->num_present < 16)                                      \
    {                                                                         \
      line_entry = &(sat_cache_line->entries[sat_cache_line->num_active]);    \
      line_entry->x = _x;                                                     \
      line_entry->attributes = attributes & ~SPRITE_ATTRIBUTE_WIDE;           \
      line_entry->pattern_line_offset = (offset);                             \
      (sat_cache_line->num_active)++;                                         \
      (sat_cache_line->num_present)++;                                        \
      sat_cache_line->flags |= flags;                                         \
    }                                                                         \
    sat_cache_line->overflow = 1;                                             \
  }                                                                           \
}                                                                             \


#define update_satb_add_offscreen_limit()                                     \
  if(sat_cache->lines[i2].num_present < 16)                                   \
    sat_cache->lines[i2].num_present++;                                       \
  else                                                                        \
    sat_cache->lines[i2].overflow = 1                                         \


#define update_satb_add_entry_no_limit(line, offset, _x, _attributes)         \
{                                                                             \
  sat_cache_line_struct *sat_cache_line = &(sat_cache->lines[line]);          \
  line_entry = &(sat_cache_line->entries[sat_cache_line->num_active]);        \
  line_entry->x = _x;                                                         \
  line_entry->attributes = _attributes;                                       \
  line_entry->pattern_line_offset = (offset);                                 \
  (sat_cache_line->num_active)++;                                             \
  (sat_cache_line->num_present)++;                                            \
  sat_cache_line->flags |= flags;                                             \
}                                                                             \


#define _update_satb_add_entry_wide_no_limit(line, offset, offset_b, _x)      \
{                                                                             \
  sat_cache_line_struct *sat_cache_line = &(sat_cache->lines[line]);          \
  line_entry = &(sat_cache_line->entries[sat_cache_line->num_active]);        \
  line_entry->x = _x;                                                         \
  line_entry->attributes = attributes_wide;                                   \
  line_entry->pattern_line_offset = (offset);                                 \
  (sat_cache_line->num_active)++;                                             \
  sat_cache_line->flags |= flags;                                             \
}                                                                             \


#define update_satb_add_offscreen_no_limit()                                  \


#define pattern_offset_calculate_normal()                                     \
  pattern_line_offset_sub =                                                   \
   pattern_line_offset + ((i2 / 16) * 32) + (i2 % 16)                         \

#define pattern_offset_calculate_flip()                                       \
  pattern_line_offset_sub = pattern_line_offset +                             \
   (((height - i2 - 1) / 16) * 32) + ((height - i2 - 1) % 16)                 \


#define update_satb_fill_sprite(direction, limit_check)                       \
{                                                                             \
  u32 pattern_line_offset_sub;                                                \
  u32 iterations = height;                                                    \
  i2 = 0;                                                                     \
                                                                              \
  if(y < 0)                                                                   \
    i2 -= y;                                                                  \
                                                                              \
  if((y + height) > 263)                                                      \
    iterations = 263 - y;                                                     \
                                                                              \
  for(; i2 < iterations; i2++)                                                \
  {                                                                           \
    pattern_offset_calculate_##direction();                                   \
    update_satb_add_entry_##limit_check(i2 + y, pattern_line_offset_sub, x,   \
     attributes);                                                             \
  }                                                                           \
}                                                                             \

#define update_satb_add_entry_half(offset, x_off, _attributes, limit_check)   \
  update_satb_add_entry_##limit_check(i2 + y,                                 \
   pattern_line_offset_sub + offset, x + x_off, _attributes);                 \
  sat_cache->lines[i2 + y].num_present++                                      \

#define update_satb_add_entry_wide(pattern_line_offset, x_offset, limit_check)\
  _update_satb_add_entry_wide_##limit_check(i2 + y, pattern_line_offset_sub,  \
   pattern_line_offset, x + x_offset)                                         \

#define update_satb_add_entry_double_normal_view_yes_no(limit_check)          \
  update_satb_add_entry_half(0, 0, attributes, limit_check)                   \

#define update_satb_add_entry_double_normal_view_no_yes(limit_check)          \
  update_satb_add_entry_half(16, 16, attributes_b, limit_check)               \

#define update_satb_add_entry_double_normal_view_yes_yes(limit_check)         \
  update_satb_add_entry_wide(0, 0, limit_check)                               \


#define update_satb_add_entry_double_flip_view_yes_no(limit_check)            \
  update_satb_add_entry_half(16, 0, attributes, limit_check)                  \

#define update_satb_add_entry_double_flip_view_no_yes(limit_check)            \
  update_satb_add_entry_half(0, 16, attributes_b, limit_check)                \

#define update_satb_add_entry_double_flip_view_yes_yes(limit_check)           \
  update_satb_add_entry_wide(16, 0, limit_check)                              \


#define update_satb_add_entry_double_normal(l_on, r_on, limit_check)          \
  update_satb_add_entry_double_normal_view_##l_on##_##r_on(limit_check)       \

#define update_satb_add_entry_double_flip(l_on, r_on, limit_check)            \
  update_satb_add_entry_double_flip_view_##l_on##_##r_on(limit_check)         \


#define update_satb_fill_sprite_double(dir, hflip, l_on, r_on, limit_check)   \
{                                                                             \
  u32 pattern_line_offset_sub;                                                \
  u32 iterations = height;                                                    \
  i2 = 0;                                                                     \
                                                                              \
  if(y < 0)                                                                   \
    i2 -= y;                                                                  \
                                                                              \
  if((y + height) > 263)                                                      \
    iterations = 263 - y;                                                     \
                                                                              \
  for(; i2 < iterations; i2++)                                                \
  {                                                                           \
    pattern_offset_calculate_##dir();                                         \
    update_satb_add_entry_double_##hflip(l_on, r_on, limit_check);            \
  }                                                                           \
}                                                                             \

#define update_satb_fill_sprite_double_option(l_on, r_on, limit_check)        \
  if(attributes & SPRITE_ATTRIBUTE_VFLIP)                                     \
  {                                                                           \
    if(attributes & SPRITE_ATTRIBUTE_HFLIP)                                   \
    {                                                                         \
      update_satb_fill_sprite_double(flip, flip, l_on, r_on, limit_check);    \
    }                                                                         \
    else                                                                      \
    {                                                                         \
      update_satb_fill_sprite_double(flip, normal, l_on, r_on, limit_check);  \
    }                                                                         \
  }                                                                           \
  else                                                                        \
  {                                                                           \
    if(attributes & SPRITE_ATTRIBUTE_HFLIP)                                   \
    {                                                                         \
      update_satb_fill_sprite_double(normal, flip, l_on, r_on, limit_check);  \
    }                                                                         \
    else                                                                      \
    {                                                                         \
      update_satb_fill_sprite_double(normal, normal, l_on, r_on, limit_check);\
    }                                                                         \
  }                                                                           \

#ifdef DEBUGGER_ON

#define mask_sprite()                                                         \
  if(debug.sprite_mask & (1ULL << i))                                         \
    continue                                                                  \

#else

#define mask_sprite()                                                         \

#endif

#define update_satb_maker(limit_check)                                        \
void update_satb_##limit_check(vdc_struct *vdc)                               \
{                                                                             \
  u16 *satb_location = vdc->sat;                                              \
  u16 *current_sprite = satb_location;                                        \
  s32 x, y, width, height;                                                    \
  sat_cache_line_entry_struct *line_entry;                                    \
  u32 pattern_line_offset;                                                    \
  u32 pattern_number;                                                         \
  u32 cgx, cgy, attributes, attributes_b, attributes_wide;                    \
  u32 i, i2;                                                                  \
  u32 flags = 0;                                                              \
  sat_cache_struct *sat_cache = &(vdc->sat_cache);                            \
  u32 screen_end = vdc->screen_end;                                           \
  s32 sprite_offset = (vdc->scanline_offset - vdc->scroll_shift) - 32;        \
                                                                              \
  /* Cycle through each sprite and place it on the appropriate sat cache    */\
  /* line if it's visible and within 16 active (and increase                */\
                                                                              \
  for(i = 0; i < 263; i++)                                                    \
  {                                                                           \
    sat_cache_line_struct *sat_cache_line = &(sat_cache->lines[i]);           \
    sat_cache_line->flags = 0;                                                \
    sat_cache_line->num_active = 0;                                           \
    sat_cache_line->num_present = 0;                                          \
    sat_cache_line->overflow = 0;                                             \
  }                                                                           \
                                                                              \
  for(i = 0; i < 64; i++, current_sprite += 4)                                \
  {                                                                           \
    mask_sprite();                                                            \
                                                                              \
    y = (current_sprite[0] & 0x3FF) - 64;                                     \
    x = (current_sprite[1] & 0x3FF) + sprite_offset;                          \
    attributes = current_sprite[3] & 0xB98F;                                  \
    cgx = ((attributes >> 8) & 0x1);                                          \
    cgy = ((attributes >> 12) & 0x3);                                         \
                                                                              \
    /* cgy 2 is not supposed to be valid, but really it's like 3            */\
    if(cgy == 2)                                                              \
      cgy = 3;                                                                \
                                                                              \
    width = (cgx + 1) * 16;                                                   \
    height = (cgy + 1) * 16;                                                  \
                                                                              \
    /* Only count sprites that fall on some scanline.                       */\
    if(((y + height) > 0) && (y < 263))                                       \
    {                                                                         \
      /* Sprites that are on the visible portion of the screen are added    */\
      /* to the list of sprites to draw; ones that aren't only increment    */\
      /* the number of sprites present on the line. This contributes to     */\
      /* the number of sprites which may be visible on a given line.        */\
                                                                              \
      if(((x + width) > 0) && (x < (s32)screen_end))                          \
      {                                                                       \
        pattern_number = (current_sprite[2] >> 1) & 0x3FF;                    \
                                                                              \
        pattern_number &= ~cgx;                                               \
                                                                              \
        if(cgy > 0)                                                           \
          pattern_number &= ~0x2;                                             \
                                                                              \
        if(cgy > 1)                                                           \
          pattern_number &= ~0x4;                                             \
                                                                              \
        if(pattern_number >= 0x200)                                           \
          continue;                                                           \
                                                                              \
        flags = (attributes & SPRITE_ATTRIBUTE_PRIORITY) == 0;                \
        if(attributes & SPRITE_ATTRIBUTE_HFLIP)                               \
          attributes |= SPRITE_ATTRIBUTE_HFLIP_CHECK;                         \
                                                                              \
        pattern_line_offset = (pattern_number * 16);                          \
                                                                              \
        if(current_sprite[2] & 0x1)                                           \
          attributes |= SPRITE_ATTRIBUTE_CG;                                  \
                                                                              \
        if(i == 0)                                                            \
          flags |= LINE_CACHE_FLAGS_SPR0;                                     \
                                                                              \
        if(cgx)                                                               \
        {                                                                     \
          attributes_b = attributes;                                          \
          if(x < 0)                                                           \
          {                                                                   \
            if(x <= -16)                                                      \
            {                                                                 \
              attributes_b &= ~SPRITE_ATTRIBUTE_WIDE;                         \
              update_satb_fill_sprite_double_option(no, yes, limit_check);    \
              continue;                                                       \
            }                                                                 \
          }                                                                   \
                                                                              \
          if((x + 32) >= screen_end)                                          \
          {                                                                   \
            if((x + 16) >= screen_end)                                        \
            {                                                                 \
              attributes &= ~SPRITE_ATTRIBUTE_WIDE;                           \
              update_satb_fill_sprite_double_option(yes, no, limit_check);    \
              continue;                                                       \
            }                                                                 \
          }                                                                   \
                                                                              \
          attributes_wide = attributes | attributes_b;                        \
          update_satb_fill_sprite_double_option(yes, yes, limit_check);       \
        }                                                                     \
        else                                                                  \
        {                                                                     \
          if(attributes & SPRITE_ATTRIBUTE_VFLIP)                             \
          {                                                                   \
            update_satb_fill_sprite(flip, limit_check);                       \
          }                                                                   \
          else                                                                \
          {                                                                   \
            update_satb_fill_sprite(normal, limit_check);                     \
          }                                                                   \
        }                                                                     \
      }                                                                       \
      else                                                                    \
      {                                                                       \
        if(y < 0)                                                             \
        {                                                                     \
          height += y;                                                        \
          y = 0;                                                              \
        }                                                                     \
                                                                              \
        if((y + height) > 263)                                                \
          height = (263 - y);                                                 \
                                                                              \
        if(cgx)                                                               \
        {                                                                     \
          for(i2 = y; i2 < (height + y); i2++)                                \
          {                                                                   \
            update_satb_add_offscreen_##limit_check();                        \
            update_satb_add_offscreen_##limit_check();                        \
          }                                                                   \
        }                                                                     \
        else                                                                  \
        {                                                                     \
          for(i2 = y; i2 < (height + y); i2++)                                \
          {                                                                   \
            update_satb_add_offscreen_##limit_check();                        \
          }                                                                   \
        }                                                                     \
      }                                                                       \
    }                                                                         \
  }                                                                           \
}                                                                             \

update_satb_maker(limit)
update_satb_maker(no_limit)

void update_satb(vdc_struct *vdc)
{
  if(config.unlimit_sprites)
    update_satb_no_limit(vdc);
  else
    update_satb_limit(vdc);
}


#define adjust_dma_ptr_up(ptr)                                                \
  ptr = (ptr + 1) & 0x7FFF                                                    \

#define adjust_dma_ptr_down(ptr)                                              \
  ptr = (ptr - 1) & 0x7FFF                                                    \

#define vram_dma_loop(source_direction, dest_direction)                       \
  while(lenr >= 0)                                                            \
  {                                                                           \
    value = vdc->vram[sour];                                                  \
    vdc->vram[desr] = value;                                                  \
                                                                              \
    adjust_dma_ptr_##source_direction(sour);                                  \
    adjust_dma_ptr_##dest_direction(desr);                                    \
                                                                              \
    lenr--;                                                                   \
  }                                                                           \


#define update_tile_cache(start, end)                                         \
{                                                                             \
  u32 start_region = (start) / 16;                                            \
  u32 end_region = (end) / 16;                                                \
  u32 i;                                                                      \
                                                                              \
  for(i = start_region; i <= end_region; i++)                                 \
  {                                                                           \
    vdc->dirty_tiles[i] = 1;                                                  \
    vdc->dirty_patterns[i / 4] = 1;                                           \
  }                                                                           \
}                                                                             \
                                                                              \

void vdc_vram_dma_line(vdc_struct *vdc)
{
  u32 sour = vdc->sour;
  u32 desr = vdc->desr;
  s32 lenr = vdc->lenr;
  u32 value;

  if(lenr > (vce.scanline_cycles / 6))
    lenr = vce.scanline_cycles / 6;

  vdc->lenr -= lenr + 1;

  switch((vdc->dcr >> 2) & 0x3)
  {
    case 0:
      update_tile_cache(desr, desr + lenr);
      vram_dma_loop(up, up);
      break;

    case 1:
      update_tile_cache(desr, desr + lenr);
      vram_dma_loop(down, up);
      break;

    case 2:
      update_tile_cache(desr - lenr, desr);
      vram_dma_loop(up, down);
      break;

    case 3:
      update_tile_cache(desr - lenr, desr);
      vram_dma_loop(down, down);
      break;
  }

  vdc->desr = desr;
  vdc->sour = sour;

  if(vdc->lenr == -1)
  {
    vdc->vram_dma_trigger = 0;

    if(vdc->dcr & 0x2)
    {
      vdc->status |= VDC_STATUS_DMA_IRQ;
      raise_interrupt(IRQ_VDC);
    }
  }
}

void vdc_frame_start(vdc_struct *vdc)
{
  // vsr high
  vdc->latch_vds = vdc->vds;
  // vsr low
  vdc->latch_vsw = vdc->vsw;
  vdc->latch_vdw = vdc->vdw;
  vdc->latch_vcr = vdc->vcr;
  
  vdc->start_line = vdc->vds + vdc->vsw;
  vdc->vblank_line = vdc->start_line + vdc->vdw + 1;
  
  if(vdc->vblank_line > 261)
    vdc->vblank_line = 261;
  
  vdc->display_counter = 0;
  
  // If SPR/BG are both off at the first scanline then the VDC enters
  // burst mode, freeing it up from accessing memory for the entire
  // frame (so nothing is rendered, but you can do DMA)
  if(vdc->cr & 0xC0)
    vdc->burst_mode = 0;
  else
    vdc->burst_mode = 1;
}

u32 vdc_check_vblank(vdc_struct *vdc)
{
  if(vdc->display_counter == vdc->vblank_line)
  {
    vdc->vblank_active = 1;

    vdc->mwr = vdc->latched_mwr;

    if(vdc->satb_dma_trigger || (vdc->dcr & 0x10))
    {
      vdc->satb_dma_trigger = 0;
      vdc->satb_dma_irq_lines = 2;

      if(vdc->satb < 0x8000)
      {
        memcpy(vdc->sat, vdc->vram + vdc->satb, 512);
        update_satb(vdc);
      }
    }
    return 1;
  }
  return 0;
}

u32 vdc_check_raster_line(vdc_struct *vdc)
{
  if(((s32)vdc->raster_line == ((s32)vdc->rcr - 0x40)) && (vdc->cr & 0x04))
  {
    vdc->status |= VDC_STATUS_RASTER_IRQ;
    raise_interrupt(IRQ_VDC);
    return 1;
  }
  return 0;
}

void vdc_burst_mode_dma(vdc_struct *vdc)
{
  if(vdc->vram_dma_trigger &&
   (vdc->burst_mode || (vdc->display_counter < vdc->start_line) ||
   (vdc->display_counter >= vdc->vblank_line)))
  {
    vdc_vram_dma_line(vdc);
  }
}

void vdc_set_effective_byr(vdc_struct *vdc)
{
  if(vdc->display_counter == (vdc->latch_vds + vdc->latch_vsw))
    vdc->effective_byr = vdc->byr;
  else
    vdc->effective_byr++;
}


void vdc_line_increment(vdc_struct *vdc)
{
  (vdc->raster_line)++;
  (vdc->display_counter)++;

  if(vdc->satb_dma_irq_lines)
  {
    vdc->satb_dma_irq_lines--;
    if(vdc->satb_dma_irq_lines == 0)
    {
      cpu.vdc_stalled = 0;

      if(vdc->dcr & 0x01)
      {
        vdc->status |= VDC_STATUS_SATB_IRQ;
        raise_interrupt(IRQ_VDC);
      }
    }
  }

  if(vdc->display_counter == vdc->start_line)
    vdc->raster_line = 0;

  if(vdc->display_counter ==
   (vdc->latch_vds + vdc->latch_vsw + vdc->latch_vdw + 3 + vdc->latch_vcr))
  {
    vdc->display_counter = 0;
  }
}


void update_frame_execute(u32 skip)
{
  s32 hds_cycles;

  static u32 check_idle_loop_line = 0;

  // Check up to three RCRs for idle loops
  u32 check_idle_loops = 3;

  do
  {
    if(check_idle_loops && (vce.frame_counter == check_idle_loop_line))
    {
      patch_idle_loop();
      check_idle_loops--;
    }

    if(vce.frame_counter == 0)
      vdc_frame_start(&vdc_a);

    vdc_a.vblank_active = 0;

    vdc_burst_mode_dma(&vdc_a);

    if(vdc_check_vblank(&vdc_a))
    {
      // Most games will be waiting before IRQs, so check here.
      if(check_idle_loops)
      {
        patch_idle_loop();
        check_idle_loops--;
      }
    }

    if(vdc_check_raster_line(&vdc_a))
    {
      if(check_idle_loops != 0)
      {
        patch_idle_loop();
        check_idle_loops--;
      }
    }

    hds_cycles = vdc_a.hds_cycles;

    execute_instructions_timer(hds_cycles);

    vdc_set_effective_byr(&vdc_a);

    if(!skip)
    {
      if (config.software_rendering)
        render_line();
      else
        render_line_hw();
    }

    if(vdc_a.vblank_active && (vdc_a.cr & 0x08))
    {
      // Execute the rest of the cycles. If a vlank IRQ is actually occuring
      // then this means we should execute 6 cycles first before actually
      // raising it (giving the game a chance to reset it)

      vdc_a.status |= VDC_STATUS_VBLANK_IRQ;

      execute_instructions_timer(6);
      hds_cycles += 6;

      if(vdc_a.status & VDC_STATUS_VBLANK_IRQ)
        raise_interrupt(IRQ_VDC);
    }

    // Active display
    execute_instructions_timer(vce.scanline_cycles - hds_cycles);

    vdc_line_increment(&vdc_a);

    vce.frame_counter++;

    if(vce.frame_counter == vce.num_lines)
      vce.frame_counter = 0;

  } while(vce.frame_counter != vdc_a.vblank_line);

  check_idle_loop_line += 16;
  if(check_idle_loop_line >= vce.num_lines)
    check_idle_loop_line -= vce.num_lines;
}

void update_frame_execute_sgx(u32 skip)
{
  s32 hds_cycles;

  static u32 check_idle_loop_line = 0;

  // Check up to three RCRs for idle loops
  u32 check_idle_loops = 3;

  do
  {
    if(check_idle_loops && (vce.frame_counter == check_idle_loop_line))
    {
      patch_idle_loop();
      check_idle_loops--;
    }

    if(vce.frame_counter == 0)
    {
      vdc_frame_start(&vdc_a);
      vdc_frame_start(&vdc_b); 
    }

    vdc_a.vblank_active = 0;
    vdc_b.vblank_active = 0;

    vdc_burst_mode_dma(&vdc_a);
    vdc_burst_mode_dma(&vdc_b);   
 
    if(vdc_check_vblank(&vdc_a))
    {
      // Most games will be waiting before IRQs, so check here.
      if(check_idle_loops)
      {
        patch_idle_loop();
        check_idle_loops--;
      }
    }
    vdc_check_vblank(&vdc_b);

    if(vdc_check_raster_line(&vdc_a))
    {
      if(check_idle_loops != 0)
      {
        patch_idle_loop();
        check_idle_loops--;
      }
    }
    vdc_check_raster_line(&vdc_b);

    hds_cycles = vdc_a.hds_cycles;

    execute_instructions_timer(hds_cycles);

    vdc_set_effective_byr(&vdc_a);
    vdc_set_effective_byr(&vdc_b);

    if(!skip)
      render_line_sgx();
      
    if(vdc_a.vblank_active && (vdc_a.cr & 0x08))
    {
      // Execute the rest of the cycles. If a vlank IRQ is actually occuring
      // then this means we should execute 6 cycles first before actually
      // raising it (giving the game a chance to reset it)

      vdc_a.status |= VDC_STATUS_VBLANK_IRQ;

      execute_instructions_timer(6);
      hds_cycles += 6;

      if(vdc_a.status & VDC_STATUS_VBLANK_IRQ)
        raise_interrupt(IRQ_VDC);
    }

    if(vdc_b.vblank_active && (vdc_b.cr & 0x08))
    {
      vdc_b.status |= VDC_STATUS_VBLANK_IRQ;

      if(vdc_b.status & VDC_STATUS_VBLANK_IRQ)
        raise_interrupt(IRQ_VDC);
    }

    // Active display
    execute_instructions_timer(vce.scanline_cycles - hds_cycles);

    vdc_line_increment(&vdc_a);
    vdc_line_increment(&vdc_b);

    vce.frame_counter++;

    if(vce.frame_counter == vce.num_lines)
      vce.frame_counter = 0;
  } while(vce.frame_counter != vdc_a.vblank_line);

  check_idle_loop_line += 16;
  if(check_idle_loop_line >= vce.num_lines)
    check_idle_loop_line -= vce.num_lines;
}

void update_status_message(void)
{
  static u32 clear_status_messages = 0;
  static u32 input_offset_last_time = 0;
  u32 base_offset = 233;

  if(config.input_message)
  {
    base_offset -= 15;
    input_offset_last_time = 1;
  }
  else

  if((input_offset_last_time == 1) &&
   ((vce.screen_width < RESOLUTION_WIDTH) &&
   (config.scale_width == 0)))
  {
    u32 message_line = 218 - (config.status_message_lines * 10);

    while(message_line < 233 - (config.status_message_lines * 10))
    {
      clear_line_edges(message_line, 0, vce.screen_center_offset,
       vce.screen_width);
      message_line++;
    }
    input_offset_last_time = 0;
  }

  if(config.status_message_counter)
  {
    u32 message_line = base_offset - (config.status_message_lines * 10);
    u32 i;

    set_font_narrow();

    for(i = 0; i < config.status_message_lines; i++)
    {
      if((vce.screen_width < RESOLUTION_WIDTH) &&
       (config.scale_width == 0))
      {
        u32 i2;

        for(i2 = message_line; i2 < message_line + 10; i2++)
        {
          clear_line_edges(i2, 0, vce.screen_center_offset,
           vce.screen_width);
        }
      }
      print_string(config.status_message[i], 0xFFFF, 0x000,
       3, message_line, RESOLUTION_WIDTH);
      message_line += 10;
    }

    config.status_message_counter--;
    if(config.status_message_counter == 0)
      config.status_message_lines = 0;

    set_font_wide();

    clear_status_messages = 1;
  }
  else

  if(clear_status_messages && (vce.screen_width < RESOLUTION_WIDTH) &&
   (config.scale_width == 0))
  {
    u32 message_line = base_offset - (STATUS_MESSAGE_LINES * 10);

    clear_status_messages = 0;

    while(message_line < base_offset + 2)
    {
      clear_line_edges(message_line, 0, vce.screen_center_offset,
       vce.screen_width);
      message_line++;
    }
  }
}

void update_frame(u32 skip)
{
  static u32 input_message_last;

  if(config.sgx_mode)
    update_frame_execute_sgx(skip);
  else
    update_frame_execute(skip);

  set_font_wide();
  char print_buffer[128];

  if(config.fast_forward)
  {
    print_string("--FF--", 0xFFFF, 0x000, 320 - (6 * 6) -
     vce.screen_center_offset, 0, vce.screen_width);
  }

  if(config.netplay_type != NETPLAY_TYPE_NONE)
  {
    char print_buffer[128];
    sprintf(print_buffer, "latency: %d, (%1.2lf, %d) -> (%1.2lf, %d)",
     netplay.frame_latency, (double)netplay.period_latency /
     LATENCY_CALCULATION_PERIOD, netplay.period_stalls,
     (double)netplay.remote_latency_report / LATENCY_CALCULATION_PERIOD,
     netplay.remote_stalls_report);

    set_font_narrow();
    print_string(print_buffer, 0xFFFF, 0x000, vce.screen_center_offset, 0,
     vce.screen_width);
    set_font_wide();
  }

  if(config.show_fps)
  {
    static u32 frames = 0;
    static u32 fps = 60;
    const u32 frame_interval = 60;

    static u64 fps_timestamp;
    static u64 last_fps_timestamp = 0;

    frames++;

    if(frames == frame_interval)
    {
      u32 time_delta;

      get_ticks_us(&fps_timestamp);
      if(last_fps_timestamp == 0)
        time_delta = 1000000;
      else
        time_delta = fps_timestamp - last_fps_timestamp;
      last_fps_timestamp = fps_timestamp;

      fps = (u64)((u64)1000000 * (u64)frame_interval) / time_delta;

      frames = 0;
    }

    //sprintf(print_buffer, "%02d/60", fps);
    sprintf(print_buffer, "%02d", fps);
    print_string(print_buffer, 0xFFFF, 0x000, 0, 0,
     vce.screen_width);
  }

  update_status_message();

  if(config.input_message)
  {
    char input_message_full[128];

    sprintf(input_message_full, "%s> %s", config.netplay_username, 
     config.input_message);

    if((vce.screen_width < RESOLUTION_WIDTH) &&
     (config.scale_width == 0))
    {
      u32 i;
      for(i = 218; i < 218 + 20; i++)
      {
        clear_line_edges(i, 0, vce.screen_center_offset, vce.screen_width);
      }
    }

    set_font_narrow();
    if(strlen(input_message_full) > SCREEN_WIDTH_NARROW_CHARS)
    {
      print_string(input_message_full, 0xFFFF, 0x000, 3, 218, RESOLUTION_WIDTH);
      print_string(input_message_full + SCREEN_WIDTH_NARROW_CHARS, 0xFFFF,
       0x000, 3, 228, RESOLUTION_WIDTH);
    }
    else
    {
      print_string(input_message_full, 0xFFFF, 0x000, 0x3, 228,
       RESOLUTION_WIDTH);
    }
    set_font_wide();
    input_message_last = 1;
  }
  else

  if(input_message_last && (vce.screen_width < RESOLUTION_WIDTH) &&
   (config.scale_width == 0))
  {
    u32 i;

    for(i = 218; i < 228 + 10; i++)
    {
      clear_line_edges(i, 0, vce.screen_center_offset, vce.screen_width);
    }

    input_message_last = 0;
  }

  vce.frames_rendered++;

  if(skip != 1)
    update_screen();
}

void dump_vram(u32 start, u32 size)
{
  u32 i, i2, offset;

  for(i = 0, offset = start; i < size / 8; i++)
  {
    for(i2 = 0; i2 < 8; i2++, offset++)
    {
      printf("%04x ", vdc_a.vram[offset]);
    }
    printf("\n");
  }
}

void dump_palette(u32 start, u32 size)
{
  u32 i, i2, offset;

  for(i = 0, offset = start; i < size / 8; i++)
  {
    for(i2 = 0; i2 < 8; i2++, offset++)
    {
      printf("%04x ", vce.palette[offset]);
    }
    printf("\n");
  }
}

void dump_spr(u32 start, u32 size)
{
  u32 i, offset;
  u32 spr_a, spr_b, spr_c, spr_d;

  char *ny = "ny";

  for(i = 0, offset = start; i < size; i++)
  {
    spr_a = vdc_a.sat[(i * 4)];
    spr_b = vdc_a.sat[(i * 4) + 1];
    spr_c = vdc_a.sat[(i * 4) + 2];
    spr_d = vdc_a.sat[(i * 4) + 3];

    printf("spr %x: (%02x %02x %02x %02x)\n", i, spr_a, spr_b, spr_c, spr_d);
    printf("  size: %d by %d\n", (((spr_d >> 8) & 0x1) + 1) * 16,
     (((spr_d >> 12) & 0x3) + 1) * 16);
    printf("  position: %d, %d\n", (spr_b & 0x3FF) - 32,
     (spr_a & 0x3FF) - 64);
    printf("  pattern: %x\n", (spr_c >> 1) & 0x1FF);
    printf("  palette: %x\n", spr_d & 0xF);
    printf("  hf: %c  vf %c  hp %c\n",
     ny[(spr_d & SPRITE_ATTRIBUTE_HFLIP) != 0],
     ny[(spr_d & SPRITE_ATTRIBUTE_VFLIP) != 0],
     ny[(spr_d & SPRITE_ATTRIBUTE_PRIORITY) != 0]);
    printf("\n");
  }
}

void dump_sprites_per_line(u32 line)
{
  printf("%d sprites on line %d\n",
   vdc_a.sat_cache.lines[line].num_present, line);
}

void dump_video_status_vdc(vdc_struct *vdc)
{
  printf("vdc.status: %04x\n", vdc->status);
  printf("vdc.register_select: %04x\n", vdc->register_select);
  printf("vdc.mawr: %04x\n", vdc->mawr);
  printf("vdc.marr: %04x\n", vdc->marr);
  printf("vdc.cr: %04x\n", vdc->cr);
  printf("vdc.rcr: %04x\n", vdc->rcr);
  printf("vdc.mwr: %04x\n", vdc->mwr);
  printf("vdc.bxr: %04x\n", vdc->bxr);
  printf("vdc.byr: %04x\n", vdc->byr);
  printf("vdc.hds: %04x\n", vdc->hds);
  printf("vdc.hsw: %04x\n", vdc->hsw);
  printf("vdc.hdw: %04x\n", vdc->hdw);
  printf("vdc.hde: %04x\n", vdc->hde);
  printf("vdc.vds: %04x\n", vdc->vds);
  printf("vdc.vsw: %04x\n", vdc->vsw);
  printf("vdc.vdw: %04x\n", vdc->vdw);
  printf("vdc.dcr: %04x\n", vdc->dcr);
  printf("vdc.vcr: %04x\n", vdc->vcr);
  printf("vdc.sour: %04x\n", vdc->sour);
  printf("vdc.desr: %04x\n", vdc->desr);
  printf("vdc.lenr: %04x\n", vdc->lenr);
  printf("vdc.satb: %04x\n", vdc->satb);
  printf("vdc.write_latch: %04x\n", vdc->write_latch);
  printf("vdc.read_latch: %04x\n", vdc->read_latch);
  printf("vdc.raster_line: %04x\n", vdc->raster_line);
  printf("vdc.effective_byr: %04x\n", vdc->effective_byr);
}

void dump_video_status()
{
  dump_video_status_vdc(&vdc_a);
  printf("vce.control: %04x\n", vce.control);
  printf("vce.frame_counter: %04x\n", vce.frame_counter);
  printf("vce.palette_offset: %04x\n", vce.palette_offset);
}

// Tile cache and pattern cache shouldn't be saved, so either make all the
// data dirty or recache it (the first option is probably better)

// Also, sat should be dealt with again upon load.

#define video_savestate_extra_load_vdc(_vdc)                                  \
  memset(_vdc.dirty_tiles, 1, 2048);                                          \
  memset(_vdc.dirty_patterns, 1, 512);                                        \
  vdc_update_width(&_vdc)                                                     \

#define video_savestate_extra_load()                                          \
{                                                                             \
  u32 i, i2;                                                                  \
  u32 *palette_ptr = vce.palette;                                             \
  u32 *palette_cache_ptr = vce.palette_cache;                                 \
  u16 *palette_cache_5551_ptr = &vce.palette_cache_5551[0];                       \
  u32 zero_entry;                                                             \
  u16 zero_entry_5551;                                                        \
                                                                              \
  zero_entry = palette_convert[palette_ptr[0]];                               \
  zero_entry_5551 = palette_convert_5551[palette_ptr[0]];                     \
                                                                              \
  for(i = 0; i < 16; i++)                                                     \
  {                                                                           \
    palette_cache_ptr[0] = zero_entry;                                        \
    palette_cache_5551_ptr[0] = zero_entry_5551;                              \
    for(i2 = 1; i2 < 16; i2++)                                                \
    {                                                                         \
      palette_cache_ptr[i2] = palette_convert[palette_ptr[i2]];               \
      palette_cache_5551_ptr[i2] = palette_convert_5551[palette_ptr[i2]];     \
    }                                                                         \
    palette_ptr += 16;                                                        \
    palette_cache_ptr += 16;                                                  \
    palette_cache_5551_ptr += 16;                                             \
  }                                                                           \
                                                                              \
  palette_cache_ptr[0] = palette_convert[palette_ptr[0]];                     \
  palette_cache_5551_ptr[0] = palette_convert_5551[palette_ptr[0]];           \
  for(i = 0; i < 16; i++)                                                     \
  {                                                                           \
    for(i2 = 1; i2 < 16; i2++)                                                \
    {                                                                         \
      palette_cache_ptr[i2] = palette_convert[palette_ptr[i2]];               \
      palette_cache_5551_ptr[i2] = palette_convert_5551[palette_ptr[i2]];     \
    }                                                                         \
    palette_ptr += 16;                                                        \
    palette_cache_ptr += 16;                                                  \
    palette_cache_5551_ptr += 16;                                             \
  }                                                                           \
                                                                              \
  if(config.sgx_mode)                                                         \
    vpc.windows_dirty = 1;                                                    \
}                                                                             \

#define video_savestate_default_v1_load()                                     \
  vdc_a.latched_mwr = vdc_a.mwr;                                              \
  if(config.sgx_mode)                                                         \
    vdc_b.latched_mwr = vdc_b.mwr                                             \

#define video_savestate_default_v1_store()                                    \

#define video_savestate_extra_store()                                         \

#define video_savestate_vdc(type, type_b, version_gate, _vdc)                 \
  file_##type##_array(savestate_file, _vdc.vram);                             \
  file_##type##_array(savestate_file, _vdc.sat);                              \
                                                                              \
  file_##type##_variable(savestate_file, _vdc.status);                        \
  file_##type##_variable(savestate_file, _vdc.register_select);               \
  file_##type##_variable(savestate_file, _vdc.mawr);                          \
  file_##type##_variable(savestate_file, _vdc.marr);                          \
  file_##type##_variable(savestate_file, _vdc.cr);                            \
  file_##type##_variable(savestate_file, _vdc.rcr);                           \
  file_##type##_variable(savestate_file, _vdc.mwr);                           \
  file_##type##_variable(savestate_file, _vdc.bxr);                           \
  file_##type##_variable(savestate_file, _vdc.byr);                           \
  file_##type##_variable(savestate_file, _vdc.hds);                           \
  file_##type##_variable(savestate_file, _vdc.hsw);                           \
  file_##type##_variable(savestate_file, _vdc.hde);                           \
  file_##type##_variable(savestate_file, _vdc.hdw);                           \
  file_##type##_variable(savestate_file, _vdc.vds);                           \
  file_##type##_variable(savestate_file, _vdc.vsw);                           \
  file_##type##_variable(savestate_file, _vdc.vdw);                           \
  file_##type##_variable(savestate_file, _vdc.dcr);                           \
  file_##type##_variable(savestate_file, _vdc.vcr);                           \
  file_##type##_variable(savestate_file, _vdc.sour);                          \
  file_##type##_variable(savestate_file, _vdc.desr);                          \
  file_##type##_variable(savestate_file, _vdc.lenr);                          \
  file_##type##_variable(savestate_file, _vdc.satb);                          \
                                                                              \
  file_##type##_variable(savestate_file, _vdc.write_latch);                   \
  file_##type##_variable(savestate_file, _vdc.read_latch);                    \
                                                                              \
  file_##type##_variable(savestate_file, _vdc.raster_line);                   \
  file_##type##_variable(savestate_file, _vdc.effective_byr);                 \
                                                                              \
  file_##type##_variable(savestate_file, dummy);                              \
  file_##type##_variable(savestate_file, dummy);                              \
                                                                              \
  file_##type##_variable(savestate_file, dummy);                              \
  file_##type##_variable(savestate_file, dummy);                              \
  file_##type##_variable(savestate_file, dummy);                              \
  file_##type##_variable(savestate_file, dummy);                              \
                                                                              \
  file_##type##_variable(savestate_file, _vdc.increment_value);               \
                                                                              \
  file_##type##_variable(savestate_file, _vdc.satb_dma_trigger);              \
  file_##type##_variable(savestate_file, _vdc.satb_dma_irq_lines);            \
                                                                              \
  file_##type##_variable(savestate_file, _vdc.vram_dma_trigger);              \
  file_##type##_variable(savestate_file, _vdc.vram_dma_irq_lines);            \
                                                                              \
  file_##type##_variable(savestate_file, _vdc.display_counter);               \
  file_##type##_variable(savestate_file, _vdc.burst_mode);                    \
                                                                              \
  video_savestate_extra_load_vdc(_vdc);                                       \


#define video_savestate_builder(type, type_b, version_gate)                   \
void video_##type_b##_savestate(savestate_##type_b##_type savestate_file)     \
{                                                                             \
  u32 dummy;                                                                  \
  video_savestate_vdc(type, type_b, version_gate, vdc_a);                     \
  if(config.sgx_mode)                                                         \
  {                                                                           \
    printf("sgx stuff for savestate %s\n", #type);                            \
    video_savestate_vdc(type, type_b, version_gate, vdc_b);                   \
    file_##type##_variable(savestate_file, vpc.window1_value);                \
    file_##type##_variable(savestate_file, vpc.window2_value);                \
    file_##type##_variable(savestate_file, vpc.window_status);                \
    file_##type##_variable(savestate_file, vpc.select);                       \
  }                                                                           \
  file_##type##_array(savestate_file, vce.palette);                           \
                                                                              \
  file_##type##_variable(savestate_file, vce.control);                        \
  file_##type##_variable(savestate_file, vce.palette_offset);                 \
  file_##type##_variable(savestate_file, vce.num_lines);                      \
                                                                              \
  file_##type##_variable(savestate_file, vce.frame_counter);                  \
                                                                              \
  if(version_gate >= 2)                                                       \
  {                                                                           \
    file_##type##_variable(savestate_file, vdc_a.latched_mwr);                \
    if(config.sgx_mode)                                                       \
      file_##type##_variable(savestate_file, vdc_b.latched_mwr);              \
  }                                                                           \
  else                                                                        \
  {                                                                           \
    video_savestate_default_v1_##type_b();                                    \
  }                                                                           \
  video_savestate_extra_##type_b();                                           \
}                                                                             \

build_savestate_functions(video);

