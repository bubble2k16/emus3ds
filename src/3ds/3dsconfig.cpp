
#include "3ds.h"
#include "string.h"
#include "3dsconfig.h"

static FILE    *fp = NULL;
static bool    WriteMode = false;   
static char    fileBuffer[4096];

//----------------------------------------------------------------------
// Opens a config .cfg file.
//----------------------------------------------------------------------
bool config3dsOpenFile(const char *filename, bool fileWriteMode)
{
    if (!fp)
    {
        WriteMode = fileWriteMode;
        fileBuffer[0] = 0;
        if (fileWriteMode)
            fp = fopen(filename, "w+");
        else
            fp = fopen(filename, "r");
        if (fp)
            return true;
        else
            return false;
    }
}


//----------------------------------------------------------------------
// Closes the config file.
//----------------------------------------------------------------------
void config3dsCloseFile()
{
    if (fp)
    {
        if (WriteMode && strlen(fileBuffer) != 0)
        {
            fprintf(fp, fileBuffer);
        }
        fclose(fp);
        fp = NULL;
    }
}



//----------------------------------------------------------------------
// Load / Save an int32 value specific to game.
//----------------------------------------------------------------------
void config3dsReadWriteInt32(char *format, int *value, int minValue, int maxValue)
{
    if (!fp)
        return;
    //if (strlen(format) == 0)
    //    return;

    if (WriteMode)
    {
        char tempBuffer[1024] = { 0 };
        if (value != NULL)
        {
            //printf ("Writing %s %d\n", format, *value);
        	//fprintf(fp, format, *value);
            snprintf(tempBuffer, 1023, format, *value);
        }
        else
        {
            //printf ("Writing %s\n", format);
        	//fprintf(fp, format);
            snprintf(tempBuffer, 1023, format);
        }

        // Flush the text buffer to disk
        //
        if (strlen(fileBuffer) + strlen(tempBuffer) > 4096)
        {
            fprintf(fp, fileBuffer);
            fileBuffer[0] = 0;
        }
        strcat(fileBuffer, tempBuffer);
    }
    else
    {
        if (value != NULL)
        {
            fscanf(fp, format, value);
            if (*value < minValue)
                *value = minValue;
            if (*value > maxValue)
                *value = maxValue;
            //printf ("Scanned %d\n", *value);
        }
        else
        {
            fscanf(fp, format);
            //printf ("skipped line\n");
        }
    }
}


//----------------------------------------------------------------------
// Load / Save a string specific to game.
//----------------------------------------------------------------------
void config3dsReadWriteString(char *writeFormat, char *readFormat, char *value)
{
    if (!fp)
        return;
    
    if (WriteMode)
    {
        char tempBuffer[1024] = { 0 };
        if (value != NULL)
        {
            //printf ("Writing %s %d\n", format, *value);
        	//fprintf(fp, writeFormat, value);
            snprintf(tempBuffer, 1023, writeFormat, value);
            
        }
        else
        {
            //printf ("Writing %s\n", format);
        	//fprintf(fp, writeFormat);
            snprintf(tempBuffer, 1023, writeFormat);
        }

        // Flush the text buffer to disk
        //
        if (strlen(fileBuffer) + strlen(tempBuffer) > 4096)
        {
            fprintf(fp, fileBuffer);
            fileBuffer[0] = 0;
        }
        strcat(fileBuffer, tempBuffer);
        
    }
    else
    {
        if (value != NULL)
        {
            fscanf(fp, readFormat, value);
            char c;
            fscanf(fp, "%c", &c);
            //printf ("Scanned %s\n", value);
        }
        else
        {
            fscanf(fp, readFormat);
            char c;
            fscanf(fp, "%c", &c);
            //fscanf(fp, "%s", dummyString);
            //printf ("skipped line\n");
        }
    }
}

