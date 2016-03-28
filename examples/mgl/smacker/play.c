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
* Environment:  DOS/Windows.
*
* Description:  This sample program shows how you can use the RAD Software
*               Smacker libraries with the MGL for playing back Smacker
*               video files under both DOS and Windows. All playback is
*               done in fullscreen video modes for maximum performance.
*
*               Although the interface to this program is a simple command
*               line interface, it support a variety of playback methods
*               depending on the underlying hardware:
*
*                   1. Decoding to a system buffer and blitting this
*                      system buffer to the screen.
*
*                   2. Decoding directly to a hardware or virtual linear
*                      framebuffer.
*
*                   3. Option 1 above with hardware double buffering.
*
*                   4. Decoding directly to an offscreen linear framebuffer
*                      and using the hardware blitter to copy the offscreen
*                      buffer to the screen.
*
*                   5. Option 4 above with hardware double buffering.
*
*               By default the program will select the highest performance
*               option available (in order from 1 to 5, with 5 being the
*               highest performance option).
*
*               You can specify the playback option to use on the command
*               line after the name of the Smacker file to play.
*
*               NOTE: This program uses the Miles Sound System Lite for
*                     sound support under DOS and Windows. A copy of this
*                     comes with the Smacker SDK from RAD Software.
*
*               NOTE: In order to be able to re-build this sample program,
*                     you must have a copy of the Smacker SDK from RAD
*                     Software. Please contact RAD Software directly for
*                     more information (801-322-4300, CompuServe: 73237,75).
*
*               NOTE: This sample program also assumes that the Smacker
*                     SDK header files and library files are somewhere on
*                     your current INCLUDE and LIBRARY directory search
*                     path.
*
*               NOTE: For brevity we also do no error checking. If an error
*                     occurs the program will simply exit without any
*                     messages.
*
*
****************************************************************************/

// TODO: Compile with Smacker SDK for MGL 5.x API

#define INCLUDE_COMMDLG_H   /* Watcom Win386 support    */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mgraph.h"
#include "ztimer.h"
#include "pmapi.h"
#include "smack.h"
#ifdef  __REALDOS__
#include <conio.h>
#endif
#ifdef  USE_UVBELIB
#include "uvbelib.h"
#endif

/*---------------------------- Global Variables ---------------------------*/

#ifdef  __WINDOWS32__
#define WIN_TYPE        "Win32"
#define NOTIFY_CODE     HIWORD(wParam)
#else
#ifdef  __WIN386__
#define WIN_TYPE        "Win386"
#else
#define WIN_TYPE        "Win16"
#endif
#define NOTIFY_CODE     HIWORD(lParam)
#endif

#define PLY_AUTO    0       /* Detect highest performance playback      */
#define PLY_SYSBUF  1       /* Decode to system buffer and blit         */
#define PLY_LINBUF  2       /* Decode direct to linear buffer           */
#define PLY_SYSDBUF 3       /* Decode and blit with double buffering    */
#define PLY_HWBUF   4       /* Decode to offscreen memory and HW blit   */
#define PLY_HWDBUF  5       /* Decode to offscreen/HW blit and dbl buff */
#define PLY_MAX     6

#define MODE_AUTO   -1      /* Autodetect the playback mode             */

typedef struct {            /* Structure to hold dirty rectangles       */
    long    x,y,w,h;
    } whRECT;

whRECT      rects1[128];    /* Rectangle lists for hardware dbl buffer  */
whRECT      rects2[128];
whRECT      merged[128];
uint        lastnum = 0;
whRECT      *lastframe = rects1;
whRECT      *curframe = rects2;
#ifdef  USE_UVBELIB
ibool       useUVBELib = true;
#endif

/*----------------------------- Implementation ----------------------------*/

/* Code to merge dirty rectangle lists from the Smacker SDK sample code */

#define RectBytes(r) (((r)->w)*((r)->h))

static void mergeinterrect(whRECT* r1, whRECT* r2, whRECT* m, whRECT* i)
{
  if (r1->x > r2->x) {
    m->w=r1->w+(r1->x - r2->x);    // do merge
    m->x=r2->x;
    i->x=r1->x;
    i->w=r1->w;
  } else {
    i->w=r1->w-(r2->x - r1->x);    // do inter
    i->x=r2->x;
    m->x=r1->x;
    m->w=r1->w;
  }
  if (r1->y > r2->y) {
    m->h=r1->h+(r1->y - r2->y);    // do merge
    m->y=r2->y;
    i->y=r1->y;
    i->h=r1->h;
  } else {
    i->h=r1->h-(r2->y - r1->y);    // do inter
    i->y=r2->y;
    m->y=r1->y;
    m->h=r1->h;
  }

  if ((r1->x+r1->w) < (r2->x+r2->w))
    m->w=(r2->x+r2->w) - m->x;    // do merge
  else
    i->w=(r2->x+r2->w) - i->x;    // do inter

  if ((r1->y+r1->h) < (r2->y+r2->h))
    m->h=(r2->y+r2->h) - m->y;    // do merge
  else
    i->h=(r2->y+r2->h) - i->y;    // do inter

  if ((i->w<=0) || (i->h<=0)) {
    i->x=0;
    i->y=0;
    i->w=0;
    i->h=0;
  }
}

