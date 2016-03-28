/****************************************************************************
*
*                   SciTech Multi-platform Graphics Library
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
* Environment:  any
*
* Description:  Implementation for the demo program.
*
****************************************************************************/

#include "demo.hpp"
#include "mvis/msgbox.hpp"
#include "mvis/mmodesel.hpp"
#include "explwind.hpp"
#include "titlwind.hpp"
#include "demowind.hpp"

/*------------------------------ Implementation ---------------------------*/

Demo::Demo(MGLDevCtx& dc)
      : MVProgram(dc),
        MVProgInit(NULL,Demo::initMenuBar,Demo::initDeskTop)
/****************************************************************************
*
* Function:     Demo::Demo
*
* Description:  Constructor for the demo application.
*
****************************************************************************/
{
    // Set up the default windows on the desktop
    MVRect  extent,r;

    deskTop->getExtent(extent);
    ExplanationWindow *ew = new ExplanationWindow(dc,extent);
    TitleWindow *tw = new TitleWindow(dc,extent);

    r.left() = 0;
    r.right() = extent.right();
    r.top() = tw->getBounds().bottom();
    r.bottom() = ew->getBounds().top();
    DemoWindow *dw = new DemoWindow(dc,r);

    deskTop->insert(tw);
    deskTop->insert(ew);
    deskTop->insert(dw);
}

MVMenuBar* Demo::initMenuBar(MGLDevCtx& dc,const MVRect& bounds)
/****************************************************************************
*
* Function:     Demo::initMenuBar
* Parameters:   bounds  - Bounding box for the entire application
* Returns:      Pointer to the newly created menu bar.
*
* Description:  Creates the menu bar definition for the application.
*
****************************************************************************/
{
    MVMenu *fileMenu = new MVMenu(dc);
    MVMenu *demoMenu = new MVMenu(dc);
    MVMenu *optionsMenu = new MVMenu(dc);

    *fileMenu
        + new MVMenuItem("~A~bout this demo...",cmAbout)
        + new MVMenuItemSeparator()
        + new MVMenuItem("~Q~uit",cmQuit,MVHotKey(kbX,mdAlt),hcNoContext,"Alt+X");
    fileMenu->doneDefinition();

    *demoMenu
        + new MVMenuItem("Status Info",cmStatusDemo)
        + new MVMenuItem("Lines",cmLineDemo)
        + new MVMenuItem("Ellipses",cmEllipseDemo)
        + new MVMenuItem("Elliptical Arcs",cmArcDemo)
        + new MVMenuItem("Rectangles",cmRectangleDemo)
        + new MVMenuItem("Polygons",cmPolygonDemo)
        + new MVMenuItem("Color control",cmColorDemo)
        + new MVMenuItem("Patterns",cmPatternDemo)
        + new MVMenuItem("Animation",cmAnimationDemo);
    demoMenu->doneDefinition();

    *optionsMenu
        + new MVMenuItem("~V~ideo mode...",cmVideoMode);
    optionsMenu->doneDefinition();

    MVMenuBar   *menuBar = new MVMenuBar(dc,bounds);

    *menuBar
        + new MVSubMenuItem("~F~ile",fileMenu)
        + new MVSubMenuItem("~D~emo",demoMenu)
        + new MVSubMenuItem("~O~ptions",optionsMenu);
    menuBar->doneDefinition();

    return (MVMenuBar*)MV_validView(menuBar);
}

MVDeskTop *Demo::initDeskTop(MGLDevCtx& dc,const MVRect& bounds)
/****************************************************************************
*
* Function:     Demo::initDeskTop
* Parameters:   bounds  - bounding rectangle for entire program view
* Returns:      Pointer to created desktop.
*
* Description:  For this application, the entire desktop is always
*               covered by the application windows, so the desktop is
*               created with an empty background to save time during
*               redraws.
*
****************************************************************************/
{
    return (MVDeskTop*)MV_validView(new MVDeskTop(dc,bounds,NULL));
}

void Demo::handleEvent(MVEvent& event,phaseType phase)
/****************************************************************************
*
* Function:     Demo::handleEvent
* Parameters:   event   - Event to handle
*               phase   - Phase of focused events
*
* Description:  Main event handling routine for the application. We first
*               let the TProgram class take care of the events for us, then
*               we check for application specific commands.
*
****************************************************************************/
{
    MVProgram::handleEvent(event,phase);

    // Now check for application commands and dispatch them
    if (event.what == evCommand) {
        switch (event.message.command) {
            case cmBeginEvents:
                event.what = evBroadcast;
                event.message.command = cmStatusDemo;
                putEvent(event);
                break;
            case cmAbout:
                MV_messageBox(dc,
                    "SciTech Multi-Platform Graphics Library Demo\n\n"
                    "Version " MGL_VERSION_STR "\n\n"
                    "Copyright (c) 1992-99 SciTech Software, Inc.\n"
                    "Written by Kendall Bennett"
                    ,mfInformation | mfOKButton | mfOKDefault | mfCenterText);
                break;
            case cmVideoMode:
                changeVideoMode();
                break;
            case cmStatusDemo:
            case cmLineDemo:
            case cmEllipseDemo:
            case cmArcDemo:
            case cmPolygonDemo:
            case cmRectangleDemo:
            case cmColorDemo:
            case cmPatternDemo:
            case cmAnimationDemo:
                // Broadcast these messages to be handled by the DemoArea.
                event.what = evBroadcast;
                putEvent(event);
                break;
            default:
                return;                 // Don't clear unhandled events
            }
        clearEvent(event);
        }
}

void Demo::changeVideoMode()
/****************************************************************************
*
* Function:     Demo::changeVideoMode
*
* Description:  Pops up a dialog box to allow the user to change the
*               current video mode, and changes to the new mode if
*               requested.
*
****************************************************************************/
{
    int newMode;

    MVModeSelector *d = new MVModeSelector(dc,"Change Video Mode",mode);
    if (deskTop->execView(d) == cmOk) {
        // Change to the newly selected video mode
        //
        // Post a cmRestart event to restart the application with the new
        // video mode.

        if ((newMode = d->getMode()) != mode) {
            MVEvent event;
            mode = newMode;
            event.what = evCommand;
            event.message.command = cmRestart;
            event.message.infoPtr = NULL;
            putEvent(event);
            }
        }
    delete d;
}
