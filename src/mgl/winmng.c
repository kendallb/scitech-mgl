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
* Language:     ANSI C
* Environment:  Any
*
* Description:  Lightweight window manager for MGL
****************************************************************************/

#include "mgl.h"


/*------------------------- Implementation --------------------------------*/


/****************************************************************************
DESCRIPTION:
Creates window manager object and attaches it to device context.

HEADER:
mgraph.h

PARAMETERS:
dc          - display device context to associate with window manager.

RETURNS:
Window manager object or NULL on error

REMARKS:
This function creates window manager object. MGL WM provides functionality
similar to that of Xlib, i.e. bare minimum needed to implement windowing
environment on top of SciTech MGL. That is, it manages hierarchy of rectangular
windows, takes care of proper repainting (but you must provide painter
functions for all windows) and clipping and distributes input events among
the windows.

In addition to allocating winmng_t object, this function also creates the root
window (which is top-level window that covers entire DC area and that is at the
top of windows hierarchy). Root window is accessible via MGL_wmGetRootWindow.
It also sets the root window visible and shows mouse cursor.

You must create winmng_t object with this function before you can use MGL WM.

Note: You shouldn't draw to the device context associated with window manager
      yourself. Use MGL WM functions MGL_wmBeginPaint and MGL_wmSetWindowPainter
      instead.

Note: MGL_wmCreate doesn't set root window's painter method. You must do
      it yourself, preferably immediately after creating the winmng_t object.

SEE ALSO:
MGL_wmDestroy, MGL_wmCreateWindow, MGL_wmUpdateDC, MGL_wmGetRootWindow,
MGL_wmProcessEvent
****************************************************************************/
winmng_t* MGLAPI MGL_wmCreate(
    MGLDC *dc)
{
    winmng_t *wm;

    if ((wm = PM_malloc(sizeof(winmng_t))) == NULL) {
        FATALERROR(grNoMem);
        return NULL;
        }
    wm->dc = dc;
    wm->globalEventHandlers = NULL;
    wm->capturedEvents = NULL;
    wm->globalCursor = NULL;
    wm->invalidatedRegion = NULL;
    wm->updatingDC = false;
    wm->activeWnd = NULL;
    wm->rootWnd = NULL; /* VS: don't remove this line */
    wm->rootWnd = MGL_wmCreateWindow(wm, NULL,
                                     0, 0, MGL_sizex(dc)+1, MGL_sizey(dc)+1);
    MGL_wmShowWindow(wm->rootWnd, true);
    MS_show();
    return wm;
}


/****************************************************************************
DESCRIPTION:
Destroys window manager.

HEADER:
mgraph.h

PARAMETERS:
wm          - window manager to destroy

REMARKS:
This function destroys the window manager object and all windows managed
by it. It doesn't destroy associated device context.

You must call this function before you destroy associated DC.

SEE ALSO:
MGL_wmCreate
****************************************************************************/
void MGLAPI MGL_wmDestroy(
    winmng_t *wm)
{
    globalevententry_t *evt, *evt2;
    MGL_wmDestroyWindow(wm->rootWnd);
    evt = wm->globalEventHandlers;
    while (evt) {
        evt2 = evt->next;
        PM_free(evt);
        evt = evt2;
        }
    if (wm->invalidatedRegion)
        MGL_freeRegion(wm->invalidatedRegion);
    PM_free(wm);
}


/****************************************************************************
PARAMETERS:
wnd          - window to remove

REMARKS:
Removes window from double-linked list of window siblings and from parent's
pointers. It does not destroy the window.
{secret}
****************************************************************************/
static void removeWindow(
    window_t *wnd)
{
    if (wnd->prev)
        wnd->prev->next = wnd->next;
    if (wnd->next)
        wnd->next->prev = wnd->prev;
    if (wnd->parent) {
        if (wnd->parent->firstChild == wnd)
            wnd->parent->firstChild = wnd->next;
        if (wnd->parent->lastChild == wnd)
            wnd->parent->lastChild = wnd->prev;
        }
    wnd->prev = NULL;
    wnd->next = NULL;
}

/****************************************************************************
PARAMETERS:
wnd          - window to add
parent       - the parent

REMARKS:
Adds window as the top most parent's child (with the exception of windows
with MGL_WM_ALWAYS_ON_TOP flag).
{secret}
****************************************************************************/
static void addWindowToTop(
    window_t *wnd,
    window_t *parent)
{
    wnd->parent = parent;
    if (parent) {
        if (!parent->lastChild) {
            parent->lastChild = parent->firstChild = wnd;
            wnd->prev = wnd->next = NULL;
            }
        else if ((wnd->flags & MGL_WM_ALWAYS_ON_TOP) ||
                !(parent->lastChild->flags & MGL_WM_ALWAYS_ON_TOP)) {
            wnd->prev = parent->lastChild;
            wnd->next = NULL;
            parent->lastChild->next = wnd;
            parent->lastChild = wnd;
            return;
            }
        else {
            window_t *pos = parent->lastChild;
            while (pos && (pos->flags & MGL_WM_ALWAYS_ON_TOP)) pos = pos->prev;
            if (!pos) {
                wnd->next = parent->firstChild;
                wnd->prev = NULL;
                parent->firstChild->prev = wnd;
                parent->firstChild = wnd;
            }
            else {
                wnd->next = pos->next;
                wnd->prev = pos;
                pos->next->prev = wnd;
                pos->next = wnd;
                }
            }
        }
    else {
        /* parent may be NULL only when creating wm->rootWnd */
        wnd->prev = wnd->next = NULL;
        }
}

/****************************************************************************
PARAMETERS:
wnd          - window to add
parent       - the parent

REMARKS:
Adds window as the bottom most parent's child (with the exception of windows
with MGL_WM_ALWAYS_ON_BOTTOM flag).
{secret}
****************************************************************************/
static void addWindowToBottom(
    window_t *wnd,
    window_t *parent)
{
    wnd->parent = parent;
    if (parent) {
        if (!parent->firstChild) {
            parent->lastChild = parent->firstChild = wnd;
            wnd->prev = wnd->next = NULL;
            }
        else if ((wnd->flags & MGL_WM_ALWAYS_ON_BOTTOM) ||
                !(parent->firstChild->flags & MGL_WM_ALWAYS_ON_BOTTOM)) {
            wnd->next = parent->firstChild;
            wnd->prev = NULL;
            parent->firstChild->prev = wnd;
            parent->firstChild = wnd;
            return;
            }
        else {
            window_t *pos = parent->firstChild;
            while (pos && (pos->flags & MGL_WM_ALWAYS_ON_BOTTOM)) pos = pos->next;
            if (!pos) {
                wnd->prev = parent->lastChild;
                wnd->next = NULL;
                parent->lastChild->next = wnd;
                parent->lastChild = wnd;
            }
            else {
                wnd->prev = pos->prev;
                wnd->next = pos;
                pos->prev->next = wnd;
                pos->prev = wnd;
                }
            }
        }
    else {
        /* parent may be NULL only when creating wm->rootWnd */
        wnd->prev = wnd->next = NULL;
        }
}