static void mergerects(whRECT* r1, u32 r1num, whRECT* r2, u32 r2num, whRECT** o, u32* onum)
{
  u32 c;
  whRECT bestmerg,tr1,tr2,tr3;
  u32 bestwast,besttot;
  whRECT* mr;
  u32 mnum;
  u32 totnum;
  u32 totamt,wasted;
  u32 rtotal;

  if (r1num==0) {     // if no rects in r1, return r2
    *o=r2;
    *onum=r2num;
  } else if (r2num==0) {  // if no rects in r2, return r1
    *o=r1;
    *onum=r1num;
  } else {
    // merge 'em

    *o=merged;
    mnum=0;

    tr1=r1[0];
    totnum=r1num+r2num-1;
    r1[0].w=-r1[0].w;
    while (totnum) {
      bestwast=0x1000000;
      rtotal=RectBytes(&tr1);
      for(c=0;c<r1num;c++) {
        if (r1[c].w>0) {
          mergeinterrect(&r1[c],&tr1,&tr2,&tr3);
          totamt=RectBytes(&tr2);
          wasted=totamt-RectBytes(&r1[c])-rtotal+RectBytes(&tr3);
          if (wasted<bestwast) {
            besttot=totamt;
            bestwast=wasted;
            bestmerg=tr2;
            mr=&r1[c];
            if (wasted==0)
              goto doamerge;
          }
        }
      }
      for(c=0;c<r2num;c++) {
        if (r2[c].w>0) {
          mergeinterrect(&r2[c],&tr1,&tr2,&tr3);
          totamt=RectBytes(&tr2);
          wasted=totamt-RectBytes(&r2[c])-rtotal+RectBytes(&tr3);
          if (wasted<bestwast) {
            besttot=totamt;
            bestwast=wasted;
            bestmerg=tr2;
            mr=&r2[c];
            if (wasted==0)
              goto doamerge;
          }
        }
      }
      if ((bestwast) && ((besttot/bestwast)<20)) {
        // more than 5% wasted blitting, so don't merge
        merged[mnum++]=tr1;
        if (bestwast!=0x1000000) {
          tr1=*mr;
          mr->w=-mr->w;
        }
      } else {
       doamerge:
        tr1=bestmerg;
        mr->w=-mr->w;
      }
      totnum--;
    }
    merged[mnum++]=tr1;
    *onum=mnum;
    for(c=0;c<r1num;c++)
      r1[c].w=abs(r1[c].w);
    for(c=0;c<r2num;c++)
      r2[c].w=abs(r2[c].w);
  }
}

/* Find a graphics mode to suit the Smacker file */

void FindGraphicsMode(ibool autoMode,int x,int y,int *mode,int *numBuffers,int *plyType)
{
    int         i,mx,my,bits,pages;
    ibool       haveLinear = false,haveAccel = false;
    const char  *name;
    uchar       *modeList;

    /* Search for a suitable graphics mode */
    if (autoMode) {
        modeList = 0;
        *mode = MODE_AUTO;
        for (i = 0; modeList[i] != 0xFF; modeList[i] = i, i++) {
            if (MGL_modeResolution(modeList[i],&mx,&my,&bits)) {
                if (mx == x && my == y && bits == 8) {
                    /* Exact match */
                    *mode = modeList[i];
                    break;
                    }
                if (mx >= x && my >= y) {
                    /* Mode is larger than the video being played. The MGL
                     * always sorts modes by increasing resolution, so modes
                     * later in the list will be higher resolution that the
                     * one that we just found.
                     *
                     * NOTE: We only choose 1:1 aspect ratio modes if we cannot
                     *       find a perfect match.
                     */
                    if ((float)x / (float)y != (1.0/3.0)) {
                        *mode = modeList[i];
                        break;
                        }
                    }
                }
            }
        if (*mode == MODE_AUTO)
            *mode = 0;
        }
    MGL_modeResolution(*mode,&mx,&my,&bits);
    pages = MGL_availablePages(*mode);

    /* Determine if we have a linear buffer and if the mode is
     * accelerated. For the moment there is no defined way to get this
     * information from the MGL without first starting a graphics mode,
     * but we can look at the driver names for the mode that is selected.
     */
    name = MGL_modeDriverName(*mode);
    if (strstr(name,"LINEAR") != 0)
        haveLinear = true;
    if (strstr(name,"ACCEL") != 0) {
        haveAccel = true;
        haveLinear = true;
        }

    /* Override cases that the user may force that are not available */
    if (!haveAccel && *plyType >= PLY_HWBUF)
        *plyType = PLY_AUTO;
    if (pages < 2 && *plyType >= PLY_HWBUF)
        *plyType = PLY_AUTO;
    if (pages < 3 && *plyType >= PLY_HWDBUF)
        *plyType = PLY_AUTO;
    if (pages < 2 && *plyType == PLY_SYSDBUF)
        *plyType = PLY_AUTO;
    if (!haveLinear && *plyType == PLY_LINBUF)
        *plyType = PLY_AUTO;

    /* Determine the highest performance playback method */
#ifndef MGL_LITE
    if (*plyType == PLY_AUTO && (mx >= x) && (my >= y)) {
        if (haveAccel) {
            /* We have a hardware accelerator so decode direct to video
             * memory and hardware blit if we have enough pages.
             */
            if (pages >= 3) {
                *plyType = PLY_HWDBUF;
                *numBuffers = 3;
                }
            else if (pages == 2) {
                *plyType = PLY_HWBUF;
                *numBuffers = 2;
                }
            }
        }
#endif
    if (*plyType == PLY_AUTO) {
        if (pages >= 2) {
            *plyType = PLY_SYSDBUF;
            *numBuffers = 2;
            }
        else if (haveLinear) {
            *plyType = PLY_LINBUF;
            *numBuffers = 1;
            }
        else {
            *plyType = PLY_SYSBUF;
            *numBuffers = 1;
            }
        }
}

