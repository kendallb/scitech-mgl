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
* Description:  PNG bitmap resource loading/unloading routines.
*
****************************************************************************/

#include "mgl.h"
#include "png.h"

/*--------------------------- Global Variables ----------------------------*/

static jmp_buf  jmpbuf;

/*------------------------- Implementation --------------------------------*/

/****************************************************************************
PARAMETERS:
cinfo   - Pointer to the information for the error handler

REMARKS:
PNG error Handling routine. The function is called when an error occurs,
and allows us to dump the error message to a log file (for CHECKED builds)
and return a failure condition to the caller.
****************************************************************************/
static void PMAPI my_png_error(
    png_structp png_ptr,
    png_const_charp message)
{
#ifdef  CHECKED
    FILE    *f;

    if ((f = fopen("png.log","a+")) == NULL)
        exit(1);
    fprintf(f,"%s\n",message);
    fclose(f);
#endif
    __MGL_result = grInvalidBitmap;
    longjmp(jmpbuf, 1);
    (void)png_ptr;
}

/****************************************************************************
PARAMETERS:
cinfo   - Pointer to the information for the error handler

REMARKS:
PNG error Handling routine. The function is called when an error occurs,
and allows us to dump the error message to a log file (for CHECKED builds)
and return a failure condition to the caller.
****************************************************************************/
static void PMAPI my_png_warn(
    png_structp png_ptr,
    png_const_charp message)
{
#ifdef  CHECKED
    FILE    *f;

    if ((f = fopen("png.log","a+")) == NULL)
        exit(1);
    fprintf(f,"%s\n",message);
    fclose(f);
#endif
    (void)png_ptr;
}

/****************************************************************************
PARAMETERS:
png_ptr - PNG library structure
data    - Data to read data from the file into
length  - Length of the data block to read

REMARKS:
Custom read function. The PNG library includes a mechanism for customizing
how file I/O is handled, and this routine replaces the default read
function with the MGL file I/O (which can be replaced by the developer).
****************************************************************************/
static void PMAPI my_png_read_data(
    png_structp png_ptr,
    png_bytep data,
    png_size_t length)
{
    png_size_t check = __MGL_fread(data,1,length,(FILE*)png_ptr->io_ptr);
    if (check != length)
        png_error(png_ptr, "Read Error");
}

/****************************************************************************
PARAMETERS:
png_ptr - PNG library structure

REMARKS:
Custom flush function. The MGL does not provide an fflush replacement,
and the file will flush automatically in all operating systems as soon
as the file handle is closed. This is good enough for what we need.
****************************************************************************/
static void PMAPI my_png_flush_data(
    png_structp png_ptr)
{
    (void)png_ptr;
}

/****************************************************************************
PARAMETERS:
png_ptr - PNG library structure
data    - Data to write to the file
length  - Length of the data block to write

REMARKS:
Custom read function. The PNG library includes a mechanism for customizing
how file I/O is handled, and this routine replaces the default write
function with the MGL file I/O (which can be replaced by the developer).
****************************************************************************/
static void PMAPI my_png_write_data(
    png_structp png_ptr,
    png_bytep data,
    png_size_t length)
{
    png_size_t check = __MGL_fwrite(data,1,length,(FILE *)png_ptr->io_ptr);
    if (check != length)
        png_error(png_ptr, "Write Error");
}

/****************************************************************************
DESCRIPTION:
Determines if the specified PNG bitmap file is available for use.

HEADER:
mgraph.h

PARAMETERS:
PNGName     - Name of PNG bitmap file to check for

RETURNS:
True if the a PNG bitmap file exists, false if not.

REMARKS:
Attempt to locate the specified PNG file, and verify that it is available
for use. If the file exists the routine checks the signature to verify that
it is really a PNG file. See MGL_loadPNG for more information on the
algorithm that MGL uses when searching for bitmap files on disk.

SEE ALSO:
MGL_loadPNG
****************************************************************************/
ibool MGLAPI MGL_availablePNG(
    const char *PNGName)
{
    FILE    *f;
    uchar   buf[4];

    /* Open the file and check the header signature for a valid PNG file */
    if ((f = _MGL_openFile(MGL_BITMAPS, PNGName, "rb")) == NULL) {
        __MGL_result = grBitmapNotFound;
        return false;
        }
    if (__MGL_fread(buf, 1, 4, f) == 4) {
        if (png_sig_cmp(buf, (png_size_t)0, 4)) {
            __MGL_fclose(f);
            __MGL_result = grOK;
            return true;
            }
        }

    /* If the file is shorter than 4 bytes or the signature fails the
     * png_sig_cmp test then the file is not a PNG file.
     */
    __MGL_fclose(f);
    __MGL_result = grInvalidBitmap;
    return false;
}

/****************************************************************************
DESCRIPTION:
Load a PNG bitmap file from disk.

HEADER:
mgraph.h

PARAMETERS:
PNGName         - Name of PNG file to load
loadPalette     - True if you wish to load the images palette
                  Only valid on PNG files with a bit depth of 8 or below
RETURNS:
Pointer to the loaded PNG file, NULL on error.

REMARKS:
If you wish to load the bitmap as a different color depth or pixel format
use the MGL_loadPNGIntoDC function.

When MGL is searching for PNG files it will first attempt to find the files
just by using the filename itself. Hence if you wish to look for a specific
PNG file, you should pass the full pathname to the file that you are
interested in. If the filename is a simple relative filename (i.e.
"MYBMP.PNG"), MGL will then search in the BITMAPS directory relative to the
path specified in mglpath variable that was passed to MGL_init. As a final
resort MGL will also look for the files in the BITMAPS directory relative
to the MGL_ROOT environment variable.

If the PNG file was not found, or an error occurred while reading the PNG
file, this function will return NULL. You can check the MGL_result error
code to determine the cause.

SEE ALSO:
MGL_unloadBitmap, MGL_availablePNG, MGL_getPNGSize,
MGL_loadPNGIntoDC, MGL_savePNGFromDC, MGL_putBitmap, MGL_loadPNGExt
****************************************************************************/
bitmap_t * MGLAPI MGL_loadPNG(
    const char *PNGName,
    ibool loadPalette)
{
    FILE        *f;
    bitmap_t    *b;

    __MGL_result = grOK;
    if ((f = _MGL_openFile(MGL_BITMAPS, PNGName, "rb")) == NULL) {
        __MGL_result = grBitmapNotFound;
        return false;
        }
    b = MGL_loadPNGExt(f,0,_MGL_fileSize(f),loadPalette);
    __MGL_fclose(f);
    return b;
}

