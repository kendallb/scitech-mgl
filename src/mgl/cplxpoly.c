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
* Description:  Complex polygon drawing routines with clipping. Polygon
*               clipping is done on a scissoring basis, where each scan
*               line is clipped individually, rather than using a routine
*               such as the Sutherland Hodgeman polygon clipping algorithm.
*               This approach has been taken since we don't expect to clip
*               polygons all that often, and those that are clipped will
*               only require a small amount of clipping.
*
*               Complex polygon drawing algorithm. This algorithm will
*               scan convert any arbitrary polygon, including ones that
*               are self-intersecting.
*
****************************************************************************/

#include "mgl.h"

/*--------------------------- Global Variables ----------------------------*/

static scanLineFunc drawScanLine;

/* Active edge table and global edge table entries for complex filled
 * polygons.
 * {secret}
 */
typedef struct edgeState {
    struct edgeState    *next;          /* Next edge in list            */
    int                 x;              /* Current x value for edge     */
    int                 startY;         /* Starting y coord for edge    */
    int                 wholePixelXMove;/* Amount to move in x          */
    int                 xDirection;     /* Direction of x motion travel */
    int                 error;          /* Current error for this edge  */
    int                 errorAdjUp;     /* Error increment adjustment   */
    int                 errorAdjDown;   /* Error decrement adjustment   */
    int                 count;          /* Number of scan lines in edge */
    } edgeState;

/* Pointers to the global edge table (GET) and the active edge table (AET)
 * for the current polygon.
 */

static edgeState    *GETPtr,*AETPtr;
static region_t     *rgn;

/*------------------------- Implementation --------------------------------*/

/****************************************************************************
PARAMETERS:
count   - Number of vertice in vertex list
vArray  - Array of vertices
vinc    - Increment to get to next vertex
buffer  - Pool of free edgeState table entries to use
xOffset - Offset of x coordinates from (0,0)
yOffset - Offset of y coordinates from (0,0)

REMARKS:
Creates the Global Edge table that contains all the edges in the polygon
in sorted order. We flip endpoints so that the edges always travel from
top to bottom. We store the GET by ascending y starting coordinates and
then by ascending x starting coordinates for edges with common y start
coordinates.
****************************************************************************/
static void buildGET(
    int count,
    fxpoint_t *vArray,
    int vinc,
    edgeState *buffer,
    fix32_t xOffset,
    fix32_t yOffset)
{
    int         i,startX,startY,endX,endY,dy,dx,width;
    fxpoint_t   *p,*prev;
    edgeState   *newEdgePtr;
    edgeState   *followingEdge,* *followingEdgeLink;

    /* Scan through the vertex list and put all non-0-height edges into
     * the GET, sorted by increasing Y start coordinate
     */
    GETPtr = NULL;
    prev = VTX(vArray,vinc,count-1);
    for (i = 0, p = vArray; i < count; i++, prev = p, INCVTX(p,vinc)) {

        /* Calculate the edge height and width  */
        startX = MGL_FIXROUND(p->x + xOffset);
        startY = MGL_FIXROUND(p->y + yOffset);
        endX = MGL_FIXROUND(prev->x + xOffset);
        endY = MGL_FIXROUND(prev->y + yOffset);

        /* Make sure the edge runs top to bottom */
        if (startY > endY) {
            SWAP(startX,endX);
            SWAP(startY,endY);
            }

        /* Skip if this can't ever be an active edge (has 0 height) */
        if ((dy = endY - startY) != 0) {
            newEdgePtr = buffer++;
            newEdgePtr->xDirection = ((dx = endX - startX) > 0) ? 1 : -1;
            width = ABS(dx);
            newEdgePtr->x = startX;     newEdgePtr->startY = startY;
            newEdgePtr->count = dy;     newEdgePtr->errorAdjDown = dy;

            /* Set up initial error term */
            if (dx >= 0)
                newEdgePtr->error = 0;
            else
                newEdgePtr->error = -dy + 1;
            if (dy >= width) {                      /* Y-major edge */
                newEdgePtr->wholePixelXMove = 0;
                newEdgePtr->errorAdjUp = width;
                }
            else {                                  /* X-major edge */
                newEdgePtr->wholePixelXMove =
                    (width / dy) * newEdgePtr->xDirection;
                newEdgePtr->errorAdjUp = width % dy;
                }

            /* Link the new edge into the GET so that the edge list is
             * still sorted by Y coordinate, and by X coordinate for all
             * edges with the same Y coordinate
             */
            followingEdgeLink = &GETPtr;
            for (;;) {
                followingEdge = *followingEdgeLink;
                if ((followingEdge == NULL) ||
                        (followingEdge->startY > startY) ||
                        ((followingEdge->startY == startY) &&
                        (followingEdge->x >= startX))) {
                    newEdgePtr->next = followingEdge;
                    *followingEdgeLink = newEdgePtr;
                    break;
                    }
                followingEdgeLink = &followingEdge->next;
                }
            }
        }
}

