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
* Description:  Header file for the QMMaterialRegistry class.
*
****************************************************************************/

#ifndef __QMODEL_MATREG_HPP
#define __QMODEL_MATREG_HPP

#ifndef __QUICK3D_QUICK3D_HPP
#include "quick3d/quick3d.hpp"
#endif

#ifndef __TCL_LIST_HPP
#include "tcl/list.hpp"
#endif

/*-------------------------- Class Definitions ----------------------------*/

//---------------------------------------------------------------------------
// Class to implement a registry of materials used in the scene. For
// color map modes this allows the registry to allocate all the color
// map indices as required to allow all the desired colors in the scene
// to be mapped in. In RGB modes this simply manages all the materials
// allocated for the scene.
//---------------------------------------------------------------------------

class QMMaterialRegistry {
protected:
    // Nested class to manage the list of materials in the registry
    struct Node : public TCListNode {
        MGLMaterial *material;
        Node(MGLMaterial* material) : material(material) {};
        ~Node() { delete material; };
        };

    typedef TCList<Node>            NodeList;
    typedef TCListIterator<Node>    NodeListIter;

    NodeList    materials;          // List of materials in the scene
    real        diffuseRange;       // Range of diffuse values (0-1)
    int         startIndex;         // Starting index in palette
    int         endIndex;           // Ending index in palette

public:
            // Constructor
            QMMaterialRegistry(int startIndex,int endIndex,
                real diffuseRange = REAL(0.75));

            // Destructor
            ~QMMaterialRegistry() {};

            // Method to add a new material to the registry
            void add(MGLMaterial *m) { materials.addToHead(new Node(m)); };

            // Method to build the palette required by the registry
            void buildPalette(palette_t *pal);

            // Method to build the material shade tables
            void buildShadeTables(MGLDevCtx& dc,int entries);
    };

#endif  // __QMODEL_MATREG_HPP
