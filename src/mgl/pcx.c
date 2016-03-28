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
* Description:  PCX bitmap resource loading/unloading routines. We support
*               loading and saving of 1/4/8 bit imagery files. Although
*               the PCX format supports 24 bit images, it is rarely used
*               for this color depth so we dont support it.
*
****************************************************************************/

#include "mgl.h"

/*------------------------- Implementation --------------------------------*/

/****************************************************************************
PARAMETERS:
h           - PCX file header info to fill in
f           - Open binary file to read bitmap from
dwOffset    - Offset to start of bitmap in file

RETURNS:
True on success, false on error.

REMARKS:
Attempts to open the specified PCX file and read the bitmap header
information. If the header information is invalid, we close the file and
return NULL.
****************************************************************************/
static ibool openPCXFileExt(
    PCXHEADER *h,
    FILE *f,
    ulong dwOffset)
{
    size_t  size;

    /* Read in the header record and verify the bitmap file */
    __MGL_result = grOK;
    __MGL_fseek(f,dwOffset,SEEK_SET);
    size = __MGL_fread(h,1,sizeof(PCXHEADER),f);
    if (size != sizeof(PCXHEADER) || h->format != 10 || (int)h->version < 0
            || h->version > 5 || h->rleflag != 1) {
        __MGL_result = grInvalidBitmap;
        return false;
        }
    PM_convLE_16(h->x1);
    PM_convLE_16(h->y1);
    PM_convLE_16(h->x2);
    PM_convLE_16(h->y2);
    PM_convLE_16(h->hres);
    PM_convLE_16(h->vres);
    PM_convLE_16(h->bplin);
    PM_convLE_16(h->scrnw);
    PM_convLE_16(h->scrnh);
    return true;
}

/****************************************************************************
PARAMETERS:
h       - PCX file header info to fill in
PCXName - Name of the bitmap file to load

RETURNS:
Handle to the opened file, NULL on error.

REMARKS:
Attempts to open the specified PCX file and read the bitmap header
information. If the header information is invalid, we close the file
and return NULL.
****************************************************************************/
static FILE *openPCXFile(
    PCXHEADER *h,
    const char *PCXName)
{
    FILE    *f;

    __MGL_result = grOK;
    f = _MGL_openFile(MGL_BITMAPS, PCXName, "rb");
    if (f == NULL) {
        __MGL_result = grBitmapNotFound;
        return NULL;
        }
    if (!openPCXFileExt(h,f,0)) {
        __MGL_fclose(f);
        return NULL;
        }
    return f;
}

/****************************************************************************
RETURNS:
Pointer to file for reading bitmap bits

REMARKS:
Locates the specified bitmap file and loads it into memory. If 'loadPalette'
is true, the palette values for the bitmap will be loaded into the
structure as well (if there is no palette, it will not be loaded),
otherwise the palette entry for the bitmap will be NULL.
****************************************************************************/
static ibool readPCXHeaderExt(
    bitmap_t *bitmap,
    palette_t *pal,
    int *palSize,
    FILE *f,
    ulong dwOffset,
    ulong dwSize,
    ibool loadPalette)
{
    PCXHEADER   header;
    int         i;
    uchar       pcxpal[768],id;

    /* Open PCX header */
    if (!openPCXFileExt(&header,f,dwOffset))
        return false;

    /* Load the header information into the bitmap header. Note that
     * we WORD align the bitmap data, as PCX's are always WORD aligned
     */
    bitmap->width = header.x2 - header.x1 + 1;
    bitmap->height = header.y2 - header.y1 + 1;
    bitmap->bitsPerPixel = header.bitpx * header.nplanes;
    bitmap->bytesPerLine = header.bplin * header.nplanes;

    /* Load the palette values into the local palette if present, or
     * the pixel format information for RGB images
     */
    switch (bitmap->bitsPerPixel) {
        case 1:     *palSize = 2 * sizeof(palette_t);   break;
        case 4:     *palSize = 16 * sizeof(palette_t);  break;
        case 8:     *palSize = 256 * sizeof(palette_t); break;
        case 24:    *palSize = sizeof(pixel_format_t);  break;
        }
    if (loadPalette && bitmap->bitsPerPixel <= 8) {
        if (bitmap->bitsPerPixel < 8) {
            /* The palette for 1 and 4 bit images is located in the
             * colors header for the image.
             */
            int size = *palSize / sizeof(palette_t);
            for (i = 0; i < size; i++) {
                pal[i].red = header.colors[i*3];
                pal[i].green = header.colors[i*3+1];
                pal[i].blue = header.colors[i*3+2];
                pal[i].alpha = 0;
                }
            }
        else {
            /* The palette for 256 color images is stored at the end of
             * the PCX file, with a value of 12 to indicate the start of
             * the palette
             */
            long size = dwSize-sizeof(pcxpal)-1;
            __MGL_fseek(f,size+dwOffset,SEEK_SET);
            __MGL_fread(&id,1,1,f);
            if (id != 12) {
                __MGL_result = grInvalidBitmap;
                return false;
                }
            __MGL_fread(pcxpal,1,sizeof(pcxpal),f);
            for (i = 0; i < 256; i++) {
                pal[i].red = pcxpal[i*3];
                pal[i].green = pcxpal[i*3+1];
                pal[i].blue = pcxpal[i*3+2];
                pal[i].alpha = 0;
                }
            __MGL_fseek(f,sizeof(PCXHEADER)+dwOffset,SEEK_SET);
            }
        }
    return true;
}

