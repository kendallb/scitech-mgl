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
* Description:  Member functions for the MVFileDialog class.
*
****************************************************************************/

#include "mvis/mvision.hpp"
#include "mvis/mfiledlg.hpp"
#include "mvis/mfontmgr.hpp"
#include "mvis/msttext.hpp"
#include "mvis/mbutton.hpp"
#include "mvis/mkeys.hpp"
#include "mvis/msgbox.hpp"
#include "pmapi.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef __WINDOWS32__
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
#ifdef __OS2_32__
#define INCL_DOSFILEMGR
#define INCL_DOSERRORS
#include <os2.h>
#endif

/*----------------------------- Implementation ----------------------------*/

#define LINESPERLIST    6

MVFileDialog::MVFileDialog(MGLDevCtx& dc,const char *defFile,
    const char *title,const char *inputName,uint flags)
    : MVDialog(dc,MVRect(0,0,100,100),title),
      MVWindowInit(MVWindow::initFrame,MVWindow::initTitleBar),
      flags(flags), filenames(5,5), directories(5,5)
/****************************************************************************
*
* Function:     MVFileDialog::MVFileDialog
* Parameters:   defFile     - Text to put up as the default file
*               title       - Title for the dialog box
*               inputName   - Name for the filename input box
*               flags       - Creation flags for the dialog box
*
* Description:  Constructor for the MVFileDialog.
*
****************************************************************************/
{
    MVLabel *label,*label2;

    options |= ofCentered;

    // Find the height of the system font, and compute the location of
    // all the elements of the input dialog.
    useFont(fmSystemFont);
    metrics_t m;
    dc.getFontMetrics(m);
    int height = dc.textHeight();
    int cellHeight = m.ascent - m.descent + 1 + 4;
    int buttonHeight = height + 12;
    int dialogWidth = (dc.sizex()*2)/3;
    int linesPerList = LINESPERLIST;

    if (dc.sizey() < 349)
        linesPerList /= 2;

    MVRect r(clientRect);
    int adjustBottom = bounds.bottom() - r.bottom();

    // Create the filename input line and label
    r.top() += height;
    r.bottom() = r.top() + height;
    r.left() = 15;
    r.right() = 15 + dc.textWidth(inputName);
    insert(label = new MVLabel(dc,r,inputName,NULL));

    r.top() = r.bottom();
    r.bottom() = r.top() + height;
    r.right() = dialogWidth-15;
    fileLine = new MVInputLine(dc,r,PM_MAX_PATH);
    label->setAttached(fileLine);

    // Create the file type and drive pick lists and labels
    r = fileLine->getBounds();
    MVRect r2(r);
    r.right() = dialogWidth/2 - 5;
    r2.left() = dialogWidth/2 + 5;
    r2.top() = r.top() = r.bottom() + 8;
    r2.bottom() = r.bottom() = r.top() + height;
    insert(label = new MVLabel(dc,r,typeText,NULL));
    insert(label2  = new MVLabel(dc,r2,driveText,NULL));

    r2.top() = r.top() = r.bottom();
    r2.bottom() = r.bottom() = r.top() + height;
    MVInputLine *fileType = new MVInputLine(dc,r,11,"<All Files>");
    fileType->setState(sfDisabled,true);        // Disable for now :-)
    driveSel = new MVInputLine(dc,r2,PM_MAX_DRIVE,NULL);
    driveSel->setState(sfDisabled,true);
    label->setAttached(fileType);
    label2->setAttached(driveSel);

    // Create the file and directory list boxes

    r = fileType->getBounds();
    r2 = driveSel->getBounds();
    r2.top() = r.top() = r.bottom() + 8;
    r2.bottom() = r.bottom() = r.top() + height;
    insert(label = new MVLabel(dc,r,fileText,NULL));
    insert(label2  = new MVLabel(dc,r2,directoryText,NULL));

    r2.top() = r.top() = r.bottom();
    r2.bottom() = r.bottom() = r.top() + cellHeight*linesPerList + 4;
    r.right() -= _MV_sysScrollBarWidth;
    r2.right() -= _MV_sysScrollBarWidth;
    fileList = new MVList(dc,r,MVPoint(r.right()-r.left()-4,cellHeight));
    directoryList = new MVList(dc,r2,MVPoint(r2.right()-r2.left()-4,cellHeight));
    label->setAttached(fileList);
    label2->setAttached(directoryList);

    // Now create the scroll bars for the lists and attach them to the
    // appropriate lists.
    MVRect  r3(r),r4(r2);
    r3.left() = r3.right()-1;
    r3.right() += _MV_sysScrollBarWidth;
    r3.bottom() -= 1;
    r4.left() = r4.right()-1;
    r4.right() += _MV_sysScrollBarWidth;
    r4.bottom() -= 1;
    MVScrollBar *vScroll = new MVScrollBar(dc,r3);
    MVScrollBar *vScroll2 = new MVScrollBar(dc,r4);

    r.top() = r.bottom()-1;
    r.bottom() = r.top() + _MV_sysScrollBarWidth;
    r.right() -= 1;
    r2.top() = r2.bottom()-1;
    r2.bottom() = r2.top() + _MV_sysScrollBarWidth;
    r2.right() -= 1;
    MVScrollBar *hScroll = new MVScrollBar(dc,r);
    MVScrollBar *hScroll2 = new MVScrollBar(dc,r2);

    fileList->setHScroll(hScroll);
    fileList->setVScroll(vScroll);
    directoryList->setHScroll(hScroll2);
    directoryList->setVScroll(vScroll2);

    // Adjust the bounds of the box to fit, including the buttons
    bounds.right() = dialogWidth;
    bounds.bottom() = r.bottom() + adjustBottom + buttonHeight + 20;
    setBounds(bounds);

    // Now add the buttons to the dialog box
    int width = dc.textWidth(cancelText) + 16;
    int sizex = width + 15;
    r.top() = bounds.bottom() - buttonHeight - 10;
    r.bottom() = r.top() + buttonHeight;
    r.right() =  r.left() + width;
    MVButton *okBut = (flags & fdOpenButton) ?
        new MVButton(dc,r,openText,cmFileOpen,bfDefault) :
        new MVButton(dc,r,saveText,cmFileSave,bfDefault);

    r.left() += sizex;  r.right() +=  sizex;
    MVButton *cancelBut = new MVButton(dc,r,cancelText,cmCancel,bfNormal);
    MVButton *helpBut = NULL;

    if (flags & fdHelpButton) {
        r.left() += sizex;  r.right() +=  sizex;
        helpBut = new MVButton(dc,r,helpText,cmHelp,bfNormal);
        }

    // Now insert all of the items into the dialog box in the correct
    // tab-ing order
    if (helpBut)
        insert(helpBut);
    insert(cancelBut);
    insert(okBut);
    insert(directoryList);
    insert(fileList);
    insert(driveSel);
    insert(fileType);
    insert(fileLine);

    // Now insert the scroll bars on top of everything else
    insert(vScroll);    insert(vScroll2);
    insert(hScroll);    insert(hScroll2);

    // Now set the default filename for the dialog box
    directory[0] = filename[0] = '\0';
    setFilename(defFile,true);
}

