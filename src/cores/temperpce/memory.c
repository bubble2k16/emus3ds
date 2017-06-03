#include "common.h"

#ifdef ARM_ARCH
memory_struct memory __attribute__ ((aligned(8192)));
#else
memory_struct memory;
#endif

#ifdef CRC_CHECK
const u32 sgx_table[6] = 
{
	0xbebfe042,
	0x4c2126b0,
	0x8c4588e2,
	0x1f041166,
	0xb486a8ed,
	0x3b13af61,
};
#endif

char *get_mpr_region_name(u32 mpr_number)
{
  static char name_buffer[32];

  switch(memory.mpr[mpr_number])
  {
    // HuCard ROM
    case 0x00 ... 0x67:
      sprintf(name_buffer, "ROM-%02x", memory.mpr[mpr_number]);
      return name_buffer;

    case 0x68 ... 0x7F:
      if(config.cd_loaded)
      {
        sprintf(name_buffer, "SCD-%02x", memory.mpr[mpr_number] - 0x60);
        return name_buffer;
      }
      sprintf(name_buffer, "ROM-%02x", memory.mpr[mpr_number]);
      return name_buffer;

    case 0x80 ... 0x87:
      if(config.cd_loaded)
      {
        sprintf(name_buffer, "CD-%02x ", memory.mpr[mpr_number] - 0x80);
        return name_buffer;
      }
      return "INV";

    // Unusued
    case 0x88 ... 0xF6:
      return "INV";

    // BRAM
    case 0xF7:
      if(memory_map_get(write, 0xF7) == cd.bram)
        return "BRAM";
      return "INV";

    // work RAM
    case 0xF8 ... 0xFB:
      if(config.sgx_mode)
      {
        sprintf(name_buffer, "RAM-%02x", memory.mpr[mpr_number] - 0xF8);
        return name_buffer;
      }
      return "RAM";

    // Unused
    case 0xFC ... 0xFE:
      return "INV";

    // I/O
    case 0xFF:
      return "I/O";
  }

  return "???";
}

u32 mpr_read(u32 mpr_regs)
{
  u32 mpr_value;
  u32 i;

  if(mpr_regs == 0)
    return memory.last_mpr_value;

  mpr_value = 0;

  // Charles MacDonald's doc simply says the values are
  // combined in some way, but he is unsure how. Bitwise
  // OR should be as good a guess as any...

  for(i = 0; i < 8; i++)
  {
    if(mpr_regs & (1 << i))
    {
      mpr_value |= memory.mpr[i];
    }
  }

  return mpr_value;
}

void mpr_write(u32 mpr_regs, u32 mpr_value)
{
  u8 *mpr_translated_value_read = memory.memory_map_read[mpr_value];
  u8 *mpr_translated_value_write = memory.memory_map_write[mpr_value];
  u32 i = 0;

  if(mpr_regs)
    memory.last_mpr_value = mpr_value;

  if(!mpr_check_ext(mpr_translated_value_read))
  {
    // Read standard
    if(!mpr_check_ext(mpr_translated_value_write))
    {
      // Read and write standard
      while(mpr_regs)
      {
        if(mpr_regs & 1)
        {
          memory.mpr[i] = mpr_value;
          mpr_translated_set_read(mpr_translated_value_read, i);
          mpr_translated_set_write(mpr_translated_value_write, i);
        }
        mpr_regs >>= 1;
        i++;
      }
    }
    else
    {
      // Read standard, write extended
      while(mpr_regs)
      {
        if(mpr_regs & 1)
        {
          memory.mpr[i] = mpr_value;
          mpr_translated_set_read(mpr_translated_value_read, i);
          mpr_translated_set_write_ext(mpr_translated_value_write, i);
        }
        mpr_regs >>= 1;
        i++;
      }
    }
  }
  else
  {
    // Read extended
    if(!mpr_check_ext(mpr_translated_value_write))
    {
      // Read extended, write standard
      while(mpr_regs)
      {
        if(mpr_regs & 1)
        {
          memory.mpr[i] = mpr_value;
          mpr_translated_set_read_ext(mpr_translated_value_read, i);
          mpr_translated_set_write(mpr_translated_value_write, i);
        }
        mpr_regs >>= 1;
        i++;
      }
    }
    else
    {
      // Read extended, write extended
      while(mpr_regs)
      {
        if(mpr_regs & 1)
        {
          memory.mpr[i] = mpr_value;
          mpr_translated_set_read_ext(mpr_translated_value_read, i);
          mpr_translated_set_write_ext(mpr_translated_value_write, i);
        }
        mpr_regs >>= 1;
        i++;
      }
    }
  }
}

void memory_remap_read(u32 region, u32 size, u8 *ptr)
{
  u32 i;

  for(i = 0; i < size; i++)
  {
    memory_map_set(read, region + i, ptr + (i * 0x2000));
  }

  for(i = 0; i < 8; i++)
  {
    if(!memory_map_check_ext(read, memory.mpr[i]))
      mpr_translated_set_read(memory.memory_map_read[memory.mpr[i]], i);
  }
}

void memory_remap_read_fixed(u32 region, u32 size, u8 *ptr)
{
  u32 i;

  for(i = 0; i < size; i++)
  {
    memory_map_set(read, region + i, ptr);
  }

  for(i = 0; i < 8; i++)
  {
    if(!memory_map_check_ext(read, memory.mpr[i]))
      mpr_translated_set_read(memory.memory_map_read[memory.mpr[i]], i);
  }
}

void memory_remap_write(u32 region, u32 size, u8 *ptr)
{
  u32 i;

  for(i = 0; i < size; i++)
  {
    memory_map_set(write, region + i, ptr + (i * 0x2000));
  }

  for(i = 0; i < 8; i++)
  {
    if(!memory_map_check_ext(write, memory.mpr[i]))
      mpr_translated_set_write(memory.memory_map_write[memory.mpr[i]], i);
  }
}

void memory_remap_write_fixed(u32 region, u32 size, u8 *ptr)
{
  u32 i;

  for(i = 0; i < size; i++)
  {
    memory_map_set(write, region + i, ptr);
  }

  for(i = 0; i < 8; i++)
  {
    if(!memory_map_check_ext(write, memory.mpr[i]))
      mpr_translated_set_write(memory.memory_map_write[memory.mpr[i]], i);
  }
}

