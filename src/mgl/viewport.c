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
* Description:  Viewport control routines
*
****************************************************************************/

#include "mgl.h"

/*------------------------- Implementation --------------------------------*/

/****************************************************************************
PARAMETERS:
dc  - Device context to re-compute clip region for

REMARKS:
Generic routine to set the internal clipping rectangle values. This routine
simply sets both the viewport space and screen space clipping rectangles. We
also ensure that the clip rectangle does not go outside of the full screen
clip space.

We also set the internal clipping rectangle to be translated into screen
space, along with fixed point versions of the same thing.
****************************************************************************/
static void __MGL_reComputeClipRegion(
    MGLDC *dc,
    rect_t *clipRectScreen,
    int adjustX,
    int adjustY)
{
    region_t    *clipRegion,*temp;

    /* First create the initial internal clip region from the user defined clip
     * rectangle or clip region, transformed into screen space and clipped
     * against the current screen clip rectangle. We guarantee that all the
     * clip rectangles are the bounding rectangles for all complex clip
     * regions, therefore the incoming screen clip rectangle will encompass
     * the entire complex clip region fully so we can clip the user region
     * against this to eliminate some work.
     */
    if (dc->clipRegionUser) {
        MGL_offsetRect(*clipRectScreen,-adjustX,-adjustY);
        clipRegion = MGL_sectRegionRect(dc->clipRegionUser,clipRectScreen);
        MGL_offsetRect(*clipRectScreen,adjustX,adjustY);
        MGL_offsetRegion(clipRegion,adjustX,adjustY);
        }
    else {
        clipRegion = MGL_rgnSolidRectCoord(
            clipRectScreen->left,clipRectScreen->top,
            clipRectScreen->right,clipRectScreen->bottom);
        }

    /* Intersect the current clip region with the MGL window manager
     * visible region to get the next internal clip region.
     */
    if (dc->visRegionWM) {
        temp = MGL_sectRegion(dc->visRegionWM,clipRegion);
        MGL_freeRegion(clipRegion);
        clipRegion = temp;
        }

    /* Intersect the current clip region with the windowed device context
     * visible region to get the final internal clip region.
     */
    if (dc->visRegionWin) {
        temp = MGL_sectRegion(dc->visRegionWin,clipRegion);
        MGL_freeRegion(clipRegion);
        clipRegion = temp;
        }

    /* Finally check to see if the resulting clip region is simple, and if
     * it is simply set the screen clip rectangle to the bounding rectangle
     * for the region and then destroy the internal clip region.
     */
    MGL_optimizeRegion(clipRegion);
    if (MGL_isSimpleRegion(clipRegion)) {
        clipRectScreen->left    = clipRegion->rect.left;
        clipRectScreen->top     = clipRegion->rect.top;
        clipRectScreen->right   = clipRegion->rect.right;
        clipRectScreen->bottom  = clipRegion->rect.bottom;
        MGL_freeRegion(clipRegion);
        clipRegion = NULL;
        }

    /* Save the final clip region */
    dc->clipRegionScreen = clipRegion;
}