/****************************************************************************
DESCRIPTION:
Creates new window.

HEADER:
mgraph.h

PARAMETERS:
wm          - window manager that will manage this window
parent      - parent window
x           - X coordinate of initial window position
y           - Y coordinate of initial window position
width       - initial window width
height      - initial window height

RETURNS:
The created window.

REMARKS:
Creates new window and assigns it to given window manager. The window is
created as invisible, you must call MGL_wmShowWindow to show it. You must
also call MGL_wmSetWindowPainter to set painting callback, otherwise the
window will be transparent.

If parent is NULL, the window is created as top level one and its parent
is set to window manager's root window.

SEE ALSO:
MGL_wmCreate, MGL_wmDestroyWindow, MGL_wmSetWindowPosition,
MGL_wmSetWindowPainter, MGL_wmSetWindowCursor, MGL_wmSetWindowFlags,
MGL_wmSetWindowUserData, MGL_wmSetWindowDestructor, MGL_wmGetWindowParent,
MGL_wmCoordGlobalToLocal, MGL_wmCoordLocalToGlobal, MGL_wmGetWindowAtPosition,
MGL_wmReparentWindow, MGL_wmLowerWindow, MGL_wmRaiseWindow, MGL_wmShowWindow,
MGL_wmInvalidateWindow, MGL_wmBeginPaint, MGL_wmPushWindowEventHandler,
MGL_wmPopWindowEventHandler, MGL_wmCaptureEvents, MGL_wmUncaptureEvents
****************************************************************************/
window_t *MGLAPI MGL_wmCreateWindow(
    winmng_t *wm,
    window_t *parent,
    int x,
    int y,
    size_t width,
    size_t height)
{
    window_t  *wnd;

    if ((wnd = PM_malloc(sizeof(window_t))) == NULL) {
        FATALERROR(grNoMem);
        return NULL;
        }
    wnd->wm = wm;
    wnd->firstChild = wnd->lastChild = NULL;
    addWindowToTop(wnd, parent ? parent : wm->rootWnd);
    wnd->eventHandlers = NULL;
    wnd->userData = NULL;
    wnd->cursor = MGL_DEF_CURSOR;
    wnd->painter = NULL;
    wnd->dtor = NULL;
    wnd->visible = false;
    MGL_wmSetWindowPosition(wnd, x, y, width, height);

    return wnd;
}


/****************************************************************************
DESCRIPTION:
Destroys the window.

HEADER:
mgraph.h

PARAMETERS:
wnd         - the window to destroy

REMARKS:
Window destruction happens in two stages: first, the destructor is called if
one was previously set with MGL_wmSetWindowDestructor and second, all children
are destroyed recursively.

SEE ALSO:
MGL_wmCreateWindow, MGL_wmSetWindowDestructor
****************************************************************************/
void MGLAPI MGL_wmDestroyWindow(
    window_t *wnd)
{
    window_t            *child, *child2;
    windowevententry_t  *evt, *evt2;

    MGL_wmInvalidateWindow(wnd);
    if (wnd->wm->activeWnd == wnd)
        wnd->wm->activeWnd = NULL;

    if (wnd->dtor)
        wnd->dtor(wnd);

    wnd->visible = false;/*so that children don't waste time with invalidating*/
    child = wnd->firstChild;
    while (child) {
        child2 = child->next;
        MGL_wmDestroyWindow(child);
        child = child2;
        }
    evt = wnd->eventHandlers;
    while (evt) {
        evt2 = evt->next;
        PM_free(evt);
        evt = evt2;
        }
    removeWindow(wnd);
    PM_free(wnd);
}


/****************************************************************************
DESCRIPTION:
Moves the window to new position and/or changes its size.

HEADER:
mgraph.h

PARAMETERS:
wnd         - the window
x           - new X coordinate of window position
y           - new Y coordinate of window position
width       - new window width
height      - new window height

REMARKS:
Changes window's position and size to new values and repaints the DC
as neccessary. Depending on window's flags and status, different actions
are triggered by the call to MGL_wmSetWindowPosition:

1. If the new size and position are same as old ones, nothing happens.
2. If wnd is top level window (i.e. its parent is wnd->wm->rootWnd) and
   size is unmodified and only position changes, optimized routine is used
   and the window is copied to its new position with MGL_bitBlt.
3. If the window has MGL_WM_FULL_REPAINT_ON_RESIZE flag or its position
   (i.e. not only size) changed, it is completely invalidated and queued
   for repaint.
4. Otherwise (i.e. the window doesn't have MGL_WM_FULL_REPAINT_ON_RESIZE
   and it only changed its size), only the different between old and new
   window area is repainted.

Note: The change in position won't be visible until the next call
      to MGL_wmUpdateDC. As an exception, if optimized bitblt move was used,
      the change is visible immediately.

SEE ALSO:
MGL_wmSetWindowFlags, MGL_wmUpdateDC, MBL_bitBlt
****************************************************************************/
void MGLAPI MGL_wmSetWindowPosition(
    window_t *wnd,
    int x,
    int y,
    size_t width,
    size_t height)
{
    if (wnd->x == x && wnd->y == y && wnd->width == width && wnd->height == height)
        return;

    /* Use fast blitting when moving top level window w/o changing its size: */
    if (wnd->wm->invalidatedRegion == NULL &&
        !wnd->wm->updatingDC &&
        wnd->visible && wnd->parent == wnd->wm->rootWnd &&
        wnd->width == width && wnd->height == height) {

        rect_t    screen, rect, rectFrom, rectTo;
        region_t *invalid;

        MGL_wmUpdateDC(wnd->wm);

        screen = MGL_defRect(0, 0, wnd->parent->width, wnd->parent->height);
        rect = MGL_defRect(wnd->x, wnd->y, wnd->x+wnd->width, wnd->y+wnd->height);
        (void)MGL_sectRect(rect, screen, &rectFrom);
        MGL_offsetRect(rectFrom, x - wnd->x, y - wnd->y);
        (void)MGL_sectRect(rectFrom, screen, &rectTo);
        rectFrom = rectTo;
        MGL_offsetRect(rectFrom, wnd->x - x, wnd->y - y);

        invalid = MGL_rgnSolidRect(rect);
        MGL_offsetRect(rect, x - wnd->x, y - wnd->y);
        (void)MGL_unionRegionRect(invalid, &rect);
        (void)MGL_diffRegionRect(invalid, &rectTo);
        MGL_wmInvalidateRegion(wnd->wm, invalid);
        MGL_freeRegion(invalid);

        MS_obscure();
        MGL_bitBlt(wnd->wm->dc, wnd->wm->dc,
                   rectFrom, rectTo.left, rectTo.top, MGL_REPLACE_MODE);
        MS_show();

        wnd->x = x;
        wnd->y = y;

        MGL_wmUpdateDC(wnd->wm);
        }

    /* Completely invalidate the window: */
    else if ((wnd->flags & MGL_WM_FULL_REPAINT_ON_RESIZE) ||
            wnd->x != x || wnd->y != y) {
        MGL_wmInvalidateWindow(wnd);
        wnd->x = x;
        wnd->y = y;
        wnd->width = width;
        wnd->height = height;
        MGL_wmInvalidateWindow(wnd);
        }

    /* Only invalidate area that is the difference between old and new size: */
    else {
        int    dw, dh;
        rect_t rect;

        dw = width - wnd->width;
        dh = height - wnd->height;

        if (dw > 0) {
            wnd->width = width;
            rect = MGL_defRect(wnd->width-dw, 0,
                               wnd->width-1, wnd->height-1);
            MGL_wmInvalidateWindowRect(wnd, &rect);
            }
        else if (dw < 0) {
            rect = MGL_defRect(width, 0,
                               wnd->width-1, wnd->height-1);
            MGL_wmInvalidateWindowRect(wnd, &rect);
            wnd->width = width;
            }

        if (dh > 0) {
            wnd->height = height;
            rect = MGL_defRect(0, wnd->height-dh,
                               wnd->width-1, wnd->height-1);
            MGL_wmInvalidateWindowRect(wnd, &rect);
            }
        else if (dh < 0) {
            rect = MGL_defRect(0, height,
                               wnd->width-1, wnd->height-1);
            MGL_wmInvalidateWindowRect(wnd, &rect);
            wnd->height = height;
            }
        }
}