void memory_remap_write_io(u32 region, u32 size,
 io_write_function_ptr *io_write_functions)
{
  u32 i;

  for(i = 0; i < size; i++)
  {
    memory_map_set_ext(write, region + i, io_write_functions);
  }

  for(i = 0; i < 8; i++)
  {
    if(memory_map_check_ext(write, memory.mpr[i]))
      mpr_translated_set_write_ext(memory.memory_map_write[memory.mpr[i]], i);
  }
}

void memory_remap_read_write(u32 region, u32 size, u8 *ptr_read,
 u8 *ptr_write)
{
  u32 i;

  for(i = 0; i < size; i++)
  {
    memory_map_set(read, region + i, ptr_read + (i * 0x2000));
    memory_map_set(write, region + i, ptr_write + (i * 0x2000));
  }

  for(i = 0; i < 8; i++)
  {
    if((u32)(memory.mpr[i] - region) < size)
    {
      mpr_translated_set_read(memory.memory_map_read[memory.mpr[i]], i);
      mpr_translated_set_write(memory.memory_map_write[memory.mpr[i]], i);
    }
  }
}

void memory_remap_read_write_io(u32 region, u32 size,
 io_read_function_ptr *io_read_functions,
 io_write_function_ptr *io_write_functions)
{
  u32 i;

  for(i = 0; i < size; i++)
  {
    memory_map_set_ext(read, region + i, io_read_functions);
    memory_map_set_ext(write, region + i, io_write_functions);
  }

  for(i = 0; i < 8; i++)
  {
    if(memory_map_check_ext(read, memory.mpr[i]))
      mpr_translated_set_read_ext(memory.memory_map_read[memory.mpr[i]], i);

    if(memory_map_check_ext(write, memory.mpr[i]))
      mpr_translated_set_write_ext(memory.memory_map_write[memory.mpr[i]], i);
  }
}


void memory_remap_read_null(u32 region, u32 size)
{
  memory_remap_read_fixed(region, size, memory.null_space_read);
}

void memory_remap_write_null(u32 region, u32 size)
{
  memory_remap_write_fixed(region, size, memory.null_space_write);
}

void memory_remap_read_write_null(u32 region, u32 size)
{
  memory_remap_read_write(region, size, memory.null_space_read,
   memory.null_space_write);
}

u32 io_buffer_read()
{
  return memory.io_buffer;
}

u32 zero_read()
{
  return 0;
}

u32 zero_three_read()
{
  return 0x03;
}

u32 five_five_read()
{
  return 0x55;
}

u32 aa_read()
{
  return 0xAA;
}

u32 ff_read()
{
  memory.io_buffer = 0xFF;
  return 0xFF;
}

void null_write(u32 value)
{
}