/* Set a Smacker palette to the hardware. The MGL always takes palette
 * values in 8 bits per primary format, so we need to convert the palette.
 */

void SetPalette(MGLDC *dc,uchar *pal)
{
    int         i;
    palette_t   mglPal[256],*p;

    for (i = 0,p = mglPal; i < 256; i++,p++) {
        p->red = (*pal++ << 2);
        p->green = (*pal++ << 2);
        p->blue = (*pal++ << 2);
        }
    MGL_setPalette(dc,mglPal,256,0);
    MGL_realizePalette(dc,256,0,false);
}

/* Play smacker file to system buffer and blt to screen */

int PlaySysBuf(MGLDC *dc,Smack *smk)
{
    int             frame,dstLeft,dstTop;
    rect_t          oldClip,vp;
    event_t         evt;
    pixel_format_t  pf;
    MGLDC           *memdc;

    /* Allocate memory for system buffer */
    if ((memdc = MGL_createMemoryDC(smk->Width,smk->Height,8,&pf)) == NULL)
        exit(1);

    /* Center bitmap on screen */
    dstLeft = (MGL_maxxDC(dc) - MGL_maxxDC(memdc)) / 2;
    dstTop = (MGL_maxyDC(dc) - MGL_maxyDC(memdc)) / 2;
    MGL_getClipRectDC(dc,&oldClip);
    if (dstLeft >= 0 && dstTop >= 0) {
        /* Disable clipping if the video fits on the screen */
        MGL_getViewportDC(dc,&vp);
        MGL_setClipRectDC(dc,vp);
        }

    /* Flush the event queue before starting playback */
    EVT_flush(EVT_EVERYEVT);

    SmackToBuffer(smk,0,0,memdc->mi.bytesPerLine,memdc->mi.yRes,memdc->surface,0);
    for (frame = 0; frame < smk->Frames; frame++) {
        if (smk->NewPalette)
            SetPalette(dc,smk->Palette);
        SmackDoFrame(smk);
        while (SmackToBufferRect(smk,SMACKSURFACEFAST)) {
            MGL_bitBltCoord(dc,memdc,
                smk->LastRectx,smk->LastRecty,
                smk->LastRectx+smk->LastRectw,
                smk->LastRecty+smk->LastRecth,
                dstLeft + smk->LastRectx,dstTop + smk->LastRecty,
                MGL_REPLACE_MODE);
            }

        if (frame < smk->Frames)
            SmackNextFrame(smk);
        do {
            if (EVT_peekNext(&evt,EVT_KEYDOWN | EVT_MOUSEDOWN))
                goto Done;
            } while (SmackWait(smk));
        }

Done:
    MGL_destroyDC(memdc);
    MGL_setClipRectDC(dc,oldClip);
    return frame;
}

/* Play smacker file directly to linear framebuffer */

int PlayLinBuf(MGLDC *dc,Smack *smk)
{
    int     frame,dstLeft,dstTop;
    event_t evt;

    /* Center bitmap on screen */
    dstLeft = (MGL_maxxDC(dc)+1 - smk->Width) / 2;
    dstTop = (MGL_maxyDC(dc)+1 - smk->Height) / 2;
    if (dstLeft < 0 || dstTop < 0)
        return 0;

    /* Flush the event queue before starting playback */
    EVT_flush(EVT_EVERYEVT);

    SmackToBuffer(smk,dstLeft,dstTop,dc->mi.bytesPerLine,dc->mi.yRes,dc->surface,0);
    for (frame = 0; frame < smk->Frames; frame++) {
        if (smk->NewPalette)
            SetPalette(dc,smk->Palette);
        SmackDoFrame(smk);
        if (frame < smk->Frames)
            SmackNextFrame(smk);
        do {
            if (EVT_peekNext(&evt,EVT_KEYDOWN | EVT_MOUSEDOWN))
                goto Done;
            } while (SmackWait(smk));
        }

Done:
    return frame;
}

/* Play smacker file to system buffer and blit with double buffering */