/****************************************************************************
DESCRIPTION:
Load a PNG bitmap file from disk using an open file.

HEADER:
mgraph.h

PARAMETERS:
f               - Open binary file to read bitmap from
dwOffset        - Offset to start of PNG file within open file
dwSize          - Size of PNG file in bytes
loadPalette     - true if you wish to load the images palette

RETURNS:
Pointer to the loaded bitmap file

REMARKS:
This function is the same as MGL_loadPNG, however it loads the file from a
previously open file. This allows you to create your own large files with
multiple files embedded in them.

SEE ALSO:
MGL_loadPNG, MGL_loadBitmap
****************************************************************************/
bitmap_t * MGLAPI MGL_loadPNGExt(
    FILE *f,
    ulong dwOffset,
    ulong dwSize,
    ibool loadPalette)
{
    bitmap_t        *bitmap;    /* MGL bitmap for holding file being created*/
    ulong           size;       /* Size of the bitmap image                 */
    png_structp     png_ptr;
    png_infop       info_ptr;
    png_uint_32     width, height;
    int             bit_depth, color_type, interlace_type, intent;
    int             number_passes, pass, numcolors;
    int             pixelsperbyte = 0;
    ulong           linelen = 0;
    double          screen_gamma, image_gamma;
    char            *gamma_str;

    /* Seek to the start of the file */
    __MGL_result = grOK;
    __MGL_fseek(f,dwOffset,SEEK_SET);

    /* Create and initialize the png_struct with the desired error handler
     * functions.  If you want to use the default stderr and longjump method,
     * you can supply NULL for the last three parameters.  We also supply the
     * the compiler header file version, so that we know if the application
     * was compiled with a compatible version of the library.
     */
    if ((png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, my_png_error, my_png_warn)) == NULL) {
        __MGL_result = grErrorBPD;
        return NULL;
        }

    /* Allocate/initialize the memory for image information */
    if ((info_ptr = png_create_info_struct(png_ptr)) == NULL) {
        png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
        __MGL_result = grErrorBPD;
        return NULL;
        }

    /* Set error handling for using the setjmp/longjmp method */
    if (setjmp(jmpbuf)) {
        /* We get here on errr, so destroy all memory we allocated */
        png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
        __MGL_result = grErrorBPD;
        return NULL;
        }

    /* This sets the MGL custom I/O functions for use with LibPNG internal
     * Read operations
     */
    png_set_read_fn(png_ptr, (void*)f, my_png_read_data);

    /* The call to png_read_info() gives us all of the information from the
     * PNG file before the first IDAT (image data chunk).
     */
    png_read_info(png_ptr, info_ptr);
    png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, &interlace_type, NULL, NULL);

    /* Note: Regarding grayscale images
     * if the image is a grayscale image the data decoded and translated as
     * if it was a paletted image with a grayscale color map.
     * if loadPalette is true then a linear grayscale palette is created
     * for the bitmap.
     */
    numcolors = 0;
    if ((color_type == PNG_COLOR_TYPE_PALETTE) || (color_type == PNG_COLOR_TYPE_GRAY)) {
        switch (bit_depth) {
            case 1: numcolors = 2; pixelsperbyte = 8; break;
            /* bit_depth = 2 is a special case since MGL can't support
             * 2 bit wide bitmap formats, we use the libpng library to
             * translate it into 8 bit, and set these variables with that
             * in mind.
             */
            case 2: numcolors = 256; pixelsperbyte = 1; break;
            case 4: numcolors = 16; pixelsperbyte = 2; break;
            case 8: numcolors = 256; pixelsperbyte = 1; break;
            }
        }
    else if (color_type == PNG_COLOR_TYPE_GRAY_ALPHA) {
        numcolors = 256;
        }

    /* Allocate memory for the bitmap and fill in header */
    if (numcolors) {
        if (color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
            linelen = width  * 2;
        else
            linelen = (width + pixelsperbyte-1) / pixelsperbyte;
        if (loadPalette) {
            size = sizeof(bitmap_t)
                 + (color_type == PNG_COLOR_TYPE_GRAY_ALPHA) * sizeof(pixel_format_t)
                 + sizeof(palette_t) * numcolors
                 + linelen * height;
            }
        else {
            size = sizeof(bitmap_t)
                 + (color_type == PNG_COLOR_TYPE_GRAY_ALPHA) * sizeof(pixel_format_t)
                 + linelen * height;
            }
        }
    else if (color_type == PNG_COLOR_TYPE_RGB_ALPHA) {
        size = sizeof(bitmap_t) + sizeof(pixel_format_t)
             + width * 4 * height;
        }
    else {
        size = sizeof(bitmap_t) + sizeof(pixel_format_t)
             + width * 3 * height;
        }
    if ((bitmap = PM_malloc(size)) == NULL) {
        png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
        __MGL_result = grNoMem;
        return NULL;
        }
    size = sizeof(bitmap_t);
    bitmap->width = width;
    bitmap->height = height;

    /* In paletted images numcolors will be > 0 */
    if (numcolors) {
        /* 8-bits per pixel with a palette */
        bitmap->bytesPerLine = linelen;
        bitmap->pf = NULL;
        if (color_type == PNG_COLOR_TYPE_GRAY_ALPHA) {
            bitmap->bitsPerPixel = 16;
            bitmap->pf = (pixel_format_t*)((uchar*)bitmap + size);
            size += sizeof(pixel_format_t);
            memcpy(bitmap->pf,&_MGL_pixelFormats[pfCI8_A],sizeof(pixel_format_t));
            }
        else {
            if (bit_depth == 2 || bit_depth == 16) /* special case, see above */
                bitmap->bitsPerPixel = 8;
            else
                bitmap->bitsPerPixel = bit_depth;
            }
        if (loadPalette) {
            bitmap->pal = (palette_t*)((uchar*)bitmap + size);
            size += sizeof(palette_t) * numcolors;
            memset(bitmap->pal,0,sizeof(palette_t) * numcolors);
            }
        else
            bitmap->pal = NULL;
        }
    else if (color_type == PNG_COLOR_TYPE_RGB_ALPHA) {
        /* 32-bit ARGB */
        bitmap->bitsPerPixel = 32;
        bitmap->bytesPerLine = width * 4;
        bitmap->pal = NULL;
        bitmap->pf = (pixel_format_t*)((uchar*)bitmap + size);
        memcpy(bitmap->pf,&_MGL_pixelFormats[pfARGB32],sizeof(pixel_format_t));
        size += sizeof(pixel_format_t);
        png_set_bgr(png_ptr);
        }
    else {
        /* 24-bit RGB */
        bitmap->bitsPerPixel = 24;
        bitmap->bytesPerLine = width * 3;
        bitmap->pal = NULL;
        bitmap->pf = (pixel_format_t*)((uchar*)bitmap + size);
        memcpy(bitmap->pf,&_MGL_pixelFormats[pfRGB24],sizeof(pixel_format_t));
        size += sizeof(pixel_format_t);
        png_set_bgr(png_ptr);
        }
    bitmap->surface = (uchar*)bitmap + size;

    /* Tell libpng to strip 16 bit/color files down to 8 bits/color */
    if (bit_depth == 16)
        png_set_strip_16(png_ptr);

    /* If the bit_depth is 2 we have no support for that type in MGL so
     * we will unpack the colors into 8 bit per pixel
     */
    if (color_type == PNG_COLOR_TYPE_PALETTE && bit_depth == 2)
        png_set_packing(png_ptr);
    if ((color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA) && bit_depth == 2)
        png_set_expand(png_ptr);

    /* Expand images with seperate alpha channels or transparent bits into
     * RGBA quads
     */
    if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS) && color_type != PNG_COLOR_TYPE_PALETTE
            && color_type != PNG_COLOR_TYPE_GRAY && color_type != PNG_COLOR_TYPE_GRAY_ALPHA)
        png_set_expand(png_ptr);

    /* Set the default gamma, which can be overridden with an env variable */
    if ((gamma_str = getenv("SCREEN_GAMMA")) != NULL)
        screen_gamma = atof(gamma_str);
    else
        screen_gamma = 2.0; /* default multiplied by image gamma (.5) should be 1  */
                            /* ie: no change, if i am wrong this is completely off */

    if (png_get_sRGB(png_ptr, info_ptr, &intent))
        png_set_sRGB(png_ptr, info_ptr, intent);
    else {
        if (png_get_gAMA(png_ptr, info_ptr, &image_gamma))
            png_set_gamma(png_ptr, screen_gamma, image_gamma);
        else
            png_set_gamma(png_ptr, screen_gamma, 0.50);
        }

    /* Turn on interlace handling.  REQUIRED if you are not using
     * png_read_image().  To see how to handle interlacing passes,
     * see the png_read_row() method below:
     */
    number_passes = png_set_interlace_handling(png_ptr);

    /* Optional call to gamma correct and add the background to the palette
     * and update info structure.  REQUIRED if you are expecting libpng to
     * update the palette for you (ie you selected such a transform above).
     */
    png_read_update_info(png_ptr, info_ptr);

    /* Read the image one or more scanlines at a time */
    for (pass = 0; pass < number_passes; pass++) {
        /* Read the image a single row at a time */
        int y;
        for (y = 0; y < (int)height; y++) {
            png_bytep row_pointers;
            row_pointers = ((uchar*)bitmap->surface)+(bitmap->bytesPerLine*y);
            png_read_rows(png_ptr, &row_pointers, NULL, 1);
            }
        }

    /* Read rest of file, and get additional chunks in info_ptr */
    png_read_end(png_ptr, info_ptr);

    /* Read the palette information */
    if (loadPalette) {
        if (png_get_valid(png_ptr, info_ptr, PNG_INFO_PLTE)) {
            png_color *thispal;
            int i;
            png_get_PLTE(png_ptr, info_ptr, &thispal, &numcolors);
            for (i = 0; i < numcolors; i++) {
                bitmap->pal[i].red = thispal[i].red;
                bitmap->pal[i].green = thispal[i].green;
                bitmap->pal[i].blue = thispal[i].blue;
                bitmap->pal[i].alpha = 0;
                }
            }
        else if (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA) {
            int i;
            int ofs = 256/numcolors;
            int pos = 0;
            for (i = 0; i < numcolors; i++) {
                bitmap->pal[i].red = pos;
                bitmap->pal[i].green = pos;
                bitmap->pal[i].blue = pos;
                bitmap->pal[i].alpha = 0;
                pos += ofs;
                }
            }
        }

    /* Clean up after the read, and free any memory allocated */
    png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
    (void)dwSize;
    return bitmap;
}

