// Temper menu code

#include "common.h"

#define menu_line(line)                                                       \
  (line * 8)                                                                  \

#define make_color16(r, g, b)                                                 \
  (b | (g << 5) | (r << 11))                                                  \


void gui_wait_for_input(gui_input_struct *gui_input)
{
  do
  {
    get_gui_input(gui_input);
  } while(gui_input->action_type == CURSOR_NONE);
}

#define FILE_LIST_ROWS ((int)((240 - 40) / 10))
#define FILE_LIST_CHARS 36
#define DIR_LIST_CHARS 16
#define FILE_LIST_POSITION 6
#define DIR_LIST_POSITION (FILE_LIST_POSITION + (FILE_LIST_CHARS * 6) + 1)


int sort_function(const void *dest_str_ptr, const void *src_str_ptr)
{
  char *dest_str = *((char **)dest_str_ptr);
  char *src_str = *((char **)src_str_ptr);

  if(src_str[0] == '.')
    return 1;

  if(dest_str[0] == '.')
    return -1;

  return strcasecmp(dest_str, src_str);
}

#define move_cursor_down()                                                    \
  if(current_column == 0)                                                     \
  {                                                                           \
    if(current_file_selection < (num_files - 1))                              \
    {                                                                         \
      current_file_selection++;                                               \
      if(current_file_in_scroll == (FILE_LIST_ROWS - 1))                      \
        current_file_scroll_value++;                                          \
      else                                                                    \
        current_file_in_scroll++;                                             \
    }                                                                         \
  }                                                                           \
  else                                                                        \
  {                                                                           \
    if(current_dir_selection < (num_dirs - 1))                                \
    {                                                                         \
      current_dir_selection++;                                                \
      if(current_dir_in_scroll == (FILE_LIST_ROWS - 1))                       \
        current_dir_scroll_value++;                                           \
      else                                                                    \
        current_dir_in_scroll++;                                              \
    }                                                                         \
  }                                                                           \

#define move_cursor_up()                                                      \
  if(current_column == 0)                                                     \
  {                                                                           \
    if(current_file_selection)                                                \
    {                                                                         \
      current_file_selection--;                                               \
      if(current_file_in_scroll == 0)                                         \
        current_file_scroll_value--;                                          \
      else                                                                    \
        current_file_in_scroll--;                                             \
    }                                                                         \
  }                                                                           \
  else                                                                        \
  {                                                                           \
    if(current_dir_selection)                                                 \
    {                                                                         \
      current_dir_selection--;                                                \
      if(current_dir_in_scroll == 0)                                          \
        current_dir_scroll_value--;                                           \
      else                                                                    \
        current_dir_in_scroll--;                                              \
    }                                                                         \
  }                                                                           \

const u32 cursor_page_rate = FILE_LIST_ROWS;

#define SEEK_TICKS_THRESHOLD (500 * 1000)
#define SEEK_MAX_CHARACTERS  6

