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
* Description:  Header file for the integer point class.
*
****************************************************************************/

#ifndef __MGLPOINT_HPP
#define __MGLPOINT_HPP

#ifndef __MGRAPH_H
#error  Please include MGRAPH.H, not MGLPOINT.HPP directly
#endif

/*--------------------------- Class Definition ----------------------------*/

#pragma pack(1)             /* Pack structures to byte granularity      */

//---------------------------------------------------------------------------
// The following defines a 2d integer point type, and defines a number of
// useful inline operations on these points. Use the defined methods to
// convert between 2d real points and 2d integer points. The methods provided
// will ensure that the results obtained are consistent.
//---------------------------------------------------------------------------

class MGLPoint {
public:
    int x,y;            // Coordinates of the point

            // Default constructor (does nothing)
    inline  MGLPoint()  {};

            // Constructor given 2 integers
    inline  MGLPoint(int x1,int y1) { x = x1; y = y1; };

            // Constructor given a point
    inline  MGLPoint(const MGLPoint& p) { x = p.x; y = p.y; };

            // Constructor given an MGL C style point
    inline  MGLPoint(const point_t& p)  { x = p.x; y = p.y; };

            // Overloaded cast to an MGL C style point
    inline  operator point_t& () const  { return (point_t&)x; };

            // Forcible cast to an MGL C style point
    inline  point_t& p() const  { return (point_t&)x; };

            // Assignment operator given a point
    inline  MGLPoint& operator = (const MGLPoint& p)
            {
                x = p.x; y = p.y; return *this;
            };

            // Standard arithmetic operators for 2d integer points
    friend  MGLPoint operator + (const MGLPoint& v1,const MGLPoint& v2);
    friend  MGLPoint operator - (const MGLPoint& v1,const MGLPoint& v2);
    friend  MGLPoint operator * (const MGLPoint& v1,const MGLPoint& v2);
    friend  MGLPoint operator * (const MGLPoint& v,int s);
    friend  MGLPoint operator * (int s,const MGLPoint& v);
    friend  MGLPoint operator / (const MGLPoint& v,int s);

            // Faster methods to add and multiply
    inline  MGLPoint& operator += (const MGLPoint& v);
    inline  MGLPoint& operator -= (const MGLPoint& v);
    inline  MGLPoint& operator *= (const MGLPoint& v);
    inline  MGLPoint& operator *= (int s);
    inline  MGLPoint& operator /= (int s);

            // Methods to negate a vector
    inline  MGLPoint operator - () const;
    inline  MGLPoint& negate();         // Faster

            // Method to determine if a point is zero
    inline  ibool isZero() const;

            // Method to determine if two points are equal
    inline  ibool operator == (const MGLPoint& p) const;

            // Method to determine if two points are not equal
    inline  ibool operator != (const MGLPoint& p) const;
    };

/*------------------------- Inline member functions -----------------------*/

//---------------------------------------------------------------------------
// Standard arithmetic operators for integer points.
//---------------------------------------------------------------------------

inline MGLPoint operator + (const MGLPoint& p1,const MGLPoint& p2)
{
    return MGLPoint(p1.x + p2.x, p1.y + p2.y);
}

inline MGLPoint operator - (const MGLPoint& p1,const MGLPoint& p2)
{
    return MGLPoint(p1.x - p2.x, p1.y - p2.y);
}

inline MGLPoint operator * (const MGLPoint& p1,const MGLPoint& p2)
{
    return MGLPoint(p1.x * p2.x, p1.y * p2.y);
}

inline MGLPoint operator * (const MGLPoint& p1,int s)
{
    return MGLPoint(p1.x * s, p1.y * s);
}

inline MGLPoint operator * (int s,const MGLPoint& p1)
{
    return MGLPoint(p1.x * s, p1.y * s);
}

inline MGLPoint operator / (const MGLPoint& p1,int s)
{
    return MGLPoint(p1.x / s, p1.y / s);
}

//---------------------------------------------------------------------------
// Faster methods to add and multiply integer points.
//---------------------------------------------------------------------------

inline MGLPoint& MGLPoint::operator += (const MGLPoint& p)
{
    x += p.x;
    y += p.y;
    return *this;
}

inline MGLPoint& MGLPoint::operator -= (const MGLPoint& p)
{
    x -= p.x;
    y -= p.y;
    return *this;
}

inline MGLPoint& MGLPoint::operator *= (const MGLPoint& p)
{
    x *= p.x;
    y *= p.y;
    return *this;
}

inline MGLPoint& MGLPoint::operator *= (int s)
{
    x *= s;
    y *= s;
    return *this;
}

inline MGLPoint& MGLPoint::operator /= (int s)
{
    x /= s;
    y /= s;
    return *this;
}

//---------------------------------------------------------------------------
// Methods to negate points.
//---------------------------------------------------------------------------

inline MGLPoint MGLPoint::operator - () const
{
    return MGLPoint(-x,-y);
}

inline MGLPoint& MGLPoint::negate()
{
    x = -x;
    y = -y;
    return *this;
}

//---------------------------------------------------------------------------
// Miscellaneous operations.
//---------------------------------------------------------------------------

inline ibool MGLPoint::isZero() const
{
    return (x == 0 && y == 0);
}

inline ibool MGLPoint::operator == (const MGLPoint& p) const
{
    return (x == p.x && y == p.y);
}

inline ibool MGLPoint::operator != (const MGLPoint& p) const
{
    return (x != p.x || y != p.y);
}

#pragma pack()

#endif  // __MGLPOINT_HPP