/****************************************************************************
DESCRIPTION:
Sets painter callback for the window.

HEADER:
mgraph.h

PARAMETERS:
wnd         - the window
painter     - painter callback

REMARKS:
The painter is responsible for redrawing the window it is attached to. It can
only draw to the part of device context convered by the window.

Every window must have a painter callback associated with it. MGL WM calls
painters as neccessary from MGL_wmUpdateDC to ensure that the device context
reflects current status of windows.

The painter callback takes two arguments: one of them is the window being
painted and the other is the device context to paint on. The DC is already
made current and clipping region is properly set.

Note: You can use MGL_wmSetWindowUserData to attach arbitrary data to
      the window and use this data in painter function.

SEE ALSO:
MGL_wmSetWindowFlags, MGL_wmShowWindow, MGL_wmCreateWindow, MGL_wmUpdateDC,
MGL_wmBeginPaint, MGL_wmSetWindowUserData, MGL_wmInvalidateWindow
****************************************************************************/
void MGLAPI MGL_wmSetWindowPainter(
    window_t *wnd,
    painter_t painter)
{
    wnd->painter = painter;
    MGL_wmInvalidateWindow(wnd);
}


/****************************************************************************
DESCRIPTION:
Sets global cursor.

HEADER:
mgraph.h

PARAMETERS:
wm          - window manager
cursor      - the cursor to set

REMARKS:
In MGL window manager, any window has associated mouse cursor that is shown
whenever the pointer enters window's area. This function enables you to
forcingly set one cursor for all windows (useful for e.g. hourglass cursor
when performing a lengthy operation). MGL WM will ignore cursors associated
with windows if there's a global one.

Call MGL_wmSetGlobalCursor(NULL) to disable global cursor.

Note: MGL_wmSetGlobalCursor does not take ownership of the cursor.

SEE ALSO:
MGL_wmSetWindowCursor
****************************************************************************/
void MGLAPI MGL_wmSetGlobalCursor(
    winmng_t *wm,
    cursor_t *cursor)
{
    wm->globalCursor = cursor;
    if (cursor == NULL) {
        if (wm->activeWnd)
            MS_setCursor(wm->activeWnd->cursor);
        else
            MS_setCursor(wm->rootWnd->cursor);
        }
    else
        MS_setCursor(cursor);
}


/****************************************************************************
DESCRIPTION:
Sets mouse cursor specific to window.

HEADER:
mgraph.h

PARAMETERS:
wnd         - window to set the cursor for
cursor      - the cursor to set

REMARKS:
In MGL window manager, any window has associated mouse cursor that is shown
whenever the pointer enters window's area. Call this function to set the
cursor (default is arrow cursor).

Note: MGL_wmSetWindowCursor does not take ownership of the cursor.

SEE ALSO:
MGL_wmSetGlobalCursor
****************************************************************************/
void MGLAPI MGL_wmSetWindowCursor(
    window_t *wnd,
    cursor_t *cursor)
{
    int mx, my, mlx, mly;

    wnd->cursor = cursor;
    MS_getPos(&mx, &my);
    MGL_wmCoordGlobalToLocal(wnd, mx, my, &mlx, &mly);
    if (mlx >= 0 && mly >= 0 && mlx < wnd->width && mly < wnd->height)
        MS_setCursor(cursor);
}


/****************************************************************************
DESCRIPTION:
Associates user data with the window.

HEADER:
mgraph.h

PARAMETERS:
wnd         - the window
data        - pointer to user data

REMARKS:
This function is primarily meant for interaction with higher-level windowing
system built on top of MGL WM. window_t structure contains user data pointer
that is not used for anything by MGL, but the user is provided with functions
for reading and writing it (MGL_wmSetWindowUserData and
MGL_wmGetWindowUserData).

You probably want to set window destructor, too, in order to free user data
before the window is destroyed.

SEE ALSO:
MGL_wmGetWindowUserData, MGL_wmSetWindowDestructor
****************************************************************************/
void MGLAPI MGL_wmSetWindowUserData(
    window_t *wnd,
    void *data)
{
    wnd->userData = data;
}


/****************************************************************************
DESCRIPTION:
Reads user data of the window.

HEADER:
mgraph.h

PARAMETERS:
wnd         - the window

RETURNS:
User data pointer.

REMARKS:
Returns user data pointer of this window or NULL if you haven't called
MGL_wmSetWindowUserData before.

SEE ALSO:
MGL_wmSetWindowUserData
****************************************************************************/
void* MGLAPI MGL_wmGetWindowUserData(
    window_t *wnd)
{
    return wnd->userData;
}


/****************************************************************************
DESCRIPTION:
Sets flags affecting window's behavior.

HEADER:
mgraph.h

PARAMETERS:
wnd         - the window
flags       - combination of flags defined in MGL_wmWindowFlags enum

REMARKS:
This function is usually called immediately after creating a window to define
its behavior. Flags may be 0 (the default) or any or-combination of possible
flags.

This function may be called several times, MGL WM will update the window
as neccessary (for example, you may call
MGL_wmSetWindowFlags(wnd, MGL_WM_ALWAYS_ON_TOP) and later
MGL_wmSetWindowFlags(wnd, 0) to disable the behavior).

SEE ALSO:
MGL_wmGetWindowFlags, MGL_wmCreateWindow
****************************************************************************/
void MGLAPI MGL_wmSetWindowFlags(
    window_t *wnd,
    long flags)
{
    wnd->flags = flags;

    if (flags & MGL_WM_ALWAYS_ON_TOP) {
        if (flags & MGL_WM_ALWAYS_ON_BOTTOM)
            flags &= ~MGL_WM_ALWAYS_ON_BOTTOM;
        MGL_wmRaiseWindow(wnd);
        }
    if (flags & MGL_WM_ALWAYS_ON_BOTTOM)
        MGL_wmLowerWindow(wnd);
}


/****************************************************************************
DESCRIPTION:
Returns currently set window flags.

HEADER:
mgraph.h

PARAMETERS:
wnd         - the window

RETURNS:
Window's flags.

SEE ALSO:
MGL_wmSetWindowFlags
****************************************************************************/
long MGLAPI MGL_wmGetWindowFlags(
    window_t *wnd)
{
    return wnd->flags;
}


/****************************************************************************
DESCRIPTION:
Sets window's destructor callback.

HEADER:
mgraph.h

PARAMETERS:
wnd         - the window
dtor        - the destructor callback

REMARKS:
Use this function to set window's destructor callback. The destructor is
called by MGL_wmDestroyWindow before destroying children and deallocating
window_t structure. This function gives you a chance to react to window
destruction (for example by freeing data set with MGL_wmSetWindowUserData).

Note: Remember that you don't always destroy windows with MGL_wmDestroyWindow;
      some windows are destroyed "implicitly". This happens when you call
      MGL_wmDestroyWindow on window that itself has child windows. Children are
      recursively destroyed and destructor callback is the only way to notify
      your code about it. As a special case, this also happens in MGL_wmDestroy
      because it calls MGL_wmDestroyWindow on the root window.

SEE ALSO:
MGL_wmDestroyWindow, MGL_wmSetWindowUserData
****************************************************************************/
void MGLAPI MGL_wmSetWindowDestructor(
    window_t *wnd,
    windtor_t dtor)
{
    wnd->dtor = dtor;
}


/****************************************************************************
DESCRIPTION:
Returns pointer to window manager's root window.

HEADER:
mgraph.h

PARAMETERS:
wm          - the window manager

RETURNS:
The root window.

REMARKS:
This function returns pointer to window manager's root window. The root window
is a special window several notable properties: You don't create or destroy it
directly, MGL_wmCreate and MGL_wmDestroy do it. It covers entire device context
and all windows are its children (or grand-children or grand-grand-children and
so on). Windows created with NULL parent are direct children of the root
window.

Note: There is neither default event handling nor painter set to root window
      by MGL_wmCreate. You have to set at least the painter yourself.

SEE ALSO:
MGL_wmCreate, MGL_wmDestroy
****************************************************************************/
window_t* MGLAPI MGL_wmGetRootWindow(
    winmng_t *wm)
{
    return wm->rootWnd;
}