s32 load_file(char **wildcards, char *result, u16 *screen_bg)
{
  DIR *current_dir;
  struct dirent *current_file;
  struct stat file_info;
  char current_dir_name[MAX_PATH];
  char current_dir_short[81];
  u32 current_dir_length;
  u32 total_filenames_allocated;
  u32 total_dirnames_allocated;
  char **file_list;
  char **dir_list;
  u32 num_files;
  u32 num_dirs;
  char *file_name;
  u32 file_name_length;
  u32 ext_pos = -1;
  u32 chosen_file, chosen_dir;
  s32 return_value = 1;
  u32 current_file_selection;
  u32 current_file_scroll_value;
  u32 current_dir_selection;
  u32 current_dir_scroll_value;
  u32 current_file_in_scroll;
  u32 current_dir_in_scroll;
  u32 current_file_number, current_dir_number;
  u32 current_column = 0;
  u32 repeat;
  u32 i;

  char seek_string[SEEK_MAX_CHARACTERS + 1];
  u32 seek_characters = 0;
  u64 last_seek_ticks = 0;

  gui_input_struct gui_input;

  char name_buffer[FILE_LIST_CHARS + 1];
  char *name_buffer_ptr;

  blit_screen(screen_bg);
  set_font_narrow();

  while(return_value == 1)
  {
    current_file_selection = 0;
    current_file_scroll_value = 0;
    current_dir_selection = 0;
    current_dir_scroll_value = 0;
    current_file_in_scroll = 0;
    current_dir_in_scroll = 0;

    total_filenames_allocated = 32;
    total_dirnames_allocated = 32;
    file_list = malloc(sizeof(char *) * 32);
    dir_list = malloc(sizeof(char *) * 32);
    memset(file_list, 0, sizeof(char *) * 32);
    memset(dir_list, 0, sizeof(char *) * 32);

    num_files = 0;
    num_dirs = 0;
    chosen_file = 0;
    chosen_dir = 0;

    getcwd(current_dir_name, MAX_PATH);

    current_dir = opendir(current_dir_name);

    do
    {
      if(current_dir)
        current_file = readdir(current_dir);
      else
        current_file = NULL;

      if(current_file)
      {
        file_name = current_file->d_name;
        file_name_length = strlen(file_name);

        if((stat(file_name, &file_info) >= 0) &&
         ((file_name[0] != '.') || (file_name[1] == '.')))
        {
          if(S_ISDIR(file_info.st_mode))
          {
            dir_list[num_dirs] =
             malloc(file_name_length + 1);

            sprintf(dir_list[num_dirs], "%s", file_name);

            num_dirs++;
          }
          else
          {
            // Must match one of the wildcards, also ignore the .
            if(file_name_length >= 4)
            {
              if(file_name[file_name_length - 4] == '.')
                ext_pos = file_name_length - 4;
              else

              if(file_name[file_name_length - 3] == '.')
                ext_pos = file_name_length - 3;

              else
                ext_pos = 0;

              for(i = 0; wildcards[i] != NULL; i++)
              {
                if(strstr((file_name + ext_pos),
                 wildcards[i]))
                {
                  file_list[num_files] =
                   malloc(file_name_length + 1);

                  sprintf(file_list[num_files], "%s", file_name);

                  num_files++;
                  break;
                }
              }
            }
          }
        }

        if(num_files == total_filenames_allocated)
        {
          file_list = realloc(file_list, sizeof(char *) *
           total_filenames_allocated * 2);
          memset(file_list + total_filenames_allocated, 0,
           sizeof(char *) * total_filenames_allocated);
          total_filenames_allocated *= 2;
        }

        if(num_dirs == total_dirnames_allocated)
        {
          dir_list = realloc(dir_list, sizeof(char *) *
           total_dirnames_allocated * 2);
          memset(dir_list + total_dirnames_allocated, 0,
           sizeof(char *) * total_dirnames_allocated);
          total_dirnames_allocated *= 2;
        }
      }
    } while(current_file);

    qsort((void *)file_list, num_files, sizeof(char *), sort_function);
    qsort((void *)dir_list, num_dirs, sizeof(char *), sort_function);

    closedir(current_dir);

    current_dir_length = strlen(current_dir_name);

    if(current_dir_length > 80)
    {
      snprintf(current_dir_short, 80,
       "...%s", current_dir_name + current_dir_length - 77);
    }
    else
    {
      snprintf(current_dir_short, 80, "%s", current_dir_name);
    }

    repeat = 1;

    if(num_files == 0)
      current_column = 1;

    while(repeat)
    {
      print_string_bg(current_dir_short, make_color16(0x1F, 0x3F, 0x1F),
       screen_bg, 6, 4, RESOLUTION_WIDTH);
      print_string_bg(control_config_exit_string,
       make_color16(0x1F, 0x3F, 0x1F), screen_bg, 20, 228, RESOLUTION_WIDTH);

      for(i = 0, current_file_number = i + current_file_scroll_value;
       i < FILE_LIST_ROWS; i++, current_file_number++)
      {
        if(current_file_number < num_files)
        {
          name_buffer_ptr = file_list[current_file_number];
          if(strlen(name_buffer_ptr) >= FILE_LIST_CHARS)
          {
            memcpy(name_buffer, name_buffer_ptr, FILE_LIST_CHARS);
            name_buffer_ptr = name_buffer;
            name_buffer_ptr[FILE_LIST_CHARS] = 0;
          }

          if((current_file_number == current_file_selection) &&
           (current_column == 0))
          {
            print_string(name_buffer_ptr, make_color16(0x1F, 0x3F, 0x1F),
             make_color16(0x0, 0x0, 0x17), FILE_LIST_POSITION, (i * 10) + 20,
             RESOLUTION_WIDTH);
          }
          else
          {
            print_string_bg(name_buffer_ptr, make_color16(0x1F, 0x3F, 0x1F),
             screen_bg, FILE_LIST_POSITION, (i * 10) + 20, RESOLUTION_WIDTH);
          }
        }
      }

      for(i = 0, current_dir_number = i + current_dir_scroll_value;
       i < FILE_LIST_ROWS; i++, current_dir_number++)
      {
        if(current_dir_number < num_dirs)
        {
          name_buffer_ptr = dir_list[current_dir_number];
          if(strlen(name_buffer_ptr) >= DIR_LIST_CHARS)
          {
            memcpy(name_buffer, name_buffer_ptr, DIR_LIST_CHARS);
            name_buffer_ptr = name_buffer;
            name_buffer_ptr[DIR_LIST_CHARS] = 0;
          }

          if((current_dir_number == current_dir_selection) &&
           (current_column == 1))
          {
            print_string(name_buffer_ptr, make_color16(0x1F, 0x3F, 0x1F),
             make_color16(0x0, 0x0, 0x17), DIR_LIST_POSITION, (i * 10) + 20,
             RESOLUTION_WIDTH);
          }
          else
          {
            print_string_bg(name_buffer_ptr, make_color16(0x1F, 0x3F, 0x1F),
             screen_bg, DIR_LIST_POSITION, (i * 10) + 20, RESOLUTION_WIDTH);
          }
        }
      }

      update_screen();

      gui_wait_for_input(&gui_input);

      switch(gui_input.action_type)
      {
        case CURSOR_DOWN:
          move_cursor_down();
          break;

        case CURSOR_UP:
          move_cursor_up();
          break;

        // Cheap but effective.
        case CURSOR_PAGE_UP:
          for(i = 0; i < cursor_page_rate; i++)
          {
            move_cursor_up();
          }
          break;

        case CURSOR_PAGE_DOWN:
          for(i = 0; i < cursor_page_rate; i++)
          {
            move_cursor_down();
          }
          break;

        case CURSOR_RIGHT:
          if(current_column == 0)
          {
            if(num_dirs != 0)
              current_column = 1;
          }
          break;

        case CURSOR_LEFT:
          if(current_column == 1)
          {
            if(num_files != 0)
              current_column = 0;
          }
          break;

        case CURSOR_SELECT:
          if(current_column == 1)
          {
            repeat = 0;
            chdir(dir_list[current_dir_selection]);
          }
          else
          {
            if(num_files != 0)
            {
              repeat = 0;
              return_value = 0;
              strcpy(result, file_list[current_file_selection]);
            }
          }
          break;

        case CURSOR_BACK:
          repeat = 0;
          chdir("..");
          break;

        case CURSOR_EXIT:
          return_value = -1;
          repeat = 0;
          break;

        case CURSOR_LETTER:
        {
          char **select_list = file_list;
          u32 num_items = num_files;
          u64 current_ticks;

          get_ticks_us(&current_ticks);
          if((current_ticks - last_seek_ticks) > SEEK_TICKS_THRESHOLD)
            seek_characters = 0;

          last_seek_ticks = current_ticks;

          if(seek_characters < SEEK_MAX_CHARACTERS)
          {
            seek_string[seek_characters] = gui_input.key_letter;
            seek_characters++;        

            seek_string[seek_characters + 1] = 0;

            if(current_column == 1)
            {
              select_list = dir_list;
              num_items = num_dirs;
            }

            for(i = 0; i < num_items; i++)
            {
              if(!strncasecmp(select_list[i], seek_string, seek_characters))
                break;
            }

            if(i != num_items)
            {
              s32 scroll_offset = i - (FILE_LIST_ROWS / 2);
              s32 in_scroll = FILE_LIST_ROWS / 2;

              if(scroll_offset < 0)
              {
                scroll_offset = 0;
                in_scroll = i;
              }

              if(current_column == 0)
              {
                current_file_selection = i;
                current_file_scroll_value = scroll_offset;
                current_file_in_scroll = in_scroll;
              }
              else
              {
                current_dir_selection = i;
                current_dir_scroll_value = scroll_offset;
                current_dir_in_scroll = in_scroll;
              }
            }
          }
          break;
        }

        default:
          break;
      }

      blit_screen(screen_bg);
    }

    for(i = 0; i < num_files; i++)
    {
      free(file_list[i]);
    }
    free(file_list);

    for(i = 0; i < num_dirs; i++)
    {
      free(dir_list[i]);
    }
    free(dir_list);
  }

  set_font_wide();
  return return_value;
}

char *pce_ext[] = { ".pce", ".bz2", ".cue", ".sgx", NULL };


void modify_snapshot_bg(menu_state_struct *menu_state,
 menu_option_struct *menu_option)
{
  if(config.rom_filename[0])
  {
    u32 load_state_flags;
    char state_name[MAX_PATH];
    char state_date[256];
    sprintf(state_name, "%s_%d.svs", config.rom_filename,
     config.savestate_number);

    menu_state->current_bg = menu_state->load_state_snapshot_bg;

    load_state_flags = load_state_snapshot(state_name,
     menu_state->load_state_snapshot_bg, state_date);

    if(load_state_flags & SS_EXT_SNAPSHOT)
    {
      buffer_half_intensity(menu_state->current_bg,
       menu_state->current_bg, 320);
    }
    else
    {
      memset(menu_state->current_bg, 0, 320 * 240 * sizeof(u16));
    }

    if(load_state_flags & SS_EXT_INVALID)
      sprintf(menu_state->bg_info_string, "(savestate does not exist)");
    else
      sprintf(menu_state->bg_info_string, "state saved at: %s", state_date);
  }
}


