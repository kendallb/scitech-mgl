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
* Description:  Implementation for the FX_Plane class. Note that the
*               Plane class itself does not manage the bitmaps, as they
*               are all owned and managed by the bitmap manager class.
*
****************************************************************************/

#include "fbpch.hpp"
#include <stdio.h>

/*------------------------- Implementation --------------------------------*/

FX_Plane::FX_Plane(ushort width,ushort height,ushort denom,
    const char *tileListName,SPR_Bitmap *bitmapList[])
    : x(0), y(0), width(width), height(height), xv(0), xslide(0), xincrem(0),
      denom(denom)
/****************************************************************************
*
* Function:     FX_Plane::FX_Plane
* Parameters:   width           - Width of the plane in tile units
*               height          - Height of the plane in tile units
*               denom           - Divisor used to divide plane speed by
*               tileListName    - Name of tile list file
*               bitmapList      - List of bitmaps to use for tiling plane
*
* Description:  Constructor for the scrolling plane class. We build the
*               plane as a set of attached bitmap tiles, which may or may
*               not be present.
*
****************************************************************************/
{
    int     i,id,tileCount = width * height;
    FILE    *f;

    // Allocate memory for bitmap array
    if ((hBM = new SPR_Bitmap*[tileCount]) == NULL)
        MGL_fatalError("Out of memory!");

    // Read tile bitmap id's from the tile list file
    if ((f = fopen(tileListName,"r")) == NULL)
        MGL_fatalError("Unable to locate tile list!\n");

    for (i = 0; i < tileCount; i++) {
        fscanf(f, "%d\n", &id);
        if (id > 0)
            hBM[i] = bitmapList[id-1];
        else hBM[i] = NULL;
        }

    fclose(f);
}

FX_Plane::~FX_Plane()
/****************************************************************************
*
* Function:     FX_Plane::~FX_Plane
*
* Description:  Destructor for the FX_Plane class.
*
****************************************************************************/
{
    delete [] hBM;
}

void FX_Plane::SetX(fix32_t newx,FB_POSITION position)
/****************************************************************************
*
* Function:     FX_Plane::SetX
* Parameters:   newx        - New X coordinate for plane
*               position    - Position type for coordinate
*
* Description:  Sets the plane starting X coordinate given the specified
*               position, and will wrap the plane coordinates at the
*               plane boundaries.
*
****************************************************************************/
{
    fix32_t xinc;

    if (position == FB_ABSOLUTE)
        x = newx;
    else if (position == FB_RELATIVE)
        x += newx;
    else if (position == FB_AUTOMATIC) {
        if (xslide > 0)
            xinc = xincrem;
        else if (xslide < 0)
            xinc = -xincrem;
        else
            xinc = 0;

        x += (xv + xinc) / denom;
        xslide -= xinc;
        xv = 0;

        if (ABS(xslide) < xincrem) {
            x += xslide / denom;
            xslide = 0;
            xincrem = 0;
            }
        }

    if (x < 0)
        x += (width * C_TILE_W) << 16;
    else if (x >= (width * C_TILE_W) << 16)
        x -= (width * C_TILE_W) << 16;
}

void FX_Plane::SetY(fix32_t newy,FB_POSITION position)
/****************************************************************************
*
* Function:     FX_Plane::SetY
* Parameters:   newy        - New Y coordinate for plane
*               position    - Position type for coordinate
*
* Description:  Sets the plane starting Y coordinate given the specified
*               position, and will wrap the plane coordinates at the
*               plane boundaries.
*
****************************************************************************/
{
    if (position == FB_ABSOLUTE)
        y = newy;
    else
        y += newy;

    if (y < 0)
        y += (height * C_TILE_H) << 16;
    else if (y >= (height * C_TILE_H) << 16)
        y -= (height * C_TILE_H) << 16;
}

void FX_Plane::SetSlideX(fix32_t xs,FB_POSITION position)
/****************************************************************************
*
* Function:     FX_Plane::SetSlideX
* Parameters:   xs          - New X slide value
*               position    - Position type for coordinate
*
****************************************************************************/
{
    if (position == FB_ABSOLUTE)
        xslide = xs;
    else if (position == FB_RELATIVE)
        xslide += xs;
}

void FX_Plane::SetVelocityX(fix32_t newxv,FB_POSITION position)
/****************************************************************************
*
* Function:     FX_Plane::SetVelocityX
* Parameters:   newxv       - New X velocity value
*               position    - Position type for coordinate
*
****************************************************************************/
{
    if (position == FB_ABSOLUTE)
        xv = newxv;
    else if (position == FB_RELATIVE)
        xv += newxv;
}

void FX_Plane::SetIncremX(fix32_t xinc,FB_POSITION position)
/****************************************************************************
*
* Function:     FX_Plane::SetIncremX
* Parameters:   xinc        - New X increment value
*               position    - Position type for coordinate
*
****************************************************************************/
{
    if (position == FB_ABSOLUTE)
        xincrem = xinc;
    else if (position == FB_RELATIVE)
        xincrem += xinc;
}

