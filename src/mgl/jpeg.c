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
* Description:  JPEG bitmap resource loading/unloading routines. We support
*               loading and saving of 24-bit imagery files, since the JPEG
*               format is RGB only. If you load a JPEG into an 8-bit device
*               context, it will be dithered. If you save a JPEG file from
*               an 8bpp device context it will be converted to a 24-bit
*               file when written to disk. Hence if you plan to edit JPEG
*               files and then save the results to disk, do this with
*               a 24-bpp display or memory DC for the best results.
*
****************************************************************************/

#include "mgl.h"
#include "jpeglib.h"
#include "jerror.h"

/*------------------------- Implementation --------------------------------*/

/*
 * ERROR HANDLING:
 *
 * The JPEG library's standard error handler (jerror.c) is divided into
 * several "methods" which you can override individually.  This lets you
 * adjust the behavior without duplicating a lot of code, which you might
 * have to update with each future release.
 *
 * We use C's setjmp/longjmp facility to return control.  This means that the
 * routine which calls the JPEG library must first execute a setjmp() call to
 * establish the return point.  We want the replacement error_exit to do a
 * longjmp().  But we need to make the setjmp buffer accessible to the
 * error_exit routine.  To do this, we make a private extension of the
 * standard JPEG error handler object.  (If we were using C++, we'd say we
 * were making a subclass of the regular error handler.)
 *
 * Here's the extended error handler struct:
 */

/* {secret} */
typedef struct {
    struct jpeg_error_mgr pub;
    jmp_buf setjmp_buffer;
    } my_error_mgr;

/* {secret} */
typedef my_error_mgr *my_error_ptr;
/* {secret} */
typedef struct jpeg_compress_struct jpeg_compress;
/* {secret} */
typedef struct jpeg_decompress_struct jpeg_decompress;

/****************************************************************************
PARAMETERS:
cinfo   - Pointer to the information for the error handler

REMARKS:
Here's the routine that will replace the standard error_exit method, and
will get called when an error occurs in the library so we can handle it.
****************************************************************************/
static void PMAPI my_error_exit(
    j_common_ptr cinfo)
{
    /* cinfo->err really points to a my_error_mgr struct */
    my_error_ptr myerr = (my_error_ptr) cinfo->err;
#ifdef  CHECKED
    FILE    *f;
    char    msg[256];

    cinfo->err->format_message(cinfo,msg);
    if ((f = fopen("jpeg.log","a+")) == NULL)
        exit(1);
    fprintf(f,"%s\n",msg);
    fclose(f);
#endif

    /* Set internal MGL error code */
    __MGL_result = grInvalidBitmap;

    /* Return control to the setjmp point */
    longjmp(myerr->setjmp_buffer, 1);
}

