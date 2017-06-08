#ifndef MENU_H
#define MENU_H

typedef enum
{
  MENU_OPTION_FIXED,
  MENU_OPTION_NUMERIC,
  MENU_OPTION_STRING
} menu_option_type_enum;

typedef struct
{
  struct _menu_struct *current_menu;
  u16 *load_state_snapshot_bg;
  u16 *screen_bg;
  u16 *screen_bg_quarter;
  u16 *current_bg;
  u32 exit_menu;
  u32 restore_screen;

  char bg_info_string[256];
} menu_state_struct;

typedef struct _menu_struct menu_struct;
typedef struct _menu_option_struct menu_option_struct;

typedef void (* draw_menu_function_type)(menu_state_struct *menu_state,
 menu_struct *menu);
typedef void (* focus_menu_function_type)(menu_state_struct *menu_state,
 menu_struct *menu, u32 focus_type);

typedef void (* draw_function_type)(menu_state_struct *menu_state,
 menu_option_struct *menu_option, u32 selected);
typedef u32 (* action_function_type)(menu_state_struct *menu_state,
 menu_option_struct *menu_option, gui_input_struct *gui_input);
typedef void (* focus_function_type)(menu_state_struct *menu_state,
 menu_option_struct *menu_option, u32 focus_type);
typedef void (* destruct_function_type)(menu_state_struct *menu_state,
 menu_option_struct *menu_option);

typedef void (* select_function_type)(menu_state_struct *menu_state,
 menu_option_struct *menu_option);

typedef enum
{
  FOCUS_TYPE_ENTER,
  FOCUS_TYPE_EXIT,
} modify_type_enum;

struct _menu_option_struct
{
  char *name;
  u32 line_number;
  draw_function_type draw_function;
  action_function_type action_function;
  focus_function_type focus_function;
  destruct_function_type destruct_function;  
};

typedef struct
{
  menu_option_struct base;
  u32 *value;
  s32 lower_limit;
  s32 upper_limit;
} menu_option_numeric_struct;

typedef struct
{
  menu_option_numeric_struct base;
  char **labels;
} menu_option_numeric_labeled_struct;

typedef struct
{
  menu_option_struct base;
  select_function_type select_function;
} menu_option_select_struct;

typedef struct
{
  menu_option_numeric_struct base;
  select_function_type modify_function;
  select_function_type select_function;
} menu_option_numeric_select_struct;

typedef struct
{
  menu_option_struct base;
  menu_struct *target_menu;
} menu_option_select_menu_struct;

#define MAX_SEGMENTS 16

typedef enum
{
  SEGMENTED_DISPLAY_CHAR,
  SEGMENTED_DISPLAY_DECIMAL,
  SEGMENTED_DISPLAY_HEX,
} menu_option_segmented_display_enum;

#define MAX_SEGMENTS 16

typedef struct
{
  menu_option_struct base;
  s8 *segment_values;
  s32 segment_lower_limit;
  s32 segment_upper_limit;
  s32 segment_insert_value;
  u32 num_segments;
  u32 num_segments_active;
  u32 segment_position;
  u32 wrap_position;
} menu_option_input_struct;

typedef struct
{
  menu_option_input_struct base;
  s8 segment_values[16];
  s32 *value;
  s32 min_value;
  s32 max_value;
  u32 zero_pad;
} menu_option_input_int_struct;

typedef struct
{
  menu_option_struct base;

  u32 num_input_ints;
  u32 spacing;
  char separator;
  s32 *value;
  u32 component_base;

  menu_option_input_int_struct **input_ints;
  s32 *component_values;
  u32 current_input_int;
} menu_option_multi_input_int_struct;

struct _menu_struct
{
  draw_menu_function_type draw_function;
  focus_menu_function_type focus_function;

  u32 column_start;
  u32 num_options;
  u32 current_option;
  u32 exit_option;
  menu_option_struct **options;
  struct _menu_struct *parent_menu;
};

menu_struct *create_menu(u32 num_options, menu_struct *parent_menu,
 draw_menu_function_type draw_function, focus_menu_function_type 
 focus_function);

#define MAX_CONTROLS 128

extern const char *control_config_exit_string;
extern const char *control_config_string;
extern const u32 control_config_start_column;
extern const u32 platform_control_count;
extern char *platform_control_names[MAX_CONTROLS];

#ifdef EXTERN_C_START
EXTERN_C_START
#endif 

menu_struct *create_menu_options(menu_state_struct *menu_state,
 menu_struct *parent_menu);
menu_struct *create_menu_pad(menu_state_struct *menu_state,
 menu_struct *parent_menu);

void select_pad_done(menu_state_struct *menu_state,
 menu_option_struct *menu_option);
void select_options_done(menu_state_struct *menu_state,
 menu_option_struct *menu_option);

void select_save_config_local(menu_state_struct *menu_state,
 menu_option_struct *menu_option);
void select_save_config_global(menu_state_struct *menu_state,
 menu_option_struct *menu_option);

void menu(u32 start_file_dialog);

#ifdef EXTERN_C_END
EXTERN_C_END
#endif 


#endif

