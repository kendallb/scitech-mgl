/****************************************************************************
*
*           The SuperVGA Kit - UniVBE Software Development Kit
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
* Environment:  IBM PC (MSDOS) Real Mode and 16/32 bit Protected Mode.
*
* Description:  Header file declaring the structures used to communicate
*               with a VESA VBE 1.x, 2.x or 3.x compliant BIOS.
*
*               Note that this module does automatic translation of all
*               information in the VBE_vgaInfo and VBE_modeInfo blocks so
*               that all pointers are converted to the native format,
*               insulating the application from dealing with a real
*               mode VBE 2.0+ implementation.
*
*               MUST be compiled in the LARGE or FLAT models.
*
****************************************************************************/

#ifndef __VESAVBE_H
#define __VESAVBE_H

#include "pmapi.h"

/*---------------------- Macros and type definitions ----------------------*/

#pragma pack(1)

/* Define the calling conventions for the code in this module */

#define VBEAPI  _ASMAPI         /* 'C' calling conventions always       */
#define VBEAPIP _ASMAPIP

/* SuperVGA information block */

typedef struct {
    s16     VESAVersion;            /* VBE version number               */
    char    *OemStringPtr;          /* Pointer to OEM string            */
    u32     Capabilities;           /* Capabilities of video card       */
    u16     *VideoModePtr;          /* Pointer to supported modes       */
    s16     TotalMemory;            /* Number of 64kb memory blocks     */

    /* VBE 2.0 extension information */

    s16     OemSoftwareRev;         /* OEM Software revision number     */
    char    *OemVendorNamePtr;      /* Pointer to Vendor Name string    */
    char    *OemProductNamePtr;     /* Pointer to Product Name string   */
    char    *OemProductRevPtr;      /* Pointer to Product Revision str  */
    } VBE_vgaInfo;
    
/* Internal version we are returned from the 16-bit BIOS, that may 
 * need transation for endian or pointer sizes.
 */     

typedef struct {
    char    VESASignature[4];       /* 'VESA' 4 byte signature          */
    s16     VESAVersion;            /* VBE version number               */
    u32     OemStringPtr;           /* Pointer to OEM string            */
    u32     Capabilities;           /* Capabilities of video card       */
    u32     VideoModePtr;           /* Pointer to supported modes       */
    s16     TotalMemory;            /* Number of 64kb memory blocks     */

    /* VBE 2.0 extension information */

    s16     OemSoftwareRev;         /* OEM Software revision number     */
    u32     OemVendorNamePtr;       /* Pointer to Vendor Name string    */
    u32     OemProductNamePtr;      /* Pointer to Product Name string   */
    u32     OemProductRevPtr;       /* Pointer to Product Revision str  */
    char    reserved[222];          /* Pad to 256 byte block size       */
    char    OemDATA[256];           /* Scratch pad for OEM data         */
    } _VBE_vgaInfoInt;

/* Flags for the Capabilities field */

#define vbe8BitDAC          0x0001  /* DAC width is switchable to 8 bit */
#define vbeNonVGA           0x0002  /* Controller is non-VGA            */
#define vbeBlankRAMDAC      0x0004  /* Programmed DAC with blank bit    */
#define vbeHWStereoSync     0x0008  /* Hardware stereo signalling       */
#define vbeEVCStereoSync    0x0010  /* HW stereo sync via EVC connector */

/* SuperVGA mode information block */