/****************************************************************************
DESCRIPTION:
Obtain the dimensions of a JPEG file from an opened file.

HEADER:
mgraph.h

PARAMETERS:
f               - Open binary file to read bitmap from
dwOffset        - Offset to start of JPEG file
dwSize          - Size of JPEG file
width           - Width of bitmap
height          - Height of bitmap
bitsPerPixel    - Pixel depth of bitmap
pf              - Place to store the bitmap pixel format information

RETURNS:
True if JPEG was found, false if not.

REMARKS:
This function is the same as MGL_getJPEGSize, however it works with a
previously opened file. This allows you to create your own large files with
multiple files embedded in them.

Note that JPEG files are inherently 24-bit, so when you call this function
it will always return information for a 24-bit RGB pixel format image.

SEE ALSO:
MGL_getJPEGSize
****************************************************************************/
ibool MGLAPI MGL_getJPEGSizeExt(
    FILE *f,
    ulong dwOffset,
    ulong dwSize,
    int *width,
    int *height,
    int *bitsPerPixel,
    pixel_format_t *pf)
{
    jpeg_decompress cinfo;      /* Main JPEG decompressor object        */
    my_error_mgr    jerr;       /* Error handling object                */

    /* We set up the normal JPEG error routines, then override error_exit */
    __MGL_result = grOK;
    if ((cinfo.err = jpeg_std_error(&jerr.pub)) == NULL)
        MGL_fatalError("Unable to load JPEG library!");
    jerr.pub.error_exit = my_error_exit;
    if (setjmp(jerr.setjmp_buffer)) {
        /* If we get here, the JPEG code has signaled an error.
         * We need to clean up the JPEG object, close the input file, and
         * return.
         */
        jpeg_destroy_decompress(&cinfo);
        __MGL_result = grErrorBPD;
        return false;
        }

    /* Initialize the JPEG decompression object and read header */
    __MGL_fseek(f,dwOffset,SEEK_SET);
    jpeg_create_decompress(&cinfo);
    jpeg_stdio_src(&cinfo, f);
    jpeg_read_header(&cinfo, TRUE);
    if (cinfo.num_components != 1 && cinfo.num_components != 3) {
        /* Only support 24-bit or grayscale images! */
        jpeg_destroy_decompress(&cinfo);
        __MGL_result = grInvalidBitmap;
        return false;
        }

    /* Get the bitmap information from the header */
    *width = cinfo.image_width;
    *height = cinfo.image_height;
    *bitsPerPixel = 24;
    *pf = _MGL_pixelFormats[pfRGB24];

    /* Finish and clean up */
    jpeg_destroy_decompress(&cinfo);
    __MGL_fseek(f,dwSize,SEEK_SET);
    return true;
}

/****************************************************************************
DESCRIPTION:
Obtain the dimensions of a JPEG file from disk.

HEADER:
mgraph.h

PARAMETERS:
JPEGName        - Name of the bitmap file to load header for
width           - Place to store the bitmap width
height          - Place to store the bitmap height
bitsPerPixel    - Place to store the bitmap pixel depth
pf              - Place to store the bitmap pixel format information

RETURNS:
True if the JPEG file was found, false if not.

REMARKS:
This functions loads all the header information for a JPEG file from disk,
without actually loading the bits for the bitmap surface. This is useful to
determine the dimensions and pixel format for the bitmap before it is loaded,
so you can create an appropriate memory device context that you can load the
bitmap into with the MGL_loadJPEGIntoDC function.

Note that JPEG files are inherently 24-bit, so when you call this function
it will always return information for a 24-bit RGB pixel format image.

SEE ALSO:
MGL_loadJPEG, MGL_loadJPEGIntoDC
****************************************************************************/
ibool MGLAPI MGL_getJPEGSize(
    const char *JPEGName,
    int *width,
    int *height,
    int *bitsPerPixel,
    pixel_format_t *pf)
{
    FILE    *f;
    ibool   ret;

    __MGL_result = grOK;
    if ((f = _MGL_openFile(MGL_BITMAPS, JPEGName, "rb")) == NULL) {
        __MGL_result = grBitmapNotFound;
        return false;
        }
    ret = MGL_getJPEGSizeExt(f,0,_MGL_fileSize(f),width,height,bitsPerPixel,pf);
    __MGL_fclose(f);
    return ret;
}

