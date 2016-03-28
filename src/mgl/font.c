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
* Description:  Font resource loading/unloading routines. Font files are
*               stored on disk in either the old MGL 1.x font file format
*               or the standard Windows 2.x bitmap font file format (not
*               Windows 3.x format). We also support TrueType font libraries
*               via the FreeType library.
*
****************************************************************************/

#include "mgl.h"

/*--------------------------- Global Variables ----------------------------*/

static TT_Engine    _MGL_ttEngine;
static ibool        _MGL_ttEngineOpen = FALSE;
static int          _MGL_xDPI = 96;
static int          _MGL_yDPI = 96;

/*------------------------- Implementation --------------------------------*/

/****************************************************************************
DESCRIPTION:
Sets the outline font dots per inch value.

HEADER:
mgraph.h

PARAMETERS:
xDPI    - Horizontal dots per inch
yDPI    - Vertical dots per inch

REMARKS:
This function set the horizontal and vertical dots per inch factor, which
is used to generate outline font bitmap glyphs. Making the value larger
results in larger font bitmaps. The default value for PC compatible
systems in 96 DPI.
****************************************************************************/
void MGLAPI MGL_setDotsPerInch(
    int xDPI,
    int yDPI)
{
    _MGL_xDPI = xDPI;
    _MGL_yDPI = yDPI;
}

/****************************************************************************
DESCRIPTION:
Gets the outline font dots per inch value.

HEADER:
mgraph.h

PARAMETERS:
xDPI    - Place to store horizontal dots per inch
yDPI    - Place to store vertical dots per inch

REMARKS:
This function returns the current the horizontal and vertical dots per inch
factor, which is used to generate outline font bitmap glyphs. Making the
value larger results in larger font bitmaps. The default value for PC
compatible systems in 96 DPI.
****************************************************************************/
void MGLAPI MGL_getDotsPerInch(
    int *xDPI,
    int *yDPI)
{
    *xDPI = _MGL_xDPI;
    *yDPI = _MGL_yDPI;
}

// TODO!
//void MGLAPI MGL_setTTFCharacterMap(int platform,int encoding);

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
PARAMETERS:
h           - Pointer to header structure to fill
fontname    - Name of the font file to load

RETURNS:
Handle to the opened file, NULL on error.

REMARKS:
Attempts to open the specified font file and read the font header
information. If the header information is invalid, we close the file and
return NULL.
****************************************************************************/
static FILE *openFontFile(
    font_header *h,
    const char *fontname)
{
    FILE    *f;

    __MGL_result = grOK;
    f = _MGL_openFile(MGL_FONTS, fontname, "rb");
    if (f == NULL) {
        __MGL_result = grFontNotFound;
        return NULL;
        }
    if (!openFontFileExt(h,f,0)) {
        __MGL_fclose(f);
        return NULL;
        }
    return f;
}

/****************************************************************************
PARAMETERS:
fi          - Pointer to FONTINFO structure to fill
f           - Open binary file to read font data from
dwOffset    - Offset to start of font in file

RETURNS:
True on success, false on error.

REMARKS:
Attempts to read the Windows 2.x font header information from the open file.
If the header information is invalid, we close the file and return NULL.
This also works for font files embedded in a .FON library.
****************************************************************************/
static ibool openWinFontFileExt(
    winFONTINFO *fi,
    FILE *f,
    ulong dwOffset)
{
    size_t  size;

    /* Read in the header record and verify the font file */
    __MGL_result = grOK;
    __MGL_fseek(f,dwOffset,SEEK_SET);
    size = __MGL_fread(fi,1,sizeof(winFONTINFO),f);
    if (size != sizeof(winFONTINFO) || getLEShort(fi->fh.dfType) != 0) {
        __MGL_result = grBadFontFile;
        return false;
        }
    if (getLEShort(fi->fh.dfVersion) != 0x200) {
        FATALERROR(grNewFontFile);
        return false;
        }
    return true;
}

/****************************************************************************
PARAMETERS:
fi          - Pointer to FONTINFO structure to fill
fontname    - Name of the font file to load

RETURNS:
Handle to the opened file, NULL on error.

REMARKS:
Attempts to open the specified font file and read the font header
information. If the header information is invalid, we close the file and
return NULL.
****************************************************************************/
static FILE *openWinFontFile(
    winFONTINFO *fi,
    const char *fontname)
{
    FILE    *f;

    __MGL_result = grOK;
    f = _MGL_openFile(MGL_FONTS, fontname, "rb");
    if (f == NULL) {
        __MGL_result = grFontNotFound;
        return NULL;
        }
    if (!openWinFontFileExt(fi,f,0)) {
        __MGL_fclose(f);
        return NULL;
        }
    return f;
}

#define TEMP_BUF_SIZE   512