void MVFileDialog::updateFileLine()
/****************************************************************************
*
* Function:     MVFileDialog::updateFileLine
*
* Description:  Updates the value in the file input line, with the
*               filename currently selected in the file list box.
*
****************************************************************************/
{
    MVPoint     cursor;
    const char  *name;

    fileList->getCurrentCell(cursor);
    fileList->getCell(cursor.x,cursor.y,name);
    setFilename(name);
}

void MVFileDialog::changeDirectory()
/****************************************************************************
*
* Function:     MVFileDialog::changeDirectory
*
* Description:  Changes to the directory specified by the directory list
*               box, reads in the new filenames for the directory.
*
****************************************************************************/
{
    MVPoint     cursor;
    const char  *dir;
    char        path[PM_MAX_PATH];

    directoryList->getCurrentCell(cursor);
    directoryList->getCell(cursor.x,cursor.y,dir);

    // Build the name for the new directory to change to

    strcpy(path,driveSel->getText());
    strcat(path,directory);
    if (dir[0] == '.' && dir[1] == '.') {
        // Go back one directory. We do this by removing the last directory
        // entry from the path that we have built so far
        for (int index = strlen(path)-2; index >= 2; index--)
            if (path[index] == '\\' || path[index] == '/') {
                path[index+1] = '\0';
                break;
                }
        }
    else {
        strcat(path,dir);
        strcat(path,"/");
        }
    strcat(path,fileLine->getText());
    setFilename(path);
    loadFilenames();
}

