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
* Description:  Simple sample program showing how to use the MGL bitmap
*               functions to convert between bitmap formats. This sample
*               also shows how the MGL functionality can be used from
*               non-graphics applications (ie: MGL_memoryDC's without
*               needing a MGL_displayDC to be created).
*
****************************************************************************/

#include "mgraph.h"
#include "cmdline.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef  ISV_LICENSE
#include "snap/graphics.h"
#include "isv.c"
#endif

/*----------------------------- Global Variables --------------------------*/

int  format_bits[] = {15,16,24,24,32,32,32,32,-1};

char *format_names[] ={
    "15",
    "16",
    "RGB",
    "BGR",
    "ARGB",
    "ABGR",
    "RGBA",
    "BGRA"
    };

pixel_format_t format_pf[] = {
    {0x1F,0x0A,3,0x1F,0x05,3,0x1F,0x00,3,0x01,0x0F,7},  /* 555 15bpp    */
    {0x1F,0x0B,3,0x3F,0x05,2,0x1F,0x00,3,0x00,0x00,0},  /* 565 16bpp    */
    {0xFF,0x10,0,0xFF,0x08,0,0xFF,0x00,0,0x00,0x00,0},  /* RGB 24bpp    */
    {0xFF,0x00,0,0xFF,0x08,0,0xFF,0x10,0,0x00,0x00,0},  /* BGR 24bpp    */
    {0xFF,0x10,0,0xFF,0x08,0,0xFF,0x00,0,0xFF,0x18,0},  /* ARGB 32bpp   */
    {0xFF,0x00,0,0xFF,0x08,0,0xFF,0x10,0,0xFF,0x18,0},  /* ABGR 32bpp   */
    {0xFF,0x18,0,0xFF,0x10,0,0xFF,0x08,0,0xFF,0x00,0},  /* RGBA 32bpp   */
    {0xFF,0x08,0,0xFF,0x10,0,0xFF,0x18,0,0xFF,0x00,0},  /* BGRA 32bpp   */
    };

/*------------------------------ Implementation ---------------------------*/

/****************************************************************************
REMARKS:
Display the command line usage information.
****************************************************************************/
static void help(void)
{
    printf("Usage: convert [infile] [outtype] [outfile]\n");
    printf("\n");
    printf("Where [outtype] is one of the following:\n");
    printf("    bmp1     - Monochrome Windows BMP file\n");
    printf("    bmp4     - 4-bit Windows BMP file\n");
    printf("    bmp8     - 8-bit Windows BMP file\n");
    printf("    bmp15    - 15-bit Windows BMP file\n");
    printf("    bmp16    - 16-bit Windows BMP file\n");
    printf("    bmpRGB   - 24-bit Windows BMP file (RGB order)\n");
    printf("    bmpBGR   - 24-bit Windows BMP file (BGR order)\n");
    printf("    bmpARGB  - 32-bit Windows BMP file (ARGB order)\n");
    printf("    bmpABGR  - 32-bit Windows BMP file (ABGR order)\n");
    printf("    bmpRGBA  - 32-bit Windows BMP file (RGBA order)\n");
    printf("    bmpBGRA  - 32-bit Windows BMP file (BGRA order)\n");
    printf("    pcx8     - 8-bit PCX file\n");
    printf("    png1     - Monochrome PNG file\n");
    printf("    png4     - 4-bit PNG file\n");
    printf("    png8     - 8-bit PNG file\n");
    printf("    png24    - 24-bit PNG file\n");
    printf("    png32    - 32-bit PNG file\n");
    printf("    jpeg     - 24-bit JPEG file\n");
    exit(1);
}

/****************************************************************************
REMARKS:
Determine the length of the input file
****************************************************************************/
long fileSize(
    FILE *f)
{
    long    size,oldpos = ftell(f);
    fseek(f,0,SEEK_END);
    size = ftell(f);
    fseek(f,oldpos,SEEK_SET);
    return size;
}