/****************************************************************************
DESCRIPTION:
Obtain the dimensions of a PNG file from disk.

HEADER:
mgraph.h

PARAMETERS:
PNGName         - Name of the bitmap file to load header for
width           - Place to store the bitmap width
height          - Place to store the bitmap height
bitsPerPixel    - Place to store the bitmap pixel depth
pf              - Place to store the bitmap pixel format information

RETURNS:
True if the PNG file was found, false if not.

REMARKS:
This functions loads all the header information for a PNG file from disk,
without actually loading the bits for the bitmap surface. This is useful to
determine the dimensions and pixel format for the bitmap before it is loaded,
so you can create an appropriate memory device context that you can load the
bitmap into with the MGL_loadPNGIntoDC function.

SEE ALSO:
MGL_loadPNG, MGL_loadPNGIntoDC
****************************************************************************/
ibool MGLAPI MGL_getPNGSize(
    const char *PNGName,
    int *width,
    int *height,
    int *bitsPerPixel,
    pixel_format_t *pf)
{
    FILE    *f;
    ibool   ret;

    __MGL_result = grOK;
    if ((f = _MGL_openFile(MGL_BITMAPS, PNGName, "rb")) == NULL) {
        __MGL_result = grBitmapNotFound;
        return false;
        }
    ret = MGL_getPNGSizeExt(f,0,_MGL_fileSize(f),width,height,bitsPerPixel,pf);
    __MGL_fclose(f);
    return ret;
}

