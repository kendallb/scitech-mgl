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
* Description:  Font converter program to load and convert MGL 1.x fonts
*               to Windows 2.x fonts.
*
****************************************************************************/

#include "mgl.h"
#include <stdio.h>
#include <stdlib.h>

/*--------------------------- Global Variables ----------------------------*/

int     _VARAPI _MGL_bufSize = DEF_MGL_BUFSIZE;
void    * _VARAPI _MGL_buf;
int     _VARAPI __MGL_result;
char    _VARAPI _MGL_path[PM_MAX_PATH] = "";
char    fontname[PM_MAX_PATH];
char    copyright[PM_MAX_PATH];

#define __MGL_fopen(filename,mode)      fopen(filename,mode)
#define __MGL_fclose(f)                 fclose(f)
#define __MGL_fseek(f,offset,whence)    fseek(f,offset,whence)
#define __MGL_ftell(f)                  ftell(f)
#define __MGL_fread(ptr,size,n,f)       fread(ptr,size,n,f)
#define __MGL_fwrite(ptr,size,n,f)      fwrite(ptr,size,n,f)
#define PM_malloc(size)             malloc(size)
#define PM_calloc(size,n)               calloc(size,n)
#define PM_free(p)                      free(p)

/*------------------------- Implementation --------------------------------*/

/****************************************************************************
PARAMETERS:
h           - Pointer to header structure to fill
f           - Open binary file to read font data from
dwOffset    - Offset to start of font in file

RETURNS:
True on success, false on error.

REMARKS:
Attempts to read the MGL 1.x font file header information from the opened
file. If the header information is invalid, we close the file and return
NULL.
****************************************************************************/
static ibool openFontFileExt(
    font_header *h,
    FILE *f,
    ulong dwOffset)
{
    /* Read in the header record and verify the font file */
    __MGL_result = grOK;
    __MGL_fseek(f,dwOffset,SEEK_SET);
    __MGL_fread(h,1,sizeof(font_header),f);
    if (strcmp(h->ident,FONT_IDENT)) {
        __MGL_result = grBadFontFile;
        return false;
        }
    return true;
}