/****************************************************************************
DESCRIPTION:
Returns parent window.

HEADER:
mgraph.h

PARAMETERS:
wnd         - the window

RETURNS:
Pointer to parent window or NULL.

REMARKS:
The fuction returns pointer to parent window, i.e. the window whose this
window is child.

If the window was created with NULL parent passed to MGL_wmCreateWindow,
this function does not return NULL. Instead, it returns pointer to WM's
root window.

MGL_wmGetWindowParent(MGL_wmGetRootWindow(wm)) returns NULL.

SEE ALSO:
MGL_wmCreateWindow, MGL_wmGetRootWindow
****************************************************************************/
window_t* MGLAPI MGL_wmGetWindowParent(
    window_t *wnd)
{
    return wnd->parent;
}


/****************************************************************************
DESCRIPTION:
Converts between local and global coordinates.

HEADER:
mgraph.h

PARAMETERS:
wnd         - the window
x           - X coordinate in global space
y           - Y coordinate in global space
xLocal      - variable to store X coordinate in local space
yLocal      - variable to store X coordinate in local space

REMARKS:
This function converts point expressed in global coordinates (i.e. relative
to device context's upper left corner) into coordinates relative to given
window's upper left corner.

wnd may be a window arbitrary deep in windows hierarchy,
MGL_wmCoordGlobalToLocal descends the hierarchy recursively.

SEE ALSO:
MGL_wmCoordGlobalToLocal, MGL_wmSetWindowPosition
****************************************************************************/
void MGLAPI MGL_wmCoordGlobalToLocal(
    window_t *wnd,
    int x,
    int y,
    int *xLocal,
    int *yLocal)
{
    while (wnd) {
        x -= wnd->x;
        y -= wnd->y;
        wnd = wnd->parent;
        }
    *xLocal = x;
    *yLocal = y;
}


/****************************************************************************
DESCRIPTION:
Converts between global and local coordinates.

HEADER:
mgraph.h

PARAMETERS:
wnd         - the window
x           - X coordinate in local space
y           - Y coordinate in local space
xGlobal     - variable to store X coordinate in global space
yGlobal     - variable to store X coordinate in global space

REMARKS:
This function converts point expressed in coordinates relative to given
window's upper left corner to global coordinates (i.e. relative
to device context's upper left corner).

wnd may be a window arbitrary deep in windows hierarchy,
MGL_wmCoordLocalToGlobal descends the hierarchy recursively.

SEE ALSO:
MGL_wmCoordLocalToGlobal, MGL_wmSetWindowPosition
****************************************************************************/
void MGLAPI MGL_wmCoordLocalToGlobal(
    window_t *wnd,
    int x,
    int y,
    int *xGlobal,
    int *yGlobal)
{
    while (wnd) {
        x += wnd->x;
        y += wnd->y;
        wnd = wnd->parent;
        }
    *xGlobal = x;
    *yGlobal = y;
}

/****************************************************************************
DESCRIPTION:
Finds window at given coordinates.

HEADER:
mgraph.h

PARAMETERS:
wm          - the window manager
x           - X coordinate (global)
y           - Y coordinate (global)

RETURNS:
Pointer to window at given position.

REMARKS:
The function returns window that draws itself at position (x,y). It properly
handles hidden and overlapping windows. The position is expressed in global
coordinates, i.e. relative to device context.

MGL_wmGetWindowAtPosition can never return NULL; if no other window is found,
pointer to wm's root window is returned;

SEE ALSO:
MGL_wmCoordLocalToGlobal, MGL_wmSetWindowPosition
****************************************************************************/
window_t* MGLAPI MGL_wmGetWindowAtPosition(
    winmng_t *wm,
    int x,
    int y)
{
    window_t *wnd, *child;
    ibool    found;

    wnd = wm->rootWnd;
    found = false;
    for (;;) {
        if (found) return wnd;
        x -= wnd->x;
        y -= wnd->y;
        /* traverse children in top to bottom Z-order: */
        found = true;
        for (child = wnd->lastChild; child; child = child->prev) {
            if (child->visible &&
                x >= child->x && y >= child->y &&
                x < child->x + child->width && y < child->y + child->height) {
                wnd = child;
                found = false;
                break;
                }
            }
        }
}


/****************************************************************************
DESCRIPTION:
Reparents window.

HEADER:
mgraph.h

PARAMETERS:
wnd         - the window
newParent   - new parent

REMARKS:
Changes window's parent. This is two-stages operation: MGL removes the window
from its current parent first and then adds it as a child of newParent.

SEE ALSO:
MGL_wmCreateWindow, MGL_wmGetWindowParent
****************************************************************************/
void MGLAPI MGL_wmReparentWindow(
    window_t *wnd,
    window_t *newParent)
{
    ibool visible = false;

    if (wnd->visible) {
        visible = true;
        MGL_wmShowWindow(wnd, false);
        }

    removeWindow(wnd);
    if (wnd->flags & MGL_WM_ALWAYS_ON_BOTTOM)
        addWindowToBottom(wnd, newParent);
    else
        addWindowToTop(wnd, newParent);

    if (visible)
        MGL_wmShowWindow(wnd, true);
}


/****************************************************************************
DESCRIPTION:
Lowers the window.

HEADER:
mgraph.h

PARAMETERS:
wmd         - the window

REMARKS:
Lowers the window so that it is behind all its siblings. The window does not
cover any part of any of its siblings after this operation.

Note that if there are sibling windows with MGL_WM_STAY_ON_BOTTOM flag then
the window will be place on top of them unless it itself has this flag.

SEE ALSO:
MGL_wmSetWindowFlags, MGL_wmRaiseWindow
****************************************************************************/
void MGLAPI MGL_wmLowerWindow(
    window_t *wnd)
{
    if (wnd == NULL || wnd->parent == NULL) return;
    MGL_wmInvalidateWindow(wnd);
    removeWindow(wnd);
    addWindowToBottom(wnd, wnd->parent);
}


/****************************************************************************
DESCRIPTION:
Raises the window.

HEADER:
mgraph.h

PARAMETERS:
wmd         - the window

REMARKS:
Raises the window in front of its siblings. No part of the window is
covered by any of its siblings after this operation.

SEE ALSO:
MGL_wmSetWindowFlags, MGL_wmLowerWindow
****************************************************************************/
void MGLAPI MGL_wmRaiseWindow(
    window_t *wnd)
{
    if (wnd == NULL || wnd->parent == NULL) return;
    removeWindow(wnd);
    addWindowToTop(wnd, wnd->parent);
    MGL_wmInvalidateWindow(wnd);
}


/****************************************************************************
DESCRIPTION:
Shows or hides the window

HEADER:
mgraph.h

PARAMETERS:
wnd         - the window
show        - true if the window shall be shown, false if hidden

REMARKS:
If show is true, shows the window and invalidates device context appropriately.
If show is false, hides the window.

Hidden window is not visible, does not accept events and
MGL_wmGetWindowAtPosition will never test against it. It is still a valid
window, though, and you can manipulate it just like any other window.
****************************************************************************/
void MGLAPI MGL_wmShowWindow(
    window_t *wnd,
    ibool show)
{
    if (show == wnd->visible) return /* no-op */;
    if (show) {
        wnd->visible = true;
        MGL_wmInvalidateWindow(wnd);
        }
    else {
        window_t *active;

        MGL_wmInvalidateWindow(wnd);
        wnd->visible = false;

        /* if activeWnd is wnd or any of its children, reset it, since it
           can't receive mouse events when hidden: */
        active = wnd->wm->activeWnd;
        while (active) {
            if (active == wnd) {
                wnd->wm->activeWnd = NULL;
                break;
                }
            active = active->parent;
            }
        }
}