int PlaySysDBuf(MGLDC *dc,Smack *smk)
{
    int             frame,dstLeft,dstTop,apage,vpage;
    rect_t          oldClip,vp;
    event_t         evt;
    MGLDC           *memdc;
    pixel_format_t  pf;
    whRECT          *r;
    ulong           rnum;
    ulong           curnum;

    /* Allocate memory for system buffer */
    if ((memdc = MGL_createMemoryDC(smk->Width,smk->Height,8,&pf)) == NULL)
        exit(1);

    /* Center bitmap on screen */
    dstLeft = (MGL_maxxDC(dc) - MGL_maxxDC(memdc)) / 2;
    dstTop = (MGL_maxyDC(dc) - MGL_maxyDC(memdc)) / 2;
    MGL_getClipRectDC(dc,&oldClip);
    if (dstLeft >= 0 && dstTop >= 0) {
        /* Disable clipping if the video fits on the screen */
        MGL_getViewportDC(dc,&vp);
        MGL_setClipRectDC(dc,vp);
        }

    /* Flush the event queue before starting playback */
    EVT_flush(EVT_EVERYEVT);

    /* Enable double buffering */
    MGL_setActivePage(dc,apage = 1);
    MGL_setVisualPage(dc,vpage = 0,false);

    SmackToBuffer(smk,0,0,memdc->mi.bytesPerLine,memdc->mi.yRes,memdc->surface,0);
    for (frame = 0; frame < smk->Frames; frame++) {
        if (smk->NewPalette)
            SetPalette(dc,smk->Palette);
        SmackDoFrame(smk);

        /* Smacker's dirty rect system is based on the current frame, so
         * we have to do a little extra work to combine the dirty rects from
         * this frame with the dirty rects of the previous frame.
         */
        curnum = 0;
        while (SmackToBufferRect(smk,SMACKSURFACEFAST)) {
            if (smk->LastRectw) {
                curframe[curnum].x = smk->LastRectx;
                curframe[curnum].y = smk->LastRecty;
                curframe[curnum].w = smk->LastRectw;
                curframe[curnum].h = smk->LastRecth;
                curnum++;
                }
            }

        /* Merge with the last set of rects */
        mergerects(curframe,curnum,lastframe,lastnum,&r,&rnum);

        /* Blit the rects */
        while (rnum--) {
            MGL_bitBltCoord(dc,memdc,
                r[rnum].x,r[rnum].y,
                r[rnum].x+r[rnum].w,
                r[rnum].y+r[rnum].h,
                dstLeft + r[rnum].x,dstTop + r[rnum].y,
                MGL_REPLACE_MODE);
            }

        /* Swap the dirty rect pointers */
        r = lastframe;
        lastframe = curframe;
        curframe = r;
        lastnum = curnum;

        /* Flip the hidden page */
        MGL_setActivePage(dc,apage ^= 1);
        MGL_setVisualPage(dc,vpage ^= 1,false);

        if (frame < smk->Frames)
            SmackNextFrame(smk);
        do {
            if (EVT_peekNext(&evt,EVT_KEYDOWN | EVT_MOUSEDOWN))
                goto Done;
            } while (SmackWait(smk));
        }
Done:
    MGL_destroyDC(memdc);
    MGL_setClipRectDC(dc,oldClip);
    return frame;
}

#ifndef MGL_LITE

/* Play smacker file to hardware offscreen memory and blit */

int PlayHWBuf(MGLDC *dc,Smack *smk)
{
    int     frame,dstLeft,dstTop;
    rect_t  oldClip,vp;
    MGLDC   *offdc;
    event_t evt;

    /* Create an offscreen memory DC for rendering to */
    if ((offdc = MGL_createOffscreenDC()) == NULL)
        exit(1);
    if ((MGL_maxxDC(offdc)+1) < smk->Width || (MGL_maxyDC(offdc)+1) < smk->Height)
        exit(1);

    /* Center video on screen */
    dstLeft = (MGL_maxxDC(dc)+1 - smk->Width) / 2;
    dstTop = (MGL_maxyDC(dc)+1 - smk->Height) / 2;
    MGL_getClipRectDC(dc,&oldClip);
    if (dstLeft < 0 || dstTop < 0)
        return 0;
    /* Disable clipping if the video fits on the screen */
    MGL_getViewportDC(offdc,&vp);
    MGL_setClipRectDC(offdc,vp);
    MGL_getViewportDC(dc,&vp);
    MGL_setClipRectDC(dc,vp);

    /* Flush the event queue before starting playback */
    EVT_flush(EVT_EVERYEVT);

    SmackToBuffer(smk,0,0,offdc->mi.bytesPerLine,offdc->mi.yRes,offdc->surface,0);
    for (frame = 0; frame < smk->Frames; frame++) {
        if (smk->NewPalette)
            SetPalette(dc,smk->Palette);
        SmackDoFrame(smk);
        while (SmackToBufferRect(smk,SMACKSURFACEFAST)) {
            MGL_bitBltCoord(dc,offdc,
                smk->LastRectx,smk->LastRecty,
                smk->LastRectx+smk->LastRectw,
                smk->LastRecty+smk->LastRecth,
                dstLeft + smk->LastRectx,dstTop + smk->LastRecty,
                MGL_REPLACE_MODE);
            }

        if (frame < smk->Frames)
            SmackNextFrame(smk);
        do {
            if (EVT_peekNext(&evt,EVT_KEYDOWN | EVT_MOUSEDOWN))
                goto Done;
            } while (SmackWait(smk));
        }
Done:
    MGL_destroyDC(offdc);
    MGL_setClipRectDC(dc,oldClip);
    return frame;
}

/* Play smacker file and hw blit with double buffering */