/****************************************************************************
PARAMETERS:
cntY    - Start y coordinate of edges to move

REMARKS:
Moves all the edges that start at the specified y coordinate from the
GET to the AET, maintaining the sort order of the AET.

The GET is Y sorted. Any edges that start at the desired Y coordinate will
be first in the GET, so we'll move edges from the GET to AET until the
first edge left in the GET is no longer at the desired Y coordinate. Also,
the GET is X sorted within each Y coordinate, so each successive edge we
add to the AET is guaranteed to belong later in the AET than the one just
added.
****************************************************************************/
static void updateAET(
    int cntY)
{
    edgeState   *AETEdge,* *AETEdgePtr,*tempEdge;
    int         cntX;

    AETEdgePtr = &AETPtr;
    while (GETPtr && (GETPtr->startY == cntY)) {
        cntX = GETPtr->x;

        /* Link the new edge into the AET so that the AET is still
         * sorted by X coordinate
         */
        for (;;) {
            AETEdge = *AETEdgePtr;
            if ((AETEdge == NULL) || (AETEdge->x >= cntX)) {
                tempEdge = GETPtr->next;
                *AETEdgePtr = GETPtr;   /* link the edge into the AET   */
                GETPtr->next = AETEdge;
                AETEdgePtr = &GETPtr->next;
                GETPtr = tempEdge;      /* unlink the edge from the GET */
                break;
                }
            else {
                AETEdgePtr = &AETEdge->next;
                }
            }
        }
}

/****************************************************************************
PARAMETERS:
cntY    - Y coordinate of scan line to output

REMARKS:
Fills in the scan lines described by the current AET entries at the
specified y coordinate using the odd/even parity rule to determine internal
and external scan lines.

Scan through the AET, drawing line segments as each pair of edge crossings
is encountered. The nearest pixel on or to the right of left edges is
drawn, and the nearest pixel to the left of but not on right edges is drawn.
****************************************************************************/
static void scanOutAET(
    int cntY)
{
    int         leftX;
    edgeState   *cntEdge;

    cntEdge = AETPtr;
    while (cntEdge) {
        leftX = cntEdge->x;
        cntEdge = cntEdge->next;
        drawScanLine(cntY,leftX,cntEdge->x);
        cntEdge = cntEdge->next;
        }
}

/****************************************************************************
PARAMETERS:
cntY    - Y coordinate of scan line to output

REMARKS:
Fills in the scan lines described by the current AET entries at the
specified y coordinate using the odd/even parity rule to determine internal
and external scan lines.

Scan through the AET, drawing line segments as each pair of edge crossings
is encountered. The nearest pixel on or to the right of left edges is
drawn, and the nearest pixel to the left of but not on right edges is drawn.
****************************************************************************/
static void scanOutRgnAET(
    int cntY)
{
    int         x1,x2;
    edgeState   *cntEdge;
    rect_t      scan;

    cntEdge = AETPtr;
    while (cntEdge) {
        x1 = cntEdge->x;
        cntEdge = cntEdge->next;
        x2 = cntEdge->x;
        if (x2 < x1)
            SWAP(x1,x2);
        if (x1 < x2) {
            scan.left = x1;
            scan.top = cntY;
            scan.right = x2;
            scan.bottom = cntY+1;
            MGL_unionRegionRect(rgn,&scan);
            }
        cntEdge = cntEdge->next;
        }
}