/****************************************************************************
DESCRIPTION:
Invalidates rectangular part of device context.

HEADER:
mgraph.h

PARAMETERS:
wm          - the window manager
rect        - rectangular area to invalidate (in global coordinates)

REMARKS:
This function invalidates given rectangular area of device context that is
associated with this window manager. The DC will be repainted next time
you call MGL_wmUpdateDC.

SEE ALSO:
MGL_wmUpdateDC, MGL_wmInvalidateRegion, MGL_wmInvalidateWindow,
MGL_wmInvalidateWindowRect, MGL_wmInvalidateWindowRegion
****************************************************************************/
void MGLAPI MGL_wmInvalidateRect(
    winmng_t *wm,
    rect_t *rect)
{
    rect_t r2;

    (void)MGL_sectRect(MGL_defRect(0, 0, wm->rootWnd->width,wm->rootWnd->height),
                       *rect, &r2);

    if (wm->invalidatedRegion)
        MGL_unionRegionRect(wm->invalidatedRegion, &r2);
    else
        wm->invalidatedRegion = MGL_rgnSolidRect(r2);
}


/****************************************************************************
DESCRIPTION:
Invalidates region of device context.

HEADER:
mgraph.h

PARAMETERS:
wm          - the window manager
region      - region to invalidate (in global coordinates)

REMARKS:
This function invalidates given region of device context that is
associated with this window manager. The DC will be repainted next time
you call MGL_wmUpdateDC.

SEE ALSO:
MGL_wmUpdateDC, MGL_wmInvalidateRect, MGL_wmInvalidateWindow,
MGL_wmInvalidateWindowRect, MGL_wmInvalidateWindowRegion
****************************************************************************/
void MGLAPI MGL_wmInvalidateRegion(
    winmng_t *wm,
    region_t *region)
{
    region_t *reg;
    rect_t   rect;

    rect = MGL_defRect(0, 0, wm->rootWnd->width,wm->rootWnd->height);
    reg = MGL_sectRegionRect(region, &rect);

    if (wm->invalidatedRegion) {
        MGL_unionRegion(wm->invalidatedRegion, reg);
        MGL_freeRegion(reg);
        }
    else
        wm->invalidatedRegion = reg;
}

/****************************************************************************
PARAMETERS:
wnd         - the window

REMARKS:
This function checks whether the window is visible on the screen or not.
A window is visible if and only if it has wnd->visible set to true and so
does its parent, grand-parent, grand-grand-parent etc. up to the root window.

RETURNS:
Returns true if the window is visible on the screen, false otherwise.

{secret}
****************************************************************************/
static ibool windowIsVisible(window_t *wnd)
{
    window_t *p;

    for (p = wnd; p; p = p->parent)
        if (!p->visible) return false;
    return true;
}


/****************************************************************************
PARAMETERS:
wnd         - the window
region      - initial region

REMARKS:
This function computes intersection of given region (in window's local
coordinate space) with visible part of the window and stores the result
in region parameter (which is therefore input-output). Parts of the window
covered by its children is substracted from the result.

Returned region is in global coordinates. It is not covered by any other
window and can be directly passed to painter function (after converting
it to window's local coordinates).

RETURNS:
Returns false, if visible region is empty or the window is hidden, true
otherwise.

{secret}
****************************************************************************/
static ibool computeVisibleRegion(
    window_t *wnd,
    region_t *region)
{
    int       x, y;
    window_t  *wnd1, *wnd2;
    rect_t    rect;

    if (!windowIsVisible(wnd)) return false;

    /* Convert to global coordinates: */
    MGL_wmCoordLocalToGlobal(wnd, 0, 0, &x, &y);
    MGL_offsetRegion(region, x, y);

    /* Substract areas covered by other windows from the region. Windows
       that may cover this window's area are its siblings higher in Z-order
       or parent's (and grand-...-parent's) siblings with higher Z-order. */
    for (wnd1 = wnd; wnd1; wnd1 = wnd1->parent) {
        for (wnd2 = wnd1->next; wnd2; wnd2 = wnd2->next) {
            if (!wnd2->visible) continue;
            MGL_wmCoordLocalToGlobal(wnd2, 0, 0, &rect.left, &rect.top);
            rect.right = rect.left + wnd2->width;
            rect.bottom = rect.top + wnd2->height;
            if (MGL_disjointRect(rect, region->rect)) continue;
            MGL_diffRegionRect(region, &rect);
            if (MGL_emptyRegion(region)) {
                /* empty region, wnd fully covered by others */
                return false;
                }
            }
        }
    return true;
}

/****************************************************************************
PARAMETERS:
wnd         - the window
region      - initial region

REMARKS:
This function computes intersection of given region (in window's local
coordinate space) with visible part of the window and adds the result to
window manager's invalidated region.

Note: This function has imporant side effect: it frees the region passed in!
{secret}
****************************************************************************/
static void invalidateWndRegion(
    window_t *wnd,
    region_t *region)
{
    if (!computeVisibleRegion(wnd, region)) {
        /* empty region, wnd fully covered by others */
        MGL_freeRegion(region);
        return;
        }

    if (wnd->wm->invalidatedRegion) {
        MGL_unionRegion(wnd->wm->invalidatedRegion, region);
        MGL_freeRegion(region);
        }
    else
        wnd->wm->invalidatedRegion = region;
}


/****************************************************************************
DESCRIPTION:
Invalidates window.

HEADER:
mgraph.h

PARAMETERS:
wnd         - the window to invalidate

REMARKS:
This function invalidates area of DC covered by the window.

The DC will be repainted next time you call MGL_wmUpdateDC.

SEE ALSO:
MGL_wmUpdateDC, MGL_wmInvalidateRect, MGL_wmInvalidateRegion,
MGL_wmInvalidateWindowRect, MGL_wmInvalidateWindowRegion
****************************************************************************/
void MGLAPI MGL_wmInvalidateWindow(
    window_t *wnd)
{
    rect_t    rect;
    int       x,y;
    region_t  *region;

    if (!windowIsVisible(wnd)) return;
    rect = MGL_defRect(0, 0, wnd->width, wnd->height);

    MGL_wmCoordLocalToGlobal(wnd, 0, 0, &x, &y);
    (void)MGL_sectRectCoord(0, 0, wnd->width, wnd->height,
                            -x, -y,
                            -x+wnd->wm->rootWnd->width, -y+wnd->wm->rootWnd->height,
                            &rect);

    region = MGL_rgnSolidRect(rect);
    invalidateWndRegion(wnd, region);
}


/****************************************************************************
DESCRIPTION:
Invalidates rectangular area of window.

HEADER:
mgraph.h

PARAMETERS:
wnd         - the window to invalidate
rect        - rectangle to invalidate (in window's coordinates)

REMARKS:
This function invalidates given rectangular region of device context that is
associated with this window manager. The rectangle is in coordinates relative
to the window's upper left corner and is intersected with window's visible
part before invalidating the DC.

The DC will be repainted next time you call MGL_wmUpdateDC.

SEE ALSO:
MGL_wmUpdateDC, MGL_wmInvalidateRect, MGL_wmInvalidateRegion,
MGL_wmInvalidateWindow, MGL_wmInvalidateWindowRegion
****************************************************************************/
void MGLAPI MGL_wmInvalidateWindowRect(
    window_t *wnd,
    rect_t *rect)
{
    region_t  *region;
    rect_t     r2;
    int        x,y;

    if (!windowIsVisible(wnd)) return;
    MGL_wmCoordLocalToGlobal(wnd, 0, 0, &x, &y);
    (void)MGL_sectRectCoord(rect->left, rect->top, rect->right+1, rect->bottom+1,
                            -x, -y,
                            -x+wnd->wm->rootWnd->width, -y+wnd->wm->rootWnd->height,
                            &r2);
    region = MGL_rgnSolidRect(r2);
    invalidateWndRegion(wnd, region);
}


