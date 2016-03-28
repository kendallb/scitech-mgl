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
* Environment:  Linux
*
* Description:  Linux specific code for binding OpenGL support with Mesa
*               in the Linux environment. All we do is provide the bindings
*               between the MGL OpenGL code and the Mesa for MGL API.
*
****************************************************************************/

#include "mgl.h"

/*------------------------- Implementation --------------------------------*/

ibool MGLAPI MGL_glHaveHWOpenGL(void)
{ return false; }

void MGLAPI MGL_glChooseVisual(MGLDC *dc,MGLVisual *visual)
{ MGLMesaChooseVisual(dc,visual); }

ibool MGLAPI MGL_glSetVisual(MGLDC *dc,MGLVisual *visual)
{ return MGLMesaSetVisual(dc,visual); }

ibool MGLAPI MGL_glCreateContext(MGLDC *dc,int flags)
{
    MGLVisual   visual;
    palette_t   pal[256];

    if (!(flags & MGL_GL_VISUAL)) {
        /* Select the visual for the programmer based on the passed in
         * flags for easy setup and initialization.
         */
        memset(&visual,0,sizeof(visual));
        visual.rgb_flag = (flags & MGL_GL_INDEX) == 0;
        visual.alpha_flag = (flags & MGL_GL_ALPHA) != 0;
        visual.db_flag = (flags & MGL_GL_DOUBLE) != 0;
        if (flags & MGL_GL_DEPTH)
            visual.depth_size = 16;
        if (flags & MGL_GL_STENCIL)
            visual.stencil_size = 8;
        if (flags & MGL_GL_ACCUM)
            visual.accum_size = 16;
        if (!MGL_glSetVisual(dc,&visual))
            return false;
        }
    if (dc->cntVis.rgb_flag && dc->mi.bitsPerPixel == 8) {
        MGL_getHalfTonePalette(pal);
        MGL_glSetPalette(dc,pal,256,0);
        MGL_glRealizePalette(dc,256,0,-1);
        }
    return MGLMesaCreateContext(dc,flags & MGL_GL_FORCEMEM);
}

void MGLAPI MGL_glDeleteContext(MGLDC *dc)
{ MGLMesaDestroyContext(dc); }


void MGLAPI MGL_glMakeCurrent(MGLDC *dc)
{ MGLMesaMakeCurrent(dc); }


void MGLAPI MGL_glSwapBuffers(MGLDC *dc,ibool waitVRT)
{ MGLMesaSwapBuffers(dc,waitVRT); }

void MGLAPI MGL_glSetPaletteEntry(MGLDC *dc,int entry,uchar red,uchar green,uchar blue)
{ MGLMesaSetPaletteEntry(dc,entry,red,green,blue); }


void MGLAPI MGL_glSetPalette(MGLDC *dc,palette_t *pal,int numColors,int startIndex)
{ MGLMesaSetPalette(dc,pal,numColors,startIndex); }


void MGLAPI MGL_glRealizePalette(MGLDC *dc,int numColors,int startIndex,int waitVRT)
{ MGLMesaRealizePalette(dc,numColors,startIndex,waitVRT); }
