/****************************************************************************
*
*                    SciTech SNAP Graphics Architecture
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
* Description:  Utility routines for SNAP Graphics video overlays.
*
****************************************************************************/

#include "gatest.h"
#include "snap/graphics/gabmp.h"

/*---------------------------- Global Variables ---------------------------*/

#define DEF_WIDTH   360
#define DEF_HEIGHT  288
#define DEF_Y_BMP   "vidtesty.bmp"
#define DEF_U_BMP   "vidtestu.bmp"
#define DEF_V_BMP   "vidtestv.bmp"
#define BMP_PATH    "bitmaps"

/*----------------------------- Implementation ----------------------------*/

/****************************************************************************
REMARKS:
Clears the video buffer for the specified YUV format. In order to get black,
the U and V channels need to be forced to half of maximum Y. Otherwise we
would get green with all channels zeroed.
    Y = R + G + B
    U = Y - R
    V = Y - B
    G = Y - R - B
      = U + V - Y
****************************************************************************/
void ClearVideoBuffer(
    GA_buf* buf)
{
    GA_color    color = 0;

    switch (buf->Format & gaVideoFormatMask) {
        case gaVideoYUV9:
            /* YUV9 planar mode has U and V in extra 1/8 buffer height */
            bufmgr.DrawRectBuf(buf,0,0,buf->HwStride,buf->Height,0,GA_REPLACE_MIX);
            bufmgr.DrawRectBuf(buf,0,buf->Height,buf->HwStride,buf->Height/8,maxcolor/2,GA_REPLACE_MIX);
            break;
        case gaVideoYUV12:
            /* YUV12 planar mode has U and V in extra 1/2 buffer height */
            bufmgr.DrawRectBuf(buf,0,0,buf->HwStride,buf->Height,0,GA_REPLACE_MIX);
            bufmgr.DrawRectBuf(buf,0,buf->Height,buf->HwStride,buf->Height/2,maxcolor/2,GA_REPLACE_MIX);
            break;
        case gaVideoYUV422:
            /* YUV422 packed modes have U and V interleaved in buffer */
            if (buf->Format & (gaVideoYUYV | gaVideoYVYU))
                color = 0x7F00;
            else if (buf->Format & (gaVideoUYVY | gaVideoVYUY))
                color = 0x007F;
            bufmgr.DrawRectBuf(buf,0,0,buf->Width,buf->Height,color,GA_REPLACE_MIX);
            defcolor = ~color;
            break;
        }
}

