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
* Description:  Header file for the disk based resource file structures
*               used by the MGL.
*
****************************************************************************/

#ifndef __RESOURCE_H
#define __RESOURCE_H

/*---------------------- Macro and type definitions -----------------------*/

#pragma pack(1)             /* Pack structures to byte granularity      */

/* Windows .CUR header and file structures */

typedef struct {
    M_uint8  bWidth;
    M_uint8  bHeight;
    M_uint8  bColorCount;
    M_uint8  bReserved;
    M_uint16 wXHotSpot;
    M_uint16 wYHotSpot;
    M_uint32 dwBytesInRes;
    M_uint32 dwImageOffset;
    } winCURSORDIRENTRY;

typedef struct {
    M_uint8             chReserved[2];
    M_uint8             chType[2];
    M_uint16            chCount;
    winCURSORDIRENTRY   chEntries[1];
    } winCURSORHEADER;

/* Windows .BMP header and file structures */

typedef struct {
    M_uint8   bfType[2];
    M_uint32  bfSize;
    M_uint32  bfRes;
    M_uint32  bfOffBits;
    } winBITMAPFILEHEADER;

typedef struct {
    M_uint32  biSize;
    M_uint32  biWidth;
    M_uint32  biHeight;
    M_uint16  biPlanes;
    M_uint16  biBitCount;
    M_uint32  biCompression;
    M_uint32  biSizeImage;
    M_uint32  biXPelsPerMeter;
    M_uint32  biYPelsPerMeter;
    M_uint32  biClrUsed;
    M_uint32  biClrImportant;
    } winBITMAPINFOHEADER;

typedef struct {
    M_uint32  biSize;
    M_int32   biWidth;
    M_int32   biHeight;
    M_uint16  biPlanes;
    M_uint16  biBitCount;
    M_uint32  biCompression;
    M_uint32  biSizeImage;
    M_int32   biXPelsPerMeter;
    M_int32   biYPelsPerMeter;
    M_uint32  biClrUsed;
    M_uint32  biClrImportant;
    } winBITMAPINFOHEADER_full;

typedef struct {
    M_uint8   rgbBlue;
    M_uint8   rgbGreen;
    M_uint8   rgbRed;
    M_uint8   rgbReserved;
    } winRGBQUAD;

typedef struct {
    winBITMAPINFOHEADER_full    header;
    winRGBQUAD                  colors[256];
    } winBITMAPINFO;

typedef struct {
    winBITMAPINFOHEADER_full    header;
    M_uint32                    maskRed;
    M_uint32                    maskGreen;
    M_uint32                    maskBlue;
    } winBITMAPINFOEXT;

/* Constants for the biCompression field */

#define winBI_RGB       0L
#define winBI_RLE8      1L
#define winBI_RLE4      2L
#define winBI_BITFIELDS 3L

/* Windows .ICO header and file structures */

typedef struct {
    M_uint8   bWidth;
    M_uint8   bHeight;
    M_uint8   bColorCount;
    M_uint8   bRes;
    M_uint8   wRes1[2];
    M_uint8   wRes2[2];
    M_uint8   dwBytesInRes[4];
    M_uint8   dwImageOffset[4];
    } winICONDIRECTORY;

typedef struct {
    M_uint8             ihRes[2];
    M_uint8             ihType[2];
    M_uint8             ihCount[2];
    winICONDIRECTORY    ihEntries[1];
    } winICONHEADER;

/* Windows 2.x .FNT header and file structures */

typedef struct {
    M_uint8   dfVersion[2];
    M_uint8   dfSize[4];
    M_uint8   dfCopyright[60];
    M_uint8   dfType[2];
    } winFONTHEADER;

typedef struct {
    M_uint8   dfPoints[2];
    M_uint8   dfVertRes[2];
    M_uint8   dfHorizRes[2];
    M_uint8   dfAscent[2];
    M_uint8   dfInternalLeading[2];
    M_uint8   dfExternalLeading[2];
    } winFONTSPACING;

typedef struct {
    M_uint8   dfItalic;
    M_uint8   dfUnderline;
    M_uint8   dfStrikeOut;
    M_uint8   dfWeight[2];
    M_uint8   dfCharSet;
    M_uint8   dfPixWidth[2];
    M_uint8   dfPixHeight[2];
    M_uint8   dfPitchAndFamily;
    M_uint8   dfAvgWidth[2];
    M_uint8   dfMaxWidth[2];
    } winFONTSTYLE;

typedef struct {
    M_uint8   dfFirstChar;
    M_uint8   dfLastChar;
    M_uint8   dfDefaultChar;
    M_uint8   dfBreakChar;
    } winFONTCHAR;

typedef struct {
    M_uint8   dfWidthBytes[2];
    M_uint8   dfDevice[4];
    M_uint8   dfFace[4];
    M_uint8   dfBitsPointer[4];
    M_uint8   dfBitsOffset[4];
    M_uint8   dfRes1;
    /* Variable length font info resides in here */
    } winFONTMISC;

