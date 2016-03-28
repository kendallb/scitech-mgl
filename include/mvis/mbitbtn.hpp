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
* Description:  Header file for the MVBitmapButton classes for buttons
*               that are defined by a set of bitmaps.
*
****************************************************************************/

#ifndef __MVIS_MBITBTN_HPP
#define __MVIS_MBITBTN_HPP

#ifndef __MVIS_MBUTTON_HPP
#include "mvis/mbutton.hpp"
#endif

/*-------------------------- Class definitions ----------------------------*/

//---------------------------------------------------------------------------
// The following is a class for creating standard buttons that are defined
// but a set of system bitmap indices. You must provide a set of 32 possible
// indices to represent all the different states that a button may be in
// from the button body state flags. In most cases you will simply use the
// same index for multiple entries.
//---------------------------------------------------------------------------

class MVSysBitmapButton : public MVButtonBase {
protected:
    char    index[32];          // Array of 16 possible bitmap indexes

            // Internal method to draw the button body in specified state
    virtual void drawBody(const MVRect& clip,uint bodyState);

public:
            // Constructor
            MVSysBitmapButton(MGLDevCtx& dc,const MVRect& bounds,
                ulong command,int *indexes,uint flags = bfNormal,
                ibool selected = false);
    };

//---------------------------------------------------------------------------
// The following is a class for creating standard buttons that are defined
// but a set of user defined bitmaps. You must provide a set of 16 possible
// bitmaps to represent all the different states that a button may be in
// from the button body state flags. In most cases you will simply use the
// same bitmap for multiple entries. Note also that this class does not
// own the bitmaps, so they will not be destroyed when the button is
// destroyed (allowing you to use them for multiple buttons).
//---------------------------------------------------------------------------

class MVBitmapButton : public MVButtonBase {
protected:
    bitmap_t    *bmp[32];       // Array of 32 possible bitmaps to display

            // Internal method to draw the button body in specified state
    virtual void drawBody(const MVRect& clip,uint bodyState);

public:
            // Constructor
            MVBitmapButton(MGLDevCtx& dc,const MVRect& bounds,ulong command,
                bitmap_t *bmps[],uint flags = bfNormal,ibool selected = false);
    };

#endif  // __MVIS_MBITBTN_HPP