/****************************************************************************
DESCRIPTION:
Obtain the dimensions of a PNG file from an opened file.

HEADER:
mgraph.h

PARAMETERS:
f               - Open binary file to read bitmap from
dwOffset        - Offset to start of PNG file
dwSize          - Size of PNG file
width           - Width of bitmap
height          - Height of bitmap
bitsPerPixel    - Pixel depth of bitmap
pf              - Place to store the bitmap pixel format information

RETURNS:
True if PNG was found, false if not.

REMARKS:
This function is the same as MGL_getPNGSize, however it works with a
previously opened file. This allows you to create your own large files with
multiple files embedded in them.

SEE ALSO:
MGL_getPNGSize
****************************************************************************/
ibool MGLAPI MGL_getPNGSizeExt(
    FILE *f,
    ulong dwOffset,
    ulong dwSize,
    int *width,
    int *height,
    int *bitsPerPixel,
    pixel_format_t *pf)
{
    png_structp png_ptr;
    png_infop   info_ptr;
    int         bit_depth, color_type, interlace_type;
    png_uint_32 mw, mh;

    /* Seek to the start of the file */
    __MGL_result = grOK;
    __MGL_fseek(f,dwOffset,SEEK_SET);

    /* Create and initialize the png_struct with the desired error handler
     * functions.  If you want to use the default stderr and longjump method,
     * you can supply NULL for the last three parameters.  We also supply the
     * the compiler header file version, so that we know if the application
     * was compiled with a compatible version of the library.
     */
    if ((png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, my_png_error, my_png_warn)) == NULL) {
        __MGL_result = grErrorBPD;
        return false;
        }

    /* Allocate/initialize the memory for image information */
    if ((info_ptr = png_create_info_struct(png_ptr)) == NULL) {
        png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
        __MGL_result = grErrorBPD;
        return false;
        }

    /* Set error handling for using the setjmp/longjmp method */
    if (setjmp(jmpbuf)) {
        /* We get here on errr, so destroy all memory we allocated */
        png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
        __MGL_result = grErrorBPD;
        return false;
        }

    /* This sets the MGL custom I/O functions for use with LibPNG internal
     * Read operations
     */
    png_set_read_fn(png_ptr, (void*)f, my_png_read_data);

    /* The call to png_read_info() gives us all of the information from the
     * PNG file before the first IDAT (image data chunk).
     */
    png_read_info(png_ptr, info_ptr);
    png_get_IHDR(png_ptr, info_ptr, &mw, &mh, &bit_depth, &color_type, &interlace_type, NULL, NULL);
    *width = mw;
    *height = mh;
    *bitsPerPixel = bit_depth;

    /* bit_depth = 2 is a special case since mgl can't support
     * 2 bit wide bitmap formats, we use the libpng library to
     * translate it into 8 bit, and set these variables with that
     * in mind.
     */
    if (bit_depth == 2)
        *bitsPerPixel = 8;
    if (color_type == PNG_COLOR_TYPE_GRAY_ALPHA) {
        *bitsPerPixel = 16;
        memcpy(pf,&_MGL_pixelFormats[pfCI8_A],sizeof(pixel_format_t));
        }
    else if (color_type == PNG_COLOR_TYPE_RGB_ALPHA) {
        /* 32-bit ARGB */
        *bitsPerPixel = 32;
        memcpy(pf,&_MGL_pixelFormats[pfARGB32],sizeof(pixel_format_t));
        }
    else if (color_type == PNG_COLOR_TYPE_RGB) {
        /* 24-bit RGB */
        *bitsPerPixel = 24;
        memcpy(pf,&_MGL_pixelFormats[pfRGB24],sizeof(pixel_format_t));
        }

    /* clean up after the read, and free any memory allocated */
    png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
    (void)dwSize;
    return true;
}

