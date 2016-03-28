/****************************************************************************
*
*                    SciTech SNAP Graphics Architecture
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
* Environment:  Any 32-bit protected mode environment
*
* Description:  Bitmap loading/saving module.
*
****************************************************************************/

#include "snap/graphics.h"
#include "snap/graphics/gapf.h"
#include "snap/graphics/gabmp.h"
#include "resource.h"
#include <string.h>

/*------------------------- Implementation --------------------------------*/

/****************************************************************************
PARAMETERS:
h           - Bitmap header info to fill in
b           - Bitmap info structure to fill info
f           - Open binary file to read bitmap from

RETURNS:
True on success, false on error.

REMARKS:
Attempts to open the specified bitmap and read the bitmap header information.
If the header information is invalid, we close the file and return NULL.
****************************************************************************/
static ibool openBitmapFile(
    winBITMAPFILEHEADER *h,
    winBITMAPINFOHEADER *b,
    FILE *f)
{
    size_t  size;

    /* Read in the header record and verify the bitmap file */
    fseek(f,0,SEEK_SET);
    size = fread(h,1,sizeof(winBITMAPFILEHEADER),f);
    if (size != sizeof(winBITMAPFILEHEADER) || h->bfType[0] != 'B' || h->bfType[1] != 'M')
        return false;

    /* Read in the bitmap info header and verify it */
    size = fread(b,1,sizeof(winBITMAPINFOHEADER),f);
    if (size != sizeof(winBITMAPINFOHEADER) || PM_getLE_32(b->biSize) != 0x28
            || PM_getLE_16(b->biPlanes) != 1 ||
            (PM_getLE_32(b->biCompression) != winBI_RGB && PM_getLE_16(b->biBitCount) <= 8))
        return false;
    return true;
}

/****************************************************************************
PARAMETERS:
bitmap      - Place to store the bitmap header
pal         - Place to store the palette of loaded
palSize     - Place to store the size of the palette
pf          - Place to store the pixel format if loaded
f           - File to read header from
loadPalette - True to load palette information if palettised

RETURNS:
Pointer to file for reading bitmap bits

REMARKS:
Loads the bitmap header information from disk.
****************************************************************************/
static ibool readBitmapHeader(
    bmp_t *bitmap,
    GA_palette *pal,
    int *palSize,
    GA_pixelFormat *pf,
    FILE *f,
    ibool loadPalette)
{
    winBITMAPFILEHEADER header;
    winBITMAPINFOHEADER bmInfo;
    N_int32             bytesPerLine,clrUsed;
    N_uint32            masks[3];

    /* Open the bitmap header */
    if (!openBitmapFile(&header,&bmInfo,f))
        return false;

    /* Load the palette values into the local palette if present, or
     * the pixel format information for RGB images
     */
    switch (PM_getLE_16(bmInfo.biBitCount)) {
        case 1:     *palSize = 2 * sizeof(GA_palette);      break;
        case 4:     *palSize = 16 * sizeof(GA_palette);     break;
        case 8:     *palSize = 256 * sizeof(GA_palette);    break;
        default:    *palSize = sizeof(GA_pixelFormat);      break;
        }
    if (PM_getLE_16(bmInfo.biBitCount) <= 8) {
        if ((clrUsed = PM_getLE_32(bmInfo.biClrUsed) * sizeof(GA_palette)) == 0)
            clrUsed = *palSize;
        if (loadPalette)
            fread(pal,1,clrUsed,f);
        else
            fseek(f,clrUsed,SEEK_CUR);
        }
    else if (PM_getLE_32(bmInfo.biCompression) == winBI_BITFIELDS) {
        /* Load the BI_BITFIELDS info from the first three palette
         * entries for new style bitmap formats.
         */
        fread(masks,1,3 * sizeof(N_uint32),f);
        }

    /* Load the header information into the bitmap header. Note that
     * we DWORD align the bitmap data, as DIB's are always DWORD aligned
     */
    bitmap->width = PM_getLE_32(bmInfo.biWidth);
    bitmap->height = PM_getLE_32(bmInfo.biHeight);
    bitmap->bitsPerPixel = PM_getLE_16(bmInfo.biBitCount);
    switch (bitmap->bitsPerPixel) {
        case 1:     bytesPerLine = (bitmap->width+7)/8; break;
        case 4:     bytesPerLine = (bitmap->width+1)/2; break;
        case 8:     bytesPerLine = bitmap->width;       break;
        case 16:    bytesPerLine = bitmap->width * 2;   break;
        case 24:    bytesPerLine = bitmap->width * 3;   break;
        case 32:    bytesPerLine = bitmap->width * 4;   break;
        default:
            return false;
        }
    bitmap->bytesPerLine = (bytesPerLine + 3) & ~3;

    /* Create a pixel format block for 15 bit and higher bitmaps */
    if (bitmap->bitsPerPixel > 8) {
        switch (bitmap->bitsPerPixel) {
            case 16:
                if (PM_getLE_32(bmInfo.biCompression) == winBI_BITFIELDS && masks[1] == 0x7E0UL)
                    *pf = GA_pf16;
                else {
                    *pf = GA_pf15;
                    bitmap->bitsPerPixel = 15;
                    }
                break;
            case 24:
                if (PM_getLE_32(bmInfo.biCompression) == winBI_BITFIELDS && masks[0] == 0xFFUL)
                    *pf = GA_pf24_BGR;
                else
                    *pf = GA_pf24_RGB;
                break;
            case 32:
                if (PM_getLE_32(bmInfo.biCompression) == winBI_BITFIELDS) {
                    if (masks[0] == 0xFFUL)
                        *pf = GA_pf32_ABGR;
                    else if (masks[0] == 0xFF0000UL)
                        *pf = GA_pf32_ARGB;
                    else if (masks[0] == 0xFF00UL)
                        *pf = GA_pf32_BGRA;
                    else
                        *pf = GA_pf32_RGBA;
                    }
                else
                    *pf = GA_pf32_ARGB;
                break;
            }
        }
    return true;
}