int PlayHWDBuf(MGLDC *dc,Smack *smk)
{
    int     frame,dstLeft,dstTop,apage,vpage;
    rect_t  oldClip,vp;
    MGLDC   *offdc;
    event_t evt;
    whRECT  *r;
    ulong   rnum;
    ulong   curnum;

    /* Create an offscreen memory DC for rendering to */
    if ((offdc = MGL_createOffscreenDC()) == NULL)
        exit(1);
    if ((MGL_maxxDC(offdc)+1) < smk->Width || (MGL_maxyDC(offdc)+1) < smk->Height)
        exit(1);

    /* Center video on screen */
    dstLeft = (MGL_maxxDC(dc)+1 - smk->Width) / 2;
    dstTop = (MGL_maxyDC(dc)+1 - smk->Height) / 2;
    MGL_getClipRectDC(dc,&oldClip);
    if (dstLeft < 0 || dstTop < 0)
        return 0;
    /* Disable clipping if the video fits on the screen */
    MGL_getViewportDC(offdc,&vp);
    MGL_setClipRectDC(offdc,vp);
    MGL_getViewportDC(dc,&vp);
    MGL_setClipRectDC(dc,vp);

    /* Flush the event queue before starting playback */
    EVT_flush(EVT_EVERYEVT);

    /* Enable double buffering */
    MGL_setActivePage(dc,apage = 1);
    MGL_setVisualPage(dc,vpage = 0,0);

    SmackToBuffer(smk,0,0,offdc->mi.bytesPerLine,offdc->mi.yRes,offdc->surface,0);
    for (frame = 0; frame < smk->Frames; frame++) {
        if (smk->NewPalette)
            SetPalette(dc,smk->Palette);
        SmackDoFrame(smk);

        /* Smacker's dirty rect system is based on the current frame, so
         * we have to do a little extra work to combine the dirty rects from
         * this frame with the dirty rects of the previous frame.
         */
        curnum = 0;
        while (SmackToBufferRect(smk,SMACKSURFACEFAST)) {
            if (smk->LastRectw) {
                curframe[curnum].x = smk->LastRectx;
                curframe[curnum].y = smk->LastRecty;
                curframe[curnum].w = smk->LastRectw;
                curframe[curnum].h = smk->LastRecth;
                curnum++;
                }
            }

        /* Merge with the last set of rects */
        mergerects(curframe,curnum,lastframe,lastnum,&r,&rnum);

        /* Blit the rects */
        while (rnum--) {
            MGL_bitBltCoord(dc,offdc,
                r[rnum].x,r[rnum].y,
                r[rnum].x+r[rnum].w,
                r[rnum].y+r[rnum].h,
                dstLeft + r[rnum].x,dstTop + r[rnum].y,
                MGL_REPLACE_MODE);
            }

        /* Swap the dirty rect pointers */
        r = lastframe;
        lastframe = curframe;
        curframe = r;
        lastnum = curnum;

        /* Flip the hidden page */
        MGL_setActivePage(dc,apage ^= 1);
        MGL_setVisualPage(dc,vpage ^= 1,false);

        if (frame < smk->Frames)
            SmackNextFrame(smk);
        do {
            if (EVT_peekNext(&evt,EVT_KEYDOWN | EVT_MOUSEDOWN))
                goto Done;
            } while (SmackWait(smk));
        }
Done:
    MGL_destroyDC(offdc);
    MGL_setClipRectDC(dc,oldClip);
    return frame;
}

#endif  /* !MGL_LITE */

#ifdef  __REALDOS__

/* Display usage information for DOS version */

void help(int driver)
{
    int     i,x,y,bits;
    uchar   *modeList = 0;

    printf("Usage: play [-l] <filename> [play back type] [video mode]\n");
    printf("\n");
    printf("Where -l is used to disable the UVBELib linkable libraries, 'play back type'\n");
    printf("is one of the 5 pre-defined playback methods and 'video mode' is the video\n");
    printf("you wish to play back in. By default if you dont specify these, the playback\n");
    printf("type and video mode are selected automatically.\n");
    printf("\n");
    printf("Press a key for list of video modes.");
    fflush(stdout);
    getch();
    printf("\n\nAvailable modes are:\n");

    for (i = 0; modeList[i] != 0xFF; modeList[i] = i, i++) {
        if (!MGL_modeResolution(modeList[i],&x,&y,&bits))
            continue;
        printf("    %2d - %4d x %4d x %2d %2d page (%s)\n",
            modeList[i],x,y,bits,
            MGL_availablePages(modeList[i]),
            MGL_modeDriverName(modeList[i]));
        }

    exit(1);
}

/* Main routine */

