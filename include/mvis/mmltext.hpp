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
* Description:  Header file for the TMultiLineText class. This class is used
*               to represent static text items in a view that may contain
*               multiple lines of text, separated by '\n' characters. The
*               text will automatically wrap at the bounds of the view.
*
****************************************************************************/

#ifndef __TMLTEXT_HPP
#define __TMLTEXT_HPP

#ifndef __TSTTEXT_HPP
#include "tsttext.hpp"
#endif

/*-------------------------- Class definitions ----------------------------*/

//---------------------------------------------------------------------------
// The TMultiLineText class is used to represent static text items in a
// view that contain multiple lines of text.
//---------------------------------------------------------------------------

class TMultiLineText : public TStaticText {
protected:
    ibool   wrap;               // True if text should wrap at right edge

public:
            // Constructor
            TMultiLineText(const TRect& bounds,const char *text,
                const TextSettings& tset,ibool wrap = true);

            // Method to draw the static text item
    virtual void draw();

            // Method to set the text for the text item - returns old value
    virtual const char *setText(const char *t);
    };

#endif  // __TMLTEXT_HPP