void setup_io_tables()
{
  u32 i;
  io_read_function_ptr *current_io_read_function =
   memory.io_read_functions;
  io_write_function_ptr *current_io_write_function =
   memory.io_write_functions;

  printf("Initializing I/O tables.\n");
  // VDC section: 0x0000 to 0x03FF
  if(config.sgx_mode)
  {
    printf("Using SGX VDC map.\n");
    for(i = 0; i < (0x400 / 0x20); i++, current_io_read_function += 0x20,
     current_io_write_function += 0x20)
    {
      current_io_read_function[0x0] = vdc_status_vdc_a;
      current_io_read_function[0x1] = zero_read;
      current_io_read_function[0x2] = vdc_data_read_low_vdc_a;
      current_io_read_function[0x3] = vdc_data_read_high_vdc_a;
      current_io_read_function[0x4] = vdc_status_vdc_a;
      current_io_read_function[0x5] = zero_read;
      current_io_read_function[0x6] = vdc_data_read_low_vdc_a;
      current_io_read_function[0x7] = vdc_data_read_high_vdc_a;

      current_io_read_function[0x8] = vpc_read_window_status_low;
      current_io_read_function[0x9] = vpc_read_window_status_high;
      current_io_read_function[0xA] = vpc_read_window1_low;
      current_io_read_function[0xB] = vpc_read_window1_high;
      current_io_read_function[0xC] = vpc_read_window2_low;
      current_io_read_function[0xD] = vpc_read_window2_high;
      current_io_read_function[0xE] = zero_read;
      current_io_read_function[0xF] = zero_read;

      current_io_read_function[0x10] = vdc_status_vdc_b;
      current_io_read_function[0x11] = zero_read;
      current_io_read_function[0x12] = vdc_data_read_low_vdc_b;
      current_io_read_function[0x13] = vdc_data_read_high_vdc_b;
      current_io_read_function[0x14] = vdc_status_vdc_b;
      current_io_read_function[0x15] = zero_read;
      current_io_read_function[0x16] = vdc_data_read_low_vdc_b;
      current_io_read_function[0x17] = vdc_data_read_high_vdc_b;

      current_io_read_function[0x18] = zero_read;
      current_io_read_function[0x19] = zero_read;
      current_io_read_function[0x1A] = zero_read;
      current_io_read_function[0x1B] = zero_read;
      current_io_read_function[0x1C] = zero_read;
      current_io_read_function[0x1D] = zero_read;
      current_io_read_function[0x1E] = zero_read;
      current_io_read_function[0x1F] = zero_read;

      current_io_write_function[0x0] = vdc_register_select_vdc_a;
      current_io_write_function[0x1] = null_write;
      current_io_write_function[0x2] = vdc_data_write_low_vdc_a;
      current_io_write_function[0x3] = vdc_data_write_high_vdc_a;
      current_io_write_function[0x4] = vdc_register_select_vdc_a;
      current_io_write_function[0x5] = null_write;
      current_io_write_function[0x6] = vdc_data_write_low_vdc_a;
      current_io_write_function[0x7] = vdc_data_write_high_vdc_a;

      current_io_write_function[0x8] = vpc_write_window_status_low;
      current_io_write_function[0x9] = vpc_write_window_status_high;
      current_io_write_function[0xA] = vpc_write_window1_low;
      current_io_write_function[0xB] = vpc_write_window1_high;
      current_io_write_function[0xC] = vpc_write_window2_low;
      current_io_write_function[0xD] = vpc_write_window2_high;
      current_io_write_function[0xE] = vpc_select;
      current_io_write_function[0xF] = null_write;

      current_io_write_function[0x10] = vdc_register_select_vdc_b;
      current_io_write_function[0x11] = null_write;
      current_io_write_function[0x12] = vdc_data_write_low_vdc_b;
      current_io_write_function[0x13] = vdc_data_write_high_vdc_b;
      current_io_write_function[0x14] = vdc_register_select_vdc_b;
      current_io_write_function[0x15] = null_write;
      current_io_write_function[0x16] = vdc_data_write_low_vdc_b;
      current_io_write_function[0x17] = vdc_data_write_high_vdc_b;

      current_io_write_function[0x18] = null_write;
      current_io_write_function[0x19] = null_write;
      current_io_write_function[0x1A] = null_write;
      current_io_write_function[0x1B] = null_write;
      current_io_write_function[0x1C] = null_write;
      current_io_write_function[0x1D] = null_write;
      current_io_write_function[0x1E] = null_write;
      current_io_write_function[0x1F] = null_write;
    }
  }
  else
  {
    for(i = 0; i < (0x400 / 0x4); i++, current_io_read_function += 0x4,
     current_io_write_function += 0x4)
    {
      current_io_read_function[0x0] = vdc_status_vdc_a;
      current_io_read_function[0x1] = zero_read;
      current_io_read_function[0x2] = vdc_data_read_low_vdc_a;
      current_io_read_function[0x3] = vdc_data_read_high_vdc_a;

      current_io_write_function[0x0] = vdc_register_select_vdc_a;
      current_io_write_function[0x1] = null_write;
      current_io_write_function[0x2] = vdc_data_write_low_vdc_a;
      current_io_write_function[0x3] = vdc_data_write_high_vdc_a;
    }
  }

  // VCE section: 0x0400 to 0x7FF
  for(i = 0; i < (0x400 / 0x8); i++, current_io_read_function += 0x8,
   current_io_write_function += 8)
  {
    current_io_read_function[0x0] = ff_read;
    current_io_read_function[0x1] = ff_read;
    current_io_read_function[0x2] = ff_read;
    current_io_read_function[0x3] = ff_read;
    current_io_read_function[0x4] = vce_data_read_low;
    current_io_read_function[0x5] = vce_data_read_high;
    current_io_read_function[0x6] = ff_read;
    current_io_read_function[0x7] = ff_read;

    current_io_write_function[0x0] = vce_control_write;
    current_io_write_function[0x1] = null_write;
    current_io_write_function[0x2] = vce_address_write_low;
    current_io_write_function[0x3] = vce_address_write_high;
    current_io_write_function[0x4] = vce_data_write_low;
    current_io_write_function[0x5] = vce_data_write_high;
    current_io_write_function[0x6] = null_write;
    current_io_write_function[0x7] = null_write;
  }

  // PSG section: 0x0800 to 0x0BFF
  for(i = 0; i < (0x400 / 0x10); i++, current_io_read_function += 0x10,
   current_io_write_function += 0x10)
  {
    current_io_read_function[0x0] = io_buffer_read;
    current_io_read_function[0x1] = io_buffer_read;
    current_io_read_function[0x2] = io_buffer_read;
    current_io_read_function[0x3] = io_buffer_read;
    current_io_read_function[0x4] = io_buffer_read;
    current_io_read_function[0x5] = io_buffer_read;
    current_io_read_function[0x6] = io_buffer_read;
    current_io_read_function[0x7] = io_buffer_read;
    current_io_read_function[0x8] = io_buffer_read;
    current_io_read_function[0x9] = io_buffer_read;
    current_io_read_function[0xA] = io_buffer_read;
    current_io_read_function[0xB] = io_buffer_read;
    current_io_read_function[0xC] = io_buffer_read;
    current_io_read_function[0xD] = io_buffer_read;
    current_io_read_function[0xE] = io_buffer_read;
    current_io_read_function[0xF] = io_buffer_read;

/*
    current_io_write_function[0x0] = psg_channel_select;
    current_io_write_function[0x1] = psg_global_balance;
    current_io_write_function[0x2] = psg_fine_frequency;
    current_io_write_function[0x3] = psg_rough_frequency;
    current_io_write_function[0x4] = psg_channel_control;
    current_io_write_function[0x5] = psg_channel_balance;
    current_io_write_function[0x6] = psg_sound_data;
    current_io_write_function[0x7] = psg_noise_control;
    current_io_write_function[0x8] = psg_lfo_frequency;
    current_io_write_function[0x9] = psg_lfo_control;
    */
    current_io_write_function[0x0] = psg_reg_queue_00;
    current_io_write_function[0x1] = psg_reg_queue_01;
    current_io_write_function[0x2] = psg_reg_queue_02;
    current_io_write_function[0x3] = psg_reg_queue_03;
    current_io_write_function[0x4] = psg_reg_queue_04;
    current_io_write_function[0x5] = psg_reg_queue_05;
    current_io_write_function[0x6] = psg_reg_queue_06;
    current_io_write_function[0x7] = psg_reg_queue_07;
    current_io_write_function[0x8] = psg_reg_queue_08;
    current_io_write_function[0x9] = psg_reg_queue_09;

    current_io_write_function[0xA] = null_write;
    current_io_write_function[0xB] = null_write;
    current_io_write_function[0xC] = null_write;
    current_io_write_function[0xD] = null_write;
    current_io_write_function[0xE] = null_write;
    current_io_write_function[0xF] = null_write;
  }

  // Timer section: 0x0C00 to 0x0FFF
  for(i = 0; i < (0x400 / 0x2); i++, current_io_read_function += 0x2,
   current_io_write_function += 0x2)
  {
    current_io_read_function[0x0] = timer_value_read;
    current_io_read_function[0x1] = timer_enable_read;

    current_io_write_function[0x0] = timer_reload_write;
    current_io_write_function[0x1] = timer_enable_write;
  }

  // IO section: 0x1000 to 0x13FF
  for(i = 0; i < 0x400; i++, current_io_read_function++,
   current_io_write_function++)
  {
    current_io_read_function[0x0] = io_port_read;
    current_io_write_function[0x0] = io_port_write;
  }

  // IRQ section: 0x1400 to 0x17FF
  for(i = 0; i < (0x400 / 0x4); i++, current_io_read_function += 0x4,
   current_io_write_function += 0x4)
  {
    current_io_read_function[0x0] = io_buffer_read;
    current_io_read_function[0x1] = io_buffer_read;
    current_io_read_function[0x2] = irq_read_enable;
    current_io_read_function[0x3] = irq_read_status;

    current_io_write_function[0x0] = null_write;
    current_io_write_function[0x1] = null_write;
    current_io_write_function[0x2] = irq_write_enable;
    current_io_write_function[0x3] = irq_write_status;
  }

  // CD-ROM section: 0x1800 to 0x180F, and 0x18Cx
  for(i = 0; i < (0x400 / 0x10); i++, current_io_read_function += 0x10,
   current_io_write_function += 0x10)
  {
    if((i & 0xC) == 0xC)
    {
      current_io_read_function[0x0] = zero_read;
      current_io_read_function[0x1] = aa_read;
      current_io_read_function[0x2] = five_five_read;
      current_io_read_function[0x3] = zero_read;
      current_io_read_function[0x4] = zero_read;
      current_io_read_function[0x5] = aa_read;
      current_io_read_function[0x6] = five_five_read;
      current_io_read_function[0x7] = zero_three_read;
      current_io_read_function[0x8] = zero_read;
      current_io_read_function[0x9] = zero_read;
      current_io_read_function[0xA] = zero_read;
      current_io_read_function[0xB] = zero_read;
      current_io_read_function[0xC] = zero_read;
      current_io_read_function[0xD] = zero_read;
      current_io_read_function[0xE] = zero_read;
      current_io_read_function[0xF] = zero_read;
    }
    else
    {
      if(i < 0x1)
      {
        current_io_read_function[0x0] = cd_bus_status;
        current_io_read_function[0x1] = cd_read_data;
        current_io_read_function[0x2] = cd_read_irq_enable;
        current_io_read_function[0x3] = cd_irq_status;
        current_io_read_function[0x4] = cd_read_reset;
        current_io_read_function[0x5] = cd_read_cdda_low;
        current_io_read_function[0x6] = cd_read_cdda_high;
        current_io_read_function[0x7] = cd_read_sub_channel;
        current_io_read_function[0x8] = cd_read_data_acknowledge;
        current_io_read_function[0x9] = zero_read;
        current_io_read_function[0xA] = adpcm_read;
        current_io_read_function[0xB] = adpcm_dma_status;
        current_io_read_function[0xC] = adpcm_status;
        current_io_read_function[0xD] = adpcm_read_command;
        current_io_read_function[0xE] = zero_read;
        current_io_read_function[0xF] = zero_read;
      }
      else
      {
        current_io_read_function[0x0] = ff_read;
        current_io_read_function[0x1] = ff_read;
        current_io_read_function[0x2] = ff_read;
        current_io_read_function[0x3] = ff_read;
        current_io_read_function[0x4] = ff_read;
        current_io_read_function[0x5] = ff_read;
        current_io_read_function[0x6] = ff_read;
        current_io_read_function[0x7] = ff_read;
        current_io_read_function[0x8] = ff_read;
        current_io_read_function[0x9] = ff_read;
        current_io_read_function[0xA] = ff_read;
        current_io_read_function[0xB] = ff_read;
        current_io_read_function[0xC] = ff_read;
        current_io_read_function[0xD] = ff_read;
        current_io_read_function[0xE] = ff_read;
        current_io_read_function[0xF] = ff_read;
      }
    }

    if(i < 0x1)
    {
      current_io_write_function[0x0] = cd_select;
      current_io_write_function[0x1] = cd_write_data;
      current_io_write_function[0x2] = cd_acknowledge;
      current_io_write_function[0x3] = null_write;
      current_io_write_function[0x4] = cd_write_reset;
      current_io_write_function[0x5] = cd_prepare_cdda_read;
      current_io_write_function[0x6] = cd_prepare_cdda_read;
      current_io_write_function[0x7] = cd_enable_bram;
      current_io_write_function[0x8] = adpcm_write_latch_low;
      current_io_write_function[0x9] = adpcm_write_latch_high;
      current_io_write_function[0xA] = adpcm_write_data;
      current_io_write_function[0xB] = adpcm_enable_dma;
      current_io_write_function[0xC] = null_write;
      current_io_write_function[0xD] = adpcm_write_command;
      current_io_write_function[0xE] = adpcm_playback_rate;
      current_io_write_function[0xF] = cd_fadeout;
    }
    else
    {
      current_io_write_function[0x0] = null_write;
      current_io_write_function[0x1] = null_write;
      current_io_write_function[0x2] = null_write;
      current_io_write_function[0x3] = null_write;
      current_io_write_function[0x4] = null_write;
      current_io_write_function[0x5] = null_write;
      current_io_write_function[0x6] = null_write;
      current_io_write_function[0x7] = null_write;
      current_io_write_function[0x8] = null_write;
      current_io_write_function[0x9] = null_write;
      current_io_write_function[0xA] = null_write;
      current_io_write_function[0xB] = null_write;
      current_io_write_function[0xC] = null_write;
      current_io_write_function[0xD] = null_write;
      current_io_write_function[0xE] = null_write;
      current_io_write_function[0xF] = null_write;
    }
  }

  // And the rest is open bus too
  for(i = 0; i < 0x400; i++, current_io_read_function++,
   current_io_write_function++)
  {
    current_io_read_function[0] = ff_read;
    current_io_write_function[0] = null_write;
  }
}