void main(int argc,char *argv[])
{
    MGLDC   *dc;
    int     mode,x,y,bits,refreshRate = MGL_DEFAULT_REFRESH;
    int     numBuffers,plyType = PLY_AUTO,frames;
    char    smackName[PM_MAX_PATH],modeName[PM_MAX_PATH];
    float   fps;
    Smack   *smk;
#ifdef  USE_UVBELIB
    int     errCode;
    char    driverPath[PM_MAX_PATH];

    if (argc >= 2 && argv[1][0] == '-' && argv[1][1] == 'l') {
        useUVBELib = false;
        argc--;
        argv++;
        }

    if (useUVBELib) {
        UV_getDriverPath(argv[0], driverPath);
        if ((errCode = UV_install(driverPath,true,true)) > UV_lastNonFatal) {
            printf(UV_getErrorStr(errCode));
            exit(1);
            }
        }
#endif
    ZTimerInit();

    /* Get playback type and filename from command line */
    if (argc != 2 && argc != 3 && argc != 4)
        help(driver);
    strcpy(smackName,argv[1]);
    if (argc >= 3) {
        plyType = atoi(argv[2]);
        if (plyType <= PLY_AUTO || plyType >= PLY_MAX) {
            printf("Play type must be between 1 and 5!\n\n");
            help(driver);
            }
        }
    if (argc == 4)
        mode = atoi(argv[3]);
    else
        mode = MODE_AUTO;

    /* Initialise the Miles Sound System Lite */
    SmackSoundMSSLiteInit();

    /* Open Smacker file */
    smk = SmackOpen(smackName,SMACKNEEDPAN|SMACKTRACKS,SMACKAUTOEXTRA);
    if (!smk)
        exit(1);

    /* Start a 1:1 graphics mode as close to the smacker files as possible */
    FindGraphicsMode((mode == MODE_AUTO),smk->Width,smk->Height,&mode,&numBuffers,&plyType);
    if (!MGL_init(".",NULL))
        MGL_fatalError(MGL_errorMsg(MGL_result()));
    if ((dc = MGL_createDisplayDC(mode,numBuffers,refreshRate)) == NULL)
        MGL_fatalError(MGL_errorMsg(MGL_result()));
    MGL_modeResolution(dc,&x,&y,&bits);
    sprintf(modeName,"%4d x %4d x %2d",x,y,bits);

#ifndef MGL_LITE
    /* Turn off identity palette checking for maximum performance */
    MGL_checkIdentityPalette(false);
#endif

    /* Play the Smacker file */
    ULZTimerOn();
    switch (plyType) {
        case PLY_SYSBUF:    frames = PlaySysBuf(dc,smk);    break;
        case PLY_LINBUF:    frames = PlayLinBuf(dc,smk);    break;
        case PLY_SYSDBUF:   frames = PlaySysDBuf(dc,smk);   break;
#ifndef MGL_LITE
        case PLY_HWBUF:     frames = PlayHWBuf(dc,smk);     break;
        case PLY_HWDBUF:    frames = PlayHWDBuf(dc,smk);    break;
#endif
        }
    ULZTimerOff();
    fps = frames / (ULZTimerCount() * (ULZTimerResolution() / 1000000.0));

    /* Close everything and exit */
    MGL_exit();
    SmackClose(smk);
    SmackSoundMSSLiteDone();

    switch (plyType) {
        case PLY_SYSBUF:
            printf("Played to system buffer with blt to screen\n");
            break;
        case PLY_LINBUF:
            printf("Played directly to screen (linear)\n");
            break;
        case PLY_SYSDBUF:
            printf("Played to system buffer with blt to double buffered screen\n");
            break;
        case PLY_HWBUF:
            printf("Played to offscreen memory with hardware blt to screen\n");
            break;
        case PLY_HWDBUF:
            printf("Played to offscreen memory with hardware blt to double buffered screen\n");
            break;
        }
    printf("\nPlayed back in: %s at %.1f fps\n\n", modeName, fps);
    printf("This program uses the Smacker Software Development Kit from RAD Software. For\n");
    printf("more information on Smacker and the Smacker SDK, please contact RAD Software\n");
    printf("directly:\n\n");
    printf("     RAD Software\n");
    printf("     850 South Main Street\n");
    printf("     Salt Lake City, UT 84101\n");
    printf("     Phone: (801) 322-4300\n");
    printf("     Fax:   (801) 359-6169\n");
    printf("     Email: jeffr@radvantage.com\n");
    printf("     CIS:   73237,75\n");

#ifdef  USE_UVBELIB
    UV_exit();
#endif
}

#else   /* __WINDOWS__ */
#include <commdlg.h>
#ifdef  USE_CTL3D
#include <ctl3d.h>
#endif
#include "play.rh"

PRIVATE short   modeNums[MAX_MODES];    /* List of modes to select from     */
PRIVATE char    szAppFilter[] = "Smacker Videos (*.smk)\0*.smk\0";
PRIVATE char    videoName[PM_MAX_PATH] = "";
PRIVATE int     mode,plyType = PLY_AUTO;

void CenterWindow(HWND hWndCenter, HWND parent, BOOL repaint)
/****************************************************************************
*
* Function:     CenterWindow
* Parameters:   hWndCenter  - Window to center
*               parent      - Handle for parent window
*               repaint     - True if window should be re-painted
*
* Description:  Centers the specified window within the bounds of the
*               specified parent window. If the parent window is NULL, then
*               we center it using the Desktop window.
*
****************************************************************************/
{
    HWND    hWndParent = (parent ? parent : GetDesktopWindow());
    RECT    RectParent;
    RECT    RectCenter;
    int     CenterX,CenterY,Height,Width;

    GetWindowRect(hWndParent, &RectParent);
    GetWindowRect(hWndCenter, &RectCenter);

    Width = (RectCenter.right - RectCenter.left);
    Height = (RectCenter.bottom - RectCenter.top);
    CenterX = ((RectParent.right - RectParent.left) - Width) / 2;
    CenterY = ((RectParent.bottom - RectParent.top) - Height) / 2;

    if ((CenterX < 0) || (CenterY < 0)) {
        /* The Center Window is smaller than the parent window. */
        if (hWndParent != GetDesktopWindow()) {
            /* If the parent window is not the desktop use the desktop size. */
            CenterX = (GetSystemMetrics(SM_CXSCREEN) - Width) / 2;
            CenterY = (GetSystemMetrics(SM_CYSCREEN) - Height) / 2;
            }
        CenterX = (CenterX < 0) ? 0: CenterX;
        CenterY = (CenterY < 0) ? 0: CenterY;
        }
    else {
        CenterX += RectParent.left;
        CenterY += RectParent.top;
        }

    /* Copy the values into RectCenter */
    RectCenter.left = CenterX;
    RectCenter.right = CenterX + Width;
    RectCenter.top = CenterY;
    RectCenter.bottom = CenterY + Height;

    /* Move the window to the new location */
    MoveWindow(hWndCenter, RectCenter.left, RectCenter.top,
            (RectCenter.right - RectCenter.left),
            (RectCenter.bottom - RectCenter.top), repaint);
}