void draw_menu_option_string(menu_state_struct *menu_state,
 menu_option_struct *menu_option, char *display_str, u32 selected)
{
  u32 column_start = menu_state->current_menu->column_start;

  if(selected)
  {
    print_string(display_str, make_color16(0x1F, 0x3F, 0x1F),
     make_color16(0x0, 0x0, 0x17), column_start, menu_option->line_number * 8,
     RESOLUTION_WIDTH);
  }
  else
  {
    print_string_bg(display_str, make_color16(0x1F, 0x3F, 0x1F),
     menu_state->current_bg, column_start, menu_option->line_number * 8,
     RESOLUTION_WIDTH);
  }
}

void draw_menu_option(menu_state_struct *menu_state,
 menu_option_struct *menu_option, u32 selected)
{
  draw_menu_option_string(menu_state, menu_option, menu_option->name,
   selected);
}

void draw_numeric(menu_state_struct *menu_state,
 menu_option_struct *menu_option, u32 selected)
{
  u32 numeric_limit = 1;
  char display_str[256];
  menu_option_numeric_struct *numeric =
   (menu_option_numeric_struct *)menu_option;

  if(numeric->upper_limit > 10000)
    numeric_limit = 5;
  else

  if(numeric->upper_limit > 1000)
    numeric_limit = 4;
  else

  if(numeric->upper_limit > 100)
    numeric_limit = 3;
  else

  if(numeric->upper_limit > 10)
    numeric_limit = 2;

  sprintf(display_str, "%s%*d", menu_option->name, numeric_limit,
   *(numeric->value));
  draw_menu_option_string(menu_state, menu_option, display_str,
   selected);
}

void draw_numeric_labeled(menu_state_struct *menu_state,
 menu_option_struct *menu_option, u32 selected)
{
  char display_str[256];
  menu_option_numeric_struct *numeric =
   (menu_option_numeric_struct *)menu_option;
  menu_option_numeric_labeled_struct *numeric_labeled =
   (menu_option_numeric_labeled_struct *)menu_option;

  sprintf(display_str, "%s%s", menu_option->name,
   numeric_labeled->labels[*(numeric->value)]);
  draw_menu_option_string(menu_state, menu_option, display_str,
   selected);
}

void draw_input_str(menu_state_struct *menu_state,
 menu_option_input_struct *input, u32 draw_offset, u32 selected,
 u32 highlight_current)
{
  u32 y_offset = input->base.line_number * 8;
  u32 space_length = input->num_segments - input->num_segments_active;

  if(selected)
  {
    char spacer_str[256];
    s8 selected_char[2];

    memset(spacer_str, ' ', space_length);
    spacer_str[space_length] = 0;

    print_string(spacer_str, make_color16(0x1F, 0x3F, 0x1F),
     make_color16(0x0, 0x00, 0x17), draw_offset, y_offset, RESOLUTION_WIDTH);

    draw_offset += space_length * 8;

    selected_char[0] = input->segment_values[input->segment_position];
    selected_char[1] = 0;

    print_string((char *)input->segment_values, make_color16(0x1F, 0x3F, 0x1F),
     make_color16(0x0, 0x10, 0x1A), draw_offset, y_offset, RESOLUTION_WIDTH);

    if(highlight_current)
    {
      print_string((char *)selected_char, make_color16(0x1F, 0x3F, 0x1F),
       make_color16(0x0, 0x30, 0x0), draw_offset +
       (input->segment_position * 8), y_offset, RESOLUTION_WIDTH);
    }
  }
  else
  {
    draw_offset += space_length * 8;

    print_string_bg((char *)input->segment_values,
     make_color16(0x1F, 0x3F, 0x1F), menu_state->current_bg, draw_offset,
     y_offset, RESOLUTION_WIDTH);
  }
}

void draw_input(menu_state_struct *menu_state,
 menu_option_struct *menu_option, u32 selected)
{
  menu_option_input_struct *input = (menu_option_input_struct *)menu_option;
  u32 display_input_offset;

  draw_menu_option_string(menu_state, menu_option, menu_option->name,
   selected);

  display_input_offset = strlen(menu_option->name);

  draw_input_str(menu_state, input, menu_state->current_menu->column_start +
   (display_input_offset * 8), selected, 1);
}

void draw_multi_input_int(menu_state_struct *menu_state,
 menu_option_struct *menu_option, u32 selected)
{
  menu_option_multi_input_int_struct *multi_input_int =
   (menu_option_multi_input_int_struct *)menu_option;
  menu_option_input_int_struct *current_input_int;
  u32 display_input_offset;
  s32 i;

  draw_menu_option_string(menu_state, menu_option, menu_option->name,
   selected);

  display_input_offset = strlen(menu_option->name);

  for(i = 0; i < multi_input_int->num_input_ints; i++)
  {
    char separator_str[2];

    separator_str[0] = multi_input_int->separator;
    separator_str[0] = '.';
    separator_str[1] = 0;

    current_input_int = multi_input_int->input_ints[i];

    draw_input_str(menu_state, (menu_option_input_struct *)current_input_int,
     menu_state->current_menu->column_start + (display_input_offset * 8),
     selected, i == multi_input_int->current_input_int);
    display_input_offset += multi_input_int->spacing;

    if(separator_str[0] && (i != multi_input_int->num_input_ints - 1) )
    {
      if(selected)
      {
        print_string(separator_str, make_color16(0x1F, 0x3F, 0x1F),
         make_color16(0x0, 0x0, 0x17), menu_state->current_menu->column_start +
         (display_input_offset * 8),  menu_option->line_number * 8,
         RESOLUTION_WIDTH);
      }
      else
      {
        print_string_bg(separator_str, make_color16(0x1F, 0x3F, 0x1F),
         menu_state->current_bg, menu_state->current_menu->column_start +
         (display_input_offset * 8),  menu_option->line_number * 8,
         RESOLUTION_WIDTH);
      }
      display_input_offset++;
    }
  }
}

u32 action_numeric(menu_state_struct *menu_state,
 menu_option_struct *menu_option, gui_input_struct *gui_input)
{
  menu_option_numeric_struct *numeric =
   (menu_option_numeric_struct *)menu_option;
  s32 value = *(numeric->value);
  u32 numeric_range = (numeric->upper_limit - numeric->lower_limit) + 1;
  s32 value_mod = 0;

  switch(gui_input->action_type)
  {
    case CURSOR_LEFT:
      value_mod = -1;
      break;

    case CURSOR_RIGHT:
      value_mod = 1;
      break;

    case CURSOR_PAGE_DOWN:
      value_mod = -10;
      break;

    case CURSOR_PAGE_UP:
      value_mod = 10;
      break;

    default:
      return gui_input->action_type;
  }

  value += value_mod;

  while(value < numeric->lower_limit)
    value += numeric_range;

  while(value > numeric->upper_limit)
    value -= numeric_range;

  *(numeric->value) = value;

  return CURSOR_NONE;
}

u32 action_select(menu_state_struct *menu_state,
 menu_option_struct *menu_option, gui_input_struct *gui_input)
{
  menu_option_select_struct *select = (menu_option_select_struct *)menu_option;

  switch(gui_input->action_type)
  {
    case CURSOR_SELECT:
      select->select_function(menu_state, menu_option);
      break;

    default:
      return gui_input->action_type;
  }

  return CURSOR_NONE;
}

