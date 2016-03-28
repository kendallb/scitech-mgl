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
* Description:  Precompiled header file for the Fox & Bear demo.
*
****************************************************************************/

#ifndef __FBPCH_HPP
#define __FBPCH_HPP

#include "gm/gm.h"
#include "gm/sprite.h"
#include "tcl/techlib.hpp"
#include "fxsprite.hpp"
#include "fxplane.hpp"
#ifdef  USE_DWSTK
#include "sound.h"
#endif

/*---------------------- Macros and type definitions ----------------------*/

// Size of tiles and size of game virtual screen. Note that we rescale
// on the fly to any real screen dimensions when we draw the bitmaps
// (only scaling coordinates; the bitmaps are prescaled when they are
// loaded) but all the game logic runs as if the game is running in a
// 640x480 mode.

#define C_TILE_W                32
#define C_TILE_H                32
#define C_SCREEN_W             640
#define C_SCREEN_H             480

// Maximum size of bitmaps on disk that need to be converted

#define BMP_MAXX        200
#define BMP_MAXY        100

// Macros for scaling to device coordinates from game coordinates

#define SCALEX(x)               (((x) * renderXRes) / C_SCREEN_W)
#define SCALEY(y)               (((y) * renderYRes) / C_SCREEN_H)
#define SCALEW(x)               SCALEX(x)+((((x) * renderXRes) % C_SCREEN_W) > 0)
#define SCALEH(y)               SCALEY(y)+((((y) * renderYRes) % C_SCREEN_H) > 0)

// Size of our parallax backgrounds and our world viewport

#define C_FORE_W                80
#define C_FORE_H                15
#define C_MID_W                 40
#define C_MID_H                 15
#define C_BACK_W                25
#define C_BACK_H                15
#define C_BACK_RECT_H           13
#define C_WORLD_W               20
#define C_WORLD_H               15

// Speed scale factors for the parallax scrolling backgrounds

#define C_BACK_DENOM            12
#define C_MID_DENOM              3
#define C_FORE_DENOM             1

// Tile total constants

#define C_TILETOTAL            123      // Total number of tiles
#define C_FBT                  112      // Total bitmaps for fox sprite
#define C_BBT                   26      // Total bitmaps for bear sprite

// Bitmap counts of each fox action group

#define C_FOXSTILL               1
#define C_FOXWALK               12
#define C_FOXRUN                16
#define C_FOXJUMP                4
#define C_FOXTHROW               2
#define C_FOXCROUCH              1
#define C_FOXSTOP                1
#define C_FOXSTUNNED             1
#define C_FOXJUMPTHROW           2
#define C_FOXCROUCHWALK         12
#define C_FOXBLURR               4

// Bitmap counts for each bear action group

#define C_BEARMISS               2
#define C_BEARWALK              12
#define C_BEARSTRIKE            12

// Constants used for timing and interaction. Note that all coordinates,
// velocities and accelerations are stored in 16.16 fixed point format.

#define C_UNIT               (long)(65536)

#define C_FOX_XMOVE          (C_UNIT / 4)

#define C_BOUNDINCREM        (  5 * C_UNIT)
#define C_BOUNDDIF           (240 * C_UNIT)

#define C_FOX_WALKMOVE       (  6 * C_UNIT)
#define C_FOX_RUNMOVE        ( 18 * C_UNIT)
#define C_FOX_JUMPMOVE       (  9 * C_UNIT)

#define C_FOX_WALKSWITCH     (  6 * C_UNIT)
#define C_FOX_JUMPSWITCH     (  9 * C_UNIT)
#define C_FOX_THROWSWITCH    ( 15 * C_UNIT)
#define C_FOX_RUNSWITCH      ( 18 * C_UNIT)
#define C_FOX_BLURRSWITCH    ( 18 * C_UNIT)

#define C_FOX_WALKTORUN      (  4 * C_UNIT)
#define C_FOX_RUNTOBLURR     ( 14 * C_UNIT)

#define C_BEAR_WALKMOVE      (  1 * C_UNIT)

#define C_BEAR_WALKSWITCH    (  6 * C_UNIT)
#define C_BEAR_STRIKESWITCH  (  8 * C_UNIT)
#define C_BEAR_MISSSWITCH    ( 10 * C_UNIT)

// Starting coordinates and dimensions of the fox sprite

#define C_FOX_STARTX         (150 * C_UNIT)
#define C_FOX_STARTY         (318 * C_UNIT)
#define C_FOX_SIZEX          108
#define C_FOX_SIZEY          105
#define C_FOX_AS             6

// Starting coordinates and dimensions of the bear sprite

#define C_BEAR_STARTX        (600 * C_UNIT)
#define C_BEAR_STARTY        (329 * C_UNIT)
#define C_BEAR_SIZEX         196
#define C_BEAR_SIZEY         88
#define C_BEAR_AS            6

// Starting coordinates and dimensions of the apple sprite

#define C_APPLE_STARTX      (50 * C_UNIT)
#define C_APPLE_STARTY      (390 * C_UNIT)
#define C_APPLE_SIZEX       32
#define C_APPLE_SIZEY       32

// MGL keyboard scancodes

#define KEYDOWN_OR_REPEAT       (EVT_KEYDOWN | EVT_KEYREPEAT)

/*---------------------------- Global Variables ---------------------------*/

extern int              renderXRes;     // Resolution of rendering device
extern int              renderYRes;
extern SPR_Bitmap       *bitmapList[];  // List of all loaded bitmaps
extern FX_Sprite        *fox;           // Fox sprite
extern FX_Sprite        *bear;          // Bear sprite
extern FX_Sprite        *apple;         // Apple sprite
extern FX_SurfacePlane  *foreground;    // Foreground plane
extern FX_Plane         *midground;     // Midground plane
extern FX_Plane         *background;    // Background plane

/*------------------------- Function Prototypes ---------------------------*/

// In FOX.CPP
void CreateFox();
void UpdateFox();

// In BEAR.CPP
void CreateBear();
void UpdateBear();

// In APPLE.CPP
void CreateApple();
void UpdateApple();

// In FOXBEAR.CPP
void SwitchFullscreen();
void SwitchWindowed();
void RegisterDrivers();

#endif  // __FBPCH_HPP

