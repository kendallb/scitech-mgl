/****************************************************************************
*
*                   SciTech Multi-platform Graphics Library
*
*  ========================================================================
*
*   Copyright (C) 1991-2004 SciTech Software, Inc. All rights reserved.
*
*   This file may be distributed and/or modified under the terms of the
*   GNU General Public License version 2.0 as published by the Free
*   Software Foundation and appearing in the file LICENSE.GPL included
*   in the packaging of this file.
*
*   Licensees holding a valid Commercial License for this product from
*   SciTech Software, Inc. may use this file in accordance with the
*   Commercial License Agreement provided with the Software.
*
*   This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING
*   THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR
*   PURPOSE.
*
*   See http://www.scitechsoft.com/license/ for information about
*   the licensing options available and how to purchase a Commercial
*   License Agreement.
*
*   Contact license@scitechsoft.com if any conditions of this licensing
*   are not clear to you, or you have questions about licensing options.
*
*  ========================================================================
*
* Language:     ANSI C
* Environment:  Any
*
* Description:  File handling functions.
*
****************************************************************************/

#include "mgl.h"

/*--------------------------- Global Variables ----------------------------*/

/* Callbacks for MGL file I/O functions */

FILE *  (*__MGL_fopen)(const char *filename,const char *mode)           = fopen;
int     (*__MGL_fclose)(FILE *f)                                        = fclose;
int     (*__MGL_fseek)(FILE *f,long offset,int whence)                  = fseek;
long    (*__MGL_ftell)(FILE *f)                                         = ftell;
size_t  (*__MGL_fread)(void *ptr,size_t size,size_t n,FILE *f)          = fread;
size_t  (*__MGL_fwrite)(const void *ptr,size_t size,size_t n,FILE *f)   = fwrite;

/*------------------------- Implementation --------------------------------*/

/****************************************************************************
PARAMETERS:
f   - Open file to determine the size of

RETURNS:
Length of the file in bytes.

REMARKS:
Determines the length of the file, without altering the current position
in the file.
{secret}
****************************************************************************/
long _MGL_fileSize(
    FILE *f)
{
    long    size,oldpos = __MGL_ftell(f);

    __MGL_fseek(f,0,SEEK_END);      /* Seek to end of file              */
    size = __MGL_ftell(f);          /* Determine the size of the file   */
    __MGL_fseek(f,oldpos,SEEK_SET); /* Seek to old position in file     */
    return size;                    /* Return the size of the file      */
}

/****************************************************************************
DESCRIPTION:
Restricts the output from the display device context to a specified output
region.

HEADER:
mgraph.h

PARAMETERS:
dir     - MGL directory to find the find under
name    - Name of the file to open

REMARKS:
Attempts to open the specified MGL file in binary mode. This routine will
use the standard MGL directory searching algorithm to find the specified
file. First an attempt is made to locate the file in the current directory.
Then we search in the directory relative to the current directory. If
that fails, we then search relative to the _MGL_path variable (initialized
by the application program via the MGL_init call). If this fails, an attempt
is made to search for the file relative to the MGL_ROOT environment variable
if this is present.
{secret}
****************************************************************************/
ibool MGLAPI _MGL_findFile(
    char *validpath,
    const char *dir,
    const char *name,
    const char *mode)
{
    FILE    *f;

    /* First try opening the file with just the file name (checking for
     * local directories and an explicit file path).
     */
    strcpy(validpath,name);
    if ((f = __MGL_fopen(validpath,mode)) == NULL) {
        strcpy(validpath, dir);
        PM_backslash(validpath);
        strcat(validpath, name);
        if ((f = __MGL_fopen(validpath,mode)) == NULL) {
            validpath[0] = '\0';
            if (_MGL_path[0] != '\0') {
                strcpy(validpath, _MGL_path);
                PM_backslash(validpath);
                }
            strcat(validpath, dir);
            PM_backslash(validpath);
            strcat(validpath, name);
            if ((f = __MGL_fopen(validpath,mode)) == NULL) {
                /* Search the environment variable */
                if (getenv(MGL_ROOT)) {
                    strcpy(validpath, getenv(MGL_ROOT));
                    PM_backslash(validpath);
                    strcat(validpath, dir);
                    PM_backslash(validpath);
                    strcat(validpath, name);
                    f = __MGL_fopen(validpath, mode);
                    }
                }
            }
        }
    if (f)
        __MGL_fclose(f);
    return (f != NULL);
}