ibool MVFileDialog::valid(ulong command)
/****************************************************************************
*
* Function:     MVFileDialog::valid
* Parameters:   command - Command causing the dialog to complete
* Returns:      True if the dialog is valid, false if not
*
* Description:  Determines if the value in the file input box is a valid
*               filename or not. We will not allow the user to quit
*               correctly unless the filename is valid.
*
****************************************************************************/
{
    if (command == cmValid || command == cmCancel)
        return true;

    if (MVDialog::valid(command)) {
        if (directoryList->getState() & sfFocused)
            changeDirectory();
        else if (fileList->getState() & sfFocused) {
            updateFileLine();
            return !hasWilds;
            }
        else {
            if (validFilename(fileLine->getText())) {
                setFilename(fileLine->getText());
                if (hasWilds) {
                    loadFilenames();
                    fileList->select();
                    }
                else
                    return true;
                }
            }
        }
    return false;
}

void MVFileDialog::handleEvent(MVEvent& event,phaseType phase)
/****************************************************************************
*
* Function:     MVFileDialog::handleEvent
* Parameters:   event   - Event to handle
*               phase   - Current phase for the event (pre,focus,post)
*
* Description:  Event handling routine for the MVFileDialog class. Attempts
*               to end the modal operation if the open or save buttons
*               are hit.
*
****************************************************************************/
{
    MVDialog::handleEvent(event,phase);

    switch (event.what) {
        case evCommand:
            switch (event.message.command) {
                case cmFileOpen:
                case cmFileSave:
                    endModal(event.message.command);
                    clearEvent(event);
                    break;
                case cmHelp:
                    MV_messageBox(dc,"Help not implemented yet",
                        mfInformation | mfOKButton | mfOKDefault);
                    clearEvent(event);
                    break;
                }
            break;
        case evBroadcast:
            switch (event.message.command) {
                case cmListCursorChanged:
                    if (event.message.infoPtr == fileList) {
                        updateFileLine();
                        clearEvent(event);
                        }
                    break;
                case cmListItemSelected:
                    if (event.message.infoPtr == fileList) {
                        endModal((flags & fdOpenButton) ?
                            cmFileOpen : cmFileSave);
                        clearEvent(event);
                        }
                    else if (event.message.infoPtr == directoryList) {
                        changeDirectory();
                        clearEvent(event);
                        }
                    break;
                }
            break;
        }
}

uint MVFileDialog::execute()
/****************************************************************************
*
* Function:     MVFileDialog::execute
* Returns:      Command ending the execution.
*
* Description:  We overload the execute method to automatically set things
*               up for us. If the contents of the current directory are
*               not loaded, we load them first.
*
****************************************************************************/
{
    if (!(flags & fdDirLoaded))
        loadFilenames();
    select(fileLine);               // Focus on the file input line
    return MVDialog::execute();
}

static ibool isrootdirectory(const char *dir)
/****************************************************************************
*
* Function:     isrootdirectory
* Parameters:   dir - Name of the file to check
* Returns:      True if a root directory
*
****************************************************************************/
{
#ifdef  __UNIX__
    if (dir[0] == '\0' || (strlen(dir) == 1 && dir[0] == '/'))
        return true;
#else
    if (strlen(dir) == 3 && dir[1] == ':' && (dir[2] == '\\' || dir[2] == '/'))
        return true;
    if (strlen(dir) == 1 && (dir[2] == '\\' || dir[2] == '/'))
        return true;
#endif
    return false;
}