/****************************************************************************
REMARKS:
Advances each edges currently in the AET table by one scan line. If the
edge drops out of scope, we remove it from the AET, otherwise we recalculate
the next x coordinate for this edge.
****************************************************************************/
static void advanceAET(void)
{
    edgeState   *cntEdge,* *cntEdgePtr;

    cntEdgePtr = &AETPtr;
    while ((cntEdge = *cntEdgePtr) != NULL) {
        /* Count off one scan line for this edge    */
        if (--(cntEdge->count) == 0) {
            /* This edge has been completely scanned, so remove it from
             * the AET
             */
            *cntEdgePtr = cntEdge->next;
            }
        else {
            /* Advance the edge's x coordinate by the minimum move */
            cntEdge->x += cntEdge->wholePixelXMove;

            /* Determine whether it's time for X to advance one extra */
            if ((cntEdge->error += cntEdge->errorAdjUp) > 0) {
                cntEdge->x += cntEdge->xDirection;
                cntEdge->error -= cntEdge->errorAdjDown;
                }
            cntEdgePtr = &cntEdge->next;
            }
        }
}

/****************************************************************************
REMARKS:
Sorts the eges currently in the AET table by x coordinate. We do this by
scanning through the AET and swapping any adjacent edges for which the
second edge is at a lower x coordinate than the first edge. Repeat until
no further swapping occurs (bubble sort).

This is a VERY simple sort procedure. We are only expecting to sort a
reasonably small amount of edges, and most of them will always be in
sorted order, so a simple sort like this is perfectly adequate!
****************************************************************************/
static void sortAET(void)
{
    edgeState   *cntEdge,* *cntEdgePtr,*tempEdge;
    ibool       swapOccurred;

    /* Scan through the AET and swap any adjacent edges for which the
     * second edge is at a lower current X coord than the first edge.
     * Repeat until no further swapping is needed.
     */
    if (AETPtr) {
        do {
            swapOccurred = false;
            cntEdgePtr = &AETPtr;
            while ((cntEdge = *cntEdgePtr)->next != NULL) {
                if (cntEdge->x > cntEdge->next->x) {
                    /* The second edge has a lower X than the first,
                     * so swap them in the AET.
                     */
                    tempEdge = cntEdge->next->next;
                    *cntEdgePtr = cntEdge->next;
                    cntEdge->next->next = cntEdge;
                    cntEdge->next = tempEdge;
                    swapOccurred = true;
                    }
                cntEdgePtr = &(*cntEdgePtr)->next;
                }
            } while (swapOccurred);
        }
}

/****************************************************************************
PARAMETERS:
count   - Number of vertices to draw
vArray  - Array of vertices
vinc    - Increment to get to next vertex
xOffset - Offset of x coordinates from (0,0)
yOffset - Offset of y coordinates from (0,0)

REMARKS:
Renders a complex polygon using the following algorithm:

    - Build the global edge table (GET)
    - Set the Active Edge table to empty (AET)
    - Repeat until the AET and the GET are empty:

        - Move new edges from the GET to the AET for this scan line.
        - Fill in the pixels for the current AET
        - Remove from the AET those edges falling out of scope.
        - Move to new scan line.
        - For each non-vertical edge in the AET update x for the new y
        - Resort the AET on the x cooridinates after updating.

{secret}
****************************************************************************/
void _MGL_complexPolygon(
    int count,
    fxpoint_t *vArray,
    int vinc,
    fix32_t xOffset,
    fix32_t yOffset)
{
    edgeState   *edgeTableBuffer;
    int         cntY;

    /* Make sure buffer is big enough for desired polygon. The default
     * 4k buffer should be bige enough to hold > 200 edges!
     */
    if ((int)(count * sizeof(edgeState)) > _MGL_bufSize)
        _MGL_scratchTooSmall();
    edgeTableBuffer = _MGL_buf;
    drawScanLine = DC.doClip ? _MGL_clipScanLine : _MGL_drawScanLine;
    buildGET(count,vArray,vinc,edgeTableBuffer,xOffset,yOffset);
    AETPtr = NULL;
    cntY = GETPtr->startY;          /* start at the top polygon vertex  */
    while (GETPtr || AETPtr) {
        updateAET(cntY);            /* update AET for this scan line    */
        scanOutAET(cntY);           /* Scan from the current AET        */
        advanceAET();               /* advance AET edges 1 scan line    */
        sortAET();                  /* resort AET on x coordinates      */
        cntY++;                     /* advance to the next scan line    */
        }
}