/*

u8 io_read(u32 address)
{
  switch(address >> 10)
  {
    // VDC (0x0000)
    case 0x0:
      switch(address & 0x3)
      {
        case 0x0:
          return vdc_status();

        case 0x1:
          return zero_read();

        case 0x2:
          return vdc_data_read_low();

        case 0x3:
          return vdc_data_read_high();
      }
      break;

    // VCE (0x0400)
    case 0x1:
      switch(address & 0x7)
      {
        case 0x4:
          return vce_data_read_low();

        case 0x5:
          return vce_data_read_high();

        case 0x0:
        case 0x1:
        case 0x2:
        case 0x3:
        case 0x6:
        case 0x7:
          return ff_read();
      }
      break;

    // PSG (0x0800)
    case 0x2:
      return io_buffer_read();

    // Timer (0x0C00)
    case 0x3:
      switch(address & 0x1)
      {
        case 0x0:
          return timer_value_read();

        case 0x1:
          return timer_enable_read();
      }
      break;

    // I/O port (0x1000)
    case 0x4:
      return io_port_read();

    // Interrupt control (0x1400)
    case 0x5:
      switch(address & 0x3)
      {
        case 0x0:
          return io_buffer_read();

        case 0x1:
          return io_buffer_read();

        case 0x2:
          return irq_read_enable();

        case 0x3:
          return irq_read_status();
      }
      break;

    // CD-ROM (0x1800)
    case 0x6:
    case 0x7:
    {
      if((address & 0x18c0) == 0x18c0)
      {
        switch(address & 0xF)
        {
          case 0x1:
          case 0x5:
            return 0xAA;

          case 0x2:
          case 0x6:
            return 0x55;

          case 0x7:
            return 0x03;

          case 0x0:
          case 0x3:
          case 0x4:
          case 0x8 ... 0xF:
            return 0x00;
        }
      }
      else
      {
        switch(address & 0xF)
        {
          case 0x0:
            return cd_bus_status();

          case 0x1:
            return cd_read_data();

          case 0x2:
            return cd_read_irq_enable();

          case 0x3:
            return cd_irq_status();

          case 0x4:
            return cd_read_reset();

          case 0x5:
            return cd_read_cdda_low();

          case 0x6:
            return cd_read_cdda_high();

          case 0x7:
            return cd_read_sub_channel();

          case 0x8:
            return cd_read_data_acknowledge();

          case 0xA:
            return adpcm_read();

          case 0xB:
            return adpcm_dma_status();

          case 0xC:
            return adpcm_status();

          case 0xD:
            return adpcm_read_command();

          case 0x9:
          case 0xE:
          case 0xF:
            return zero_read();
        }
      }
    }
  }

  return 0;
}

void io_write(u32 address, u32 value)
{
  switch(address >> 10)
  {
    // VDC (0x0000)
    case 0x0:
      switch(address & 0x3)
      {
        case 0x0:
          vdc_register_select(value);
          break;

        case 0x1:
          null_write(value);
          break;

        case 0x2:
          vdc_data_write_low(value);
          break;

        case 0x3:
          vdc_data_write_high(value);
          break;
      }
      break;

    // VCE (0x0400)
    case 0x1:
      switch(address & 0x7)
      {
        case 0x0:
          vce_control_write(value);
          break;

        case 0x2:
          vce_address_write_low(value);
          break;

        case 0x3:
          vce_address_write_high(value);
          break;

        case 0x4:
          vce_data_write_low(value);
          break;

        case 0x5:
          vce_data_write_high(value);
          break;

        case 0x1:
        case 0x6:
        case 0x7:
          null_write(value);
          break;
      }
      break;

    // PSG (0x0800)
    case 0x2:
      switch(address & 0xF)
      {
        case 0x0:
          psg_channel_select(value);
          break;

        case 0x1:
          psg_global_balance(value);
          break;

        case 0x2:
          psg_fine_frequency(value);
          break;

        case 0x3:
          psg_rough_frequency(value);
          break;

        case 0x4:
          psg_channel_control(value);
          break;

        case 0x5:
          psg_channel_balance(value);
          break;

        case 0x6:
          psg_sound_data(value);
          break;

        case 0x7:
          psg_noise_control(value);
          break;

        case 0x8:
          psg_lfo_frequency(value);
          break;

        case 0x9:
          psg_lfo_control(value);
          break;

        default:
          null_write(value);
          break;
      }
      break;

    // Timer (0x0C00)
    case 0x3:
      switch(address & 0x1)
      {
        case 0x0:
          timer_reload_write(value);
          break;

        case 0x1:
          timer_enable_write(value);
          break;
      }
      break;

    // I/O port (0x1000)
    case 0x4:
      io_port_write(value);
      break;

    // Interrupt control (0x1400)
    case 0x5:
      switch(address & 0x3)
      {
        case 0x0:
        case 0x1:
          null_write(value);
          break;

        case 0x2:
          irq_write_enable(value);
          break;

        case 0x3:
          irq_write_status(value);
          break;
      }
      break;

    // Usually CD-ROM (0x1C00)
    case 0x7:
      if(address >= 0x1FF0)
      {
        // Street Fighter 2
        switch(address & 0x3)
        {
          case 0x0:
            map_sf2_0(value);
            break;

          case 0x1:
            map_sf2_1(value);
            break;

          case 0x2:
            map_sf2_2(value);
            break;

          case 0x3:
            map_sf2_3(value);
            break;
        }
        break;
      }

      // Fall through

    // CD-ROM (0x1800)
    case 0x6:
      switch(address & 0xF)
      {
        case 0x0:
          cd_select(value);
          break;

        case 0x1:
          cd_write_data(value);
          break;

        case 0x2:
          cd_acknowledge(value);
          break;

        case 0x3:
          null_write(value);
          break;

        case 0x4:
          cd_write_reset(value);
          break;

        case 0x5:
        case 0x6:
          cd_prepare_cdda_read(value);
          break;

        case 0x7:
          cd_enable_bram(value);
          break;

        case 0x8:
          adpcm_write_latch_low(value);
          break;

        case 0x9:
          adpcm_write_latch_high(value);
          break;

        case 0xA:
          adpcm_write_data(value);
          break;

        case 0xB:
          adpcm_enable_dma(value);
          break;

        case 0xC:
          null_write(value);
          break;

        case 0xD:
          adpcm_write_command(value);
          break;

        case 0xE:
          adpcm_playback_rate(value);
          break;

        case 0xF:
          cd_fadeout(value);
          break;
      }

      break;
  }
}

*/

