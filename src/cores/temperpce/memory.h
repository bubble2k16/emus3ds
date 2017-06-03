#ifndef MEMORY_H
#define MEMORY_H

typedef u32 (* io_read_function_ptr)();
typedef void (* io_write_function_ptr)(u32 value);

// 2560KB + 8KB + 8B + 16B + 2KB + 64B + 16KB + 64KB
// 2650KB + 88B

// This is done to make sure that they come after each other in RAM.
typedef struct
{
  // Big for Street Fighter 2 and Arcade Card space
  u8 hucard_rom[1024 * 512 * 5];
  // 32KB for SGX games
  u8 work_ram[1024 * 8 * 4];
  u8 mpr[8];

  u32 io_buffer;
  u32 last_mpr_value;

  // Only save in savestate if it's actually SF2
  s32 sf2_region;

  // Don't save these in a savestate

  u32 rom_pages;

  u8 *memory_map_read[0x100];
  u8 *memory_map_write[0x100];
  u8 *mpr_translated[16];
  // CPU interpreter can store things here and use it as a means of
  // communication, since it'll have to have easy access to mpr_translated.
  u32 ext_values[16];

  // Set this to all 0xFF to emulate open read.
  u8 null_space_read[1024 * 8] __attribute__ ((aligned(4)));

  // Writes just go here and disappear. Nothing can read back from it.
  u8 null_space_write[1024 * 8] __attribute__ ((aligned(4)));

  io_read_function_ptr io_read_functions[8192];
  io_write_function_ptr io_write_functions[8192];
} memory_struct;

extern memory_struct memory;

#ifdef EXTERN_C_START
EXTERN_C_START
#endif 

// At least mpr and io_buffer must be saved in a save state. For now they're
// not in a struct, but this should probably be changed.

char *get_mpr_region_name(u32 mpr_number);
u32 mpr_read(u32 mpr_regs);
void mpr_write(u32 mpr_regs, u32 mpr_value);
u8 io_read(u32 address);
void io_write(u32 address, u32 value);
s32 load_rom(char *path);

void get_bram_path(char *path);

void map_populous_ram();

void null_write(u32 value);
u32 zero_read();
u32 ff_read();

void memory_remap_read(u32 region, u32 size, u8 *ptr);
void memory_remap_read_null(u32 region, u32 size);
void memory_remap_write(u32 region, u32 size, u8 *ptr);
void memory_remap_write_null(u32 region, u32 size);
void memory_remap_write_io(u32 region, u32 size,
 io_write_function_ptr *io_write_function);
void memory_remap_read_write(u32 region, u32 size, u8 *ptr_read,
 u8 *ptr_write);
void memory_remap_read_write_io(u32 region, u32 size,
 io_read_function_ptr *io_read_functions,
 io_write_function_ptr *io_write_functions);
void memory_remap_read_write_null(u32 region, u32 size);

void map_sf2(u32 region);

void map_sf2_0(u32 value);
void map_sf2_1(u32 value);
void map_sf2_2(u32 value);
void map_sf2_3(u32 value);

void memory_load_savestate(savestate_load_type savestate_file);
void memory_store_savestate(savestate_store_type savestate_file);

void initialize_memory();
void reset_memory();

#ifdef EXTERN_C_END
EXTERN_C_END
#endif 

#define load_mem_zp(dest, address)                                            \
  dest = zero_page[address]                                                   \

#define load_mem_zp_16(dest, address)                                         \
{                                                                             \
  u32 dest_low, dest_high;                                                    \
  dest_low = zero_page[address];                                              \
  dest_high = (zero_page[(address + 1) & 0xFF]);                              \
  dest = dest_low | (dest_high << 8);                                         \
}                                                                             \

#define mem_check_alert()                                                     \
  if(cpu.alert)                                                               \
  {                                                                           \
    cpu.alert = 0;                                                            \
    if(cpu.irq_raised)                                                        \
    {                                                                         \
      check_pending_interrupts(irq.status);                                   \
      cpu.irq_raised = 0;                                                     \
    }                                                                         \
    else                                                                      \
    {                                                                         \
      cpu_cycles_remaining = 0;                                               \
      goto cpu_execution_done;                                                \
    }                                                                         \
  }                                                                           \