static ibool loadfiles(const char *filename,FileList& fileList)
/****************************************************************************
*
* Function:     loadfiles
* Parameters:   filename    - Name of files to look for
*               fileList    - Place to store the filenames
* Returns:      True on success, false on memory error
*
* Description:  Loads a list of all the filenames from the current
*               directory into the specified name list. Does not include
*               any subdirectories in the list.
*
****************************************************************************/
{
    PM_findData findData;
    void        *hfile;
    ibool       valid;

    findData.dwSize = sizeof(findData);
    valid = (hfile = PM_findFirstFile(filename,&findData)) != NULL;
    while (valid) {
        if (!(findData.attrib & PM_FILE_DIRECTORY)) {
            fileList.add(new TCDynStr(findData.name));
            if (MV_lowMemory())
                return false;
            }
        valid = PM_findNextFile(hfile,&findData);
        }
    if (hfile)
        PM_findClose(hfile);
    return true;
}

static ibool loaddirectories(const char *filename,FileList& fileList)
/****************************************************************************
*
* Function:     loaddirectories
* Parameters:   filename    - Name of directories to look for
*               fileList    - Place to store the filenames
* Returns:      True on success, false on memory error
*
* Description:  Loads a list of all the directories from the current
*               directory into the specified name list.
*
****************************************************************************/
{
    PM_findData findData;
    void        *hfile;
    ibool       valid;

    findData.dwSize = sizeof(findData);
    valid = (hfile = PM_findFirstFile(filename,&findData)) != NULL;
    while (valid) {
        if ((findData.attrib & PM_FILE_DIRECTORY) && findData.name[0] != '.') {
            fileList.add(new TCDynStr(findData.name));
            if (MV_lowMemory())
                return false;
            }
        valid = PM_findNextFile(hfile,&findData);
        }
    if (hfile)
        PM_findClose(hfile);
    return true;
}

static ibool validfilename(const char *path)
/****************************************************************************
*
* Function:     validfilename
* Parameters:   path    - Name of the file to check
* Returns:      True if a valid file, false if non existant or a directory
*
* Description:  Checks if the specified file is a valid filename.
*
****************************************************************************/
{
    PM_findData findData;
    void        *hfile;
    ibool       valid = false;

    findData.dwSize = sizeof(findData);
    if ((hfile = PM_findFirstFile(path,&findData)) != NULL) {
        if (!(findData.attrib & PM_FILE_DIRECTORY))
            valid = true;
        }
    if (hfile)
        PM_findClose(hfile);
    return valid;
}

static ibool issubdirectory(const char *dir)
/****************************************************************************
*
* Function:     issubdirectory
* Parameters:   dir - Name of the file to check
* Returns:      True if a subdirectory, false if a normal file
*
* Description:  Checks if the specified file is a subdirectory or a normal
*               file.
*
****************************************************************************/
{
    PM_findData findData;
    void        *hfile;
    ibool       isdir = false;
    char        path[PM_MAX_PATH];

    findData.dwSize = sizeof(findData);
    strcpy(path,dir);
    if (path[strlen(path)-1] == '\\')
        path[strlen(path)-1] = 0;
    if ((hfile = PM_findFirstFile(path,&findData)) != NULL) {
        if (findData.attrib & PM_FILE_DIRECTORY)
            isdir = true;
        }
    if (hfile)
        PM_findClose(hfile);
    if (isrootdirectory(dir))
        return true;
    return isdir;
}

