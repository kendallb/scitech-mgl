/* $Header: /usr/people/sam/tiff/libtiff/RCS/tiffio.h,v 1.93 1996/03/07 17:00:14 sam Rel $ */

/*
 * Copyright (c) 1988-1996 Sam Leffler
 * Copyright (c) 1991-1996 Silicon Graphics, Inc.
 *
 * Permission to use, copy, modify, distribute, and sell this software and 
 * its documentation for any purpose is hereby granted without fee, provided
 * that (i) the above copyright notices and this permission notice appear in
 * all copies of the software and related documentation, and (ii) the names of
 * Sam Leffler and Silicon Graphics may not be used in any advertising or
 * publicity relating to the software without the specific, prior written
 * permission of Sam Leffler and Silicon Graphics.
 * 
 * THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND, 
 * EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY 
 * WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  
 * 
 * IN NO EVENT SHALL SAM LEFFLER OR SILICON GRAPHICS BE LIABLE FOR
 * ANY SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND,
 * OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER OR NOT ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF 
 * LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE 
 * OF THIS SOFTWARE.
 */

#ifndef _TIFFIO_
#define _TIFFIO_

/*
 * TIFF I/O Library Definitions.
 */
#include "tiff.h"

/* Compile all public functions with __cdecl calling conventions */

#include "pmapi.h"
#define TIFFAPI     _ASMAPI
#define TIFFAPIP    _ASMAPIP

/* Pack all structures to a 1 byte boundary for compatibility between
 * binary portable DLL's and the loading code.
 */

#pragma pack(1)

/*
 * This define can be used in code that requires
 * compilation-related definitions specific to a
 * version or versions of the library.  Runtime
 * version checking should be done based on the
 * string returned by TIFFGetVersion.
 */
#define TIFFLIB_VERSION 19960307    /* March 7, 1996 */

/*
 * TIFF is defined as an incomplete type to hide the
 * library's internal data structures from clients.
 */
typedef struct tiff TIFF;

/*
 * The following typedefs define the intrinsic size of
 * data types used in the *exported* interfaces.  These
 * definitions depend on the proper definition of types
 * in tiff.h.  Note also that the varargs interface used
 * pass tag types and values uses the types defined in
 * tiff.h directly.
 *
 * NB: ttag_t is unsigned int and not unsigned short because
 *     ANSI C requires that the type before the ellipsis be a
 *     promoted type (i.e. one of int, unsigned int, pointer,
 *     or double) and because we defined pseudo-tags that are
 *     outside the range of legal Aldus-assigned tags.
 * NB: tsize_t is int32 and not uint32 because some functions
 *     return -1.
 * NB: toff_t is not off_t for many reasons; TIFFs max out at
 *     32-bit file offsets being the most important
 */
typedef unsigned int ttag_t;    /* directory tag */
typedef uint16 tdir_t;      /* directory index */
typedef uint16 tsample_t;   /* sample number */
typedef uint32 tstrip_t;    /* strip number */
typedef uint32 ttile_t;     /* tile number */
typedef int32 tsize_t;      /* i/o size in bytes */
#if defined(_WINDOWS) || defined(__WIN32__) || defined(_Windows)
#include <windows.h>
#ifdef __WIN32__
DECLARE_HANDLE(thandle_t);  /* Win32 file handle */
#else
typedef HFILE thandle_t;    /* client data handle */
#endif
#else
typedef void* thandle_t;    /* client data handle */
#endif
typedef void* tdata_t;      /* image data ref */
typedef int32 toff_t;       /* file offset */

#ifndef NULL
#define NULL    0
#endif

/*
 * Flags to pass to TIFFPrintDirectory to control
 * printing of data structures that are potentially
 * very large.   Bit-or these flags to enable printing
 * multiple items.
 */