/****************************************************************************
PARAMETERS:
dc  - Device context to recompute the internal clipping parameters for

REMARKS:
Internal routine to recompute all the internal clipping parameters for the
device context, taking into account the user supplied clip region,
window manager clip region and windowed device context clip region. This
routine simply sets both the viewport space and screen space clipping
rectangles. We also ensure that the clip rectangle does not go outside of
the full screen clip space.

We also set the internal clipping rectangle to be translated into screen
space, along with fixed point versions of the same thing.
{secret}
****************************************************************************/
void __MGL_recomputeClipping(
    MGLDC *dc)
{
    rect_t  r;
    int     adjustX = dc->viewPort.left - dc->viewPortOrg.x;
    int     adjustY = dc->viewPort.top - dc->viewPortOrg.y;

    /* Clip user supplied clip rectangle to screen boundary, viewport
     * and visible rectangle boundaries
     */
    r = dc->clipRectUser;
    MGL_offsetRect(r,adjustX,adjustY);
    MGL_sectRectFast(r,dc->bounds,&r);
    MGL_sectRectFast(r,dc->viewPort,&r);
    MGL_sectRectFast(r,dc->visRectWM,&r);
    MGL_sectRectFast(r,dc->visRectWin,&r);

    /* Destroy any pre-existing complex screen clipping region */
    if (dc->clipRegionScreen) {
        MGL_freeRegion(dc->clipRegionScreen);
        dc->clipRegionScreen = NULL;
        }

    /* If we have any complex clip regions, compute the final
     * screen space complex clip region first before setting the
     * screen space clip regions. If the combined complex clip
     * regions boil down to a simple rectangle, this will end
     * up being the final screen space clip rectangle below.
     */
    if (dc->clipRegionUser || dc->visRegionWM || dc->visRegionWin)
        __MGL_reComputeClipRegion(dc,&r,adjustX,adjustY);

    /* Now set internal clipping rectangles */
    dc->clipRectView.left = r.left - adjustX;
    dc->clipRectView.top = r.top - adjustY;
    dc->clipRectView.right = r.right - adjustX;
    dc->clipRectView.bottom = r.bottom - adjustY;
    dc->clipRectViewFX.left = MGL_TOFIX(r.left);
    dc->clipRectViewFX.top = MGL_TOFIX(r.top);
    dc->clipRectViewFX.right = MGL_TOFIX(r.right);
    dc->clipRectViewFX.bottom = MGL_TOFIX(r.bottom);
    dc->clipRectScreen.left = r.left;
    dc->clipRectScreen.top = r.top;
    dc->clipRectScreen.right = r.right;
    dc->clipRectScreen.bottom = r.bottom;
    dc->clipRectScreenFX.left = MGL_TOFIX(dc->clipRectScreen.left);
    dc->clipRectScreenFX.top = MGL_TOFIX(dc->clipRectScreen.top);
    dc->clipRectScreenFX.right = MGL_TOFIX(dc->clipRectScreen.right);
    dc->clipRectScreenFX.bottom = MGL_TOFIX(dc->clipRectScreen.bottom);

    /* Make sure we flush to the appropriate DC's as necessary */
    if (dc == &DC) {
        _MGL_dcPtr->clipRectView        = dc->clipRectView;
        _MGL_dcPtr->clipRectViewFX      = dc->clipRectViewFX;
        _MGL_dcPtr->clipRectScreen      = dc->clipRectScreen;
        _MGL_dcPtr->clipRectScreenFX    = dc->clipRectScreenFX;
        _MGL_dcPtr->clipRegionScreen    = dc->clipRegionScreen;
        }
    else if (dc == _MGL_dcPtr) {
        DC.clipRectView                 = dc->clipRectView;
        DC.clipRectViewFX               = dc->clipRectViewFX;
        DC.clipRectScreen               = dc->clipRectScreen;
        DC.clipRectScreenFX             = dc->clipRectScreenFX;
        DC.clipRegionScreen             = dc->clipRegionScreen;
        }
}

/****************************************************************************
DESCRIPTION:
Sets the currently active viewport.

HEADER:
mgraph.h

PARAMETERS:
view    - New global viewport bounding rectangle

REMARKS:
Sets the dimensions of the currently active viewport. These dimensions are global to
the entire display area used by the currently active video device driver. Note that
when the viewport is changing, the viewport origin and current position is always
reset back to (0,0).

All output in MGL is relative to the current viewport, so by changing the viewport
to a new value you can make all output appear in a different rectangular portion of
the video display.

Note:   Setting the viewport also implicitly sets the clip rectangle to cover
        the entire viewport bounds, which also clears any user defined complex
        clip region.

SEE ALSO:
MGL_getViewport, MGL_setRelViewport, MGL_clearViewport,
MGL_setClipRect, MGL_setViewportOrg
****************************************************************************/
void MGLAPI MGL_setViewport(
    rect_t view)
{
    MGL_setViewportDC(&DC,view);
}

