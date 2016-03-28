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
* Description:  Header file for the MVView class. This class is the base
*               class for user interface classes in the library.
*
****************************************************************************/

#ifndef __MVIS_MVIEW_HPP
#define __MVIS_MVIEW_HPP

#ifndef __MVIS_MVISION_HPP
#include "mvis/mvision.hpp"
#endif

#ifndef __MVIS_MRECT_HPP
#include "mvis/mrect.hpp"
#endif

#ifndef __MVIS_MPOINT_HPP
#include "mvis/mpoint.hpp"
#endif

#ifndef __MVIS_MEVENT_HPP
#include "mvis/mevent.hpp"
#endif

#ifndef __MVIS_MFONTMGR_HPP
#include "mvis/mfontmgr.hpp"
#endif

#ifndef __TCL_DLIST_HPP
#include "tcl/dlist.hpp"
#endif

/*------------------------------ Constants --------------------------------*/

const uint

// MVView state masks

    sfVisible       = 0x001,        // Set if the view is visible
    sfActive        = 0x002,        // Set if the view is active
    sfFocused       = 0x004,        // Set if the view is focused
    sfDisabled      = 0x008,        // Set if the view is disabled
    sfModal         = 0x010,        // Set if the view is modal
    sfExposed       = 0x020,        // Set if the view is exposed
    sfLockRepaint   = 0x040,        // Set if repaints are locked

// MVView option masks

    ofSelectable    = 0x001,        // The view can be selected
    ofFirstClick    = 0x002,        // First mouse click is processed
    ofPreProcess    = 0x004,        // View recieves pre-focused events
    ofPostProcess   = 0x008,        // View recieves post-focused events
    ofCenterX       = 0x010,        // View is centered in X
    ofCenterY       = 0x020,        // View is centered in Y
    ofCentered      = 0x030,        // View is centered in X and Y
    ofNoBackground  = 0x040,        // View has no background
    ofClipToClient  = 0x080,        // View should be clipped to client rect
    ofSelectFront   = 0x100,        // View should move to front on select

// MVView drag mode masks

    dmMoveable      = 0x01,         // Allows the view to move
    dmLimitLoX      = 0x02,         // Lefthand side is limited in movement
    dmLimitHiX      = 0x04,         // Righthand side is limited in movement
    dmLimitLoY      = 0x08,         // Top side is limited in movement
    dmLimitHiY      = 0x10,         // Bottom side is limited in movement
    dmLimitAll      = dmLimitLoX | dmLimitLoY | dmLimitHiX | dmLimitHiY,

// MVView grow mode masks

    gmAnchorLoX     = 0x01,         // Lefthand side cannot move
    gmAnchorHiX     = 0x02,         // Righthand side cannot move
    gmAnchorLoY     = 0x04,         // Top side cannot move
    gmAnchorHiY     = 0x08,         // Bottom side cannot move
    gmGrowRel       = 0x10;         // Keep relative side to owner

/*-------------------------- Class definitions ----------------------------*/

class MVGroup;

//---------------------------------------------------------------------------
// This class is the base class for all user interface classes in the
// library, providing the necessary interface common to all views. Note
// that all views are derived from TCDListNode, so all views can be placed
// onto a linked list.
//---------------------------------------------------------------------------

class MVView : public TCDListNode {
protected:
    MGLDevCtx   dc;             // Device context for drawing this view
    MVRect      bounds;         // Bounding rectangle for the view
    MVPoint     size;           // Size of the view
    MVRegion    visRgn;         // Visible region for this view
    MVRegion    clipRgn;        // Current clip region for this view
    MVRect      clipRect;       // Current clip rectangle for draw call
    MVGroup     *owner;         // MVGroup that owns this view
    uint        state;          // State of the view
    uint        options;        // Option flags for the view
    uchar       dragMode;       // Drag mode for view
    uchar       growMode;       // Grow mode for view
    uint        helpCtx;        // Help context number for the view

            // Phase of event handling for focused events
    enum    phaseType {phFocused,phPreProcess,phPostProcess};

            // Convert a coordinate from global to local coords
            void globalToLocal(MVPoint& p) const    { dc.globalToLocal(p); };
            void localToGlobal(MVPoint& p) const    { dc.localToGlobal(p); };

            // Draw a rectangle with the specified line width.
            // The rectangle will be completely bounded by the rectangle.
            void drawRect(int left,int top,int right,int bottom,int width = 1);
            void drawRect(const MVRect& r, int width = 1)
            { drawRect(r.left(),r.top(),r.right(),r.bottom(),width); };

            // Draw a standard 3d frame inside rectangle
            void drawFrame(int left,int top,int right,int bottom,ibool inset);
            void drawFrame(const MVRect& r,ibool inset)
            { drawFrame(r.left(),r.top(),r.right(),r.bottom(),inset); };

            // Draw a focus rectangle
            void drawFocusRect(int left,int top,int right,int bottom,int color);
            void drawFocusRect(const MVRect& r,int color)
            { drawFocusRect(r.left(),r.top(),r.right(),r.bottom(),color); };

