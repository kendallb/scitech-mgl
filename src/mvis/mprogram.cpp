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
* Description:  Member functions for the MVProgram class.
*
****************************************************************************/

#include "mvis/mvision.hpp"
#include "mvis/mprogram.hpp"
#include "mvis/msgbox.hpp"
#include <stdlib.h>
#include <iostream.h>

/*---------------------------- Global Variables ---------------------------*/

MVProgram       *MVProgram::application = NULL;
MVStatusBar     *MVProgram::statusBar = NULL;
MVMenuBar       *MVProgram::menuBar = NULL;
MVDeskTop       *MVProgram::deskTop = NULL;
void            *_MV_quickPaint = NULL;

/*----------------------------- Implementation ----------------------------*/

MVProgram::MVProgram(MGLDevCtx& dc,int systemFont,int paletteType)
    : MVGroup(dc,MVRect(0,0,0,0)),
      MVProgInit(MVProgram::initStatusBar,
                 MVProgram::initMenuBar,
                 MVProgram::initDeskTop)
/****************************************************************************
*
* Function:     MVProgram::MVProgram
* Parameters:   mglPath     - Path to MGL system files
*               systemFont  - Font family to use as system font
*
* Description:  Constructor for the MVProgram class. Takes a parameters for
*               an already initialised MGL device context to use as the
*               main user interface device context.
*
*               This is the main initialisation routine for the MegaVision
*               and handles all of the standard system resource
*               initialisation and loading for the application program.
*
****************************************************************************/
{
    state = sfVisible | sfFocused | sfActive | sfModal | sfExposed;
    options |= ofNoBackground;
    modalState = 0;
    pending.what = evNothing;

    // Load the default MegaVision system resources
    MV_loadSystemResources(dc,systemFont,paletteType);

    // Determine the dimensions for the program class
    setBounds(MVRect(0,0,dc.maxx()+1,dc.maxy()+1));

    // Adjust the size of the deskTop given the sizes of the menu bar and
    // status line.
    MVRect  extent; getExtent(extent);
    MVRect  deskExtent(extent);

    lock();
    if (createMenuBar && (menuBar = createMenuBar(dc,extent)) != NULL) {
        insert(menuBar);
        menuBar->setState(sfActive,true);
        deskExtent.top() = menuBar->getBounds().botRight.y;
        }

/*  if (createStatusLine && (statusLine = createStatusLine(extent)) != NULL) {
        insert(statusLine);
        statusLine->setState(sfActive,true);
        deskExtent.bottom() = statusLine->getBounds().top();
        }*/

    if (createDeskTop && (deskTop = createDeskTop(dc,deskExtent)) != NULL) {
        insert(deskTop);
        deskTop->setState(sfActive | sfFocused,true);
        }

    if (valid(cmValid))
        application = this;
}

MVProgram::~MVProgram()
/****************************************************************************
*
* Function:     MVProgram::~MVProgram
*
* Description:  Destructor for the MVProgram class
*
****************************************************************************/
{
//  delete statusBar;
    delete menuBar;
    delete deskTop;
    MV_fontManager.shutDown();  // Shutdown the font manager
}

ibool MVProgram::valid(ulong command)
/****************************************************************************
*
* Function:     MVProgram::valid
* Parameters:   command - Command ending the modal operation
* Returns:      True if the group is valid
*
* Description:  Checks to see that the standard objects such as the
*               status line, menu bar and desktop were created successfully,
*               and then check the items in the group.
*
****************************************************************************/
{
    if (command == cmValid) {
/*      if (createStatusBar && statusBar == NULL)
            return false;*/
        if (createMenuBar && menuBar == NULL)
            return false;
        if (createDeskTop && deskTop == NULL)
            return false;
        return true;
        }
    else if (command == cmQuit) {
        // The user is attempting to quit the program, so check to see if
        // this is valid and return true if it is.
        return exitProgram();
        }
    else return MVGroup::valid(command);
}

MVStatusBar *MVProgram::initStatusBar(MGLDevCtx&,const MVRect&)
/****************************************************************************
*
* Function:     MVProgram::initStatusBar
* Parameters:   rect    - bounding rectangle for entire program view
* Returns:      Pointer to created status line.
*
* Description:  By default we have no status line
*
****************************************************************************/
{
    return NULL;
}

MVMenuBar *MVProgram::initMenuBar(MGLDevCtx& dc,const MVRect& bounds)
/****************************************************************************
*
* Function:     MVProgram::initMenuBar
* Parameters:   bounds  - bounding rectangle for entire program view
* Returns:      Pointer to created menu bar.
*
* Description:  Set up a very simple menu bar to allow the user to quit.
*
****************************************************************************/
{
    MVMenu  *fileMenu = new MVMenu(dc);

    *fileMenu
        + new MVMenuItem("~Q~uit",cmQuit,MVHotKey(kbX,mdAlt),hcNoContext,"Alt+X");
    fileMenu->doneDefinition();

    MVMenuBar   *menuBar = new MVMenuBar(dc,bounds);

    *menuBar
        + new MVSubMenuItem("~F~ile",fileMenu);
    menuBar->doneDefinition();

    return (MVMenuBar*)MV_validView(menuBar);
}