#define TIFFPRINT_NONE      0x0     /* no extra info */
#define TIFFPRINT_STRIPS    0x1     /* strips/tiles info */
#define TIFFPRINT_CURVES    0x2     /* color/gray response curves */
#define TIFFPRINT_COLORMAP  0x4     /* colormap */
#define TIFFPRINT_JPEGQTABLES   0x100       /* JPEG Q matrices */
#define TIFFPRINT_JPEGACTABLES  0x200       /* JPEG AC tables */
#define TIFFPRINT_JPEGDCTABLES  0x200       /* JPEG DC tables */

/*
 * RGBA-style image support.
 */
typedef unsigned char TIFFRGBValue;     /* 8-bit samples */
typedef struct _TIFFRGBAImage TIFFRGBAImage;
/*
 * The image reading and conversion routines invoke
 * ``put routines'' to copy/image/whatever tiles of
 * raw image data.  A default set of routines are 
 * provided to convert/copy raw image data to 8-bit
 * packed ABGR format rasters.  Applications can supply
 * alternate routines that unpack the data into a
 * different format or, for example, unpack the data
 * and draw the unpacked raster on the display.
 */
typedef void (TIFFAPIP tileContigRoutine)
    (TIFFRGBAImage*, uint32*, uint32, uint32, uint32, uint32, int32, int32,
    unsigned char*);
typedef void (TIFFAPIP tileSeparateRoutine)
    (TIFFRGBAImage*, uint32*, uint32, uint32, uint32, uint32, int32, int32,
    unsigned char*, unsigned char*, unsigned char*, unsigned char*);
/*
 * RGBA-reader state.
 */
typedef struct {                /* YCbCr->RGB support */
    TIFFRGBValue* clamptab;         /* range clamping table */
    int*    Cr_r_tab;
    int*    Cb_b_tab;
    int32*  Cr_g_tab;
    int32*  Cb_g_tab;
    float   coeffs[3];          /* cached for repeated use */
} TIFFYCbCrToRGB;

struct _TIFFRGBAImage {
    TIFF*   tif;                /* image handle */
    int stoponerr;          /* stop on read error */
    int isContig;           /* data is packed/separate */
    int alpha;              /* type of alpha data present */
    uint32  width;              /* image width */
    uint32  height;             /* image height */
    uint16  bitspersample;          /* image bits/sample */
    uint16  samplesperpixel;        /* image samples/pixel */
    uint16  orientation;            /* image orientation */
    uint16  photometric;            /* image photometric interp */
    uint16* redcmap;            /* colormap pallete */
    uint16* greencmap;
    uint16* bluecmap;
                        /* get image data routine */
    int (*get)(TIFFRGBAImage*, uint32*, uint32, uint32);
    union {
        void (*any)(TIFFRGBAImage*);
        tileContigRoutine   contig;
        tileSeparateRoutine separate;
    } put;                  /* put decoded strip/tile */
    TIFFRGBValue* Map;          /* sample mapping array */
    uint32** BWmap;             /* black&white map */
    uint32** PALmap;            /* palette image map */
    TIFFYCbCrToRGB* ycbcr;          /* YCbCr conversion state */
};

/*
 * Macros for extracting components from the
 * packed ABGR form returned by TIFFReadRGBAImage.
 */
#define TIFFGetR(abgr)  ((abgr) & 0xff)
#define TIFFGetG(abgr)  (((abgr) >> 8) & 0xff)
#define TIFFGetB(abgr)  (((abgr) >> 16) & 0xff)
#define TIFFGetA(abgr)  (((abgr) >> 24) & 0xff)

/*
 * A CODEC is a software package that implements decoding,
 * encoding, or decoding+encoding of a compression algorithm.
 * The library provides a collection of builtin codecs.
 * More codecs may be registered through calls to the library
 * and/or the builtin implementations may be overridden.
 */
typedef int (TIFFAPIP TIFFInitMethod)(TIFF*, int);
typedef struct {
    char*       name;
    uint16      scheme;
    TIFFInitMethod  init;
} TIFFCodec;