u32 action_select_menu(menu_state_struct *menu_state,
 menu_option_struct *menu_option, gui_input_struct *gui_input)
{
  menu_option_select_menu_struct *select_menu =
   (menu_option_select_menu_struct *)menu_option;

  switch(gui_input->action_type)
  {
    case CURSOR_SELECT:
    {
      menu_struct *target_menu = select_menu->target_menu;
      menu_struct *current_menu = menu_state->current_menu;
      menu_state->current_menu = target_menu;

      if(current_menu->focus_function)
        current_menu->focus_function(menu_state, target_menu, FOCUS_TYPE_EXIT);

      if(target_menu->focus_function)
        target_menu->focus_function(menu_state, target_menu, FOCUS_TYPE_ENTER);
      break;
    }

    default:
      return gui_input->action_type;
  }

  return CURSOR_NONE;
}

u32 action_numeric_select(menu_state_struct *menu_state,
 menu_option_struct *menu_option, gui_input_struct *gui_input)
{
  menu_option_numeric_select_struct *numeric_select =
   (menu_option_numeric_select_struct *)menu_option;

  u32 new_gui_action = action_numeric(menu_state, menu_option, gui_input);

  switch(gui_input->action_type)
  {
    case CURSOR_LEFT:
    case CURSOR_RIGHT:
    case CURSOR_PAGE_UP:
    case CURSOR_PAGE_DOWN:
    case CURSOR_BACK:
      if(numeric_select->modify_function)
        numeric_select->modify_function(menu_state, menu_option);
      break;

    case CURSOR_SELECT:
      numeric_select->select_function(menu_state, menu_option);
      break;

    default:
      break;
  }

  return new_gui_action;
}

u32 action_input(menu_state_struct *menu_state,
 menu_option_struct *menu_option, gui_input_struct *gui_input)
{
  menu_option_input_struct *input = (menu_option_input_struct *)menu_option;
  s32 segment_position = input->segment_position;
  s32 segment_position_mod = 0;
  s32 segment_value = input->segment_values[segment_position];  
  s32 segment_value_mod = 0;

  switch(gui_input->action_type)
  {
#ifdef KEYBOARD_SUPPORT
    case CURSOR_PAGE_UP:
      segment_value_mod = -1;
      break;
      
    case CURSOR_PAGE_DOWN:
      segment_value_mod = 1;
      break;

    case CURSOR_LEFT:
      segment_position_mod = -1;
      break;

    case CURSOR_RIGHT:
      segment_position_mod = 1;
      break;
#else
    case CURSOR_LEFT:
      segment_value_mod = -1;
      break;
      
    case CURSOR_RIGHT:
      segment_value_mod = 1;
      break;

    case CURSOR_PAGE_UP:
      segment_position_mod = -1;
      break;

    case CURSOR_PAGE_DOWN:
      segment_position_mod = 1;
      break;
#endif

    case CURSOR_BACK:
      if(input->num_segments_active > 1)
      {
        (input->num_segments_active)--;
        if(segment_position < input->num_segments_active)
        {
          memmove(&(input->segment_values[segment_position]),
           &(input->segment_values[segment_position + 1]),
           (input->num_segments_active - segment_position));
        }
        else
        {
          input->segment_position = segment_position - 1;
        }

        if(segment_position)
          segment_position_mod = -1;
      }
      break;

    case CURSOR_SELECT:
      if(input->num_segments_active < input->num_segments)
      {
        memmove(&(input->segment_values[segment_position + 1]),
         &(input->segment_values[segment_position]),
         input->num_segments_active - segment_position);
        (input->num_segments_active)++;

        if(input->segment_insert_value != -1)
        {
          input->segment_values[segment_position + 1] =
           input->segment_insert_value;
        }
        segment_position_mod = 1;
      }
      break;

    case CURSOR_LETTER:
      if((gui_input->key_letter >= input->segment_lower_limit) &&
       (gui_input->key_letter <= input->segment_upper_limit))
      {
        input->segment_values[segment_position] = gui_input->key_letter;
        segment_position_mod = 1;
      }
      break;
      
    default:
      return gui_input->action_type;
  }

  if(segment_value_mod)
  {
    segment_value += segment_value_mod;
    if(segment_value < input->segment_lower_limit)
      segment_value = input->segment_upper_limit;

    if(segment_value > input->segment_upper_limit)
      segment_value = input->segment_lower_limit;

    input->segment_values[segment_position] = segment_value;
  }

  if(segment_position_mod)
  {
    segment_position += segment_position_mod;

    if(input->wrap_position)
    {
      if(segment_position < 0)
        segment_position = input->num_segments_active - 1;

      if(segment_position >= input->num_segments_active)
        segment_position = 0;
    }
    else
    {
      if((segment_position < 0) ||
       (segment_position >= input->num_segments_active))
      {
        return gui_input->action_type;
      }
    }

    input->segment_position = segment_position;
  }

  input->segment_values[input->num_segments_active] = 0;

  return CURSOR_NONE;
}

u32 action_multi_input_int(menu_state_struct *menu_state,
 menu_option_struct *menu_option, gui_input_struct *gui_input)
{
  menu_option_multi_input_int_struct *multi_input_int =
   (menu_option_multi_input_int_struct *)menu_option;
  menu_option_input_int_struct *current_input_int =
   multi_input_int->input_ints[multi_input_int->current_input_int];

  gui_action_type gui_action = action_input(menu_state,
   (menu_option_struct *)current_input_int, gui_input);

  switch(gui_action)
  {
#ifdef KEYBOARD_SUPPORT
    case CURSOR_RIGHT:
#else
    case CURSOR_PAGE_DOWN:
#endif
      if(multi_input_int->current_input_int <
        multi_input_int->num_input_ints - 1)
      {
        (multi_input_int->current_input_int)++;
      }
      break;

#ifdef KEYBOARD_SUPPORT
    case CURSOR_LEFT:
#else
    case CURSOR_PAGE_UP:
#endif
      if(multi_input_int->current_input_int > 0)
        (multi_input_int->current_input_int)--;
      break;

    default:
      return gui_action;
  }

  return CURSOR_NONE;
}

void focus_input_int(menu_state_struct *menu_state,
 menu_option_struct *menu_option, u32 focus_type)
{
  if(focus_type == FOCUS_TYPE_EXIT)
  {
    s32 value;

    menu_option_input_int_struct *input_int =
     (menu_option_input_int_struct *)menu_option;
    sscanf((char *)input_int->segment_values, "%d", &value);

    if(value > input_int->max_value)
      value = input_int->max_value;

    if(value < input_int->min_value)
      value = input_int->min_value;

    if(input_int->zero_pad)
    {
      sprintf((char *)input_int->segment_values, "%0*d",
       input_int->base.num_segments, value);
    }
    else
    {
      sprintf((char *)input_int->segment_values, "%d", value);
    }

    input_int->base.num_segments_active =
     strlen((char *)input_int->segment_values);

    *(input_int->value) = value;
  }
}

void focus_multi_input_int(menu_state_struct *menu_state,
 menu_option_struct *menu_option, u32 focus_type)
{
  if(focus_type == FOCUS_TYPE_EXIT)
  {  
    menu_option_multi_input_int_struct *multi_input_int =
     (menu_option_multi_input_int_struct *)menu_option;
    menu_option_input_int_struct *current_input_int;
    u32 _value = 0;
    u32 i;

    for(i = 0; i < multi_input_int->num_input_ints; i++)
    {
      current_input_int = multi_input_int->input_ints[i];

      ((menu_option_struct *)current_input_int)->focus_function(menu_state,
       (menu_option_struct *)current_input_int, focus_type);

      _value *= multi_input_int->component_base;
      _value += *(current_input_int->value);
    }
    *(multi_input_int->value) = _value;
  }
}


