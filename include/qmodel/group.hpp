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
* Description:  Header file for the QMGroup class.
*
****************************************************************************/

#ifndef __QMODEL_GROUP_HPP
#define __QMODEL_GROUP_HPP

#ifndef __QMODEL_PRIMITIV_HPP
#include "qmodel/primitiv.hpp"
#endif

/*-------------------------- Class Definitions ----------------------------*/

//---------------------------------------------------------------------------
// Class to represent a group of objects in the scene.
//---------------------------------------------------------------------------

class QMGroup : public QMObject {
protected:
    QMObjList   children;

public:
            // Constructor
            QMGroup() : QMObject() {};

            // Destructor
    virtual ~QMGroup() {};

            // Method to add an object to the group
            void add(QMObject* o) { children.addToHead(o); };

            // Method to detach a child from the group
            void detach(QMObject *o);

            // Methods to render the object
    virtual void renderShaded(Quick3d& dc);
    virtual void renderWireframe(Quick3d& dc);

            // Method to pre-process the object prior to rendering
    virtual void preProcess();

            // Method to return the bounds of the object
    virtual void getBounds(FXPoint3d& min,FXPoint3d& max);

            // Method to compute the dirty rectangle for the object
    virtual void computeDirtyRect(Quick3d& dc,MGLRect& dirty,ibool viewChanged);
    };

#endif  // __QMODEL_GROUP_HPP