void MVFileDialog::loadFilenames()
/****************************************************************************
*
* Function:     MVFileDialog::loadFilenames
*
* Description:  Loads the dialog box with all the filenames in the
*               current directory.
*
****************************************************************************/
{
    char    buf[PM_MAX_PATH];
    uint    j;
    int     i;
    ibool   subDir = (strlen(directory) != 1) && !isrootdirectory(filename);

    if (flags & fdDirLoaded)    // Don't reload if already loaded
        return;

    filenames.empty();          // Empty filename and directory collections
    directories.empty();

    // Load all of the filenames for the current directory

    if (!loadfiles(filename,filenames))
        goto memError;
#ifdef  __UNIX__
    PM_makepath(buf,(char*)driveSel->getText(),directory,"*",NULL);
#else
    PM_makepath(buf,(char*)driveSel->getText(),directory,"*",".*");
#endif
    if (!loaddirectories(buf,directories))
        goto memError;

    // Sort all of the filenames and directories, and insert the data
    // into the appropriate list boxes.

    filenames.sort();
    directories.sort();

    fileList->setDataBounds(MVRect(0,0,1,filenames.numberOfItems()));
    for (j = 0; j < filenames.numberOfItems(); j++)
        fileList->setCell(0,j,*filenames[j]);
    fileList->setHRange(0,0);
    i = filenames.numberOfItems()-1;
    fileList->setVRange(0,MAX(0,i));

    directoryList->setDataBounds(MVRect(0,0,1,directories.numberOfItems()+subDir));
    if (subDir)
        directoryList->setCell(0,0,"..");
    for (j = 0; j < directories.numberOfItems(); j++)
        directoryList->setCell(0,j+subDir,*directories[j]);
    directoryList->setHRange(0,0);
    directoryList->setVRange(0,directories.numberOfItems()-1+subDir);

    fileList->clearSelection();
    fileList->selectCell(0,0);
    fileList->setCurrentCell(0,0);
    directoryList->clearSelection();
    directoryList->selectCell(0,0);
    directoryList->setCurrentCell(0,0);

    // Update the file and directory lists

    fileList->update();
    directoryList->update();

    flags |= fdDirLoaded;       // Flag that directory is now loaded
    return;

memError:
    // Flag that a memory error occurred when loading filenames

    filenames.empty();
    directories.empty();
    MV_messageBox(dc,tooManyFilesText,mfError | mfOKButton | mfOKDefault);
    endModal(cmCancel);
}

ibool hasWildcards(const char *p)
/****************************************************************************
*
* Function:     hasWildcards
* Parameters:   p   - Filename to check for wildcards
* Returns:      True if the filename has wildcards.
*
****************************************************************************/
{
    int i = strlen(p);

    while (i--) {
        if (*p == '*' || *p == '?')
            return true;
        p++;
        }
    return false;
}

#ifndef __UNIX__
/****************************************************************************
REMARKS:
Function to determine if a drive is a valid drive or not. Under Unix this
function will return false for anything except a value of 3 (considered
the root drive, and equivalent to C: for non-Unix systems). The drive
numbering is:

    1   - Drive A:
    2   - Drive B:
    3   - Drive C:
    etc

****************************************************************************/
static ibool PMAPI driveValid(
    char drive)
{
#ifdef __WINDOWS32__
    char    buf[5];
    int     type;

    sprintf(buf,"%c:\\", drive);
    return ((type = GetDriveType(buf)) != 0 && type != 1);
#elif defined(__OS2_32__)
    ulong   cntDisk,cntDriveMap;
    ibool   valid;

    DosQueryCurrentDisk(&cntDisk,&cntDriveMap);
    valid = (DosSetDefaultDisk(drive - 'A' + 1) == NO_ERROR);
    DosSetDefaultDisk(cntDisk);
    return valid;
#else
    RMREGS  regs;
    regs.h.dl = (uchar)(drive - 'A' + 1);
    regs.h.ah = 0x36;               // Get disk information service
    PM_int86(0x21,&regs,&regs);
    return regs.x.ax != 0xFFFF;     // AX = 0xFFFF if disk is invalid
#endif
}
#endif