#include <stdio.h>
#include <stdarg.h>

#if defined(__cplusplus)
extern "C" {
#endif
typedef void        (TIFFAPIP TIFFErrorHandler)(const char*, const char*, va_list);
typedef tsize_t     (TIFFAPIP TIFFReadWriteProc)(thandle_t, tdata_t, tsize_t);
typedef toff_t      (TIFFAPIP TIFFSeekProc)(thandle_t, toff_t, int);
typedef int         (TIFFAPIP TIFFCloseProc)(thandle_t);
typedef toff_t      (TIFFAPIP TIFFSizeProc)(thandle_t);
typedef int         (TIFFAPIP TIFFMapFileProc)(thandle_t, tdata_t*, toff_t*);
typedef void        (TIFFAPIP TIFFUnmapFileProc)(thandle_t, tdata_t, toff_t);
typedef void        (TIFFAPIP TIFFExtendProc)(TIFF*);

const char*         TIFFAPI TIFFGetVersion(void);

const TIFFCodec*    TIFFAPI TIFFFindCODEC(uint16);
TIFFCodec*      TIFFAPI TIFFRegisterCODEC(uint16, const char*, TIFFInitMethod);
void            TIFFAPI TIFFUnRegisterCODEC(TIFFCodec*);

tdata_t         TIFFAPI _TIFFmalloc(tsize_t);
tdata_t         TIFFAPI _TIFFrealloc(tdata_t, tsize_t);
void            TIFFAPI _TIFFmemset(tdata_t, int, tsize_t);
void            TIFFAPI _TIFFmemcpy(tdata_t, const tdata_t, tsize_t);
int             TIFFAPI _TIFFmemcmp(const tdata_t, const tdata_t, tsize_t);
void            TIFFAPI _TIFFfree(tdata_t);

void            TIFFAPI TIFFClose(TIFF*);
int         TIFFAPI TIFFFlush(TIFF*);
int         TIFFAPI TIFFFlushData(TIFF*);
int         TIFFAPI TIFFGetField(TIFF*, ttag_t, ...);
int         TIFFAPI TIFFVGetField(TIFF*, ttag_t, va_list);
int         TIFFAPI TIFFGetFieldDefaulted(TIFF*, ttag_t, ...);
int         TIFFAPI TIFFVGetFieldDefaulted(TIFF*, ttag_t, va_list);
int         TIFFAPI TIFFReadDirectory(TIFF*);
tsize_t         TIFFAPI TIFFScanlineSize(TIFF*);
tsize_t         TIFFAPI TIFFRasterScanlineSize(TIFF*);
tsize_t         TIFFAPI TIFFStripSize(TIFF*);
tsize_t         TIFFAPI TIFFVStripSize(TIFF*, uint32);
tsize_t         TIFFAPI TIFFTileRowSize(TIFF*);
tsize_t         TIFFAPI TIFFTileSize(TIFF*);
tsize_t         TIFFAPI TIFFVTileSize(TIFF*, uint32);
uint32          TIFFAPI TIFFDefaultStripSize(TIFF*, uint32);
void            TIFFAPI TIFFDefaultTileSize(TIFF*, uint32*, uint32*);
int         TIFFAPI TIFFFileno(TIFF*);
int         TIFFAPI TIFFGetMode(TIFF*);
int         TIFFAPI TIFFIsTiled(TIFF*);
int         TIFFAPI TIFFIsByteSwapped(TIFF*);
int         TIFFAPI TIFFIsUpSampled(TIFF*);
int         TIFFAPI TIFFIsMSB2LSB(TIFF*);
uint32          TIFFAPI TIFFCurrentRow(TIFF*);
tdir_t          TIFFAPI TIFFCurrentDirectory(TIFF*);
uint32          TIFFAPI TIFFCurrentDirOffset(TIFF*);
tstrip_t        TIFFAPI TIFFCurrentStrip(TIFF*);
ttile_t         TIFFAPI TIFFCurrentTile(TIFF*);
int             TIFFAPI TIFFReadBufferSetup(TIFF*, tdata_t, tsize_t);
int             TIFFAPI TIFFWriteBufferSetup(TIFF*, tdata_t, tsize_t);
int             TIFFAPI TIFFLastDirectory(TIFF*);
int             TIFFAPI TIFFSetDirectory(TIFF*, tdir_t);
int             TIFFAPI TIFFSetSubDirectory(TIFF*, uint32);
int             TIFFAPI TIFFUnlinkDirectory(TIFF*, tdir_t);
int             TIFFAPI TIFFSetField(TIFF*, ttag_t, ...);
int             TIFFAPI TIFFVSetField(TIFF*, ttag_t, va_list);
int             TIFFAPI TIFFWriteDirectory(TIFF *);
#if defined(c_plusplus) || defined(__cplusplus)
void            TIFFAPI TIFFPrintDirectory(TIFF*, FILE*, long = 0);
int             TIFFAPI TIFFReadScanline(TIFF*, tdata_t, uint32, tsample_t = 0);
int             TIFFAPI TIFFWriteScanline(TIFF*, tdata_t, uint32, tsample_t = 0);
int             TIFFAPI TIFFReadRGBAImage(TIFF*, uint32, uint32, uint32*, int = 0);
#else
void            TIFFAPI TIFFPrintDirectory(TIFF*, FILE*, long);
int             TIFFAPI TIFFReadScanline(TIFF*, tdata_t, uint32, tsample_t);
int             TIFFAPI TIFFWriteScanline(TIFF*, tdata_t, uint32, tsample_t);
int             TIFFAPI TIFFReadRGBAImage(TIFF*, uint32, uint32, uint32*, int);
#endif
int             TIFFAPI TIFFRGBAImageOK(TIFF*, char [1024]);
int             TIFFAPI TIFFRGBAImageBegin(TIFFRGBAImage*, TIFF*, int, char [1024]);
int             TIFFAPI TIFFRGBAImageGet(TIFFRGBAImage*, uint32*, uint32, uint32);
void            TIFFAPI TIFFRGBAImageEnd(TIFFRGBAImage*);
TIFF*           TIFFAPI TIFFOpen(const char*, const char*);
TIFF*           TIFFAPI TIFFFdOpen(int, const char*, const char*);
TIFF*           TIFFAPI TIFFClientOpen(const char*, const char*,thandle_t,TIFFReadWriteProc, TIFFReadWriteProc,TIFFSeekProc, TIFFCloseProc,TIFFSizeProc,TIFFMapFileProc, TIFFUnmapFileProc);
const char*         TIFFAPI TIFFFileName(TIFF*);
void            TIFFAPI TIFFError(const char*, const char*, ...);
void            TIFFAPI TIFFWarning(const char*, const char*, ...);
TIFFErrorHandler    TIFFAPI TIFFSetErrorHandler(TIFFErrorHandler);
TIFFErrorHandler    TIFFAPI TIFFSetWarningHandler(TIFFErrorHandler);
TIFFExtendProc      TIFFAPI TIFFSetTagExtender(TIFFExtendProc);
ttile_t         TIFFAPI TIFFComputeTile(TIFF*, uint32, uint32, uint32, tsample_t);
int             TIFFAPI TIFFCheckTile(TIFF*, uint32, uint32, uint32, tsample_t);
ttile_t         TIFFAPI TIFFNumberOfTiles(TIFF*);
tsize_t         TIFFAPI TIFFReadTile(TIFF*,tdata_t, uint32, uint32, uint32, tsample_t);
tsize_t         TIFFAPI TIFFWriteTile(TIFF*,tdata_t, uint32, uint32, uint32, tsample_t);
tstrip_t        TIFFAPI TIFFComputeStrip(TIFF*, uint32, tsample_t);
tstrip_t        TIFFAPI TIFFNumberOfStrips(TIFF*);
tsize_t         TIFFAPI TIFFReadEncodedStrip(TIFF*, tstrip_t, tdata_t, tsize_t);
tsize_t         TIFFAPI TIFFReadRawStrip(TIFF*, tstrip_t, tdata_t, tsize_t);
tsize_t         TIFFAPI TIFFReadEncodedTile(TIFF*, ttile_t, tdata_t, tsize_t);
tsize_t         TIFFAPI TIFFReadRawTile(TIFF*, ttile_t, tdata_t, tsize_t);
tsize_t         TIFFAPI TIFFWriteEncodedStrip(TIFF*, tstrip_t, tdata_t, tsize_t);
tsize_t         TIFFAPI TIFFWriteRawStrip(TIFF*, tstrip_t, tdata_t, tsize_t);
tsize_t         TIFFAPI TIFFWriteEncodedTile(TIFF*, ttile_t, tdata_t, tsize_t);
tsize_t         TIFFAPI TIFFWriteRawTile(TIFF*, ttile_t, tdata_t, tsize_t);
void            TIFFAPI TIFFSetWriteOffset(TIFF*, toff_t);
void            TIFFAPI TIFFSwabShort(uint16*);
void            TIFFAPI TIFFSwabLong(uint32*);
void            TIFFAPI TIFFSwabDouble(double*);
void            TIFFAPI TIFFSwabArrayOfShort(uint16*, unsigned long);
void            TIFFAPI TIFFSwabArrayOfLong(uint32*, unsigned long);
void            TIFFAPI TIFFSwabArrayOfDouble(double*, unsigned long);
void            TIFFAPI TIFFReverseBits(unsigned char *, unsigned long);
const unsigned char*    TIFFAPI TIFFGetBitRevTable(int);
#if defined(__cplusplus)
}
#endif