/****************************************************************************
DESCRIPTION:
Loads a PNG file directly into an existing device context.

HEADER:
mgraph.h

PARAMETERS:
dc              - Device context to load bitmap into
PNGName         - Name of PNG file to load
dstLeft         - Left coordinate to load PNG at
dstTop          - Top coordinate to load PNG at
loadPalette     - True if you would like to replace the dc's palette with PNG file's

RETURNS:
True if the PNG file was loaded, false on error.

REMARKS:
Locates the specified PNG file and loads it into the specified device context at the
specified destination coordinates. If the PNG is of a different pixel depth than the
device context that it is being loaded into, the PNG will be converted as it is loaded
to the pixel format of the device context it is being loaded into.

If the PNG file has a higher bit depth than the surface the pixel values
will be dithered to the correct color depth. This will use the currently selected
palette if loadPalette is false, or the HalfTone palette if true.

When MGL is searching for bitmap files it will first attempt to find the files just by
using the filename itself. Hence if you wish to look for a specific bitmap file, you
should pass the full pathname to the file that you are interested in. If the filename is
a simple relative filename (i.e. "MYFILE.PNG"), MGL will then search in the
BITMAPS directory relative to the path specified in mglpath variable that was
passed to MGL_init. As a final resort MGL will also look for the files in the
BITMAPS directory relative to the MGL_ROOT environment variable.

If the bitmap file was not found, or an error occurred while reading the bitmap file,
this function will return false. You can check the MGL_result error code to
determine the cause.

If the PNG file has interlacing enabled MGL must create a memory buffer internally
to render the image into before blitting to the dc. Therefore using non-interlaced
images will reduce memory overhead and speed performance.

SEE ALSO:
MGL_availablePNG, MGL_getPNGSize,MGL_loadPNG, MGL_savePNGFromDC
****************************************************************************/
ibool MGLAPI MGL_loadPNGIntoDC(
    MGLDC *dc,
    const char *PNGName,
    int dstLeft,
    int dstTop,
    ibool loadPalette)
{
    FILE    *f;
    ibool   ret;

    __MGL_result = grOK;
    if ((f = _MGL_openFile(MGL_BITMAPS, PNGName, "rb")) == NULL) {
        __MGL_result = grBitmapNotFound;
        return false;
        }
    ret = MGL_loadPNGIntoDCExt(dc,f,0,_MGL_fileSize(f),dstLeft,dstTop,loadPalette);
    __MGL_fclose(f);
    return ret;
}