ibool MVFileDialog::validFilename(const char *filename)
/****************************************************************************
*
* Function:     MVFileDialog::validFilename
* Parameters:   filename    - Filename to check for validity
* Returns:      True if the filename is valid.
*
* Description:  To determine this, we first check if the directory is
*               valid. If the 'fdMustExist' flag is set, then we also check
*               to ensure that the file selected also exists. If any errors
*               occur, we pop up a dialog box to that effect and return
*               false.
*
****************************************************************************/
{
    char    path[PM_MAX_PATH];
    char    drive[PM_MAX_DRIVE];
    char    dir[PM_MAX_PATH];
    char    name[PM_MAX_PATH];
    char    ext[PM_MAX_PATH];
    ibool   invalid;

    PM_splitpath((char*)filename,drive,dir,name,ext);

    if (drive[0] == 0) {
        strcpy(drive,driveSel->getText());
        if (dir[0] == 0)
            strcpy(dir,directory);
        }

    // Check to see that the drive/directory exists

    strcpy(path,drive);     strcat(path,dir);
    int len = strlen(path);
    if (path[len-1] == '\\' || path[len-1] == '/')
        path[--len] = '\0';
#ifndef __UNIX__
    if (path[len-1] == ':')
        invalid = !driveValid((char)toupper(path[len-2]));
    else
#endif
        invalid = !issubdirectory(path) && !isrootdirectory(path);
    if (invalid) {
        MV_messageBox(dc,invalidDriveText,mfError | mfOKButton | mfOKDefault);
        fileLine->select();
        return false;
        }

    // Check that the file exists if required. If the filename has wildcards,
    // then it is valid.

    if (!hasWildcards(filename)) {
        strcat(path,"/");
        if (MVFileDialog::flags & fdMustExist) {
            // Check that the file exists or is a subdirectory
            strcat(path,name);
            strcat(path,ext);
            if (!validfilename(path) && !issubdirectory(path)) {
                MV_messageBox(dc,invalidFileText,mfError | mfOKButton | mfOKDefault);
                fileLine->select();
                fileLine->setText(fileLine->getText());
                return false;
                }
            }
        else {
            // First check to see that the path to the file exists
            if (!issubdirectory(path)) {
                MV_messageBox(dc,invalidFileText,mfError | mfOKButton | mfOKDefault);
                fileLine->select();
                fileLine->setText(fileLine->getText());
                return false;
                }
            }
        }

    return true;
}

#ifdef  __UNIX__

void MVFileDialog::setFilename(const char *filename, ibool loadCWD)
/****************************************************************************
*
* Function:     MVFileDialog::setFilename
* Parameters:   filename    - New filename.
*               loadCWD     - True if we should load the CWD
*
* Description:  Sets the current filename for the dialog box to 'name'. We
*               split up the name into it's component parts, storing the
*               file and extension information in the file input line,
*               and the directory information in the directory string.
*
*               If 'loadCWD' is true, and no directory is specified in the
*               filename, the current working directory will be loaded
*               and used to build the full pathname, otherwise the already
*               loaded values will be used.
*
****************************************************************************/
{
    char    *name;
    char    cwd[PM_MAX_PATH + PM_MAX_PATH];
    int i, len = strlen(filename);

    strcpy(cwd, filename);
    for (i = len - 1; i >= 0; i--) {
        if (filename[i] == '/') {
            cwd[i] = '\0';
            break;
            }
        }
    name = (char *)&filename[i + 1];

    if (i < 0) {
        if (loadCWD)
            PM_getCurrentPath(directory,PM_MAX_PATH);
        strcpy(cwd, directory);
        }
    else
        strcpy(directory, cwd);

    if (cwd[strlen(directory)-1] != '/')
        strcat(directory,"/");

    if (cwd[strlen(cwd)-1] != '/')
        strcat(cwd,"/");

    strcat(cwd, name[0] == '\0' ? "*" : name);

    hasWilds = hasWildcards(cwd);

    // Now set the filename and extension of the file input line.
    fileLine->setText(name[0] == '\0' ? "*" : name);

    // Save the complete filename in the filename variable. If the name
    // has changed, then flag that the directory contents will need to be
    // reloaded.

    if (strcmp(cwd,MVFileDialog::filename) != 0) {
        MVFileDialog::flags &= ~fdDirLoaded;
        strcpy(MVFileDialog::filename,cwd);
        }

    if (!hasWilds && issubdirectory(cwd)) {
        // The selected file is actually a directory, so load all the
        // files in this directory
        if (cwd[strlen(cwd)-1] == '/')
            strcat(cwd,"*");
        else
            strcat(cwd,"/*");
        setFilename(cwd);
        }
}

