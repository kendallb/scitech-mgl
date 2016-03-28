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
* Description:  Code to implement the Apple.
*
****************************************************************************/

#include "fbpch.hpp"

/*------------------------------ Implementation ---------------------------*/

void CreateApple()
/****************************************************************************
*
* Function:     CreateApple
*
* Description:  Creates the apple sprite with all it's associated bitmaps.
*
****************************************************************************/
{
    apple = new FX_Sprite(1, C_APPLE_STARTX, C_APPLE_STARTY,
        C_APPLE_SIZEX, C_APPLE_SIZEY, C_FORE_W * C_TILE_W,
        C_FORE_H * C_TILE_H, 0, false);
    apple->AddBitmap(bitmapList[61],FB_NONE,FB_RIGHT,32,32,0,0);
    apple->SetAction(FB_NONE, FB_RIGHT);
}

void UpdateApple()
/****************************************************************************
*
* Function:     UpdateApple
*
* Description:  Update the apple sprite for the next frame
*
****************************************************************************/
{
    if ((apple->VelocityX() != 0) && (apple->X() >= 420 * C_UNIT)) {
        apple->SetX(0, FB_ABSOLUTE);
        apple->SetY(0, FB_ABSOLUTE);
        apple->SetAccelerationX(0, FB_ABSOLUTE);
        apple->SetAccelerationY(0, FB_ABSOLUTE);
        apple->SetVelocityX(0, FB_ABSOLUTE);
        apple->SetVelocityY(0, FB_ABSOLUTE);
        apple->SetActive(FALSE);
        }
}