/****************************************************************************
DESCRIPTION:
Sets the currently active viewport for a specific DC.

HEADER:
mgraph.h

PARAMETERS:
dc      - Device context to change viewport for
view    - New global viewport bounding rectangle

REMARKS:
This function is the same as MGL_setViewport, however the device context
does not have to be the current device context.

SEE ALSO:
MGL_setViewport, MGL_getViewport, MGL_setRelViewport, MGL_clearViewport,
MGL_setClipRect, MGL_setViewportOrg
****************************************************************************/
void MGLAPI MGL_setViewportDC(
    MGLDC *dc,
    rect_t view)
{
    /* Set the internal viewport variables */
    dc->viewPort.left       = (view.left += dc->size.left);
    dc->viewPort.top        = (view.top += dc->size.top);
    dc->viewPort.right      = (view.right += dc->size.left);
    dc->viewPort.bottom     = (view.bottom += dc->size.top);
    dc->viewPortFX.left     = MGL_TOFIX(dc->viewPort.left);
    dc->viewPortFX.top      = MGL_TOFIX(dc->viewPort.top);
    dc->viewPortFX.right    = MGL_TOFIX(dc->viewPort.right);
    dc->viewPortFX.bottom   = MGL_TOFIX(dc->viewPort.bottom);

    /* Reset the current position and viewport origin */
    dc->viewPortOrg.x       = 0;
    dc->viewPortOrg.y       = 0;
    dc->a.CP.x              = 0;
    dc->a.CP.y              = 0;

    /* Reset the clip rectangle to the entire viewport bounds */
    view.right -= view.left;
    view.bottom -= view.top;
    view.left = view.top = 0;
    MGL_setClipRectDC(dc,view);

    /* Make sure we flush to the appropriate DC's as necessary */
    if (dc == &DC) {
        _MGL_dcPtr->viewPort        = dc->viewPort;
        _MGL_dcPtr->viewPortFX      = dc->viewPortFX;
        _MGL_dcPtr->viewPortOrg     = dc->viewPortOrg;
        _MGL_dcPtr->a.CP            = dc->a.CP;
        }
    else if (dc == _MGL_dcPtr) {
        DC.viewPort                 = dc->viewPort;
        DC.viewPortFX               = dc->viewPortFX;
        DC.viewPortOrg              = dc->viewPortOrg;
        DC.a.CP                     = dc->a.CP;
        }
}

/****************************************************************************
DESCRIPTION:
Sets a viewport relative to the current viewport.

HEADER:
mgraph.h

PARAMETERS:
view    - Bounding rectangle for the new viewport

REMARKS:
Sets the current viewport to the viewport specified by view, relative to the currently
active viewport. The new viewport is restricted to fall within the bounds of the
currently active viewport. Note that when the viewport is changing, the viewport
origin is always reset back to (0,0).

All output in MGL is relative to the current viewport, so by changing the viewport
to a new value you can make all output appear in a different rectangular portion of
the video display.

SEE ALSO:
MGL_getViewport, MGL_setViewport, MGL_clearViewport, MGL_setClipRect,
MGL_setViewportOrg
****************************************************************************/
void MGLAPI MGL_setRelViewport(
    rect_t view)
{
    MGL_setRelViewportDC(&DC,view);
}

/****************************************************************************
DESCRIPTION:
Sets a viewport relative to the current viewport for a specific DC.

HEADER:
mgraph.h

PARAMETERS:
dc      - Device context to change viewport for
view    - Bounding rectangle for the new viewport

REMARKS:
This function is the same as MGL_setRelViewport, however the device context
does not have to be the current device context.

SEE ALSO:
MGL_setRelViewport, MGL_getViewport, MGL_setViewport, MGL_clearViewport,
MGL_setClipRect, MGL_setViewportOrg
****************************************************************************/
void MGLAPI MGL_setRelViewportDC(
    MGLDC *dc,
    rect_t view)
{
    MGL_offsetRect(view,dc->viewPort.left - dc->size.left,dc->viewPort.top - dc->size.top);
    MGL_setViewportDC(dc,view);
}