#define load_mem(dest, address)                                               \
{                                                                             \
  u32 _address = address;                                                     \
  u8 *mpr_translated_value = memory.mpr_translated[_address >> 13];           \
                                                                              \
  if(!mpr_check_ext(mpr_translated_value))                                    \
  {                                                                           \
    dest = mpr_translate(mpr_translated_value, _address);                     \
  }                                                                           \
  else                                                                        \
  {                                                                           \
    mpr_translate_ext_read(mpr_translated_value, _address, dest);             \
                                                                              \
    if((_address & 0x1800) == 0)                                              \
      op_take_cycles(1);                                                      \
                                                                              \
    mem_check_alert();                                                        \
  }                                                                           \
}                                                                             \

#define load_mem_safe(dest, address)                                          \
  dest = mpr_translate(memory.mpr_translated[(address) >> 13], address)       \


// TODO: This can be optimized

#define load_mem_16(dest, address)                                            \
{                                                                             \
  u32 dest_low, dest_high;                                                    \
  load_mem(dest_low, address);                                                \
  load_mem(dest_high, address + 1);                                           \
                                                                              \
  dest = dest_low | (dest_high << 8);                                         \
}                                                                             \

#define load_mem_safe_16(dest, address)                                       \
{                                                                             \
  u32 dest_low, dest_high;                                                    \
  load_mem_safe(dest_low, address);                                           \
  load_mem_safe(dest_high, address + 1);                                      \
                                                                              \
  dest = dest_low | (dest_high << 8);                                         \
}                                                                             \


#define store_mem_zp(src, address)                                            \
  check_write_breakpoint(src, (address + 0x2000));                            \
  zero_page[address] = src                                                    \

#define store_mem(src, address)                                               \
{                                                                             \
  u32 _address = address;                                                     \
  u8 *mpr_translated_value = memory.mpr_translated[0x8 + (_address >> 13)];   \
  check_write_breakpoint(src, _address);                                      \
  cpu.cycles_remaining = cpu_cycles_remaining;                                \
                                                                              \
  if(!mpr_check_ext(mpr_translated_value))                                    \
  {                                                                           \
    mpr_translate(mpr_translated_value, _address) = src;                      \
  }                                                                           \
  else                                                                        \
  {                                                                           \
    mpr_translate_ext_write(mpr_translated_value, _address, src);             \
                                                                              \
    if((_address & 0x1800) == 0)                                              \
      op_take_cycles(1);                                                      \
                                                                              \
    mem_check_alert();                                                        \
  }                                                                           \
}                                                                             \

#define store_mem_safe(src, address)                                          \
  mpr_translate(memory.mpr_translated[(address) >> 13], address) = src        \


#define rmw_mem(var, address, operation, extra)                               \
{                                                                             \
  u32 _address = address;                                                     \
  u8 *mpr_translated_value = memory.mpr_translated[_address >> 13];           \
  check_write_breakpoint(var, _address);                                      \
  cpu.cycles_remaining = cpu_cycles_remaining;                                \
                                                                              \
  if(!mpr_check_ext(mpr_translated_value))                                    \
  {                                                                           \
    var = mpr_translate(mpr_translated_value, _address);                      \
    operation(var, extra);                                                    \
    mpr_translate(mpr_translated_value, address) = var;                       \
  }                                                                           \
  else                                                                        \
  {                                                                           \
    mpr_translate_ext_read(mpr_translated_value, _address, var);              \
    operation(var, extra);                                                    \
    mpr_translate_ext_write(mpr_translated_value + (0x1000 * sizeof(void *)), \
     _address, var);                                                          \
                                                                              \
    if((_address & 0x1800) == 0)                                              \
      op_take_cycles(2);                                                      \
                                                                              \
    mem_check_alert();                                                        \
  }                                                                           \
}                                                                             \