void destroy_multi_input_int(menu_state_struct *menu_state,
 menu_option_struct *menu_option)
{
  menu_option_multi_input_int_struct *multi_input_int =
   (menu_option_multi_input_int_struct *)menu_option;
  menu_option_input_int_struct *current_input_int;
  u32 i;

  for(i = 0; i < multi_input_int->num_input_ints; i++)
  {
    current_input_int = multi_input_int->input_ints[i];
    free(current_input_int);    
  }

  free(multi_input_int->input_ints);
  free(multi_input_int->component_values);
}


void destroy_menu(menu_state_struct *menu_state, menu_struct *menu);

void destroy_select_menu(menu_state_struct *menu_state, menu_option_struct
 *menu_option)
{
  menu_option_select_menu_struct *select_menu =
   (menu_option_select_menu_struct *)menu_option;

  destroy_menu(menu_state, select_menu->target_menu);
}


#define create_allocate(name, type)                                            \
  if(name == NULL)                                                             \
    name = malloc(sizeof(type))                                                \

menu_option_struct *create_menu_option(menu_option_struct *menu_option,
 char *name, u32 line_number, draw_function_type draw_function,
 action_function_type action_function, focus_function_type focus_function,
 destruct_function_type destruct_function)
{
  create_allocate(menu_option, menu_option_struct);
  menu_option->name = name;
  menu_option->line_number = line_number;
  menu_option->draw_function = draw_function;
  menu_option->action_function = action_function;
  menu_option->focus_function = focus_function;
  menu_option->destruct_function = destruct_function;

  return menu_option;
}

menu_option_select_struct *create_select(menu_option_select_struct
 *select, char *name, u32 line_number, select_function_type
 select_function)
{
  create_allocate(select, menu_option_select_struct);
  create_menu_option(&(select->base), name, line_number, draw_menu_option,
   action_select, NULL, NULL);
  
  select->select_function = select_function;
  return select;
}

menu_option_select_menu_struct *create_select_menu(
 menu_option_select_menu_struct *select_menu, char *name, u32 line_number,
 menu_struct *target_menu)
{
  create_allocate(select_menu, menu_option_select_menu_struct);
  create_menu_option(&(select_menu->base), name, line_number, draw_menu_option,
   action_select_menu, NULL, destroy_select_menu);
  select_menu->target_menu = target_menu;

  return select_menu;
}

menu_option_numeric_struct *create_numeric(menu_option_numeric_struct *numeric,
 char *name, u32 line_number, u32 *value, u32 lower_limit, u32 upper_limit)
{
  create_allocate(numeric, menu_option_numeric_struct);
  create_menu_option(&(numeric->base), name, line_number, draw_numeric,
   action_numeric, NULL, NULL);
  numeric->value = value;
  numeric->lower_limit = lower_limit;
  numeric->upper_limit = upper_limit;

  return numeric;
}

menu_option_numeric_select_struct *create_numeric_select(
 menu_option_numeric_select_struct *numeric_select, char *name,
 u32 line_number, u32 *value, u32 lower_limit, u32 upper_limit,
 select_function_type select_function, select_function_type modify_function,
 focus_function_type focus_function)
{
  create_allocate(numeric_select, menu_option_numeric_select_struct);
  create_numeric(&(numeric_select->base), name, line_number, value,
   lower_limit, upper_limit);
 
  numeric_select->base.base.action_function = action_numeric_select; 
  numeric_select->base.base.focus_function = focus_function; 
  numeric_select->select_function = select_function;
  numeric_select->modify_function = modify_function;
  return numeric_select;
}

menu_option_numeric_labeled_struct *create_numeric_labeled(
 menu_option_numeric_labeled_struct *numeric_labeled, char *name,
 u32 line_number, u32 *value, u32 lower_limit, u32 upper_limit, char **labels)
{
  create_allocate(numeric_labeled, menu_option_numeric_labeled_struct);
  menu_option_struct *base = (menu_option_struct *)numeric_labeled;

  create_numeric((menu_option_numeric_struct *)base, name, line_number, value,
   lower_limit, upper_limit);
  numeric_labeled->labels = labels;

  base->draw_function = draw_numeric_labeled;

  return numeric_labeled;
}

menu_option_input_struct *create_input(menu_option_input_struct *input,
 char *name, u32 line_number, s8 *segment_values, u32 segment_lower_limit,
 u32 segment_upper_limit, u32 segment_insert_value, u32 num_segments,
 u32 wrap_position)
{
  create_allocate(input, menu_option_input_struct);
  
  create_menu_option(&(input->base), name, line_number, draw_input,
   action_input, NULL, NULL);
  
  input->segment_values = segment_values;
  input->segment_lower_limit = segment_lower_limit;
  input->segment_upper_limit = segment_upper_limit;
  input->segment_insert_value = segment_insert_value;
  input->wrap_position = wrap_position;

  input->num_segments = num_segments;
  input->num_segments_active = strlen((char *)segment_values);
  input->segment_position = 0;

  return input;
}

menu_option_input_int_struct *create_input_int(menu_option_input_int_struct
 *input_int, char *name, u32 line_number, s32 *value, u32 num_segments,
 s32 min_value, s32 max_value, u32 zero_pad)
{
  create_allocate(input_int, menu_option_input_int_struct);

  if(zero_pad)
    sprintf((char *)input_int->segment_values, "%0*d", num_segments, *value);
  else
    sprintf((char *)input_int->segment_values, "%d", *value);

  create_input(&(input_int->base), name, line_number, input_int->segment_values,
   '0', '9', '0', num_segments, 0);

  input_int->value = value;
  input_int->min_value = min_value;
  input_int->max_value = max_value;
  input_int->zero_pad = zero_pad;

  input_int->base.base.focus_function = focus_input_int;

  return input_int;
}

menu_option_multi_input_int_struct *create_multi_input_int(
 menu_option_multi_input_int_struct *multi_input_int, char *name, 
 u32 line_number, u32 num_input_ints, u32 spacing, u32 separator, s32 *value,
 u32 component_base)
{
  menu_option_input_int_struct *current_input_int;
  s32 _value = *value;
  s32 current_component_value;
  s32 i;

  create_allocate(multi_input_int, menu_option_multi_input_int_struct);

  create_menu_option(&(multi_input_int->base), name, line_number,
   draw_multi_input_int, action_multi_input_int, focus_multi_input_int,
   destroy_multi_input_int);

  multi_input_int->num_input_ints = num_input_ints;
  multi_input_int->spacing = spacing;
  multi_input_int->separator = separator;
  multi_input_int->value = value;
  multi_input_int->component_base = component_base;

  multi_input_int->input_ints =
   malloc(sizeof(menu_option_multi_input_int_struct *) * num_input_ints);
  multi_input_int->component_values = malloc(sizeof(s32) * num_input_ints);
  multi_input_int->current_input_int = 0;

  for(i = num_input_ints - 1; i >= 0; i--)
  {
    current_component_value = _value % component_base;
    
    multi_input_int->component_values[i] = current_component_value;
    current_input_int = create_input_int(NULL, "", line_number,
     &(multi_input_int->component_values[i]), spacing, 0,
     component_base - 1, 1);
    multi_input_int->input_ints[i] = current_input_int;

    _value /= component_base;
  }

  return multi_input_int;
}