/****************************************************************************
DESCRIPTION:
Obtain the dimensions of a PCX file from an opened file.

HEADER:
mgraph.h

PARAMETERS:
f               - Open binary file to read bitmap from
dwOffset        - Offset to start of PCX file
dwSize          - Size of PCX file
width           - Width of bitmap
height          - Height of bitmap
bitsPerPixel    - Pixel depth of bitmap

RETURNS:
True if PCX was found, false if not.

REMARKS:
This function is the same as MGL_getPCXSize, however it works with a
previously opened file. This allows you to create your own large files with
multiple files embedded in them.

SEE ALSO:
MGL_getPCXSize
****************************************************************************/
ibool MGLAPI MGL_getPCXSizeExt(
    FILE *f,
    ulong dwOffset,
    ulong dwSize,
    int *width,
    int *height,
    int *bitsPerPixel)
{
    palette_t   pal[256];
    bitmap_t    bmh;
    int         palSize;

    if (!readPCXHeaderExt(&bmh,pal,&palSize,f,dwOffset,dwSize,false))
        return false;
    *width = bmh.width;
    *height = ABS(bmh.height);
    *bitsPerPixel = bmh.bitsPerPixel;
    (void)dwSize;
    return true;
}

/****************************************************************************
DESCRIPTION:
Obtain the dimensions of a PCX file from disk.

HEADER:
mgraph.h

PARAMETERS:
PCXName         - Name of the bitmap file to load header for
width           - Place to store the bitmap width
height          - Place to store the bitmap height
bitsPerPixel    - Place to store the bitmap pixel depth

RETURNS:
True if the PCX file was found, false if not.

REMARKS:
This functions loads all the header information for a PCX file from disk, without
actually loading the bits for the bitmap surface. This is useful to determine the
dimensions and pixel format for the bitmap before it is loaded, so you can create an
appropriate memory device context that you can load the bitmap into with the
MGL_loadPCXIntoDC function.

SEE ALSO:
MGL_loadPCX, MGL_loadPCXIntoDC
****************************************************************************/
ibool MGLAPI MGL_getPCXSize(
    const char *PCXName,
    int *width,
    int *height,
    int *bitsPerPixel)
{
    FILE    *f;
    ibool   ret;

    __MGL_result = grOK;
    if ((f = _MGL_openFile(MGL_BITMAPS, PCXName, "rb")) == NULL) {
        __MGL_result = grBitmapNotFound;
        return false;
        }
    ret = MGL_getPCXSizeExt(f,0,_MGL_fileSize(f),width,height,bitsPerPixel);
    __MGL_fclose(f);
    return ret;
}

static int      gcount,gdata,bufSize;
static uchar    *bufStart,*cStart,*cEnd;

/****************************************************************************
PARAMETERS:
f   - File to read data from

REMARKS:
Reads a large chunk of data from disk into the MGL scratch buffer for
decoding our PCX bitmap data from. Because we read large chunks at a time
(32Kb for protected mode) it vastly speeds up the time to read and
decode a PCX file from disk.
****************************************************************************/
static void readChunk(
    FILE *f)
{
    ulong size;
    size = fread(bufStart,1,bufSize,f);
    cStart = bufStart;
    cEnd = cStart + size;
}

/****************************************************************************
PARAMETERS:
f   - File to read data from

REMARKS:
Writes a large chunk of data from disk from the MGL scratch buffer for
encoding our PCX bitmap data into.
****************************************************************************/
static void writeChunk(
    FILE *f)
{
    fwrite(bufStart,1,cStart - bufStart,f);
    cStart = bufStart;
    cEnd = cStart + bufSize;
}

#define PCX_GETC(data,f)    \
    if (cStart == cEnd)     \
        readChunk(f);       \
    (data) = *cStart++

#define PCX_PUTC(data,f)    \
    *cStart++ = (data);     \
    if (cStart == cEnd)     \
        writeChunk(f)