/****************************************************************************
DESCRIPTION:
Invalidates region of window.

HEADER:
mgraph.h

PARAMETERS:
wnd         - the window to invalidate
region      - region to invalidate (in window's coordinates)

REMARKS:
This function invalidates given region of device context that is
associated with this window manager. The region is in coordinates relative
to the window's upper left corner and is intersected with window's visible
part before invalidating the DC.

The DC will be repainted next time you call MGL_wmUpdateDC.

SEE ALSO:
MGL_wmUpdateDC, MGL_wmInvalidateRect, MGL_wmInvalidateRegion,
MGL_wmInvalidateWindow, MGL_wmInvalidateWindowRect
****************************************************************************/
void MGLAPI MGL_wmInvalidateWindowRegion(
    window_t *wnd,
    region_t *region)
{
    region_t  *r;
    int        x,y;
    rect_t     rect;

    if (!windowIsVisible(wnd)) return;
    MGL_wmCoordLocalToGlobal(wnd, 0, 0, &x, &y);
    MGL_defRect(-x, -y, -x+wnd->wm->rootWnd->width, -y+wnd->wm->rootWnd->height);
    r = MGL_sectRegionRect(region, &rect);
    invalidateWndRegion(wnd, r);
}


/****************************************************************************
PARAMETERS:
wnd               - the window to update
clipRect          - clipping rectangle that must not be exceeded
invalidatedRegion - invalidated region being repainted (initially taken from wm)

RETURNS:
Returns true if there's anything left for updating, false otherwise.

REMARKS:
This function updates part of window manager's invalidated region that is
covered by the window - that is, paints the window by calling its painter
function and calls updateWindow for all its children.

As a side effect, the function updates WM's invalidated region so that it
no longer contains the window.
{secret}
****************************************************************************/
static ibool updateWindow(
    window_t *wnd,
    rect_t *clipRect,
    region_t *invalidatedRegion)
{
    rect_t    rect, wndRect;
    int       x,y;
    window_t  *child;

    if (!wnd->visible)
        return true;
    MGL_wmCoordLocalToGlobal(wnd, 0, 0, &x, &y);
    wndRect = MGL_defRect(x, y, x+wnd->width, y+wnd->height);
    (void)MGL_sectRect(*clipRect, wndRect, &rect);
    if (MGL_disjointRect(invalidatedRegion->rect, rect))
        return true;

    /* This loop is two-fold: one, it iterates over all children
       and updates them (top-to-bottom Z-order);
       two, it substracts children's areas from invalidatedRegion. */
    for (child = wnd->lastChild; child; child = child->prev) {
        if (!updateWindow(child, &rect, invalidatedRegion))
            return false; /* empty region, wnd fully covered by others */
        }

    /* Finally, render wnd itself, children excluded. The loop above
       modified invalidatedRegion so that it now contains visible
       area of wnd minus all its children. */
    if (wnd->painter) {
        if ((x - rect.left < 0) || (y - rect.top < 0)) {
            /* Happens when "rect"'s upper left corner was clipped. We can't use
               "rect" for viewport in such case because it would shift drawing area
               to the right side. */
            region_t *reg;
            reg = MGL_sectRegionRect(invalidatedRegion, &rect);
            MGL_offsetRegion(reg, -x, -y);
            MGL_setViewport(wndRect);
            MGL_setClipRegion(reg);
            MGL_freeRegion(reg);
            }
        else {
            MGL_offsetRegion(invalidatedRegion, -x, -y);
            MGL_setViewport(rect);
            MGL_setClipRegion(invalidatedRegion);
            MGL_offsetRegion(invalidatedRegion, x, y);
            }

        wnd->painter(wnd, wnd->wm->dc);
        MGL_restoreAttributes(&wnd->wm->dcAttrs);
        }
    MGL_diffRegionRect(invalidatedRegion, &rect);
    return !MGL_emptyRegion(invalidatedRegion);
}


/****************************************************************************
DESCRIPTION:
Updates invalidated parts of window manager's DC.

HEADER:
mgraph.h

PARAMETERS:
wm          - the window manager

REMARKS:
This function updates the device context by redrawing all areas previously
invalidated by either explicit calls to MGL_wmInvalidateXXX functions or
by operations that modify visual appearance of windows (such as
MGL_wmSetWindowPosition, MGL_wmShowWindow or MGL_wmRaiseWindow).

Painter callbacks attached to windows are used to do actual painting.

Note: A typical application will run in event loop and repeatedly call
      EVT_halt, MGL_wmProcessEvent and MGL_wmUpdateDC.

Note: This function automatically hides mouse pointer if neccessary
      to avoid occurence rendering artifacts.

SEE ALSO:
MGL_wmInvalidateRect, MGL_wmInvalidateRegion, MGL_wmInvalidateWindow,
MGL_wmInvalidateWindowRect, MGL_wmInvalidateWindowRegion, MGL_wmProcessEvent,
MGL_wmSetWindowPainter
****************************************************************************/
void MGLAPI MGL_wmUpdateDC(
    winmng_t *wm)
{
    rect_t    clipRect;
    region_t *invalidatedRegion;

    if (wm->updatingDC || wm->invalidatedRegion == NULL) return;

    wm->updatingDC = true;
    invalidatedRegion = wm->invalidatedRegion;
    wm->invalidatedRegion = NULL;
    MGL_makeCurrentDC(wm->dc);
    MGL_getAttributes(&wm->dcAttrs);
    MS_obscure();
    clipRect = MGL_defRect(0, 0, wm->rootWnd->width, wm->rootWnd->height);
    updateWindow(wm->rootWnd, &clipRect, invalidatedRegion);
    clipRect = MGL_defRect(0, 0, wm->rootWnd->width, wm->rootWnd->height);
    MGL_setViewport(clipRect);
    MGL_setClipRect(clipRect);

    MS_show();

    MGL_freeRegion(invalidatedRegion);
    wm->updatingDC = false;
}


/****************************************************************************
DESCRIPTION:
Creates DC suitable for painting on window

HEADER:
mgraph.h

PARAMETERS:
wnd         - the window to paint on

RETURNS:
Returns device context that you can paint on.

REMARKS:
If you can't use painter function to paint on a window for some reason,
MGL provides this fuction. It returns a DC prepared for drawing on it
(specifically, clipping region is set to clip off anything that is not in
window's visible part and coordinate system is changed to be local to the
window).

You must call MGL_wmEndPaint when you're done with painting on the window.

Note: Avoid setting clipping region on returned DC. If you must do so,
      make sure you intersect your clipping region with current clipping
      region of the DC!

Note: This function automatically hides mouse pointer if neccessary
      to avoid occurence rendering artifacts.

Note: The effect of MGL_wmBeginPaint/MGL_wmEndPaint drawing is temporary,
      MGL_wmUpdateDC may (and probably will) redraw it using the painter
      later.

SEE ALSO:
MGL_wmUpdateDC, MGL_wmSetWindowPainter, MGL_wmEndPaint
****************************************************************************/
MGLDC* MGLAPI MGL_wmBeginPaint(
    window_t *wnd)
{
    rect_t    rect;
    int       x,y;
    region_t  *region;
    window_t  *child;

    MGL_makeCurrentDC(wnd->wm->dc);
    MGL_getAttributes(&wnd->wm->dcAttrs);

    MGL_wmCoordLocalToGlobal(wnd, 0, 0, &x, &y);
    (void)MGL_sectRectCoord(0, 0, wnd->width, wnd->height,
                            -x, -y,
                            -x+wnd->wm->rootWnd->width, -y+wnd->wm->rootWnd->height,
                            &rect);
    region = MGL_rgnSolidRect(rect);
    if (computeVisibleRegion(wnd, region)) {
        for (child = wnd->firstChild; child; child = child->next) {
            if (!child->visible) continue;
            rect = MGL_defRect(x + child->x,
                               y + child->y,
                               x + child->x + child->width,
                               y + child->y + child->height);
            MGL_diffRegionRect(region, &rect);
            if (MGL_emptyRegion(region))
                break;
            }
        rect = MGL_defRect(x, y, x+wnd->width, y+wnd->height);
        MGL_setViewport(rect);
        MGL_offsetRegion(region, -x, -y);
        MGL_setClipRegion(region);
        }
    else {
        MGL_setViewport(MGL_defRect(0, 0, 0, 0));
        }

    MGL_freeRegion(region);
    MS_obscure();
    return wnd->wm->dc;
}


