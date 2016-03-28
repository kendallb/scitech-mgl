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
* Environment:  Any
*
* Description:  Code to implement the Fox character.
*
****************************************************************************/

#include "fbpch.hpp"
#include <stdlib.h>

/*------------------------------ Implementation ---------------------------*/

void CreateFox()
/****************************************************************************
*
* Function:     CreateFox
*
* Description:  Creates the Fox sprite with all it's associated bitmaps.
*
****************************************************************************/
{
    static short x[C_FBT] = {
         7, 15, 18, 11,  6,  3,  7, 15, 17, 11,  6,  3,
         7, 15, 18, 11,  6,  3,  7, 15, 17, 11,  6,  3,
        10,  3,  5, 16,  9, 13, 31, 24,  9,  3,  5, 16, 10, 13, 33, 23,
        10,  3,  5, 16,  9, 13, 31, 24,  9,  3,  5, 16, 10, 13, 33, 23,
        11, 11, 31, 31,  7,  7, 27, 27,  8, 10,  8, 10,
        26,  6, 26,  6, 17, 21, 21, 24, 17, 21, 21, 24,
         1,  0,  0,  1,  0,  0,  1,  0,  0,  1,  1,  1,
         1,  0,  0,  1,  0,  0,  1,  0,  0,  1,  1,  1,
         2,  2, -1,  0,  2,  2, -1,  0
        };
    static short y[C_FBT] = {
        20, 24, 26, 25, 27, 19, 20, 25, 26, 25, 29, 21,
        20, 24, 26, 25, 27, 19, 20, 25, 26, 25, 29, 21,
        42, 42, 31, 19, 13, 11, 20, 33, 40, 43, 31, 19, 14, 12, 20, 33,
        42, 42, 31, 19, 13, 11, 20, 33, 40, 43, 31, 19, 14, 12, 20, 33,
        14, 14, 20, 20, 58, 58, 26, 26, 20, 24, 20, 24,
         0,  9,  0,  9, 20, 11, 10,  9, 20, 11, 10,  9,
        61, 61, 61, 61, 60, 60, 61, 61, 61, 61, 60, 60,
        61, 61, 61, 61, 60, 60, 61, 61, 61, 61, 60, 60,
        45, 45, 45, 45, 45, 45, 45, 45
        };
    static ushort w[C_FBT] = {
        75, 73, 73, 82, 92, 84, 74, 74, 73, 81, 91, 84,
        75, 73, 73, 82, 92, 84, 74, 74, 73, 81, 91, 84,
        88, 92, 88, 78, 80, 78, 70, 84, 88, 92, 88, 78, 79, 79, 68, 85,
        88, 92, 88, 78, 80, 78, 70, 84, 88, 92, 88, 78, 79, 79, 68, 85,
        65, 65, 61, 61, 88, 88, 72, 72, 57, 86, 57, 86,
        54, 92, 54, 92, 59, 57, 57, 52, 59, 57, 57, 52,
        98, 99, 99, 99,100,100, 98,101,100, 99,100, 98,
        98, 99, 99, 99,100,100, 98,101,100, 99,100, 98,
        94, 94, 97, 96, 94, 94, 97, 96
        };
    static ushort h[C_FBT] = {
        78, 74, 72, 73, 71, 79, 78, 73, 72, 73, 69, 77,
        78, 74, 72, 73, 71, 79, 78, 73, 72, 73, 69, 77,
        56, 56, 67, 79, 85, 87, 78, 65, 58, 55, 67, 79, 84, 86, 78, 65,
        56, 56, 67, 79, 85, 87, 78, 65, 58, 55, 67, 79, 84, 86, 78, 65,
        84, 84, 85, 85, 40, 40, 72, 72, 78, 74, 78, 74,
        88, 82, 88, 82, 84, 87, 86, 85, 84, 87, 86, 85,
        37, 37, 37, 37, 38, 38, 37, 37, 37, 37, 38, 38,
        37, 37, 37, 37, 38, 38, 37, 37, 37, 37, 38, 38,
        54, 53, 51, 54, 54, 53, 51, 54
        };

    fox = new FX_Sprite(C_FBT, C_FOX_STARTX, C_FOX_STARTY, C_FOX_SIZEX,
        C_FOX_SIZEY, C_FORE_W * C_TILE_W, C_FORE_H * C_TILE_H,
        C_FOX_AS, true);

    for (int i = 0; i < C_FBT; i++) {
        FB_ACTION      action = FB_STILL;
        FB_DIRECTION   direction = FB_LEFT;
        if (i < 12)         {   action = FB_WALK;      direction = FB_RIGHT;  }
        else if (i < 24)    {   action = FB_WALK;      direction = FB_LEFT;   }
        else if (i < 40)    {   action = FB_RUN;       direction = FB_RIGHT;  }
        else if(i < 56)     {   action = FB_RUN;       direction = FB_LEFT;   }
        else if (i == 56)   {   action = FB_STILL;     direction = FB_RIGHT;  }
        else if (i == 57)   {   action = FB_STILL;     direction = FB_LEFT;   }
        else if (i == 58)   {   action = FB_STUNNED;   direction = FB_RIGHT;  }
        else if (i == 59)   {   action = FB_STUNNED;   direction = FB_LEFT;   }
        else if (i == 60)   {   action = FB_CROUCH;    direction = FB_RIGHT;  }
        else if (i == 61)   {   action = FB_CROUCH;    direction = FB_LEFT;   }
        else if (i == 62)   {   action = FB_STOP;      direction = FB_RIGHT;  }
        else if (i == 63)   {   action = FB_STOP;      direction = FB_LEFT;   }
        else if (i < 66)    {   action = FB_THROW;     direction = FB_RIGHT;  }
        else if (i < 68)    {   action = FB_THROW;     direction = FB_LEFT;   }
        else if (i < 70)    {   action = FB_JUMPTHROW; direction = FB_RIGHT;  }
        else if (i < 72)    {   action = FB_JUMPTHROW; direction = FB_LEFT;   }
        else if (i < 76)    {   action = FB_JUMP;      direction = FB_RIGHT;  }
        else if (i < 80)    {   action = FB_JUMP;      direction = FB_LEFT;   }
        else if (i < 92)    {   action = FB_CROUCHWALK;direction = FB_RIGHT;  }
        else if (i < 104)   {   action = FB_CROUCHWALK;direction = FB_LEFT;   }
        else if (i < 108)   {   action = FB_BLURR;     direction = FB_RIGHT;  }
        else if (i < 112)   {   action = FB_BLURR;     direction = FB_LEFT;   }
        fox->AddBitmap(bitmapList[i + C_TILETOTAL],action,direction,
            w[i], h[i], x[i], y[i]);
        }
    fox->SetAction(FB_STILL, FB_RIGHT);
}