/****************************************************************************
DESCRIPTION:
Load a PNG file directly into an existing device context

HEADER:
mgraph.h

PARAMETERS:
dc              - Device context to output to
f               - Open binary file to read bitmap from
dwOffset        - Offset to start of PNG file within open file
dwSize          - Size of PNG file in bytes
dstLeft         - Left coordinate to align left edge of bitmap with
dstTop          - Top coordinate to align top edge of bitmap with
loadPalette     - true if you wish to load the images palette

RETURNS:
True if the PNG was loaded, false on error.

REMARKS:
This function is the same as MGL_loadPNGIntoDC, however it loads the file from a
previously open file. This allows you to create your own large files with
multiple files embedded in them.

If the PNG file has interlacing enabled MGL must create a memory buffer internally
to render the image into before blitting to the dc. Therefore using non-interlaced
images will reduce memory overhead and speed performance.

SEE ALSO:
MGL_loadPNGIntoDC, MGL_loadBitmapIntoDCExt, MGL_loadPNG
****************************************************************************/
ibool MGLAPI MGL_loadPNGIntoDCExt(
    MGLDC *dc,
    FILE *f,
    ulong dwOffset,
    ulong dwSize,
    int dstLeft,
    int dstTop,
    ibool loadPalette)
{
    bitmap_t        bmp;
    png_structp     png_ptr;
    png_infop       info_ptr;
    png_uint_32     width, height;
    int             bit_depth, color_type, interlace_type, intent;
    int             number_passes, pass, numcolors, havePal;
    double          screen_gamma, image_gamma;
    char            *gamma_str;
    palette_t       pal[256];
    int             bitsPerPixel,oldCheckId;
    pixel_format_t  pf;

    /* Seek to the start of the file */
    __MGL_fseek(f,dwOffset,SEEK_SET);

    /* Create and initialize the png_struct with the desired error handler
     * functions.  If you want to use the default stderr and longjump method,
     * you can supply NULL for the last three parameters.  We also supply the
     * the compiler header file version, so that we know if the application
     * was compiled with a compatible version of the library.
     */
    if ((png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, my_png_error, my_png_warn)) == NULL) {
        __MGL_result = grErrorBPD;
        return false;
        }

    /* Allocate/initialize the memory for image information */
    if ((info_ptr = png_create_info_struct(png_ptr)) == NULL) {
        png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
        __MGL_result = grErrorBPD;
        return false;
        }

    /* Set error handling for using the setjmp/longjmp method */
    bmp.surface = NULL;
    if (setjmp(jmpbuf)) {
        /* We get here on errr, so destroy all memory we allocated */
        PM_free(bmp.surface);
        png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
        __MGL_result = grErrorBPD;
        return false;
        }

    /* This sets the MGL custom I/O functions for use with LibPNG internal
     * Read operations
     */
    png_set_read_fn(png_ptr, (void*)f, my_png_read_data);

    /* The call to png_read_info() gives us all of the information from the
     * PNG file before the first IDAT (image data chunk).
     */
    png_read_info(png_ptr, info_ptr);
    png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, &interlace_type, NULL, NULL);

    /* Note: Regarding grayscale images
     * if the image is a grayscale image the data decoded and translated as
     * if it was a paletted image with a grayscale color map.
     * if loadPalette is true then a linear grayscale palette is created
     * for the bitmap.
     */
    numcolors = 0;
    if ((color_type == PNG_COLOR_TYPE_PALETTE) || (color_type == PNG_COLOR_TYPE_GRAY) || (color_type == PNG_COLOR_TYPE_GRAY_ALPHA)) {
        switch (bit_depth) {
            case 1: numcolors = 2; break;
            /* bit_depth = 2 is a special case since MGL can't support
             * 2 bit wide bitmap formats, we use the libpng library to
             * translate it into 8 bit, and set these variables with that
             * in mind.
             */
            case 2: numcolors = 256; break;
            case 4: numcolors = 16; break;
            case 8: numcolors = 256; break;
            case 16: numcolors = 256; break;
            }
        }

    /* In paletted images numcolors will be > 0 */
    if (numcolors) {
        /* 8-bits per pixel with a palette, or 8-bit grayscale with an 8-bit alpha
         * channel.
         */
        if (color_type == PNG_COLOR_TYPE_GRAY_ALPHA) {
            bitsPerPixel = 16;
            memcpy(&pf,&_MGL_pixelFormats[pfCI8_A],sizeof(pixel_format_t));
            }
        else if (bit_depth == 2 || bit_depth == 16) /* special case, see above */
            bitsPerPixel = 8;
        else
            bitsPerPixel = bit_depth;
        }
    else if (color_type == PNG_COLOR_TYPE_RGB_ALPHA) {
        /* 32-bit ARGB */
        bitsPerPixel = 32;
        memcpy(&pf,&_MGL_pixelFormats[pfARGB32],sizeof(pixel_format_t));
        png_set_bgr(png_ptr);
        }
    else {
        /* 24-bit RGB */
        bitsPerPixel = 24;
        memcpy(&pf,&_MGL_pixelFormats[pfRGB24],sizeof(pixel_format_t));
        png_set_bgr(png_ptr);
        }

    /* Turn on interlace handling.  REQUIRED if you are not using
     * png_read_image().  To see how to handle interlacing passes,
     * see the png_read_row() method below:
     */
    number_passes = png_set_interlace_handling(png_ptr);

    /* Because of the nature of how png files are read and stored we cannot
     * load multiple pass images directly into the frame buffer,
     * to avoid multiple translations on the same pixel we create a temporary
     * dc that is the size of the full image. If the image is not multiple
     * pass then we create a buffer for only one line.
     */
    bmp.width = width;
    bmp.height = (number_passes > 1) ? height : 1;
    bmp.bitsPerPixel = bitsPerPixel;
    switch (bitsPerPixel) {
        case 1:
            bmp.bytesPerLine = (width+7)/8;
            break;
        case 4:
            bmp.bytesPerLine = (width+1)/2;
            break;
        case 8:
            bmp.bytesPerLine = width;
            break;
        case 15:
        case 16:
            bmp.bytesPerLine = width*2;
            break;
        case 24:
            bmp.bytesPerLine = width*3;
            break;
        case 32:
            bmp.bytesPerLine = width*4;
            break;
        }
    bmp.pal = pal;
    bmp.pf = &pf;
    if ((bmp.surface = PM_calloc(1,bmp.bytesPerLine * bmp.height)) == NULL) {
        png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
        FATALERROR(grNoMem);
        return false;
        }

    /* Tell libpng to strip 16 bit/color files down to 8 bits/color */
    if (bit_depth == 16)
        png_set_strip_16(png_ptr);

    /* If the bit_depth is 2 we have no support for that type in MGL so
     * we will unpack the colors into 8 bit per pixel
     */
    if (color_type == PNG_COLOR_TYPE_PALETTE && bit_depth == 2)
        png_set_packing(png_ptr);
    if ((color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA) && bit_depth == 2)
        png_set_expand(png_ptr);

    /* Expand images with seperate alpha channels or transparent bits into
     * RGBA quads
     */
    if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS) && color_type != PNG_COLOR_TYPE_PALETTE
            && color_type != PNG_COLOR_TYPE_GRAY && color_type != PNG_COLOR_TYPE_GRAY_ALPHA)
        png_set_expand(png_ptr);

    /* Set the default gamma, which can be overridden with an env variable */
    if ((gamma_str = getenv("SCREEN_GAMMA")) != NULL)
        screen_gamma = atof(gamma_str);
    else
        screen_gamma = 2.0; /* default multiplied by image gamma (.5) should be 1  */
                            /* ie: no change, if i am wrong this is completely off */
    if (png_get_sRGB(png_ptr, info_ptr, &intent))
        png_set_sRGB(png_ptr, info_ptr, intent);
    else {
        if (png_get_gAMA(png_ptr, info_ptr, &image_gamma))
            png_set_gamma(png_ptr, screen_gamma, image_gamma);
        else
            png_set_gamma(png_ptr, screen_gamma, 0.50);
        }

    /* Optional call to gamma correct and add the background to the palette
     * and update info structure.  REQUIRED if you are expecting libpng to
     * update the palette for you (ie you selected such a transform above).
     */
    png_read_update_info(png_ptr, info_ptr);

    /* Add the palette to the temporary dc, if there is a palette */
    oldCheckId = MGL_checkIdentityPalette(false);
    havePal = png_get_valid(png_ptr, info_ptr, PNG_INFO_PLTE);
    if ((havePal && color_type == PNG_COLOR_TYPE_PALETTE) || (color_type == PNG_COLOR_TYPE_GRAY) || (color_type == PNG_COLOR_TYPE_GRAY_ALPHA)) {
        if (havePal) {
            png_color *thispal;
            int i;
            png_get_PLTE(png_ptr, info_ptr, &thispal, &numcolors);
            for (i = 0; i < numcolors; i++) {
                pal[i].red = thispal[i].red;
                pal[i].green = thispal[i].green;
                pal[i].blue = thispal[i].blue;
                pal[i].alpha = 0;
                }
            }
        else {
            int i;
            int ofs = 256/numcolors;
            int pos = 0;
            for (i = 0; i < numcolors; i++) {
                pal[i].red = pos;
                pal[i].green = pos;
                pal[i].blue = pos;
                pal[i].alpha = 0;
                pos += ofs;
                }
            }
        /* If loadPalette is true we also load the palette into the DC */
        if (loadPalette && numcolors == 256) {
            MGL_setPalette(dc,pal,MGL_getPaletteSize(dc),0);
            MGL_realizePalette(dc,MGL_getPaletteSize(dc),0,true);
            }
        }
    else {
        /* There is no palette included in the image data
         * If a palette is required by the target dc we will just add the
         * halftone palette.
         */
        if (loadPalette && (dc->mi.modeFlags & MGL_IS_COLOR_INDEX)) {
            MGL_getHalfTonePalette(pal);
            MGL_setPalette(dc,pal,MGL_getPaletteSize(dc),0);
            MGL_realizePalette(dc,MGL_getPaletteSize(dc),0,true);
            }
        }

    /* Note if the below if statement was changed to read || has_alpha and the
     * png_handle_alpha() function was called this would effectively make
     * libpng alpha blend the incoming image with the current contents of the
     * device context. I did not add this because the method below is kind of
     * slow compared to just writing the image.
     */
    if (number_passes > 1) {
        /* Read the image one or more scanlines at a time */
        for (pass = 0; pass < number_passes; pass++) {
            /* Read the image a single row at a time */
            int y;
            for (y = 0; y < (int)height; y++) {
                png_bytep row_pointers;
                row_pointers = ((uchar*)bmp.surface)+(bmp.bytesPerLine*y);
                /* because the line needs data from the previous pass we must
                 * load that data from the DC.
                 */
                png_read_rows(png_ptr, &row_pointers, NULL, 1);
                }
            MGL_putBitmap(dc, dstLeft, dstTop, &bmp, MGL_REPLACE_MODE);
            }
        }
    else {
        int y;
        for (y = 0; y < (int)height; y++) {
            png_bytep row_pointers;
            row_pointers = ((uchar*)bmp.surface);
            png_read_rows(png_ptr, &row_pointers, NULL, 1);
            MGL_putBitmap(dc, dstLeft, dstTop+y, &bmp, MGL_REPLACE_MODE);
            }
        }

    /* Read rest of file, and get additional chunks in info_ptr */
    png_read_end(png_ptr, info_ptr);

    /* Clean up after the read, and free any memory allocated */
    png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
    PM_free(bmp.surface);
    MGL_checkIdentityPalette(oldCheckId);
    __MGL_result = grOK;
    (void)dwSize;
    return true;
}