/****************************************************************************
PARAMETERS:
bits    - Bitmaps definition for the glyph
width   - Width of the bitmap
height  - Height of the bitmap

RETURNS:
Handle to the opened file, NULL on error.

REMARKS:
Converts the glyph definition from the Windows 2.x font file format
(column major) to the MGL monochrome bitmap format (row major).
****************************************************************************/
static void swapGlyph(
    uchar *bits,
    int width,
    int height)
{
    uchar   *p,buf[TEMP_BUF_SIZE];
    int     i,j,size = height * width;

    if (width == 1)
        return;
    if (size > TEMP_BUF_SIZE)
        MGL_fatalError("Temporary glyph buffer is too small!!");
    memcpy(buf,bits,size);
    for (j = 0, p = buf; j < width; j++) {
        for (i = 0; i < height; i++) {
            bits[i*width+j] = *p++;
            }
        }
}

/****************************************************************************
PARAMETERS:
bitFont     - Bitmap font to measure
ch          - Chararacter to measure
charAscent  - Place to store the character ascent value
charDescent - Place to store the character descent value

REMARKS:
This function finds the character ascent and descent values for a glyphs,
by searching for the first non-zero line from the top and bottom of the
bitmap definition. We do this once when the font is loaded to pre-compute
these values internally. We then use the character ascent and descent
values to avoid drawing font glyph information that is not present to speed
up font rendering.
****************************************************************************/
static void getCharMetrics(
    bitmap_font *bitFont,
    int glyph,
    short *charAscent,
    short *charDescent)
{
    int     i,j,offset,dx,byteWidth,fontAscent;
    uchar   *bytes;

    /* Find the address of our glyph in the font tables */
    byteWidth = (bitFont->iwidth[glyph] + 7) / 8;
    offset = bitFont->offset[glyph];
    bytes = (uchar*)&bitFont->def[offset];

    /* Measure the char ascent value from the top of the bitFont glyph */
    offset = dx = 0;
    fontAscent = bitFont->fontHeight + bitFont->descent - 1;
    for (i = 0; i < fontAscent; i++) {
        for (j = 0; j < byteWidth; j++)
            if (bytes[offset++] != 0) {
                dx = true;
                break;
                }
        if (dx == true)
            break;
        }
    *charAscent = fontAscent - i;

    /* Measure the char descent value from the bottom of the bitFont glyph */
    offset = (bitFont->fontHeight-1) * byteWidth + byteWidth-1;
    dx = false;
    for (i = bitFont->fontHeight-1; i > fontAscent; i--) {
        for (j = 0; j < byteWidth; j++)
            if (bytes[offset--] != 0) {
                dx = true;
                break;
                }
        if (dx == true)
            break;
        }
    *charDescent = fontAscent - i;
}