u8 io_read(u32 address)
{
  return memory.io_read_functions[address]();
}

void io_write(u32 address, u32 value)
{
  memory.io_write_functions[address](value);
}

void map_populous_ram()
{
  u32 i;

  for(i = 0x40; i < 0x44; i++)
  {
    memory_map_set(read, i, memory.hucard_rom + (i * 0x2000));
    memory_map_set(write, i, memory.hucard_rom + (i * 0x2000));
  }
}

void setup_hucard_map()
{
  u32 i;

  // 0x30 page ROMs have strange interleaving
  if(memory.rom_pages == 0x30)
  {
    for(i = 0; i < 0x10; i++)
    {
      memory_map_set(read, i + 0x00, memory.hucard_rom + ((i + 0x00) << 13));
      memory_map_set(read, i + 0x10, memory.hucard_rom + ((i + 0x10) << 13));
      memory_map_set(read, i + 0x20, memory.hucard_rom + ((i + 0x00) << 13));
      memory_map_set(read, i + 0x30, memory.hucard_rom + ((i + 0x10) << 13));
      memory_map_set(read, i + 0x40, memory.hucard_rom + ((i + 0x20) << 13));
      memory_map_set(read, i + 0x50, memory.hucard_rom + ((i + 0x00) << 13));
      memory_map_set(read, i + 0x60, memory.hucard_rom + ((i + 0x10) << 13));
      memory_map_set(read, i + 0x70, memory.hucard_rom + ((i + 0x20) << 13));
    }
  }
  else

  if(memory.rom_pages == 0x40)
  {
    for(i = 0; i < 0x20; i++)
    {
      memory_map_set(read, i + 0x00, memory.hucard_rom + ((i + 0x00) << 13));
      memory_map_set(read, i + 0x20, memory.hucard_rom + ((i + 0x20) << 13));
      memory_map_set(read, i + 0x40, memory.hucard_rom + ((i + 0x20) << 13));
      memory_map_set(read, i + 0x60, memory.hucard_rom + ((i + 0x20) << 13));
    }
  }
  else
  {
    u32 rom_mask = 1;

    while(rom_mask < memory.rom_pages)
      rom_mask <<= 1;

    rom_mask--;

    for(i = 0; i < 0x80; i++)
    {
      memory_map_set(read, i, memory.hucard_rom + ((i & rom_mask) << 13));
    }
  }

  // MPR 7 is initialized to 0
  mpr_write(0x80, 0x00);

  if(memory.sf2_region == -1)
  {
    // Write protect ROM by redirecting writes to null space
    printf("nulling ROM writes\n");
    memory_remap_write_null(0x0, 0x80);

    for(i = 0x1FF0; i < 0x2000; i++)
    {
      memory.io_write_functions[i] = null_write;
    }
  }
  else
  {
    // Capture ROM writes to I/O, for Street Fighter 2
    memory_remap_write_io(0x0, 0x80, memory.io_write_functions);

    for(i = 0x1FF0; i < 0x2000; i += 4)
    {
      memory.io_write_functions[i] = map_sf2_0;
      memory.io_write_functions[i + 1] = map_sf2_1;
      memory.io_write_functions[i + 2] = map_sf2_2;
      memory.io_write_functions[i + 3] = map_sf2_3;
    }
  }

  // Setup ext RAM
  if(config.cd_loaded)
  {
    if(config.cd_system_type >= CD_SYSTEM_TYPE_V3)
    {
      printf("setting up SCD RAM\n");
      for(i = 0x0; i < 0x18; i++)
      {
        memory_map_set(read, i + 0x68, cd.ext_ram + (i * 0x2000));
        memory_map_set(write, i + 0x68, cd.ext_ram + (i * 0x2000));
      }
    }

    // Setup Arcade Card
    if(config.cd_system_type == CD_SYSTEM_TYPE_ACD)
    {
      printf("setting up Arcade Card\n");
      map_arcade_card();
    }
    else
    {
      printf("unmap ACD\n");
      unmap_arcade_card();
    }
  }
  else
  {
    printf("unmap ACD\n");
    unmap_arcade_card();
  }

  // Setup Populous RAM
  if(config.populous_loaded)
    map_populous_ram();
}