/****************************************************************************
DESCRIPTION:
Save a portion of a device context to a PNG file on disk.

HEADER:
mgraph.h

PARAMETERS:
dc          - Device context to save
PNGName     - Name of bitmap file to save
left        - Left coordinate of bitmap to save
top         - Top coordinate of bitmap to save
right       - Right coordinate of bitmap to save
bottom      - Bottom coordinate of bitmap to save

RETURNS:
True on success, false on error.

REMARKS:
This function saves a portion of a device context as a PNG format bitmap file to
disk. If this function fails for some reason, it will return false and you can get the
error code from the MGL_result function.

This function supports saving in native formats of 1, 4, 8, 24, and 32 bits.
Images with bit depths of 15 or 16 bits will be expanded to 24 bits.

Note that the source rectangle for the bitmap to be saved is not clipped to the
current clipping rectangle for the device context, but it is mapped to the current
viewport. If you specify dimensions that are larger than the current device context,
you will get garbage in the bitmap file as a result.

SEE ALSO:
MGL_savePNGFromDCExt, MGL_loadPNG, MGL_loadPNGIntoDC
****************************************************************************/
ibool MGLAPI MGL_savePNGFromDC(
    MGLDC *dc,
    const char *PNGName,
    int left,
    int top,
    int right,
    int bottom)
{
    return MGL_savePNGFromDCExt(dc,PNGName,left,top,right,bottom,true);
}