/****************************************************************************
PARAMETERS:
PCXName     - Name of bitmap to get size info for
p           - Pointer to buffer to decode into
rowbyte     - Number of bytes in a row to decode

REMARKS:
Decodes a single scanline of data from the PCX file. Note that this routine
properly handles runs that cross a scanline boundary, which the PCX file
format allows.
****************************************************************************/
static void decodeScan(
    FILE *f,
    uchar *p,
    int rowbytes)
{
    int data = gdata,count = gcount,bytecnt = 0;

    while (bytecnt < rowbytes) {
        /* Read another run from the file */
        if (count < 1) {
            PCX_GETC(data,f);
            if ((data & PCX_RUN_MASK) == PCX_RUN_MASK) {
                count = data & PCX_CNT_MASK;
                PCX_GETC(data,f);
                }
            else
                count = 1;
            }

        /* Decode the run into the bitmap */
        while (count--) {
            *p++ = data;
            if (++bytecnt == rowbytes)
                break;
            }
        }
    gdata = data;
    gcount = count;
}

/****************************************************************************
PARAMETERS:
PCXName     - Name of bitmap to get size info for
p           - Pointer to buffer to decode into
rowbyte     - Number of bytes in a row to decode

REMARKS:
Decodes a single scanline of data from the PCX file. Note that this routine
properly handles runs that cross a scanline boundary, which the PCX file
format allows.

This version decodes a 4bpp planar scanline into a 4bpp packed pixel format,
used to store 4bpp bitmaps internally.
****************************************************************************/
static void decodeScan4(
    FILE *f,
    uchar *dst,
    int rowbytes)
{
    int     data = gdata,count = gcount,bytecnt = 0;
    int     planebytes = rowbytes / 4;
    int     planecnt = 0;
    uchar   *p;

    p = dst;
    while (bytecnt < planebytes && planecnt < 4) {
        /* Read another run from the file */
        if (count < 1) {
            PCX_GETC(data,f);
            if ((data & PCX_RUN_MASK) == PCX_RUN_MASK) {
                count = data & PCX_CNT_MASK;
                PCX_GETC(data,f);
                }
            else
                count = 1;
            }

        /* Decode the run into the bitmap */
        while (count--) {
            if (planecnt == 0) {
                if (data & 0x80) p[0]  = 0x10; else p[0] = 0x00;
                if (data & 0x40) p[0] |= 0x01;
                if (data & 0x20) p[1]  = 0x10; else p[1] = 0x00;
                if (data & 0x10) p[1] |= 0x01;
                if (data & 0x08) p[2]  = 0x10; else p[2] = 0x00;
                if (data & 0x04) p[2] |= 0x01;
                if (data & 0x02) p[3]  = 0x10; else p[3] = 0x00;
                if (data & 0x01) p[3] |= 0x01;
                }
            else if (planecnt == 1) {
                if (data & 0x80) p[0] |= 0x20;
                if (data & 0x40) p[0] |= 0x02;
                if (data & 0x20) p[1] |= 0x20;
                if (data & 0x10) p[1] |= 0x02;
                if (data & 0x08) p[2] |= 0x20;
                if (data & 0x04) p[2] |= 0x02;
                if (data & 0x02) p[3] |= 0x20;
                if (data & 0x01) p[3] |= 0x02;
                }
            else if (planecnt == 2) {
                if (data & 0x80) p[0] |= 0x40;
                if (data & 0x40) p[0] |= 0x04;
                if (data & 0x20) p[1] |= 0x40;
                if (data & 0x10) p[1] |= 0x04;
                if (data & 0x08) p[2] |= 0x40;
                if (data & 0x04) p[2] |= 0x04;
                if (data & 0x02) p[3] |= 0x40;
                if (data & 0x01) p[3] |= 0x04;
                }
            else {
                if (data & 0x80) p[0] |= 0x80;
                if (data & 0x40) p[0] |= 0x08;
                if (data & 0x20) p[1] |= 0x80;
                if (data & 0x10) p[1] |= 0x08;
                if (data & 0x08) p[2] |= 0x80;
                if (data & 0x04) p[2] |= 0x08;
                if (data & 0x02) p[3] |= 0x80;
                if (data & 0x01) p[3] |= 0x08;
                }
            if (++bytecnt == planebytes) {
                planecnt++;
                bytecnt = 0;
                p = dst;
                break;
                }
            p += 4;
            }
        }
    gdata = data;
    gcount = count;
}

