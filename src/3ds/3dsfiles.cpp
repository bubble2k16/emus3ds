#include <algorithm>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <vector>

#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <3ds.h>
#include <dirent.h>


#include "3dstypes.h"
#include "3dsfiles.h"


static char currentDir[_MAX_PATH] = "";

static char temp_path[_MAX_PATH] = "";
static char temp_drive[_MAX_PATH] = "";
static char temp_dir[_MAX_PATH] = "";
static char temp_fname[_MAX_PATH] = "";
static char temp_ext[_MAX_PATH] = "";

static char empty_path[2] = "";
static char empty_drive[2] = "";
static char empty_dir[2] = "";
static char empty_fname[2] = "";
static char empty_ext[2] = "";

#define SLASH_STR "/"

//----------------------------------------------------------------------
// Splits the path into different components.
//----------------------------------------------------------------------
void _splitpath (const char *path, char *drive, char *dir, char *fname, char *ext)
{
    if (!path) path = temp_path;
    if (!drive) drive = temp_drive;
    if (!dir) dir = temp_dir;
    if (!fname) fname = temp_fname;
    if (!ext) ext = temp_ext;

	*drive = 0;

	const char	*slash = strrchr(path, SLASH_CHAR),
				*dot   = strrchr(path, '.');

	if (dot && slash && dot < slash)
		dot = NULL;

	if (!slash)
	{
		*dir = 0;

		strcpy(fname, path);

		if (dot)
		{
			fname[dot - path] = 0;
			strcpy(ext, dot + 1);
		}
		else
			*ext = 0;
	}
	else
	{
		strcpy(dir, path);
		dir[slash - path] = 0;

		strcpy(fname, slash + 1);

		if (dot)
		{
			fname[dot - slash - 1] = 0;
			strcpy(ext, dot + 1);
		}
		else
			*ext = 0;
	}
}


//----------------------------------------------------------------------
// Make path from the different components.
//----------------------------------------------------------------------
void _makepath (char *path, const char *, const char *dir, const char *fname, const char *ext)
{
    if (!path) path = empty_path;
    if (!dir) dir = empty_dir;
    if (!fname) fname = empty_fname;
    if (!ext) ext = empty_ext;
    
	if (dir && *dir)
	{
		strcpy(path, dir);
		strcat(path, SLASH_STR);
	}
	else
		*path = 0;

	strcat(path, fname);

	if (ext && *ext)
	{
		strcat(path, ".");
		strcat(path, ext);
	}
}


//----------------------------------------------------------------------
// Initialize the library
//----------------------------------------------------------------------
void file3dsInitialize(void)
{
    getcwd(currentDir, _MAX_PATH);
}


//----------------------------------------------------------------------
// Takes the original file name and replace the extension.
//----------------------------------------------------------------------
const char * file3dsReplaceFilenameExtension (char *fileName, char *extension)
{
	static char	s[_MAX_PATH + 1];
	char		drive[_MAX_PATH + 1], dir[_MAX_PATH + 1], fname[_MAX_PATH + 1], ext[_MAX_PATH + 1];

	_splitpath(fileName, drive, dir, fname, ext);
	snprintf(s, _MAX_PATH + 1, "%s/%s%s", dir, fname, extension);

	return (s);
}

//----------------------------------------------------------------------
// Gets the current directory.
//----------------------------------------------------------------------
char *file3dsGetCurrentDir(void)
{
    return currentDir;
}


//----------------------------------------------------------------------
// Go up to the parent directory.
//----------------------------------------------------------------------
void file3dsGoToParentDirectory(void)
{
    int len = strlen(currentDir);

    if (len > 1)
    {
        for (int i = len - 2; i>=0; i--)
        {
            if (currentDir[i] == '/')
            {
                currentDir[i + 1] = 0;
                break;
            }
        }
    }
}


//----------------------------------------------------------------------
// Go up to the child directory.
//----------------------------------------------------------------------
void file3dsGoToChildDirectory(char *childDir)
{
    strncat(currentDir, childDir, _MAX_PATH);
    strncat(currentDir, "/", _MAX_PATH);
}


//----------------------------------------------------------------------
// Gets the extension of a given file.
//----------------------------------------------------------------------
char *file3dsGetExtension(char *filePath)
{
    int len = strlen(filePath);
    char *extension = &filePath[len];

    for (int i = len - 1; i >= 0; i--)
    {
        if (filePath[i] == '.')
        {
            extension = &filePath[i + 1];
            break;
        }
    }
    return extension;
}


//----------------------------------------------------------------------
// Case-insensitive check for substring.
//----------------------------------------------------------------------
char* stristr( char* str1, const char* str2 )
{
    char* p1 = str1 ;
    const char* p2 = str2 ;
    char* r = *p2 == 0 ? str1 : 0 ;

    while( *p1 != 0 && *p2 != 0 )
    {
        if( tolower( *p1 ) == tolower( *p2 ) )
        {
            if( r == 0 )
            {
                r = p1 ;
            }

            p2++ ;
        }
        else
        {
            p2 = str2 ;
            if( tolower( *p1 ) == tolower( *p2 ) )
            {
                r = p1 ;
                p2++ ;
            }
            else
            {
                r = 0 ;
            }
        }

        p1++ ;
    }

    return *p2 == 0 ? r : 0 ;
}

//----------------------------------------------------------------------
// Load all ROM file names (up to 512 ROMs)
//
// Specify a comma separated list of extensions.
//
//----------------------------------------------------------------------
std::vector<std::string> file3dsGetFiles(char *extensions, int maxFiles)
{
    std::vector<std::string> files;
    char buffer[_MAX_PATH];

    struct dirent* dir;
    DIR* d = opendir(currentDir);

    if (strlen(currentDir) > 1)
    {
        // Insert the parent directory.
        snprintf(buffer, _MAX_PATH, "\x01 ..");   
        files.push_back(buffer);
    }

    if (d)
    {
        while ((dir = readdir(d)) != NULL)
        {
            char *dot = strrchr(dir->d_name, '.');

            if (dir->d_name[0] == '.')
                continue;
            if (dir->d_type == DT_DIR)
            {
                snprintf(buffer, _MAX_PATH, "\x01 %s", dir->d_name);
                files.push_back(buffer);
            }
            if (dir->d_type == DT_REG)
            {
                char *ext = file3dsGetExtension(dir->d_name);

                if (!stristr(extensions, ext))
                    continue;

                files.push_back(dir->d_name);
            }
        }
        closedir(d);
    }

    std::sort(files.begin(), files.end());

    return files;
}