void flip_rom()
{
  u32 i;
  u32 current_byte;
  u32 new_byte;

  u32 rom_size = memory.rom_pages * 0x2000;

  // In USA ROMs, the bits in each byte are reversed due to having
  // a different physical pin arrangement.

  printf("Mirroring bits in USA ROM.\n");

  // 0000b -> 0000b
  // 0001b -> 1000b
  // 0010b -> 0100b
  // 0011b -> 1100b
  // 0100b -> 0010b
  // 0101b -> 1010b
  // 0110b -> 0110b
  // 0111b -> 1110b
  // 1000b -> 0001b
  // 1001b -> 1001b
  // 1010b -> 0101b
  // 1011b -> 1101b
  // 1100b -> 0011b
  // 1101b -> 1011b
  // 1110b -> 0111b
  // 1111b -> 1111b

  char bit_reverse_nibble[16] =
  {
    0x0, 0x8, 0x4, 0xC, 0x2, 0xA, 0x6, 0xE,
    0x1, 0x9, 0x5, 0xD, 0x3, 0xB, 0x7, 0xF
  };

  for(i = 0; i < rom_size; i++)
  {
    current_byte = memory.hucard_rom[i];

    new_byte = bit_reverse_nibble[current_byte >> 4];
    new_byte |= bit_reverse_nibble[current_byte & 0xF] << 4;

    memory.hucard_rom[i] = new_byte;
  }
}