typedef struct {
    winFONTHEADER   fh;
    winFONTSPACING  fs;
    winFONTSTYLE    ft;
    winFONTCHAR     fc;
    winFONTMISC     fm;
    } winFONTINFO;

typedef struct {
    M_uint8   gWidth[2];
    M_uint8   gOffset[2];
    } winGLYPH;

/* Executable File Format Information */

typedef struct {
    M_uint8   exSignature[2];
    M_uint8   exExtraBytes[2];
    M_uint8   exPages[2];
    M_uint8   exRelocItems[2];
    M_uint8   exHeaderSize[2];
    M_uint8   exMinAlloc[2];
    M_uint8   exMaxAlloc[2];
    M_uint8   exInitSS[2];
    M_uint8   exInitSP[2];
    M_uint8   exCheckSum[2];
    M_uint8   exInitIP[2];
    M_uint8   exInitCS[2];
    M_uint8   exRelocTable[2];
    M_uint8   exOverlay[2];
    M_uint8   reserved1[4];
    M_uint8   reserved2[28];
    M_uint8   winInfoOffset[2];
    M_uint8   wreserved[2];
    M_uint8   msdosStub[1];
    } winOLDHEADER;

typedef struct {
    M_uint8           signature[2];
    M_uint8           linkerVersion;
    M_uint8           linkerRevision;
    M_uint8           entryTabOffset[2];
    M_uint8           entryTabLen[2];
    M_uint8           reserved1[4];
    M_uint8           exeFlags[2];
    M_uint8           dataSegNum[2];
    M_uint8           localHeapSize[2];
    M_uint8           stackSize[2];
    M_uint8           cs_ip[4];
    M_uint8           ss_sp[4];
    M_uint8           segTabEntries[2];
    M_uint8           modTabEntries[2];
    M_uint8           nonResTabSize[2];
    M_uint8           segTabOffset[2];
    M_uint8           resTabOffset[2];
    M_uint8           resNameTabOffset[2];
    M_uint8           modTabOffset[2];
    M_uint8           impTabOffset[2];
    M_uint8           nonResTabOffset[2];
    M_uint8           reserved2[2];
    M_uint8           numEntryPoints[2];
    M_uint8           numResourceSegs[2];
    M_uint8           targetOS;
    M_uint8           miscFlags;
    M_uint8           fastLoadOffset[2];
    M_uint8           fastLoadSize[2];
    M_uint8           reserved3[2];
    M_uint8           winRevision;
    M_uint8           winVersion;
    } winWININFO;

typedef struct {
    M_uint8           rnOffset[2];
    M_uint8           rnLength[2];
    M_uint8           rnFlags[2];
    M_uint8           rnID[2];
    M_uint8           reserved[4];
    } winNAMEINFO;

typedef struct {
    M_uint8           rtTypeID[2];
    M_uint8           rtResourceCount[2];
    M_uint8           rtReserved[4];
    /* Array of winNAMEINFO Structures */
    } winTYPEINFO;

/* MGL 1.x font file header structure */

typedef struct {
    char    ident[21];          /* Font file identification string      */
    char    name[_MGL_FNAMESIZE];/* Font's name                         */
    char    copyright[175];     /* Copyright notice etc                 */
    M_int8  majorversion;       /* Major version number                 */
    M_int8  minorversion;       /* Minor version number                 */
    } font_header;

/* Identification flag for font files */

#define FONT_IDENT  "MegaGraph Font File:"

#define _MAXVEC     256         /* Number of characters in font defn    */

/* MGL 1.x Vector Font structure */

typedef struct {
    char            name[_MGL_FNAMESIZE];/* Name of the font            */
    M_int16         fontType;       /* Type of font                     */
    M_int16         maxWidth;       /* Maximum character width          */
    M_int16         maxKern;        /* Maximum character kern           */
    M_int16         fontWidth;      /* Font width                       */
    M_int16         fontHeight;     /* Font height                      */
    M_int16         ascent;         /* Font ascent value                */
    M_int16         descent;        /* Font descent value               */
    M_int16         leading;        /* Font leading value               */
    M_uint8         width[_MAXVEC]; /* Character width table            */
    M_int16         offset[_MAXVEC];/* Offset table into vector defn.   */
    M_uint8         def[1];         /* Vector definitions for font      */
    } vec_font;

/* Missing symbol for vector fonts is index 0 */

#define VEC_MISSINGSYMBOL(f) 0

/* MGL 1.x bitmap font structure */