/****************************************************************************
REMARKS:
Find the pixel format from the passed in name string.
****************************************************************************/
int findPixelFormat(
    char *name,
    int *bits,
    pixel_format_t *pf)
{
    int i;

    for (i = 0; i < format_bits[i] != -1; i++) {
        if (stricmp(name,format_names[i]) == 0) {
            *bits = format_bits[i];
            *pf = format_pf[i];
            return true;
            }
        }
    return false;
}

/****************************************************************************
REMARKS:
Main program entry point
****************************************************************************/
int main(int argc,char *argv[])
{
    FILE            *in;
    ulong           dwSize;
    int             width,height,bits;
    pixel_format_t  pf;
    MGLDC           *memDC;
    int             quality = 90;

    /* Parse the command line */
    if (argc != 4)
        help();

    /* Register the ISV license file if desired */
#ifdef  ISV_LICENSE
    GA_registerLicense(OemLicense,false);
#endif

    /* Initialise the MGL */
    if (MGL_init("..\\..\\..\\",NULL) == 0)
        MGL_fatalError(MGL_errorMsg(MGL_result()));

    /* Open input file and determine dimensions */
    if ((in = fopen(argv[1],"rb")) == NULL) {
        printf("Unable to open input file '%s'\n", argv[1]);
        return -1;
        }
    dwSize = fileSize(in);
    if (!MGL_getBitmapSizeExt(in,0,dwSize,&width,&height,&bits,&pf)) {
        if (!MGL_getPCXSizeExt(in,0,dwSize,&width,&height,&bits)) {
            if (!MGL_getJPEGSizeExt(in,0,dwSize,&width,&height,&bits,&pf)) {
                if (!MGL_getPNGSizeExt(in,0,dwSize,&width,&height,&bits,&pf)) {
                    printf("Unknown input bitmap format!\n");
                    return -1;
                    }
                }
            }
        }

    /* Create an MGL memory DC in the correct format for output bitmap,
     * load the bitmap and save it to disk in the new format
     */
    if (strnicmp(argv[2],"bmp",3) == 0) {
        if (!findPixelFormat(argv[2]+3,&bits,&pf)) {
            printf("Unknown destination pixel format!");
            return -1;
            }
        if ((memDC = MGL_createMemoryDC(width,height,bits,&pf)) == NULL)
            MGL_fatalError(MGL_errorMsg(MGL_result()));
        if (!MGL_saveBitmapFromDC(memDC,argv[3],0,0,width,height))
            MGL_fatalError(MGL_errorMsg(MGL_result()));
        }
    else if (strnicmp(argv[2],"pcx",3) == 0) {
        if ((memDC = MGL_createMemoryDC(width,height,8,NULL)) == NULL)
            MGL_fatalError(MGL_errorMsg(MGL_result()));
        if (!MGL_savePCXFromDC(memDC,argv[3],0,0,width,height))
            MGL_fatalError(MGL_errorMsg(MGL_result()));
        }
    else if (strnicmp(argv[2],"png",3) == 0) {
        if (!findPixelFormat(argv[2]+3,&bits,&pf)) {
            printf("Unknown destination pixel format!");
            return -1;
            }
        if ((memDC = MGL_createMemoryDC(width,height,bits,&pf)) == NULL)
            MGL_fatalError(MGL_errorMsg(MGL_result()));
        if (!MGL_savePNGFromDC(memDC,argv[3],0,0,width,height))
            MGL_fatalError(MGL_errorMsg(MGL_result()));
        }
    else if (strnicmp(argv[2],"jpeg",3) == 0) {
        if (!findPixelFormat("RGB",&bits,&pf)) {
            printf("Unknown destination pixel format!");
            return -1;
            }
        if ((memDC = MGL_createMemoryDC(width,height,bits,&pf)) == NULL)
            MGL_fatalError(MGL_errorMsg(MGL_result()));
        if (!MGL_saveJPEGFromDC(memDC,argv[3],0,0,width,height,quality))
            MGL_fatalError(MGL_errorMsg(MGL_result()));
        }
    else {
        printf("Unknown destination bitmap format!");
        return -1;
        }

    MGL_exit();
    fclose(in);
    return 0;
}
