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
* Description:  Code to implement the Bear character.
*
****************************************************************************/

#include "fbpch.hpp"

/*---------------------------- Global Variables ---------------------------*/

int                 chewCount;      // Number of times bear has chewed

/*------------------------------ Implementation ---------------------------*/

void CreateBear()
/****************************************************************************
*
* Function:     CreateBear
*
* Description:  Creates the bear sprite with all it's associated bitmaps.
*
****************************************************************************/
{
    static short x[C_BBT] = {
        14, 10,
         8, 12, 13, 14, 10, 10,  9,  9,  9,  9,  8, 9,
        11,  6,  1,  0,  3, 13, 11,  7,  1,  1,  3, 14
        };
    static short y[C_BBT] = {
         7,  7,
         3,  8,  9,  7,  7,  3,  3,  3,  3,  3,  3,  3,
         1,  1,  2,  2,  3,  1,  0,  1,  1,  2,  3,  2
        };
    static ushort w[C_BBT] = {
        127,129,
        127,153,183,153,129,138,146,150,152,151,143,139,
        131,136,140,141,136,125,131,135,140,140,136,126
        };
    static ushort h[C_BBT] = {
        80, 80,
        84, 79, 78, 80, 80, 84, 84, 84, 84, 84, 84, 84,
        86, 86, 86, 85, 84, 86, 87, 86, 87, 85, 84, 86
        };

    bear = new FX_Sprite(C_BBT, C_BEAR_STARTX, C_BEAR_STARTY,
        C_BEAR_SIZEX, C_BEAR_SIZEY, C_FORE_W * C_TILE_W,
        C_FORE_H * C_TILE_H, C_BEAR_AS, true);

    for (int i = 0; i < C_BBT; i++) {
        FB_ACTION      action = FB_WALK;
        FB_DIRECTION   direction = FB_LEFT;

        if (i < 2)          {   action = FB_MISS;      direction = FB_LEFT;   }
        else if (i < 8)     {   action = FB_STRIKE;    direction = FB_LEFT;   }
        else if (i < 14)    {   action = FB_CHEW;      direction = FB_LEFT;   }
        else if (i < 26)    {   action = FB_WALK;      direction = FB_LEFT;   }
        bear->AddBitmap(bitmapList[i + C_TILETOTAL + C_FBT],action,direction,
            w[i], h[i], x[i], y[i]);
        }
    bear->SetAction(FB_WALK, FB_LEFT);
    bear->SetVelocityX(-C_BEAR_WALKMOVE, FB_ABSOLUTE);
    bear->SetSwitch(C_BEAR_WALKSWITCH, FB_ABSOLUTE);
}

void UpdateBear()
/****************************************************************************
*
* Function:     UpdateBear
*
* Description:  Update the bear sprite for the next frame
*
****************************************************************************/
{
    long foxX   = fox->X();
    long foxY   = fox->Y();
    long bearX  = bear->X();
    long bearY  = bear->Y();
    long appleX = apple->X();
    long appleY = apple->Y();

    switch (bear->Action()) {
        case FB_STRIKE:
            if (bear->CurrentBitmapId() == 2) {
                if ((bearX > foxX - C_UNIT * 30) && (bearX < foxX + C_UNIT * 40) && (bearY < foxY + C_UNIT * 60)) {
                    fox->SetActive(FALSE);
#ifdef  USE_DWSTK
                    sound_PlaySound(sound_STRIKE);
#endif
                    }
                else {
                    bear->SetAction(FB_MISS, FB_SAME);
                    bear->SetSwitch(C_BEAR_MISSSWITCH, FB_ABSOLUTE);
                    bear->SetSwitchDone(FALSE);
                    }
                }
            else if (bear->SwitchDone()) {
                bear->SetAction(FB_CHEW, FB_SAME);
                bear->SetSwitchDone(FALSE);
                chewCount = 0;
                }
            break;
        case FB_MISS:
            if (bear->SwitchDone()) {
                bear->SetAction(FB_WALK, FB_SAME);
                bear->SetVelocityX(-C_BEAR_WALKMOVE, FB_ABSOLUTE);
                bear->SetSwitch(C_BEAR_WALKSWITCH, FB_ABSOLUTE);
                bear->SetSwitchType(FB_HOR);
                }
            break;
        case FB_WALK:
            if (apple->Active() && (appleX > bearX) && (appleX > bearX + 80 * C_UNIT) && (appleY > bearY + 30 * C_UNIT)) {
                bear->SetAction(FB_STRIKE, FB_SAME);
                bear->SetVelocityX(0, FB_ABSOLUTE);
                bear->SetSwitchType(FB_TIME);
                bear->SetSwitch(C_BEAR_STRIKESWITCH, FB_ABSOLUTE);
                bear->SetSwitchDone(FALSE);
                }
            else if ((bearX > foxX - C_UNIT * 30) && (bearX < foxX + C_UNIT * 30) && (bearY < foxY + C_UNIT * 60)) {
                bear->SetAction(FB_STRIKE, FB_SAME);
                bear->SetVelocityX(0, FB_ABSOLUTE);
                bear->SetSwitchType(FB_TIME);
                bear->SetSwitch(C_BEAR_STRIKESWITCH, FB_ABSOLUTE);
                bear->SetSwitchDone(FALSE);
                }
            break;
        case FB_CHEW:
            if (++chewCount >= 512) {
                bear->SetAction(FB_STRIKE, FB_SAME);
                bear->SetSwitch(C_BEAR_STRIKESWITCH, FB_ABSOLUTE);
                bear->SetVelocityX(0, FB_ABSOLUTE);
                bear->SetSwitchDone(FALSE);

                if (fox->Direction() == FB_RIGHT) {
                    foreground->SetSlideX(-C_BOUNDDIF, FB_RELATIVE);
                    midground->SetSlideX( -C_BOUNDDIF, FB_RELATIVE);
                    background->SetSlideX(-C_BOUNDDIF, FB_RELATIVE);
                    }
                long chewDif = fox->X();

                fox->SetActive(TRUE);
#ifdef  USE_DWSTK
                sound_PlaySound(sound_STUNNED);
#endif
                fox->SetAction(FB_STUNNED, FB_LEFT);
                fox->SetX(bear->X(), FB_ABSOLUTE);
                fox->SetY(bear->Y(), FB_ABSOLUTE);
                fox->SetAccelerationX(0, FB_ABSOLUTE);
                fox->SetAccelerationY(C_UNIT / 2, FB_ABSOLUTE);
                fox->SetVelocityX(-8 * C_UNIT, FB_ABSOLUTE);
                fox->SetVelocityY(-10 * C_UNIT, FB_ABSOLUTE);
                fox->SetSwitch(0, FB_ABSOLUTE);

                chewDif -= fox->X();

                foreground->SetSlideX(-chewDif, FB_RELATIVE);
                midground->SetSlideX( -chewDif, FB_RELATIVE);
                background->SetSlideX(-chewDif, FB_RELATIVE);
                foreground->SetIncremX(C_BOUNDINCREM, FB_ABSOLUTE);
                midground->SetIncremX(C_BOUNDINCREM, FB_ABSOLUTE);
                background->SetIncremX(C_BOUNDINCREM, FB_ABSOLUTE);
                }
            break;
        default:
            break;
        }
}