void FX_Plane::Display()
/****************************************************************************
*
* Function:     FX_Plane::Display
* Parameters:   xRes,yRes   - Current screen resolution
*
* Description:  Displays the plane at the current position.
*
****************************************************************************/
{
    int x1   = (x >> 16) / C_TILE_W;
    int xmod = (x >> 16) % C_TILE_W;
    int y1   = (y >> 16) / C_TILE_H;
    int ymod = (y >> 16) % C_TILE_H;
    int x2   = x1 + C_SCREEN_W / C_TILE_W;
    int y2   = y1 + C_SCREEN_H / C_TILE_H;

    // Draw the current sprite bitmap. Note that we scale the destination
    // coordinates to our destination device if we are not in the native
    // graphics mode that all game logic is run in (640x480 in this case).
    if (renderYRes == C_SCREEN_H) {
        for (int j = y1; j < y2; j++)
            for (int i = x1; i <= x2; i++) {
                int n = (i % width) + j * width;
                if (hBM[n] != NULL) {
                    hBM[n]->draw((i - x1) * C_TILE_W - xmod,
                                 (j - y1) * C_TILE_H - ymod);
                    }
                }
        }
    else {
        for (int j = y1; j < y2; j++)
            for (int i = x1; i <= x2; i++) {
                int n = (i % width) + j * width;
                if (hBM[n] != NULL) {
                    hBM[n]->draw(SCALEX((i - x1) * C_TILE_W - xmod),
                                 SCALEY((j - y1) * C_TILE_H - ymod));
                    }
                }
        }
}

FX_SurfacePlane::FX_SurfacePlane(ushort width,ushort height,ushort denom,
    const char *tileListName,
    SPR_Bitmap *bitmapList[],const char *surfaceListName)
    : FX_Plane(width,height,denom,tileListName,bitmapList)
/****************************************************************************
*
* Function:     FX_SurfacePlane::FX_SurfacePlane
* Parameters:   width           - Width of the plane in tile units
*               height          - Height of the plane in tile units
*               denom           - TODO:
*               tileListName    - Name of tile list file
*               bitmapList      - List of bitmaps to use for tiling plane
*               surfaceNameList - Name of optional surface list
*
* Description:  Constructor for the scrolling plane class that has an
*               associated surface.
*
****************************************************************************/
{
    int     i,id,tileCount = width * height;
    FILE    *f;

    // Allocate memory for surface array
    if ((surface = new ibool[tileCount]) == NULL)
        MGL_fatalError("Out of memory!");

    // Read surface list
    if ((f = fopen(surfaceListName,"r")) == NULL)
        MGL_fatalError("Unable to locate surface list!\n");

    for (i = 0; i < tileCount; i++) {
        fscanf(f, "%d\n", &id);
        surface[i] = (id != 0);
        }

    fclose(f);
}

FX_SurfacePlane::~FX_SurfacePlane()
/****************************************************************************
*
* Function:     FX_SurfacePlane::~FX_SurfacePlane
*
* Description:  Destructor for the FX_SurfacePlane class.
*
****************************************************************************/
{
    delete [] surface;
}

void FX_SurfacePlane::KeepOnSurface(FX_Sprite *sprite)
/****************************************************************************
*
* Function:     FX_SurfacePlane::KeepOnSurface
* Parameters:   sprite  - Sprite to keep on surface
*
* Description:  Keeps the sprite on the plane surface by moving it up or
*               down one tile as needed.
*
****************************************************************************/
{
    const FX_SpriteBitmap *bmp = sprite->CurrentBitmap();
    int x = (sprite->X() >> 16) / C_TILE_W;
    int y = ((sprite->Y() >> 16) + bmp->y + bmp->height) / C_TILE_H;
    int n = (x % width) + y * width;

    // Move up or down on surface
    if (!surface[n]) {
        if (surface[n + width])
            y += 1;
        else if (surface[n - width])
            y -= 1;
        }

    sprite->SetY((y * C_TILE_H - (bmp->y + bmp->height)) << 16, FB_ABSOLUTE);
}

ibool FX_SurfacePlane::OnSurface(const FX_Sprite *sprite)
/****************************************************************************
*
* Function:     FX_SurfacePlane::OnSurface
* Parameters:   sprite  - Sprite to text
*
* Description:  Tests if the given sprite is currently on the plane surface
*               or in mid air.
*
****************************************************************************/
{
    const FX_SpriteBitmap *bmp = sprite->CurrentBitmap();
    int x = ((sprite->X() >> 16) + sprite->Width() / 2) / C_TILE_W;
    int y = ((sprite->Y() >> 16) + bmp->y + bmp->height) / C_TILE_H;
    return surface[(x % width) + y * width];
}