/****************************************************************************
DESCRIPTION:
Sets the logical viewport origin.

HEADER:
mgraph.h

PARAMETERS:
org - New logical viewport origin.

REMARKS:
This function sets the currently active viewport origin. When a new viewport is set
with the MGL_setViewport function, the viewport origin is reset to (0,0), which
means that any primitives drawn at pixel location (0,0) will appear at the top left
hand corner of the viewport.

You can change the logical coordinate of the viewport origin to any value you
please, which will effectively offset all drawing within the currently active
viewport. Hence if you set the viewport origin to (10,10), drawing a pixel at (10,10)
would make it appear at the top left hand corner of the viewport.

SEE ALSO:
MGL_setViewportOrgDC, MGL_getViewportOrg, MGL_setViewport
****************************************************************************/
void  MGLAPI MGL_setViewportOrg(
    point_t org)
{
    MGL_setViewportOrgDC(&DC,org);
}

/****************************************************************************
DESCRIPTION:
Sets the logical viewport origin for a specific DC.

HEADER:
mgraph.h

PARAMETERS:
dc  - Device context to change viewport for
org - New logical viewport origin.

REMARKS:
This function is the same as MGL_setViewportOrg, however the device context
does not have to be the current device context.

SEE ALSO:
MGL_setViewportOrg, MGL_getViewportOrg, MGL_setViewport
****************************************************************************/
void  MGLAPI MGL_setViewportOrgDC(
    MGLDC *dc,
    point_t org)
{
    /* Set the viewport origin */
    dc->viewPortOrg = org;
    MGL_offsetRect(dc->viewPort,-org.x,-org.y);

    /* Make sure we flush to the appropriate DC's as necessary */
    if (dc == &DC)
        _MGL_dcPtr->viewPortOrg = dc->viewPortOrg;
    else if (dc == _MGL_dcPtr)
        DC.viewPortOrg = dc->viewPortOrg;
}

/****************************************************************************
DESCRIPTION:
Returns the current viewport origin.

HEADER:
mgraph.h

PARAMETERS:
org - Place to store the viewport origin

REMARKS:
This function returns a copy of the currently active viewport origin. When a new
viewport is set with the MGL_setViewport function, the viewport origin is reset to
(0,0), which means that any primitives drawn at pixel location (0,0) will appear at
the top left hand corner of the viewport.

You can change the logical coordinate of the viewport origin to any value you
please, which will effectively offset all drawing within the currently active
viewport. Hence if you set the viewport origin to (10,10), drawing a pixel at (10,10)
would make it appear at the top left hand corner of the viewport.

SEE ALSO:
MGL_getViewportOrgDC, MGL_setViewport, MGL_setViewportOrg
****************************************************************************/
void MGLAPI MGL_getViewportOrg(
    point_t *org)
{
    MGL_getViewportOrgDC(&DC,org);
}

/****************************************************************************
DESCRIPTION:
Returns the current viewport origin for a specific DC.

HEADER:
mgraph.h

PARAMETERS:
dc  - Device context to change viewport for
org - Place to store the viewport origin

REMARKS:
This function is the same as MGL_getViewportOrg, however the device context
does not have to be the current device context.

SEE ALSO:
MGL_getViewportOrg, MGL_setViewport, MGL_setViewportOrg
****************************************************************************/
void MGLAPI MGL_getViewportOrgDC(
    MGLDC *dc,
    point_t *org)
{
    *org = dc->viewPortOrg;
}

/****************************************************************************
DESCRIPTION:
Returns the currently active viewport.

HEADER:
mgraph.h

PARAMETERS:
view    - Place to store the current viewport

REMARKS:
This function returns a copy of the currently active viewport. These dimensions are
global to the entire device context surface. When the viewport is changed with this
function, the viewport origin is reset to (0,0).

All output in MGL is relative to the current viewport, so by changing the viewport
to a new value you can make all output appear in a different rectangular portion of
the device surface.

SEE ALSO:
MGL_getViewportDC, MGL_setViewport, MGL_setRelViewport, MGL_setViewportOrg,
MGL_clearViewport, MGL_setClipRect
****************************************************************************/
void MGLAPI MGL_getViewport(
    rect_t *view)
{
    MGL_getViewportDC(&DC,view);
}

