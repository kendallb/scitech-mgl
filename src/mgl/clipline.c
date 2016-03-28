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
*                   Portions Copyright (c) 1987 X Consortium
*           Portions Copyright 1987 by Digital Equipment Corporation,
*                           Maynard, Massachusetts.
*
* Language:     ANSI C
* Environment:  Any
*
* Description:  Integer line clipping routines.
*
****************************************************************************/

#include "mgl.h"                /* Private declarations                 */
#include "clipline.h"

/*------------------------- Implementation --------------------------------*/

/****************************************************************************
DESCRIPTION:
Clips a bresenham line to an integer clipping rectangle.

PARAMETERS:
ix1     - Pointer to x coordinate of first endpoint to clip
iy1     - Pointer to y coordinate of first endpoint to clip
ix2     - Pointer to x coordinate of second endpoint to clip
iy2     - Pointer to y coordinate of second endpoint to clip
left    - Left coordinate of clip rectangle
top     - Top coordinate of clip rectangle
right   - Right coordinate of clip rectangle
bottom  - Bottom coordinate of clip rectangle

RETURNS:
1 for partially clipped line, -1 for completely clipped line, 0 if not
clipped at all.

REMARKS:
The bresenham error equation used in the MGL line drawing routines is:

        e = error
        dx = difference in raw X coordinates
        dy = difference in raw Y coordinates
        M = # of steps in X direction
        N = # of steps in Y direction
        B = 0 to prefer diagonal steps in a given octant,
            1 to prefer axial steps in a given octant

        For X major lines:
                e = 2Mdy - 2Ndx - dx - B
                -2dx <= e < 0

        For Y major lines:
                e = 2Ndx - 2Mdy - dy - B
                -2dy <= e < 0

At the start of the line, we have taken 0 X steps and 0 Y steps,
so M = 0 and N = 0:

        X major e = 2Mdy - 2Ndx - dx - B
                  = -dx - B

        Y major e = 2Ndx - 2Mdy - dy - B
                  = -dy - B

At the end of the line, we have taken dx X steps and dy Y steps,
so M = dx and N = dy:

        X major e = 2Mdy - 2Ndx - dx - B
                  = 2dxdy - 2dydx - dx - B
                  = -dx - B
        Y major e = 2Ndx - 2Mdy - dy - B
                  = 2dydx - 2dxdy - dy - B
                  = -dy - B

Thus, the error term is the same at the start and end of the line.

Let us consider clipping an X coordinate.  There are 4 cases which
represent the two independent cases of clipping the start vs. the
end of the line and an X major vs. a Y major line.  In any of these
cases, we know the number of X steps (M) and we wish to find the
number of Y steps (N).  Thus, we will solve our error term equation.
If we are clipping the start of the line, we will find the smallest
N that satisfies our error term inequality.  If we are clipping the
end of the line, we will find the largest number of Y steps that
satisfies the inequality.  In that case, since we are representing
the Y steps as (dy - N), we will actually want to solve for the
smallest N in that equation.

Case 1:  X major, starting X coordinate moved by M steps

                -2dx <= 2Mdy - 2Ndx - dx - B < 0
        2Ndx <= 2Mdy - dx - B + 2dx     2Ndx > 2Mdy - dx - B
        2Ndx <= 2Mdy + dx - B           N > (2Mdy - dx - B) / 2dx
        N <= (2Mdy + dx - B) / 2dx

Since we are trying to find the smallest N that satisfies these
equations, we should use the > inequality to find the smallest:

        N = floor((2Mdy - dx - B) / 2dx) + 1
          = floor((2Mdy - dx - B + 2dx) / 2dx)
          = floor((2Mdy + dx - B) / 2dx)

Case 1b: X major, ending X coordinate moved to M steps

Same derivations as Case 1, but we want the largest N that satisfies
the equations, so we use the <= inequality:

        N = floor((2Mdy + dx - B) / 2dx)

Case 2: X major, ending X coordinate moved by M steps

                -2dx <= 2(dx - M)dy - 2(dy - N)dx - dx - B < 0
                -2dx <= 2dxdy - 2Mdy - 2dxdy + 2Ndx - dx - B < 0
                -2dx <= 2Ndx - 2Mdy - dx - B < 0
        2Ndx >= 2Mdy + dx + B - 2dx     2Ndx < 2Mdy + dx + B
        2Ndx >= 2Mdy - dx + B           N < (2Mdy + dx + B) / 2dx
        N >= (2Mdy - dx + B) / 2dx

Since we are trying to find the highest number of Y steps that
satisfies these equations, we need to find the smallest N, so
we should use the >= inequality to find the smallest:

        N = ceiling((2Mdy - dx + B) / 2dx)
          = floor((2Mdy - dx + B + 2dx - 1) / 2dx)
          = floor((2Mdy + dx + B - 1) / 2dx)

Case 2b: X major, starting X coordinate moved to M steps from end

Same derivations as Case 2, but we want the smallest number of Y
steps, so we want the highest N, so we use the < inequality:

        N = ceiling((2Mdy + dx + B) / 2dx) - 1
          = floor((2Mdy + dx + B + 2dx - 1) / 2dx) - 1
          = floor((2Mdy + dx + B + 2dx - 1 - 2dx) / 2dx)
          = floor((2Mdy + dx + B - 1) / 2dx)

Case 3: Y major, starting X coordinate moved by M steps

                -2dy <= 2Ndx - 2Mdy - dy - B < 0
        2Ndx >= 2Mdy + dy + B - 2dy     2Ndx < 2Mdy + dy + B
        2Ndx >= 2Mdy - dy + B           N < (2Mdy + dy + B) / 2dx
        N >= (2Mdy - dy + B) / 2dx

Since we are trying to find the smallest N that satisfies these
equations, we should use the >= inequality to find the smallest:

        N = ceiling((2Mdy - dy + B) / 2dx)
          = floor((2Mdy - dy + B + 2dx - 1) / 2dx)
          = floor((2Mdy - dy + B - 1) / 2dx) + 1

Case 3b: Y major, ending X coordinate moved to M steps

Same derivations as Case 3, but we want the largest N that satisfies
the equations, so we use the < inequality:

        N = ceiling((2Mdy + dy + B) / 2dx) - 1
          = floor((2Mdy + dy + B + 2dx - 1) / 2dx) - 1
          = floor((2Mdy + dy + B + 2dx - 1 - 2dx) / 2dx)
          = floor((2Mdy + dy + B - 1) / 2dx)

Case 4: Y major, ending X coordinate moved by M steps

                -2dy <= 2(dy - N)dx - 2(dx - M)dy - dy - B < 0
                -2dy <= 2dxdy - 2Ndx - 2dxdy + 2Mdy - dy - B < 0
                -2dy <= 2Mdy - 2Ndx - dy - B < 0
        2Ndx <= 2Mdy - dy - B + 2dy     2Ndx > 2Mdy - dy - B
        2Ndx <= 2Mdy + dy - B           N > (2Mdy - dy - B) / 2dx
        N <= (2Mdy + dy - B) / 2dx

Since we are trying to find the highest number of Y steps that
satisfies these equations, we need to find the smallest N, so
we should use the > inequality to find the smallest:

        N = floor((2Mdy - dy - B) / 2dx) + 1

Case 4b: Y major, starting X coordinate moved to M steps from end

Same analysis as Case 4, but we want the smallest number of Y steps
which means the largest N, so we use the <= inequality:

        N = floor((2Mdy + dy - B) / 2dx)

Now let's try the Y coordinates, we have the same 4 cases.

Case 5: X major, starting Y coordinate moved by N steps

                -2dx <= 2Mdy - 2Ndx - dx - B < 0
        2Mdy >= 2Ndx + dx + B - 2dx     2Mdy < 2Ndx + dx + B
        2Mdy >= 2Ndx - dx + B           M < (2Ndx + dx + B) / 2dy
        M >= (2Ndx - dx + B) / 2dy

Since we are trying to find the smallest M, we use the >= inequality:

        M = ceiling((2Ndx - dx + B) / 2dy)
          = floor((2Ndx - dx + B + 2dy - 1) / 2dy)
          = floor((2Ndx - dx + B - 1) / 2dy) + 1

Case 5b: X major, ending Y coordinate moved to N steps

Same derivations as Case 5, but we want the largest M that satisfies
the equations, so we use the < inequality:

        M = ceiling((2Ndx + dx + B) / 2dy) - 1
          = floor((2Ndx + dx + B + 2dy - 1) / 2dy) - 1
          = floor((2Ndx + dx + B + 2dy - 1 - 2dy) / 2dy)
          = floor((2Ndx + dx + B - 1) / 2dy)

Case 6: X major, ending Y coordinate moved by N steps

                -2dx <= 2(dx - M)dy - 2(dy - N)dx - dx - B < 0
                -2dx <= 2dxdy - 2Mdy - 2dxdy + 2Ndx - dx - B < 0
                -2dx <= 2Ndx - 2Mdy - dx - B < 0
        2Mdy <= 2Ndx - dx - B + 2dx     2Mdy > 2Ndx - dx - B
        2Mdy <= 2Ndx + dx - B           M > (2Ndx - dx - B) / 2dy
        M <= (2Ndx + dx - B) / 2dy

Largest # of X steps means smallest M, so use the > inequality:

        M = floor((2Ndx - dx - B) / 2dy) + 1

Case 6b: X major, starting Y coordinate moved to N steps from end

Same derivations as Case 6, but we want the smallest # of X steps
which means the largest M, so use the <= inequality:

        M = floor((2Ndx + dx - B) / 2dy)

Case 7: Y major, starting Y coordinate moved by N steps

                -2dy <= 2Ndx - 2Mdy - dy - B < 0
        2Mdy <= 2Ndx - dy - B + 2dy     2Mdy > 2Ndx - dy - B
        2Mdy <= 2Ndx + dy - B           M > (2Ndx - dy - B) / 2dy
        M <= (2Ndx + dy - B) / 2dy

To find the smallest M, use the > inequality:

        M = floor((2Ndx - dy - B) / 2dy) + 1
          = floor((2Ndx - dy - B + 2dy) / 2dy)
          = floor((2Ndx + dy - B) / 2dy)

Case 7b: Y major, ending Y coordinate moved to N steps

Same derivations as Case 7, but we want the largest M that satisfies
the equations, so use the <= inequality:

        M = floor((2Ndx + dy - B) / 2dy)

Case 8: Y major, ending Y coordinate moved by N steps

                -2dy <= 2(dy - N)dx - 2(dx - M)dy - dy - B < 0
                -2dy <= 2dxdy - 2Ndx - 2dxdy + 2Mdy - dy - B < 0
                -2dy <= 2Mdy - 2Ndx - dy - B < 0
        2Mdy >= 2Ndx + dy + B - 2dy     2Mdy < 2Ndx + dy + B
        2Mdy >= 2Ndx - dy + B           M < (2Ndx + dy + B) / 2dy
        M >= (2Ndx - dy + B) / 2dy

To find the highest X steps, find the smallest M, use the >= inequality:

        M = ceiling((2Ndx - dy + B) / 2dy)
          = floor((2Ndx - dy + B + 2dy - 1) / 2dy)
          = floor((2Ndx + dy + B - 1) / 2dy)

Case 8b: Y major, starting Y coordinate moved to N steps from the end

Same derivations as Case 8, but we want to find the smallest # of X
steps which means the largest M, so we use the < inequality:

        M = ceiling((2Ndx + dy + B) / 2dy) - 1
          = floor((2Ndx + dy + B + 2dy - 1) / 2dy) - 1
          = floor((2Ndx + dy + B + 2dy - 1 - 2dy) / 2dy)
          = floor((2Ndx + dy + B - 1) / 2dy)

So, our equations are:

        1:  X major move x1 to x1+M     floor((2Mdy + dx - B) / 2dx)
        1b: X major move x2 to x1+M     floor((2Mdy + dx - B) / 2dx)
        2:  X major move x2 to x2-M     floor((2Mdy + dx + B - 1) / 2dx)
        2b: X major move x1 to x2-M     floor((2Mdy + dx + B - 1) / 2dx)

        3:  Y major move x1 to x1+M     floor((2Mdy - dy + B - 1) / 2dx) + 1
        3b: Y major move x2 to x1+M     floor((2Mdy + dy + B - 1) / 2dx)
        4:  Y major move x2 to x2-M     floor((2Mdy - dy - B) / 2dx) + 1
        4b: Y major move x1 to x2-M     floor((2Mdy + dy - B) / 2dx)

        5:  X major move y1 to y1+N     floor((2Ndx - dx + B - 1) / 2dy) + 1
        5b: X major move y2 to y1+N     floor((2Ndx + dx + B - 1) / 2dy)
        6:  X major move y2 to y2-N     floor((2Ndx - dx - B) / 2dy) + 1
        6b: X major move y1 to y2-N     floor((2Ndx + dx - B) / 2dy)

        7:  Y major move y1 to y1+N     floor((2Ndx + dy - B) / 2dy)
        7b: Y major move y2 to y1+N     floor((2Ndx + dy - B) / 2dy)
        8:  Y major move y2 to y2-N     floor((2Ndx + dy + B - 1) / 2dy)
        8b: Y major move y1 to y2-N     floor((2Ndx + dy + B - 1) / 2dy)

We have the following constraints on all of the above terms:

        0 < M,N <= 2^15          2^15 can be imposed by miZeroClipLine
        0 <= dx/dy <= 2^16 - 1
        0 <= B <= 1

The floor in all of the above equations can be accomplished with a
simple C divide operation provided that both numerator and denominator
are positive.

Since dx,dy >= 0 and since moving an X coordinate implies that dx != 0
and moving a Y coordinate implies dy != 0, we know that the denominators
are all > 0.

For all lines, (-B) and (B-1) are both either 0 or -1, depending on the
bias.  Thus, we have to show that the 2MNdxy +/- dxy terms are all >= 1
or > 0 to prove that the numerators are positive (or zero).

For X Major lines we know that dx > 0 and since 2Mdy is >= 0 due to the
constraints, the first four equations all have numerators >= 0.

For the second four equations, M > 0, so 2Mdy >= 2dy so (2Mdy - dy) >= dy
So (2Mdy - dy) > 0, since they are Y major lines.  Also, (2Mdy + dy) >= 3dy
or (2Mdy + dy) > 0.  So all of their numerators are >= 0.

For the third set of four equations, N > 0, so 2Ndx >= 2dx so (2Ndx - dx)
>= dx > 0.  Similarly (2Ndx + dx) >= 3dx > 0.  So all numerators >= 0.

For the fourth set of equations, dy > 0 and 2Ndx >= 0, so all numerators
are > 0.

To consider overflow, consider the case of 2 * M,N * dx,dy + dx,dy.  This
is bounded <= 2 * 2^15 * (2^16 - 1) + (2^16 - 1)
           <= 2^16 * (2^16 - 1) + (2^16 - 1)
           <= 2^32 - 2^16 + 2^16 - 1
           <= 2^32 - 1
Since the (-B) and (B-1) terms are all 0 or -1, the maximum value of
the numerator is therefore (2^32 - 1), which does not overflow an unsigned
32 bit variable.
{secret}
****************************************************************************/
int _MGL_clipLine(
    int xmin,
    int ymin,
    int xmax,
    int ymax,
    int *new_x1,
    int *new_y1,
    int *new_x2,
    int *new_y2,
    int absDeltaX,
    int absDeltaY,
    int *pt1_clipped,
    int *pt2_clipped,
    int flags,
    int bias,
    int outcode1,
    int outcode2)
{
    int     swapped = 0,clipDone,clip1,clip2;
    int     x1,y1,x2,y2,x1_orig,y1_orig,x2_orig,y2_orig;
    int     xmajor,negslope = 0,anchorval = 0,eqn= 0;
    ulong   utmp = 0;

    x1 = x1_orig = *new_x1;
    y1 = y1_orig = *new_y1;
    x2 = x2_orig = *new_x2;
    y2 = y2_orig = *new_y2;

    clip1 = 0;
    clip2 = 0;

    xmajor = IsXMajorOctant(flags);
    bias = ((bias >> (flags & 0x7)) & 1);

    for (;;) {
        if ((outcode1 & outcode2) != 0) {       /* Trivial reject */
            clipDone = -1;
            clip1 = outcode1;
            clip2 = outcode2;
            break;
            }
        else if ((outcode1 | outcode2) == 0) {  /* Trivial accept */
            clipDone = 1;
            if (swapped) {
                SWAPINT_PAIR(x1, y1, x2, y2);
                SWAPINT(clip1, clip2);
                }
            break;
            }
        else {
            /* Only clip one point at a time */
            if (outcode1 == 0) {
                SWAPINT_PAIR(x1, y1, x2, y2);
                SWAPINT_PAIR(x1_orig, y1_orig, x2_orig, y2_orig);
                SWAPINT(outcode1, outcode2);
                SWAPINT(clip1, clip2);
                swapped = !swapped;
                }

            clip1 |= outcode1;
            if (outcode1 & OUT_LEFT) {
                negslope = IsYDecreasingOctant(flags);
                utmp = xmin - x1_orig;
                if (xmajor)
                    eqn = (swapped) ? EQN2 : EQN1;
                else
                    eqn = (swapped) ? EQN4 : EQN3;
                anchorval = y1_orig;
                x1 = xmin;
                }
            else if (outcode1 & OUT_ABOVE) {
                negslope = IsXDecreasingOctant(flags);
                utmp = ymin - y1_orig;
                if (xmajor)
                    eqn = (swapped) ? EQN6 : EQN5;
                else
                    eqn = (swapped) ? EQN8 : EQN7;
                anchorval = x1_orig;
                y1 = ymin;
                }
            else if (outcode1 & OUT_RIGHT) {
                negslope = IsYDecreasingOctant(flags);
                utmp = x1_orig - xmax;
                if (xmajor)
                    eqn = (swapped) ? EQN2 : EQN1;
                else
                    eqn = (swapped) ? EQN4 : EQN3;
                anchorval = y1_orig;
                x1 = xmax;
                }
            else if (outcode1 & OUT_BELOW) {
                negslope = IsXDecreasingOctant(flags);
                utmp = y1_orig - ymax;
                if (xmajor)
                    eqn = (swapped) ? EQN6 : EQN5;
                else
                    eqn = (swapped) ? EQN8 : EQN7;
                anchorval = x1_orig;
                y1 = ymax;
                }

            if (swapped)
                negslope = !negslope;

            utmp <<= 1;                 /* utmp = 2N or 2M */
            if (eqn & T_2NDX)
                utmp = (utmp * absDeltaX);
            else /* (eqn & T_2MDY) */
                utmp = (utmp * absDeltaY);
            if (eqn & T_DXNOTY) {
                if (eqn & T_SUBDXORY)
                    utmp -= absDeltaX;
                else
                    utmp += absDeltaX;
                }
            else {/* (eqn & T_DYNOTX) */
                if (eqn & T_SUBDXORY)
                    utmp -= absDeltaY;
                else
                    utmp += absDeltaY;
                }
            if (eqn & T_BIASSUBONE)
                utmp += bias - 1;
            else /* (eqn & T_SUBBIAS) */
                utmp -= bias;
            if (eqn & T_DIV2DX)
                utmp /= (absDeltaX << 1);
            else /* (eqn & T_DIV2DY) */
                utmp /= (absDeltaY << 1);
            if (eqn & T_ADDONE)
                utmp++;

            if (negslope)
                utmp = -((long)utmp);

            if (eqn & T_2NDX)   /* We are calculating X steps */
                x1 = anchorval + utmp;
            else                /* else, Y steps */
                y1 = anchorval + utmp;

            outcode1 = 0;
            MIOUTCODES(outcode1, x1, y1, xmin, ymin, xmax, ymax);
            }
        }

    /* Return the clipped coordinates and status */
    *new_x1 = x1;
    *new_y1 = y1;
    *new_x2 = x2;
    *new_y2 = y2;
    *pt1_clipped = clip1;
    *pt2_clipped = clip2;
    return clipDone;
}