/****************************************************************************
REMARKS:
Loads YUV test bitmaps into the video overlay buffer
Returns false if overlay bitmaps aren't found or fail to load
****************************************************************************/
int LoadVideoBuffer(
    GA_buf* buf)
{
    bmp_t   *ybmp,*ubmp,*vbmp;
    GA_buf  *ybuf,*ubuf,*vbuf;
    N_uint8 *s,*d,*u,*v;
    int     i,j,k,l,w,h,uvstride,scale;
    int     n0 = 0, n1 = 0, n2 = 0, n3 = 0;
    char    drive[PM_MAX_DRIVE], dir[PM_MAX_PATH], name[PM_MAX_PATH], ext[PM_MAX_PATH];
    char    bmppath[PM_MAX_PATH];
    static  char ybmpname[PM_MAX_PATH],ubmpname[PM_MAX_PATH],vbmpname[PM_MAX_PATH];
    static  ibool firstTime = true;

    if (firstTime) {
        /* Construct the filename paths for loading the bitmaps */
        PM_splitpath(exePathName,drive,dir,name,ext);
        PM_makepath(bmppath,drive,dir,NULL,NULL);
        /* No path info at all?  Just use '.' */
        if (bmppath[0] == 0)
            bmppath[0] = '.', bmppath[1] = 0;
        PM_backslash(bmppath);
        strcat(bmppath,BMP_PATH);
        PM_backslash(bmppath);
        strcpy(ybmpname,bmppath);
        strcat(ybmpname,DEF_Y_BMP);
        strcpy(ubmpname,bmppath);
        strcat(ubmpname,DEF_U_BMP);
        strcpy(vbmpname,bmppath);
        strcat(vbmpname,DEF_V_BMP);
        firstTime = false;
        }

    if (access(ybmpname,R_OK) || access(ubmpname,R_OK) || access(vbmpname,R_OK))
        return false;

    /* Load individual Y,U,V bitmaps */
    ybmp = LoadBitmap(ybmpname,true);
    ubmp = LoadBitmap(ubmpname,true);
    vbmp = LoadBitmap(vbmpname,true);

    if ((ybmp == NULL) || (ubmp == NULL) || (vbmp == NULL))
        return false;

    /* Create system memory buffers for bitmaps */
    ybuf = bufmgr.AllocCustomBuffer(ybmp->width,ybmp->height,ybmp->bitsPerPixel,ybmp->pf,ybmp->surface);
    ubuf = bufmgr.AllocCustomBuffer(ubmp->width,ubmp->height,ubmp->bitsPerPixel,ubmp->pf,ubmp->surface);
    vbuf = bufmgr.AllocCustomBuffer(vbmp->width,vbmp->height,vbmp->bitsPerPixel,vbmp->pf,vbmp->surface);

    /* Perform palette conversion for non-uniform grayscale images */
    s = d = (N_uint8*)ybuf->Surface;
    for (i = 0; i < ybuf->Height * ybuf->Stride; i++) {
        d[i] = ybmp->pal[s[i]].Blue;
        }
    s = d = (N_uint8*)ubuf->Surface;
    for (i = 0; i < ubuf->Height * ubuf->Stride; i++) {
        d[i] = ubmp->pal[s[i]].Blue;
        }
    s = d = (N_uint8*)vbuf->Surface;
    for (i = 0; i < vbuf->Height * vbuf->Stride; i++) {
        d[i] = vbmp->pal[s[i]].Blue;
        }

    /* U and V channels are either 1/2 or 1/4 scale */
    scale = (buf->Format & gaVideoYUV9) ? 4 : 2;
    w = (buf->Width > ybuf->Width) ? ybuf->Width : buf->Width;
    h = (buf->Height > ybuf->Height) ? ybuf->Height : buf->Height;

    /* Reduce existing 1/2 scale U and V bitmaps for 1/4 scale format */
    if (scale == 4) {
        s = d = (N_uint8*)ubuf->Surface;
        for (i = 0; i < ubuf->Height/2; i++) {
            for (j = k = 0; j < ubuf->Width/2; j++, k+=2) {
                 d[j] = s[k];
                 }
            s += ubuf->Stride*2;
            d += ubuf->Stride;
            }
        s = d = (N_uint8*)vbuf->Surface;
        for (i = 0; i < vbuf->Height/2; i++) {
            for (j = k = 0; j < vbuf->Width/2; j++, k+=2) {
                 d[j] = s[k];
                 }
            s += vbuf->Stride*2;
            d += vbuf->Stride;
            }
        }

    switch (buf->Format & gaVideoFormatMask) {
        case gaVideoYUV9:
        case gaVideoYUV12:
            /* YUV planar modes have U and V at 1/2 or 1/4 size at end of buffer */
            bufmgr.BitBltBuf(ybuf,0,0,w,h,0,0,GA_REPLACE_MIX);
            bufmgr.LockBuffer(buf);
            w /= scale;
            h /= scale;
            uvstride = buf->HwStride/scale;
            if (buf->Format & gaVideoYUV)
                d = (N_uint8*)buf->Surface + buf->HwStride * buf->Height;
            else
                d = (N_uint8*)buf->Surface + buf->HwStride * buf->Height * (scale*scale+1) / (scale*scale);
            s = (N_uint8*)ubuf->Surface;
            for (i = 0; i < h; i++) {
                memcpy(d,s,w);
                s += ubuf->Stride;
                d += uvstride;
                }
            if (buf->Format & gaVideoYUV)
                d = (N_uint8*)buf->Surface + buf->HwStride * buf->Height * (scale*scale+1) / (scale*scale);
            else
                d = (N_uint8*)buf->Surface + buf->HwStride * buf->Height;
            s = (N_uint8*)vbuf->Surface;
            for (i = 0; i < h; i++) {
                memcpy(d,s,w);
                s += vbuf->Stride;
                d += uvstride;
                }
            bufmgr.UnlockBuffer(buf);
            break;
        case gaVideoYUV422:
            /* YUV422 packed modes have U and V interleaved in buffer */
            switch (buf->Format & gaVideoYUVLayoutMask) {
#ifdef __BIG_ENDIAN__
                case gaVideoYUYV: n0 = 1; n1 = 0; n2 = 3; n3 = 2; break;
                case gaVideoYVYU: n0 = 1; n3 = 0; n2 = 3; n1 = 2; break;
                case gaVideoUYVY: n1 = 1; n0 = 0; n3 = 3; n2 = 2; break;
                case gaVideoVYUY: n3 = 1; n0 = 0; n1 = 3; n2 = 2; break;
#else
                case gaVideoYUYV: n0 = 0; n1 = 1; n2 = 2; n3 = 3; break;
                case gaVideoYVYU: n0 = 0; n3 = 1; n2 = 2; n1 = 3; break;
                case gaVideoUYVY: n1 = 0; n0 = 1; n3 = 2; n2 = 3; break;
                case gaVideoVYUY: n3 = 0; n0 = 1; n1 = 2; n2 = 3; break;
#endif
                }
            bufmgr.LockBuffer(buf);
            d = (N_uint8*)buf->Surface;
            s = (N_uint8*)ybuf->Surface;
            u = (N_uint8*)ubuf->Surface;
            v = (N_uint8*)vbuf->Surface;
            for (i = 0; i < h; i++) {
                for (j = k = l = 0; k < w; j+=4, k+=2, l++) {
                    d[j+n0] = s[k];
                    d[j+n1] = u[l];
                    d[j+n2] = s[k+1];
                    d[j+n3] = v[l];
                    }
                d += buf->Stride;
                s += ybuf->Stride;
                if (i % 2) {
                    u += ubuf->Stride;
                    v += vbuf->Stride;
                    }
                }
            bufmgr.UnlockBuffer(buf);
            break;
        }

    /* Release buffer and bitmap resources */
    bufmgr.FreeBuffer(vbuf);
    bufmgr.FreeBuffer(ubuf);
    bufmgr.FreeBuffer(ybuf);

    UnloadBitmap(vbmp);
    UnloadBitmap(ubmp);
    UnloadBitmap(ybmp);
    return true;
}

