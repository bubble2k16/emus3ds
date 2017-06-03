#ifndef EVENT_H
#define EVENT_H

// These actions are configurable.

typedef enum
{
  CONFIG_BUTTON_UP,
  CONFIG_BUTTON_DOWN,
  CONFIG_BUTTON_LEFT,
  CONFIG_BUTTON_RIGHT,
  CONFIG_BUTTON_I,
  CONFIG_BUTTON_II,
  CONFIG_BUTTON_III,
  CONFIG_BUTTON_IV,
  CONFIG_BUTTON_V,
  CONFIG_BUTTON_VI,
  CONFIG_BUTTON_RUN,
  CONFIG_BUTTON_SELECT,
  CONFIG_BUTTON_RAPID_I ,
  CONFIG_BUTTON_RAPID_II,
  CONFIG_BUTTON_RAPID_III,
  CONFIG_BUTTON_RAPID_IV,
  CONFIG_BUTTON_RAPID_V,
  CONFIG_BUTTON_RAPID_VI,
  CONFIG_BUTTON_MENU,
  CONFIG_BUTTON_SAVE_STATE,
  CONFIG_BUTTON_LOAD_STATE,
  CONFIG_BUTTON_VOLUME_DOWN,
  CONFIG_BUTTON_VOLUME_UP,
  CONFIG_BUTTON_FAST_FORWARD,
  CONFIG_BUTTON_NONE
} config_buttons_enum;

// These actions can't be configured and are triggered by
// fixed inputs.

typedef enum
{
  KEY_ACTION_QUIT,
  KEY_ACTION_BG_OFF,
  KEY_ACTION_SPR_OFF,
  KEY_ACTION_DEBUG_BREAK,
  KEY_ACTION_DEBUG_TILE,
  KEY_ACTION_DEBUG_MAP,
  KEY_ACTION_DEBUG_SPR,
  KEY_ACTION_NETPLAY_TALK,
  KEY_ACTION_NETPLAY_TALK_CURSOR_LEFT,
  KEY_ACTION_NETPLAY_TALK_CURSOR_RIGHT,
  KEY_ACTION_NETPLAY_TALK_CURSOR_BACKSPACE,
  KEY_ACTION_NETPLAY_TALK_CURSOR_ENTER,
  KEY_ACTION_NONE,
} key_action_enum;

typedef enum
{
  CURSOR_UP,
  CURSOR_DOWN,
  CURSOR_LEFT,
  CURSOR_RIGHT,
  CURSOR_SELECT,
  CURSOR_BACK,
  CURSOR_EXIT,
  CURSOR_PAGE_UP,
  CURSOR_PAGE_DOWN,
  CURSOR_LETTER,
  CURSOR_NONE
} gui_action_type;

#endif

// These define autorepeat values (in microseconds), tweak as necessary.

#define BUTTON_REPEAT_START    250000
#define BUTTON_REPEAT_CONTINUE 50000

typedef enum
{
  BUTTON_NOT_HELD,
  BUTTON_HELD_INITIAL,
  BUTTON_HELD_REPEAT
} button_repeat_state_type;

typedef enum
{
  INPUT_ACTION_TYPE_PRESS,
  INPUT_ACTION_TYPE_RELEASE,
  INPUT_ACTION_TYPE_REPEAT
} input_action_type_enum;

typedef enum
{
  HAT_STATUS_UP,
  HAT_STATUS_UP_RIGHT,
  HAT_STATUS_RIGHT,
  HAT_STATUS_DOWN_RIGHT,
  HAT_STATUS_DOWN,
  HAT_STATUS_DOWN_LEFT,
  HAT_STATUS_LEFT,
  HAT_STATUS_UP_LEFT,
  HAT_STATUS_CENTER,
  HAT_STATUS_NONE,
} hat_status_enum;

typedef struct
{
  u32 action_type;
  u32 config_button_action;
  u32 key_action;
  u32 key_letter;
  u32 hat_status;
} event_input_struct;

typedef struct
{
  gui_action_type action_type;
  u32 key_letter;
} gui_input_struct;


#ifdef EXTERN_C_START
EXTERN_C_START
#endif 


void initialize_event(void);
void get_gui_input(gui_input_struct *gui_input);
void clear_gui_actions(void);
void update_events(void);
u32 update_input(event_input_struct *event_input);


#ifdef EXTERN_C_END
EXTERN_C_END
#endif 

