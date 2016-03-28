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
* Description:  Common header file for Graphics Accelerator routines used
*               by all SciTech SNAP Graphics accelerated device drivers.
*
****************************************************************************/

#ifndef __DRIVERS_COMMON_GSNAP_H
#define __DRIVERS_COMMON_GSNAP_H

#include "drivers/common/common.h"

/*---------------------- Macro and type definitions -----------------------*/

#pragma pack(1)             /* Pack structures to byte granularity      */

/* The following defines a structure used to cache all necessary mode
 * information after the initial detection for a device, so that
 * it can be re-used efficiently to determine the capabilities of all
 * available display modes for all available display device drivers. The
 * information in here is a subset of that returnd in the GA_modeInfo
 * structure to save space.
 */

typedef struct {
    N_int16         mode;
    N_int16         XResolution;
    N_int16         YResolution;
    N_uint8         BitsPerPixel;
    N_uint8         HaveRefresh;
    N_uint32        Attributes;
    N_int16         BytesPerScanLine;
    GA_pixelFormat  PixelFormat;
    N_int16         MaxBuffers;
    N_uint32        BitmapStartAlign;
    N_uint32        BitmapStridePad;
    } GA_mdEntry;

/* The following defines the data structure used to maintain the
 * information about the display device after the SciTech SNAP Graphics detection code
 * has been run.
 */

typedef struct {
    short           detected;           /* Driver already detected?         */
    int             saveMode;           /* Save mode for safe mode          */
    char            saveMemName[PM_MAX_PATH]; /* Filename used for safe mode  */
    ibool           accelerated;        /* True if accelerated              */
    GA_mdEntry      modeList[MAX_DRIVER_MODES+1];/* List of available modes */
    GA_initFuncs    init;               /* SNAP Graphics init functions     */
    GA_driverFuncs  drv;                /* SNAP Graphics driver functions   */
    GA_cursorFuncs  cursor;             /* SNAP Graphics cursor functions   */
    GA_videoFuncs   video;              /* SNAP Graphics video functions    */
    GA_bufferFuncs  buf;                /* SNAP Graphics buffer functions   */
    } SNAP_detectInfo;

#define SVGA(dc)    (((SNAP_data*)&(dc)->v->d)->svga)

/* The following defines the internal VGA device driver state buffer. A
 * copy of this is allocated for us in the device context created for
 * the device driver. This state buffer is used to maintain device specific
 * state information that is device context specific.
 */

typedef struct {
    display_vec v;              /* Display device vectors and info      */
    N_uint32    oldMode;        /* Old display mode before graphics     */
    } SNAP_state;

/* The following defines the data structures stored for each SciTech SNAP Graphics
 * driver per display device instance data.
 */

typedef struct {
    SNAP_state      state;
    SNAP_detectInfo svga;
    } SNAP_data;

#pragma pack()

/*------------------------- Function Prototypes ---------------------------*/

void *  MGLAPI SNAP_initInstance(void *inst);
ibool   MGLAPI SNAP_load(SNAP_data *inst);
ibool   MGLAPI SNAP_unload(SNAP_data *inst);
ibool   MGLAPI SNAP_hardwareDetect(SNAP_data *instance,ibool accelerated,int id,int *numModes,modetab availableModes);
ibool   MGLAPI SNAP_setMode(MGLDC *dc,N_uint32 mode,N_uint32 *oldMode,N_int32 virtualX,N_int32 virtualY,N_int32 numBuffers,ibool stereo,N_int32 refreshRate,ibool useLinearBlits);
ibool   MGLAPI SNAP_initHardware(SNAP_data *instance,MGLDC *dc,modeent *mode,ulong hwnd,int virtualX,int virtualY,int numBuffers,ibool stereo,int refreshRate,ibool useLinearBlits);
void    MGLAPI SNAP_restoreTextMode(MGLDC *dc);
ibool   MGLAPI SNAP_restoreGraphMode(MGLDC *dc);
void    MGLAPI SNAP_setActivePage(MGLDC *dc,int page);
void    MGLAPI SNAP_setVisualPage(MGLDC *dc,int page,int waitVRT);
void    MGLAPI SNAP_vSync(MGLDC *dc);
int     MGLAPI SNAP_isVSync(MGLDC *dc);
void    MGLAPI SNAP_setDisplayStart(MGLDC *dc,int x,int y,int waitFlag);
void    MGLAPI SNAP_startStereo(MGLDC *dc);
void    MGLAPI SNAP_stopStereo(MGLDC *dc);
int     MGLAPI SNAP_getCurrentScanLine(MGLDC *dc);
void    MGLAPI SNAP_setCursor(MGLDC *dc,cursor_t *curs);
void    MGLAPI SNAP_setCursorPos(MGLDC *dc,int x,int y);
void    MGLAPI SNAP_showCursor(MGLDC *dc,ibool visible);
void    MGLAPI SNAP_setCursorColor(MGLDC *dc,color_t foreColor,color_t backColor);
void    MGLAPI SNAP_makeCurrent(MGLDC *dc,MGLDC *oldDC,ibool partial);
void    MGLAPI SNAP_setGammaRamp(MGLDC *dc,palette_ext_t *pal,int num,int index,int waitVRT);
void    MGLAPI SNAP_getGammaRamp(MGLDC *dc,palette_ext_t *pal,int num,int index);
void    MGLAPI SNAP_realizePalette(MGLDC *dc,palette_t *pal,int num,int index,int waitVRT);
void    MGLAPI SNAP_realizePaletteExt(MGLDC *dc,palette_ext_t *pal,int num,int index,int waitVRT);
void    MGLAPI SNAP_beginVisibleClipRegion(MGLDC *dc);
void    MGLAPI SNAP_endVisibleClipRegion(MGLDC *dc);

/* Hardware device driver support routines */

void    MGLAPI SNAP_BeginDirectAccess(MGLDC *dc);
void    MGLAPI SNAP_BeginDirectAccessSLOW(MGLDC *dc);
void    MGLAPI SNAP_EndDirectAccessSLOW(MGLDC *dc);
void    MGLAPI SNAP_EndDirectAccessExt(MGLDC *dc,int left,int top,int right,int bottom);

#endif  /* __DRIVERS_COMMON_GSNAP_H */

