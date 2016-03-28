/****************************************************************************
*
*                     SciTech SNAP Graphics Architecture
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
* Description:  Module to implement the code to test the SNAP Graphics
*               Architecture.
*
****************************************************************************/

#ifndef __GATEST_H
#define __GATEST_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "snap/gasdk.h"
#include "snap/gl3d.h"
#include "ztimer.h"
#include "event.h"
#include "gconsole.h"
#ifdef __UNIX__
#include <unistd.h>
#else
#include <io.h>
#ifndef R_OK  // stupid VAC doesn't define this
#define R_OK 04
#endif
#endif

/*---------------------------- Global Variables ---------------------------*/

#ifdef  __cplusplus
extern "C" {            /* Use "C" linkage when in C++ mode */
#endif

/* Global variables in gatest.c */

extern int              cntDevice,numDevices;
extern ibool            softwareEmulate,useGTF,fullSpeed,breakPoint,doSmallTest;
extern ibool            doubleBuffer;
extern ibool            noDither;
extern char             accelTestName[];
extern char             exePathName[];
extern GA_initFuncs     init;
extern GA_driverFuncs   driver;
extern GA_glInitFuncs   glFuncs;
extern GA_monoCursor    arrowCursor;

typedef ibool   (*doTestType)(GC_devCtx *gc,GA_glCtx *gl,int xRes,int yRes,int bitsPerPixel,N_uint32 flags,int refreshRate,GA_CRTCInfo *crtc,N_uint32 planeMask,ibool force1bppShadow);

/*------------------------- Function Prototypes ---------------------------*/

/* General utility functions in util.c */

void    SetGraphicsMode(GC_devCtx *gc);
void    RestoreMode(GC_devCtx *gc);
int     GetModeName(char *buf,GA_modeInfo *mi);
void    InitMouse(int xRes,int yRes);
void    GetMousePos(int *x,int *y);
int     EVT_kbhit(void);
int     EVT_getch(void);
int     GetString(GC_devCtx *gc,char *str,int maxLen);

/* DPMS tests in dpmstest.c */

void    DPMSTests(GC_devCtx *gc);

/* DDC tests in ddctest.c */

void    DDCTests(GC_devCtx *gc);

/* DDC/CI tests in ddccitst.c */

void    DDCCITests(GC_devCtx *gc);

/* Text mode tests in txttest.c */

ibool   doTextTest(GC_devCtx *gc,GA_glCtx *gl,int xRes,int yRes,int bitsPerPixel,N_uint32 flags,int refreshRate,GA_CRTCInfo *crtc,N_uint32 planeMask,ibool force1bppShadow);

/* Centering and refresh control in center.c */

ibool   doCenter(GC_devCtx *gc,GA_glCtx *gl,int xRes,int yRes,int bitsPerPixel,N_uint32 flags,int refreshRate,GA_CRTCInfo *crtc,N_uint32 planeMask,ibool force1bppShadow);

/* TV centering and refresh control in tv.c */

ibool   CenterTVMode(GC_devCtx *gc,N_uint32 mode);

/* OpenGL compatible 3D tests in opengl.c */

ibool   doOpenGLTest(GC_devCtx *gc,GA_glCtx *gl,int xRes,int yRes,int bitsPerPixel,N_uint32 flags,int refreshRate,GA_CRTCInfo *crtc,N_uint32 planeMask,ibool force1bppShadow);
int     bounceTest(GA_HGLRC glrc,int width,int height);
int     gearsTest(GA_HGLRC glrc,int width,int height);
int     gears2Test(GA_HGLRC glrc,int width,int height);
int     mechTest(GA_HGLRC glrc,int width,int height);
int     mothTest(GA_HGLRC glrc,int width,int height);
int     steamTest(GA_HGLRC glrc,int width,int height);

/* Visual tests in tests.c */

void    displaymodeInfo(void);
void    moire(GA_color defcolor);
int     moireTest(void);
int     pageFlipTest(void);
int     paletteTest(void);
int     wideDACTest(void);
ibool   doTest(GC_devCtx *gc,GA_glCtx *gl,int xRes,int yRes,int bitsPerPixel,N_uint32 flags,int refreshRate,GA_CRTCInfo *crtc,N_uint32 planeMask,ibool force1bppShadow);
ibool   doScrollTest(GC_devCtx *gc,GA_glCtx *gl,int xRes,int yRes,int bitsPerPixel,N_uint32 flags,int refreshRate,GA_CRTCInfo *crtc,N_uint32 planeMask,ibool force1bppShadow);
ibool   doZoomTest(GC_devCtx *gc,GA_glCtx *gl,int xRes,int yRes,int bitsPerPixel,N_uint32 flags,int refreshRate,GA_CRTCInfo *crtc,N_uint32 planeMask,ibool force1bppShadow);

/* Video Overlay tests in video.c */

ibool   doVideoOverlayTest(GC_devCtx *gc,GA_glCtx *gl,int xRes,int yRes,int bitsPerPixel,N_uint32 flags,int refreshRate,GA_CRTCInfo *crtc,N_uint32 planeMask,ibool force1bppShadow);

/* Video utility functions in vidutil.c */

void    ClearVideoBuffer(GA_buf* buf);
int     LoadVideoBuffer(GA_buf* buf);

/* Video capture tests in capture.c */

ibool   doVideoCaptureTest(GC_devCtx *gc,GA_glCtx *gl,int xRes,int yRes,int bitsPerPixel,N_uint32 flags,int refreshRate,GA_CRTCInfo *crtc,N_uint32 planeMask,ibool force1bppShadow);

/* Acceleration tests in conform/2dtests.c */

int     doAccelTests(const char *testName);
void    doCursorInit(void);
int     doCursorTest(const char *testName);
void    displayTestNames(void);

/* Stereo display mode test in tests.c */

ibool   doStereoTest(GC_devCtx *gc,GA_glCtx *gl,int xRes,int yRes,int bitsPerPixel,N_uint32 flags,int refreshRate,GA_CRTCInfo *crtc,N_uint32 planeMask,ibool force1bppShadow);

#ifdef  __cplusplus
}                       /* End of "C" linkage for C++   */
#endif

#endif  /* __GATEST_H */
