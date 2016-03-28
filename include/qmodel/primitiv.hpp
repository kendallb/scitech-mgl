/****************************************************************************
*
*               QuickModeler - A Real-Time 3D Modeling System
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
* Environment:  any
*
* Description:  Header file for the QMPrimitive base class.
*
****************************************************************************/

#ifndef __QMODEL_PRIMITIV_HPP
#define __QMODEL_PRIMITIV_HPP

#ifndef __QMODEL_OBJECT_HPP
#include "qmodel/object.hpp"
#endif

/*-------------------------- Class Definitions ----------------------------*/

//---------------------------------------------------------------------------
// Base class for all primitive objects which can be rendered in the scene
//---------------------------------------------------------------------------

class QMPrimitive : public QMObject {
public:
            // Constructor
            QMPrimitive() : QMObject() {};

            // Destructor
    virtual ~QMPrimitive() {};

            // Method to determine if a node is a primitive
    virtual ibool isPrimitive() const { return true; };

            // Default method to compute the dirty rectangle for primitives
    virtual void computeDirtyRect(Quick3d& dc,MGLRect& dirty,ibool viewChanged);
    };

#endif  // __QMODEL_PRIMITIV_HPP
