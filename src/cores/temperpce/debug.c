#include "common.h"

debug_struct debug;

void print_segment(u8 *offset)
{
  u32 i, i2;

  printf("\n    ");
  for(i = 0; i < 16; i++)
  {
    printf("%02x ", i);
  }

  for(i = 0; i < 16; i++)
  {
    printf("\n%02x: ", i * 16);
    for(i2 = 0; i2 < 16; i2++)
    {
      printf("%02x ", offset[(i * 16) + i2]);
    }
  }

  printf("\n");
}

void print_zero_page()
{
  print_segment(mpr_translate_offset(memory.mpr_translated[1], 0x2000));
}

void print_stack()
{
  print_segment(mpr_translate_offset(memory.mpr_translated[1], 0x2100));
}

void print_debug(int a, int x, int y, int p, int s, int pc, u32 remaining)
{
  char disasm[128];
  char flags_str[] = "--------";
  char *flags_set = "nvtbdizc";
  u32 i;
  u32 disasm_pc = pc;

  for(i = 0; i < 8; i++)
  {
    if(p & (0x80 >> i))
      flags_str[i] = flags_set[i];
  }

  disasm_instruction(disasm, &disasm_pc);

  printf("%s\n", disasm);
  printf("a: %02x\n", a);
  printf("x: %02x\ty: %02x\n", x, y);
  printf("s: %02x\tpc: %04x\n", s, pc);
  printf("p: %02x\t%s\n", p, flags_str);

  for(i = 0; i < 8; i++)
  {
    printf("%s ", get_mpr_region_name(i));
  }

  printf("\n%x instructions in (%d cycles until break) (line %d)"
   " (irq %x:%x)",
   debug.instruction_count, remaining, vce.frame_counter,
   irq.status, cpu.irq_raised);
}


u32 parse_commands(char *command, char **commands, u32 max_commands)
{
  char *char_pos = command;
  char *last_char_pos = char_pos;
  u32 command_pos = 1;

  commands[0] = char_pos;

  char_pos = strchr(char_pos, ' ');
  while((char_pos != NULL) && (command_pos < max_commands))
  {
    *char_pos = 0;
    last_char_pos = char_pos + 1;
    commands[command_pos] = char_pos + 1;
    char_pos = strchr(char_pos + 1, ' ');
    command_pos++;
  }

  last_char_pos[strlen(last_char_pos) - 1] = 0;

  return command_pos;
}

#define is_command(command_str, _commands)                                    \
  (!strcmp(commands[0], command_str) && (command_count == (_commands + 1)))   \

#define is_command_var(command_str, _min_commands, _max_commands)             \
  (!strcmp(commands[0], command_str) &&                                       \
   (command_count >= (_min_commands + 1)) &&                                  \
   (command_count <= (_max_commands + 1)))                                    \

#define is_command_hex_num(command_str, _commands)                            \
  (!strncmp(commands[0], command_str, sizeof(command_str) - 1) &&             \
   (command_count == (_commands + 1)) &&                                      \
   isxdigit(commands[0][sizeof(command_str)]))                                \


#define hex_arg(num)                                                          \
  strtol(commands[num + 1], NULL, 16)                                         \

#define lhex_arg(num)                                                         \
  strtoll(commands[num + 1], NULL, 16)                                        \

