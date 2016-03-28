/****************************************************************************
*
*                    MegaVision Application Framework
*
*      A C++ GUI Toolkit for the SciTech Multi-platform Graphics Library
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
* Language:     C++ 3.0
* Environment:  Any
*
* Description:  Header file for the MVRenderArea class. This class is used
*               to represent a special type of view that is used for
*               performing double buffered animation in a window. It is
*               an abstract base class that relies upon a user suppiled
*               render() method to render the actual image in the viewing
*               area. It automatically takes care of setting up and
*               maintaining the double buffered view (if double buffering
*               is available).
*
****************************************************************************/

#ifndef __MVIS_MRENDERA_HPP
#define __MVIS_MRENDERA_HPP

#ifndef __MVIS_MVIEW_HPP
#include "mvis/mview.hpp"
#endif

/*-------------------------- Class definitions ----------------------------*/

//---------------------------------------------------------------------------
// The MVRenderArea class is an abstract base class used to represent
// an area for peforming double buffered animation.
//---------------------------------------------------------------------------

class MVRenderArea : public MVView {
protected:
    ibool       doubleBuffered;     // True if double buffering is on

            // Method to swap the display buffers
            void swapBuffers(ibool waitVRT = true)
            {
                if (doubleBuffered)
                    dc.swapBuffers(waitVRT);
            };

            // Method to render the display and swap buffers
            void renderAndSwap(ibool waitVRT = true)
            { render(); swapBuffers(waitVRT); };

            // Method to turn double buffering on
            void doubleBufferOn();

            // Method to turn double buffering off
            void doubleBufferOff();

            // Clip and render against a single rectangle
    static  MVRenderArea *clipAndRenderView;

            // Paint the view clipping to clip region (calls render)
    virtual void paint();

public:
    static  void _clipAndRender(const rect_t *r);

public:
            // Constructor
            MVRenderArea(MGLDevCtx& dc,const MVRect& bounds);

            // The draw function is not used for the MVRenderArea
    virtual void draw(const MVRect&) {};

            // Method to render a user defined frame
    virtual void render() = 0;
    };

#endif  // __MVIS_MRENDERA_HPP