/****************************************************************************
DESCRIPTION:
Save a portion of a device context to a PNG file on disk.

HEADER:
mgraph.h

PARAMETERS:
dc          - Device context to save
PNGName     - Name of bitmap file to save
left        - Left coordinate of bitmap to save
top         - Top coordinate of bitmap to save
right       - Right coordinate of bitmap to save
bottom      - Bottom coordinate of bitmap to save
savePalette - True to save palette, false to skip it

RETURNS:
True on success, false on error.

REMARKS:
This function saves a portion of a device context as a PNG format bitmap file to
disk. If this function fails for some reason, it will return false and you can get the
error code from the MGL_result function.

This function supports saving in native formats of 1, 4, 8, 24, and 32 bits.
Images with bit depths of 15 or 16 bits will be expanded to 24 bits. It also
supports saving 8-bit + alpha bitmaps as 8-bit grayscale + alpha PNG files. The
PNG specification itself does not support saving the palette information, however
in practice you can save a palette to this bitmap format and have the bitmap load
and display correctly in most PNG viewers. MGL can also load those bitmaps properly
from disk as well. If you pass a value of false to 'savePalette' the palette
will not be saved in the PNG image (ie: resulting image is grayscale).

Note that the source rectangle for the bitmap to be saved is not clipped to the
current clipping rectangle for the device context, but it is mapped to the current
viewport. If you specify dimensions that are larger than the current device context,
you will get garbage in the bitmap file as a result.

SEE ALSO:
MGL_loadPNG, MGL_loadPNGIntoDC
****************************************************************************/
ibool MGLAPI MGL_savePNGFromDCExt(
    MGLDC *dc,
    const char *PNGName,
    int left,
    int top,
    int right,
    int bottom,
    ibool savePalette)
{
    FILE            *fp;
    palette_t       pal[256];
    png_color       pngpal[256];
    MGLDC           *memDC = NULL;
    png_structp     png_ptr;
    png_infop       info_ptr;
    png_uint_32     mw = right-left;
    png_uint_32     mh = bottom-top;
    int             bit_depth = 0;
    png_byte        color_type = 0;
    int             number_passes, pass;

    /* Attempt to open the file for writing */
    __MGL_result = grOK;
    if ((fp = __MGL_fopen(PNGName,"wb")) == NULL) {
        __MGL_result = grBitmapNotFound;
        return false;
        }

    /* If possible we will attempt to save every type of information in its
     * native format. For modes where there is no native format (15/16 bit)
     * we will convert it to 24 bit. Note, PNG uses different values for bit
     * depth than MGL. PNG bit depths refer to bits per plane
     */
    switch (dc->mi.bitsPerPixel) {
        case 1:
        case 4:
        case 8:
            memDC = MGL_createMemoryDC(right-left, 1, dc->mi.bitsPerPixel, NULL);
            bit_depth = dc->mi.bitsPerPixel;
            color_type = PNG_COLOR_TYPE_PALETTE;
            break;
        case 16:
            if (dc->mi.modeFlags & MGL_IS_COLOR_INDEX) {
                memDC = MGL_createMemoryDC(right-left, 1, 16, &_MGL_pixelFormats[pfCI8_A]);
                bit_depth = 8;
                color_type = PNG_COLOR_TYPE_GRAY_ALPHA;
                break;
                }
            // Fall through to 24-bit case if not 8-bit + alpha format
        case 15:
        case 24:
            memDC = MGL_createMemoryDC(right-left, 1, 24, &_MGL_pixelFormats[pfRGB24]);
            bit_depth = 8;
            color_type = PNG_COLOR_TYPE_RGB;
            break;
        case 32:
            memDC = MGL_createMemoryDC(right-left, 1, 24, &_MGL_pixelFormats[pfRGB24]);
            bit_depth = 8;
            color_type = PNG_COLOR_TYPE_RGB;
            break;
        }
    if (memDC == NULL) {
        fclose(fp);
        __MGL_result = grNoMem;
        return false;
        }

    /* Set palette for memory DC the same as input DC */
    if (dc->mi.modeFlags & MGL_IS_COLOR_INDEX) {
        MGL_getPalette(dc,pal,dc->mi.maxColor+1,0);
        MGL_setPalette(memDC,pal,dc->mi.maxColor+1,0);
        MGL_realizePalette(memDC,dc->mi.maxColor+1,0,false);
        }

    /* Create and initialize the png_struct with the desired error handler
     * functions.  If you want to use the default stderr and longjump method,
     * you can supply NULL for the last three parameters.  We also check that
     * the library version is compatible with the one used at compile time,
     * in case we are using dynamically linked libraries.
     */
    if ((png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, my_png_error, my_png_warn)) == NULL) {
        fclose(fp);
        __MGL_result = grErrorBPD;
        return false;
        }

    /* Allocate/initialize the image information data */
    if ((info_ptr = png_create_info_struct(png_ptr)) == NULL) {
        fclose(fp);
        png_destroy_write_struct(&png_ptr,  (png_infopp)NULL);
        __MGL_result = grErrorBPD;
        return false;
        }

    /* Set error handling for using the setjmp/longjmp method */
    if (setjmp(jmpbuf)) {
        /* Free all of the memory associated with the png_ptr and info_ptr */
        png_destroy_write_struct(&png_ptr,  (png_infopp)NULL);
        fclose(fp);
        __MGL_result = grErrorBPD;
        return false;
        }

    /* Replace the write functions with the MGL versions */
    png_set_write_fn(png_ptr, (void*)fp, my_png_write_data, my_png_flush_data);

    /* Flip RGB pixel ordering to the format we need (BGR according to PNG is actually RGB for us!) */
    png_set_bgr(png_ptr);

    /* Set the image information here.  Width and height are up to 2^31,
     * bit_depth is one of 1, 2, 4, 8, or 16, but valid values also depend on
     * the color_type selected. color_type is one of PNG_COLOR_TYPE_GRAY,
     * PNG_COLOR_TYPE_GRAY_ALPHA, PNG_COLOR_TYPE_PALETTE, PNG_COLOR_TYPE_RGB,
     * or PNG_COLOR_TYPE_RGB_ALPHA.  interlace is either PNG_INTERLACE_NONE or
     * PNG_INTERLACE_ADAM7, and the compression_type and filter_type MUST
     * currently be PNG_COMPRESSION_TYPE_BASE and PNG_FILTER_TYPE_BASE.
     */
    png_set_IHDR(png_ptr, info_ptr, mw, mh, bit_depth, color_type,
        PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
    if (savePalette && (dc->mi.modeFlags & MGL_IS_COLOR_INDEX)) {
        /* set the palette if there is one, for indexed-color images */
        int i;
        for (i = 0; i <= (int)dc->mi.maxColor; i++) {
            pngpal[i].red = pal[i].red;
            pngpal[i].green = pal[i].green;
            pngpal[i].blue = pal[i].blue;
            }
        png_set_PLTE(png_ptr, info_ptr, pngpal, (dc->mi.maxColor+1));
        }

    /* Write the file header information */
    png_write_info(png_ptr, info_ptr);

    /* Write out the image data by one or more scanlines
     * The number of passes is either 1 for non-interlaced images,
     * or 7 for interlaced images.
     */
    number_passes = png_set_interlace_handling(png_ptr);
    for (pass = 0; pass < number_passes; pass++) {
        int y;
        png_bytep p = (png_bytep)memDC->surface;
        for (y = 0; y < (int)mh; y++) {
            MGL_bitBltCoord(memDC,dc,
                left,top+y,
                right,top+y+1,
                0,0,MGL_REPLACE_MODE);
            png_write_rows(png_ptr, &p, 1);
            }
        }

    /* Clean up and exit */
    png_write_end(png_ptr, info_ptr);
    png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
    fclose(fp);
    MGL_destroyDC(memDC);
    return true;
}

