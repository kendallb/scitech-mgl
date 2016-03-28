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
* Description:  Header file for the MVFileDialog class. This class is used
*               to represent and dialog box for selecting a file to
*               open, or save, in a standard manner.
*
****************************************************************************/

#ifndef __MVIS_MFILEDLG_HPP
#define __MVIS_MFILEDLG_HPP

#include "mvis/mdialog.hpp"
#include "mvis/minputl.hpp"
#include "mvis/mlist.hpp"
#include "tcl/array.hpp"
#include "tcl/str.hpp"
#include <stdlib.h>

#ifndef _MAX_PATH
#if defined(__UNIX__) || defined(__OS2__)
#define _MAX_PATH   256
#define _MAX_DIR    256
#define _MAX_DRIVE  256
#define _MAX_FNAME  256
#define _MAX_EXT    256
#else
#include <dir.h>
#define _MAX_PATH   MAXPATH
#define _MAX_DIR    MAXDIR
#define _MAX_DRIVE  MAXDRIVE
#define _MAX_FNAME  MAXFILE
#define _MAX_EXT    MAXEXT
#endif
#endif

/*------------------------------ Constants --------------------------------*/

const uint

    fdOpenButton    = 0x0001,           // Dialog has an open button
    fdSaveButton    = 0x0002,           // Dialog has a save button
    fdHelpButton    = 0x0004,           // Dialog has a help button
    fdMustExist     = 0x0008,           // The file selected must exist

    fdDirLoaded     = 0x0100,           // The directory is currently loaded

    fdOpenDialog    = fdOpenButton | fdHelpButton,
    fdBrowseDialog  = fdOpenButton | fdHelpButton | fdMustExist,
    fdSaveAsDialog  = fdSaveButton | fdHelpButton;

/*-------------------------- Class definitions ----------------------------*/

typedef TCISArray<TCDynStr> FileList;   // Array of filenames

//---------------------------------------------------------------------------
// The MVFileDialog class is designed to provide a standard mechanism for
// allowing users to interactively load and save files.
//---------------------------------------------------------------------------

class MVFileDialog : public MVDialog {
protected:
    uint                flags;              // Creation flags
    MVInputLine         *fileLine;          // Input line holding filename
    MVInputLine         *driveSel;          // Input line holding drive
    MVList              *fileList;          // TList of available files
    MVList              *directoryList;     // TList of available directories
    char                directory[_MAX_DIR];// Current directory
    char                filename[_MAX_PATH];// Full absolute filename including path
    ibool               hasWilds;           // True if filename has wildcards
    FileList            filenames;          // Array of filenames
    FileList            directories;        // Array of directories

    static const char *openText;            // Text for standard buttons
    static const char *saveText;
    static const char *helpText;
    static const char *cancelText;
    static const char *typeText;
    static const char *driveText;
    static const char *fileText;
    static const char *directoryText;
    static const char *invalidDriveText;
    static const char *invalidFileText;
    static const char *tooManyFilesText;

            // Method to update the file input line
            void updateFileLine();

            // Method to change to the specified directory
            void changeDirectory();

            // Checks to see if a filename is valid
            ibool validFilename(const char *filename);

            // Method to load the current directory's filenames
            void loadFilenames();

public:
            // Constructor
            MVFileDialog(MGLDevCtx& dc,const char *defFile,const char *title,
                const char *inputName,uint flags);

            // Virtual to determine if the view was constructed correctly
    virtual ibool valid(ulong command);

            // Virtual to handle an event
    virtual void handleEvent(MVEvent& event,phaseType phase);

            // Virtual to execute the file dialog
    virtual uint execute();

            // Method to get the filename
            const char *getFilename() const { return filename; };
            void setFilename(const char *filename,ibool loadCWD = false);
    };

#endif  // __MVIS_MFILEDLG_HPP