void step_debug(int a, int x, int y, int p, int s, int pc, int remaining)
{
  // Moved here for a slight speedup, but rearranging the header in the ASM
  // will help more.

  u32 should_break = 0;
  u32 should_print_debug = 0;

  debug.last_pc = debug.current_pc;
  debug.current_pc = pc;

  switch(debug.mode)
  {
    case DEBUG_PC_BREAKPOINT:
      if(pc == debug.breakpoint)
      {
        debug.previous_mode = DEBUG_PC_BREAKPOINT;
        debug.mode = DEBUG_STEP;
        should_print_debug = 1;
        should_break = 1;
      }
      break;

    case DEBUG_COUNTDOWN_BREAKPOINT:
      debug.breakpoint--;
      if(debug.breakpoint == 0)
      {
        debug.previous_mode = DEBUG_COUNTDOWN_BREAKPOINT;
        debug.mode = DEBUG_STEP;
        should_print_debug = 1;
        should_break = 1;
      }
      break;

    case DEBUG_NEXT_FRAME_A:
      if(vdc_a.display_counter != 0)
        debug.mode = DEBUG_NEXT_FRAME_B;
      break;

    case DEBUG_NEXT_FRAME_B:
      if(vdc_a.display_counter == 0)
      {
        debug.breakpoint--;
        if(debug.breakpoint == 0)
        {
          debug.mode = DEBUG_STEP;
          should_print_debug = 1;
          should_break = 1;
        }
        else
        {
          debug.mode = DEBUG_NEXT_FRAME_A;
        }
      }
      break;

    case DEBUG_STEP:
      should_break = 1;
      should_print_debug = 1;
      break;

    case DEBUG_STEP_RUN:
      should_print_debug = 1;
      break;

    case DEBUG_WRITE_BREAKPOINT:
    case DEBUG_RUN:
      break;
  }

  if(should_print_debug)
  {
    print_debug(a, x, y, p, s, pc, remaining);
    printf("\n");
  }

  if(should_break)
  {
    char command[256];
    char *commands[16];
    u32 take_commands = 1;
    u32 command_count;

    do
    {
      audio_pause();

      printf(": ");
      fgets(command, 255, stdin);
      command_count = parse_commands(command, commands, 16);

      if(is_command("break", 1))
      {
        debug.breakpoint_original = debug.breakpoint;
        debug.breakpoint = hex_arg(0);
        debug.mode = DEBUG_PC_BREAKPOINT;
        take_commands = 0;
      }
      else

      if(is_command("skip", 1))
      {
        debug.breakpoint_original = debug.breakpoint;
        debug.breakpoint = hex_arg(0);
        debug.mode = DEBUG_COUNTDOWN_BREAKPOINT;
        take_commands = 0;
      }
      else

      if(is_command_var("next_frame", 0, 1))
      {
        if(command_count > 1)
          debug.breakpoint = hex_arg(0);
        else
          debug.breakpoint = 1;

        if(vdc_a.display_counter == 0)
          debug.mode = DEBUG_NEXT_FRAME_A;
        else
          debug.mode = DEBUG_NEXT_FRAME_B;
        take_commands = 0;
      }
      else

      if(is_command("resume", 0))
      {
        debug.mode = debug.previous_mode;
        take_commands = 0;
      }
      else

      if(is_command("run", 0))
      {
        debug.mode = DEBUG_RUN;
        take_commands = 0;
      }
      else

      if(is_command("step_run", 0))
      {
        debug.mode = DEBUG_STEP_RUN;
        take_commands = 0;
      }
      else

      if(is_command("write_break", 1))
      {
        debug.breakpoint = hex_arg(0);
        debug.mode = DEBUG_WRITE_BREAKPOINT;
        take_commands = 0;
      }
      else

      if(is_command("disasm_function", 1))
      {
        u32 disasm_pc = hex_arg(0);
        disasm_function(disasm_pc);
      }
      else

      if(is_command("disasm_block", 2))
      {
        u32 disasm_pc = hex_arg(0);
        u32 count = hex_arg(1);
        disasm_block(disasm_pc, count);
      }
      else

      if(is_command("zero_page", 0))
      {
        print_zero_page();
      }
      else

      if(is_command("stack", 0))
      {
        print_stack();
      }
      else

      if(is_command("byte_at", 1))
      {
        u32 address = hex_arg(0);
        u32 dest;
        // This won't read I/O, in fact it'll crash (to fix...)
        load_mem_safe(dest, address);
        printf("%02x\n", dest);
      }
      else

      if(is_command("store_byte", 2))
      {
        u32 value = hex_arg(0);
        u32 address = hex_arg(1);

        // This won't store I/O, etc
        store_mem_safe(value, address);
      }
      else

      if(is_command("vram", 2))
      {
        u32 start = hex_arg(0);
        u32 size = hex_arg(1);
        dump_vram(start, size);
      }
      else

      if(is_command("palette", 2))
      {
        u32 start = hex_arg(0);
        u32 size = hex_arg(1);
        dump_palette(start, size);
      }
      else

      if(is_command("sprites", 2))
      {
        u32 start = hex_arg(0);
        u32 size = hex_arg(1);
        dump_spr(start, size);
      }
      else

      if(is_command_var("sprite_mask", 0, 1))
      {
        if(command_count > 1)
        {
          u64 sprite_mask = lhex_arg(0);
          debug.sprite_mask = sprite_mask;
        }
        else
        {
          printf("sprites allowed on: %llx\n", debug.sprite_mask);
        }
      }
      else

      if(is_command("line_sprites", 1))
      {
        u32 line_number = hex_arg(0);
        dump_sprites_per_line(line_number);
      }
      else

      if(is_command("acd_ram", 2))
      {
        u32 start = hex_arg(0);
        u32 size = hex_arg(1);
        dump_acd_ram(start, size);
      }
      else

      if(is_command("mpr_translated_info", 0))
      {
        u32 i;

        for(i = 0; i < 16; i++)
        {
          printf("mpr translated %d: %x (%x, ext %d)\n", i,
           (u32)memory.mpr_translated[i],
           (((u32)memory.mpr_translated[i]) << 1) + (i * 0x2000),
           ((u32)memory.mpr_translated[i]) >> 31);
        }
        printf("memory.work_ram: %p, memory.hucard_ram: %p\n",
         memory.work_ram, memory.hucard_rom);
      }
      else

      if(is_command("cpu_status", 0))
      {
        printf("cpu.a: %01x\n", cpu.a);
        printf("cpu.x: %01x\n", cpu.x);
        printf("cpu.y: %01x\n", cpu.y);
        printf("cpu.s: %01x\n", cpu.s);
        printf("cpu.p: %01x\n", cpu.p);
        printf("cpu.cpu_divider: %01x\n", cpu.cpu_divider);
        printf("cpu.irq_raised: %01x\n", cpu.irq_raised);
        printf("cpu.global_cycles: %08llx\n", cpu.global_cycles);
        printf("cpu.extra_cycles: %02x\n", cpu.extra_cycles);
      }
      else

      if(is_command("video_status", 0))
      {
        dump_video_status();
      }
      else

      if(is_command("irq_status", 0))
      {
        printf("irq.enable: %02x\n", irq.enable);
        printf("irq.status: %02x\n", irq.status);
      }
      else

      if(is_command("cd_status", 0))
      {
        dump_cd_status();
      }
      else

      if(is_command("save", 1))
      {
        save_state(commands[1], NULL);
      }
      else

      if(is_command("load", 1))
      {
        load_state(commands[1], NULL, 0);

        a = cpu.a;
        x = cpu.x;
        y = cpu.y;
        p = cpu.p;
        s = cpu.s;
        pc = cpu.pc;
      }
      else

      if(is_command("state", 0))
      {
        print_debug(a, x, y, p, s, pc, remaining);
        printf("\n");
      }
      else

      if(is_command("q", 0) || is_command("quit", 0))
      {
        quit();
      }
      else

      if(is_command("", 0))
      {
        take_commands = 0;
      }
      else

      if(is_command_hex_num("s", 0))
      {
        sscanf(commands[0], "s%x", &debug.breakpoint);
        debug.breakpoint_original = debug.breakpoint;
        debug.mode = DEBUG_COUNTDOWN_BREAKPOINT;
        take_commands = 0;
      }
    } while(take_commands);

    printf("\n");
    audio_unpause();
  }

  debug.instruction_count++;
}

void set_debug_mode(debug_mode_enum debug_mode)
{
  debug.previous_mode = debug.mode;
  debug.mode = debug_mode;
}

void initialize_debug()
{
  debug.mode = DEBUG_RUN;
  debug.previous_mode = DEBUG_RUN;
}

void reset_debug()
{
  debug.instruction_count = 0;
  debug.current_pc = 0;
}

