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
* Description:  Header file for the QMMaterial class.
*
****************************************************************************/

#ifndef __QMODEL_MATERIAL_HPP
#define __QMODEL_MATERIAL_HPP

#ifndef __QMODEL_OBJECT_HPP
#include "qmodel/object.hpp"
#endif

/*-------------------------- Class Definitions ----------------------------*/

//---------------------------------------------------------------------------
// Class to represent bound front and back material properties. Note that
// the material binding owns the child object and will delete it, but does
// not own the materials since materials will most likely be bound to
// multiple objects in the scene.
//---------------------------------------------------------------------------

class QMMaterial : public QMObject {
protected:
    QMObject    *child;     // Child material is bound to
    MGLMaterial *front;     // Front face material
    MGLMaterial *back;      // Back face material
    char        ownChild;   // True if we own the child

public:
            // Constructor
            QMMaterial(QMObject *child,MGLMaterial *front,MGLMaterial *back,
                ibool ownChild = true);

            // Destructor
    virtual ~QMMaterial() { if (ownChild) delete child; };

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

#endif  // __QMODEL_MATERIAL_HPP