/****************************************************************************
PARAMETERS:
PCXName     - Name of bitmap to get size info for
p           - Pointer to buffer to decode into
rowbyte     - Number of bytes in a row to decode

REMARKS:
Decodes a single scanline of data from the PCX file. Note that this routine
properly handles runs that cross a scanline boundary, which the PCX file
format allows.

This version decoded 4bpp scanline data into 8bpp scanline data.
****************************************************************************/
static void decodeScan4to8(
    FILE *f,
    uchar *dst,
    int rowbytes)
{
    int     data = gdata,count = gcount,bytecnt = 0;
    int     planebytes = rowbytes / 4;
    int     planecnt = 0;
    uchar   *p;

    p = dst;
    while (bytecnt < planebytes && planecnt < 4) {
        /* Read another run from the file */
        if (count < 1) {
            PCX_GETC(data,f);
            if ((data & PCX_RUN_MASK) == PCX_RUN_MASK) {
                count = data & PCX_CNT_MASK;
                PCX_GETC(data,f);
                }
            else
                count = 1;
            }

        /* Decode the run into the bitmap */
        while (count--) {
            if (planecnt == 0) {
                if (data & 0x80) p[0] = 0x01; else p[0] = 0x00;
                if (data & 0x40) p[1] = 0x01; else p[1] = 0x00;
                if (data & 0x20) p[2] = 0x01; else p[2] = 0x00;
                if (data & 0x10) p[3] = 0x01; else p[3] = 0x00;
                if (data & 0x08) p[4] = 0x01; else p[4] = 0x00;
                if (data & 0x04) p[5] = 0x01; else p[5] = 0x00;
                if (data & 0x02) p[6] = 0x01; else p[6] = 0x00;
                if (data & 0x01) p[7] = 0x01; else p[7] = 0x00;
                }
            else if (planecnt == 1) {
                if (data & 0x80) p[0] |= 0x02;
                if (data & 0x40) p[1] |= 0x02;
                if (data & 0x20) p[2] |= 0x02;
                if (data & 0x10) p[3] |= 0x02;
                if (data & 0x08) p[4] |= 0x02;
                if (data & 0x04) p[5] |= 0x02;
                if (data & 0x02) p[6] |= 0x02;
                if (data & 0x01) p[7] |= 0x02;
                }
            else if (planecnt == 2) {
                if (data & 0x80) p[0] |= 0x04;
                if (data & 0x40) p[1] |= 0x04;
                if (data & 0x20) p[2] |= 0x04;
                if (data & 0x10) p[3] |= 0x04;
                if (data & 0x08) p[4] |= 0x04;
                if (data & 0x04) p[5] |= 0x04;
                if (data & 0x02) p[6] |= 0x04;
                if (data & 0x01) p[7] |= 0x04;
                }
            else {
                if (data & 0x80) p[0] |= 0x08;
                if (data & 0x40) p[1] |= 0x08;
                if (data & 0x20) p[2] |= 0x08;
                if (data & 0x10) p[3] |= 0x08;
                if (data & 0x08) p[4] |= 0x08;
                if (data & 0x04) p[5] |= 0x08;
                if (data & 0x02) p[6] |= 0x08;
                if (data & 0x01) p[7] |= 0x08;
                }
            if (++bytecnt == planebytes) {
                planecnt++;
                bytecnt = 0;
                p = dst;
                break;
                }
            p += 8;
            }
        }
    gdata = data;
    gcount = count;
}

/****************************************************************************
PARAMETERS:
PCXName     - Name of bitmap to get size info for
p           - Pointer to buffer to decode into
rowbyte     - Number of bytes in a row to decode

REMARKS:
Decodes a single scanline of data from the PCX file. Note that this routine
properly handles runs that cross a scanline boundary, which the PCX file
format allows.

This version decodes planar 24bpp scanline data into packed 24bpp format.
****************************************************************************/
static void decodeScan24(
    FILE *f,
    uchar *dst,
    int rowbytes)
{
    int     data = gdata,count = gcount,bytecnt = 0;
    int     planebytes = rowbytes / 3;
    int     planecnt = 0;
    uchar   *p;

    p = dst+2;
    while (bytecnt < planebytes && planecnt < 3) {
        /* Read another run from the file */
        if (count < 1) {
            PCX_GETC(data,f);
            if ((data & PCX_RUN_MASK) == PCX_RUN_MASK) {
                count = data & PCX_CNT_MASK;
                PCX_GETC(data,f);
                }
            else
                count = 1;
            }

        /* Decode the run into the bitmap */
        while (count--) {
            *p = data;
            p += 3;
            if (++bytecnt == planebytes) {
                planecnt++;
                bytecnt = 0;
                p = dst+2-planecnt;
                break;
                }
            }
        }
    gdata = data;
    gcount = count;
}

/****************************************************************************
PARAMETERS:
PCXName     - Name of bitmap to get size info for
p           - Pointer to buffer to encode from
rowbyte     - Number of bytes in a row to encode

REMARKS:
Encodes a single scanline of data to the PCX file.
****************************************************************************/
static void encodeScan(
    FILE *f,
    uchar *p,
    int rowbytes)
{
    int n,n1,n2 = 0;

    while (n2 < rowbytes) {
        /* Compute number of bytes in this run */
        n1 = n2;
        n2 = n1 + 1;
        while (p[n2] == p[n1] && (n2 < rowbytes))
            n2++;
        n = n2 - n1;

        /* Write multiple runs to disk */
        while (n > PCX_MAX_RUN) {
            PCX_PUTC(0xFF,f);
            PCX_PUTC(p[n1],f);
            n -= PCX_MAX_RUN;
            }

        /* Write final run to disk */
        if ((n > 1) || ((p[n1] & PCX_RUN_MASK) == PCX_RUN_MASK))
            PCX_PUTC(n | PCX_RUN_MASK,f);
        PCX_PUTC(p[n1],f);
        }
}