void draw_menu_main(menu_state_struct *menu_state, menu_struct *menu)
{
  print_string_bg("Temper version " TEMPER_VERSION,
   make_color16(0x1F, 0x3F, 0x1F), menu_state->current_bg, 70,
   menu_line(4), RESOLUTION_WIDTH);
}

void draw_menu_options(menu_state_struct *menu_state, menu_struct *menu)
{
  print_string_bg("Configure Options", make_color16(0x1F, 0x3F, 0x1F),
   menu_state->current_bg, menu->column_start, menu_line(6),
   RESOLUTION_WIDTH);
}

void draw_menu_pad(menu_state_struct *menu_state, menu_struct *menu)
{
  print_string_bg(control_config_string, make_color16(0x1F, 0x3F, 0x1F),
   menu_state->current_bg, menu->column_start, menu_line(4),
   RESOLUTION_WIDTH);
}

void draw_menu_netplay(menu_state_struct *menu_state, menu_struct *menu)
{
  print_string_bg("Configure Netplay", make_color16(0x1F, 0x3F, 0x1F),
   menu_state->current_bg, menu->column_start, menu_line(8),
   RESOLUTION_WIDTH);
}


void select_load_state(menu_state_struct *menu_state,
 menu_option_struct *menu_option)
{
  if(config.rom_filename[0])
  {
    char state_name[MAX_PATH];
    sprintf(state_name, "%s_%d.svs", config.rom_filename,
     config.savestate_number);

    load_state(state_name, NULL, 0);
    menu_state->exit_menu = 1;
  }
}

void select_save_state(menu_state_struct *menu_state,
 menu_option_struct *menu_option)
{
  if(config.rom_filename[0])
  {
    char state_name[MAX_PATH];
    sprintf(state_name, "%s_%d.svs", config.rom_filename,
     config.savestate_number);

    if((config.snapshot_format) == SS_SNAPSHOT_OFF)
      save_state(state_name, NULL);
    else
      save_state(state_name, menu_state->screen_bg);
  }
}

void select_load_game(menu_state_struct *menu_state,
 menu_option_struct *menu_option)
{
  char file_loaded[MAX_PATH];

  if(load_file(pce_ext, file_loaded, menu_state->current_bg) != -1)
  {
    if(load_rom(file_loaded) != -1)
    {
      reset_pce();
      menu_state->exit_menu = 1;
      menu_state->restore_screen = 0;
    }
  }
}

char *cd_ext[] = { ".cue", NULL };

void select_swap_cd(menu_state_struct *menu_state,
 menu_option_struct *menu_option)
{
  char file_loaded[MAX_PATH];
  if(load_file(cd_ext, file_loaded, menu_state->current_bg) != -1)
  {
    load_bin_cue(file_loaded);
    menu_state->exit_menu = 1;
    menu_state->restore_screen = 0;
  }
}

void focus_savestate(menu_state_struct *menu_state,
 menu_option_struct *menu_option, u32 focus_type)
{
  switch(focus_type)
  {
    case FOCUS_TYPE_ENTER:
      modify_snapshot_bg(menu_state, NULL);
      break;

    case FOCUS_TYPE_EXIT:
      menu_state->bg_info_string[0] = 0;
      menu_state->current_bg = menu_state->screen_bg_quarter;
      break;

    default:
      break;
  }
}

void focus_menu_options(menu_state_struct *menu_state, menu_struct *menu,
 u32 focus_type)
{
  static u32 old_clock_speed;
  static u32 old_ram_timings;
  static u32 old_gamma_percent;
  static u32 old_scale_width;

  if(focus_type == FOCUS_TYPE_ENTER)
  {
    old_ram_timings = config.ram_timings;
    old_clock_speed = config.clock_speed;
    old_gamma_percent = config.gamma_percent;
    old_scale_width = config.scale_width;
  }
  else
  {
#ifdef CONFIG_OPTIONS_CLOCK_SPEED
    if(old_clock_speed != config.clock_speed)
      set_clock_speed(config.clock_speed);
#endif

#ifdef CONFIG_OPTIONS_RAM_TIMINGS
    if(old_ram_timings != config.ram_timings)
    {
      if(config.ram_timings)
        set_fast_ram_timings();
      else
        set_default_ram_timings();
    }
#endif

#ifdef CONFIG_OPTIONS_GAMMA
    if(old_gamma_percent != config.gamma_percent)
      set_gamma(config.gamma_percent);
#endif
  }
}

void focus_menu_pad(menu_state_struct *menu_state, menu_struct *menu,
 u32 focus_type)
{
#if(defined(GP2X_BUILD) || defined(WIZ_BUILD) || defined(PSP_BUILD))
  if(focus_type == FOCUS_TYPE_EXIT)
  {
    u32 i;

    // If nothing's mapped to the menu we have a problem. Better force it
    // for something.

    // This is only an issue for the non-PC version, because it has the
    // M key to fall back on.

    for(i = 0; i < 16; i++)
    {
      if(config.pad[i] == CONFIG_BUTTON_MENU)
        break;
    }

    if(i == 16)
#ifdef GP2X_BUILD
      config.pad[GP2X_CONF_VOL_MID] = CONFIG_BUTTON_MENU;
#else
      config.pad[PSP_CONF_TRIANGLE] = CONFIG_BUTTON_MENU;
#endif
  }
#endif
}

void focus_menu_netplay(menu_state_struct *menu_state,
 menu_struct *menu, u32 focus_type)
{
  if(focus_type == FOCUS_TYPE_ENTER)
  {
  }
  else
  {
  }
}


void select_save_snapshot(menu_state_struct *menu_state,
 menu_option_struct *menu_option)
{
  if(config.rom_filename[0])
    save_screenshot(menu_state->screen_bg);
}

void select_restart(menu_state_struct *menu_state,
 menu_option_struct *menu_option)
{
  if(config.rom_filename[0])
  {
    reset_pce();
    menu_state->exit_menu = 1;
    menu_state->restore_screen = 0;
  }
}

void select_quit(menu_state_struct *menu_state,
 menu_option_struct *menu_option)
{
  quit();
}

void select_return(menu_state_struct *menu_state,
 menu_option_struct *menu_option)
{
  if(config.rom_filename[0])
    menu_state->exit_menu = 1;
}


void select_exit_current_menu(menu_state_struct *menu_state,
 menu_option_struct *menu_option)
{
  menu_struct *current_menu = menu_state->current_menu;

  if(menu_option && (menu_option->focus_function))
    menu_option->focus_function(menu_state, menu_option, FOCUS_TYPE_EXIT);

  if(current_menu->focus_function)
    current_menu->focus_function(menu_state, current_menu, FOCUS_TYPE_EXIT);

  if(current_menu->parent_menu)
  {
    current_menu = current_menu->parent_menu;
    if(current_menu->focus_function)
      current_menu->focus_function(menu_state, current_menu, FOCUS_TYPE_ENTER);
    menu_state->current_menu = current_menu;
  }
}