typedef struct {
    s16     ModeAttributes;         /* Mode attributes                  */
    s8      WinAAttributes;         /* Window A attributes              */
    s8      WinBAttributes;         /* Window B attributes              */
    s16     WinGranularity;         /* Window granularity in k          */
    s16     WinSize;                /* Window size in k                 */
    u16     WinASegment;            /* Window A segment                 */
    u16     WinBSegment;            /* Window B segment                 */
    u32     WinFuncPtr;             /* Pointer to window function       */
    s16     BytesPerScanLine;       /* Bytes per scanline               */
    s16     XResolution;            /* Horizontal resolution            */
    s16     YResolution;            /* Vertical resolution              */
    s8      XCharSize;              /* Character cell width             */
    s8      YCharSize;              /* Character cell height            */
    s8      NumberOfPlanes;         /* Number of memory planes          */
    s8      BitsPerPixel;           /* Bits per pixel                   */
    s8      NumberOfBanks;          /* Number of CGA style banks        */
    s8      MemoryModel;            /* Memory model type                */
    s8      BankSize;               /* Size of CGA style banks          */
    s8      NumberOfImagePages;     /* Number of images pages (Max)     */
    s8      res1;                   /* Reserved                         */
    s8      RedMaskSize;            /* Size of direct color red mask    */
    s8      RedFieldPosition;       /* Bit posn of lsb of red mask      */
    s8      GreenMaskSize;          /* Size of direct color green mask  */
    s8      GreenFieldPosition;     /* Bit posn of lsb of green mask    */
    s8      BlueMaskSize;           /* Size of direct color blue mask   */
    s8      BlueFieldPosition;      /* Bit posn of lsb of blue mask     */
    s8      RsvdMaskSize;           /* Size of direct color res mask    */
    s8      RsvdFieldPosition;      /* Bit posn of lsb of res mask      */
    s8      DirectColorModeInfo;    /* Direct color mode attributes     */

    /* VBE 2.0 extension information */
    u32     PhysBasePtr;            /* Physical address for linear buf  */
    u32     res2;                   /* Reserved                         */
    s16     res3;                   /* Reserved                         */

    /* VBE 3.0 extension information */
    s16     LinBytesPerScanLine;    /* Bytes per scanline               */
    s8      BnkNumberOfImagePages;  /* Number of images pages (banked)  */
    s8      LinNumberOfImagePages;  /* Number of images pages (linear)  */
    s8      LinRedMaskSize;         /* Size of direct color red mask    */
    s8      LinRedFieldPosition;    /* Bit posn of lsb of red mask      */
    s8      LinGreenMaskSize;       /* Size of direct color green mask  */
    s8      LinGreenFieldPosition;  /* Bit posn of lsb of green mask    */
    s8      LinBlueMaskSize;        /* Size of direct color blue mask   */
    s8      LinBlueFieldPosition;   /* Bit posn of lsb of blue mask     */
    s8      LinRsvdMaskSize;        /* Size of direct color res mask    */
    s8      LinRsvdFieldPosition;   /* Bit posn of lsb of res mask      */
    u32     MaxPixelClock;          /* Maximum pixel clock              */

    u8      res4[190];              /* Pad to 256 byte block size       */
    } VBE_modeInfo;

/* CRTC information block for refresh rate control */

typedef struct {
    u16     HorizontalTotal;        /* Horizontal total (pixels)        */
    u16     HorizontalSyncStart;    /* Horizontal sync start position   */
    u16     HorizontalSyncEnd;      /* Horizontal sync end position     */
    u16     VerticalTotal;          /* Vertical Total (lines)           */
    u16     VerticalSyncStart;      /* Vertical sync start position     */
    u16     VerticalSyncEnd;        /* Vertical sync end position       */
    u8      Flags;                  /* Initialisation flags for mode    */
    u32     PixelClock;             /* Pixel clock in units of Hz       */
    u16     RefreshRate;            /* Refresh rate in units of 0.01 Hz */
    } VBE_CRTCInfo;

/* Definitions for CRTC information block flags */

#define vbeDoubleScan       0x0001  /* Enable double scanned mode       */
#define vbeInterlaced       0x0002  /* Enable interlaced mode           */
#define vbeHSyncNeg         0x0004  /* Horizontal sync is negative      */
#define vbeVSyncNeg         0x0008  /* Vertical sync is negative        */

/* 32 bit protected mode info block */

