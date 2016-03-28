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
* Description:  Member functions for the MVInputLine class.
*
****************************************************************************/

#include "mvis/mvision.hpp"
#include "mvis/minputl.hpp"
#include "mvis/mfontmgr.hpp"
#include "mvis/mgroup.hpp"
#include "mvis/mkeys.hpp"
#include "ztimerc.h"
#include <string.h>
#include <ctype.h>

#define OFFSETX     4
#define OFFSETY     4

/*----------------------------- Implementation ----------------------------*/

char    buf[2] = "1";                       // Single character buffer

MVInputLine::MVInputLine(MGLDevCtx& dc,const MVRect& bounds,int maxLen,
    const char *defaultText)
    : MVView(dc,bounds), text(new char[maxLen+1]), maxLen(maxLen), flags(0)
/****************************************************************************
*
* Function:     MVInputLine::MVInputLine
* Parameters:   bounds      - Bounding box for the input line
*               maxLen      - Maximum number of characters in input line
*               defaultText - Text to use as default (NULL for none)
*
* Description:  Constructor for the MVInputLine class.
*
****************************************************************************/
{
    ulong resolution;

    setBounds(bounds);
    curPos = firstPos = selStart = selEnd = 0;
    setText(defaultText);
    options |= ofSelectable | ofFirstClick;
    ULZTimerResolution(&resolution);
    cursorRate = (int)(9.0 / (18.2 * (resolution / 1000000.0)));
    oldTicks = 0;
}

MVInputLine::~MVInputLine()
/****************************************************************************
*
* Function:     MVInputLine::~MVInputLine
*
* Description:  Destructor for the MVInputLine. Simply deletes the text.
*
****************************************************************************/
{
    delete [] text;
}

void MVInputLine::setBounds(const MVRect& bounds)
/****************************************************************************
*
* Function:     MVInputLine::setBounds
* Parameters:   bounds  - New bounding box for the input line
*
* Description:  Computes the bounding box for the input line. The bounding
*               box is always resized to fit the current system font
*               (the bottom coordinate is changed).
*
****************************************************************************/
{
    MVRect  b(bounds);

    metrics_t   m;
    useFont(fmSystemFont);
    dc.getFontMetrics(m);

    starty = MAX(2 - m.descent,OFFSETY);
    b.bottom() = b.top() + starty + (m.ascent - m.descent + 1) + 3;
    MVView::setBounds(b);

    // Work out the number of characters to scroll left or right, to be
    // approximately 1/3 of the input line width.

    int width = scrollChars = 0;

    while (width < size.x/3) {
        width += dc.charWidth('M');
        scrollChars++;
        }
}

void MVInputLine::deleteChars(int start,int count)
/****************************************************************************
*
* Function:     MVInputLine::deleteChars
* Parameters:   start   - Starting character to delete
*               count   - Number of characters to delete
*
* Description:  Deletes the specified number of characters starting at
*               start from the text.
*
****************************************************************************/
{
    if (start >= len || len == 0)
        return;

    int copyfrom = start + count;

    if (copyfrom >= len) {
        text[start] = '\0';         // Just null terminate the string
        len = start;
        }
    else {
        memmove(&text[start],&text[copyfrom],(len-copyfrom)+1);
        len -= count;
        }
    flags |= tlDirty;
}

void MVInputLine::insertChar(char ch)
/****************************************************************************
*
* Function:     MVInputLine::insertChar
* Parameters:   ch      - Character to insert
*
* Description:  Inserts a character into the string. If the string has
*               reached the maximum length, we beep the speaker.
*
****************************************************************************/
{
    if (flags & tlOverwriteMode) {
        if (curPos == len)
            len++;
        }
    else
        len++;

    if (len > maxLen) {
        len--;
        MV_beep();
        return;
        }

    if (!(flags & tlOverwriteMode))
        memmove(&text[curPos+1],&text[curPos],len-curPos);
    text[curPos++] = ch;
    flags |= tlDirty;
}

ibool MVInputLine::deleteHighlight()
/****************************************************************************
*
* Function:     MVInputLine::deleteHighlight
* Returns:      True if the highlight was deleted, false if not.
*
* Description:  Deletes the highlighted characters
*
****************************************************************************/
{
    if (selStart < selEnd) {
        deleteChars(selStart,selEnd-selStart);
        curPos = selStart;
        clearHighlight();
        return true;
        }
    return false;
}