MVDeskTop *MVProgram::initDeskTop(MGLDevCtx& dc,const MVRect& bounds)
/****************************************************************************
*
* Function:     MVProgram::initDeskTop
* Parameters:   bounds  - bounding rectangle for entire program view
* Returns:      Pointer to created desktop.
*
* Description:  By default we draw a solid desktop.
*
****************************************************************************/
{
    return (MVDeskTop*)MV_validView(new MVDeskTop(dc,bounds,
        new MVBackGround(dc,bounds)));
}

ibool MVProgram::exitProgram()
/****************************************************************************
*
* Function:     MVProgram::exitProgram
* Returns:      True if program should be terminated.
*
* Description:  When the user attempts to terminate the program, this
*               method will be called to verify the command. By default
*               it just returns true, but you should override it to
*               prompt for saving files etc.
*
****************************************************************************/
{
    return true;
}

void MVProgram::idle()
/****************************************************************************
*
* Function:     MVProgram::idle
*
* Description:  Default idle routine. Updates the status line periodically
*               and posts messages when the command set is modified.
*
****************************************************************************/
{
    MVRect  oldView;

    dc.getViewport(oldView);
    dc.setViewport(bounds);
    MVGroup::idle();
    dc.setViewport(oldView);

    // Do all the above :-)
}

void MVProgram::outOfMemory()
/****************************************************************************
*
* Function:     MVProgram::outOfMemory
*
* Description:  Default out of memory routine - pops up a dialog box to
*               this effect.
*
****************************************************************************/
{
    MV_messageBox(dc,"Out of memory: Could not complete the last operation.",
        mfError | mfOKButton | mfOKDefault);
}

void MVProgram::handleEvent(MVEvent& event,phaseType)
/****************************************************************************
*
* Function:     MVProgram::handleEvent
* Parameters:   event   - Event to handle
*               phase   - Current phase for the event (pre,focus,post)
*
* Description:  Main event handling routine for the program group. Note
*               before delegating the event down the chain of command, we
*               set the viewport to the bounds of the entire program
*               view.
*
****************************************************************************/
{
    // Check for Alt-1 to Alt-9 for window selection and post the
    // appropriate messages here
    MVGroup::handleEvent(event);
    if (event.what == evCommand &&
            (event.message.command == cmQuit ||
             event.message.command == cmRestart)) {
        endModal(event.message.command);
        clearEvent(event);
        }

    if (event.what == evKeyDown && event.key.charScan.scanCode == kbF10) {
        event.what = evCommand;
        event.message.command = cmMenu;
        event.message.infoPtr = NULL;
        putEvent(event);
        clearEvent(event);
        }
}

void MVProgram::putEvent(MVEvent& event)
/****************************************************************************
*
* Function:     MVProgram::putEvent
* Parameters:   event   - Event to post
*
* Description:  Posts an event by storing it in the pending variable.
*
****************************************************************************/
{
    memcpy(&pending,&event,sizeof(MVEvent));
}

ibool MVProgram::getEvent(MVEvent& event,ulong mask)
/****************************************************************************
*
* Function:     MVProgram::getEvent
* Parameters:   event   - Place to store the event
* Returns:      True if an event was pending
*
* Description:  Gets the next pending event, looking in the event queue
*               if there are no pending messages.
*
****************************************************************************/
{
    // Check for repaint events first, and handle if one is found.
    if (MV_eventQueue.getNext(event,evRepaint | evRepaintOS)) {
        if (event.what == evRepaintOS)
            MVProgram::application->quickRepaint();
        doRepaint();
        clearEvent(event);
        return false;
        }

    if (mask == evRepaint)
        return false;

    if (pending.what != evNothing) {
        memcpy(&event,&pending,sizeof(MVEvent));
        pending.what = evNothing;
        }
    else {
        if (!MV_eventQueue.getNext(event,mask))
            idle();
        }

    if (statusBar) {
        // Handle the special case of global status line events in here
        }

    return (event.what != evNothing);
}

ibool MVProgram::peekEvent(MVEvent& event,ulong mask)
/****************************************************************************
*
* Function:     MVProgram::peekEvent
* Parameters:   event   - Place to store the event
* Returns:      True if an event is pending
*
* Description:
*
****************************************************************************/
{
    if (pending.what != evNothing)
        memcpy(&event,&pending,sizeof(MVEvent));
    else
        MV_eventQueue.peekNext(event,mask & ~evRepaint);
    return (event.what != evNothing);
}