#ifdef __INTEL__
typedef struct {
    short   setWindow;              /* Offset of Set Window call        */
    short   setDisplayStart;        /* Offset of Set Display Start call */
    short   setPalette;             /* Offset of Set Primary Palette    */
    short   IOPrivInfo;             /* Offset of I/O priveledge info    */

    /* UniVBE extensions */
    ulong   extensionSig;           /* Identify extension (0xFBADFBAD)  */
    long    setWindowLen;           /* Used to virtualise framebuffer   */
    long    setDisplayStartLen;
    long    setPaletteLen;

    /* ... code and tables located in here */
    } VBE_pmInfo;
#endif    

#define VBE20_EXT_SIG               0xFBADFBADUL

typedef enum {
    vbeMemTXT      = 0,             /* Text mode memory model           */
    vbeMemCGA      = 1,             /* CGA style mode                   */
    vbeMemHGC      = 2,             /* Hercules graphics style mode     */
    vbeMemPL       = 3,             /* 16 color VGA style planar mode   */
    vbeMemPK       = 4,             /* Packed pixel mode                */
    vbeMemX        = 5,             /* Non-chain 4, 256 color (ModeX)   */
    vbeMemRGB      = 6,             /* Direct color RGB mode            */
    vbeMemYUV      = 7              /* Direct color YUV mode            */
    } VBE_memModels;

/* Flags for combining with video modes during mode set */

#define vbeDontClear    0x8000      /* Dont clear display memory        */
#define vbeLinearBuffer 0x4000      /* Enable linear framebuffer mode   */
#define vbeRefreshCtrl  0x0800      /* Use refresh rate control         */
#define vbeModeMask     0x03FF      /* Mask for VBE mode numbers        */

/* Flags for the mode attributes returned by VBE_getModeInfo. If
 * vbeMdNonBanked is set to 1 and vbeMdLinear is also set to 1, then only
 * the linear framebuffer mode is available.
 */

#define vbeMdAvailable  0x0001      /* Video mode is available          */
#define vbeMdTTYOutput  0x0004      /* TTY BIOS output is supported     */
#define vbeMdColorMode  0x0008      /* Mode is a color video mode       */
#define vbeMdGraphMode  0x0010      /* Mode is a graphics mode          */
#define vbeMdNonVGA     0x0020      /* Mode is not VGA compatible       */
#define vbeMdNonBanked  0x0040      /* Banked mode is not supported     */
#define vbeMdLinear     0x0080      /* Linear mode supported            */
#define vbeMdDoubleScan 0x0100      /* Mode is double scanned           */
#define vbeMdInterlaced 0x0200      /* Supports interlaced timings      */
#define vbeMdTripleBuf  0x0400      /* Mode supports HW triple buffer   */
#define vbeMdStereo     0x0800      /* Mode supports hardware stereo    */
#define vbeMdDualDisp   0x1000      /* Mode supports dual display start */

/* Flags for save/restore state calls */

#define vbeStHardware   0x0001      /* Save the hardware state          */
#define vbeStBIOS       0x0002      /* Save the BIOS state              */
#define vbeStDAC        0x0004      /* Save the DAC state               */
#define vbeStSVGA       0x0008      /* Save the SuperVGA state          */
#define vbeStAll        0x000F      /* Save all states                  */

/* Palette entry structure, either 6 or 8 bit format. This is the VBE 2.0
 * palette format and the same as used by Windows (which is NOT the same
 * as the normal VGA BIOS palette format structure).
 */

typedef struct {
    u8      blue;                   /* Blue component of color          */
    u8      green;                  /* Green component of color         */
    u8      red;                    /* Blue component of color          */
    u8      alpha;                  /* Alpha or alignment byte          */
    } VBE_palette;

/* Structure containing device state information in the VESAVBE module */

typedef struct {
    uchar *     VESABuf_ptr;        /* Pointer to VESABuf               */
    uint        VESABuf_rseg;       /* Real mode segment of VESABuf     */
    uint        VESABuf_roff;       /* Real mode offset of VESABuf      */
    int         VBEVersion;         /* VBE version detected             */
    int         VBEMemory;          /* Amount of memory on board        */
#ifdef __INTEL__
    short       MMIOSel;            /* Selector to MMIO registers       */
    short       code32Sel;          /* Selector for 32-bit code segment */
    VBE_pmInfo  *pmInfo;            /* Global PM code block             */
    VBE_pmInfo  *pmInfo32;
#endif    
    } VBE_state;