/****************************************************************************
PARAMETERS:
bitmapName  - Name of bitmap file to load
loadPalette - True if the palette should also be loaded

RETURNS:
Pointer to the loaded bitmap file, NULL on error.

REMARKS:
Load a bitmap file from disk into memory.
****************************************************************************/
bmp_t * LoadBitmap(
    const char *bitmapName,
    ibool loadPalette)
{
    FILE            *f;
    bmp_t           bmh,*bitmap;
    GA_palette      pal[256];
    GA_pixelFormat  pf;
    N_int32         size;
    int             i,palSize;
    N_uint8         *p;

    /* Open the bitmap header */
    if ((f = fopen(bitmapName, "rb")) == NULL)
        return NULL;
    if (!readBitmapHeader(&bmh,pal,&palSize,&pf,f,loadPalette))
        return NULL;

    /* Allocate memory for the bitmap */
    if (!loadPalette)
        palSize = 0;
    size = (N_int32)bmh.bytesPerLine * bmh.height + palSize;
    if ((bitmap = malloc(sizeof(bmp_t)+size)) == NULL)
        return NULL;
    *bitmap = bmh;
    size = sizeof(bmp_t);
    if (bitmap->bitsPerPixel <= 8) {
        if (palSize) {
            bitmap->pal = (GA_palette*)((N_uint8*)bitmap + size);
            memcpy(bitmap->pal,pal,palSize);
            }
        else
            bitmap->pal = NULL;
        bitmap->pf = NULL;
        }
    else {
        bitmap->pf = (GA_pixelFormat*)((N_uint8*)bitmap + size);
        *bitmap->pf = pf;
        bitmap->pal = NULL;
        }
    size += palSize;
    bitmap->surface = (N_uint8*)bitmap + size;

    /* Now read in the bits in the bitmap. We need to handle both cases
     * of bottom up and top down DIB's.
     */
    if (bitmap->height < 0) {
        bitmap->height = -bitmap->height;
        p = bitmap->surface;
        for (i = 0; i < bitmap->height; i++, p += bitmap->bytesPerLine)
            fread(p,1,bitmap->bytesPerLine,f);
        }
    else {
        p = (N_uint8 *)bitmap->surface + (N_int32)bitmap->bytesPerLine * (bitmap->height-1);
        for (i = 0; i < bitmap->height; i++, p -= bitmap->bytesPerLine)
            fread(p,1,bitmap->bytesPerLine,f);
        }
    fclose(f);
    return bitmap;
}

