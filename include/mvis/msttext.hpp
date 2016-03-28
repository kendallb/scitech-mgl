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
* Description:  Header file for the MVStaticText class. This class is used
*               to represent text items in a view that are static and
*               do not respond to events. Test items clear the visRgn
*               as they dont clear the background that they are drawn over.
*               Hence you can overlay static text items on top of other
*               background items.
*
****************************************************************************/

#ifndef __MVIS_MSTTEXT_HPP
#define __MVIS_MSTTEXT_HPP

#ifndef __MVIS_MVIEW_HPP
#include "mvis/mview.hpp"
#endif

/*------------------------------ Constants --------------------------------*/

uint const

// Static text creation flags

    sfLeftJust      = 0x0001,       // Text is left justified
    sfRightJust     = 0x0002,       // Text is right justified
    sfTopJust       = 0x0004,       // Text is top justified
    sfBottomJust    = 0x0008,       // Text is bottom justified
    sfWrap          = 0x0010,       // Text wraps at right edge
    sfNormal        = sfLeftJust | sfTopJust | sfWrap;

/*-------------------------- Class definitions ----------------------------*/

//---------------------------------------------------------------------------
// The MVStaticText class is used to represent text items in a view that
// are static and do not respond to any events. MVStaticText views cannot
// be focused.
//
// Note that the text will be clipped to the bounds of the text item.
//---------------------------------------------------------------------------

class MVStaticText : public MVView {
protected:
    const char      *text;      // Text to display
    MVPoint         start;      // Position to begin drawing title at
    MVTextJust      tjust;      // Text justification
    int             textColor;  // Color value to display text
    uint            flags;      // Creation flags

public:
            // Constructor
            MVStaticText(MGLDevCtx& dc,const MVRect& bounds,
                const char *text,uint flags = sfNormal,
                int color = scStaticText);

            // Method to set the bounds for the static text item
    virtual void setBounds(const MVRect& bounds);

            // Method to draw the static text item
    virtual void draw(const MVRect& dirty);

            // Method to set the text for the text item - returns old value
    virtual const char *setText(const char *t);
    };

//---------------------------------------------------------------------------
// The MVLabel class is a special static text class that can be used to
// automatically select another view when it is clicked.
//---------------------------------------------------------------------------

class MVLabel : public MVStaticText {
protected:
    MVView  *attached;          // View attached to the label

public:
            // Constructor
            MVLabel(MGLDevCtx& dc,const MVRect& bounds,const char *text,
                MVView *attached,uint flags = sfNormal,
                int color = scStaticText)
                : MVStaticText(dc,bounds,text,flags,color),
                  attached(attached) {};

            // Method to set/get the attached view
            void setAttached(MVView *a) { attached = a; };
            MVView *getAttached()       { return attached; };

            // Virtual to handle an event
    virtual void handleEvent(MVEvent& event,phaseType phase = phFocused);
    };

//---------------------------------------------------------------------------
// The TMultiLineText class is used to represent static text items in a
// view that contain multiple lines of text.
//---------------------------------------------------------------------------

class MVMultiLineText : public MVStaticText {
protected:
    int     lines;              // Number of lines of text

public:
            // Constructor
            MVMultiLineText(MGLDevCtx& dc,const MVRect& bounds,
                const char *text,uint flags = sfNormal,
                int color = scStaticText);

            // Method to draw the static text item
    virtual void draw(const MVRect& dirty);

            // Method to set the text for the text item - returns old value
    virtual const char *setText(const char *t);

            // Method to obtain the number of lines
            int numberOfLines() { return lines; };
    };

#endif  // __MVIS_MSTTEXT_HPP
