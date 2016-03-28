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
* Description:  Header file for the MVButton class. This class is the base
*               class for all buttons, and simply draws the solid button
*               and handles all the normal interaction.
*
****************************************************************************/

#ifndef __MVIS_MBUTTON_HPP
#define __MVIS_MBUTTON_HPP

#ifndef __MVIS_MVIEW_HPP
#include "mvis/mview.hpp"
#endif

/*------------------------------ Constants --------------------------------*/

uint const

// Button flags

    bfNormal        = 0x0000,       // Button is normal
    bfDefault       = 0x0001,       // Button is the default button
    bfLeftJust      = 0x0002,       // Button caption is left justified
    bfRightJust     = 0x0004,       // Button caption is right justified
    bfTopJust       = 0x0008,       // Button caption is top justtified
    bfBottomJust    = 0x0010,       // Button caption is bottom justified
    bfBroadcast     = 0x0020,       // Broadcast the command to owner view
    bfTwoState      = 0x0040,       // Button is a two state button
    bfSelectable    = 0x0080,       // Button is selectable
    bfGrabDefault   = 0x0100,       // True if button grabs default status
    bfAutoSelect    = 0x0200,       // Button grabs focus as soon as pressed
    bfDontDrawFace  = 0x0400,       // Dont draw button face in base class

// Button body states passed to drawBody()

    bsNormal        = 0x0000,       // Button is normal
    bsSelected      = 0x0001,       // Button is selected
    bsPressed       = 0x0002,       // Button is pressed
    bsDisabled      = 0x0004,       // Button is disabled
    bsFocused       = 0x0008,       // Button is focused
    bsDefault       = 0x0010;       // Button is default

/*-------------------------- Class definitions ----------------------------*/

//---------------------------------------------------------------------------
// The following is the base class for all button types. It does not
// draw the button caption, but relies on subclasses of this class to draw
// and appropriate caption. The palette values here are suggested colors
// only for the captions. Naturally if the caption is a color bitmap, the
// colors will be different!
//
// MVButton's are intended to be inserted into TDialogs.
//---------------------------------------------------------------------------

class MVButtonBase : public MVView {
protected:
    ulong   command;            // Command to generate when pressed
    uint    flags;              // Flags for this button
    ibool   amDefault;          // True if button is currently the default
    ibool   selected;           // True if button is selected
    ibool   pressed;            // True if button is currently pressed
    ibool   tracking;           // True when tracking button movement

            // Internal method to draw the button body in specified state
    virtual void drawBody(const MVRect& clip,uint bodyState);

            // Internal routine to activate the button when pressed
    virtual void activate();

            // Internal routine to send clicked message to owner
    virtual void buttonClicked();

public:
            // Constructor
            MVButtonBase(MGLDevCtx& dc,const MVRect& bounds,ulong command,
                uint flags,ibool selected = false);

            // Virtual to handle an event
    virtual void handleEvent(MVEvent& event,phaseType phase = phFocused);

            // Method to draw the button in current state
    virtual void draw(const MVRect& dirty);

            // Method to set the command posted by the button
            void setCommand(ulong c)    { command = c; };

            // Method to set a state flag
    virtual void setState(uint state,ibool set);

            // Return true if button is selected
            ibool isSelected()  { return selected; };

            // Select or deselect the button
            void setSelected(ibool sel);

            // Method to make the button the default
            void setDefault(ibool enable);
    };

//---------------------------------------------------------------------------
// The following class is used to represent a standard button, that
// contains text drawn within the bounds of the button.
//---------------------------------------------------------------------------

class MVButton : public MVButtonBase {
protected:
    char        *title;         // Text to display
    MVPoint     start;          // Position to begin drawing title at
    MVTextJust  tjust;          // Text justification
    char        hotChar;        // Character code for hot character
    uint        hotIndex;       // Index of hot character
    MVPoint     hot1,hot2;      // Coordinates of hot character underscore
    MVRect      focusRect;      // Focus rectangle for text

            // Internal method to draw the button body
    virtual void drawBody(const MVRect& clip,uint bodyState);

public:
            // Constructor
            MVButton(MGLDevCtx& dc,const MVRect& bounds,const char *title,
                ulong command,uint flags = bfNormal,ibool selected = false);

            // Destructor
    virtual ~MVButton();

            // Method to set the bounds for the button
    virtual void setBounds(const MVRect& bounds);

            // Virtual to handle an event
    virtual void handleEvent(MVEvent& event,phaseType phase = phFocused);

            // Method to set the title for the button
    virtual void setTitle(const char *t);
    };

#endif  // __MVIS_MBUTTON_HPP