/****************************************************************************
DESCRIPTION:
Load a JPEG bitmap file from disk using an open file.

HEADER:
mgraph.h

PARAMETERS:
f               - Open binary file to read bitmap from
dwOffset        - Offset to start of JPEG file within open file
dwSize          - Size of JPEG file in bytes
num8BitColors   - Number of colors for 8-bit image, 0 for RGB images, -1 for grayscale

RETURNS:
Pointer to the loaded bitmap file

REMARKS:
This function is the same as MGL_loadJPEG, however it loads the file from a
previously open file. This allows you to create your own large files with
multiple files embedded in them.

SEE ALSO:
MGL_loadJPEG, MGL_loadBitmap
****************************************************************************/
bitmap_t * MGLAPI MGL_loadJPEGExt(
    FILE *f,
    ulong dwOffset,
    ulong dwSize,
    int num8BitColors)
{
    bitmap_t        *bitmap;    /* MGL bitmap file being created        */
    ulong           size;       /* Size of the bitmap image             */
    jpeg_decompress cinfo;      /* Main JPEG decompressor object        */
    my_error_mgr    jerr;       /* Error handling object                */
    JSAMPARRAY      buffer;     /* Output row buffer                    */
    int             i,row_stride;/* physical row width in output buffer */
    uchar           *p,*pb;

    /* We set up the normal JPEG error routines, then override error_exit */
    __MGL_result = grOK;
    if ((cinfo.err = jpeg_std_error(&jerr.pub)) == NULL)
        MGL_fatalError("Unable to load JPEG library!");
    jerr.pub.error_exit = my_error_exit;
    if (setjmp(jerr.setjmp_buffer)) {
        /* If we get here, the JPEG code has signaled an error.
         * We need to clean up the JPEG object, close the input file, and
         * return.
         */
        jpeg_destroy_decompress(&cinfo);
        __MGL_result = grErrorBPD;
        return NULL;
        }

    /* Initialize the JPEG decompression object and read header */
    __MGL_fseek(f,dwOffset,SEEK_SET);
    jpeg_create_decompress(&cinfo);
    jpeg_stdio_src(&cinfo, f);
    jpeg_read_header(&cinfo, TRUE);
    if (cinfo.num_components != 1 && cinfo.num_components != 3) {
        /* Only support 24-bit or grayscale images! */
        jpeg_destroy_decompress(&cinfo);
        __MGL_result = grInvalidBitmap;
        return NULL;
        }
    if (cinfo.num_components == 1)
        num8BitColors = -1;

    /* Allocate memory for the bitmap */
    if (num8BitColors != 0) {
        size = sizeof(bitmap_t) + sizeof(pixel_format_t)
             + sizeof(palette_t)*256
             + cinfo.image_width * cinfo.image_height;
        }
    else {
        size = sizeof(bitmap_t) + sizeof(pixel_format_t)
             + cinfo.image_width * 3 * cinfo.image_height;
        }
    if ((bitmap = PM_malloc(size)) == NULL) {
        jpeg_destroy_decompress(&cinfo);
        FATALERROR(grNoMem);
        return NULL;
        }
    size = sizeof(bitmap_t);
    bitmap->width = cinfo.image_width;
    bitmap->height = cinfo.image_height;
    if (num8BitColors != 0) {
        /* 8-bits per pixel with a palette */
        bitmap->bitsPerPixel = 8;
        bitmap->bytesPerLine = cinfo.image_width;
        bitmap->pal = (palette_t*)((uchar*)bitmap + size);
        bitmap->pf = NULL;
        size += sizeof(palette_t) * 256;
        memset(bitmap->pal,0,sizeof(palette_t) * 256);
        }
    else {
        /* 24-bit RGB */
        bitmap->bitsPerPixel = 24;
        bitmap->bytesPerLine = cinfo.image_width * 3;
        bitmap->pal = NULL;
        bitmap->pf = (pixel_format_t*)((uchar*)bitmap + size);
        memcpy(bitmap->pf,&_MGL_pixelFormats[pfRGB24],sizeof(pixel_format_t));
        size += sizeof(pixel_format_t);
        }
    bitmap->surface = (uchar*)bitmap + size;

    /* Set parameters for decompression to 8-bits per pixel */
    if (num8BitColors > 0) {
        cinfo.quantize_colors = true;
        cinfo.desired_number_of_colors = num8BitColors;
        cinfo.two_pass_quantize = true;
        cinfo.dither_mode = JDITHER_FS;
        }
    else if (num8BitColors == -1) {
        /* Decode to a grayscale image */
        cinfo.out_color_space = JCS_GRAYSCALE;
        }

    /* Start decompressor and decode the image a scanline at a time */
    jpeg_start_decompress(&cinfo);
    p = bitmap->surface;
    row_stride = cinfo.output_width * cinfo.output_components;
    buffer = (*cinfo.mem->alloc_sarray)((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);
    if (num8BitColors > 0) {
        /* Decode the image to 8 bits per pixel data */
        while (cinfo.output_scanline < cinfo.output_height) {
            jpeg_read_scanlines(&cinfo, &p, 1);
            p += bitmap->bytesPerLine;
            }
        /* Copy the quantized color map */
        for (i = 0; i < cinfo.actual_number_of_colors; i++) {
            bitmap->pal[i].red = cinfo.colormap[0][i];
            bitmap->pal[i].green = cinfo.colormap[1][i];
            bitmap->pal[i].blue = cinfo.colormap[2][i];
            }
        }
    else if (cinfo.output_components == 3) {
        /* Swap the RGB ordering as our default is the reverse */
        while (cinfo.output_scanline < cinfo.output_height) {
            jpeg_read_scanlines(&cinfo, buffer, 1);
            for (i = 0,pb = buffer[0]; i < (int)cinfo.image_width; i++,pb += 3,p += 3) {
                p[0] = pb[2];
                p[1] = pb[1];
                p[2] = pb[0];
                }
            }
        }
    else {
        /* Decode the image to 8 bits per pixel grayscale data */
        while (cinfo.output_scanline < cinfo.output_height) {
            jpeg_read_scanlines(&cinfo, &p, 1);
            p += bitmap->bytesPerLine;
            }
        /* Set the grayscale color map */
        for (i = 0; i < 256; i++) {
            bitmap->pal[i].red = i;
            bitmap->pal[i].green = i;
            bitmap->pal[i].blue = i;
            }
        }

    /* Finish decompression and clean up */
    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    __MGL_fseek(f,dwSize,SEEK_SET);
    return bitmap;
}

/****************************************************************************
DESCRIPTION:
Load a JPEG bitmap file from disk.

HEADER:
mgraph.h

PARAMETERS:
JPEGName        - Name of JPEG file to load
num8BitColors   - Number of colors for 8-bit image, 0 for RGB images, -1 for grayscale

RETURNS:
Pointer to the loaded JPEG file, NULL on error.

REMARKS:
Locates the specified JPEG file and loads it into a lightweight bitmap
structure. Because JPEG files are inherently 24-bit, when you load a JPEG
file with this function it will always be decoded as a 24-bit RGB bitmap
file, unless you set num8BitColors parameter. If the num8BitColors parameter is set
to a value other than 0, it causes the JPEG decoder to quantize down to an
8 bits per pixel bitmap with an optimized floyd-steinberg dither (better
than the MGL's simple halftone dithering, but the bitmap will contain a
custom palette). The number of significant colors in the output image will
be set to the value you specify, so you can use this to quantise down to
a color table smaller than 8-bits per pixel. To decode as a 24-bit image,
simply set this field to 0.

Note that if you set num8BitColors to -1, the JPEG decoder will decode the
image as a grayscale bitmap which is faster than decoding the full color
image (useful for preview operations etc). Note that images that are
decoded as grayscale are 8-bits per pixel with a grayscale color map and
should display as true grayscale images in all color depths.

If you wish to load the bitmap as a
different color depth or pixel format use the MGL_loadJPEGIntoDC function.
The MGL will however properly decode grayscale JPEG files, but they will be
loaded as a 24-bit bitmap since the MGL does not natively support grayscale
bitmaps.

When MGL is searching for JPEG files it will first attempt to find the files
just by using the filename itself. Hence if you wish to look for a specific
JPEG file, you should pass the full pathname to the file that you are
interested in. If the filename is a simple relative filename (i.e.
"MYBMP.JPG"), MGL will then search in the BITMAPS directory relative to the
path specified in mglpath variable that was passed to MGL_init. As a final
resort MGL will also look for the files in the BITMAPS directory relative
to the MGL_ROOT environment variable.

If the JPEG file was not found, or an error occurred while reading the JPEG
file, this function will return NULL. You can check the MGL_result error
code to determine the cause.

The routine allocates a lightweight bitmap structure for holding the JPEG
file, which loads the bitmap with the minimum memory overheads. You can draw
the JPEG file on any device context surface using the MGL_putBitmap function,
but you don't have the full flexibility of using a memory device context for
the bitmap surface. If you need more control over the bitmap, you can
allocate a memory device context to hold the bitmap data and load the bitmap
with the MGL_loadJPEGIntoDC function.

SEE ALSO:
MGL_unloadBitmap, MGL_availableJPEG, MGL_getJPEGSize,
MGL_loadJPEGIntoDC, MGL_saveJPEGFromDC, MGL_putBitmap, MGL_loadJPEGExt
****************************************************************************/
bitmap_t * MGLAPI MGL_loadJPEG(
    const char *JPEGName,
    int num8BitColors)
{
    FILE        *f;
    bitmap_t    *b;

    __MGL_result = grOK;
    if ((f = _MGL_openFile(MGL_BITMAPS, JPEGName, "rb")) == NULL) {
        __MGL_result = grBitmapNotFound;
        return false;
        }
    b = MGL_loadJPEGExt(f,0,_MGL_fileSize(f),num8BitColors);
    __MGL_fclose(f);
    return b;
}

/****************************************************************************
DESCRIPTION:
Loads a JPEG file directly into an existing device context using an open file.

HEADER:
mgraph.h

PARAMETERS:
dc              - Device context to load bitmap into
f               - Open binary file to read bitmap from
dwOffset        - Offset to start of JPEG file within open file
dwSize          - Size of JPEG file in bytes
dstLeft         - Left coordinate to load JPEG at
dstTop          - Top coordinate to load JPEG at
num8BitColors   - Number of colors for 8-bit image, 0 for RGB images, -1 for grayscale

RETURNS:
Pointer to the loaded JPEG file, NULL on error.

REMARKS:
This function is the same as MGL_loadJPEGIntoDC, however it loads the file from
a previously open file. This allows you to create your own large files with
multiple files embedded in them.

SEE ALSO:
MGL_loadJPEGIntoDC, MGL_loadBitmapIntoDC
****************************************************************************/
ibool MGLAPI MGL_loadJPEGIntoDCExt(
    MGLDC *dc,
    FILE *f,
    ulong dwOffset,
    ulong dwSize,
    int dstLeft,
    int dstTop,
    int num8BitColors)
{
    bitmap_t        bmp;
    jpeg_decompress cinfo;      /* Main JPEG decompressor object        */
    my_error_mgr    jerr;       /* Error handling object                */
    palette_t       pal[256];           /* Temporary space for palette  */
    uchar           cmap_red[256];
    uchar           cmap_green[256];
    uchar           cmap_blue[256];
    uchar           *colormap[3];
    ibool           oldCheckId;
    int             i;
    uchar           *p;

    /* We set up the normal JPEG error routines, then override error_exit */
    __MGL_result = grOK;
    if ((cinfo.err = jpeg_std_error(&jerr.pub)) == NULL)
        MGL_fatalError("Unable to load JPEG library!");
    jerr.pub.error_exit = my_error_exit;
    bmp.surface = NULL;
    if (setjmp(jerr.setjmp_buffer)) {
        /* If we get here, the JPEG code has signaled an error.
         * We need to clean up the JPEG object, close the input file, and
         * return.
         */
        PM_free(bmp.surface);
        jpeg_destroy_decompress(&cinfo);
        __MGL_result = grErrorBPD;
        return false;
        }

    /* Initialize the JPEG decompression object and read header */
    __MGL_fseek(f,dwOffset,SEEK_SET);
    jpeg_create_decompress(&cinfo);
    jpeg_stdio_src(&cinfo, f);
    jpeg_read_header(&cinfo, TRUE);
    if (cinfo.num_components != 1 && cinfo.num_components != 3) {
        /* Only support 24-bit or grayscale images! */
        jpeg_destroy_decompress(&cinfo);
        __MGL_result = grInvalidBitmap;
        return false;
        }
    if (cinfo.num_components == 1)
        num8BitColors = -1;

    /* Allocate a temporary bitmap for the bitmap scanlines */
    bmp.width = cinfo.image_width;
    bmp.height = 1;
    if (num8BitColors != 0) {
        bmp.bitsPerPixel = 8;
        bmp.bytesPerLine = cinfo.image_width;
        bmp.pal = pal;
        }
    else {
        bmp.bitsPerPixel = 24;
        bmp.bytesPerLine = cinfo.image_width*3;
        bmp.pf = &_MGL_pixelFormats[pfBGR24];
        }
    if ((bmp.surface = PM_malloc(bmp.bytesPerLine)) == NULL) {
        jpeg_destroy_decompress(&cinfo);
        FATALERROR(grNoMem);
        return false;
        }
    oldCheckId = MGL_checkIdentityPalette(false);

    /* Store the grayscale palette in the destination DC */
    if (num8BitColors == -1) {
        for (i = 0; i < 256; i++) {
            pal[i].red = i;
            pal[i].green = i;
            pal[i].blue = i;
            pal[i].alpha = 0;
            }
        if (dc->mi.modeFlags & MGL_IS_COLOR_INDEX) {
            MGL_setPalette(dc,pal,256,0);
            if (MGL_getVisualPage(dc) == MGL_getActivePage(dc))
                MGL_realizePalette(dc,256,0,false);
            }
        }

    /* Set parameters for decompression to 8-bits per pixel */
    if (num8BitColors > 0) {
        cinfo.quantize_colors = true;
        cinfo.desired_number_of_colors = num8BitColors;
        cinfo.two_pass_quantize = true;
        cinfo.dither_mode = JDITHER_FS;

        /* Setup the quantized color map */
        cinfo.actual_number_of_colors = num8BitColors;
        cinfo.colormap = colormap;
        colormap[0] = cmap_red;
        colormap[1] = cmap_green;
        colormap[2] = cmap_blue;
        MGL_getPalette(dc,pal,256,0);
        for (i = 0; i < cinfo.actual_number_of_colors; i++) {
            cinfo.colormap[0][i] = pal[i].red;
            cinfo.colormap[1][i] = pal[i].green;
            cinfo.colormap[2][i] = pal[i].blue;
            }
        }
    else if (num8BitColors == -1) {
        /* Decode to a grayscale image */
        cinfo.out_color_space = JCS_GRAYSCALE;
        }

    /* Start decompressor and decode the image a scanline at a time */
    jpeg_start_decompress(&cinfo);
    p = bmp.surface;
    while (cinfo.output_scanline < cinfo.output_height) {
        jpeg_read_scanlines(&cinfo, &p, 1);
        MGL_putBitmap(dc,dstLeft,dstTop+cinfo.output_scanline,&bmp,MGL_REPLACE_MODE);
        }

    /* Finish decompression and clean up */
    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    PM_free(bmp.surface);
    MGL_checkIdentityPalette(oldCheckId);
    __MGL_fseek(f,dwSize,SEEK_SET);
    return true;
}

/****************************************************************************
DESCRIPTION:
Loads a JPEG file directly into an existing device context.

HEADER:
mgraph.h

PARAMETERS:
dc              - Device context to load bitmap into
JPEGName        - Name of JPEG file to load
dstLeft         - Left coordinate to load JPEG at
dstTop          - Top coordinate to load JPEG at
num8BitColors   - Number of colors for 8-bit image, 0 for RGB images, -1 for grayscale

RETURNS:
True if the JPEG file was loaded, false on error.

REMARKS:
Locates the specified JPEG file and loads it into the specified device context at the
specified destination coordinates. If the JPEG is of a different pixel depth than the
device context that it is being loaded into, the JPEG will be converted as it is loaded
to the pixel format of the device context it is being loaded into.

If the num8BitColors parameter is set to a value other than 0, it causes the
JPEG decoder to quantize down to an 8 bits per pixel bitmap with an
optimized floyd-steinberg dither (better than the MGL's simple halftone
dithering) using the palette of the destination device context as the source
for the dither. The number of significant colors used from the destination
device contexts palette for the dither is set by the num8BitColors parameter.
If however the destination device context is not 8-bits per pixel, the image
is simply decoded as a 24-bit bitmap.

Note that if you set num8BitColors to -1, the JPEG decoder will decode the
image as a grayscale bitmap which is faster than decoding the full color
image (useful for preview operations etc). Note that images that are
decoded as grayscale are 8-bits per pixel with a grayscale color map and
should display as true grayscale images in all color depths. If the
destination device context is 8-bits per pixel, the color palette will be
changed to a grayscale color palette.

When MGL is searching for bitmap files it will first attempt to find the files just by
using the filename itself. Hence if you wish to look for a specific bitmap file, you
should pass the full pathname to the file that you are interested in. If the filename is
a simple relative filename (i.e. "MYFILE.JPEG"), MGL will then search in the
BITMAPS directory relative to the path specified in mglpath variable that was
passed to MGL_init. As a final resort MGL will also look for the files in the
BITMAPS directory relative to the MGL_ROOT environment variable.

If the bitmap file was not found, or an error occurred while reading the bitmap file,
this function will return false. You can check the MGL_result error code to
determine the cause.

SEE ALSO:
MGL_availableJPEG, MGL_getJPEGSize,MGL_loadJPEG, MGL_saveJPEGFromDC
****************************************************************************/
ibool MGLAPI MGL_loadJPEGIntoDC(
    MGLDC *dc,
    const char *JPEGName,
    int dstLeft,
    int dstTop,
    int num8BitColors)
{
    FILE    *f;
    ibool   ret;

    __MGL_result = grOK;
    if ((f = _MGL_openFile(MGL_BITMAPS, JPEGName, "rb")) == NULL) {
        __MGL_result = grBitmapNotFound;
        return false;
        }
    ret = MGL_loadJPEGIntoDCExt(dc,f,0,_MGL_fileSize(f),dstLeft,dstTop,num8BitColors);
    __MGL_fclose(f);
    return ret;
}

/****************************************************************************
DESCRIPTION:
Save a portion of a device context to JPEG on disk.

HEADER:
mgraph.h

PARAMETERS:
dc          - Device context to save
JPEGName    - Name of bitmap file to save
left        - Left coordinate of bitmap to save
top         - Top coordinate of bitmap to save
right       - Right coordinate of bitmap to save
bottom      - Bottom coordinate of bitmap to save
quality     - Quality factor for compression (1-100)

RETURNS:
True on success, false on error.

REMARKS:
This function saves a portion of a device context as a JPEG format bitmap file to
disk. If this function fails for some reason, it will return false and you can get the
error code from the MGL_result function.

Note that the source rectangle for the bitmap to be saved is not clipped to the
current clipping rectangle for the device context, but it is mapped to the current
viewport. If you specify dimensions that are larger than the current device context,
you will get garbage in the bitmap file as a result.

Note that MGL currently only supports saving bitmap data to JPEG files from 8 bits
per pixel device contexts.

SEE ALSO:
MGL_LoadJPEG,MGL_loadJPEGIntoDC
****************************************************************************/
ibool MGLAPI MGL_saveJPEGFromDC(
    MGLDC *dc,
    const char *JPEGName,
    int left,
    int top,
    int right,
    int bottom,
    int quality)
{
    FILE            *f;
    jpeg_compress   cinfo;      /* Main JPEG compressor object          */
    my_error_mgr    jerr;       /* Error handling object                */
    palette_t       pal[256];
    MGLDC           *memDC;
    uchar           *p;

    /* Attempt to open the file for writing */
    __MGL_result = grOK;
    if ((f = __MGL_fopen(JPEGName,"wb")) == NULL) {
        __MGL_result = grBitmapNotFound;
        return false;
        }

    /* Allocate a temporary buffer in the correct format for compression */
    if ((memDC = MGL_createMemoryDC(right-left,1,24,&_MGL_pixelFormats[pfBGR24])) == NULL) {
        fclose(f);
        __MGL_result = grNoMem;
        return false;
        }
    /* Set palette for 24-bit memory DC the same as input DC */
    MGL_getPalette(dc,pal,256,0);
    MGL_setPalette(memDC,pal,256,0);
    MGL_realizePalette(memDC,256,0,false);

    /* We set up the normal JPEG error routines, then override error_exit */
    if ((cinfo.err = jpeg_std_error(&jerr.pub)) == NULL)
        MGL_fatalError("Unable to load JPEG library!");
    jerr.pub.error_exit = my_error_exit;
    if (setjmp(jerr.setjmp_buffer)) {
        /* If we get here, the JPEG code has signaled an error.
         * We need to clean up the JPEG object, close the output file, and
         * return.
         */
        fclose(f);
        MGL_destroyDC(memDC);
        jpeg_destroy_compress(&cinfo);
        __MGL_result = grErrorBPD;
        return false;
        }

    /* Initialize the JPEG decompression object and read header */
    jpeg_create_compress(&cinfo);
    jpeg_stdio_dest(&cinfo, f);

    /* Set parameters for compression. Input data is always 24-bit RGB */
    cinfo.image_width = right-left;
    cinfo.image_height = bottom-top;
    cinfo.input_components = 3;
    cinfo.in_color_space = JCS_RGB;
    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, quality, true);

    /* Loop around passing one scanline at a time to the compressor */
    jpeg_start_compress(&cinfo, true);
    p = memDC->surface;
    while (cinfo.next_scanline < cinfo.image_height) {
        /* jpeg_write_scanlines expects an array of pointers to scanlines.
         * Here the array is only one element long, but you could pass
         * more than one scanline at a time if that's more convenient.
         */
        MGL_bitBltCoord(memDC,dc,
            left,top+cinfo.next_scanline,
            right,top+cinfo.next_scanline+1,
            0,0,MGL_REPLACE_MODE);
        jpeg_write_scanlines(&cinfo, &p, 1);
        }

    /* Finish compression and clean up */
    jpeg_finish_compress(&cinfo);
    jpeg_destroy_compress(&cinfo);
    fclose(f);
    MGL_destroyDC(memDC);
    return true;
}

/****************************************************************************
DESCRIPTION:
Determines if the specified JPEG bitmap file is available for use.

HEADER:
mgraph.h

PARAMETERS:
JPEGName    - Name of JPEG bitmap file to check for

RETURNS:
True if the JPEG bitmap file exists, false if not.

REMARKS:
Attempt to locate the specified JPEG file, and verify that it is available
for use. See MGL_loadJPEG for more information on the algorithm that MGL
uses when searching for bitmap files on disk.

SEE ALSO:
MGL_loadJPEG
****************************************************************************/
ibool MGLAPI MGL_availableJPEG(
    const char *JPEGName)
{
    FILE        *f;

    __MGL_result = grOK;
    if ((f = _MGL_openFile(MGL_BITMAPS, JPEGName, "rb")) == NULL) {
        __MGL_result = grBitmapNotFound;
        return false;
        }
    __MGL_fclose(f);
    return true;
}