typedef struct {
    char            name[_MGL_FNAMESIZE];/* Name of the font            */
    M_int16         fontType;       /* Type of font                     */
    M_int16         maxWidth;       /* Maximum character width          */
    M_int16         maxKern;        /* Maximum character kern           */
    M_int16         fontWidth;      /* Font width                       */
    M_int16         fontHeight;     /* Font height                      */
    M_int16         ascent;         /* Font ascent value                */
    M_int16         descent;        /* Font descent value               */
    M_int16         leading;        /* Font leading value               */
    M_uint8         width[_MAXVEC]; /* Character width table            */
    M_int16         offset[_MAXVEC];/* Offset table into character defn.*/
    M_uint8         iwidth[_MAXVEC];/* Character image width table      */
    M_uint8         loc[_MAXVEC];   /* Character location table         */
    M_uint8         def[1];         /* Bitmap definitions for font      */
    } bitmap_font_old;

/* PCX file format header information */

typedef struct {
    M_int8    format;                     /* Always 10 for PCX        */
    M_int8    version;                    /* Version info             */
    M_int8    rleflag;                    /* Set to 1                 */
    M_int8    bitpx;                      /* Bits per pixel           */
    M_uint16  x1,y1,x2,y2;                /* Image bounds in pixels   */
    M_uint16  hres,vres;                  /* Image resolution in dpi  */
    M_uint8   colors[48];                 /* Palette                  */
    M_int8    vmode;                      /* (ignored)                */
    M_int8    nplanes;                    /* Plane count (v2.5=0)     */
    M_uint16  bplin;                      /* Bytes per scanline       */
    M_uint8   paltype;                    /* 1 for color, 2 for gray  */
    M_uint8   res1;                       /* Reserved (0)             */
    M_uint16  scrnw,scrnh;                /* Screen size in pixels    */
    M_uint8   extra[54];                  /* Pad to 128 bytes         */
    } PCXHEADER;

#define PCX_RUN_MASK    0xC0            /* Mask top 2 bits          */
#define PCX_CNT_MASK    0x3F            /* Mask out pixel count     */
#define PCX_MAX_RUN     0x3F            /* Maxium repeat count      */

/* Macros for extracting values from resource file structures in little
 * endian or big endian format. We define the following macros:
 *
 *  getLEShort()    - Get a short in little endian format
 *  getLELong()     - Get a long in little endian format
 *  getBEShort()    - Get a short in big endian format
 *  getBESHory()    - Get a long in big endian format
 *
 *  putLEShort()    - Put a short in little endian format
 *  putLELong()     - Put a long in little endian format
 *  putBEShort()    - Put a short in big endian format
 *  putBESHory()    - Put a long in big endian format
 *
 * Note that all of these macros expect the input argument to be an
 * array of characters, although the macros will work with the argument
 * as a native short or long.
 */

#define _TOB(v) ((M_uint8*)&(v))
#define _TOS(v) ((M_uint16*)&(v))
#define _TOL(v) ((M_uint32*)&(v))

#ifdef  __BIG_ENDIAN__
#define getBEShort(v)   (*_TOS(v))
#define getBELong(v)    (*_TOL(v))
#define getLEShort(v)   ((_TOB(v)[1] << 8) | _TOB(v)[0])
#define getLELong(v)    ((_TOB(v)[3] << 24) | (_TOB(v)[2] << 16) | (_TOB(v)[1] << 8) | _TOB(v)[0])
#define putLEShort(p,v) (_TOB(p)[0] = (v) & 0xFF),(_TOB(p)[1] = ((v) >> 8) & 0xFF)
#define putLELong(p,v)  (_TOB(p)[0] = (v) & 0xFF),(_TOB(p)[1] = ((v) >> 8) & 0xFF),(_TOB(p)[2] = ((v) >> 16) & 0xFF),(_TOB(p)[3] = ((v) >> 24) & 0xFF)
#define putBEShort(p,v) (*_TOS(p)) = (v)
#define putBELong(p,v)  (*_TOL(p)) = (v)
#else
#define getBEShort(v)   ((_TOS(v)[1] << 8) | _TOS(v)[0])
#define getBELong(v)    ((_TOL(v)[3] << 24) | (_TOL(v)[2] << 16) | (_TOL(v)[1] << 8) | _TOL(v)[0])
#define getLEShort(v)   (*_TOS(v))
#define getLELong(v)    (*_TOL(v))
#define putLEShort(p,v) (*_TOS(p)) = (v)
#define putLELong(p,v)  (*_TOL(p)) = (v)
#define putBEShort(p,v) (_TOB(p)[0] = (v) & 0xFF),(_TOB(p)[1] = ((v) >> 8) & 0xFF)
#define putBELong(p,v)  (_TOB(p)[0] = (v) & 0xFF),(_TOB(p)[1] = ((v) >> 8) & 0xFF),(_TOB(p)[2] = ((v) >> 16) & 0xFF),(_TOB(p)[3] = ((v) >> 24) & 0xFF)
#endif

/* Macros to convert short's and longs in place in a structure */

#define convLEShort(v)  (v = getLEShort(v))
#define convLELong(v)   (v = getLELong(v))

#pragma pack()              /* Return to default packing                */

#endif  /* __RESOURCE_H */