/****************************************************************************
DESCRIPTION:
Load a lightweight PCX file from disk using an open file.

HEADER:
mgraph.h

PARAMETERS:
f           - Open binary file to read bitmap from
dwOffset    - Offset to start of PCX file within open file
dwSize      - Size of PCX file in bytes
loadPalette - If true, palette values are loaded as well.

RETURNS:
Pointer to the loaded bitmap file

REMARKS:
This function is the same as MGL_loadPCX, however it loads the file from a
previously open file. This allows you to create your own large files with
multiple files embedded in them.

SEE ALSO:
MGL_loadPCX, MGL_loadBitmap
****************************************************************************/
bitmap_t * MGLAPI MGL_loadPCXExt(
    FILE *f,
    ulong dwOffset,
    ulong dwSize,
    ibool loadPalette)
{
    bitmap_t    bmh,*bitmap;
    palette_t   pal[256];       /* Temporary space for palette  */
    long        size;
    int         i,palSize;
    uchar       *p;

    /* Read the bitmap header information */
    if (!readPCXHeaderExt(&bmh,pal,&palSize,f,dwOffset,dwSize,loadPalette))
        return NULL;

    /* Allocate memory for the bitmap */
    if (!loadPalette && bmh.bitsPerPixel <= 8)
        palSize = 0;
    size = (long)bmh.bytesPerLine * bmh.height + palSize;
    if ((bitmap = PM_malloc(sizeof(bitmap_t)+size)) == NULL) {
        FATALERROR(grNoMem);
        return NULL;
        }
    *bitmap = bmh;
    if (bitmap->bitsPerPixel <= 8) {
        if (palSize) {
            bitmap->pal = (palette_t*)((uchar*)bitmap + sizeof(bitmap_t));
            memcpy(bitmap->pal,pal,palSize);
            }
        else
            bitmap->pal = NULL;
        bitmap->pf = NULL;
        }
    else {
        bitmap->pal = NULL;
        bitmap->pf = (pixel_format_t*)((uchar*)bitmap + sizeof(bitmap_t));
        *bitmap->pf = _MGL_pixelFormats[pfRGB24];
        }
    bitmap->surface = (uchar*)bitmap + sizeof(bitmap_t) + palSize;

    /* Decode the pixels in the bitmap */
    p = bitmap->surface;
    bufStart = (uchar*)_MGL_buf;
    bufSize = _MGL_bufSize;
    readChunk(f);
    gcount = gdata = 0;
    if (bitmap->bitsPerPixel == 4) {
        for (i = 0; i < bitmap->height; i++, p += bitmap->bytesPerLine) {
            decodeScan4(f,(uchar*)p,bitmap->bytesPerLine);
            }
        }
    else if (bitmap->bitsPerPixel == 24) {
        for (i = 0; i < bitmap->height; i++, p += bitmap->bytesPerLine) {
            decodeScan24(f,(uchar*)p,bitmap->bytesPerLine);
            }
        }
    else {
        for (i = 0; i < bitmap->height; i++, p += bitmap->bytesPerLine) {
            decodeScan(f,(uchar*)p,bitmap->bytesPerLine);
            }
        }
    return bitmap;
}

/****************************************************************************
DESCRIPTION:
Load a lightweight PCX file from disk.

HEADER:
mgraph.h

PARAMETERS:
PCXName     - Name of PCX file to load
loadPalette - True if the palette should also be loaded

RETURNS:
Pointer to the loaded PCX file, NULL on error.

REMARKS:
Locates the specified PCX file and loads it into a lightweight bitmap
structure. MGL can load any 1, 4, 8 or 24 bits per pixel PCX file.

If loadPalette is true, the palette values for the PCX will be loaded into
the structure as well (if there is no palette, it will not be loaded),
otherwise the palette entry for the PCX will be NULL. For small PCX files
you can save space by not loading the palette for the PCX.

When MGL is searching for PCX files it will first attempt to find the files
just by using the filename itself. Hence if you wish to look for a specific
PCX file, you should pass the full pathname to the file that you are
interested in. If the filename is a simple relative filename (i.e.
"MYBMP.PCX"), MGL will then search in the BITMAPS directory relative to the
path specified in mglpath variable that was passed to MGL_init. As a final
resort MGL will also look for the files in the BITMAPS directory relative
to the MGL_ROOT environment variable.

If the PCX file was not found, or an error occurred while reading the PCX
file, this function will return NULL. You can check the MGL_result error
code to determine the cause.

The routine allocates a lightweight bitmap structure for holding the PCX
file, which loads the bitmap with the minimum memory overheads. You can draw
the PCX file on any device context surface using the MGL_putBitmap function,
but you don't have the full flexibility of using a memory device context for
the bitmap surface. If you need more control over the bitmap, you can
allocate a memory device context to hold the bitmap data and load the bitmap
with the MGL_loadPCXIntoDC function.

SEE ALSO:
MGL_unloadBitmap, MGL_availablePCX, MGL_getPCXSize,
MGL_loadPCXIntoDC, MGL_savePCXFromDC, MGL_putBitmap, MGL_loadPCXExt
****************************************************************************/
bitmap_t * MGLAPI MGL_loadPCX(
    const char *PCXName,
    ibool loadPalette)
{
    FILE        *f;
    bitmap_t    *b;

    __MGL_result = grOK;
    if ((f = _MGL_openFile(MGL_BITMAPS, PCXName, "rb")) == NULL) {
        __MGL_result = grBitmapNotFound;
        return false;
        }
    b = MGL_loadPCXExt(f,0,_MGL_fileSize(f),loadPalette);
    __MGL_fclose(f);
    return b;
}

