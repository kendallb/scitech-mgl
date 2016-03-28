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
* Description:  Defines the standard routines for popping up simple
*               message boxes. The text in message boxes is always
*               displayed in the standard system font, in the standard
*               static text color.
*
****************************************************************************/

#ifndef __MVIS_MSGBOX_HPP
#define __MVIS_MSGBOX_HPP

#ifndef __MVIS_MVISION_HPP
#include "mvis/mvision.hpp"
#endif

/*------------------------------ Constants --------------------------------*/

const uint

// Standard message box classes

    mfNothing       = 0x0000,           // Don't display an icon
    mfWarning       = 0x0001,           // Display the warning icon
    mfError         = 0x0002,           // Display the error icon
    mfInformation   = 0x0003,           // Display the information icon
    mfConfirmation  = 0x0004,           // Display the confirmation icon

// Text control flags

    mfCenterText    = 0x0008,           // Center the text in the box

// Message box button flags

    mfYesButton     = 0x0010,           // Box has a Yes button
    mfNoButton      = 0x0020,           // Box has a No button
    mfOKButton      = 0x0040,           // Box has an OK button
    mfCancelButton  = 0x0080,           // Box has a Cancel button

    mfYesDefault    = 0x0100,           // Yes button is the default
    mfNoDefault     = 0x0200,           // No button is the default
    mfOKDefault     = 0x0400,           // OK button is the default
    mfCancelDefault = 0x0800,           // Cancel button is the default

    mfYesNo         = mfYesButton | mfNoButton,
    mfYesNoCancel   = mfYesButton | mfNoButton | mfCancelButton,
    mfOKCancel      = mfOKButton | mfCancelButton;

/*-------------------------- Class Definitions ----------------------------*/

struct MVMsgBoxText {
    static const char *yesText;
    static const char *noText;
    static const char *okText;
    static const char *cancelText;
    static const char *warningText;
    static const char *errorText;
    static const char *informationText;
    static const char *confirmText;
    };

/*------------------------- Function Prototypes ---------------------------*/

uint MV_messageBox(MGLDevCtx dc,const char *msg,uint options);
uint MV_messageBox(MGLDevCtx dc,uint options,const char *msg, ... );
uint MV_messageBoxEx(MGLDevCtx dc,const char *msg,const char *title,uint options);
uint MV_messageBoxEx(MGLDevCtx dc,uint options,const char *title,const char *msg, ... );
uint MV_inputBox(MGLDevCtx dc,const char *title,const char *label,char *s,int limit);

#endif  // __MVIS_MSGBOX_HPP