void MVProgram::eventError(MVEvent&)
/****************************************************************************
*
* Function:     MVProgram::eventError
* Parameters:   event   - Event causing the error
*
* Description:  End of the line for missing event routines. By default we
*               do nothing, but you can override this to bring up a
*               dialog box etc.
*
*               Maybe this could bring up a box by default.
*
****************************************************************************/
{
}

void MVProgram::invalidRect(MVRect& rect)
/****************************************************************************
*
* Function:     MVProgram::invalidRect
* Parameters:   rect    - Rectangle to invalidate.
*               global  - True if repaint event should be handled globally
*
* Description:  Invalidates the specified rectangle, requiring it to be
*               refreshed. This is the MVProgram view, so we save the invalid
*               rectangle for the pending repaint event (posted by
*               the MVProgram object).
*
****************************************************************************/
{
    if (state & sfLockRepaint)
        return;

    // Clip to bounding rectangle for entire program
    rect &= bounds;
    if (invalid.isEmpty()) {
        // This is the first invalidRect call, so post an evRepaint message.
        MVEvent event;
        event.what = evRepaint;
        MV_eventQueue.post(event);
        }

    // Combine the update rectangle with the current invalid region.
    invalid += rect;
}

void MVProgram::validRect(MVRect& rect)
/****************************************************************************
*
* Function:     MVProgram::validRect
* Parameters:   rect    - Rectangle to invalidate.
*
* Description:  Validates the specified rectangle, removing it from the
*               pending repaint event. This is the MVProgram view, so we
*               remove the valid rectangle from the pending repaint event.
*
****************************************************************************/
{
    if (state & sfLockRepaint)
        return;

    // Clip to bounding rectangle for entire program
    rect &= bounds;

    // Remove the rectangle from the invalid region
    invalid -= rect;
    if (invalid.isEmpty()) {
        // The invalid region is now empty, so remove the repaint event
        MVEvent event;
        MV_eventQueue.getNext(event,evRepaint);
        invalid.empty();
        }
}

void MVProgram::quickRepaint()
/****************************************************************************
*
* Function:     MVProgram::quickRepaint
*
* Description:  Performs a quick repaint on the entire program display,
*               which can be used to refresh the display quickly if the
*               entire display image is invalidated.
*
****************************************************************************/
{
    _MV_quickPaint = application;
    application->repaint();
}

void MVProgram::doRepaint()
/****************************************************************************
*
* Function:     MVProgram::doRepaint
*
* Description:  Performs a repaint operation for the entire program view.
*               We perform a series of draw operations, clipping to each
*               of the indivdual clipping rectangles in the current update
*               region. If the region is complex this can be expensive, but
*               in most cases it will consist of only a small number of
*               update rectangles.
*
****************************************************************************/
{
    MS_obscure();
    clipRgn = invalid;
    invalid.empty();
    clipRgn.optimize();
    MV_pushViewport(dc);
    dc.setViewport(bounds);
    paint();
    MV_popViewport(dc);
    MS_show();

    // All subsequent paints are not quick paints
    _MV_quickPaint = NULL;
}

static int _ASMAPI MVProgram_doSuspendApp(MGLDC*,int flags)
/****************************************************************************
*
* Function:     MVProgram_doSuspendApp
* Parameters:   flags   - Suspend application flags sent from the MGL
*
* Description:  Handles suspend application calls for the application. All
*               we do is simply repaint the entire display after we get back
*               as the MGL takes care of the rest.
*
****************************************************************************/
{
    if (flags & MGL_REACTIVATE) {
        // If we are re-activating the application, simply perform a
        // quick repaint for the entire application
        MVEvent evt;
        MVProgram::application->quickRepaint();
        MVProgram::application->getEvent(evt,evRepaint);
        }
    return MGL_SUSPEND_APP;
}

uint MVProgram::run()
/****************************************************************************
*
* Function:     MVProgram::run
*
* Description:  Runs the program until completion.
*
****************************************************************************/
{
    if (valid(cmValid)) {
        MGL_setSuspendAppCallback(MVProgram_doSuspendApp);
        select(deskTop);            // Select the desktop
        MS_show();                  // Show the mouse cursor
        unlock();                   // Unlock repaint events
        _MV_quickPaint = this;      // Use full quick paint for initial draw
        uint retval = execute();    // Begin processing events
        MS_hide();
        return retval;
        }
    else {
        MGL_fatalError("Not enough memory to start program");
        return cmQuit;
        }
}

void MVProgram::suspend()
/****************************************************************************
*
* Function:     MVProgram::suspend
*
* Description:  Suspend execution of the program to allow shelling to DOS
*               etc.
*
****************************************************************************/
{
#ifdef  MGLDOS
    // systemError.syspend();
    MGL_suspend();
#endif
}

void MVProgram::resume()
/****************************************************************************
*
* Function:     MVProgram::resume
*
* Description:  Resumes execution of the program after shelling to DOS etc.
*
****************************************************************************/
{
#ifdef  MGLDOS
    MGL_resume();
    // systemError.resume();
    MS_show();
#endif
}