/****************************************************************************
DESCRIPTION:
Load a lightweight PCX file from disk using an open file.

HEADER:
mgraph.h

PARAMETERS:
dc          - Device context to load bitmap into
f           - Open binary file to read bitmap from
dwOffset    - Offset to start of PCX file within open file
dwSize      - Size of PCX file in bytes
dstLeft     - Left coordinate to load PCX at
dstTop      - Top coordinate to load PCX at
loadPalette - True if the palette should also be loaded

RETURNS:
Pointer to the loaded PCX file, NULL on error.

REMARKS:
This function is the same as MGL_loadPCXIntoDC, however it loads the file
from a previously open file. This allows you to create your own large files
with multiple files embedded in them.

SEE ALSO:
MGL_loadPCXIntoDC, MGL_loadBitmapIntoDC
****************************************************************************/
ibool MGLAPI MGL_loadPCXIntoDCExt(
    MGLDC *dc,
    FILE *f,
    ulong dwOffset,
    ulong dwSize,
    int dstLeft,
    int dstTop,
    ibool loadPalette)
{
    bitmap_t            bmh;
    palette_t           pal[256];       /* Temporary space for palette  */
    int                 i,palSize,height;
    int                 decodeBytes;
    ibool               oldCheckId;

    /* Read the bitmap header */
    if (!readPCXHeaderExt(&bmh,pal,&palSize,f,dwOffset,dwSize,loadPalette))
        return false;

    /* Allocate a temporary bitmap to convert the scanlines */
    bmh.pal = pal;
    bmh.pf = &_MGL_pixelFormats[pfRGB24];
    if (bmh.bitsPerPixel == 4) {
        /* The temporary scanline is actually 8bpp when decoding 4bpp images */
        if ((bmh.surface = PM_malloc(bmh.width)) == NULL) {
            FATALERROR(grNoMem);
            return false;
            }
        }
    else if ((bmh.surface = PM_malloc(bmh.bytesPerLine)) == NULL) {
        FATALERROR(grNoMem);
        return false;
        }
    oldCheckId = MGL_checkIdentityPalette(false);

    /* Store the palette in the destination DC */
    if (loadPalette && (bmh.bitsPerPixel == 4 || bmh.bitsPerPixel == 8)) {
        MGL_setPalette(dc,pal,palSize / sizeof(palette_t),0);
        if (MGL_getVisualPage(dc) == MGL_getActivePage(dc))
            MGL_realizePalette(dc,palSize / sizeof(palette_t),0,false);
        }
    else if (loadPalette && ((dc->mi.modeFlags & MGL_IS_COLOR_INDEX) && bmh.bitsPerPixel > 8)) {
        MGL_getHalfTonePalette(pal);
        MGL_setPalette(dc,pal,MGL_getPaletteSize(dc),0);
        if (MGL_getVisualPage(dc) == MGL_getActivePage(dc))
            MGL_realizePalette(dc,MGL_getPaletteSize(dc),0,false);
        }

    /* Now read in the bits in the bitmap, by reading the data a scanline
     * at a time into our temporary memory DC, and then blting this to
     * the destination DC.
     */
    bufStart = (uchar*)_MGL_buf;
    bufSize = _MGL_bufSize;
    readChunk(f);
    gcount = gdata = 0;
    height = bmh.height;
    bmh.height = 1;
    if (bmh.bitsPerPixel == 4) {
        /* To decode 4bpp PCX planar images, we decode the planar scanline
         * into a temporary decode buffer, and then combine the decoded
         * planes into a temporary 8bpp memory buffer. Finally the 8bpp
         * memory buffer is blitted to the display DC (we use 8bpp bitmaps
         * for efficiency).
         */
        decodeBytes = bmh.bytesPerLine;
        bmh.bytesPerLine = bmh.width;
        bmh.bitsPerPixel = 8;
        for (i = 0; i < height; i++) {
            decodeScan4to8(f,bmh.surface,decodeBytes);
            MGL_putBitmap(dc,dstLeft,dstTop+i,&bmh,MGL_REPLACE_MODE);
            }
        }
    else if (bmh.bitsPerPixel == 24) {
        /* 24-bit images are run length encoded by stored plane by plane */
        for (i = 0; i < height; i++) {
            decodeScan24(f,bmh.surface,bmh.bytesPerLine);
            MGL_putBitmap(dc,dstLeft,dstTop+i,&bmh,MGL_REPLACE_MODE);
            }
        }
    else {
        /* 8-bit images are simply run length encoded and easy to decode */
        for (i = 0; i < height; i++) {
            decodeScan(f,bmh.surface,bmh.bytesPerLine);
            MGL_putBitmap(dc,dstLeft,dstTop+i,&bmh,MGL_REPLACE_MODE);
            }
        }
    PM_free(bmh.surface);
    MGL_checkIdentityPalette(oldCheckId);
    return true;
}