/****************************************************************************
DESCRIPTION:
Generates a solid complex polygonal region.

HEADER:
mgraph.h

PARAMETERS:
count   - Number of vertices to draw
vArray  - Array of vertices
vinc    - Increment to get to next vertex
xOffset - Offset of X coordinates
yOffset - Offset of Y coordinates

REMARKS:
This function generates a complex region that represents a convex polygon.
A "convex" polygon is defined as a polygon such that every horizontal line
drawn through the polygon would cross exactly two active edges (neither
horizontal lines nor zero-length edges count as active edges; both are
acceptable anywhere in the polygon). Right and left edges may cross (polygons
may be nonsimple). Attempting to scan convert a polygon that does non fit
this description will produce unpredictable results.

Note:   All vertices are offset by (xOffset,yOffset) and are in 16.16
        fixed point format.

SEE ALSO:
MGL_rgnPolygonCnvxFX, MGL_rgnPolygon
****************************************************************************/
region_t * MGLAPI MGL_rgnPolygonFX(
    int count,
    fxpoint_t *vArray,
    int vinc,
    fix32_t xOffset,
    fix32_t yOffset)
{
    edgeState   *edgeTableBuffer;
    int         cntY;

    /* Make sure buffer is big enough for desired polygon. The default
     * 4k buffer should be bige enough to hold > 200 edges!
     */
    if ((int)(count * sizeof(edgeState)) > _MGL_bufSize)
        _MGL_scratchTooSmall();

    /* Initialise the span list */
    rgn = MGL_newRegion();

    /* Now build the region spans from the scratch buffer */
    edgeTableBuffer = _MGL_buf;
    buildGET(count,vArray,vinc,edgeTableBuffer,xOffset,yOffset);
    AETPtr = NULL;
    cntY = GETPtr->startY;          /* start at the top polygon vertex  */
    while (GETPtr || AETPtr) {
        updateAET(cntY);            /* update AET for this scan line    */
        scanOutRgnAET(cntY);        /* Scan from the current AET        */
        advanceAET();               /* advance AET edges 1 scan line    */
        sortAET();                  /* resort AET on x coordinates      */
        cntY++;                     /* advance to the next scan line    */
        }

    /* Fill in the span table and bounding rectangle for the region */
    return rgn;
}

/****************************************************************************
DESCRIPTION:
Generates a solid complex polygonal region.

HEADER:
mgraph.h

PARAMETERS:
count   - Number of vertices to draw
vArray  - Array of vertices
vinc    - Increment to get to next vertex
xOffset - Offset of X coordinates
yOffset - Offset of Y coordinates

REMARKS:
This function generates a complex region that represents a convex polygon.
A "convex" polygon is defined as a polygon such that every horizontal line
drawn through the polygon would cross exactly two active edges (neither
horizontal lines nor zero-length edges count as active edges; both are
acceptable anywhere in the polygon). Right and left edges may cross (polygons
may be nonsimple). Attempting to scan convert a polygon that does non fit
this description will produce unpredictable results.

Note:   All vertices are offset by (xOffset,yOffset) and are in regular
        integer format.

SEE ALSO:
MGL_rgnPolygonCnvxFX, MGL_rgnPolygonFX
****************************************************************************/
region_t * MGLAPI MGL_rgnPolygon(
    int count,
    point_t *vArray,
    int vinc,
    int xOffset,
    int yOffset)
{
    return MGL_rgnPolygonFX(count,_MGL_copyToFixed(count,vArray),
        sizeof(fxpoint_t),MGL_TOFIX(xOffset),MGL_TOFIX(yOffset));
}

