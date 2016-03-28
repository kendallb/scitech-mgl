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
* Description:  Header for the FX_Plane class.
*
****************************************************************************/

#ifndef __PLANE_HPP
#define __PLANE_HPP

#ifndef __FXSPRITE_HPP
#include "fxsprite.hpp"
#endif

/*-------------------------- Class Definitions ----------------------------*/

//---------------------------------------------------------------------------
// Class representing a scrolling plane, which built up of a number of
// bitmap tiles. Tiles may be marked missing from parts of the plane.
//---------------------------------------------------------------------------

class FX_Plane {
protected:
    SPR_Bitmap  **hBM;
    fix32_t     x;
    fix32_t     y;
    ushort      width;
    ushort      height;
    fix32_t     xv;
    fix32_t     xslide;
    fix32_t     xincrem;
    ushort      denom;

public:
            // Constructor
            FX_Plane(ushort width,ushort height,ushort denom,
                const char *tileListName,SPR_Bitmap *bitmapList[]);

            // Destructor
    virtual ~FX_Plane();

            // Set plane position etc
            void SetX(fix32_t x,FB_POSITION position);
            void SetY(fix32_t y,FB_POSITION position);
            void SetSlideX(fix32_t xs,FB_POSITION position);
            void SetVelocityX(fix32_t xv,FB_POSITION position);
            void SetIncremX(fix32_t xinc,FB_POSITION position);

            // Display the plane
            void Display();

            // Get info
            fix32_t X() const       { return x; };
            fix32_t Y() const       { return y; };
            ushort Width() const    { return width; };
            ushort Height() const   { return height; };
            fix32_t SlideX() const  { return xslide; };
            fix32_t IncremX() const { return xincrem; };
    };

//---------------------------------------------------------------------------
// Class representing a scrolling plane, which also has an attached surface
// list to represent areas that characters sprites like the fox can walk
// on.
//---------------------------------------------------------------------------

class FX_SurfacePlane : public FX_Plane {
protected:
    ibool        *surface;

public:
            // Constructor
            FX_SurfacePlane(ushort width,ushort height,ushort denom,
                const char *tileListName,SPR_Bitmap *bitmapList[],
                const char *surfaceListName);

            // Destructor
    virtual ~FX_SurfacePlane();

            // Keep the sprite on the plane surface
            void KeepOnSurface(FX_Sprite *sprite);

            // Return true if sprite is on plane surface
            ibool OnSurface(const FX_Sprite *sprite);
    };

#endif  // __PLANE_HPP