/****************************************************************************
DESCRIPTION:
Loads a PCX file directly into an existing device context.

HEADER:
mgraph.h

PARAMETERS:
dc          - Device context to load bitmap into
PCXName     - Name of PCX file to load
dstLeft     - Left coordinate to load PCX at
dstTop      - Top coordinate to load PCX at
loadPalette - True if the palette should also be loaded

RETURNS:
True if the PCX file was loaded, false on error.

REMARKS:
Locates the specified PCX file and loads it into the specified device context
at the specified destination coordinates. If the PCX is of a different pixel
depth than the device context that it is being loaded into, the PCX will be
converted as it is loaded to the pixel format of the device context it is
being loaded into. MGL can load any 1, 4, 8 or 24 bits per pixel PCX file.

If loadPalette is true, the palette values for the PCX will be loaded and
stored in the device context's palette. If the device context being loaded
into is the currently active display device, the palette will also be
realized before the bits in the bitmap are loaded.

When MGL is searching for bitmap files it will first attempt to find the
files just by using the filename itself. Hence if you wish to look for a
specific bitmap file, you should pass the full pathname to the file that
you are interested in. If the filename is a simple relative filename (i.e.
"MYFILE.PCX"), MGL will then search in the BITMAPS directory relative to the
path specified in mglpath variable that was passed to MGL_init. As a final
resort MGL will also look for the files in the BITMAPS directory relative
to the MGL_ROOT environment variable.

If the bitmap file was not found, or an error occurred while reading the
bitmap file, this function will return false. You can check the MGL_result
error code to determine the cause.

SEE ALSO:
MGL_availablePCX, MGL_getPCXSize,MGL_loadPCX, MGL_savePCXFromDC
****************************************************************************/
ibool MGLAPI MGL_loadPCXIntoDC(
    MGLDC *dc,
    const char *PCXName,
    int dstLeft,
    int dstTop,
    ibool loadPalette)
{
    FILE    *f;
    ibool   ret;

    __MGL_result = grOK;
    if ((f = _MGL_openFile(MGL_BITMAPS, PCXName, "rb")) == NULL) {
        __MGL_result = grBitmapNotFound;
        return false;
        }
    ret = MGL_loadPCXIntoDCExt(dc,f,0,_MGL_fileSize(f),dstLeft,dstTop,loadPalette);
    __MGL_fclose(f);
    return ret;
}

/****************************************************************************
PARAMTERS:
dc      - Device context to create bitmap from
left    - Left coordinate of rectangle to save
top     - Top coordinate of rectangle to save
right   - Right coordinate of rectangle to save
bottom  - Bottom coordinate of rectangle to save
bitmap  - Bitmap header to fill in
palSize - Place to store size of palette

REMARKS:
Fills in the bitmap header for the sub-portion of the specified device
context.
****************************************************************************/
static void createBitmapHeader(
    MGLDC *dc,
    int *left,
    int *top,
    int *right,
    int *bottom,
    bitmap_t *bitmap)
{
    int     bytesPerLine;

    /* Clip the source rectangle to the device context bounds */
    *left += dc->viewPort.left;
    *right += dc->viewPort.left;
    *top += dc->viewPort.top;
    *bottom += dc->viewPort.top;
    if (*left < 0) *left = 0;
    if (*right > (dc->mi.xRes+1)) *right = dc->mi.xRes+1;
    if (*top < 0) *top = 0;
    if (*bottom > (dc->mi.yRes+1)) *bottom = dc->mi.yRes+1;

    /* Create the header information into the bitmap header. Note that
     * we WORD align the bitmap data, as PCX's are always WORD aligned
     */
    bitmap->width = (*right-*left);
    bitmap->height = (*bottom-*top);
    bitmap->bitsPerPixel = dc->mi.bitsPerPixel;
    switch (dc->mi.bitsPerPixel) {
        case 1:     bytesPerLine = (bitmap->width+7)/8; break;
        case 4:     bytesPerLine = (bitmap->width+1)/2; break;
        case 8:     bytesPerLine = bitmap->width;       break;
        case 15:
        case 16:    bytesPerLine = bitmap->width * 2;   break;
        case 24:    bytesPerLine = bitmap->width * 3;   break;
        default:    bytesPerLine = bitmap->width * 4;   break;
        }
    bitmap->bytesPerLine = (bytesPerLine + 1) & ~1;
}

