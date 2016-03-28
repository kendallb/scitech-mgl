/****************************************************************************
*
*                   SciTech SNAP Graphics Graphics Architecture
*
*               Copyright (C) 1991-2002 SciTech Software, Inc.
*                            All rights reserved.
*
*  ======================================================================
*  |REMOVAL OR MODIFICATION OF THIS HEADER IS STRICTLY PROHIBITED BY LAW|
*  |                                                                    |
*  |This copyrighted computer code contains proprietary technology      |
*  |owned by SciTech Software, Inc., located at 505 Wall Street,        |
*  |Chico, CA 95928 USA (http://www.scitechsoft.com).                   |
*  |                                                                    |
*  |The contents of this file are subject to the SciTech SNAP Graphics  |
*  |License; you may *not* use this file or related software except in  |
*  |compliance with the License. You may obtain a copy of the License   |
*  |at http://www.scitechsoft.com/snap-license.txt                      |
*  |                                                                    |
*  |Software distributed under the License is distributed on an         |
*  |"AS IS" basis, WITHOUT WARRANTY OF ANY KIND, either express or      |
*  |implied. See the License for the specific language governing        |
*  |rights and limitations under the License.                           |
*  |                                                                    |
*  |REMOVAL OR MODIFICATION OF THIS HEADER IS STRICTLY PROHIBITED BY LAW|
*  ======================================================================
*
* Language:     ANSI C
* Environment:  Any 32-bit protected mode environment
*
* Description:  Header file for the disk based resource file structures
*               used by the SNAP conformance tests.
*
****************************************************************************/

#ifndef __RESOURCE_H
#define __RESOURCE_H

/*---------------------- Macro and type definitions -----------------------*/

#pragma pack(1)             /* Pack structures to byte granularity      */

/* Windows .BMP header and file structures */

typedef struct {
    N_uint8    bfType[2];
    N_uint32   bfSize;
    N_uint32   bfRes;
    N_uint32   bfOffBits;
    } winBITMAPFILEHEADER;

typedef struct {
    N_uint32   biSize;
    N_uint32   biWidth;
    N_uint32   biHeight;
    N_uint16   biPlanes;
    N_uint16   biBitCount;
    N_uint32   biCompression;
    N_uint32   biSizeImage;
    N_uint32   biXPelsPerMeter;
    N_uint32   biYPelsPerMeter;
    N_uint32   biClrUsed;
    N_uint32   biClrImportant;
    } winBITMAPINFOHEADER;

typedef struct {
    N_uint32   biSize;
    N_int32    biWidth;
    N_int32    biHeight;
    N_uint16   biPlanes;
    N_uint16   biBitCount;
    N_uint32   biCompression;
    N_uint32   biSizeImage;
    N_int32    biXPelsPerMeter;
    N_int32    biYPelsPerMeter;
    N_uint32   biClrUsed;
    N_uint32   biClrImportant;
    } winBITMAPINFOHEADER_full;

typedef struct {
    N_uint8    rgbBlue;
    N_uint8    rgbGreen;
    N_uint8    rgbRed;
    N_uint8    rgbReserved;
    } winRGBQUAD;

typedef struct {
    winBITMAPINFOHEADER_full    header;
    winRGBQUAD                  colors[256];
    } winBITMAPINFO;

typedef struct {
    winBITMAPINFOHEADER_full    header;
    N_uint32                    maskRed;
    N_uint32                    maskGreen;
    N_uint32                    maskBlue;
    } winBITMAPINFOEXT;

/* Constants for the biCompression field */

#define winBI_RGB       0L
#define winBI_RLE8      1L
#define winBI_RLE4      2L
#define winBI_BITFIELDS 3L

#pragma pack()              /* Return to default packing                */

#endif  /* __RESOURCE_H */