void MVInputLine::clearHighlight()
/****************************************************************************
*
* Function:     MVInputLine::clearHighlight
*
* Description:  Clears the highlight if present.
*
****************************************************************************/
{
    if (selStart < selEnd) {
        selStart = selEnd = 0x7FFF;
        flags |= tlDirty;
        }
}

int MVInputLine::findPos(const MVPoint& p)
/****************************************************************************
*
* Function:     MVInputLine::findPos
* Parameters:   p   - Point to find position from
* Returns:      Index of the character hit
*
* Description:  Attempts to find the character index hit by the point
*               p. If the point is halfway between two characters, we
*               find the closest selection point on either side.
*
****************************************************************************/
{
    int startx = bounds.left() + OFFSETX;
    int pos = firstPos;
    int width = dc.charWidth(text[pos]);

    useFont(fmSystemFont);
    while ((startx + width/2) < p.x) {
        startx += width;
        if (++pos == len)
            break;
        width = dc.charWidth(text[pos]);
        }
    return pos;
}

void MVInputLine::selectLeft(int count,ulong modifiers)
/****************************************************************************
*
* Function:     MVInputLine::selectLeft
* Parameters:   count   - Number of characters to select
*               modifiers   - keyboard shift modifiers
*
* Description:  Selects the next character on the left. If the shift
*               modifier's are set, we also extend the selection.
*
****************************************************************************/
{
    drawCursor(false);

    useFont(fmSystemFont);
    int oldPos = curPos;

    for (int i = 0; i < count; i++) {
        if (curPos <= 0)
            break;
        int width = dc.charWidth(text[--curPos]);
        curLoc -= width;
        }

    if (oldPos != curPos) {
        if (modifiers & mdShift) {
            if (selStart == selEnd) {
                selEnd = curPos+count;
                selStart = curPos;
                }
            else if (selStart > curPos) {
                if (selStart < oldPos)
                    selEnd = selStart;
                selStart = curPos;
                }
            else
                selEnd = curPos;
            flags |= tlDirty;
            }
        else
            clearHighlight();
        }

    repositionText();
    refresh();
}

void MVInputLine::selectRight(int count,ulong modifiers)
/****************************************************************************
*
* Function:     MVInputLine::selectRight
* Parameters:   count   - Number of characters to select
*               modifiers   - keyboard shift modifiers
*
* Description:  Selects the next character on the right. If the shift
*               modifier's are set, we also extend the selection.
*
****************************************************************************/
{
    drawCursor(false);

    useFont(fmSystemFont);
    int oldPos = curPos;

    for (int i = 0; i < count; i++) {
        if (curPos == len)
            break;
        int width = dc.charWidth(text[curPos++]);
        curLoc += width;
        }

    if (oldPos != curPos) {
        if (modifiers & mdShift) {
            if (selStart == selEnd) {
                selStart = curPos-count;
                selEnd = curPos;
                }
            else if (selEnd < curPos) {
                if (selEnd > oldPos)
                    selStart = selEnd;
                selEnd = curPos;
                }
            else
                selStart = curPos;
            flags |= tlDirty;
            }
        else
            clearHighlight();
        }

    repositionText();
    refresh();
}