/****************************************************************************
DESCRIPTION:
Returns the currently active viewport for a specific DC.

HEADER:
mgraph.h

PARAMETERS:
dc      - Device context to change viewport for
view    - Place to store the current viewport

REMARKS:
This function is the same as MGL_getViewport, however the device context
does not have to be the current device context.

SEE ALSO:
MGL_getViewport, MGL_setViewport, MGL_setRelViewport, MGL_setViewportOrg,
MGL_clearViewport, MGL_setClipRect
****************************************************************************/
void MGLAPI MGL_getViewportDC(
    MGLDC *dc,
    rect_t *view)
{
    view->left      = dc->viewPort.left - dc->size.left;
    view->top       = dc->viewPort.top - dc->size.top;
    view->right     = dc->viewPort.right - dc->size.left;
    view->bottom    = dc->viewPort.bottom - dc->size.top;
}

/****************************************************************************
DESCRIPTION:
Converts a point from global to local coordinates.

HEADER:
mgraph.h

PARAMETERS:
dc  - Device context in which the point is defined
p   - Pointer to point to be converted

REMARKS:
This function converts a coordinate from global coordinates to local coordinates.
Global coordinates are defined relative to the entire output device context surface,
while local coordinates are relative to the currently active viewport.

This routine is usually used to convert mouse coordinate values from global screen
coordinates to the local coordinate system of the currently active viewport.

SEE ALSO:
MGL_globalToLocalDC, MGL_localToGlobal
****************************************************************************/
void MGLAPI MGL_globalToLocal(
    point_t *p)
{
    p->x -= DC.viewPort.left;
    p->y -= DC.viewPort.top;
}

/****************************************************************************
DESCRIPTION:
Converts a point from global to local coordinates.

HEADER:
mgraph.h

PARAMETERS:
dc  - Device context in which the point is defined
p   - Pointer to point to be converted

REMARKS:
This function is the same as MGL_globalToLocal, however the device context
does not have to be the current device context.

SEE ALSO:
MGL_globalToLocal, MGL_localToGlobal
****************************************************************************/
void MGLAPI MGL_globalToLocalDC(
    MGLDC *dc,
    point_t *p)
{
    p->x -= dc->viewPort.left;
    p->y -= dc->viewPort.top;
}

/****************************************************************************
DESCRIPTION:
Converts a point from local to global coordinates.

HEADER:
mgraph.h

PARAMETERS:
p   - Pointer to point to be converted

REMARKS:
This function converts a coordinate from local coordinates to global coordinates.
Global coordinates are defined relative to the entire output device display, while
local coordinates are relative to the currently active viewport.

SEE ALSO:
MGL_localToGlobalDC, MGL_globalToLocal
****************************************************************************/
void MGLAPI MGL_localToGlobal(
    point_t *p)
{
    p->x += DC.viewPort.left;
    p->y += DC.viewPort.top;
}

/****************************************************************************
DESCRIPTION:
Converts a point from local to global coordinates for a specific DC.

HEADER:
mgraph.h

PARAMETERS:
dc  - Device context in which the point is defined
p   - Pointer to point to be converted

REMARKS:
This function is the same as MGL_localToGlobal, however the device context
does not have to be the current device context.

SEE ALSO:
MGL_localToGlobal, MGL_globalToLocal
****************************************************************************/
void MGLAPI MGL_localToGlobalDC(
    MGLDC *dc,
    point_t *p)
{
    p->x += dc->viewPort.left;
    p->y += dc->viewPort.top;
}

/****************************************************************************
DESCRIPTION:
Sets the current clipping rectangle.

HEADER:
mgraph.h

PARAMETERS:
clip    - New clipping rectangle to be used

REMARKS:
Sets the current clipping rectangle coordinates. The current clipping
rectangle is used to clip all output, and is always defined as being
relative to the currently active viewport. The clipping rectangle can be
no larger than the currently active viewport, and will be truncated if
an attempt is made to allow clipping outside of the active viewport.

Note:   Setting a new clip rectangle with this function clears any
        existing complex clip region in the device context.

SEE ALSO:
MGL_setClipRectDC, MGL_getClipRect, MGL_setViewport, MGL_getViewport,
MGL_setClipRegion
****************************************************************************/
void MGLAPI MGL_setClipRect(
    rect_t clip)
{
    MGL_setClipRectDC(&DC,clip);
}

