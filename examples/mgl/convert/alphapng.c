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
* Description:  Simple sample program to convert 32-bit ARGB PNG files with
*               alpha into 8-bit grayscale + alpha bitmap files. The
*               resulting file is not actually a grayscale bitmap, but rather
*               a color index bitmap that will match the palette of the
*               passed f palette PNG bitmap file (which must be an 8-bit
*               color index bitmap). This program can be used to convert
*               bitmaps to be used f the MGL for 8-bit + alpha blending
*               functions. We have to use the PNG greyscale bitmap format
*               because PNG does not support 8-bit color index + alpha
*               bitmaps (even though it theoretically could).
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

pixel_format_t alpha_pf = {0xFF,0x00,0,0x00,0x00,0,0x00,0x00,0,0xFF,0x08,0};

/*------------------------------ Implementation ---------------------------*/

/****************************************************************************
REMARKS:
Display the command line usage information.
****************************************************************************/
static void help(void)
{
    printf("Usage: alphapng <infile> <palette> <outfile> [grayscale]\n");
    printf("\n");
    printf("Where <infile> is a 32-bit ARGB PNG bitmap file, <palette> is an:\n");
    printf("8-bit color index PNG bitmap file and <outfile> is where the resulting\n");
    printf("8-bit bitmap will be written. If 'grayscale' is passed as the\n");
    printf("last parameter, the color palette will not be saved and the bitmap\n");
    printf("will be saved in grayscale format.\n");
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
Main program entry point
****************************************************************************/
int ConvertBitmap(
    const char *inputFilename,
    const char *paletteFilename,
    const char *outputFilename,
    ibool grayscale)
{
    FILE            *f;
    ulong           dwSize;
    int             status = 0,width,height,bits;
    pixel_format_t  pf;
    bitmap_t        *inputBMP = NULL;
    bitmap_t        *paletteBMP = NULL;
    MGLDC           *memDC = NULL;

    /* Open input file and determine pixel format */
    if ((f = fopen(inputFilename,"rb")) == NULL) {
        printf("Unable to open input file '%s'\n", inputFilename);
        status = -1;
        goto Exit;
        }
    dwSize = fileSize(f);
    if (!MGL_getPNGSizeExt(f,0,dwSize,&width,&height,&bits,&pf)) {
        printf("Unknown input bitmap format!\n");
        status = -1;
        goto Exit;
        }
    if (bits != 32) {
        printf("Input bitmap must be a 32-bit PNG file!\n");
        status = -1;
        goto Exit;
        }
    fclose(f);

    /* Create the memory DC to hold the output bitmap */
    if ((memDC = MGL_createMemoryDC(width,height,16,&alpha_pf)) == NULL) {
        printf("%s\n", MGL_errorMsg(MGL_result()));
        status = -1;
        goto Exit;
        }

    /* Open palette file and determine pixel format */
    if ((f = fopen(paletteFilename,"rb")) == NULL) {
        printf("Unable to open palette file '%s'\n", paletteFilename);
        status = -1;
        goto Exit;
        }
    dwSize = fileSize(f);
    if (!MGL_getPNGSizeExt(f,0,dwSize,&width,&height,&bits,&pf)) {
        printf("Unknown palette bitmap format!\n");
        status = -1;
        goto Exit;
        }
    if (bits != 8) {
        printf("Palette bitmap must be an 8-bit PNG file!\n");
        status = -1;
        goto Exit;
        }
    fclose(f);

    /* Load the input bitmap */
    if ((inputBMP = MGL_loadPNG(inputFilename,true)) == NULL) {
        printf("%s\n", MGL_errorMsg(MGL_result()));
        status = -1;
        goto Exit;
        }

    /* Load the palette bitmap */
    if ((paletteBMP = MGL_loadPNG(paletteFilename,true)) == NULL) {
        printf("%s\n", MGL_errorMsg(MGL_result()));
        status = -1;
        goto Exit;
        }

    /* Set palette in output bitmap, down convert the input bitmap with
     * closest color matching and save it to disk in the new format.
     */
    MGL_makeCurrentDC(memDC);
    MGL_setDitherMode(MGL_DITHER_OFF);
    MGL_setPalette(memDC,paletteBMP->pal,256,0);
    MGL_realizePalette(memDC,256,0,true);
    MGL_putBitmap(memDC,0,0,inputBMP,MGL_REPLACE_MODE);
    if (!MGL_savePNGFromDCExt(memDC,outputFilename,0,0,MGL_sizex(memDC),MGL_sizey(memDC),!grayscale)) {
        printf("%s\n", MGL_errorMsg(MGL_result()));
        status = -1;
        }

Exit:
    if (inputBMP)
        MGL_unloadBitmap(inputBMP);
    if (paletteBMP)
        MGL_unloadBitmap(paletteBMP);
    if (memDC)
        MGL_destroyDC(memDC);
    return status;
}

/****************************************************************************
REMARKS:
Main program entry point
****************************************************************************/
int main(int argc,char *argv[])
{
    int status;

    /* Parse the command line */
    if (argc < 4)
        help();

    /* Register the ISV license file if desired */
#ifdef  ISV_LICENSE
    GA_registerLicense(OemLicense,false);
#endif

    /* Initialise the MGL */
    if (MGL_init("..\\..\\..\\",NULL) == 0)
        MGL_fatalError(MGL_errorMsg(MGL_result()));

    /* Do the conversion */
    status = ConvertBitmap(argv[1],argv[2],argv[3],argc > 4);

    /* Clean up and exit */
    MGL_exit();
    return status;
}

