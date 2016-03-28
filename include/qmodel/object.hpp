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
* Description:  Header file for the QMObject base class.
*
****************************************************************************/

#ifndef __QMODEL_OBJECT_HPP
#define __QMODEL_OBJECT_HPP

#ifndef __QUICK3D_QUICK3D_HPP
#include "quick3d/quick3d.hpp"
#endif

#ifndef __TCL_LIST_HPP
#include "tcl/list.hpp"
#endif

/*-------------------------- Class Definitions ----------------------------*/

//---------------------------------------------------------------------------
// Base class for all objects in the scene. All QMObject's are reference
// counted, so you can create instances of objects to save space (up to
// 65,536 instances). Objects must *always* be allocated with new and free
// before being inserted into a scene.
//---------------------------------------------------------------------------

class QMObject : public TCListNode {
public:
            // Constructor
            QMObject()  {};

            // Destructor
    virtual ~QMObject() {};

            // Methods to render the object
    virtual void renderShaded(Quick3d& dc) = 0;
    virtual void renderWireframe(Quick3d& dc) = 0;

            // Method to determine if a node is a primitive
    virtual ibool isPrimitive() const { return false; };

            // Method to return the bounds of the object
    virtual void getBounds(FXPoint3d& min,FXPoint3d& max) = 0;

            // Method to compute the dirty rectangle for the object
    virtual void computeDirtyRect(Quick3d& dc,MGLRect& dirty,ibool viewChanged = false) = 0;

            // Method to pre-process the object prior to rendering
    virtual void preProcess() {};
    };

typedef TCList<QMObject>            QMObjList;
typedef TCListIterator<QMObject>    QMObjListIter;

/*--------------------------- Global Variables ----------------------------*/

extern  ibool   QMrenderBounds;
extern  color_t QMboundsColor;

/*-------------------------- Function Prototypes --------------------------*/

void    QMcomputeNormal(FXVec3d& normal,int count,FXPoint3d *v,int orientation);
void    QMcomputeNormal(FXVec3d& normal,int count,FXPoint3d *v,short *index,int orientation);
void    QMdrawWireframeBox(Quick3d& dc,const FXPoint3d& min,const FXPoint3d& max);

#endif  // __QMODEL_OBJECT_HPP