void UpdateFox()
/****************************************************************************
*
* Function:     UpdateFox
*
* Description:  Update the fox sprite for the next frame
*
****************************************************************************/
{
    long            foxSpeedX,foxSpeedY;
    FB_ACTION       foxAction;
    FB_DIRECTION    foxDir;

    if (!fox->Active())
        return;

    if (abs(fox->VelocityX()) < C_FOX_XMOVE)
        fox->SetVelocityX(0, FB_ABSOLUTE);

    foxAction = fox->Action();

    if (fox->VelocityY() == 0) {
        if (!foreground->OnSurface(fox)) {
            if ((foxAction == FB_WALK) || (foxAction == FB_RUN) || (foxAction == FB_CROUCHWALK))
                fox->SetAccelerationY(C_UNIT / 2, FB_ABSOLUTE);
            else if (foxAction == FB_STOP) {
                fox->SetAccelerationY(C_UNIT / 2, FB_ABSOLUTE);
                fox->SetAccelerationX(0, FB_ABSOLUTE);
                }
            }
        }
    else if (fox->VelocityY() > 2 * C_UNIT) {
        if ((foxAction == FB_WALK) || (foxAction == FB_RUN) || (foxAction == FB_CROUCHWALK)) {
            fox->SetSwitchForward(FALSE);
            fox->SetAction(FB_JUMP, FB_SAME);
            fox->SetSwitchType(FB_TIME);
            fox->SetSwitch(C_FOX_JUMPSWITCH, FB_ABSOLUTE);
            }
        if (foxAction == FB_STOP) {
            fox->SetAction(FB_STUNNED, FB_SAME);
            fox->SetAccelerationX(-fox->VelocityX() / 25, FB_ABSOLUTE);
            }
        }

    foxSpeedX = fox->VelocityX();
    foxSpeedY = fox->VelocityY();
    foxAction = fox->Action();
    foxDir    = fox->Direction();

    switch (foxAction) {
        case FB_STUNNED:
            if ((fox->VelocityY() >= 0) && foreground->OnSurface(fox)) {
                fox->SetAccelerationY(0, FB_ABSOLUTE);
                fox->SetAction(FB_STOP, FB_SAME);
                fox->SetVelocityY(0, FB_ABSOLUTE);
                fox->SetAccelerationX(-foxSpeedX / 25, FB_ABSOLUTE);
                foreground->KeepOnSurface(fox);
                }
            break;
        case FB_CROUCHWALK:
            if (foxSpeedX == 0)
                fox->SetAction(FB_CROUCH, FB_SAME);
            else if (foxSpeedX > C_FOX_WALKMOVE)
                fox->SetVelocityX(C_FOX_WALKMOVE, FB_ABSOLUTE);
            else if (foxSpeedX < -C_FOX_WALKMOVE)
                fox->SetVelocityX(-C_FOX_WALKMOVE, FB_ABSOLUTE);
            break;
        case FB_STOP:
            if (foxSpeedX == 0) {
                fox->SetAction(FB_STILL, FB_SAME);
                fox->SetAccelerationX(0, FB_ABSOLUTE);
                }
            break;
        case FB_RUN:
            if ((foxSpeedX < C_FOX_WALKTORUN) && (foxSpeedX > 0)) {
                fox->SetAction(FB_WALK, FB_RIGHT);
                fox->SetSwitch(C_FOX_WALKSWITCH, FB_ABSOLUTE);
                }
            else if (foxSpeedX > C_FOX_RUNTOBLURR) {
                fox->SetAction(FB_BLURR, FB_RIGHT);
                fox->SetSwitch(C_FOX_BLURRSWITCH, FB_ABSOLUTE);
                }
            else if ((foxSpeedX > -C_FOX_WALKTORUN) && (foxSpeedX < 0)) {
                fox->SetAction(FB_WALK, FB_LEFT);
                fox->SetSwitch(C_FOX_WALKSWITCH, FB_ABSOLUTE);
                }
            else if (foxSpeedX < -C_FOX_RUNTOBLURR) {
                fox->SetAction(FB_BLURR, FB_LEFT);
                fox->SetSwitch(C_FOX_BLURRSWITCH, FB_ABSOLUTE);
                }
            break;
        case FB_WALK:
            if (foxSpeedX == 0)
                fox->SetAction(FB_STILL, FB_SAME);
            else if (foxSpeedX > C_FOX_WALKTORUN) {
                fox->SetAction(FB_RUN, FB_RIGHT);
                fox->SetSwitch(C_FOX_RUNSWITCH, FB_ABSOLUTE);
                }
            else if (foxSpeedX < -C_FOX_WALKTORUN) {
                fox->SetAction(FB_RUN, FB_LEFT);
                fox->SetSwitch(C_FOX_RUNSWITCH, FB_ABSOLUTE);
                }
            break;
        case FB_BLURR:
            if ((foxSpeedX < C_FOX_RUNTOBLURR) && (foxSpeedX > C_FOX_WALKTORUN)) {
                fox->SetAction(FB_RUN, FB_RIGHT);
                fox->SetSwitch(C_FOX_RUNSWITCH, FB_ABSOLUTE);
                }
            else if ((foxSpeedX > -C_FOX_RUNTOBLURR) && (foxSpeedX < -C_FOX_WALKTORUN)) {
                fox->SetAction(FB_RUN, FB_LEFT);
                fox->SetSwitch(C_FOX_RUNSWITCH, FB_ABSOLUTE);
                }
            break;
        case FB_JUMPTHROW:
            if (fox->SwitchDone()) {
                fox->SetSwitchForward(FALSE);
                fox->SetAction(FB_JUMP, FB_SAME);
                fox->SetSwitch(C_FOX_JUMPSWITCH, FB_ABSOLUTE);
                fox->SetSwitchDone(FALSE);
                fox->SetAccelerationY(C_UNIT / 2, FB_ABSOLUTE);
                }
            else if ((fox->CurrentBitmapId() == 1) && (foxDir == FB_RIGHT)) {
                apple->SetActive(TRUE);
                apple->SetX(fox->X() + 60 * C_UNIT, FB_ABSOLUTE);
                apple->SetY(fox->Y() + 30 * C_UNIT, FB_ABSOLUTE);
                apple->SetVelocityX(8 * C_UNIT, FB_ABSOLUTE);
                apple->SetVelocityY(-4 * C_UNIT, FB_ABSOLUTE);
                apple->SetAccelerationX(0, FB_ABSOLUTE);
                apple->SetAccelerationY(C_UNIT / 4, FB_ABSOLUTE);
                }
            else if ((fox->CurrentBitmapId() == 1) && (foxDir == FB_LEFT)) {
                apple->SetActive(TRUE);
                apple->SetX(fox->X() + 15 * C_UNIT, FB_ABSOLUTE);
                apple->SetY(fox->Y() + 30 * C_UNIT, FB_ABSOLUTE);
                apple->SetVelocityX(-8 * C_UNIT, FB_ABSOLUTE);
                apple->SetVelocityY(-4 * C_UNIT, FB_ABSOLUTE);
                apple->SetAccelerationX(0, FB_ABSOLUTE);
                apple->SetAccelerationY(C_UNIT / 4, FB_ABSOLUTE);
                }
            break;
        case FB_THROW:
            if (fox->SwitchDone()) {
                fox->SetAction(FB_STILL, FB_SAME);
                fox->SetSwitchType(FB_HOR);
                fox->SetSwitch(0, FB_ABSOLUTE);
                fox->SetSwitchDone(FALSE);
                }
            else if ((fox->CurrentBitmapId() == 1) && (foxDir == FB_RIGHT)) {
                apple->SetActive(TRUE);
                apple->SetX(fox->X() + 60 * C_UNIT, FB_ABSOLUTE);
                apple->SetY(fox->Y() + 50 * C_UNIT, FB_ABSOLUTE);
                apple->SetVelocityX(8 * C_UNIT, FB_ABSOLUTE);
                apple->SetVelocityY(-4 * C_UNIT, FB_ABSOLUTE);
                apple->SetAccelerationX(0, FB_ABSOLUTE);
                apple->SetAccelerationY(C_UNIT / 4, FB_ABSOLUTE);
                }
            else if ((fox->CurrentBitmapId() == 1) && (foxDir == FB_LEFT)) {
                apple->SetActive(TRUE);
                apple->SetX(fox->X() + 20 * C_UNIT, FB_ABSOLUTE);
                apple->SetY(fox->Y() + 50 * C_UNIT, FB_ABSOLUTE);
                apple->SetVelocityX(-8 * C_UNIT, FB_ABSOLUTE);
                apple->SetVelocityY(-4 * C_UNIT, FB_ABSOLUTE);
                apple->SetAccelerationX(0, FB_ABSOLUTE);
                apple->SetAccelerationY(C_UNIT / 4, FB_ABSOLUTE);
                }
            break;
        case FB_JUMP:
            if ((foxSpeedY >= 0) && fox->SwitchForward())
                fox->SetSwitchForward(FALSE);
            else if (!fox->SwitchForward()) {
                if (foreground->OnSurface(fox)) {
                    if (foxSpeedX >= C_FOX_RUNMOVE) {
                        fox->SetAction(FB_RUN, FB_SAME);
                        fox->SetSwitch(C_FOX_RUNSWITCH, FB_ABSOLUTE);
                        }
                    else if (foxSpeedX == 0) {
                        fox->SetAction(FB_STILL, FB_SAME);
                        fox->SetSwitch(C_FOX_WALKSWITCH, FB_ABSOLUTE);
                        }
                    else {
                        fox->SetAction(FB_WALK, FB_SAME);
                        fox->SetSwitch(C_FOX_WALKSWITCH, FB_ABSOLUTE);
                        }
                    fox->SetAccelerationY(0, FB_ABSOLUTE);
                    fox->SetVelocityY(0, FB_ABSOLUTE);
                    fox->SetSwitchType(FB_HOR);
                    fox->SetSwitchForward(TRUE);
                    foreground->KeepOnSurface(fox);
                    fox->SetSwitchDone(FALSE);
                    }
                }
            break;
        default:
            break;
        }
}