s32 load_syscard()
{
  FILE *rom_file;
  u32 file_size;
  u32 header_size;
  u32 rom_size;

  char syscard_path[MAX_PATH];
  char *syscard_name;

  switch(config.cd_system_type)
  {
    case CD_SYSTEM_TYPE_V1:
      syscard_name = "syscard1";
      break;

    case CD_SYSTEM_TYPE_V2:
      syscard_name = "syscard2";
      break;

    default:
    case CD_SYSTEM_TYPE_V3:
    case CD_SYSTEM_TYPE_ACD:
      syscard_name = "syscard3";
      break;

    case CD_SYSTEM_TYPE_GECD:
      syscard_name = "games_express";
      break;
  }

  printf("loading syscard %s\n", syscard_name);

  //sprintf(syscard_path, "%s%csyscards%c%s.bin", config.main_path,
  // DIR_SEPARATOR_CHAR, DIR_SEPARATOR_CHAR, syscard_name);
  sprintf(syscard_path, "/3ds/temperpce_3ds/syscards/%s.bin", syscard_name);

  rom_file = fopen(syscard_path, "rb");

  if(rom_file == NULL)
  {
    //sprintf(syscard_path, "%s%csyscards%c%s.pce", config.main_path,
    // DIR_SEPARATOR_CHAR, DIR_SEPARATOR_CHAR, syscard_name);
    sprintf(syscard_path, "/3ds/temperpce_3ds/syscards/%s.pce", syscard_name);

    printf("could not open syscard, trying %s\n", syscard_path);
    rom_file = fopen(syscard_path, "rb");

    if(rom_file == NULL)
      return -1;
  }

  fseek(rom_file, 0, SEEK_END);
  file_size = ftell(rom_file);

  header_size = file_size & 0x1FFF;
  rom_size = file_size - header_size;

  fseek(rom_file, header_size, SEEK_SET);
  fread(memory.hucard_rom, rom_size, 1, rom_file);
  fclose(rom_file);

  memory.rom_pages = rom_size / 0x2000;

  if(memory.hucard_rom[0x1FFF] < 0xE0)
    flip_rom();

  return 0;
}

void get_bram_path(char *path)
{
  sprintf(path, "%s%c%s.sav", config.main_path,
    DIR_SEPARATOR_CHAR, config.rom_filename);
  /*if(config.per_game_bram)
  {
    sprintf(path, "%s%cbram%c%s.sav", config.main_path,
     DIR_SEPARATOR_CHAR, DIR_SEPARATOR_CHAR, config.rom_filename);
  }
  else
  {
    sprintf(path, "%s%cbram%cbram.sav", config.main_path,
     DIR_SEPARATOR_CHAR, DIR_SEPARATOR_CHAR);
  }*/
}

s32 load_rom(char *path)
{
  FILE *rom_file = fopen(path, "rb");
  char *name_ptr, *dot_ptr;
  u32 file_size;
  u32 header_size;
  u32 rom_size;
  u32 bz2_compressed = 0;
  char path_name[MAX_PATH];
  struct stat sb;
  #ifdef CRC_CHECK
  u32 crc32, result;
  #endif
  u8 i;

  if(config.rom_filename[0])
  {
    get_bram_path(path_name);
    save_bram(path_name);
  }

  dot_ptr = strrchr(path, '.');

  config.cd_loaded = 0;
  config.sgx_mode = 0;
  
  if(dot_ptr != NULL)
  {
    // bin eh? Bet you meant cue, didn't you.
    if(strstr(path, ".bin") || strstr(path, ".iso"))
    {
      if((strlen(path) > 7) && (strcasecmp(dot_ptr - 7, "syscard.bin")))
      {
        printf("Load cue, not bin, stupid.\n");
        strcpy(dot_ptr + 1, "cue");
      }
    }

    if(strstr(path, ".cue"))
    {
      if(load_bin_cue(path) == -1)
        return -1;

      config.cd_loaded = 1;
    }

    if(strstr(path, "sgx"))
      config.sgx_mode = 1;
      
    #ifdef CRC_CHECK
    result = Crc32_ComputeFile(rom_file, &crc32);
    for (i=0;i<6;i++)
    {
		if (crc32 == sgx_table[i])
		{
			config.sgx_mode = 1;
			break;
		}
	}  
	#endif
      
    if(strstr(path, ".bz2"))
    {
      if((strlen(path) > 4) && (strstr(path, ".sgx.bz2")))
        config.sgx_mode = 1;

      bz2_compressed = 1;
    }

    *dot_ptr = 0;
  }

  name_ptr = strrchr(path, DIR_SEPARATOR_CHAR);

  if(name_ptr == NULL)
  {
    strcpy(config.rom_filename, path);
  }
  else
  {
    *name_ptr = 0;
    name_ptr++;
    strcpy(config.rom_filename, name_ptr);
  }

  getcwd(config.rom_directory, MAX_PATH);
  printf("got ROM dir %s, name %s\n", config.rom_directory,
   config.rom_filename);

  if(!config.cd_loaded)
  {
#ifdef BZ_SUPPORT
    if(bz2_compressed)
    {
      int error_code;
      BZFILE *bz_rom_file = BZ2_bzReadOpen(&error_code, rom_file, 0, 1,
       NULL, 0);

      printf("Loading BZ2 compressed ROM.\n");

      file_size = BZ2_bzRead(&error_code, bz_rom_file, memory.hucard_rom,
       1024 * 512 * 5);

      BZ2_bzReadClose(&error_code, bz_rom_file);

      header_size = file_size & 0x1FFF;
      rom_size = file_size - header_size;

      if(header_size != 0)
        memmove(memory.hucard_rom, memory.hucard_rom + header_size, rom_size);
    }
    else
#endif
    {
      fseek(rom_file, 0, SEEK_END);
      file_size = ftell(rom_file);

      header_size = file_size & 0x1FFF;
      rom_size = file_size - header_size;

      fseek(rom_file, header_size, SEEK_SET);
      fread(memory.hucard_rom, rom_size, 1, rom_file);
      fclose(rom_file);
    }

    memory.rom_pages = rom_size / 0x2000;

    //printf("header size: %x, rom size %x (%x pages)\n", header_size, rom_size,
    // memory.rom_pages);
  }

  if(memory.rom_pages > 0x80)
  {
    printf("loading Street Fighter 2\n");
    memory.sf2_region = 0;
  }
  else
  {
    memory.sf2_region = -1;
  }

  if(!memcmp(memory.hucard_rom + 0x1F26, "POPULOUS", strlen("POPULOUS")))
  {
    printf("loading Populous\n");
    config.populous_loaded = 1;
  }
  else
  {
    config.populous_loaded = 0;
  }

  sprintf(path_name, "%s.cfg", config.rom_filename);

  if(load_config_file(path_name) == -1)
    load_config_file("temper.cfg");

  get_bram_path(path_name);
  if(stat(path_name, &sb))
    create_bram(path_name);
  else
    load_bram(path_name);

  if(config.cd_loaded)
  {
    if(load_syscard() == -1)
      return -1;
  }

  // This detects swapped Legend of Hero Tonma ROMs, which have a
  // false positive due to the reset vector being 0xFF00.

  if(((memory.hucard_rom[0x1FFF] < 0xE0) ||
   ((((u16 *)memory.hucard_rom)[0x1FFE / 2] == 0xFF00) &&
   ((memory.hucard_rom[0x1F00]) == 0x1E)))
   && !config.cd_loaded)
  {
    flip_rom();
  }

  // This prevents messes from happening with audio deadlock, hopefully
  audio_unstall_callback();

  return 0;
}