#else   /* !__UNIX__ */

void MVFileDialog::setFilename(const char *filename,ibool loadCWD)
/****************************************************************************
*
* Function:     MVFileDialog::setFilename
* Parameters:   filename    - New filename.
*               loadCWD     - True if we should load the CWD
*
* Description:  Sets the current filename for the dialog box to 'name'. We
*               split up the name into it's component parts, storing the
*               file and extension information in the file input line,
*               and the directory information in the directory string.
*
*               If 'loadCWD' is true, and no directory is specified in the
*               filename, the current working directory will be loaded
*               and used to build the full pathname, otherwise the already
*               loaded values will be used.
*
****************************************************************************/
{
    char    cwd[PM_MAX_PATH];
    char    cwd_drive[PM_MAX_DRIVE];
    char    cwd_dir[PM_MAX_PATH];
    char    drive[PM_MAX_DRIVE];
    char    name[PM_MAX_PATH];
    char    ext[PM_MAX_PATH];

    hasWilds = false;
    PM_splitpath((char*)filename,cwd_drive,cwd_dir,name,ext);

    if (cwd_drive[0] != 0) {
        if (strcmp(cwd_drive,drive) != 0) {
            // Changing drives, so load new current directory for drive
            directory[0] = 0;
            }
        strcpy(drive,cwd_drive);
        loadCWD = true;
        }
    else
        strcpy(drive,driveSel->getText());
    if (cwd_dir[0] != 0)
        strcpy(directory,cwd_dir);

    if (loadCWD && (drive[0] == 0 || directory[0] == 0)) {
        if (drive[0] == 0)
            PM_getCurrentPath(cwd,PM_MAX_PATH);
        else
            PM_getdcwd(toupper(drive[0])-'A'+1,cwd,PM_MAX_PATH);

        // _PM_getdcwd will not append the / character to the path if a
        // directory component is present. If the directory is the root
        // directory, it will store the backslash.

        int len = strlen(cwd);
        if (cwd[len-1] == '\\' || cwd[len-1] == '/')
            cwd[--len] = '\0';
        strcat(cwd,"\\*.*");
        PM_splitpath(cwd,cwd_drive,cwd_dir,NULL,NULL);
        if (drive[0] == 0)
            strcpy(drive,cwd_drive);
        if (directory[0] == 0) {
            strcpy(drive,cwd_drive);
            strcpy(directory,cwd_dir);
            }
        }

    if (filename[0] == 0) {
        name[0] = '*';                  // Insert default extension
        name[1] = '\0';
        ext[0] = '.';
        ext[1] = '*';
        ext[2] = '\0';
        }
    hasWilds = hasWildcards(filename);

    // Now set the filename and extension of the file input line.

    strcpy(cwd,name);
    strcat(cwd,ext);
    fileLine->setText(cwd);
    driveSel->setText(drive);

    // Save the complete filename in the filename variable. If the name
    // has changed, then flag that the directory contents will need to be
    // reloaded.

    PM_makepath(cwd,drive,directory,name,ext);
    if (stricmp(cwd,MVFileDialog::filename) != 0) {
        MVFileDialog::flags &= ~fdDirLoaded;
        strcpy(MVFileDialog::filename,cwd);
        }

    if (!hasWilds && issubdirectory(cwd)) {
        // The selected file is actually a directory, so load all the
        // files in this directory
        if (cwd[strlen(cwd)-1] == '\\')
            strcat(cwd,"*.*");
        else
            strcat(cwd,"\\*.*");
        setFilename(cwd);
        }
}

#endif  /* !__UNIX__ */