/****************************************************************************
PARAMETERS:
width           - Width of the bitmap to create in pixels
height          - Height of the bitmap to create in pixels
bitsPerPixel    - Color depth for the bitmap to create
pal             - Palette date for the bitmap to create (can be NULL)
pf              - Pixel format for bitmap to create (NULL if <= 8bpp)
surface         - Pointer to bitmap surface (NULL to allocate surface)

RETURNS:
Pointer to the allocated bitmap structure, NULL on error.

REMARKS:
Create an bitmap structure in memory. If you pass in a non-NULL value for
surface, it is assumed you have already allocated the memory for the surface.
Otherwise the memory for the surface is allocated in system memory.
****************************************************************************/
bmp_t * CreateBitmap(
    int width,
    int height,
    int bitsPerPixel,
    int bytesPerLine,
    GA_palette *pal,
    GA_pixelFormat *pf,
    void *surface)
{
    int     palSize;
    N_int32 size;
    bmp_t   *bitmap;

    /* Allocate memory for the bitmap */
    switch (bitsPerPixel) {
        case 8:
            if (bytesPerLine == -1)
                bytesPerLine = width;
            if (pal)
                palSize = 256 * sizeof(GA_palette);
            else
                palSize = 0;
            break;
        case 15:
        case 16:
            if (bytesPerLine == -1)
                bytesPerLine = width * 2;
            palSize = sizeof(GA_pixelFormat);
            break;
        case 24:
            if (bytesPerLine == -1)
                bytesPerLine = width * 3;
            palSize = sizeof(GA_pixelFormat);
            break;
        default:
            if (bytesPerLine == -1)
                bytesPerLine = width * 4;
            palSize = sizeof(GA_pixelFormat);
            break;
        }
    if (surface)
        size = palSize;
    else
        size = bytesPerLine * height + palSize;
    if ((bitmap = malloc(sizeof(bmp_t)+size)) == NULL)
        return NULL;

    /* Fill in the bitmap header information */
    bitmap->width = width;
    bitmap->height = height;
    bitmap->bitsPerPixel = bitsPerPixel;
    bitmap->bytesPerLine = bytesPerLine;
    size = sizeof(bmp_t);
    if (bitsPerPixel <= 8) {
        if (palSize) {
            bitmap->pal = (GA_palette*)((N_uint8*)bitmap + size);
            memcpy(bitmap->pal,pal,palSize);
            }
        else
            bitmap->pal = NULL;
        bitmap->pf = NULL;
        }
    else {
        bitmap->pf = (GA_pixelFormat*)((N_uint8*)bitmap + size);
        *bitmap->pf = *pf;
        bitmap->pal = NULL;
        }
    if (surface)
        bitmap->surface = surface;
    else {
        size += palSize;
        bitmap->surface = (N_uint8*)bitmap + size;
        }
    return bitmap;
}