#ifdef __LP64__

  #define memory_map_set(type, offset, ptr)                                   \
    memory.memory_map_##type[offset] = (u8 *)((u64)(ptr) >> 1)                \
  
  #define memory_map_get(type, offset)                                        \
    (u8 *)((u64)memory.memory_map_##type[offset] << 1)                        \

  #define memory_map_check_ext(type, offset)                                  \
    ((u64)memory.memory_map_##type[offset] & 0x8000000000000000ULL)           \
  
  #define memory_map_set_ext(type, offset, ptr)                               \
    memory.memory_map_##type[offset] =                                        \
     (u8 *)(((u64)(ptr) >> 1) | 0x8000000000000000ULL)                        \
  
  #define mpr_check_ext(mpr_value)                                            \
    ((u64)mpr_value & 0x8000000000000000ULL)                                  \
  
  #define mpr_translate(_mpr_translated_value, __address)                     \
    (((u8 *)((u64)_mpr_translated_value << 1))[__address])                    \
  
  #define mpr_translate_offset(_mpr_translated_value, __address)              \
    (((u8 *)((u64)_mpr_translated_value << 1)) + (__address))                 \
  
  #define mpr_translate_ext_read(_mpr_translated_value, __address, dest)      \
    dest = ((io_read_function_ptr *)((u64)(_mpr_translated_value)             \
     << 1))[__address]()                                                      \
  
  #define mpr_translate_ext_write(_mpr_translated_value, __address, src)      \
    (((io_write_function_ptr *)((u64)(_mpr_translated_value)                  \
     << 1))[__address])(src)                                                  \
  
  #define mpr_translate_ext_offset(_mpr_translated_value, __address)          \
    (((u64)(_mpr_translated_value) << 1) + (__address * sizeof(void *)))      \

  #define mpr_translated_set_read_ext(mpr_value, offset)                      \
    memory.mpr_translated[offset] = (mpr_value) - ((offset) << 15)            \
  
  #define mpr_translated_set_write_ext(mpr_value, offset)                     \
    memory.mpr_translated[(offset + 0x8)] = (mpr_value) - ((offset) << 15)    \

#else

  #define memory_map_set(type, offset, ptr)                                   \
    memory.memory_map_##type[offset] = (u8 *)((u32)(ptr) >> 1)                \
  
  #define memory_map_get(type, offset)                                        \
    (u8 *)((u32)memory.memory_map_##type[offset] << 1)                        \
  
  #define memory_map_check_ext(type, offset)                                  \
    ((u32)memory.memory_map_##type[offset] & 0x80000000)                      \
  
  #define memory_map_set_ext(type, offset, ptr)                               \
    memory.memory_map_##type[offset] = (u8 *)(((u32)(ptr) >> 1) | 0x80000000) \
  
  #define mpr_check_ext(mpr_value)                                            \
    ((u32)mpr_value & 0x80000000)                                             \
  
  #define mpr_translate(_mpr_translated_value, __address)                     \
    (((u8 *)((u32)_mpr_translated_value << 1))[__address])                    \
  
  #define mpr_translate_offset(_mpr_translated_value, __address)              \
    (((u8 *)((u32)_mpr_translated_value << 1)) + (__address))                 \
  
  #define mpr_translate_ext_read(_mpr_translated_value, __address, dest)      \
    dest = ((io_read_function_ptr *)((u32)(_mpr_translated_value)             \
      << 1))[__address]()                                                     \
  
  #define mpr_translate_ext_write(_mpr_translated_value, __address, src)      \
    (((io_write_function_ptr *)((u32)(_mpr_translated_value)                  \
     << 1))[__address])(src)                                                  \
  
  #define mpr_translate_ext_offset(_mpr_translated_value, __address)          \
    (((u32)(_mpr_translated_value) << 1) + (__address * sizeof(void *)))      \

  #define mpr_translated_set_read_ext(mpr_value, offset)                      \
    memory.mpr_translated[offset] = (mpr_value) - ((offset) << 14)            \
  
  #define mpr_translated_set_write_ext(mpr_value, offset)                     \
    memory.mpr_translated[(offset + 0x8)] = (mpr_value) - ((offset) << 14)    \

#endif


#define mpr_translated_set_read(mpr_value, offset)                            \
  memory.mpr_translated[offset] = (mpr_value) - ((offset) << 12)              \

#define mpr_translated_set_write(mpr_value, offset)                           \
  memory.mpr_translated[(offset + 0x8)] = (mpr_value) - ((offset) << 12)      \


#endif

