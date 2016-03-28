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
* Description:  Header file for the MVScrollBar classes. These classes
*               are used to represent the horizontal and vertical
*               scroll bar objects.
*
*               You may make the scroll bar's as wide as you wish, but the
*               standard scroll bar width is 14 pixels.
*
****************************************************************************/

#ifndef __MVIS_MSCROLLB_HPP
#define __MVIS_MSCROLLB_HPP

#ifndef __MVIS_MVIEW_HPP
#include "mvis/mview.hpp"
#endif

/*------------------------------ Constants --------------------------------*/

uint const

// Scroll bar part codes

    sbLeftArrow     = 0,
    sbRightArrow    = 1,
    sbPageLeft      = 2,
    sbPageRight     = 3,
    sbThumb         = 4,

// Scroll bar flags

    sbInteracting   = 0x0001,       // The scroll bar is interacting
    sbDirtyLeft     = 0x0002,       // The left arrow is dirty
    sbDirtyRight    = 0x0004,       // The right arrow is dirty
    sbDirtyThumb    = 0x0008,       // The thumb is dirty
    sbDirty         = sbDirtyLeft | sbDirtyRight | sbDirtyThumb;

/*-------------------------- Class definitions ----------------------------*/

//---------------------------------------------------------------------------
// The following is the class used to represent both horiztontal and
// vertical scroll bars.
//
// MVScrollBar's are intended to be inserted into MVWindow and descendant
// classes.
//---------------------------------------------------------------------------

class MVScrollBar : public MVView {
protected:
    int     value;              // Current value for scroll bar
    int     minVal,maxVal;      // Minimum/maximum scroll bar values
    int     pageStep;           // Amount to move if page area activated
    int     arrowStep;          // Amount to move if arrow area activated
    MVRect  leftArrow;          // Bounds for left arrow
    MVRect  rightArrow;         // Bounds for right arrow
    MVRect  thumb;              // Bounds for thumb
    ibool   vertical;           // True if scroll bar is vertical
    uint    flags;              // Flags for the scroll bar

            // Methods to draw the scroll bar parts
            void drawLeftArrow(ibool down);
            void drawRightArrow(ibool down);
            void drawThumb(int which);

            // Method to determine which part of scroll bar was clicked
            int getPartHit(const MVPoint& p);

            // Method to draw the specified part
            void drawPart(int part,ibool down);

            // Method to compute the location of the thumb
            void moveThumb();

            // Methods to change the scroll bar value, and redraw it
            void changeValue(int part);
            void changeValue(const MVPoint& where);
public:
            // Constructor
            MVScrollBar(MGLDevCtx& dc,const MVRect& bounds);

            // Virtual to handle an event
    virtual void handleEvent(MVEvent& event,phaseType phase);

            // Method to move the group to another location
    virtual void moveTo(int x,int y);

            // Method to set the bounds for the scroll bar
    virtual void setBounds(const MVRect& bounds);

            // Method to draw the scrollbar in current state
    virtual void draw(const MVRect& dirty);

            // Method to update the state of the scroll bar (thumb posn)
            void update();

            // Methods to get/set the values for the scroll bar
            void setValue(int v);
            void setMinVal(int v);
            void setMaxVal(int v);
            void setPageStep(int s)     { pageStep = s; };
            void setArrowStep(int s)    { arrowStep = s; };
            int getValue() const        { return value; };
            int getMinVal() const       { return minVal; };
            int getMaxVal() const       { return maxVal; };
            int getPageStep() const     { return pageStep; };
            int getArrowStep() const    { return arrowStep; };
            void setRange(int min,int max);
    };

#endif  // __MVIS_MSCROLLB_HPP
