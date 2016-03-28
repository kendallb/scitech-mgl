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
* Environment:  32 bit flat model only!
*
* Description:  Implementation for the FX_Sprite class. Note that the
*               Sprite class itself does not manage the bitmaps, as they
*               are all owned and managed by the bitmap manager class.
*
*
****************************************************************************/

#include "fbpch.hpp"

/*------------------------- Implementation --------------------------------*/

FX_Sprite::FX_Sprite(ushort bitmapCount,fix32_t x,fix32_t y,ushort width,
    ushort height,ushort xmax,ushort ymax,short as,ibool active)
    : numBitmaps(0), bitmapCount(bitmapCount), active(active), x(x), y(y),
      width(width), height(height), xv(0), yv(0), xa(0), ya(0), xmax(xmax),
      ymax(ymax), absSwitch(as), relSwitch(0), switchType(FB_HOR),
      switchForward(true), switchDone(false)
/****************************************************************************
*
* Function:     FX_Sprite::FX_Sprite
* Parameters:   bitmapCount     - Number of bitmaps in the sprite
*               x,y             - Initial location for the sprite
*               width,height    - Dimensions of the sprite
*               xmax            - Maximum sprite X coordinate
*               ymax            - maximum sprite Y coordinate
*               as              - Default acceleration value
*               active          - True if active by default
*
* Description:  Constructor for the sprite class
*
****************************************************************************/
{
    // Allocate memory for bitmap array
    hSBM = new FX_SpriteBitmap[bitmapCount];
    if (hSBM == NULL)
        MGL_fatalError("Out of memory!");
}

FX_Sprite::~FX_Sprite()
/****************************************************************************
*
* Function:     FX_Sprite::~FX_Sprite
*
* Description:  Destructor for the FX_Sprite class
*
****************************************************************************/
{
    delete [] hSBM;
}

void FX_Sprite::AddBitmap(SPR_Bitmap *hBM,FB_ACTION action,FB_DIRECTION direction,
    ushort width,ushort height,short x,short y)
/****************************************************************************
*
* Function:     FX_Sprite::AddBitmap
* Parameters:   hBM         - Pointer to bitmap to add
*               action      - Action code for this bitmap
*               direction   - Direction code for this bitmap
*               width       - Width of sprite (in game coords, not screen)
*               height      - Heigth of sprite (in game coords, not screen)
*               x,y         - Starting location of bitmap within sprite
*
* Description:  Adds a new bitmap to the sprites bitmap list. This is used
*               simply to build the bitmap list for the sprite when it
*               is created.
*
****************************************************************************/
{
    CHECK(numBitmaps < bitmapCount);

    hSBM[numBitmaps].hBM        = hBM;
    hSBM[numBitmaps].action     = action;
    hSBM[numBitmaps].direction  = direction;
    hSBM[numBitmaps].x          = x;
    hSBM[numBitmaps].y          = y;
    hSBM[numBitmaps].width      = width;
    hSBM[numBitmaps].height     = height;
    numBitmaps++;
}

void FX_Sprite::SetAction(FB_ACTION action,FB_DIRECTION direction)
/****************************************************************************
*
* Function:     FX_Sprite::SetAction
* Parameters:   action      - New action code for sprite
*               direction   - New direction code for sprite
*
* Description:  Changes the current action for the sprite to the specified
*               action and direction.
*
****************************************************************************/
{
    int c = 0;

    if (direction == FB_SAME)
        direction = currentDirection;

    while ((hSBM[c].action != action) || (hSBM[c].direction != direction))
        c++;
#ifdef  USE_DWSTK
    switch (action) {
        case FB_STOP:
            sound_PlaySound(sound_STOP);
            break;
        case FB_MISS:
            sound_PlaySound(sound_MISS);
            break;
        case FB_JUMPTHROW:
        case FB_THROW:
            sound_PlaySound(sound_THROW);
            break;
        }
#endif
    currentAction    = action;
    currentDirection = direction;
    currentBitmap    = (ushort)c;
    relSwitch        = 0;
}

void FX_Sprite::SetActive(ibool a)
{
    active = a;
    if (!active) {
        xv = yv = xa = ya = 0;
        }
}

ibool FX_Sprite::SetVelocityX(fix32_t newxv,FB_POSITION position)
{
    if (!active)
        return false;
    if (position == FB_ABSOLUTE)
        xv = newxv;
    else if (position == FB_RELATIVE)
        xv += newxv;
    return true;
}

ibool FX_Sprite::SetVelocityY(fix32_t newyv,FB_POSITION position)
{
    if (!active)
        return false;
    if (position == FB_ABSOLUTE)
        yv = newyv;
    else if (position == FB_RELATIVE)
        yv += newyv;
    return true;
}

void FX_Sprite::SetAccelerationX(fix32_t newxa,FB_POSITION position)
{
    if (position == FB_ABSOLUTE)
        xa = newxa;
    else if (position == FB_RELATIVE)
        xa += newxa;
}

void FX_Sprite::SetAccelerationY(fix32_t newya,FB_POSITION position)
{
    if (position == FB_ABSOLUTE)
        ya = newya;
    else if (position == FB_RELATIVE)
        ya += newya;
}