#pragma pack()

/*-------------------------- Function Prototypes --------------------------*/

#ifdef  __cplusplus
extern "C" {                        /* Use "C" linkage when in C++ mode */
#endif

/* VBE_init allocates a real mode memory block for use by the internal VESA
 * VBE functions when calling real mode code. VBE_getRMBuf is used by other
 * libraries and modules to gain access to the same real mode memory block
 * so that we only require one real mode memory block per application.
 */

void    VBEAPI VBE_init(void);
void *  VBEAPI VBE_getRMBuf(uint *len,uint *rseg,uint *roff);
void    VBEAPI VBE_setStateBuffer(VBE_state *state);

/* Main VBE API routines */

void    VBEAPI VBE_callESDI(RMREGS *regs, void *buffer, int size);
int     VBEAPI VBE_detect(VBE_vgaInfo *vgaInfo);
ibool   VBEAPI VBE_getModeInfo(int mode,VBE_modeInfo *modeInfo);
long    VBEAPI VBE_getPageSize(VBE_modeInfo *modeInfo);
ibool   VBEAPI VBE_setVideoMode(int mode);
int     VBEAPI VBE_getVideoMode(void);
ibool   VBEAPI VBE_setBank(int window,int bank);
int     VBEAPI VBE_getBank(int window);
ibool   VBEAPI VBE_setPixelsPerLine(int pixelsPerLine,int *newBytes,int *newPixels,int *maxScanlines);
ibool   VBEAPI VBE_setBytesPerLine(int bytesPerLine,int *newBytes,int *newPixels,int *maxScanlines);
ibool   VBEAPI VBE_getScanlineLength(int *bytesPerLine,int *pixelsPerLine,int *maxScanlines);
ibool   VBEAPI VBE_getMaxScanlineLength(int *maxBytes,int *maxPixels);
ibool   VBEAPI VBE_setDisplayStart(int x,int y,ibool waitVRT);
ibool   VBEAPI VBE_getDisplayStart(int *x,int *y);
ibool   VBEAPI VBE_setDACWidth(int width);
int     VBEAPI VBE_getDACWidth(void);
ibool   VBEAPI VBE_setPalette(int start,int num,VBE_palette *pal,ibool waitVRT);
void *  VBEAPI VBE_getBankedPointer(VBE_modeInfo *modeInfo);

/* Functions to return the addresses of the VBE 2.0 32 bit code */

void *  VBEAPI VBE_getLinearPointer(VBE_modeInfo *modeInfo);
void *  VBEAPI VBE_getSetBank(void);
void *  VBEAPI VBE_getSetDisplayStart(void);
void *  VBEAPI VBE_getSetPalette(void);
void    VBEAPI VBE_freePMCode(void);
ibool   VBEAPI VBE_getBankFunc32(int *codeLen,void **bankFunc,int dualBanks,int bankAdjust);
void    VBEAPI VBE_sharePMCode(void);
void    VBEAPI VBE_mapPMCode(void);

/* New VBE/Core 3.0 functions */

ibool   VBEAPI VBE_setVideoModeExt(int mode,VBE_CRTCInfo *crtc);
ibool   VBEAPI VBE_setDisplayStartAlt(ulong startAddr,ibool waitVRT);
int     VBEAPI VBE_getDisplayStartStatus(void);
ibool   VBEAPI VBE_enableStereoMode(void);
ibool   VBEAPI VBE_disableStereoMode(void);
ibool   VBEAPI VBE_setStereoDisplayStart(ulong leftAddr,ulong rightAddr,ibool waitVRT);
ulong   VBEAPI VBE_getClosestClock(ushort mode,ulong pixelClock);

#ifdef  __cplusplus
}                                   /* End of "C" linkage for C++       */
#endif

#endif  /* __VESAVBE_H */