/****************************************************************************
DESCRIPTION:
Finishes painting on window

HEADER:
mgraph.h

PARAMETERS:
wnd         - the window you painted on

REMARKS:
This function must be called after using MGL_wmBeginPaint on the window.
It restores mouse cursor and device context's attributes.

SEE ALSO:
MGL_wmUpdateDC, MGL_wmSetWindowPainter, MGL_wmBeginPaint
****************************************************************************/
void MGLAPI MGL_wmEndPaint(
    window_t *wnd)
{
    rect_t scrRect;

    MS_show();
    MGL_restoreAttributes(&wnd->wm->dcAttrs);
    scrRect = MGL_defRect(0, 0, wnd->wm->rootWnd->width, wnd->wm->rootWnd->height);
    MGL_setViewport(scrRect);
    MGL_setClipRect(scrRect);
}


/****************************************************************************
DESCRIPTION:
Processes event and distributes it to windows.

HEADER:
mgraph.h

PARAMETERS:
wm          - window manager
event       - event to distribute

RETURNS:
Returns true if the event was processed or false otherwise.

REMARKS:
MGL_wmProcessEvent distributes events to windows and passes them to
event handlers that the user attached to windows. This happens in several steps:

First, global event handlers (see MGL_wmPushGlobalEventHandler) are searched
for one that can handle this event (by ANDing event->what with the mask)
and if such handler exists, the event is passed to it. If global handler
returns true, processing ends and MGL_wmProcessEvent returns true. If
the handler returns false, search continues among other global handlers.

Next, redirection table is searched for windows that have captured events
of this type. If such window is found, it will be used in the next step.
Otherwise, MGL_wmProcessEvent uses the window under mouse pointer.

Finally, event table of the window determined in the previous step is searched
for a handler that accepts this type of events and the event is passed to it.
If the handler returns true, the function returns with true, otherwise it
continues with the rest of handlers.

If none of the above steps succeeded, false is returned.

Note: This function also handles changes of mouse cursor as the pointer moves
      from one window to another.

Note: Mouse position information in event_t is NOT translated to window's
      local coordinates!

Note: A typical application will run in event loop and repeatedly call
      EVT_halt, MGL_wmProcessEvent and MGL_wmUpdateDC.

SEE ALSO:
MGL_wmPushWindowEventHandler, MGL_wmPopWindowEventHandler,
MGL_wmRemoveWindowEventHandler, MGL_wmPushGlobalEventHandler,
MGL_wmPopGlobalEventHandler, MGL_wmRemoveGlobalEventHander,
MGL_wmCaptureEvents, MGL_wmUncaptureEvents
****************************************************************************/
ibool MGLAPI MGL_wmProcessEvent(
    winmng_t *wm,
    event_t *event)
{
    window_t              *wnd;
    windowevententry_t    *ew;
    globalevententry_t    *eg;
    captureentry_t        *ce;

    /* handle cursor changes: */
    if ((event->what & EVT_MOUSEMOVE) || wm->activeWnd == NULL) {
        /* TODO: is this fast enough? Needs testing. */
        wnd = MGL_wmGetWindowAtPosition(wm, event->where_x, event->where_y);
        if (wnd != wm->activeWnd) {
            if (!wm->globalCursor &&
                (wm->activeWnd == NULL || wnd->cursor != wm->activeWnd->cursor))
                MS_setCursor(wnd->cursor);
            wm->activeWnd = wnd;
            }
        }

    wnd = wm->activeWnd;

    /* process with global handlers: */
    for (eg = wm->globalEventHandlers; eg; eg = eg->next) {
        if ((eg->mask & event->what) && eg->hndFunc(event))
            return true;
        }

    /* redirect captured events: */
    for (ce = wm->capturedEvents; ce; ce = ce->next) {
        if ((ce->mask & event->what) && windowIsVisible(ce->wnd)) {
            wnd = ce->wnd;
            break;
            }
        }

    /* process normal events: */
    for (ew = wnd->eventHandlers; ew; ew = ew->next) {
        if ((ew->mask & event->what) && ew->hndFunc(wnd, event))
            return true;
        }

    return false;
}


/****************************************************************************
DESCRIPTION:
Adds event handler to the window.

HEADER:
mgraph.h

PARAMETERS:
wnd         - window
hndFunc     - handler callback function
mask        - mask of events the handler will handle (for event_t.what)
id          - arbitrary user-choosen ID of this handler, used only when
              removing the event handler from the middle of handlers stack

REMARKS:
This function adds event handler to the stack of window's event handlers. These
handlers are callback functions that are called by MGL_wmProcessEvent when
it is determined that the event belongs to this window and event handler.

The algorithm used to determine target window for an event is described in
MGL_wmProcessEvent documentation. The event is passed to the first event handler
on the stack whose mask produces non-zero result of bitwise and with
event_t.what member of the event.

An event handler returns true if it processed the event (that is, the event is
processed no longer) or false if it didn't (in which case the event goes to
the next handler on the stack).

Note: Handlers that were added later take precedence over earlier pushed ones.

SEE ALSO:
MGL_wmProcessEvent, MGL_wmPopWindowEventHandler,
MGL_wmRemoveWindowEventHandler, MGL_wmPushGlobalEventHandler,
MGL_wmPopGlobalEventHandler, MGL_wmRemoveGlobalEventHander,
MGL_wmCaptureEvents, MGL_wmUncaptureEvents
****************************************************************************/
void MGLAPI MGL_wmPushWindowEventHandler(
    window_t *wnd,
    windoweventhandler_t hndFunc,
    ulong mask,
    int id)
{
    windowevententry_t *e;

    if ((e = PM_malloc(sizeof(windowevententry_t))) == NULL)
        FATALERROR(grNoMem);
    e->mask = mask;
    e->hndFunc = hndFunc;
    e->id = id;
    e->next = wnd->eventHandlers;
    wnd->eventHandlers = e;
}


/****************************************************************************
DESCRIPTION:
Removes top most event handler of the window.

HEADER:
mgraph.h

PARAMETERS:
wnd         - window

RETURNS:
False if the stack was already empty, true otherwise.

REMARKS:
This function removes the handler on the top of handlers stack of the window,
if there is any.

SEE ALSO:
MGL_wmProcessEvent, MGL_wmPushWindowEventHandler,
MGL_wmRemoveWindowEventHandler, MGL_wmPushGlobalEventHandler,
MGL_wmPopGlobalEventHandler, MGL_wmRemoveGlobalEventHander,
MGL_wmCaptureEvents, MGL_wmUncaptureEvents
****************************************************************************/
ibool MGLAPI MGL_wmPopWindowEventHandler(
    window_t *wnd)
{
    windowevententry_t *e;

    e = wnd->eventHandlers;
    if (e == NULL) return false;
    wnd->eventHandlers = e->next;
    PM_free(e);
    return true;
}