/****************************************************************************
PARAMETERS:
dir     - MGL directory to open the find under
name    - Name of the file to open
mode    - Mode passed to fopen to open the file

RETURNS:
Pointer to the opened file, or NULL if not found.
{secret}
****************************************************************************/
FILE * _MGL_openFile(
    const char *dir,
    const char *name,
    const char *mode)
{
    char    validpath[PM_MAX_PATH];

    if (_MGL_findFile(validpath,dir,name,mode))
        return __MGL_fopen(validpath,mode);
    return NULL;
}

/****************************************************************************
DESCRIPTION:
Overrides the default file I/O functions used by MGL.

HEADER:
mgraph.h

PARAMETERS:
fio - Structure containing new file I/O functions

REMARKS:
This function allows the programmer to override the default file I/O functions used
by all the MGL functions that access files (bitmap, font, icon and cursor loading).
By default the standard C I/O functions are used and you can reset back to the
standard C I/O functions by calling this function with the fio parameter set to
NULL.

This function is useful for creating your own file system, such as storing all the
bitmaps, fonts and icons that your application requires in a large file of your own
format. This way end users browsing your program's data files will not be able to
view any of the data (game developers may wish to keep the bitmaps used for levels
in the game secret to make it harder for the user to cheat when playing the game).

This function allows you to overload the fopen, fclose, fseek, ftell, fread and fwrite
functions used by MGL. See the fileio_t structure for more information.

****************************************************************************/
void MGLAPI MGL_setFileIO(
    fileio_t *fio)
{
    if (fio) {
        __MGL_fopen     = fio->fopen;
        __MGL_fclose    = fio->fclose;
        __MGL_fseek     = fio->fseek;
        __MGL_ftell     = fio->ftell;
        __MGL_fread     = fio->fread;
        __MGL_fwrite    = fio->fwrite;
        }
    else {
        __MGL_fopen     = fopen;
        __MGL_fclose    = fclose;
        __MGL_fseek     = fseek;
        __MGL_ftell     = ftell;
        __MGL_fread     = fread;
        __MGL_fwrite    = fwrite;
        }
}

/****************************************************************************
DESCRIPTION:
Opens a stream.

HEADER:
mgraph.h

PARAMETERS:
filename    - Filename
mode        - Mode to open file in.

RETURNS:
Pointer to newly opened stream, or NULL in the event of an error.

REMARKS:
This function is identical to the C library fopen function, but goes via
the MGL's internal file handling function pointers, which by default simply
points to the standard C library functions. These functions are intended
to allow the application programmer to override all the MGL file I/O
functions with MGL_setFileIO, for custom I/O handling.

SEE ALSO:
MGL_fclose, MGL_fseek, MGL_ftell, MGL_fread, MGL_fwrite, MGL_setFileIO
****************************************************************************/
FILE * MGLAPI MGL_fopen(
    const char *filename,
    const char *mode)
{
    return __MGL_fopen(filename,mode);
}

/****************************************************************************
DESCRIPTION:
Closes an open disk file.

HEADER:
mgraph.h

PARAMETERS:
f   - Pointer to file to close

RETURNS:
0 on success, EOF on an error.

REMARKS:
This function is identical to the C library fclose function, but goes via
MGL's internal file handling function pointers, which by default simply
point to the standard C library functions. These functions are intended
to allow the application programmer to override all the MGL file I/O
functions with MGL_setFileIO, for custom I/O handling.

SEE ALSO:
MGL_fopen, MGL_fseek, MGL_ftell, MGL_fread, MGL_fwrite, MGL_setFileIO
****************************************************************************/
int MGLAPI MGL_fclose(
    FILE *f)
{
    return __MGL_fclose(f);
}