/****************************************************************************
PARAMETERS:
dc      - Display device context in which the rectangle is located .
clip    - New clipping rectangle to be used

REMARKS:
Internal function to set the internal clip rectangle. The only differences
between this and the public API function is that this function does
not re-compute the internal clip rectangles and clip regions.
****************************************************************************/
static void __MGL_setClipRect(
    MGLDC *dc,
    rect_t clip)
{
    /* Set the user supplied clipping rectangle */
    dc->clipRectUser = clip;

    /* If a user defined clip region exists, destroy it. We also destroy
     * the screen clipping region here as it will be re-created again
     * later if we have any visible clip regions that must be accounted for.
     */
    if (dc->clipRegionUser)
        MGL_freeRegion(dc->clipRegionUser);
    if (dc->clipRegionScreen)
        MGL_freeRegion(dc->clipRegionScreen);
    dc->clipRegionUser = NULL;
    dc->clipRegionScreen = NULL;

    /* Make sure we flush to the appropriate DC's as necessary */
    if (dc == &DC) {
        _MGL_dcPtr->clipRectUser        = dc->clipRectUser;
        _MGL_dcPtr->clipRegionUser      = dc->clipRegionUser;
        _MGL_dcPtr->clipRegionScreen    = dc->clipRegionScreen;
        }
    else if (dc == _MGL_dcPtr) {
        DC.clipRectUser                 = dc->clipRectUser;
        DC.clipRegionUser               = dc->clipRegionUser;
        DC.clipRegionScreen             = dc->clipRegionScreen;
        }
}

/****************************************************************************
DESCRIPTION:
Sets the current clipping rectangle for a specific DC.

HEADER:
mgraph.h

PARAMETERS:
dc      - Display device context in which the rectangle is located .
clip    - New clipping rectangle to be used

REMARKS:
This function is the same as MGL_setClipRect, however the device context
does not have to be the current device context.

Note:   Setting a new clip rectangle with this function clears any
        existing complex clip region in the device context.

SEE ALSO:
MGL_setClipRect, MGL_getClipRect, MGL_setViewport, MGL_getViewport,
MGL_setClipRegion
****************************************************************************/
void MGLAPI MGL_setClipRectDC(
    MGLDC *dc,
    rect_t clip)
{
    __MGL_setClipRect(dc,clip);
    __MGL_recomputeClipping(dc);
}

/****************************************************************************
DESCRIPTION:
Returns the current clipping rectangle.

HEADER:
mgraph.h

PARAMETERS:
clip    - Place to store the current clipping rectangle

REMARKS:
Returns the current clipping rectangle coordinates. The current clipping
rectangle is used to clip all output, and is always defined as being
relative to the currently active viewport. The clipping rectangle can be
no larger than the currently active viewport.

Note:   If a complex clip region is currently active, this function returns
        the bounding rectangle for the complex clip region.

SEE ALSO:
MGL_getClipRectDC, MGL_setClipRect, MGL_getClipRegion
****************************************************************************/
void MGLAPI MGL_getClipRect(
    rect_t *clip)
{
    MGL_getClipRectDC(&DC,clip);
}

/****************************************************************************
DESCRIPTION:
Returns the current clipping rectangle for a specific DC.

HEADER:
mgraph.h

PARAMETERS:
dc      - Display device context in which the rectangle is defined
clip    - Place to store the current clipping rectangle

REMARKS:
This function is the same as MGL_getClipRect, however the device context
does not have to be the current device context.

Note:   If a complex clip region is currently active, this function returns
        the bounding rectangle for the complex clip region.

SEE ALSO:
MGL_getClipRect, MGL_setClipRect, MGL_getClipRegion
****************************************************************************/
void MGLAPI MGL_getClipRectDC(
    MGLDC *dc,
    rect_t *clip)
{
    *clip = dc->clipRectView;
}