/****************************************************************************
PARAMETERS:
dc  - Device context to save
bitmapName  - Name of bitmap file to save
left    - Left coordinate of bitmap to save
top - Top coordinate of bitmap to save
right   - Right coordinate of bitmap to save
bottom  - Bottom coordinate of bitmap to save

RETURNS:
True on success, false on error.

REMARKS:
This function saves a bitmap file to disk.
****************************************************************************/
ibool SaveBitmap(
    bmp_t *bmp,
    const char *bitmapName)
{
    FILE                *f;
    winBITMAPFILEHEADER hdr;
    winBITMAPINFOHEADER bmInfo;
    N_int32             size;
    int                 i,width,palSize=0,scanBytes;
    N_uint8             *p,*s,*pBuf,*scanBuf;
    N_uint32            masks[3];

    /* Attempt to open the file for writing */
    if ((f = fopen(bitmapName,"wb")) == NULL)
        return false;

    /* Build the bitmap file headers and write to disk */
    memset(&hdr,0,sizeof(hdr));
    hdr.bfType[0] = 'B';
    hdr.bfType[1] = 'M';
    memset(&bmInfo,0,sizeof(bmInfo));
    bmInfo.biSize   = sizeof(winBITMAPINFOHEADER);
    bmInfo.biWidth  = bmp->width;
    bmInfo.biHeight = bmp->height;
    bmInfo.biPlanes = 1;
    if (bmp->bitsPerPixel == 8) {
        palSize = 256 * sizeof(GA_palette);
        bmInfo.biBitCount = 8;
        bmInfo.biCompression = winBI_RGB;
        bmInfo.biClrUsed = 256;
        bmInfo.biBitCount = 8;
        bmInfo.biCompression = winBI_RGB;
        bmInfo.biClrUsed =256;
        scanBytes = (bmp->width + 3) & ~3;
        size = (N_int32)scanBytes * bmp->height;
        }
    else {
        bmInfo.biBitCount = 24;
        bmInfo.biClrUsed = 0;
        bmInfo.biCompression = winBI_RGB;
        scanBytes = ((bmp->width * 3) + 3) & ~3;
        size = (N_int32)scanBytes * bmp->height;
        }
    bmInfo.biSizeImage = size;
    bmInfo.biClrImportant = 0;
    PM_convLE_32(bmInfo.biSize);
    PM_convLE_32(bmInfo.biWidth);
    PM_convLE_32(bmInfo.biHeight);
    PM_convLE_32(bmInfo.biPlanes);
    PM_convLE_32(bmInfo.biBitCount);
    PM_convLE_32(bmInfo.biCompression);
    PM_convLE_32(bmInfo.biClrUsed);
    PM_convLE_32(bmInfo.biSizeImage);
    PM_convLE_32(bmInfo.biClrImportant);

    /* Write header and palette data to disk */
    if (bmp->bitsPerPixel <= 8)
        size += sizeof(hdr) + sizeof(bmInfo) + palSize;
    else if (PM_getLE_32(bmInfo.biCompression) == winBI_BITFIELDS)
        size += sizeof(hdr) + sizeof(bmInfo) + sizeof(masks);
    else
        size += sizeof(hdr) + sizeof(bmInfo);
    hdr.bfSize = size;
    hdr.bfOffBits = size - PM_getLE_32(bmInfo.biSizeImage);
    PM_convLE_32(hdr.bfSize);
    PM_convLE_32(hdr.bfOffBits);
    fwrite(&hdr,1,sizeof(hdr),f);
    fwrite(&bmInfo,1,sizeof(bmInfo),f);
    if (bmp->bitsPerPixel <= 8)
        fwrite(bmp->pal,1,palSize,f);
    else if (PM_getLE_32(bmInfo.biCompression) == winBI_BITFIELDS)
        fwrite(masks,1,sizeof(masks),f);

    /* Now write the bits from the device context to disk. Note that we
     * write the data in bottom up DIB format, which is the standard DIB
     * format for files saved to disk. Note also that we convert all
     * RGB bitmaps to 24bpp bitmaps so we can view them with paint programs.
     */
    pBuf = bmp->surface + (bmp->height-1)*bmp->bytesPerLine;
    if ((scanBuf = malloc(scanBytes)) == NULL)
        return false;
    for (i = bmp->height; i > 0; i--) {
        p = pBuf;
        width = bmp->width;
        switch (bmp->bitsPerPixel) {
            case 8:
                fwrite(p,1,scanBytes,f);
                break;
            case 15:
                s = scanBuf;
                while (width--) {
                    N_uint32 c = *((N_uint16*)p);
                    UNPACK_COLOR_15(c,s[2],s[1],s[0]);
                    p += 2;
                    s += 3;
                    }
                fwrite(scanBuf,1,scanBytes,f);
                break;
            case 16:
                s = scanBuf;
                while (width--) {
                    N_uint32 c = *((N_uint16*)p);
                    UNPACK_COLOR_16(c,s[2],s[1],s[0]);
                    p += 2;
                    s += 3;
                    }
                fwrite(scanBuf,1,scanBytes,f);
                break;
            case 24:
                if (bmp->pf->RedPosition == 0) {
                    /* Bitmap is in the extended 24 bit BGR format */
                    s = scanBuf;
                    while (width--) {
                        N_uint32 c = *((N_uint32*)p);
                        UNPACK_COLOR_BGR(c,s[2],s[1],s[0]);
                        p += 3;
                        s += 3;
                        }
                    fwrite(scanBuf,1,scanBytes,f);
                    }
                else
                    fwrite(p,1,scanBytes,f);
                break;
            case 32:
                s = scanBuf;
                if (bmp->pf->RedPosition == 0) {
                    while (width--) {
                        N_uint32 c = *((N_uint32*)p);
                        UNPACK_COLOR_ABGR(c,s[2],s[1],s[0]);
                        p += 4;
                        s += 3;
                        }
                    }
                else if (bmp->pf->RedPosition == 8) {
                    while (width--) {
                        N_uint32 c = *((N_uint32*)p);
                        UNPACK_COLOR_BGRA(c,s[2],s[1],s[0]);
                        p += 4;
                        s += 3;
                        }
                    }
                else if (bmp->pf->RedPosition == 16) {
                    while (width--) {
                        N_uint32 c = *((N_uint32*)p);
                        UNPACK_COLOR_ARGB(c,s[2],s[1],s[0]);
                        p += 4;
                        s += 3;
                        }
                    }
                else {
                    while (width--) {
                        N_uint32 c = *((N_uint32*)p);
                        UNPACK_COLOR_RGBA(c,s[2],s[1],s[0]);
                        p += 4;
                        s += 3;
                        }
                    }
                fwrite(scanBuf,1,scanBytes,f);
                break;
            }
        pBuf -= bmp->bytesPerLine;
        }
    free(scanBuf);
    fclose(f);
    return true;
}

/****************************************************************************
PARAMETERS:
bitmap  - Pointer to bitmap to unload

REMARKS:
Unloads the specified bitmap file from memory, and frees up all the system
resources associated with this bitmap.
****************************************************************************/
void UnloadBitmap(
    bmp_t *bitmap)
{
    if (bitmap)
        free(bitmap);
}
