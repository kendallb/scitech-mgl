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
* Language:     ANSI C++
* Environment:  any
*
* Description:  Implementation for the MGL region C++ wrapper class.
*
****************************************************************************/

#include "mgraph.hpp"

/*---------------------------- Implementation -----------------------------*/

MGLRegion& MGLRegion::operator = (const MGLRegion& r)
{
    MGL_freeRegion(rgn);
    rgn = MGL_copyRegion(r.rgn);
    return *this;
}

MGLRegion& MGLRegion::operator = (const MGLRect& r)
{
    MGL_freeRegion(rgn);
    rgn = MGL_rgnSolidRect(r.r());
    return *this;
}

MGLRegion& MGLRegion::operator = (region_t *r)
{
    MGL_freeRegion(rgn);
    rgn = r;
    return *this;
}

region_t * MGLRegion::operator + (const MGLRegion& r) const
{
    region_t *tmp = MGL_copyRegion(rgn);
    MGL_unionRegion(tmp,r.rgn);
    return tmp;
}

region_t * MGLRegion::operator + (const MGLRect& r) const
{
    region_t *tmp = MGL_copyRegion(rgn);
    MGL_unionRegionRect(tmp,&r.r());
    return tmp;
}

MGLRegion& MGLRegion::operator &= (const MGLRegion& r)
{
    region_t *old = rgn;
    rgn = MGL_sectRegion(rgn,r.rgn);
    MGL_freeRegion(old);
    return *this;
}

MGLRegion& MGLRegion::operator &= (const MGLRect& r)
{
    region_t *old = rgn;
    rgn = MGL_sectRegionRect(rgn,&r.r());
    MGL_freeRegion(old);
    return *this;
}

region_t * MGLRegion::operator - (const MGLRegion& r) const
{
    region_t *tmp = MGL_copyRegion(rgn);
    MGL_diffRegion(tmp,r.rgn);
    return tmp;
}

region_t * MGLRegion::operator - (const MGLRect& r) const
{
    region_t *tmp = MGL_copyRegion(rgn);
    MGL_diffRegionRect(tmp,&r.r());
    return tmp;
}