void MVInputLine::handleEvent(MVEvent& event,phaseType phase)
/****************************************************************************
*
* Function:     MVInputLine::handleEvent
* Parameters:   event   - Event to handle
*               phase   - Current phase for the event (pre,focus,post)
*
* Description:  Event handling routines for the MVInputLine class.
*
****************************************************************************/
{
    MVView::handleEvent(event,phase);

    switch (event.what) {
        case evKeyDown:
        case evKeyAuto:
            switch (event.key.charScan.scanCode) {
                case kbLeft:
                    selectLeft(1,event.key.modifiers);
                    break;
                case kbRight:
                    selectRight(1,event.key.modifiers);
                    break;
                case kbHome:
                    selectLeft(curPos,event.key.modifiers);
                    break;
                case kbEnd:
                    selectRight(len-curPos,event.key.modifiers);
                    break;
                case kbBack:
                    if (!deleteHighlight() && curPos > 0)
                        deleteChars(--curPos,1);
                    repositionText();
                    refresh();
                    break;
                case kbDel:
                    if (event.what == evKeyAuto)
                        break;
                    if (!deleteHighlight())
                        deleteChars(curPos,1);
                    refresh();
                    break;
                case kbIns:
                    if (event.what == evKeyAuto)
                        break;
                    drawCursor(false);
                    flags ^= tlOverwriteMode;
                    drawCursor(true);
                    break;
                default:
                    // We have a normal key press here, so insert the
                    // character into the text

                    if (isprint(event.key.charScan.charCode)) {
                        deleteHighlight();
                        insertChar(event.key.charScan.charCode);
                        repositionText();
                        refresh();
                        }
                    else
                        return;
                }
            clearEvent(event);
            break;
        case evMouseDown:
            if (event.mouse.doubleClick) {
                // A double click selects all of the text in the input line

                setText(text);
                }
            else if (event.mouse.buttons & mbLeftButton) {
                ibool   firstSelect = true;

                while (event.what != evMouseUp) {
                    switch (event.what) {
                        case evMouseDown:
                            clearHighlight();
                            refresh();
                        case evMouseAuto:
                        case evMouseMove:
                            MVPoint p(event.where);
                            globalToLocal(p);
                            if (bounds.includes(p)) {
                                // Selection within the window

                                int newPos = findPos(p);
                                if (newPos != curPos) {
                                    if (newPos < curPos)
                                        selectLeft(curPos-newPos,
                                            firstSelect ? 0 : mdShift);
                                    else
                                        selectRight(newPos-curPos,
                                            firstSelect ? 0 : mdShift);
                                    }
                                firstSelect = false;
                                }
                            else if (event.what == evMouseAuto) {
                                // Auto selection outside window to scroll

                                if (p.x < bounds.left()) {
                                    if (curPos > 0)
                                        selectLeft(scrollChars,mdShift);
                                    }
                                else if (curPos < len)
                                    selectRight(scrollChars,mdShift);
                                }
                            break;
                        }
                    getEvent(event);
                    }
                }
            clearEvent(event);
            break;
        }
}

void MVInputLine::repaint(const MVRect&)
/****************************************************************************
*
* Function:     MVInputLine::repaint
* Parameters:   dirty   - Clipping rectangle
*
* Description:  Repaints the input line in the current state. This assumes
*               that the entire input line is visible, and that the
*               viewport is currently set up for the owning view.
*               We simply display the text in the input line, without
*               redrawing the borders.
*
****************************************************************************/
{
    MS_obscure();

    // Clip the text at input line boundary
    MVRect  r(bounds);
    r.top() += 2;       r.left() += 2;
    r.right() -= 2;     r.bottom() -= 2;
    setClipRect(r);
    dc.setColor(getColor(scInputInterior));
    dc.fillRect(bounds);

    useFont(fmSystemFont);

    // Draw each letter of the text string individually, highlighting
    // if necessary.
    int     i,startx = bounds.left() + OFFSETX;
    ibool   highlight = false;
    char    *p;
    color_t textColor = getColor(scInputText);

    r.top() = bounds.top() + 2;
    r.bottom() = bounds.bottom() - 3;
    r.left() = startx;
    curLoc = -1;

    if ((state & sfFocused) && selStart < firstPos) {
        highlight = true;
        textColor = getColor(scInputSelectedText);
        }

    for (i = firstPos,p = text + firstPos; i < len; i++) {
        buf[0] = *p++;
        r.right() = startx + dc.charWidth(buf[0]);

        if (state & sfFocused) {
            if (i == selStart) {
                highlight = true;
                textColor = getColor(scInputSelectedText);
                }
            if (i == selEnd) {
                highlight = false;
                textColor = getColor(scInputText);
                }
            if (highlight) {
                dc.setColor(getColor(scInputSelection));
                dc.fillRect(r);
                }
            }

        // Save the cursor insertion point location for later
        if (i == curPos)
            curLoc = startx;

        dc.setColor(textColor);
        dc.drawStr(startx,bounds.top()+starty,buf);
        if ((startx = r.left() = r.right()) >= bounds.right()-2)
            break;
        }
    if (curLoc == -1)
        curLoc = startx;
    flags &= ~tlDirty;
    MS_show();
}

void MVInputLine::refresh()
/****************************************************************************
*
* Function:     MVInputLine::refresh
*
* Description:  Refreshes the display if dirty.
*
****************************************************************************/
{
    if (flags & tlDirty)
        repaint(bounds);
    drawCursor(true);
}

void MVInputLine::draw(const MVRect& clip)
/****************************************************************************
*
* Function:     MVInputLine::draw
* Parameters:   clip    - Clipping rectangle to draw the view with
*
* Description:  Draws the input line in the current state.
*
****************************************************************************/
{
    MS_obscure();
    MV_setBorderColors(getColor(scShadow),getColor(scHighlight));
    MV_drawBorder(bounds,MV_BDR_OUTSET,1);
    MV_setBorderColors(getColor(scBlack),getColor(scButtonFace));
    MV_drawBorder(bounds.insetBy(1,1),MV_BDR_OUTSET,1);
    repaint(clip);
    MS_show();
}