/****************************************************************************
PARAMETERS:
f           - Open binary file to read font data from
dwOffset    - Offset to start of font in file

RETURNS:
Pointer to the font data, or NULL on error.

REMARKS:
Attempts to locate and load in a Windows 2.x bitmap font file and convert
it to the structure used internally in the MGL for rendering MGL style
bitmap fonts. If the font is invalid or we cant find it we return NULL.
****************************************************************************/
static font_t *loadWinFontFileExt(
    FILE *f,
    ulong dwOffset)
{
    int         i,numChars,glyphSize,offset,dpi;
    bitmap_font *font;
    winGLYPH    *g;
    winFONTINFO fi;

    /* Open the font file header */
    if (!openWinFontFileExt(&fi,f,dwOffset))
        return NULL;

    /* Allocate memory for the font file */
    glyphSize = getLELong(fi.fm.dfFace) - getLELong(fi.fm.dfBitsOffset);
    numChars = fi.fc.dfLastChar - fi.fc.dfFirstChar+1;
    if ((int)(sizeof(*g) * numChars) > _MGL_bufSize)
        MGL_fatalError("MGL internal buffer is too small!");
    g = _MGL_buf;
    if ((font = PM_calloc(sizeof(bitmap_font) + (numChars*FONT_INDEX_SIZE) + glyphSize, 1)) == NULL) {
        FATALERROR(grLoadMem);
        return NULL;
        }

    /* Read the face name of the font */
    __MGL_fseek(f,getLELong(fi.fm.dfFace) + dwOffset,SEEK_SET);
    __MGL_fread(font->name,1,_MGL_FNAMESIZE,f);

    /* Build the MGL font header information */
    if (getLEShort(fi.ft.dfPixWidth) != 0) {
        font->fontType =  MGL_FIXEDFONT;
        font->fontWidth = getLEShort(fi.ft.dfPixWidth);
        }
    else {
        font->fontType =  MGL_PROPFONT;
        font->fontWidth = getLEShort(fi.ft.dfMaxWidth);
        }
    font->fontHeight = getLEShort(fi.ft.dfPixHeight);
    font->maxWidth = getLEShort(fi.ft.dfMaxWidth);
    font->maxKern = 0;
    font->ascent = getLEShort(fi.fs.dfAscent) - getLEShort(fi.fs.dfInternalLeading) - 1;
    font->descent = getLEShort(fi.fs.dfAscent) - getLEShort(fi.ft.dfPixHeight);
    font->leading = getLEShort(fi.fs.dfExternalLeading);
    font->startGlyph = fi.fc.dfFirstChar;
    font->numGlyphs = numChars;

    /* Find the point size for the font, normalising it to 96DPI */
    dpi = getLEShort(fi.fs.dfVertRes);
    font->pointSize = (getLEShort(fi.fs.dfPoints) * dpi) / 96;

    /* Set up pointers to the tables within the allocated memory space */
    font->width = (short*)((char*)font + sizeof(bitmap_font));
    font->offset = (long*)((char*)font->width + font->numGlyphs*sizeof(*font->width));
    font->iwidth = (short*)((char*)font->offset + font->numGlyphs*sizeof(*font->offset));
    font->loc = (short*)((char*)font->iwidth + font->numGlyphs*sizeof(*font->iwidth));
    font->charAscent = (short*)((char*)font->loc + font->numGlyphs*sizeof(*font->loc));
    font->charDescent = (short*)((char*)font->charAscent + font->numGlyphs*sizeof(*font->charAscent));
    font->valid = (char*)font->charDescent + font->numGlyphs*sizeof(*font->charDescent);
    font->def = (uchar*)font->valid + font->numGlyphs*sizeof(*font->valid);

    /* Read the glyph table from disk */
    __MGL_fseek(f,sizeof(winFONTINFO) + dwOffset,SEEK_SET);
    __MGL_fread(g,sizeof(winGLYPH),numChars,f);

    /* Read the glyph definitions from disk */
    __MGL_fseek(f,getLELong(fi.fm.dfBitsOffset) + dwOffset,SEEK_SET);
    __MGL_fread(font->def,1,glyphSize,f);

    for (i = offset = 0; i < font->numGlyphs-1; i++) {
        font->width[i] = getLEShort(g[i].gWidth);
        font->iwidth[i] = font->width[i];
        font->offset[i] = offset;
        font->valid[i] = true;
        if (font->width[i] == 0) {
            /* Missing symbol */
            font->offset[i] = -1;
            font->charAscent[i] = 0;
            font->charDescent[i] = 0;
            }
        else
            getCharMetrics(font,i,&font->charAscent[i],&font->charDescent[i]);
        font->loc[i] = 0;
        swapGlyph(font->def+offset,(font->iwidth[i]+7)/8,font->fontHeight);
        offset += (getLEShort(g[i+1].gOffset) - getLEShort(g[i].gOffset));
        }

    /* Create the missing symbol definition */
    i = '.' - fi.fc.dfFirstChar;
    font->width[font->numGlyphs-1] = font->width[i];
    font->iwidth[font->numGlyphs-1] = font->iwidth[i];
    font->offset[font->numGlyphs-1] = font->offset[i];
    font->loc[font->numGlyphs-1] = font->loc[i];
    font->valid[font->numGlyphs-1] = true;
    return (font_t*)font;
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
    short           fontType;
    size_t          size,allocSize;
    font_t          *font;
    vec_font        *vecFont;
    bitmap_font_old *oldFont;
    bitmap_font     *bitFont;
    font_header     header;

    /* Try and load a Windows font file */
    if (!openFontFileExt(&header,f,dwOffset))
        return loadWinFontFileExt(f,dwOffset);

    /* Read font header into _MGL_buf */
    font = (font_t*)((uchar*)_MGL_buf);
    __MGL_fseek(f,dwOffset + sizeof(font_header),SEEK_SET);
    __MGL_fread(&fontType,1,sizeof(fontType),f);
    PM_convLE_16(fontType);

    /* Switch on Font Type */
    if (fontType == MGL_VECTORFONT) {
        /* Allocate memory and read in the entire MGL 1.x vector font file */
        size = dwSize - sizeof(font_header);
        if ((vecFont = PM_malloc(size + _MGL_FNAMESIZE)) == NULL) {
            FATALERROR(grLoadMem);
            return NULL;
            }
        strcpy(vecFont->name,header.name);
        __MGL_fseek(f,dwOffset + sizeof(font_header),SEEK_SET);
        __MGL_fread(&vecFont->fontType,1,size,f);

#ifdef __BIG_ENDIAN__
        /* Convert to big endian format if needed */
        PM_convLE_16(vecFont->fontType);
        PM_convLE_16(vecFont->maxWidth);
        PM_convLE_16(vecFont->maxKern);
        PM_convLE_16(vecFont->fontWidth);
        PM_convLE_16(vecFont->fontHeight);
        PM_convLE_16(vecFont->ascent);
        PM_convLE_16(vecFont->descent);
        PM_convLE_16(vecFont->leading);
        for (i = 0; i < _MAXVEC; i++)
            PM_convLE_16(vecFont->offset[i]);

        /* Byte swap all glyph data if needed */
        size = (size - _MGL_FNAMESIZE - sizeof(vec_font)) & ~1;
        for (i = 0; i < size; i += 2) {
            vector *vec;
            vec = (vector*)&vecFont->def[i];
            PM_convLE_16(*vec);
            }
#endif

        /* Set the missing symbol to a period (0'th character for vector
         * fonts)
         */
        i = '.';
        vecFont->width[0] = vecFont->width[i];
        vecFont->offset[0] = vecFont->offset[i];
        return (font_t*)vecFont;
        }
    else {
        /* Allocate memory for the font file */
        numChars = _MAXVEC+1;
        oldFont = (bitmap_font_old*)font;
        size = dwSize - sizeof(font_header) - sizeof(bitmap_font_old) + _MGL_FNAMESIZE + 1;
        allocSize = sizeof(bitmap_font) + (numChars*FONT_INDEX_SIZE) + size;
        if ((bitFont = PM_calloc(allocSize,1)) == NULL) {
            FATALERROR(grLoadMem);
            return NULL;
            }

        /* Load old font header information and convert to new format */
        strcpy(bitFont->name,header.name);
        __MGL_fseek(f,dwOffset + sizeof(font_header),SEEK_SET);
        __MGL_fread(&oldFont->fontType,1,sizeof(bitmap_font_old)-_MGL_FNAMESIZE-1,f);
        /* Convert to big endian if needed */
        PM_convLE_16(oldFont->fontType);
        PM_convLE_16(oldFont->maxWidth);
        PM_convLE_16(oldFont->maxKern);
        PM_convLE_16(oldFont->fontWidth);
        PM_convLE_16(oldFont->fontHeight);
        PM_convLE_16(oldFont->ascent);
        PM_convLE_16(oldFont->descent);
        PM_convLE_16(oldFont->leading);
        for (i = 0; i < _MAXVEC; i++)
            PM_convLE_16(oldFont->offset[i]);

        bitFont->fontType   = oldFont->fontType;
        bitFont->maxWidth   = oldFont->maxWidth;
        bitFont->maxKern    = oldFont->maxKern;
        bitFont->fontWidth  = oldFont->fontWidth;
        bitFont->fontHeight = oldFont->fontHeight;
        bitFont->ascent     = oldFont->ascent;
        bitFont->descent    = oldFont->descent;
        bitFont->leading    = oldFont->leading;
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
        bitFont->def = (uchar*)bitFont->valid + bitFont->numGlyphs*sizeof(*bitFont->valid);

        /* Read the bitmap definitions from disk */
        __MGL_fread(bitFont->def,1,size,f);

        /* Copy font index data from oldFont to our new font */
        for (i = 0; i < numChars-1; i++) {
            bitFont->width[i] = oldFont->width[i];
            bitFont->offset[i] = oldFont->offset[i];
            bitFont->iwidth[i] = oldFont->iwidth[i];
            bitFont->loc[i] = oldFont->loc[i];
            bitFont->valid[i] = true;
            if (bitFont->offset[i] == -1) {
                bitFont->charAscent[i] = 0;
                bitFont->charDescent[i] = 0;
                }
            else
                getCharMetrics(bitFont,i,&bitFont->charAscent[i],&bitFont->charDescent[i]);
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
        bitFont->antialiased = false;
        return (font_t*)bitFont;
        }
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
Determines if a specific font file is available for use.

HEADER:
mgraph.h

PARAMETERS:
fontname    - Relative filename of the required font file

RETURNS:
True if font file is available, false if not.

REMARKS:
Attempt to locate the specified font file, and verify that it is available for use. See
MGL_loadFont for more information on the algorithm that MGL uses when
searching for font files on disk.

SEE ALSO:
MGL_loadFont
****************************************************************************/
ibool MGLAPI MGL_availableFont(
    const char *fontname)
{
    FILE        *f;
    font_header header;
    winFONTINFO fi;

    __MGL_result = grOK;
    if ((f = openFontFile(&header,fontname)) == NULL)
        if ((f = openWinFontFile(&fi,fontname)) == NULL)
            return false;
    __MGL_fclose(f);
    return true;
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

Note: This function is now obsolete. Please use the font library functions.

SEE ALSO:
MGL_loadFont, MGL_openFontLib, MGL_unloadFontLib, MGL_loadFontInstance,
MGL_unloadFontInstance
****************************************************************************/
void MGLAPI MGL_unloadFont(
    font_t *font)
{
    if (font) {
        if ((font->fontType & MGL_FONTTYPEMASK) != MGL_VECTORFONT) {
            if (BITFONT(font)->lib && BITFONT(font)->lib->fontLibType == MGL_TRUETYPEFONT_LIB)
                PM_free(BITFONT(font)->def);
            }
        PM_free(font);
        }
}

/****************************************************************************
DESCRIPTION:
Unloads a font instance from memory.

HEADER:
mgraph.h

PARAMETERS:
font    - Pointer to font instance to unload

REMARKS:
Unloads the specified font instance from memory, and frees up all the system
resources associated with this font instance.

SEE ALSO:
MGL_openFontLib, MGL_unloadFontLib, MGL_loadFontInstance
****************************************************************************/
void MGLAPI MGL_unloadFontInstance(
    font_t *font)
{
    if ((font->fontType & MGL_FONTTYPEMASK) != MGL_VECTORFONT) {
        if (BITFONT(font)->lib && BITFONT(font)->lib->fontLibType != MGL_BITMAPFONT_LIB)
            TT_Done_Instance(BITFONT(font)->instance);
        }
    MGL_unloadFont(font);
}

/****************************************************************************
DESCRIPTION:
Load a font file for use from an opened file.

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
****************************************************************************/
static font_lib_t *openBitmapFontLibExt(
    FILE *f,
    ibool ownHandle,
    ulong dwOffset,
    ulong dwSize)
{
    int         i,alignShift,count,dpi;
    char        temp[2];
    uchar       winInfoOffset[2];
    winWININFO  winInfo;
    winTYPEINFO winTypeInfo;
    winNAMEINFO winNameInfo;
    winFONTINFO fi;
    bitmap_font_lib *lib;

    /* Read in the header record and verify the font file */
    __MGL_result = grBadFontFile;
    __MGL_fseek(f,dwOffset,SEEK_SET);
    if (__MGL_fread(temp,1,sizeof(temp),f) != sizeof(temp))
        return NULL;
    if ((temp[0] != 'M') || (temp[1] != 'Z'))
        return NULL;
    __MGL_fseek(f,dwOffset+0x3C,SEEK_SET);
    if (__MGL_fread(winInfoOffset,1,sizeof(winInfoOffset),f) != sizeof(winInfoOffset))
        return NULL;
    __MGL_fseek(f,dwOffset+getLEShort(winInfoOffset),SEEK_SET);
    if (__MGL_fread(&winInfo,1,sizeof(winInfo),f) != sizeof(winInfo))
        return NULL;
    if ((winInfo.signature[0] != 'N') || (winInfo.signature[1] != 'E'))
        return NULL;
    if ((getLEShort(winInfo.exeFlags) & 0x8000) != 0x8000)
        return NULL;
    __MGL_fseek(f,dwOffset+getLEShort(winInfoOffset)+getLEShort(winInfo.resTabOffset),SEEK_SET);
    if (__MGL_fread(temp,1,sizeof(temp),f) != sizeof(temp))
        return NULL;
    alignShift = getLEShort(temp);
    for (;;) {
        if (__MGL_fread(&winTypeInfo,1,sizeof(winTypeInfo),f) != sizeof(winTypeInfo))
            return NULL;
        if (getLEShort(winTypeInfo.rtTypeID) == 0)
            return NULL;
        if (getLEShort(winTypeInfo.rtTypeID) == 0x8008)
            break;
        count = getLEShort(winTypeInfo.rtResourceCount);
        __MGL_fseek(f,count * sizeof(winNAMEINFO),SEEK_CUR);
        }
    count = getLEShort(winTypeInfo.rtResourceCount);
    if ((lib = PM_malloc(sizeof(bitmap_font_lib) + (count*9))) == NULL) {
        FATALERROR(grLoadMem);
        return NULL;
        }
    lib->fontLibType = MGL_BITMAPFONT_LIB;
    lib->f = f;
    lib->ownHandle = ownHandle;
    lib->dwOffset = dwOffset;
    lib->numFonts = count;
    lib->fontSizes = ((char*)lib) + sizeof(bitmap_font_lib);
    lib->offset = (M_uint32*)(lib->fontSizes + count);
    lib->length = (M_uint32*)(lib->offset + count);
    for (i = 0; i < count; i++) {
        if (__MGL_fread(&winNameInfo,1,sizeof(winNameInfo),f) != sizeof(winNameInfo))
            goto Error;
        lib->offset[i] = (M_uint32)getLEShort(winNameInfo.rnOffset) << alignShift;
        lib->length[i] = getLEShort(winNameInfo.rnLength);
        }
    for (i = 0; i < count; i++) {
        if (!openWinFontFileExt(&fi,f,dwOffset+lib->offset[i]))
            goto Error;

        if (i == 0) {
            /* Read the face name of the font */
            __MGL_fseek(f,getLELong(fi.fm.dfFace)+dwOffset+lib->offset[i],SEEK_SET);
            __MGL_fread(lib->name,1,_MGL_FNAMESIZE,f);
            }

        /* Find the point size for the font, normalising it to 96DPI */
        dpi = getLEShort(fi.fs.dfVertRes);
        lib->fontSizes[i] = (getLEShort(fi.fs.dfPoints) * dpi) / 96;
        }
    (void)dwSize;
    return (font_lib_t*)lib;

Error:
    PM_free(lib);
    return NULL;
}

/****************************************************************************
DESCRIPTION:
Load a font file for use from an opened file.

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
****************************************************************************/
static font_lib_t *openTTFontLibExt(
    FILE *f,
    ibool ownHandle,
    ulong dwOffset,
    ulong dwSize)
{
// Global variables to maintain the engine.
// Variable to let us know if its already opened.
// do parts a-c from spec.
    (void)f;
    (void)ownHandle;
    (void)dwOffset;
    (void)dwSize;
    return NULL;
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
MGL_loadFont, MGL_openFontLib, MGL_unloadFontLib, MGL_loadFontInstance,
MGL_unloadFontInstance
****************************************************************************/
font_lib_t * MGLAPI MGL_openFontLibExt(
    FILE *f,
    ulong dwOffset,
    ulong dwSize)
{
    font_lib_t *lib;
    if ((lib = openBitmapFontLibExt(f,FALSE,dwOffset,dwSize)) == NULL)
        lib = openTTFontLibExt(f,FALSE,dwOffset,dwSize);
    return lib;
}

/****************************************************************************
DESCRIPTION:
Opens a font library for use.

HEADER:
mgraph.h

PARAMETERS:
fontname    - Name of the font library to open.

RETURNS:
Pointer to the opened font library, NULL on error.

REMARKS:
Locates the specified font library and opens the file. MGL can load any
Windows 2.x style font files (Windows 3.x font files are not supported, but
Windows 2.x font files are the standard files even for Windows 3.1. Most resource
editors can only output 2.x style font files). Consult the Windows SDK
documentation for the format of Windows font files.

If the font library was not found, or an error occurred while reading the font
library, this function will return NULL. You can check the MGL_result error
code to determine the cause.
****************************************************************************/
static font_lib_t * MGL_openTTFontLib(
    const char *libname)
{
    tt_font_lib         *lib;
    char                fullPath[PM_MAX_PATH];
    TT_Face_Properties  props;
    int                 i,j,found;
    ushort              platform,encoding,language,id;
    char                *str;
    TT_String           *stringPtr;
    TT_UShort           stringLength;

    __MGL_result = grOK;
    if (!_MGL_findFile(fullPath, MGL_FONTS, libname, "rb")) {
        __MGL_result = grFontNotFound;
        return NULL;
        }

    /* Initialize FreeType Engine if we have not done so already */
    if (!_MGL_ttEngineOpen) {
        if (TT_Init_FreeType(&_MGL_ttEngine) != 0)
            MGL_fatalError("Unable to initialise FreeType!");
        _MGL_ttEngineOpen = true;
        }

    /* Alocate memory for a library structure */
    if ((lib = PM_malloc(sizeof(tt_font_lib))) == NULL) {
        FATALERROR(grLoadMem);
        return NULL;
        }

    /* Open the font */
    if (TT_Open_Face(_MGL_ttEngine, fullPath, &lib->face)) {
        __MGL_result = grBadFontFile;
        PM_free(lib);
        return NULL;
        }

    /* Get the face properties */
    TT_Get_Face_Properties(lib->face, &props);

    /* Read the face name of the font */
    for (i = 0; i < props.num_Names; i++) {
        TT_Get_Name_ID(lib->face, i, &platform, &encoding, &language, &id);
        TT_Get_Name_String(lib->face, i, &stringPtr, &stringLength);
        if (id == TT_NAME_ID_FULL_NAME) {
            found = 0;

            /* Try to find a Microsoft English name */
            if ((platform == TT_PLATFORM_MICROSOFT) &&
                (language == TT_MS_LANGID_ENGLISH_UNITED_STATES))
                found = 1;

            /* Found a Unicode Name */
            if (found) {
                str = lib->name;
                for ( i = 1, j = 0; i < stringLength; i += 2 )
                    str[j++] = stringPtr[i];
                str[j] = '\0';
                break;
                }
            }
        }

    /* Initialize the library structure members */
    lib->fontLibType = MGL_TRUETYPEFONT_LIB;
    lib->ownHandle = FALSE;
    lib->dwOffset = 0;
    lib->numFonts = props.num_Faces;
    lib->numGlyphs = props.num_Glyphs;
    return (font_lib_t*)lib;
}

/****************************************************************************
DESCRIPTION:
Opens a font library for use.

HEADER:
mgraph.h

PARAMETERS:
fontname    - Name of the font library to open.

RETURNS:
Pointer to the opened font library, NULL on error.

REMARKS:
Locates the specified font library and opens the file. MGL can load any
Windows 2.x style font files (Windows 3.x font files are not supported, but
Windows 2.x font files are the standard files even for Windows 3.1. Most resource
editors can only output 2.x style font files). Consult the Windows SDK
documentation for the format of Windows font files.

If the font library was not found, or an error occurred while reading the font
library, this function will return NULL. You can check the MGL_result error
code to determine the cause.

SEE ALSO:
MGL_loadFont, MGL_openFontLibExt, MGL_loadFontInstance,
MGL_unloadFontInstance, MGL_closeFontLib
****************************************************************************/
font_lib_t * MGLAPI MGL_openFontLib(
    const char *libname)
{
    FILE        *f;
    font_lib_t  *lib;

    __MGL_result = grOK;
    if ((f = _MGL_openFile(MGL_FONTS, libname, "rb")) == NULL) {
        __MGL_result = grFontNotFound;
        return NULL;
        }
    if ((lib = openBitmapFontLibExt(f,TRUE,0,_MGL_fileSize(f))) == NULL) {
        __MGL_fclose(f);
        lib = MGL_openTTFontLib(libname);
        }
    return lib;
}

/****************************************************************************
DESCRIPTION:
Load a font file for use.

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
****************************************************************************/
static font_t * loadBitmapFontInstance(
    font_lib_t *fontlib,
    float pointSize,
    float slant,
    float angle,
    ibool antialiased)
{
    int             i,closest = 0,diff,min,size;
    bitmap_font_lib *lib = (bitmap_font_lib*)fontlib;
    font_t          *font;

    if ((slant != 0) || (angle != 0) || (antialiased))
        return NULL;
    min = 0x7FFFFFFF;
    size = (int)pointSize;
    for (i = 0; i < lib->numFonts; i++) {
        diff = abs(lib->fontSizes[i] - size);
        if (diff < min) {
            closest = i;
            min = diff;
            }
        }
    font = MGL_loadFontExt(lib->f,lib->dwOffset+lib->offset[closest],lib->length[closest]);
    return font;
}

/****************************************************************************
PARAMETERS:
a           - Value to scale to integer (16.16 fixed point)
scaleFactor - Scale factor to convert from 16.16 to 26.6 fixed point

RETURNS:
Value scaled to an integer

REMARKS:
This function takes the 16.16 fixed point value and the 16.16 scale factor
that will result in a 26.6 fixed point value. We then convert this to an
integer with rounding. Note that we take special care with rounding negative
numbers to ensure that they round correctly.
****************************************************************************/
static int ScaleToInteger(
    TT_Fixed a,
    TT_Fixed scaleFactor)
{
    a = TT_MulFix(a,scaleFactor);
    if (a < 0)
        return -((-a + 31) / 64);
    return (a + 32) / 64;
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
static font_t * loadTTFontInstance(
    font_lib_t *fontlib,
    float pointSize,
    float slant,
    float angle,
    ibool antialiased)
{
    int                 i,j,maxWidth;
    int                 ascent,descent,lineGap;
    ushort              platform,encoding,charMapIdx,idx;
    tt_font_lib         *lib = (tt_font_lib*)fontlib;
    bitmap_font         tmp,*font;
    TT_Face_Properties  props;
    TT_Instance_Metrics metrics;
    TT_Glyph            glyph;
    TT_Instance         instance;

    if ((slant != 0) || (angle != 0))
        return NULL;

    /* Create an instance object for the face */
    if (TT_New_Instance(lib->face, &instance))
        return NULL;
    /* Set the Horizontal & Vertical resolution */
    if (TT_Set_Instance_Resolutions(instance, _MGL_xDPI, _MGL_yDPI))
        return NULL;
    /* Set the Point Size (in 26.6 fixed format) */
    if (TT_Set_Instance_CharSize(instance, pointSize*64))
        return NULL;
    /* Create a glyph container */
    if (TT_New_Glyph(lib->face, &glyph))
        return NULL;

    /* Get the face properties and instance metrics */
    TT_Get_Face_Properties(lib->face, &props);
    TT_Get_Instance_Metrics(instance,&metrics);

    /* Find the scaled instance measurements */
    maxWidth = ScaleToInteger(props.horizontal->advance_Width_Max,metrics.x_scale);
    ascent = ScaleToInteger(props.horizontal->Ascender,metrics.y_scale);
    descent = ScaleToInteger(props.horizontal->Descender,metrics.y_scale);
    lineGap = ScaleToInteger(props.horizontal->Line_Gap,metrics.y_scale);

    /* Build the TEMP font header information */
    tmp.fontType =  MGL_PROPFONT;
    if (antialiased)
        tmp.fontType |= MGL_ANTIALIASEDFONT;

    tmp.fontWidth = maxWidth;
    tmp.fontHeight = ascent - descent;
    tmp.maxKern = 0;
    tmp.ascent = ScaleToInteger(props.horizontal->Descender + props.header->Units_Per_EM,metrics.y_scale);
    tmp.descent = descent;
    tmp.leading = lineGap;
    tmp.numGlyphs = props.num_Glyphs;;
    tmp.antialiased = antialiased;

    platform = 0;
    charMapIdx = -1;
    while (platform != 3) {
        charMapIdx++;
        if (charMapIdx >= 0xFFFF)
            return NULL;
        TT_Get_CharMap_ID(lib->face, charMapIdx, &platform, &encoding);
        }
    /* Get the character map */
    if (TT_Get_CharMap(lib->face, charMapIdx, &lib->charMap))
        return NULL;

    /* Find the starting glyph */
    j = tmp.numGlyphs;
    tmp.startGlyph = 0;
    for (i = 0; i < j; i++) {
        idx = TT_Char_Index(lib->charMap,i);
        if (idx > 0) {
            tmp.startGlyph = i;
            break;
            }
        tmp.numGlyphs--;
        }

    /* Increment the number of glyphs to account for the missing symbol */
    tmp.numGlyphs++;

    /* Check _MGL_bufSize for enough memory in temporary buffer */
    if ((int)(tmp.numGlyphs*FONT_INDEX_SIZE) > _MGL_bufSize) {
        FATALERROR(grLoadMem);
        return NULL;
        }

    /* Set up temporary pointer in _MGL_buf */
    tmp.width = (short*)((char*)_MGL_buf);
    tmp.offset = (long*)((char*)tmp.width + tmp.numGlyphs*sizeof(*tmp.width));
    tmp.iwidth = (short*)((char*)tmp.offset + tmp.numGlyphs*sizeof(*tmp.offset));
    tmp.loc = (short*)((char*)tmp.iwidth + tmp.numGlyphs*sizeof(*tmp.iwidth));
    tmp.charAscent = (short*)((char*)tmp.loc + tmp.numGlyphs*sizeof(*tmp.loc));
    tmp.charDescent = (short*)((char*)tmp.charAscent + tmp.numGlyphs*sizeof(*tmp.charAscent));
    tmp.valid = (char*)tmp.charDescent + tmp.numGlyphs*sizeof(*tmp.charDescent);

    /* Loop through all glyphs marking them as invalid. We don't actually
     * load and rasterise the glyphs until they are needed.
     */
    maxWidth = 0;
    for (i = 0; i < tmp.numGlyphs-1; i++)
        tmp.valid[i] = false;
    TT_Done_Glyph(glyph);
    tmp.maxWidth = maxWidth;

    /* Allocate memory for the font file */
    if ((font = PM_calloc(sizeof(bitmap_font) + (tmp.numGlyphs*FONT_INDEX_SIZE), 1)) == NULL) {
        FATALERROR(grLoadMem);
        return NULL;
        }

    /* Copy the TEMP header information to the MGL font header */
    *font = tmp;

    if ((font->def = PM_calloc(4096, 1)) == NULL) {
        FATALERROR(grLoadMem);
        return NULL;
        }
    font->bitmapBufferUsed = 0;
    font->bitmapBufferSize = 4096;

    /* Set up pointers to the tables within the allocated memory space */
    font->pointSize = (int)(pointSize+0.5);
    font->width = (short*)((char*)font + sizeof(bitmap_font));
    font->offset = (long*)((char*)font->width + font->numGlyphs*sizeof(*font->width));
    font->iwidth = (short*)((char*)font->offset + font->numGlyphs*sizeof(*font->offset));
    font->loc = (short*)((char*)font->iwidth + font->numGlyphs*sizeof(*font->iwidth));
    font->charAscent = (short*)((char*)font->loc + font->numGlyphs*sizeof(*font->loc));
    font->charDescent = (short*)((char*)font->charAscent + font->numGlyphs*sizeof(*font->charAscent));
    font->valid = (char*)font->charDescent + font->numGlyphs*sizeof(*font->charDescent);
    /* Font->def is defined above as ttf now uses 2 seperate areas */
    font->lib = (bitmap_font_lib*)fontlib;

    /* Copy the font table data from _MGL_buf */
    memcpy(font->width,_MGL_buf,font->numGlyphs*FONT_INDEX_SIZE);
    memcpy(font->name,fontlib->name,sizeof(font->name));

    /* Copy TrueType instance to font */
    font->instance = instance;

    /* Create the missing symbol definition.
     * not defined yet, since the character '.' may not exist
     * within the font
     */
    font->iwidth[BIT_MISSINGSYMBOL(font)] = 0;
    font->width[BIT_MISSINGSYMBOL(font)] = 0;
    font->offset[BIT_MISSINGSYMBOL(font)] = 0;
    font->loc[BIT_MISSINGSYMBOL(font)] = 0;
    font->charAscent[BIT_MISSINGSYMBOL(font)] = 0;
    font->charDescent[BIT_MISSINGSYMBOL(font)] = 0;
    font->valid[BIT_MISSINGSYMBOL(font)] = true;
    return (font_t*)font;
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
MGL_openFontLib, MGL_unloadFontInstance, MGL_closeFontLib
****************************************************************************/
font_t * MGLAPI MGL_loadFontInstance(
    font_lib_t *fontlib,
    float pointSize,
    float slant,
    float angle,
    ibool antialiased)
{
    switch (fontlib->fontLibType) {
        case MGL_BITMAPFONT_LIB:
            return loadBitmapFontInstance(fontlib,pointSize,slant,angle,antialiased);
        case MGL_TRUETYPEFONT_LIB:
            return loadTTFontInstance(fontlib,pointSize,slant,angle,antialiased);
        }
    __MGL_result = grBadFontFile;
    return NULL;
}

/****************************************************************************
DESCRIPTION:
Unloads a font file from memory.

HEADER:
mgraph.h

PARAMETERS:
font    - Pointer to font to unload

REMARKS:
Unloads the specified font file from memory, and frees up all the system resources
associated with this font.

SEE ALSO:
MGL_openFontLib, MGL_openFontLibExt
****************************************************************************/
void MGLAPI MGL_closeFontLib(
    font_lib_t *lib)
{
    if (lib) {
        if (lib->fontLibType == MGL_TRUETYPEFONT_LIB)
            TT_Close_Face(((tt_font_lib*)lib)->face);
        if (lib->ownHandle)
            __MGL_fclose(lib->f);
        PM_free(lib);
        }
}