void RefreshModeList(HWND hwnd)
/****************************************************************************
*
* Function:     RefreshModeList
* Parameters:   hwnd    - Handle to dialog box window
*
* Description:  Refreshes the list of available video modes in the video
*               mode list box.
*
****************************************************************************/
{
    char    buf[MAX_STR];
    int     i,modes,x,y,bits,selectBits = 8,mode;
    uchar   *modeList;
    HWND    hwndLst = GetDlgItem(hwnd,IDC_MODELIST);

    /* Init MGL to construct mode list */
    MGL_init(".",NULL);

    SendMessage(hwndLst,LB_RESETCONTENT,0,0);
    SendMessage(hwndLst,LB_ADDSTRING,0,(LONG)"Auto Detect");
    modes = 0;
    modeNums[modes++] = MODE_AUTO;
    for (modeList = 0,i = 0; modeList[i] != 0xFF; modeList[i] = i, i++) {
        if (!MGL_modeResolution(mode = modeList[i],&x,&y,&bits))
            continue;
        if (bits != selectBits)
            continue;
        sprintf(buf,"%4d x %4d x %2d %2d page (%s)",
            x,y,bits,MGL_availablePages(mode),
            MGL_modeDriverName(mode));
        SendMessage(hwndLst,LB_ADDSTRING,0,(LONG)buf);
        modeNums[modes++] = mode;
        }
    SendMessage(hwndLst,LB_SETCURSEL,0,0);

    MGL_exit();
}

ibool OpenVideoFile(HWND hwnd)
/****************************************************************************
*
* Function:     OpenVideoFile
* Parameters:   hwnd    - Handle to parent window
* Returns:      True for valid bitmap name, false otherwise
*
* Description:  Open the bitmap file and load it into the memory DC.
*
****************************************************************************/
{
    OPENFILENAME    ofn;

    videoName[0] = 0;

    /* Prompt user for file to open */
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.hInstance = NULL;
    ofn.lpstrFilter = szAppFilter;
    ofn.lpstrCustomFilter = NULL;
    ofn.nMaxCustFilter = 0;
    ofn.nFilterIndex = 0;
    ofn.lpstrFile = videoName;
    ofn.nMaxFile = sizeof(videoName);
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.lpstrTitle = NULL;
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;
    ofn.nFileOffset = 0;
    ofn.nFileExtension = 0;
    ofn.lpstrDefExt = NULL;
    ofn.lCustData = 0;
    ofn.lpfnHook = NULL;
    ofn.lpTemplateName = NULL;
    return GetOpenFileName(&ofn);
}

void playVideo(int plyType,int mode,char *smackName)
/****************************************************************************
*
* Function:     playVideo
* Parameters:   mode        - Mode to play video in
*               smackName   - Name of video to load and play
*
* Description:  Procedure to play the specified video in the specified
*               mode. This code takes care of creating the necessary
*               MGL device contexts, and calling the appropriate playback
*               routine to play the video.
*
****************************************************************************/
{
    MGLDC   *dc;
    int     numBuffers,frames,x,y,bits,refreshRate = MGL_DEFAULT_REFRESH;
    float   fps;
    Smack   *smk;
    char    buf[255],modeName[255];

    /* Determine the number of buffers for forced playback modes */
    switch (plyType) {
        case PLY_SYSBUF:    numBuffers = 1; break;
        case PLY_LINBUF:    numBuffers = 1; break;
        case PLY_SYSDBUF:   numBuffers = 2; break;
        case PLY_HWBUF:     numBuffers = 2; break;
        case PLY_HWDBUF:    numBuffers = 3; break;
        }

    /* Open Smacker file */
    smk = SmackOpen(smackName,SMACKNEEDPAN|SMACKTRACKS,SMACKAUTOEXTRA);
    if (!smk)
        exit(1);

    /* Start a 1:1 graphics mode as close to the smacker files as possible */
    FindGraphicsMode((mode == MODE_AUTO),smk->Width,smk->Height,&mode,&numBuffers,&plyType);
    if (!MGL_init(".",NULL))
        MGL_fatalError(MGL_errorMsg(MGL_result()));
    if ((dc = MGL_createDisplayDC(mode,numBuffers,refreshRate)) == NULL)
        MGL_fatalError(MGL_errorMsg(MGL_result()));
    MGL_modeResolution(dc,&x,&y,&bits);
    sprintf(modeName,"%4d x %4d x %2d",x,y,bits);

    /* Delay for half a second before starting playback */
    MGL_delay(500);

#ifndef MGL_LITE
    /* Turn off identity palette checking for maximum performance */
    MGL_checkIdentityPalette(false);
#endif

    /* Play the Smacker file */
    ULZTimerOn();
    switch (plyType) {
        case PLY_SYSBUF:    frames = PlaySysBuf(dc,smk);    break;
        case PLY_LINBUF:    frames = PlayLinBuf(dc,smk);    break;
        case PLY_SYSDBUF:   frames = PlaySysDBuf(dc,smk);   break;
#ifndef MGL_LITE
        case PLY_HWBUF:     frames = PlayHWBuf(dc,smk);     break;
        case PLY_HWDBUF:    frames = PlayHWDBuf(dc,smk);    break;
#endif
        }
    ULZTimerOff();
    fps = frames / (ULZTimerCount() * (ULZTimerResolution() / 1000000.0));

    /* Close everything and exit */
    MGL_exit();
    SmackClose(smk);

    /* Delay for a second before return to Windows */
    MGL_delay(1000);

    /* Display playback info message on screen */
    switch (plyType) {
        case PLY_SYSBUF:
            sprintf(buf,"Played to system buffer with blt to screen\r\n\r\n"
                "Video mode: %s\r\nFrame rate: %.2f fps",modeName,fps);
            break;
        case PLY_LINBUF:
            sprintf(buf,"Played directly to screen (linear)\r\n\r\n"
                "Video mode: %s\r\nFrame rate: %.2f fps",modeName,fps);
            break;
        case PLY_SYSDBUF:
            sprintf(buf,"Played to system buffer with blt to double buffered screen\r\n\r\n"
                "Video mode: %s\r\nFrame rate: %.2f fps",modeName,fps);
            break;
        case PLY_HWBUF:
            sprintf(buf,"Played to offscreen memory with hardware blt to screen\r\n\r\n"
                "Video mode: %s\r\nFrame rate: %.2f fps",modeName,fps);
            break;
        case PLY_HWDBUF:
            sprintf(buf,"Played to offscreen memory with hardware blt to double buffered screen\r\n\r\n"
                "Video mode: %s\r\nFrame rate: %.2f fps",modeName,fps);
            break;
        }
    MessageBox(NULL,buf,
        "Video Playback Information",
        MB_ICONINFORMATION | MB_OK | MB_TASKMODAL);
}