/****************************************************************************
DESCRIPTION:
Load a font file for use from an opened file.

HEADER:
mgraph.h

PARAMETERS:
f           - Open binary file to read font data from
dwOffset    - Offset to start of font in file
dwSize      - Size of the file in bytes

RETURNS:
Pointer to the font data, or NULL on error.

REMARKS:
This function is the same as MGL_loadFont, however it works with a
previously opened file. This allows you to create your own large files with
multiple files embedded in them.

SEE ALSO:
MGL_loadFont
****************************************************************************/
font_t * MGLAPI MGL_loadFontExt(
    FILE *f,
    ulong dwOffset,
    ulong dwSize)
{
    int             i,numChars;
    size_t          size,allocSize;
    bitmap_font_old *oldFont = NULL;
    bitmap_font     *bitFont = NULL;
    font_header     header;

    /* Try and load a Windows font file */
    if (!openFontFileExt(&header,f,dwOffset))
        return NULL;

    /* Allocate memory for the font file */
    numChars = _MAXVEC+1;
    oldFont = (bitmap_font_old*)_MGL_buf;
    size = dwSize - sizeof(font_header) - sizeof(bitmap_font_old) + _MGL_FNAMESIZE + 1;
    allocSize = sizeof(bitmap_font) + (numChars*FONT_INDEX_SIZE) + size;
    if ((bitFont = PM_calloc(allocSize,1)) == NULL) {
        __MGL_result = grLoadMem;
        return NULL;
        }

    /* Load old font header information and convert to new format */
    strcpy(bitFont->name,header.name);
    strcpy(fontname,header.name);
    if (strnicmp(fontname,"PC",2) != NULL && strnicmp(fontname,"Fixed",5) != NULL && strnicmp(fontname,"Clean",5) != NULL)
        fontname[strlen(fontname)-3] = 0;
    strcpy(copyright,header.copyright+27);
    __MGL_fseek(f,dwOffset + sizeof(font_header),SEEK_SET);
    __MGL_fread(&oldFont->fontType,1,sizeof(bitmap_font_old)-_MGL_FNAMESIZE-1,f);
    bitFont->fontType   = getLEShort(oldFont->fontType);
    bitFont->maxWidth   = getLEShort(oldFont->maxWidth);
    bitFont->maxKern    = getLEShort(oldFont->maxKern);
    bitFont->fontWidth  = getLEShort(oldFont->fontWidth);
    bitFont->fontHeight = getLEShort(oldFont->fontHeight);
    bitFont->ascent     = getLEShort(oldFont->ascent);
    bitFont->descent    = getLEShort(oldFont->descent);
    bitFont->leading    = getLEShort(oldFont->leading);
    bitFont->startGlyph = 0;
    bitFont->numGlyphs  = numChars;

    /* Set up pointers to the tables within the allocated memory space */
    bitFont->width = (short*)((char*)bitFont + sizeof(bitmap_font));
    bitFont->offset = (long*)((char*)bitFont->width + bitFont->numGlyphs*sizeof(*bitFont->width));
    bitFont->iwidth = (short*)((char*)bitFont->offset + bitFont->numGlyphs*sizeof(*bitFont->offset));
    bitFont->loc = (short*)((char*)bitFont->iwidth + bitFont->numGlyphs*sizeof(*bitFont->iwidth));
    bitFont->charAscent = (short*)((char*)bitFont->loc + bitFont->numGlyphs*sizeof(*bitFont->loc));
    bitFont->charDescent = (short*)((char*)bitFont->charAscent + bitFont->numGlyphs*sizeof(*bitFont->charAscent));
    bitFont->valid = (char*)bitFont->charDescent + bitFont->numGlyphs*sizeof(*bitFont->charDescent);
    bitFont->def = (char*)bitFont->valid + bitFont->numGlyphs*sizeof(*bitFont->valid);

    /* Read the bitmap definitions from disk */
    __MGL_fread(bitFont->def,1,size,f);

    /* Copy font index data from oldFont to our new font */
    for (i = 0; i < numChars-1; i++) {
        bitFont->width[i] = oldFont->width[i];
        bitFont->offset[i] = oldFont->offset[i];
        bitFont->iwidth[i] = oldFont->iwidth[i];
        bitFont->loc[i] = oldFont->loc[i];
        bitFont->valid[i] = true;
        bitFont->charAscent[i] = 0;
        bitFont->charDescent[i] = 0;
        }

    /* Find real point size for the font (based on the font height and
     * 96DPI for the pixel size for the font).
     */
    bitFont->pointSize = (bitFont->fontHeight * 72) / 96;

    /* Create the missing symbol definition (set it to a period) */
    i = '.';
    bitFont->width[bitFont->numGlyphs-1] = bitFont->width[i];
    bitFont->iwidth[bitFont->numGlyphs-1] = bitFont->iwidth[i];
    bitFont->offset[bitFont->numGlyphs-1] = bitFont->offset[i];
    bitFont->loc[bitFont->numGlyphs-1] = bitFont->loc[i];
    bitFont->valid[bitFont->numGlyphs-1] = true;
    return (font_t*)bitFont;
}

