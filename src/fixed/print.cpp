/****************************************************************************
*
*                 High Speed Fixed/Floating Point Library
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
* Language:     ANSI C
* Environment:  any
*
* Description:  C++ printing routines for XForm classes. Include directly
*               in your code if you need these functions.
*
****************************************************************************/

#include "fx/xform2d.h"
#include "fx/xform3d.h"

/*------------------------- Implementation --------------------------------*/

ostream& operator << (ostream& o,const FXForm2d& m)
/****************************************************************************
*
* Function:     operator <<
* Parameters:   o   - Stream to display matrix on
*               m   - Matrix to display
* Returns:      stream used for output
*
****************************************************************************/
{
    o << "3x3 Matrix:\n";
    for (int i = 0; i < 2; i++) {
        o << "  ";
        for (int j = 0; j < 3; j++) {
            o.width(9);
            o.precision(4);
            o.setf(ios::showpoint);
            o << FXrealToDbl(m.mat[i][j]) << "  ";
            }
        o << endl;
        }
    o << "  ";
    o.width(9); o.precision(4); o.setf(ios::showpoint);
    o << 0.0 << "  ";
    o.width(9); o.precision(4); o.setf(ios::showpoint);
    o << 0.0 << "  ";
    o.width(9); o.precision(4); o.setf(ios::showpoint);
    o << 1.0 << endl;
    return o;
}

ostream& operator << (ostream& o,const FXForm3d& m)
/****************************************************************************
*
* Function:     operator <<
* Parameters:   o   - Stream to display matrix on
*               m   - Matrix to display
* Returns:      stream used for output
*
****************************************************************************/
{
    o << "4x4 Matrix:\n";
    for (int i = 0; i < 4; i++) {
        o << "  ";
        for (int j = 0; j < 4; j++) {
            o.width(9);
            o.precision(4);
            o.setf(ios::showpoint);
            o << FXrealToDbl(m.mat[i][j]) << "  ";
            }
        o << "\n";
        }
    return o;
}
