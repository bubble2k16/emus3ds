
#include "3dscheat.h"
#include "3dsinterface.h"
#include "stdio.h"
#include "string.h"

int cheatCount = 0;
extern SMenuItem cheatMenu[MAX_CHEATS + 1];

typedef struct
{
    bool    enabled;
    char    name[100];
    char    code[50];
} SCheat;

SCheat cheats[MAX_CHEATS + 1];

//----------------------------------------------------------
// Strips the new line from the string read from fgets.
//----------------------------------------------------------
void cheat3dsStripNewLine(char *s)
{
    int len = strlen(s);
    for (int i = 0; i < len; i++)
    {
        if (s[i] == '\n' || s[i] == '\r')
            s[i] = 0;
    }
}



//----------------------------------------------------------
// Clears all cheats from menu.
//----------------------------------------------------------
void cheat3dsClearCheatsFromMenu()
{
    cheatCount = 0;

    cheatMenu[0] = MENU_MAKE_DISABLED("No cheats available for this game.");
    cheatMenu[1] = MENU_MAKE_LASTITEM();
}

//----------------------------------------------------------
// Adds a cheat to the menu.
//----------------------------------------------------------
void cheat3dsAddCheatToMenu(bool enabled, char *name, char *code)
{
    if (cheatCount >= MAX_CHEATS)
        return;

    cheats[cheatCount].enabled = enabled;
    strncpy(cheats[cheatCount].name, name, 99);    
    strncpy(cheats[cheatCount].code, code, 99);    

    cheatMenu[cheatCount] = MENU_MAKE_CHECKBOX(
        cheatCount + 50000, cheats[cheatCount].name, enabled);

    cheatCount++;

    cheatMenu[cheatCount] = MENU_MAKE_LASTITEM();
}


//----------------------------------------------------------
// This implements a generic text file format (.CHX)
// instead of the emulator's default cheat file formats.
//----------------------------------------------------------
bool cheat3dsLoadCheatTextFile (const char *filename)
{
    cheat3dsClearCheatsFromMenu();

    FILE *fp = fopen (filename, "r");
    if (fp == NULL)
    {
        return false;
    }

    char line[400];
    char *enabled;
    char *code;
    char *name;

    while (!feof(fp))
    {
        uint32 addr;
        uint8 byte;
        
        fgets(line, 399, fp);
        cheat3dsStripNewLine(line);

        enabled = line;

        code = strchr(line, ',');
        if (code == NULL)
            continue;
        *code = 0; code ++;

        name = strchr(code, ',');
        if (name == NULL)
            continue;
        *name = 0; name++;

        bool cheatEnabled = (enabled[0] == 'Y' || enabled[0] == 'y') ? 1 : 0;

        printf ("C: %s, %s, %s\n", enabled, code, name);
        bool addedSuccess = impl3dsAddCheat(cheatEnabled, name, code);
        if (addedSuccess)
            cheat3dsAddCheatToMenu(cheatEnabled, name, code);

        printf ("added success");
    }
    fclose(fp);

    return true;
}



//----------------------------------------------------------
// This saves the cheats back into the file, retaining
// the enabled/disabled states of each cheat.
//----------------------------------------------------------
bool cheat3dsSaveCheatTextFile (const char *filename)
{
    if (cheatCount == 0)
        return false;

    FILE *fp = fopen (filename, "w");
    if (fp == NULL)
        return false;

    for (int i = 0; i < cheatCount; i++)
    {
        fprintf(fp, "%s,%s,%s\n", 
            cheats[i].enabled ? "Y" : "N",
            cheats[i].code,
            cheats[i].name);
    }
    fclose(fp);

    return true;
}


//----------------------------------------------------------
// Sets the enabled flag of the cheat.
//----------------------------------------------------------
void cheat3dsSetCheatEnabledFlag(int cheatIndex, bool enabled)
{
    if (cheatIndex >= cheatCount)
        return;
    
    cheats[cheatIndex].enabled = enabled;
}