/****************************************************************************
PARAMETERS:
x1          - X coordinate of first endpoint to clip
y1          - Y coordinate of first endpoint to clip
x2          - X coordinate of second endpoint to clip
y2          - Y coordinate of second endpoint to clip
drawLast    - True if last pixel should be drawn
clipLeft    - Left coordinate of clip rectangle
clipTop     - Top coordinate of clip rectangle
clipRight   - Right coordinate of clip rectangle
clipBottom  - Bottom coordinate of clip rectangle

REMARKS:
Internal function to clip and draw an integer line, such that the clipped
line segment draws the *exact* same set of pixels that the unclipped line
would have drawn. We also correctly handle arbitrarily large lines, while
still allowing the clipped line segment to be drawn in hardware.
{secret}
****************************************************************************/
void _MGL_drawClippedLineInt(
    int x1,
    int y1,
    int x2,
    int y2,
    ibool drawLast,
    int clipLeft,
    int clipTop,
    int clipRight,
    int clipBottom)
{
    int absDeltaX,absDeltaY,initialError,majorInc,diagInc;
    int clipdx,clipdy;
    int new_x1 = x1, new_y1 = y1, new_x2 = x2, new_y2 = y2;
    int count,flags,clip1,clip2,outcode1,outcode2;

    /* Pre-clip the line and check for trival accept/reject */
    outcode1 = 0;
    outcode2 = 0;
    OUTCODES(outcode1,x1,y1);
    OUTCODES(outcode2,x2,y2);
    if ((outcode1 | outcode2) == 0) {
        /* Trivial accept */
        if (DC.a.lineStyle == MGL_LINE_STIPPLE)
            DC.r.DrawStippleLineInt(x1,y1,x2,y2,drawLast,DC.a.backMode == MGL_TRANSPARENT_BACKGROUND);
        else
            DC.r.DrawLineInt(x1,y1,x2,y2,drawLast);
        return;
        }
    else if (outcode1 & outcode2) {
        /* Trivial reject */
        return;
        }

    /* Calculate bresenham parameters */
    flags = gaLineXPositive | gaLineYPositive | gaLineXMajor | gaLineDoLastPel;
    if ((absDeltaX = x2 - x1) < 0) {
        absDeltaX = -absDeltaX;
        flags &= ~gaLineXPositive;
        }
    if ((absDeltaY = y2 - y1) < 0) {
        absDeltaY = -absDeltaY;
        flags &= ~gaLineYPositive;
        }
    if (absDeltaX > absDeltaY) {
        majorInc = absDeltaY * 2;
        diagInc = majorInc - absDeltaX * 2;
        initialError = majorInc - absDeltaX;
        }
    else {
        majorInc = absDeltaX * 2;
        diagInc = majorInc - absDeltaY * 2;
        initialError = majorInc - absDeltaY;
        flags &= ~gaLineXMajor;
        }

    /* Clip the line and reject it if nothing is to be drawn */
    clip1 = 0;
    clip2 = 0;
    if (_MGL_clipLine(clipLeft,clipTop,clipRight-1,clipBottom-1,
            &new_x1, &new_y1, &new_x2, &new_y2,
            absDeltaX, absDeltaY, &clip1, &clip2,
            flags, 0, outcode1, outcode2) == -1) {
        /* Trivial rejection */
        return;
        }
    if (flags & gaLineXMajor)
        count = abs(new_x2 - new_x1);
    else
        count = abs(new_y2 - new_y1);

    /* Always draw the last pixel if it has been clipped */
    if (clip2 != 0 || drawLast)
        count++;

    /* Now draw the clipped line */
    if (count) {
        /* Unwind bresenham error term to first point */
        if (clip1) {
            clipdx = abs(new_x1 - x1);
            clipdy = abs(new_y1 - y1);
            if (flags & gaLineXMajor)
                initialError += ((clipdy*diagInc) + ((clipdx-clipdy)*majorInc));
            else
                initialError += ((clipdx*diagInc) + ((clipdy-clipdx)*majorInc));
            }
        if (DC.a.lineStyle == MGL_LINE_STIPPLE)
            DC.r.DrawBresenhamStippleLine(new_x1,new_y1,initialError,majorInc,diagInc,count,flags,DC.a.backMode == MGL_TRANSPARENT_BACKGROUND);
        else
            DC.r.DrawBresenhamLine(new_x1,new_y1,initialError,majorInc,diagInc,count,flags);
        }
}
