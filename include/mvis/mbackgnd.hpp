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
* Description:  Header file for the TBackGround class. This class is used
*               to draw a patterned background in the system background
*               colors.
*
****************************************************************************/

#ifndef __MVIS_MBACKGND_HPP
#define __MVIS_MBACKGND_HPP

#ifndef __MVIS_MVIEW_HPP
#include "mvis/mview.hpp"
#endif

/*-------------------------- Class definitions ----------------------------*/

//---------------------------------------------------------------------------
// The TBackGround class is a view that draws a patterned backGround in
// the system background colors.
//---------------------------------------------------------------------------

class MVBackGround : public MVView {
protected:
    int         fillStyle;      // Style for filling background
    pattern_t   pat;            // Pattern to draw with

public:
            // Constructor
            MVBackGround(MGLDevCtx& dc,const MVRect& bounds,
                int fillStyle = MGL_BITMAP_SOLID,pattern_t *pat = MGL_SOLID_FILL);

            // Virtual destructor
    virtual ~MVBackGround();

            // Method to draw the background
    virtual void draw(const MVRect& dirty);
    };

#endif  // __MVIS_MBACKGND_HPP