/* Imports to SciTech MGL Binary Portable DLL's */

typedef struct {
    ulong   dwSize;
    size_t  (PMAPIP MGL_fread)(void *ptr,size_t size,size_t n,FILE *f);
    size_t  (PMAPIP MGL_fwrite)(const void *ptr,size_t size,size_t n,FILE *f);
    } TIFF_imports;

#ifdef  __DRIVER__
extern TIFF_imports _VARAPI _TIFF_imports;
#define MGL_fread(ptr,size,n,f)     _TIFF_imports.MGL_fread(ptr,size,n,f)
#define MGL_fwrite(ptr,size,n,f)    _TIFF_imports.MGL_fwrite(ptr,size,n,f)
#else
size_t  PMAPI MGL_fread(void *ptr,size_t size,size_t n,FILE *f);
size_t  PMAPI MGL_fwrite(const void *ptr,size_t size,size_t n,FILE *f);
#endif

/* Exports from the SciTech MGL Binary Portable DLL */

typedef struct {
    ulong           dwSize;
    const char*         (TIFFAPIP TIFFGetVersion)(void);
    const TIFFCodec*    (TIFFAPIP TIFFFindCODEC)(uint16);
    TIFFCodec*      (TIFFAPIP TIFFRegisterCODEC)(uint16, const char*, TIFFInitMethod);
    void            (TIFFAPIP TIFFUnRegisterCODEC)(TIFFCodec*);
    void            (TIFFAPIP TIFFClose)(TIFF*);
    int         (TIFFAPIP TIFFFlush)(TIFF*);
    int         (TIFFAPIP TIFFFlushData)(TIFF*);
    int         (TIFFAPIP TIFFGetField)(TIFF*, ttag_t, ...);
    int         (TIFFAPIP TIFFVGetField)(TIFF*, ttag_t, va_list);
    int         (TIFFAPIP TIFFGetFieldDefaulted)(TIFF*, ttag_t, ...);
    int         (TIFFAPIP TIFFVGetFieldDefaulted)(TIFF*, ttag_t, va_list);
    int         (TIFFAPIP TIFFReadDirectory)(TIFF*);
    tsize_t         (TIFFAPIP TIFFScanlineSize)(TIFF*);
    tsize_t         (TIFFAPIP TIFFRasterScanlineSize)(TIFF*);
    tsize_t         (TIFFAPIP TIFFStripSize)(TIFF*);
    tsize_t         (TIFFAPIP TIFFVStripSize)(TIFF*, uint32);
    tsize_t         (TIFFAPIP TIFFTileRowSize)(TIFF*);
    tsize_t         (TIFFAPIP TIFFTileSize)(TIFF*);
    tsize_t         (TIFFAPIP TIFFVTileSize)(TIFF*, uint32);
    uint32          (TIFFAPIP TIFFDefaultStripSize)(TIFF*, uint32);
    void            (TIFFAPIP TIFFDefaultTileSize)(TIFF*, uint32*, uint32*);
    int         (TIFFAPIP TIFFFileno)(TIFF*);
    int         (TIFFAPIP TIFFGetMode)(TIFF*);
    int         (TIFFAPIP TIFFIsTiled)(TIFF*);
    int         (TIFFAPIP TIFFIsByteSwapped)(TIFF*);
    int         (TIFFAPIP TIFFIsUpSampled)(TIFF*);
    int         (TIFFAPIP TIFFIsMSB2LSB)(TIFF*);
    uint32          (TIFFAPIP TIFFCurrentRow)(TIFF*);
    tdir_t          (TIFFAPIP TIFFCurrentDirectory)(TIFF*);
    uint32          (TIFFAPIP TIFFCurrentDirOffset)(TIFF*);
    tstrip_t        (TIFFAPIP TIFFCurrentStrip)(TIFF*);
    ttile_t         (TIFFAPIP TIFFCurrentTile)(TIFF*);
    int             (TIFFAPIP TIFFReadBufferSetup)(TIFF*, tdata_t, tsize_t);
    int             (TIFFAPIP TIFFWriteBufferSetup)(TIFF*, tdata_t, tsize_t);
    int             (TIFFAPIP TIFFLastDirectory)(TIFF*);
    int             (TIFFAPIP TIFFSetDirectory)(TIFF*, tdir_t);
    int             (TIFFAPIP TIFFSetSubDirectory)(TIFF*, uint32);
    int             (TIFFAPIP TIFFUnlinkDirectory)(TIFF*, tdir_t);
    int             (TIFFAPIP TIFFSetField)(TIFF*, ttag_t, ...);
    int             (TIFFAPIP TIFFVSetField)(TIFF*, ttag_t, va_list);
    int             (TIFFAPIP TIFFWriteDirectory)(TIFF *);
    void            (TIFFAPIP TIFFPrintDirectory)(TIFF*, FILE*, long);
    int             (TIFFAPIP TIFFReadScanline)(TIFF*, tdata_t, uint32, tsample_t);
    int             (TIFFAPIP TIFFWriteScanline)(TIFF*, tdata_t, uint32, tsample_t);
    int             (TIFFAPIP TIFFReadRGBAImage)(TIFF*, uint32, uint32, uint32*, int);
    int             (TIFFAPIP TIFFRGBAImageOK)(TIFF*, char [1024]);
    int             (TIFFAPIP TIFFRGBAImageBegin)(TIFFRGBAImage*, TIFF*, int, char [1024]);
    int             (TIFFAPIP TIFFRGBAImageGet)(TIFFRGBAImage*, uint32*, uint32, uint32);
    void            (TIFFAPIP TIFFRGBAImageEnd)(TIFFRGBAImage*);
    TIFF*           (TIFFAPIP TIFFOpen)(const char*, const char*);
    TIFF*           (TIFFAPIP TIFFFdOpen)(int, const char*, const char*);
    TIFF*           (TIFFAPIP TIFFClientOpen)(const char*, const char*,thandle_t,TIFFReadWriteProc, TIFFReadWriteProc,TIFFSeekProc, TIFFCloseProc,TIFFSizeProc,TIFFMapFileProc, TIFFUnmapFileProc);
    const char*         (TIFFAPIP TIFFFileName)(TIFF*);
    void            (TIFFAPIP TIFFError)(const char*, const char*, ...);
    void            (TIFFAPIP TIFFWarning)(const char*, const char*, ...);
    TIFFErrorHandler    (TIFFAPIP TIFFSetErrorHandler)(TIFFErrorHandler);
    TIFFErrorHandler    (TIFFAPIP TIFFSetWarningHandler)(TIFFErrorHandler);
    TIFFExtendProc      (TIFFAPIP TIFFSetTagExtender)(TIFFExtendProc);
    ttile_t         (TIFFAPIP TIFFComputeTile)(TIFF*, uint32, uint32, uint32, tsample_t);
    int             (TIFFAPIP TIFFCheckTile)(TIFF*, uint32, uint32, uint32, tsample_t);
    ttile_t         (TIFFAPIP TIFFNumberOfTiles)(TIFF*);
    tsize_t         (TIFFAPIP TIFFReadTile)(TIFF*,tdata_t, uint32, uint32, uint32, tsample_t);
    tsize_t         (TIFFAPIP TIFFWriteTile)(TIFF*,tdata_t, uint32, uint32, uint32, tsample_t);
    tstrip_t        (TIFFAPIP TIFFComputeStrip)(TIFF*, uint32, tsample_t);
    tstrip_t        (TIFFAPIP TIFFNumberOfStrips)(TIFF*);
    tsize_t         (TIFFAPIP TIFFReadEncodedStrip)(TIFF*, tstrip_t, tdata_t, tsize_t);
    tsize_t         (TIFFAPIP TIFFReadRawStrip)(TIFF*, tstrip_t, tdata_t, tsize_t);
    tsize_t         (TIFFAPIP TIFFReadEncodedTile)(TIFF*, ttile_t, tdata_t, tsize_t);
    tsize_t         (TIFFAPIP TIFFReadRawTile)(TIFF*, ttile_t, tdata_t, tsize_t);
    tsize_t         (TIFFAPIP TIFFWriteEncodedStrip)(TIFF*, tstrip_t, tdata_t, tsize_t);
    tsize_t         (TIFFAPIP TIFFWriteRawStrip)(TIFF*, tstrip_t, tdata_t, tsize_t);
    tsize_t         (TIFFAPIP TIFFWriteEncodedTile)(TIFF*, ttile_t, tdata_t, tsize_t);
    tsize_t         (TIFFAPIP TIFFWriteRawTile)(TIFF*, ttile_t, tdata_t, tsize_t);
    void            (TIFFAPIP TIFFSetWriteOffset)(TIFF*, toff_t);
    void            (TIFFAPIP TIFFSwabShort)(uint16*);
    void            (TIFFAPIP TIFFSwabLong)(uint32*);
    void            (TIFFAPIP TIFFSwabDouble)(double*);
    void            (TIFFAPIP TIFFSwabArrayOfShort)(uint16*, unsigned long);
    void            (TIFFAPIP TIFFSwabArrayOfLong)(uint32*, unsigned long);
    void            (TIFFAPIP TIFFSwabArrayOfDouble)(double*, unsigned long);
    void            (TIFFAPIP TIFFReverseBits)(unsigned char *, unsigned long);
    const unsigned char*    (TIFFAPIP TIFFGetBitRevTable)(int);
    } TIFF_exports;

typedef TIFF_exports * (PMAPIP TIFF_initLibrary_t)(PM_imports *pmImp,TIFF_imports *jpegImp);

/* Return structure packing to compiler default */

#pragma pack()

#endif /* _TIFFIO_ */