/****************************************************************************
DESCRIPTION:
Repositions the file pointer on a stream.

HEADER:
mgraph.h

PARAMETERS:
f       - Stream of interest
offset  - Offset of location from whence
whence  - New location of file pointer

RETURNS:
0 if move was successful, otherwise non-zero.

REMARKS:
This function is identical to the C library fseek function, but goes via
MGL's internal file handling function pointers, which by default simply
point to the standard C library functions. These functions are intended
to allow the application programmer to override all the MGL file I/O
functions with MGL_setFileIO, for custom I/O handling.

SEE ALSO:
MGL_fopen, MGL_fclose, MGL_ftell, MGL_fread, MGL_fwrite, MGL_setFileIO
****************************************************************************/
int MGLAPI MGL_fseek(
    FILE *f,
    long offset,
    int whence)
{
    return __MGL_fseek(f,offset,whence);
}

/****************************************************************************
DESCRIPTION:
Returns the current file pointer.

HEADER:
mgraph.h

PARAMETERS:
f   - Pointer to file of interest

RETURNS:
Current file pointer on success, -1L on error.

REMARKS:
This function is identical to the C library fopen function, but goes via
the MGL's internal file handling function pointers, which by default simply
points to the standard C library functions. These functions are intended
to allow the application programmer to override all the MGL file I/O
functions with MGL_setFileIO, for custom I/O handling.

SEE ALSO:
MGL_fopen, MGL_fclose, MGL_fseek, MGL_fread, MGL_fwrite, MGL_setFileIO
****************************************************************************/
long MGLAPI MGL_ftell(
    FILE *f)
{
    return __MGL_ftell(f);
}

/****************************************************************************
DESCRIPTION:
Reads data from a stream.

HEADER:
mgraph.h

PARAMETERS:
ptr - Pointer to block in stream at which to begin read
size    - Size of items to be read from stream
n   - Number of items to be read from stream
f   - Stream to be read

RETURNS:
Number of items read in, or a short count (possibly 0).

REMARKS:
This function is identical to the C library fread function, but goes via
the MGL's internal file handling function pointers, which by default simply
points to the standard C library functions. These functions are intended
to allow the application programmer to override all the MGL file I/O
functions with MGL_setFileIO, for custom I/O handling.

SEE ALSO:
MGL_fopen, MGL_fclose, MGL_fseek, MGL_ftell, MGL_fwrite, MGL_setFileIO
****************************************************************************/
size_t MGLAPI MGL_fread(
    void *ptr,
    size_t size,
    size_t n,
    FILE *f)
{
    return __MGL_fread(ptr,size,n,f);
}

/****************************************************************************
DESCRIPTION:
Writes to a stream.

HEADER:
mgraph.h

PARAMETERS:
ptr - Pointer to the starting location of data to be written
size    - Size of items to be written to file
n   - Number of items to be written to file
f   - Pointer to the file stream to write the data to

RETURNS:
The number of items written.

REMARKS:
This function is identical to the C library fopen function, but goes via
the MGL's internal file handling function pointers, which by default simply
points to the standard C library functions. These functions are intended
to allow the application programmer to override all the MGL file I/O
functions with MGL_setFileIO, for custom I/O handling.

SEE ALSO:
MGL_fopen, MGL_fclose, MGL_fseek, MGL_ftell, MGL_fread, MGL_setFileIO
****************************************************************************/
size_t MGLAPI MGL_fwrite(
    const void *ptr,
    size_t size,
    size_t n,
    FILE *f)
{
    return __MGL_fwrite(ptr,size,n,f);
}
