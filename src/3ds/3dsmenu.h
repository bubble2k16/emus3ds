
#ifndef _3DSMENU_H_
#define _3DSMENU_H_

#define MENUITEM_DISABLED           -1
#define MENUITEM_HEADER1            0
#define MENUITEM_HEADER2            1
#define MENUITEM_ACTION             2
#define MENUITEM_CHECKBOX           3
#define MENUITEM_GAUGE              4
#define MENUITEM_PICKER             5
#define MENUITEM_PICKER2            6
#define MENUITEM_LASTITEM           9999

typedef struct
{
    int     Type;               // -1 - Disabled
                                // 0 - Header
                                // 1 - Header 2
                                // 2 - Action 
                                // 3 - Checkbox
                                // 4 - Gauge
                                // 5 - Picker
                                // 6 - Picker 2 (doesn't show the selected item)

    int     ID;                 
    
    char    *Text;

    char    *Description;

    int     Value;              
                                // Type = Gauge:
                                //   Value = Gauge Value
                                // Type = Checkbox:
                                //   0, unchecked
                                //   1, checked
                                // Type = Picker:
                                //   Selected ID of Picker

    int     GaugeMinValue;
    int     GaugeMaxValue;      // Set MinValue < MaxValue to make the gauge visible.

    // All these fields are used if this is a picker.
    // (ID = 100000)
    //
    char    *PickerDescription;
    int     PickerItemCount;
    void    *PickerItems;
    int     PickerBackColor;
} SMenuItem;



//-------------------------------------------------------
// Draw a black screen with a specified opacity.
//-------------------------------------------------------
void menu3dsDrawBlackScreen(float opacity = 1.0f);


//-------------------------------------------------------
// Sets a flag to tell the menu selector
// to transfer the emulator's rendered frame buffer
// to the actual screen's frame buffer.
//
// Usually you will set this to true during emulation,
// and set this to false when this program first runs.
//-------------------------------------------------------
void menu3dsSetTransferGameScreen(bool transfer);


//-------------------------------------------------------
// Sets the sub-title to the tab. Use this to Display
// the current directory the user is in.
//-------------------------------------------------------
void menu3dsSetTabSubTitle(int tabIndex, char *subtitle);


//-------------------------------------------------------
// Adds a new tab to the menu. 
//-------------------------------------------------------
void menu3dsAddTab(char *title, SMenuItem *menuItems);


//-------------------------------------------------------
// Clear and remove all tabs and menus. 
//-------------------------------------------------------
void menu3dsClearMenuTabs();


//-------------------------------------------------------
// Sets the current viewing tab.
//-------------------------------------------------------
void menu3dsSetCurrentMenuTab(int tabIndex);


//-------------------------------------------------------
// Moves the cursor to the item by its ID.
//-------------------------------------------------------
void menu3dsSetSelectedItemIndexByID(int tabIndex, int ID);


//-------------------------------------------------------
// Sets the value of the menu item by its ID.
//-------------------------------------------------------
void menu3dsSetValueByID(int tabIndex, int ID, int value);


//-------------------------------------------------------
// Gets the value of the menu item by its ID.
//-------------------------------------------------------
int menu3dsGetValueByID(int tabIndex, int ID);


//-------------------------------------------------------
// Gets the menu item by its ID.
//-------------------------------------------------------
SMenuItem* menu3dsGetMenuItemByID(int tabIndex, int ID);


//-------------------------------------------------------
// Slides the menu up and runs it until an action is 
// selected, or the user quits by pressing B.
//
// NOTE: You must call menu3dsHideMenu to transit
//       the menu away.
//-------------------------------------------------------
int menu3dsShowMenu(bool (*itemChangedCallback)(int ID, int value), bool animateMenu);


//-------------------------------------------------------
// Hides the menu by sliding it down.
//-------------------------------------------------------
void menu3dsHideMenu();


//-------------------------------------------------------
// Slides the dialog up and runs it until an action is 
// selected, or the user quits by pressing B.
//
// NOTE: You must call menu3dsHideMenu to transit
//       the menu away.
//-------------------------------------------------------
int menu3dsShowDialog(char *title, char *dialogText, int dialogBackColor, SMenuItem *menuItems, int selectedID = -1);


//-------------------------------------------------------
// Hides the dialog by sliding it down.
//-------------------------------------------------------
void menu3dsHideDialog();


//-------------------------------------------------------
// Takes a screenshot and saves it to the specified
// path as a BMP file.
//-------------------------------------------------------
bool menu3dsTakeScreenshot(const char *path);



#define MENU_MAKE_ACTION(ID, text) \
    { MENUITEM_ACTION, ID, text, NULL, 0 }

#define MENU_MAKE_DIALOG_ACTION(ID, text, desc) \
    { MENUITEM_ACTION, ID, text, desc, 0 }

#define MENU_MAKE_DISABLED(text) \
    { MENUITEM_DISABLED, -1, text, NULL }

#define MENU_MAKE_HEADER1(text) \
    { MENUITEM_HEADER1, -1, text, NULL }

#define MENU_MAKE_HEADER2(text) \
    { MENUITEM_HEADER2, -1, text, NULL }

#define MENU_MAKE_CHECKBOX(ID, text, value) \
    { MENUITEM_CHECKBOX, ID, text, NULL, value }

#define MENU_MAKE_GAUGE(ID, text, min, max, value) \
    { MENUITEM_GAUGE, ID, text, NULL, value, min, max }

#define MENU_MAKE_PICKER(ID, text, pickerDescription, pickerOptions, backColor) \
    { MENUITEM_PICKER, ID, text, NULL, 0, 0, 0, pickerDescription, sizeof(pickerOptions)/sizeof(SMenuItem), pickerOptions, backColor }

#define MENU_MAKE_PICKER2(ID, text, pickerDescription, pickerOptions, backColor) \
    { MENUITEM_PICKER2, ID, text, NULL, 0, 0, 0, pickerDescription, sizeof(pickerOptions)/sizeof(SMenuItem), pickerOptions, backColor }

#define MENU_MAKE_LASTITEM() \
    { MENUITEM_LASTITEM }


extern SMenuItem optionsForNoYes[];
extern SMenuItem optionsForOk[];


#define DIALOGCOLOR_RED     0xF44336
#define DIALOGCOLOR_GREEN   0x4CAF50
#define DIALOGCOLOR_CYAN    0x0097A7

#endif