/****************************************************************************
DESCRIPTION:
Sets the current complex clipping region.

HEADER:
mgraph.h

PARAMETERS:
clip    - New complex clipping region to be used

REMARKS:
This function sets the current complex clipping region for the current device
context. Complex clip regions are defined as unions of rectangles, and
allow all rendering functions in the MGL to be clipped to arbitrary
regions on the screen. Setting a complex clip region override the current
setting for a simple clip rectangle. The complex clip region is used to
clip all output, and is always defined as being relative to the currently
active viewport. The clipping region can be no larger than the currently
active viewport, and will be truncated if an attempt is made to allow
clipping outside of the active viewport.

SEE ALSO:
MGL_setClipRegionDC, MGL_getClipRegion, MGL_setViewport, MGL_getViewport,
MGL_setClipRect
****************************************************************************/
void MGLAPI MGL_setClipRegion(
    region_t *region)
{
    MGL_setClipRegionDC(&DC,region);
}

/****************************************************************************
DESCRIPTION:
Sets the current complex clipping region for a specific DC.

HEADER:
mgraph.h

PARAMETERS:
dc      - Display device context in which the region is located .
region  - New complex clipping region to be used

REMARKS:
This function sets the current complex clipping region for the specified
device context. Complex clip regions are defined as unions of rectangles, and
allow all rendering functions in the MGL to be clipped to arbitrary
regions on the screen. Setting a complex clip region override the current
setting for a simple clip rectangle. The complex clip region is used to
clip all output, and is always defined as being relative to the currently
active viewport. The clipping region can be no larger than the currently
active viewport, and will be truncated if an attempt is made to allow
clipping outside of the active viewport.

SEE ALSO:
MGL_setClipRegion, MGL_getClipRegion, MGL_setViewport, MGL_getViewport,
MGL_setClipRect
****************************************************************************/
void MGLAPI MGL_setClipRegionDC(
    MGLDC *dc,
    region_t *region)
{
    /* Setup the initial clip rectangle for the region bounds. Note
     * that this also destroys any pre-existing clip region in the
     * DC when __MGL_setClipRect() is called, so dc->clipRegionUser
     * is NULL when that function returns.
     */
    CHECK(region != NULL);
    __MGL_setClipRect(dc,region->rect);

    /* Now if the region is not a simple region, copy it as the
     * user supplied complex clip region.
     */
    if (!MGL_isSimpleRegion(region))
        dc->clipRegionUser = MGL_copyRegion(region);

    /* Make sure we flush to the appropriate DC's as necessary */
    if (dc == &DC)
        _MGL_dcPtr->clipRegionUser      = dc->clipRegionUser;
    else if (dc == _MGL_dcPtr)
        DC.clipRegionUser               = dc->clipRegionUser;

    /* Now recompute the internal clip region based on the clip
     * rectangle and any visible clip region for window clipping.
     */
    __MGL_recomputeClipping(dc);
}

/****************************************************************************
DESCRIPTION:
Returns the current complex clipping region.

HEADER:
mgraph.h

PARAMETERS:
region  - Place to store the current complex clipping region

REMARKS:
Returns the current complex clipping region for the current device context.
This function works even if no complex clip region is active, and only
a simple clip rectangle is in use. In this case, the resulting clip region
is a simple region that contains only the clip rectangle for the device
context.

SEE ALSO:
MGL_getClipRegionDC, MGL_setClipRegion, MGL_setClipRect
****************************************************************************/
void MGLAPI MGL_getClipRegion(
    region_t *region)
{
    MGL_getClipRegionDC(&DC,region);
}

/****************************************************************************
DESCRIPTION:
Returns the current complex clipping region for a specific DC.

HEADER:
mgraph.h

PARAMETERS:
dc      - Device context to get clip region for
region  - Place to store the current complex clipping region

REMARKS:
Returns the current complex clipping region for the specific device context.
This function works even if no complex clip region is active, and only
a simple clip rectangle is in use. In this case, the resulting clip region
is a simple region that contains only the clip rectangle for the device
context.

SEE ALSO:
MGL_getClipRegion, MGL_setClipRegion, MGL_setClipRect
****************************************************************************/
void MGLAPI MGL_getClipRegionDC(
    MGLDC *dc,
    region_t *region)
{
    CHECK(region != NULL);
    if (dc->clipRegionUser)
        MGL_copyIntoRegion(region,dc->clipRegionUser);
    else {
        MGL_clearRegion(region);
        region->rect = dc->clipRectUser;
        }
}