void select_save_config_local(menu_state_struct *menu_state,
 menu_option_struct *menu_option)
{
  if(config.rom_filename[0])
  {
    char config_file_name[MAX_PATH];
    sprintf(config_file_name, "%s.cfg", config.rom_filename);
    save_config_file(config_file_name);
  }
  select_exit_current_menu(menu_state, NULL);
}

void select_save_config_global(menu_state_struct *menu_state,
 menu_option_struct *menu_option)
{
  save_config_file("temper.cfg");
  select_exit_current_menu(menu_state, NULL);
}


menu_struct *create_menu(u32 num_options, menu_struct *parent_menu,
 draw_menu_function_type draw_function, focus_menu_function_type 
 focus_function)
{
  menu_struct *menu = malloc(sizeof(menu_struct));
  menu->num_options = num_options;
  menu->parent_menu  = parent_menu;
  menu->draw_function = draw_function;
  menu->focus_function = focus_function;

  menu->current_option = 0;
  menu->options = malloc(sizeof(menu_option_struct *) * num_options);

  return menu;
}

void destroy_menu(menu_state_struct *menu_state, menu_struct *menu)
{
  menu_option_struct *current_menu_option;
  u32 i;

  for(i = 0; i < menu->num_options; i++)
  {
    current_menu_option = menu->options[i];
    if(current_menu_option->destruct_function)
      current_menu_option->destruct_function(menu_state, current_menu_option);

    free(current_menu_option);
  }
  free(menu->options);
  free(menu);
}

void draw_menu(menu_state_struct *menu_state, menu_struct *menu)
{
  menu_option_struct *current_option;
  u32 i;

  if(menu->draw_function)
    menu->draw_function(menu_state, menu);

  for(i = 0; i < menu->num_options; i++)
  {
    current_option = menu->options[i];
    current_option->draw_function(menu_state, current_option,
     i == menu->current_option);
  }
}

#define add_menu_option(create_call)                                           \
  menu_options[current_menu_option] = create_call;                             \
  current_menu_option++;                                                       \
  current_line_number++                                                        \

#define add_save_config_options()                                              \
  add_menu_option(create_select(NULL, "Exit: save for all games",              \
   current_line_number, select_save_config_global));                           \
  add_menu_option(create_select(NULL, "Exit: save for this game",              \
   current_line_number,  select_save_config_local));                           \
  add_menu_option(create_select(NULL, "Exit without saving",                   \
   current_line_number, select_exit_current_menu))                             \


menu_struct *create_menu_options(menu_state_struct *menu_state,
 menu_struct *parent_menu)
{
  menu_struct *menu = create_menu(18, parent_menu, draw_menu_options,
   focus_menu_options);
  void **menu_options = (void **)menu->options;
  u32 current_menu_option = 0;
  u32 current_line_number = 8;

  static char *yes_no_labels[] = { " no", "yes" };
  static char *scale_labels[] =
  {
    "fullscreen", "        1x", "        2x", "        3x", "        4x"
  };
  static char *cd_card_labels[] = { "  v1", "  v2", "  v3", " acd", "gecd"  };

  add_menu_option(create_numeric_labeled(NULL, "Scale screen     ",
   current_line_number, &(config.scale_factor), 0, 4, scale_labels));
  add_menu_option(create_numeric_labeled(NULL, "Show fps                ",
   current_line_number, &(config.show_fps), 0, 1, yes_no_labels));
  add_menu_option(create_numeric_labeled(NULL, "Enable sound            ",
   current_line_number, &(config.enable_sound), 0, 1, yes_no_labels));
  add_menu_option(create_numeric_labeled(NULL, "Fast forward            ",
   current_line_number, &(config.fast_forward), 0, 1, yes_no_labels));

#ifdef CONFIG_OPTIONS_CLOCK_SPEED
  add_menu_option(create_numeric(NULL, "Clock speed             ",
   current_line_number, &(config.clock_speed), CLOCK_SPEED_MINIMUM,
   CLOCK_SPEED_MAXIMUM));
#endif

#ifdef CONFIG_OPTIONS_GAMMA
  add_menu_option(create_numeric(NULL, "Gamma percent           ",
   current_line_number, &(config.gamma_percent), 0, 300));
#endif

#ifdef CONFIG_OPTIONS_RAM_TIMINGS
  add_menu_option(create_numeric_labeled(NULL, "Fast RAM timings        ",
   current_line_number, &(config.ram_timings), 0, 1, yes_no_labels));
#endif

  add_menu_option(create_numeric_labeled(NULL, "Patch idle loops        ", 
   current_line_number, &(config.patch_idle_loops), 0, 1, yes_no_labels));
  add_menu_option(create_numeric_labeled(NULL, "Snapshot in saves       ",
   current_line_number, &(config.snapshot_format), 0, 1, yes_no_labels));

#ifdef BZ_SUPPORT
  add_menu_option(create_numeric_labeled(NULL, "BZ2 compressed saves    ",
   current_line_number, &(config.bz2_savestates), 0, 1, yes_no_labels));
#endif

  add_menu_option(create_numeric_labeled(NULL, "Use six button pad      ",
   current_line_number, &(config.six_button_pad), 0, 1, yes_no_labels));
  add_menu_option(create_numeric_labeled(NULL, "CD-ROM System          ",
   current_line_number, &(config.cd_system_type), 0, 4, cd_card_labels));
  add_menu_option(create_numeric_labeled(NULL, "Per-game BRAM saves     ",
   current_line_number, &(config.per_game_bram), 0, 1, yes_no_labels));
  add_menu_option(create_numeric_labeled(NULL, "Scale screen width      ",
   current_line_number, &(config.scale_width), 0, 1, yes_no_labels));
  add_menu_option(create_numeric_labeled(NULL, "Allow >16 spr per line  ",
   current_line_number, &(config.unlimit_sprites), 0, 1, yes_no_labels));

  current_line_number++;

  add_save_config_options();

  menu->num_options = current_menu_option;
  menu->column_start = control_config_start_column;
  return menu;
}

menu_struct *create_menu_pad(menu_state_struct *menu_state,
 menu_struct *parent_menu)
{
  menu_struct *menu = create_menu(platform_control_count + 3, parent_menu,
   draw_menu_pad, focus_menu_pad);
  void **menu_options = (void **)menu->options;
  u32 current_menu_option = 0;
  u32 current_line_number = 6;
  u32 i;

  static char *button_strings[] =
  {
    "          Up", "        Down", "        Left", "       Right",
    "           I", "          II", "         III", "          IV",
    "           V", "          VI", "         Run", "      Select",
    "     Rapid I", "    Rapid II", "   Rapid III", "    Rapid IV",
    "     Rapid V", "    Rapid VI", "        Menu", "  Save state",
    "  Load state", " Volume down", "   Volume up", "Fast forward",
    "        None"
  };

  for(i = 0; i < platform_control_count; i++)
  {
    add_menu_option(create_numeric_labeled(NULL, platform_control_names[i],
     current_line_number, &(config.pad[i]), 0, 24,
     button_strings));
  }

  current_line_number++;

  add_save_config_options();

  menu->column_start = control_config_start_column;
  return menu;
}