/****************************************************************************
DESCRIPTION:
Removes event handler of the window.

HEADER:
mgraph.h

PARAMETERS:
wnd         - window
id          - ID of the handler to remove

RETURNS:
False if the stack did not contain handler with this ID, true otherwise.

REMARKS:
This function removes the handler with given ID (as passed to
MGL_wmPushWindowHandler) from window's handlers stack. There may not be any
handler with such ID; in that case, the function returns false.

SEE ALSO:
MGL_wmProcessEvent, MGL_wmPushWindowEventHandler,
MGL_wmPopWindowEventHandler, MGL_wmPushGlobalEventHandler,
MGL_wmPopGlobalEventHandler, MGL_wmRemoveGlobalEventHander,
MGL_wmCaptureEvents, MGL_wmUncaptureEvents
****************************************************************************/
ibool MGLAPI MGL_wmRemoveWindowEventHandler(
    window_t *wnd,
    int id)
{
    windowevententry_t *e, *e2;

    e = wnd->eventHandlers;
    if (e == NULL) return false;
    if (e->id == id)
        wnd->eventHandlers = e->next;
    else {
        while (e->next && e->next->id != id) e = e->next;
        if (e->next == NULL) return false;
        e2 = e->next;
        e->next = e->next->next;
        e = e2;
        }
    PM_free(e);
    return true;
}


/****************************************************************************
DESCRIPTION:
Adds event handler to the window manager.

HEADER:
mgraph.h

PARAMETERS:
wm          - window manager
hndFunc     - handler callback function
mask        - mask of events the handler will handle (for event_t.what)
id          - arbitrary user-choosen ID of this handler, used only when
              removing the event handler from the middle of handlers stack

REMARKS:
This function adds event handler to the stack of window manager's global
event handlers. The meaning of parameters is same as in
MGL_wmPushWindowEventHandler and they work in very similar way.
The only difference is that global event handlers are independent of windows
(they don't have window parameter) and that they take precedence over
window-specific handlers.

SEE ALSO:
MGL_wmProcessEvent, MGL_wmPushWindowEventHandler, MGL_wmPopWindowEventHandler,
MGL_wmRemoveWindowEventHandler,
MGL_wmPopGlobalEventHandler, MGL_wmRemoveGlobalEventHander,
MGL_wmCaptureEvents, MGL_wmUncaptureEvents
****************************************************************************/
void MGLAPI MGL_wmPushGlobalEventHandler(
    winmng_t *wm,
    globaleventhandler_t hndFunc,
    ulong mask,
    int id)
{
    globalevententry_t *e;

    if ((e = PM_malloc(sizeof(globalevententry_t))) == NULL)
        FATALERROR(grNoMem);
    e->mask = mask;
    e->hndFunc = hndFunc;
    e->id = id;
    e->next = wm->globalEventHandlers;
    wm->globalEventHandlers = e;
}


/****************************************************************************
DESCRIPTION:
Removes top most global event handler.

HEADER:
mgraph.h

PARAMETERS:
wm          - window manager

RETURNS:
False if the stack was already empty, true otherwise.

REMARKS:
This function removes the handler on the top of global event handlers stack,
if there is any.

SEE ALSO:
MGL_wmProcessEvent, MGL_wmPushWindowEventHandler, MGL_wmPopWindowEventHandler,
MGL_wmRemoveWindowEventHandler, MGL_wmPushGlobalEventHandler,
MGL_wmRemoveGlobalEventHander, MGL_wmCaptureEvents, MGL_wmUncaptureEvents
****************************************************************************/
ibool MGLAPI MGL_wmPopGlobalEventHandler(
    winmng_t *wm)
{
    globalevententry_t *e;

    e = wm->globalEventHandlers;
    if (e == NULL) return false;
    wm->globalEventHandlers = e->next;
    PM_free(e);
    return true;
}


/****************************************************************************
DESCRIPTION:
Removes global event handler.

HEADER:
mgraph.h

PARAMETERS:
wm          - window
id          - ID of the handler to remove

RETURNS:
False if the stack did not contain handler with this ID, true otherwise.

REMARKS:
This function removes the handler with given ID (as passed to
MGL_wmPushWindowHandler) from global event handlers stack. There may not be any
handler with such ID; in that case, the function returns false.

SEE ALSO:
MGL_wmProcessEvent, MGL_wmPushWindowEventHandler,
MGL_wmPopWindowEventHandler, MGL_wmRemoveWindowEventHandler,
MGL_wmPushGlobalEventHandler, MGL_wmPopGlobalEventHandler,
MGL_wmCaptureEvents, MGL_wmUncaptureEvents
****************************************************************************/
ibool MGLAPI MGL_wmRemoveGlobalEventHandler(
    winmng_t *wm,
    int id)
{
    globalevententry_t *e, *e2;

    e = wm->globalEventHandlers;
    if (e == NULL) return false;
    if (e->id == id)
        wm->globalEventHandlers = e->next;
    else {
        while (e->next && e->next->id != id) e = e->next;
        if (e->next == NULL) return false;
        e2 = e->next;
        e->next = e->next->next;
        e = e2;
        }
    PM_free(e);
    return true;
}


/****************************************************************************
DESCRIPTION:
Captures events and redirects them to specific window.

HEADER:
mgraph.h

PARAMETERS:
wnd         - window
mask        - mask of events to capture (for event_t.what)
id          - arbitrary user-specified ID of the capture entry
              (used when removing the entry with MGL_wmUncaptureEvents)

REMARKS:
It is sometimes useful to redirect certain kinds of events to a particular
window instead of to the one under mouse pointer (e.g. you may want to send
all keyboard events to the window that has focus in a program with Windows
feel). This function allows you to redirect all events whose mask matches
event_t.what of the event (i.e. event_t.what & mask != 0) to the wnd window.

You may capture events several times, to the same window or to different ones.
Capture entries behave like stack: the ones that were added later take
precedence over earlier registered ones.

The ID is used to uniquely identify the entry when removing it and you must
ensure they are unique among all captures that are active at the same time.

SEE ALSO:
MGL_wmProcessEvent, MGL_wmPushWindowEventHandler, MGL_wmPopWindowEventHandler,
MGL_wmRemoveWindowEventHandler, MGL_wmPushGlobalEventHandler,
MGL_wmPopGlobalEventHandler, MGL_wmRemoveGlobalEventHandler,
MGL_wmUncaptureEvents
****************************************************************************/
void MGLAPI MGL_wmCaptureEvents(
    window_t *wnd,
    ulong mask,
    int id)
{
    captureentry_t *e;

    if ((e = PM_malloc(sizeof(captureentry_t))) == NULL)
        FATALERROR(grNoMem);
    e->mask = mask;
    e->wnd = wnd;
    e->id = id;
    e->next = wnd->wm->capturedEvents;
    wnd->wm->capturedEvents = e;
}


/****************************************************************************
DESCRIPTION:
Captures events and redirects them to specific window.

HEADER:
mgraph.h

PARAMETERS:
wnd         - window
id          - ID of the capture record to remove

REMARKS:
Cancels the instruction to capture events to the window as previously
specified by a call to MGL_wmCaptureEvents.

SEE ALSO:
MGL_wmProcessEvent, MGL_wmPushWindowEventHandler, MGL_wmPopWindowEventHandler,
MGL_wmRemoveWindowEventHandler, MGL_wmPushGlobalEventHandler,
MGL_wmPopGlobalEventHandler, MGL_wmRemoveGlobalEventHandler,
MGL_wmCaptureEvents
****************************************************************************/
void MGLAPI MGL_wmUncaptureEvents(
    window_t *wnd,
    int id)
{
    captureentry_t *e, *e2;

    e = wnd->wm->capturedEvents;
    if (e == NULL) return;
    if (e->wnd == wnd && e->id == id)
        wnd->wm->capturedEvents = e->next;
    else {
        while (e->next &&
              (e->next->wnd != wnd || e->next->id != id)) e = e->next;
        if (e->next == NULL) return;
        e2 = e->next;
        e->next = e->next->next;
        e = e2;
        }
    PM_free(e);
}
