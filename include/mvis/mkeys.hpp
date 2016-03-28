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
* Description:  Header file defining the keyboard constants used by the
*               library. Also defines the class used to represent keyboard
*               hot keys.
*
****************************************************************************/

#ifndef __MVIS_MKEYS_HPP
#define __MVIS_MKEYS_HPP

#ifndef __MVIS_MEVENT_HPP
#include "mvis/mevent.hpp"
#endif

/*------------------------------ Constants --------------------------------*/

const uint

// Scan codes for keypad keys (backwards compatible with MGL 4.0)

    kbGrayEnter = KB_padEnter,
    kbGrayMinus = KB_padMinus,
    kbGrayPlus  = KB_padPlus,
    kbGrayTimes = KB_padTimes,
    kbGrayDivide= KB_padDivide,

// Scan codes for keypad keys

    kbPadEnter  = KB_padEnter,
    kbPadMinus  = KB_padMinus,
    kbPadPlus   = KB_padPlus,
    kbPadTimes  = KB_padTimes,
    kbPadDivide = KB_padDivide,
    kbPadLeft   = KB_padLeft,
    kbPadRight  = KB_padRight,
    kbPadUp     = KB_padUp,
    kbPadDown   = KB_padDown,
    kbPadIns    = KB_padInsert,
    kbPadDel    = KB_padDelete,
    kbPadHome   = KB_padHome,
    kbPadEnd    = KB_padEnd,
    kbPadPgUp   = KB_padPageUp,
    kbPadPgDn   = KB_padPageDown,
    kbPadCenter = KB_padCenter,

// Scan codes for function keys

    kbF1        = KB_F1,
    kbF2        = KB_F2,
    kbF3        = KB_F3,
    kbF4        = KB_F4,
    kbF5        = KB_F5,
    kbF6        = KB_F6,
    kbF7        = KB_F7,
    kbF8        = KB_F8,
    kbF9        = KB_F9,
    kbF10       = KB_F10,
    kbF11       = KB_F11,
    kbF12       = KB_F12,

// Scan codes for cursor control keys

    kbLeft      = KB_left,
    kbRight     = KB_right,
    kbUp        = KB_up,
    kbDown      = KB_down,
    kbIns       = KB_insert,
    kbDel       = KB_delete,
    kbHome      = KB_home,
    kbEnd       = KB_end,
    kbPgUp      = KB_pageUp,
    kbPgDn      = KB_pageDown,
    kbCenter    = KB_padCenter,
    kbCaps      = KB_capsLock,
    kbNum       = KB_numLock,
    kbScroll    = KB_scrollLock,
    kbLShift    = KB_leftShift,
    kbRShift    = KB_rightShift,
    kbLCtrl     = KB_leftCtrl,
    kbRCtrl     = KB_rightCtrl,
    kbLAlt      = KB_leftAlt,
    kbRAlt      = KB_rightAlt,
    kbLWindows  = KB_leftWindows,
    kbRWindows  = KB_rightWindows,
    kbMenu      = KB_menu,

// Scan codes for normal keyboard keys

    kbEsc       = KB_esc,
    kb1         = KB_1,
    kb2         = KB_2,
    kb3         = KB_3,
    kb4         = KB_4,
    kb5         = KB_5,
    kb6         = KB_6,
    kb7         = KB_7,
    kb8         = KB_8,
    kb9         = KB_9,
    kb0         = KB_0,
    kbMinus     = KB_minus,
    kbEquals    = KB_equals,
    kbBackSlash = KB_backSlash,
    kbBack      = KB_backspace,
    kbTab       = KB_tab,
    kbQ         = KB_Q,
    kbW         = KB_W,
    kbE         = KB_E,
    kbR         = KB_R,
    kbT         = KB_T,
    kbY         = KB_Y,
    kbU         = KB_U,
    kbI         = KB_I,
    kbO         = KB_O,
    kbP         = KB_P,
    kbLeftBrace = KB_leftSquareBrace,
    kbRightBrace= KB_rightSquareBrace,
    kbEnter     = KB_enter,
    kbA         = KB_A,
    kbS         = KB_S,
    kbD         = KB_D,
    kbF         = KB_F,
    kbG         = KB_G,
    kbH         = KB_H,
    kbJ         = KB_J,
    kbK         = KB_K,
    kbL         = KB_L,
    kbSemiColon = KB_semicolon,
    kbApostrophe= KB_apostrophe,
    kbZ         = KB_Z,
    kbX         = KB_X,
    kbC         = KB_C,
    kbV         = KB_V,
    kbB         = KB_B,
    kbN         = KB_N,
    kbM         = KB_M,
    kbComma     = KB_comma,
    kbPeriod    = KB_period,
    kbDivide    = KB_divide,
    kbSpace     = KB_space,
    kbTilde     = KB_tilde,

// No key is valid

    kbNoKey     = 0x00;

/*-------------------------- Class definitions ----------------------------*/

class MVHotKey {
protected:
    uint    scanCode;           // Scan code for the key
    ulong   modifiers;          // Modifier flags (shift, ctrl, alt)

public:
            // Constructor
            MVHotKey(uint scanCode,ulong modifiers = 0)
                : scanCode(scanCode), modifiers(modifiers & mdKey) {};

            // Method to test a keyboard event for hot key value
            ibool operator == (const MVKeyEvent& event) const;
    };

#endif  // __MVIS_MKEYS_HPP