menu_struct *create_menu_netplay(menu_state_struct *menu_state,
 menu_struct *parent_menu)
{
  menu_struct *menu = create_menu(8, parent_menu, draw_menu_netplay,
   focus_menu_netplay);
  void **menu_options = (void **)menu->options;
  u32 current_menu_option = 0;
  u32 current_line_number = 10;

  static char *netplay_types[] = { "  none", "server", "client" };

  add_menu_option(create_numeric_labeled(NULL,
   "Netplay type                  ", current_line_number,
   &(config.netplay_type), 0, 2, netplay_types));

  current_line_number++;

  if(config.netplay_username[0] == 0)
  {
    config.netplay_username[0] = ' ';
    config.netplay_username[1] = 0;
  }

  add_menu_option(create_input(NULL, "Username             ",
   current_line_number, (s8 *)config.netplay_username, ' ', '~', -1, 15, 1));

  current_line_number++;

  add_menu_option(create_multi_input_int(NULL, "Server IP Address    ",
   current_line_number, 4, 3, '.', (s32 *)&(config.netplay_ip), 256));
  add_menu_option(create_input_int(NULL, "Connection Port                ",
   current_line_number, (s32 *)&(config.netplay_port), 5, 0, 65535, 0));
  add_menu_option(create_numeric(NULL, "Server Latency                    ",
   current_line_number, &(config.netplay_server_frame_latency), 0, 99));

  current_line_number++;

  add_save_config_options();

  menu->column_start = 16;
  return menu;
}

menu_struct *create_menu_main(menu_state_struct *menu_state) 
{
  menu_struct *menu = create_menu(11, NULL, draw_menu_main, NULL);
  menu_struct *options_menu = create_menu_options(menu_state, menu);
  menu_struct *pad_menu = create_menu_pad(menu_state, menu);
  menu_struct *netplay_menu = create_menu_netplay(menu_state, menu);

  void **menu_options = (void **)menu->options;
  u32 current_menu_option = 0;
  u32 current_line_number = 13;

  add_menu_option(create_select_menu(NULL, "Change options",
   current_line_number, options_menu));
  add_menu_option(create_select_menu(NULL, "Configure pad ", 
   current_line_number, pad_menu));
  add_menu_option(create_select_menu(NULL, "Netplay       ", 
   current_line_number, netplay_menu));

  current_line_number++;

  add_menu_option(create_numeric_select(NULL, "Load state   ",
   current_line_number, &(config.savestate_number), 0, 9, select_load_state,
   modify_snapshot_bg, focus_savestate));
  add_menu_option(create_numeric_select(NULL, "Save state   ",
   current_line_number, &(config.savestate_number), 0, 9, select_save_state,
   modify_snapshot_bg, focus_savestate));
  add_menu_option(create_select(NULL, "Save snapshot ", current_line_number, 
   select_save_snapshot));

  current_line_number++;

  add_menu_option(create_select(NULL, "Load new game ",
   current_line_number, select_load_game));
  add_menu_option(create_select(NULL, "Restart game  ", 
   current_line_number, select_restart));

  current_line_number++;

  add_menu_option(create_select(NULL, "Swap CD       ", 
   current_line_number, select_swap_cd));
  add_menu_option(create_select(NULL, "Return to game",  
   current_line_number, select_return));

  current_line_number++;

  add_menu_option( create_select(NULL, "Exit Temper   ",
   current_line_number, select_quit));

  menu->column_start = 104;

  return menu;
}

void menu(u32 start_file_dialog)
{
  menu_struct *main_menu;
  s32 menu_option_change = 0;

  gui_input_struct gui_input;
  gui_action_type current_action;

  menu_struct *current_menu;
  menu_option_struct *current_menu_option;
  menu_state_struct menu_state;
  u32 audio_pause_state;
  u32 netplay_can_send = netplay.can_send;

  main_menu = create_menu_main(&menu_state);

  menu_state.screen_bg_quarter = malloc(320 * 240 * sizeof(u16));
  menu_state.screen_bg = malloc(320 * 240 * sizeof(u16));
  menu_state.load_state_snapshot_bg = malloc(320 * 240 * sizeof(u16));

  menu_state.current_menu = main_menu;
  menu_state.current_bg = menu_state.screen_bg_quarter;
  menu_state.exit_menu = 0;
  menu_state.restore_screen = 1;
  menu_state.bg_info_string[0] = 0;

  audio_pause_state = audio_pause();

  if(netplay_can_send)
    send_netplay_pause();

  copy_screen(menu_state.screen_bg);
  copy_screen_quarter_intensity(menu_state.screen_bg_quarter);

  if(start_file_dialog)
    select_load_game(&menu_state, NULL);

  while(menu_state.exit_menu == 0)
  {
    current_menu = menu_state.current_menu;

    synchronize();

    blit_screen(menu_state.current_bg);

    set_font_narrow();
    print_string_bg(menu_state.bg_info_string, make_color16(0x14, 0x33, 0x16),
     menu_state.current_bg, 16, menu_line(2), RESOLUTION_WIDTH);
    set_font_wide();

    draw_menu(&menu_state, current_menu);

    current_menu_option = current_menu->options[current_menu->current_option];

    update_screen();

    gui_wait_for_input(&gui_input);
    current_action = CURSOR_NONE;
    if(current_menu_option->action_function)
    {
      current_action = current_menu_option->action_function(&menu_state,
       current_menu_option, &gui_input);
    }

    switch(current_action)
    {
      case CURSOR_UP:
        menu_option_change = -1;
        break;

      case CURSOR_DOWN:
        menu_option_change = 1;
        break;

      case CURSOR_EXIT:
        select_exit_current_menu(&menu_state, current_menu_option);
        break;

      default:
        break;
    }

    if(menu_option_change != 0)
    {
      s32 current_menu_option_number = current_menu->current_option +
       menu_option_change;

      if(current_menu_option->focus_function)
      {
        current_menu_option->focus_function(&(menu_state),
         current_menu_option, FOCUS_TYPE_EXIT);
      }

      if(current_menu_option_number < 0)
        current_menu_option_number = current_menu->num_options - 1;

      if(current_menu_option_number >= current_menu->num_options)
        current_menu_option_number = 0;

      current_menu_option = current_menu->options[current_menu_option_number];
      current_menu->current_option = current_menu_option_number;

      if(current_menu_option->focus_function)
      {
        current_menu_option->focus_function(&(menu_state),
         current_menu_option, FOCUS_TYPE_ENTER);
      }
      menu_option_change = 0;
    }
  }

  audio_revert_pause_state(audio_pause_state);

  if(netplay_can_send && netplay.active)
    send_netplay_unpause();

  // Wait for buttons to be released.
  while(1)
  {
    get_gui_input(&gui_input);
    if(gui_input.action_type == CURSOR_NONE)
      break;
  }
  clear_gui_actions();

  set_multi_buffer_mode();
  clear_all_buffers();

  if(menu_state.restore_screen)
  {
    blit_screen(menu_state.screen_bg);
    update_screen();
  }

  destroy_menu(&menu_state, main_menu);

  free(menu_state.load_state_snapshot_bg);
  free(menu_state.screen_bg_quarter);
  free(menu_state.screen_bg);
  
   #ifdef SDL_TRIPLEBUF
   unsigned char i;
   for(i=0;i<3;i++)
   {
		clear_screen();
		update_screen();
   }
   #endif
}