ibool FX_Sprite::SetX(fix32_t newx,FB_POSITION position)
{
    if (!active)
        return false;
    if (position == FB_AUTOMATIC) {
        xv += xa;
        x += xv;
        }
    else if (position == FB_ABSOLUTE)
        x = newx;
    else if (position == FB_RELATIVE)
        x += newx;
    if (x < 0)
        x += xmax << 16;
    else if (x >= xmax << 16)
        x -= xmax << 16;
    return true;
}

ibool FX_Sprite::SetY(fix32_t newy,FB_POSITION position)
{
    if (!active)
        return false;
    if (position == FB_AUTOMATIC) {
        yv += ya;
        y += yv;
        }
    else if (position == FB_ABSOLUTE)
        y = newy;
    else if (position == FB_RELATIVE)
        y += newy;
    if (y < 0)
        y += ymax << 16;
    else if (y >= ymax << 16)
        y -= ymax << 16;
    return true;
}

void FX_Sprite::SetSwitch(fix32_t as,FB_POSITION position)
{
    if (position == FB_ABSOLUTE)
        absSwitch = as;
    else if (position == FB_RELATIVE)
        absSwitch += as;
}

void FX_Sprite::SetBitmap(int newBitmap)
/****************************************************************************
*
* Function:     FX_Sprite::SetBitmap
* Parameters:   newBitmap   - New bitmap id within current action set
*
* Description:  Sets the current sprite bitmap to the bitmap for the
*               current sprites action.
*
****************************************************************************/
{
    int c = 0;
    while ((currentAction != hSBM[c].action) || (currentDirection != hSBM[c].direction))
        c++;
    currentBitmap = (ushort)(c + newBitmap);
}

int FX_Sprite::CurrentBitmapId() const
/****************************************************************************
*
* Function:     FX_Sprite::CurrentBitmapId
* Returns:      Current bitmap id for the sprite
*
****************************************************************************/
{
    int c = 0;
    while ((currentAction != hSBM[c].action) || (currentDirection != hSBM[c].direction))
        c++;
    return currentBitmap - c;
}

void FX_Sprite::AdvanceBitmap()
/****************************************************************************
*
* Function:     FX_Sprite::AdvanceBitmap
*
* Description:  Advances the current bitmap for the sprite automatically
*               depending on it's current action, direction etc.
*
****************************************************************************/
{
    int             c,n;
    FB_ACTION       act,curAct;
    FB_DIRECTION    dir,curDir;

    curAct = currentAction;
    curDir = currentDirection;

    if (switchForward) {
        c   = currentBitmap + 1;
        act = hSBM[c].action;
        dir = hSBM[c].direction;

        if (c >= bitmapCount || (curAct != act) || (curDir != dir))
            SetBitmap(0);
        else
            currentBitmap = (ushort)c;
        }
    else {
        c   = currentBitmap - 1;
        act = hSBM[c].action;
        dir = hSBM[c].direction;

        if (c < 0) {
            n = 0;
            while ((curAct == hSBM[n].action) && (curDir == hSBM[n].direction))
                n++;
            currentBitmap = (ushort)(n - 1);
            }
        else if ((curAct != act) || (curDir != dir)) {
            n = c + 1;
            while ((curAct == hSBM[n].action) && (curDir == hSBM[n].direction))
                n++;
            currentBitmap = (ushort)(n - 1);
            }
        else
            currentBitmap = (ushort)c;
        }
}

ibool FX_Sprite::Display(fix32_t xPlane)
/****************************************************************************
*
* Function:     FX_Sprite::Display
* Parameters:   xPlane      - ??
*
* Description:  Displays the current bitmap in the sprite at the current
*               location and advances the sprite bitmap.
*
****************************************************************************/
{
    if (!active)
        return false;

    // Compute sprite position
    FX_SpriteBitmap *bmp = &hSBM[currentBitmap];
    if (!bmp)
        return false;
    int shortx  = x >> 16;
    int shorty  = y >> 16;
    int planex  = xPlane >> 16;
    if (shortx < planex - C_SCREEN_W)
        shortx += xmax;
    else if (shortx >= planex + C_SCREEN_W)
        shortx -= xmax;
    int left = shortx - planex;
    if (currentDirection == FB_RIGHT)
        left += bmp->x;
    else
        left += width - bmp->x - bmp->width;

    // Draw the current sprite bitmap. Note that we scale the destination
    // coordinates to our destination device if we are not in the native
    // graphics mode that all game logic is run in (640x480 in this case).
    if (!bmp->hBM)
        return false;
    if (renderYRes == C_SCREEN_H)
        bmp->hBM->draw(left,shorty + bmp->y);
    else
        bmp->hBM->draw(SCALEX(left),SCALEY(shorty + bmp->y));

    // Advance the current bitmap to the next in sequence
    if (switchType == FB_HOR) {
        relSwitch += ABS(xv);

        if (relSwitch >= absSwitch) {
            relSwitch = 0;
            AdvanceBitmap();
            }
        }
    else if (switchType == FB_VER) {
        relSwitch += ABS(yv);

        if (relSwitch >= absSwitch) {
            relSwitch = 0;
            AdvanceBitmap();
            if (CurrentBitmapId() == 0)
                SetSwitchDone(true);
            }
        }
    else if (switchType == FB_TIME) {
        relSwitch += MGL_FIX_1;

        if (relSwitch >= absSwitch) {
            relSwitch = 0;
            AdvanceBitmap();
            if (CurrentBitmapId() == 0 )
                SetSwitchDone(true);
            }
        }
    return true;
}

