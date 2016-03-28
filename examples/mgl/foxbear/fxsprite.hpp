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
* Environment:  32 bit flat mode only!
*
* Description:  Header for the FX_Sprite class.
*
****************************************************************************/

#ifndef __FXSPRITE_HPP
#define __FXSPRITE_HPP

#ifndef __SPRBMP_HPP
#include "gm\sprbmp.hpp"
#endif

/*-------------------------- Class Definitions ----------------------------*/

//---------------------------------------------------------------------------
// Enum definitions for various aspects of the sprites.
//---------------------------------------------------------------------------

enum FB_POSITION {
    FB_ABSOLUTE,
    FB_RELATIVE,
    FB_AUTOMATIC
    };

enum FB_SWITCHING {
    FB_HOR,
    FB_VER,
    FB_TIME
    };

enum FB_DIRECTION {
    FB_SAME,
    FB_RIGHT,
    FB_LEFT
    };

enum FB_ACTION {
    FB_NONE,
    FB_STILL,
    FB_WALK,
    FB_RUN,
    FB_JUMP,
    FB_THROW,
    FB_CROUCH,
    FB_STOP,
    FB_STUNNED,
    FB_JUMPTHROW,
    FB_CROUCHWALK,
    FB_BLURR,
    FB_STRIKE,
    FB_MISS,
    FB_CHEW
    };

//---------------------------------------------------------------------------
// Structure for maintaining a single bitmap in the sprite.
//---------------------------------------------------------------------------

struct FX_SpriteBitmap {
    SPR_Bitmap      *hBM;               // Pointer to bitmap to draw
    FB_ACTION       action;             // Action group associated with
    FB_DIRECTION    direction;          // Direction group associated with
    short           x;                  // Starting X coordinate within sprite
    short           y;                  // Starting Y coordinate within sprite
    ushort          width;              // Bitmap width
    ushort          height;             // Bitmap height
    };

//---------------------------------------------------------------------------
// Class representing a moving, animated sprite.
//---------------------------------------------------------------------------

class FX_Sprite {
protected:
    FX_SpriteBitmap *hSBM;              // Array of all bitmaps for sprite
    ushort          numBitmaps;         // Number of bitmaps in sprite
    ushort          bitmapCount;        // Maximum number of bitmaps
    FB_ACTION       currentAction;      // Current sprite action
    FB_DIRECTION    currentDirection;   // Current sprite direction
    ushort          currentBitmap;      // Current sprite bitmap index
    ibool           active;             // true if sprite is active
    fix32_t         x,y;                // Current location
    ushort          width;              // Total sprite width
    ushort          height;             // Total sprite height
    fix32_t         xv,yv;              // Current velocity
    fix32_t         xa,ya;              // Current acceleration
    ushort          xmax,ymax;          // Maximum sprite coordinates
    fix32_t         absSwitch;
    fix32_t         relSwitch;
    FB_SWITCHING    switchType;
    ibool           switchForward;
    ibool           switchDone;

public:
            // Constructor
            FX_Sprite(ushort bitmapCount,fix32_t x,fix32_t y,ushort width,
                ushort height,ushort xmax,ushort ymax,short as,ibool active);

            // Destructor
            ~FX_Sprite();

            // Add a new bitmap to the sprites bitmap array
            void AddBitmap(SPR_Bitmap *hBM,FB_ACTION action,FB_DIRECTION direction,
                ushort width,ushort height,short x,short y);

            // Set the sprite to a new action group
            void SetAction(FB_ACTION action,FB_DIRECTION direction);

            // Change sprite direction
            void ChangeDirection();

            // Set active flag for sprite
            void SetActive(ibool active);

            // Set sprite position
            ibool SetX(fix32_t x,FB_POSITION position);
            ibool SetY(fix32_t y,FB_POSITION position);

            // Set the sprite velocity vectors
            ibool SetVelocityX(fix32_t xv,FB_POSITION position);
            ibool SetVelocityY(fix32_t yv,FB_POSITION position);

            // Set the sprite acceleration vectors
            void SetAccelerationX(fix32_t xa,FB_POSITION position);
            void SetAccelerationY(fix32_t ya,FB_POSITION position);

            // Set the sprite switches
            void SetSwitch(fix32_t absSwitch,FB_POSITION position);
            void IncrementSwitch(fix32_t n);
            void SetSwitchType(FB_SWITCHING switchType);
            void SetSwitchForward(ibool switchForward);
            void SetSwitchDone(ibool switchDone);

            // Set the current bitmap within action/direction group
            void SetBitmap(int newBitmap);
            int CurrentBitmapId() const;
            const FX_SpriteBitmap *CurrentBitmap() const;
            void AdvanceBitmap();

            // Display the sprite
            ibool Display(fix32_t xPlane);

            // Get sprite info
            FB_ACTION Action() const           { return currentAction; };
            FB_DIRECTION Direction() const     { return currentDirection; };
            ibool Active() const             { return active; };
            fix32_t VelocityX() const       { return xv; };
            fix32_t VelocityY() const       { return yv; };
            fix32_t AccelerationX() const   { return xa; };
            fix32_t AccelerationY() const   { return ya; };
            fix32_t X() const               { return x; };
            fix32_t Y() const               { return y; };
            ushort Width() const            { return width; };
            ushort Height() const           { return height; };
            FB_SWITCHING SwitchType() const    { return switchType; };
            ibool SwitchForward() const      { return switchForward; };
            ibool SwitchDone()               { return switchDone; };
    };

/*------------------------- Inline member functions -----------------------*/

inline void FX_Sprite::ChangeDirection()
{
    SetAction(currentAction, currentDirection == FB_RIGHT ? FB_LEFT : FB_RIGHT);
}

inline void FX_Sprite::IncrementSwitch(fix32_t n)
{
    relSwitch += n;
}

inline void FX_Sprite::SetSwitchType(FB_SWITCHING st)
{
    switchType = st;
    relSwitch  = 0;
}

inline void FX_Sprite::SetSwitchForward(ibool sf)
{
    switchForward = sf;
}

inline void FX_Sprite::SetSwitchDone(ibool sd)
{
    switchDone = sd;
}

inline const FX_SpriteBitmap *FX_Sprite::CurrentBitmap() const
{
    return &hSBM[CurrentBitmapId()];
}

#endif  // __FXSPRITE_HPP