/****************************************************************************
DESCRIPTION:
Load a font file for use.

HEADER:
mgraph.h

PARAMETERS:
fontname    - Name of the font file to load

RETURNS:
Pointer to the loaded font file, NULL on error.

REMARKS:
Locates the specified font file and loads it into memory. MGL can load any
Windows 2.x style font files (Windows 3.x font files are not supported, but
Windows 2.x font files are the standard files even for Windows 3.1. Most resource
editors can only output 2.x style font files). Consult the Windows SDK
documentation for the format of Windows font files.

When MGL is searching for font files it will first attempt to find the files just by
using the filename itself. Hence if you wish to look for a specific font file, you
should pass the full pathname to the file that you are interested in. If the filename is
a simple relative filename (i.e. "MYFONT.FNT"), MGL will then search in the
FONTS directory relative to the path specified in mglpath variable that was passed
to MGL_init. As a final resort MGL will also look for the files in the FONTS
directory relative to the MGL_ROOT environment variable.

If the font file was not found, or an error occurred while reading the font file, this
function will return NULL. You can check the MGL_result error code to determine
the cause.

SEE ALSO:
MGL_unloadFont, MGL_useFont, MGL_availableFont.
****************************************************************************/
font_t * MGLAPI MGL_loadFont(
    const char *fontname)
{
    FILE    *f;
    font_t  *font;

    __MGL_result = grOK;
    if ((f = _MGL_openFile(MGL_FONTS, fontname, "rb")) == NULL) {
        __MGL_result = grFontNotFound;
        return NULL;
        }
    font = MGL_loadFontExt(f,0,_MGL_fileSize(f));
    __MGL_fclose(f);
    return font;
}