void initialize_memory()
{
  memset(memory.null_space_read, 0xFF, 8 * 1024);
  memset(memory.ext_values, 0, 16 * sizeof(u32));
}

// Call this after a ROM is loaded.

void reset_memory()
{
  u32 i;

  memory_map_set(read, 0, memory.hucard_rom);

  for(i = 0x80; i <= 0x87; i++)
  {
    memory_map_set(read, i, cd.work_ram + ((i - 0x80) << 13));
    memory_map_set(write, i, cd.work_ram + ((i - 0x80) << 13));
  }

  for(i = 0x88; i <= 0xF7; i++)
  {
    memory_map_set(read, i, memory.null_space_read);
    memory_map_set(write, i, memory.null_space_write);
  }

  // Map main RAM.

  if(config.sgx_mode)
  {
    // SGX mode has a full 32KB of RAM
    for(i = 0xF8; i <= 0xFB; i++)
    {
      memory_map_set(read, i, memory.work_ram + ((i - 0xF8) << 13));
      memory_map_set(write, i, memory.work_ram + ((i - 0xF8) << 13));
    }
  }
  else
  {
    // Normal mode just has 8KB of RAM
    for(i = 0xF8; i <= 0xFB; i++)
    {
      memory_map_set(read, i, memory.work_ram);
      memory_map_set(write, i, memory.work_ram);
    }
  }

  for(i = 0xFC; i <= 0xFE; i++)
  {
    memory_map_set(read, i, memory.null_space_read);
    memory_map_set(write, i, memory.null_space_write);
  }

  memory_map_set_ext(read, i, memory.io_read_functions);
  memory_map_set_ext(write, i, memory.io_write_functions);


  // MPR 0-6 are supposed to be initialized with random values,
  // but I don't want something that indeterministic.
  mpr_write(0x7F, 0x80);

  // Actually, Youkai Douchuuki does a jsr before it maps the
  // first page. This is recovered a real machine because reading
  // from I/O causes a BRK, but this is expensive to really emulate
  // correctly, so we'll just give it the mapping to make it work
  // here.

  mpr_write(0x02, 0xF8);

  memset(memory.work_ram, 0x00, 1024 * 32);

  setup_io_tables();
  setup_hucard_map();
}

#define _map_sf2(region)                                                      \
  if(region != memory.sf2_region)                                             \
  {                                                                           \
    memory.sf2_region = region;                                               \
    memory_remap_read(0x40, 0x40, memory.hucard_rom + (0x40 * 0x2000) +       \
     (memory.sf2_region * (0x40 * 0x2000)));                                  \
  }                                                                           \

#define map_sf2_builder(region)                                               \
void map_sf2_##region(u32 value)                                              \
{                                                                             \
  _map_sf2(region);                                                           \
}                                                                             \

map_sf2_builder(0);
map_sf2_builder(1);
map_sf2_builder(2);
map_sf2_builder(3);

void map_sf2(u32 region)
{
  _map_sf2(region);
}


u32 load_mem_safe_f(u32 src)
{
  u32 value;

  load_mem_safe(value, src);

  return value;
}


u32 load_mem_safe_16_f(u32 src)
{
  u32 value;

  load_mem_safe_16(value, src);

  return value;
}

#define memory_savestate_extra_load()                                         \
{                                                                             \
  u32 i;                                                                      \
  u8 *mpr_translated_value;                                                   \
                                                                              \
  for(i = 0; i < 8; i++)                                                      \
  {                                                                           \
    mpr_translated_value = memory.memory_map_read[memory.mpr[i]];             \
    if(mpr_check_ext(mpr_translated_value))                                   \
      mpr_translated_set_read_ext(mpr_translated_value, i);                   \
    else                                                                      \
      mpr_translated_set_read(mpr_translated_value, i);                       \
                                                                              \
    mpr_translated_value = memory.memory_map_write[memory.mpr[i]];            \
    if(mpr_check_ext(mpr_translated_value))                                   \
      mpr_translated_set_write_ext(mpr_translated_value, i);                  \
    else                                                                      \
      mpr_translated_set_write(mpr_translated_value, i);                      \
  }                                                                           \
}                                                                             \

#define memory_savestate_extra_store()                                        \

#define memory_savestate_builder(type, type_b, version_gate)                  \
void memory_##type_b##_savestate(savestate_##type_b##_type savestate_file)    \
{                                                                             \
  if(config.sgx_mode)                                                         \
  {                                                                           \
    file_##type(savestate_file, memory.work_ram, 32 * 1024);                  \
  }                                                                           \
  else                                                                        \
  {                                                                           \
    file_##type(savestate_file, memory.work_ram, 8 * 1024);                   \
  }                                                                           \
  file_##type##_array(savestate_file, memory.mpr);                            \
  file_##type##_variable(savestate_file, memory.io_buffer);                   \
  file_##type##_variable(savestate_file, memory.last_mpr_value);              \
                                                                              \
  memory_savestate_extra_##type_b();                                          \
}                                                                             \

build_savestate_functions(memory);
