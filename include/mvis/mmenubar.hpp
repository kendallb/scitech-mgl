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
* Description:  Header file for the MVMenuBar class. This class is used
*               to implement the standard menu bar mechanism for the
*               library.
*
****************************************************************************/

#ifndef __MVIS_MMENUBAR_HPP
#define __MVIS_MMENUBAR_HPP

#ifndef __MVIS_MMENU_HPP
#include "mvis/mmenu.hpp"
#endif

/*-------------------------- Class definitions ----------------------------*/

//---------------------------------------------------------------------------
// The MVMenuBar class is the class used to implement the standard horizontal
// menu bar mechanism for the library. The palette is the same as the
// standard MVMenu palette.
//---------------------------------------------------------------------------

class MVMenuBar : public MVMenu {
protected:
            // Method to draw a menu item
    virtual void drawItem(int index);

            // Method to compute the bounding rectangle for an item
    virtual MVRect& setItemBounds(int index,const MVPoint& start);

            // Method to find which menu item was hit
            int findSelected(MVPoint& where);

public:
            // Constructor
            MVMenuBar(MGLDevCtx& dc,const MVRect& bounds)
                : MVMenu(dc,bounds) {};

            // Virtual to execute the menu tracking the mouse etc
    virtual uint execute();

            // Method to complete the menu bar definition
    virtual void doneDefinition();
    };

#endif  // __MVIS_MMENUBAR_HPP