            // Draw a drag rectangle (XOR)
            void drawDragRect(int left,int top,int right,int bottom,int width = 1);
            void drawDragRect(const MVRect& r,int width = 1)
            { drawDragRect(r.left(),r.top(),r.right(),r.bottom(),width); };

            // Draw a border with the specified line width
            void drawBorder(int left,int top,int right,int bottom,int style,int width)
            { MV_drawBorderCoord(left,top,right,bottom,style,width); };
            void drawBorder(const MVRect& r,int style, int width)
            { MV_drawBorder(r,style,width); };

            // Draw a line with the current system line width settings
            void drawLine(int left,int top,int right,int bottom,int width = 1);

            // Draw string with hot character underline (optionally disabled)
            void drawHotStr(const MVPoint& start,const MVPoint& hot1,
                const MVPoint& hot2,ibool hotChar,const MVPoint& offset,
                ibool depressed,const char *str,ibool disabled,
                color_t textColor);

            // Method to load a font and select it into the current DC
            void useFont(const MVFontRecord& fontRec)
            { dc.useFont(MV_fontManager.useFont(fontRec)); };

            // Clip and draw against a single rectangle
    static  MVView *clipAndDrawView;

            // Method to paint the view clipping to clip region (calls draw)
    virtual void paint();

public:
    static  void _clipAndDraw(const rect_t *r);

public:
            // Constructor
            MVView(MGLDevCtx& dc,const MVRect& bounds);

            // Virtual destructor
    virtual ~MVView();

            // Virtual to determine if the view was constructed correctly
    virtual ibool valid(ulong command);

            // Method to clear the event, flagging it as handled
            void clearEvent(MVEvent& event)
                { event.what = evNothing; event.message.infoPtr = this; };

            // Virtual to handle an event
    virtual void handleEvent(MVEvent& event,phaseType phase = phFocused);

            // Methods to retrieve and post events - eventually calls
            // eventQueue.xxx() if not overridden
    virtual void putEvent(MVEvent& event);
    virtual ibool getEvent(MVEvent& event,ulong mask = evAll);
    virtual ibool peekEvent(MVEvent& event,ulong mask = evAll);

            // Methods to clip and draw the view
    virtual void draw(const MVRect& dirty) = 0;

            // Method to set the clip rectangle for the view
            void setClipRect(const MVRect& clip);

            // Setup/reset the viewport relative to owners viewport
            void setupViewport();
            void setupOwnerViewport();
            void resetViewport()    { MV_popViewport(dc); };

            // Methods to show and hide the view
            void show();
            void hide();

            // Methods to enable and disable the view
            void enable();
            void disable();
            void enableView()   { enable(); };
            void disableView()  { disable(); };

            // Methods to lock/unlock repaint events
            void lock()     { state |= sfLockRepaint; };
            void unlock()   { state &= ~sfLockRepaint; };

            // Method to set the bounds of the view
    virtual void setBounds(const MVRect& bounds);

            // Method to move the view to another location
    virtual void moveTo(int x,int y);
            void moveTo(const MVPoint& p)   { moveTo(p.x,p.y); };

            // Methods to obtain bounding information
            const MVRect& getBounds() const { return bounds; };
            void getExtent(MVRect& e) const
                { e.left() = e.top() = 0; e.botRight = size; };
            const MVPoint& getSize() const  { return size; };

            // Methods to invalidate the specified rectangle
    virtual void invalidRect(MVRect& rect);

            // Methods to validate the specified rectangle
    virtual void validRect(MVRect& rect);

            // Methods to request a repaint for the specified rectangle
            void repaint(void)
                { repaint(MVRect(0,0,size.x,size.y)); };
            void repaint(const MVRect& rect);

            // Methods to remove a rectangle from repaint region
            void unrepaint(void)
                { unrepaint(MVRect(0,0,size.x,size.y)); };
            void unrepaint(const MVRect& rect);

            // Returns true if the 'global' coordinate is in the view
            ibool includes(const MVPoint& p) const;

            // Methods to examine the flags of the view
            uint getState() const       { return state; };
            uint getOptions() const { return options; };

            // Method to set a state flag
    virtual void setState(uint state,ibool set);

            // Method to set an options flag
            void setOption(uint option,ibool set);

            // Method to select the view
            void select();

            // Method to select the view and move to front of owners list
            void selectToFront();

            // Obtain the delta to convert from global to local coords
    virtual void findGlobalDelta(MVPoint& d);

            // Method to find the currently active modal view
    virtual MVView *findModalView();

            // Method called when idle
    virtual void idle();

            // Method called to obtain the cursor definition
    virtual MGLCursor *getCursor(const MVPoint& p);

            // Method to map a system color index to a real color value
            color_t getColor(int index);

            // Method to obtain the help context number for the view
    virtual uint getHelpCtx() const;

            // Method to set/get the owner for the view
            void setOwner(MVGroup* owner)   { MVView::owner = owner; };
            MVGroup *getOwner()             { return MVView::owner; };

    static  MVView *capturedView;

            // Capture mouse events to focused view
            void captureMouse()     { capturedView = this; };
            void releaseMouse()     { capturedView = NULL; };

    friend  class MVGroup;
    };

typedef TCDListIterator<MVView> MVViewIterator;

#endif  // __MVIS_MVIEW_HPP
