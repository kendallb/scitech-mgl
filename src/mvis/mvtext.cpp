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
* Description:  Declarations of all the static text strings used by
*               the MegaVision. This should all eventually be moved into
*               a resource file.
*
****************************************************************************/

#include "mvis/mvision.hpp"
#include "mvis/msgbox.hpp"
#include "mvis/mfiledlg.hpp"
#include "mvis/mmodesel.hpp"

/*----------------------------- Implementation ----------------------------*/

const char *MVMsgBoxText::yesText           = "~Y~es";
const char *MVMsgBoxText::noText            = "~N~o";
const char *MVMsgBoxText::okText            = "O~K~";
const char *MVMsgBoxText::cancelText        = "Cancel";
const char *MVMsgBoxText::warningText       = "Warning";
const char *MVMsgBoxText::errorText         = "Error";
const char *MVMsgBoxText::informationText   = "Information";
const char *MVMsgBoxText::confirmText       = "Confirm";

const char *MVFileDialog::openText          = "~O~pen";
const char *MVFileDialog::saveText          = "~S~ave";
const char *MVFileDialog::helpText          = "Help";
const char *MVFileDialog::cancelText        = "Cancel";
const char *MVFileDialog::typeText          = "Type of file:";
const char *MVFileDialog::driveText         = "Drive:";
const char *MVFileDialog::fileText          = "File:";
const char *MVFileDialog::directoryText     = "Directory:";
const char *MVFileDialog::invalidDriveText  = "Invalid drive or directory";
const char *MVFileDialog::invalidFileText   = "Invalid file name";
const char *MVFileDialog::tooManyFilesText  = "Insufficient memory to load all files";

const char *MVModeSelector::setText         = "~S~et Mode";
const char *MVModeSelector::helpText        = "Help";
const char *MVModeSelector::cancelText      = "Cancel";
const char *MVModeSelector::modeListText    = "Video mode:";