BOOL CALLBACK MainDlgProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
/****************************************************************************
*
* Function:     MainDlgProc
*
* Description:  Dialog procedure for front end dialog box.
*
****************************************************************************/
{
    HWND    hwndLst;
    int     mode;
    char    buf[MAX_STR];
    char    format[MAX_STR];

    switch (msg) {
        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case IDC_CANCEL:
                    EndDialog(hwnd,IDC_CANCEL);
                    break;
                case IDC_SELECT_VIDEO:
                    OpenVideoFile(hwnd);
                    break;
                case IDC_MODELIST:
                    if (NOTIFY_CODE != LBN_DBLCLK)
                        break;
                case IDC_OK:
                    hwndLst = GetDlgItem(hwnd,IDC_MODELIST);
                    if (SendMessage(hwndLst,LB_GETCOUNT,0,0)) {
                        if (videoName[0] == 0 && !OpenVideoFile(hwnd))
                            break;
                        mode = modeNums[SendMessage(hwndLst,LB_GETCURSEL,0,0)];
                        playVideo(plyType,mode,videoName);
                        }
                    break;
                case IDC_AUTODETECT:    plyType = PLY_AUTO;         break;
                case IDC_SYSMEM:        plyType = PLY_SYSBUF;       break;
                case IDC_LINMEM:        plyType = PLY_LINBUF;       break;
                case IDC_SYSMEMDBUF:    plyType = PLY_SYSDBUF;      break;
                case IDC_HWMEM:         plyType = PLY_HWBUF;        break;
                case IDC_HWMEMDBUF:     plyType = PLY_HWDBUF;       break;
                }
            break;
        case WM_INITDIALOG:
            CenterWindow(hwnd,NULL,FALSE);
            GetDlgItemText(hwnd,IDC_VERSIONSTR,format,sizeof(format));
            sprintf(buf,format,WIN_TYPE);
            SetDlgItemText(hwnd,IDC_VERSIONSTR,buf);
            CheckDlgButton(hwnd,IDC_AUTODETECT,TRUE);
            plyType = PLY_AUTO;
            RefreshModeList(hwnd);
            return TRUE;
        }
    lParam = lParam;
    return FALSE;
}

ibool HaveWin95(void)
{
    int verMajor = GetVersion() & 0xFF;
    return (verMajor >= 4);
}

int PASCAL WinMain(HINSTANCE hInst,HINSTANCE hPrev,LPSTR szCmdLine,int sw)
{
#ifdef  USE_UVBELIB
    int     errCode;
    char    driverPath[PM_MAX_PATH];
    char    szModuleName[PM_MAX_PATH];
#endif

#ifdef  USE_CTL3D
    if (!HaveWin95()) {
        Ctl3dRegister(hInst);
        Ctl3dAutoSubclass(hInst);
        }
#endif

#ifdef  USE_UVBELIB
    if (useUVBELib) {
        GetModuleFileName(hInst,szModuleName,sizeof(szModuleName));
        UV_getDriverPath(szModuleName, driverPath);
        if ((errCode = UV_install(driverPath,true,true)) > UV_lastNonFatal) {
            MessageBox(NULL,UV_getErrorStr(errCode),"Fatal UVBELib Error!",
                MB_ICONEXCLAMATION | MB_OK | MB_TASKMODAL);
            exit(1);
            }
        }
#endif
    ZTimerInit();

    DialogBox(hInst,MAKEINTRESOURCE(IDD_MAINDLG),NULL,(DLGPROC)MainDlgProc);

    /* Display message box about Smacker */
    MessageBox(NULL,
        "This program uses the Smacker Software Development Kit from RAD\r\n"
        "Software. For more information on Smacker and the Smacker SDK,\r\n"
        "please contact RAD Software directly:\r\n\r\n"
        "     RAD Software\r\n"
        "     850 South Main Street\r\n"
        "     Salt Lake City, UT 84101\r\n"
        "     Phone: (801) 322-4300\r\n"
        "     Fax:   (801) 359-6169\r\n"
        "     Email: jeffr@radvantage.com\r\n"
        "     CIS:   73237,75",
        "Information about Smacker",
        MB_ICONINFORMATION | MB_OK | MB_TASKMODAL);

#ifdef  USE_UVBELIB
    UV_exit();
#endif
#ifdef  USE_CTL3D
    if (!HaveWin95())
        Ctl3dUnregister(hInst);
#endif
    (void)hPrev;
    (void)szCmdLine;
    (void)sw;
    return 0;
}

#endif  /* __WINDOWS__ */

#include "radmal.i"