void MVInputLine::drawCursor(ibool visible)
/****************************************************************************
*
* Function:     MVInputLine::drawCursor
* Parameters:   visible - True if the cursor is visible
*
* Description:  Draws the cursor in the specified state, and resets the
*               time to change the state of the cursor.
*
****************************************************************************/
{
    if (visible)
        flags |= tlCursorVisible;
    else
        flags &= ~tlCursorVisible;

    MVRect  c(curLoc,bounds.top()+2,curLoc+1,bounds.bottom()-3);
    ibool   selected = (selStart <= curPos && curPos < selEnd);

    if (flags & tlOverwriteMode && curPos < len)
        c.right() = curLoc + dc.charWidth(text[curPos]);

    // Draw the cursor at the correct location
    MS_obscure();
    dc.setClipRect(c);
    dc.setColor(getColor((uchar)(visible ?
        ((selected && curPos < selEnd) ? scInputSelCursor : scInputCursor) :
        (selected ? scInputSelection : scInputInterior))));
    dc.fillRect(c);
    dc.setColor(getColor((uchar)(visible ? scInputSelectedText :
        (selected ? scInputSelectedText : scInputText))));
    buf[0] = text[curPos];
    dc.drawStr(curLoc,bounds.top()+starty,buf);
    MS_show();

    // Reset the time till the cursor flips again
    oldTicks = ULZReadTime();
}

void MVInputLine::idle()
/****************************************************************************
*
* Function:     MVInputLine::idle
*
* Description:  This routine gets called periodically during idle moments
*               when the input line is focused. It will flash the cursor
*               at the specified time interval.
*
****************************************************************************/
{
    long ticks = ULZReadTime();
    if (ticks - oldTicks > cursorRate)
        drawCursor((flags ^ tlCursorVisible) & tlCursorVisible);
}

MGLCursor *MVInputLine::getCursor(const MVPoint&)
/****************************************************************************
*
* Function:     MVInputLine::getCursor
* Returns:      Pointer to the IBEAM cursor.
*
* Description:  Sets the cursor definition to the IBEAM cursor when the
*               cursor is within the input line.
*
****************************************************************************/
{
    return (state & sfDisabled) ? (MGLCursor*)NULL : MV_IBEAM_CURSOR;
}

void MVInputLine::repositionText()
/****************************************************************************
*
* Function:     MVInputLine::repositionText
*
* Description:  Repositions the text within the current window, so that the
*               cursor insertion point is within the set of characters
*               currently within view.
*
****************************************************************************/
{
    useFont(fmSystemFont);

    if (curPos < firstPos) {
        if ((firstPos = curPos - scrollChars*2) < 0)
            firstPos = 0;
        flags |= tlDirty;
        }
    else if (curLoc + dc.charWidth(text[curPos]) >= bounds.right()-2) {
        if ((firstPos = curPos - scrollChars) < 0)
            firstPos = 0;
        flags |= tlDirty;
        }
}

void MVInputLine::setText(const char *text)
/****************************************************************************
*
* Function:     MVInputLine::setText
* Parameters:   text        - text to copy
*
* Description:  Sets the current text for the input line.
*
****************************************************************************/
{
    if (text)
        strncpy(MVInputLine::text,text,maxLen);
    else
        MVInputLine::text[0] = '\0';
    MVInputLine::text[maxLen] = '\0';
    len = strlen(MVInputLine::text);

    // Select all of the text

    selStart = firstPos = 0;
    curPos = selEnd = len;
    useFont(fmSystemFont);
    curLoc = bounds.left() + OFFSETX + dc.textWidth(text);
    repositionText();
    if (owner) {
        if (state & sfExposed) {
            if (!(owner->getState() & sfLockRepaint)) {
                setupOwnerViewport();
                repaint(bounds);
                resetViewport();
                }
            }
        else
            owner->repaint(bounds);
        }
}

void MVInputLine::setState(uint aState,ibool set)
/****************************************************************************
*
* Function:     MVInputLine::setState
* Parameters:   aState  - State flag to set
*               set     - True if flag should be set, false if cleared
*
****************************************************************************/
{
    MVView::setState(aState,set);

    if (set && (aState & sfFocused)) {
        // The input line was just focused so turn off the cursor and
        // reset the tick count so that the cursor will be displayed the
        // next time the input line is painted.

        oldTicks = 0;
        flags &= tlCursorVisible;
        }
}