/****************************************************************************
DESCRIPTION:
Unloads a font file from memory.

HEADER:
mgraph.h

PARAMETERS:
font    - Pointer to font to unload

REMARKS:
Unloads the specified font file from memory, and frees up all the system
resources associated with this font.

NOTE: This function is now obsolete. Please use the font library functions.

SEE ALSO:
MGL_openFontLib, MGL_unloadFontLib, MGL_loadFontInstance,
MGL_unloadFontInstance
****************************************************************************/
void MGLAPI MGL_unloadFont(
    font_t *font)
{
    if (font)
        PM_free(font);
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
file. First an attempt is made to locate the file relative to the _MGL_path
variable (initialized by the application program via the MGL_init call).
If this fails, an attempt is made to search for the file relative to the
MGL_ROOT environment variable if this is present. Lastly we search the
current directory for the file (without adding the dir extension).
Otherwise we return NULL.
****************************************************************************/
/* {secret} */
ibool MGLAPI _MGL_findFile(
    char *validpath,
    const char * dir,
    const char *name,
    const char *mode)
{
    FILE    *f;

    /* First try opening the file with just the file name (checking for
     * local directories and an explicit file path).
     */
    strcpy(validpath,name);
    if ((f = __MGL_fopen(validpath,mode)) == NULL) {
        validpath[0] = '\0';
        if (_MGL_path[0] != '\0') {
            strcpy(validpath, _MGL_path);
#ifdef  __UNIX__
            if (validpath[strlen(validpath)-1] != '/')
                strcat(validpath, "/");
#else
            if (validpath[strlen(validpath)-1] != '\\')
                strcat(validpath, "\\");
#endif
            }
        strcat(validpath, dir);
        strcat(validpath, name);

        if ((f = __MGL_fopen(validpath,mode)) == NULL) {
            /* Search the environment variable */

            if (getenv(MGL_ROOT)) {
                strcpy(validpath, getenv(MGL_ROOT));
#ifdef  __UNIX__
                if (validpath[strlen(validpath)-1] != '/')
                    strcat(validpath,"/");
#else
                if (validpath[strlen(validpath)-1] != '\\')
                    strcat(validpath,"\\");
#endif
                strcat(validpath, dir);
                strcat(validpath, name);
                if ((f = __MGL_fopen(validpath, mode)) == NULL) {
                    /* Search the current directory for the file */
                    strcpy(validpath, name);
                    f = __MGL_fopen(validpath, mode);
                    }
                }
            }
        }
    if (f)
        __MGL_fclose(f);
    return (f != NULL);
}

/* {secret} */
FILE * _MGL_openFile(const char *dir, const char *name, const char *mode)
/****************************************************************************
*
* Function:     _MGL_openFile
* Parameters:   dir     - MGL directory to find the find under
*               name    - Name of the file to open
* Returns:      Pointer to the opened file, or NULL if not found.
*
****************************************************************************/
{
    char    validpath[PM_MAX_PATH];

    if (_MGL_findFile(validpath,dir,name,mode))
        return __MGL_fopen(validpath,mode);
    return NULL;
}

/* {secret} */
long _MGL_fileSize(FILE *f)
/****************************************************************************
*
* Function:     _MGL_fileSize
* Parameters:   f   - Open file to determine the size of
* Returns:      Length of the file in bytes.
*
* Description:  Determines the length of the file, without altering the
*               current position in the file.
*
****************************************************************************/
{
    long    size,oldpos = __MGL_ftell(f);

    __MGL_fseek(f,0,SEEK_END);      /* Seek to end of file              */
    size = __MGL_ftell(f);          /* Determine the size of the file   */
    __MGL_fseek(f,oldpos,SEEK_SET); /* Seek to old position in file     */
    return size;                    /* Return the size of the file      */
}

/****************************************************************************
PARAMETERS:
outBits - Place to store the rotated bitmap definition
bits    - Bitmaps definition for the glyph
width   - Image width for the bitmap
height  - Height of the bitmap

RETURNS:
Handle to the opened file, NULL on error.

REMARKS:
Converts the glyph definition from the MGL monochrome bitmap format (row
major) to the Windows 2.x font file format (column major).
****************************************************************************/
static void swapGlyph(
    uchar *outBits,
    uchar *bits,
    int width,
    int height)
{
    int     i,j;

    for (j = 0; j < width; j++) {
        for (i = 0; i < height; i++) {
            *outBits++ = bits[i*width+j];
            }
        }
}

/****************************************************************************
PARAMETERS:
font        - Font to write
filename    - Name of the font file to write

REMARKS:
Writes a Windows 2.x font file to disk. This is not a font library, but a
single font file. Use a resource editor to get the fonts into a font
library.
****************************************************************************/
void WriteWin2xFont(
    bitmap_font *font,
    const char *filename,
    ibool bold,
    ibool italic)
{
    FILE        *f;
    winFONTINFO fi;
    ulong       glyphSize,fileSize,faceOffset,bitsOffset,gOffset;
    winGLYPH    g;
    int         width,i,gSize,fontAscent;

    if ((f = fopen(filename,"wb")) == NULL)
        PM_fatalError("Unable to open file for output!");

    /* Find the size of the objects in the file */
    glyphSize = 0;
    for (i = 0; i < font->numGlyphs-1; i++) {
        if (i > 0 && font->offset[i] != -1) {
            width = (font->width[i]+7)/8;
            gSize = width * font->fontHeight;
            glyphSize += gSize;
            }
        }
    bitsOffset = sizeof(fi) + (font->numGlyphs-1) * sizeof(g);
    faceOffset = bitsOffset + glyphSize;
    fileSize = faceOffset + strlen(fontname)+1;

    /* Build the font header */
    memset(&fi,0,sizeof(fi));
    putLEShort(fi.fh.dfVersion,0x200);
    putLELong(fi.fh.dfSize,fileSize);
    strncpy(fi.fh.dfCopyright,copyright,sizeof(fi.fh.dfCopyright));
    fi.fh.dfCopyright[sizeof(fi.fh.dfCopyright)-1] = 0;
    putLEShort(fi.fh.dfType,0);

    /* Build the font spacing record */
    fontAscent = font->fontHeight + font->descent - 1;
    putLEShort(fi.fs.dfPoints,font->pointSize);
    putLEShort(fi.fs.dfVertRes,96);
    putLEShort(fi.fs.dfHorizRes,96);
    putLEShort(fi.fs.dfAscent,fontAscent);
    putLEShort(fi.fs.dfInternalLeading,fontAscent - font->ascent);
    putLEShort(fi.fs.dfExternalLeading,font->leading);

    /* Build the font style record */
    fi.ft.dfItalic = italic;
    fi.ft.dfUnderline = 0;
    fi.ft.dfStrikeOut = 0;
    if (bold)
        putLEShort(fi.ft.dfWeight,700);
    else
        putLEShort(fi.ft.dfWeight,400);
    fi.ft.dfCharSet = 0;
    if ((font->fontType & MGL_FONTTYPEMASK) == MGL_FIXEDFONT) {
        putLEShort(fi.ft.dfPixWidth,font->fontWidth);
        fi.ft.dfPitchAndFamily = 0x30;
        putLEShort(fi.ft.dfAvgWidth,font->fontWidth);
        putLEShort(fi.ft.dfMaxWidth,font->fontWidth);
        }
    else {
        putLEShort(fi.ft.dfPixWidth,0);
        fi.ft.dfPitchAndFamily = 0x01;
        putLEShort(fi.ft.dfAvgWidth,font->width['X']);
        putLEShort(fi.ft.dfMaxWidth,font->fontWidth);
        }
    putLEShort(fi.ft.dfPixHeight,font->fontHeight);

    /* Build font char record */
    fi.fc.dfFirstChar = font->startGlyph;
    fi.fc.dfLastChar = font->startGlyph+font->numGlyphs-2;
    fi.fc.dfDefaultChar = '.' - font->startGlyph;
    fi.fc.dfBreakChar = ' ' - font->startGlyph;

    /* Build the font misc record */
    if ((font->fontType & MGL_FONTTYPEMASK) == MGL_FIXEDFONT)
        putLEShort(fi.fm.dfWidthBytes,font->iwidth['x'-font->startGlyph]/8);
    else
        putLEShort(fi.fm.dfWidthBytes,0);   //??
    putLELong(fi.fm.dfDevice,0);
    putLELong(fi.fm.dfFace,faceOffset);
    putLELong(fi.fm.dfBitsPointer,0);
    putLELong(fi.fm.dfBitsOffset,bitsOffset);

    /* Write the header record to disk */
    __MGL_fwrite(&fi,1,sizeof(fi),f);

    /* Write each glyph record to disk */
    gOffset = ftell(f) + sizeof(g) * (font->numGlyphs-1);
    if (gOffset != bitsOffset)
        PM_fatalError("Offsets not the same!");
    for (i = 0; i < font->numGlyphs-1; i++) {
        if (i > 0 && font->offset[i] != -1) {
            putLEShort(g.gWidth,font->width[i]);
            putLEShort(g.gOffset,gOffset);
            width = (font->width[i]+7)/8;
            gSize = width * font->fontHeight;
            gOffset += gSize;
            }
        else {
            putLEShort(g.gWidth,0);
            putLEShort(g.gOffset,gOffset);
            }
        __MGL_fwrite(&g,1,sizeof(g),f);
        }

    /* Write the gyph bitmaps to disk */
    for (i = 0; i < font->numGlyphs-1; i++) {
        if (i > 0 && font->offset[i] != -1) {
            width = (font->width[i]+7)/8;
            gSize = width * font->fontHeight;
            memset(_MGL_buf,0,gSize);
            if (font->iwidth[i])
                swapGlyph(_MGL_buf,&font->def[font->offset[i]],(font->iwidth[i]+7)/8,font->fontHeight);
            __MGL_fwrite(_MGL_buf,1,gSize,f);
            }
        }

    /* Write the font facename to disk */
    __MGL_fwrite(fontname,1,strlen(fontname)+1,f);
    fclose(f);
}

void main(int argc,char *argv[])
{
    ibool   isbold = false;
    ibool   isitalic = false;
    font_t  *font;

    if ((_MGL_buf = malloc(_MGL_bufSize)) == NULL)
        PM_fatalError("Out of memory!");

    if ((font = MGL_loadFont(argv[1]))) {
        if (font->fontType != MGL_VECTORFONT) {
            isbold = strstr(fontname,"Bold") != NULL;
            isitalic = strstr(fontname,"Italic") != NULL;
            WriteWin2xFont(BITFONT(font),argv[1],isbold,isitalic);
            MGL_unloadFont(font);
            printf("Converted font '%s' ('%s').\n", argv[1], fontname);
            }
        else {
            printf("Vector font file '%s' skipped.\n", argv[1]);
            }
        }
    else {
        printf("Unable to load font file!\n");
        }
}