/****************************************************************************
DESCRIPTION:
Save a portion of a device context to PCX on disk.

HEADER:
mgraph.h

PARAMETERS:
dc  - Device context to save
PCXName - Name of bitmap file to save
left    - Left coordinate of bitmap to save
top - Top coordinate of bitmap to save
right   - Right coordinate of bitmap to save
bottom  - Bottom coordinate of bitmap to save

RETURNS:
True on success, false on error.

REMARKS:
This function saves a portion of a device context as a PCX format bitmap file to
disk. If this function fails for some reason, it will return false and you can get the
error code from the MGL_result function.

Note that the source rectangle for the bitmap to be saved is not clipped to the
current clipping rectangle for the device context, but it is mapped to the current
viewport. If you specify dimensions that are larger than the current device context,
you will get garbage in the bitmap file as a result.

Note that MGL currently only supports saving bitmap data to PCX files from 8 bits
per pixel device contexts.

SEE ALSO:
MGL_LoadPCX,MGL_loadPCXIntoDC
****************************************************************************/
ibool MGLAPI MGL_savePCXFromDC(
    MGLDC *dc,
    const char *PCXName,
    int left,
    int top,
    int right,
    int bottom)
{
    FILE        *f;
    bitmap_t    bmh;
    PCXHEADER   hdr;
    int         i;
    uchar       *p,id;
    palette_t   *pal = (palette_t*)dc->colorTab;

    /* Build the bitmap header for the image */
    __MGL_result = grOK;
    createBitmapHeader(dc,&left,&top,&right,&bottom,&bmh);
    if (bmh.bitsPerPixel != 8) {
        __MGL_result = grInvalidBitmap;
        return false;           /* Only supports 8 bit PCX files */
        }

    /* Attempt to open the file for writing */
    if ((f = __MGL_fopen(PCXName,"wb")) == NULL) {
        __MGL_result = grBitmapNotFound;
        return false;
        }

    /* Build the bitmap file header and write to disk */
    memset(&hdr,0,sizeof(hdr));
    hdr.format = 10;
    hdr.version = 5;
    hdr.rleflag = 1;
    hdr.paltype = 1;
    hdr.bitpx = 8;
    hdr.nplanes = 1;
    putLEShort(hdr.x1,0);
    putLEShort(hdr.x2,bmh.width-1);
    putLEShort(hdr.y1,0);
    putLEShort(hdr.y2,bmh.height-1);
    putLEShort(hdr.bplin,bmh.bytesPerLine);
    putLEShort(hdr.hres,100);
    putLEShort(hdr.vres,100);
    __MGL_fwrite(&hdr,1,sizeof(hdr),f);

    /* Now write the bits from the device context to disk */
    p = _MGL_buf;
    bufSize = _MGL_bufSize/2;
    bufStart = (uchar*)_MGL_buf + bufSize;
    memset(p,bmh.bytesPerLine,0);
    cStart = bufStart;
    cEnd = cStart + bufSize;
    for (i = top; i < bottom; i++) {
        dc->r.GetBitmapSys(p,bmh.bytesPerLine,left,i,right-left,1,0,0,GA_REPLACE_MIX);
        encodeScan(f,(uchar*)p,bmh.bytesPerLine);
        }
    writeChunk(f);

    /* Now write the palette data to disk after the image data */
    id = 12;
    __MGL_fwrite(&id,1,1,f);
    for (i = 0; i < 256; i++, pal++) {
        __MGL_fwrite(&pal->red,1,1,f);
        __MGL_fwrite(&pal->green,1,1,f);
        __MGL_fwrite(&pal->blue,1,1,f);
        }
    __MGL_fclose(f);
    return true;
}

/****************************************************************************
DESCRIPTION:
Determines if the specified PCX bitmap file is available for use.

HEADER:
mgraph.h

PARAMETERS:
PCXName     - Name of PCX bitmap file to check for

RETURNS:
True if the PCX bitmap file exists, false if not.

REMARKS:
SEE ALSO:
****************************************************************************/
ibool MGLAPI MGL_availablePCX(
    const char *PCXName)
{
    FILE        *f;
    PCXHEADER   header;

    __MGL_result = grOK;
    if ((f = openPCXFile(&header,PCXName)) == NULL)
        return false;
    __MGL_fclose(f);
    return true;
}
