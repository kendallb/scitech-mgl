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
* Description:  Header file for the QMPolyhedron classes.
*
****************************************************************************/

#ifndef __QMODEL_POLYHEDR_HPP
#define __QMODEL_POLYHEDR_HPP

#ifndef __QMODEL_PRIMITIV_HPP
#include "qmodel/primitiv.hpp"
#endif

/*-------------------------- Class Definitions ----------------------------*/

//---------------------------------------------------------------------------
// Abstract base class for all polyhedron primitives.
//---------------------------------------------------------------------------

typedef void (*QMPolyhedronCallback)(int percent);

class QMPolyhedron : public QMPrimitive {
public:
    struct Poly {
        short   index;          // Index to first edge for polygon
        short   numEdges;       // Number of edges for the polygon
        };

protected:
    short       numVertices;    // Number of vertices in the object
    short       numPolys;       // Number of polygons in the object
    short       numEdges;       // Number of edges in the object
    FXPoint3d   *vertices;      // List of vertices for the object
    FXVec3d     *normals;       // Pre-computed polygon normals for object
    Poly        *polys;         // List of polygons for the object
    short       *edgeData;      // Array of polygon edge data
    ibool        isConvex;      // True if object is convex
    ibool       isSmooth;       // True if object is smooth
    int         backfaceMethod; // Type of backface handling
    int         orientation;    // Polygon orientation flag
    int         shadingType;    // Type of shading to use
    real        cntSmoothAngle; // Current smoothing angle in use
    QMPolyhedronCallback callback; // Callback for when building normals

            // Add the list of vertex normals to array
            int findPolyNormals(FXVec3d *toSmooth,FXVec3d *polyNormals,
                int polyIndex,int vertexIndex,real minDot);

            // Now smooth the vertex normals
            void smoothNormal(FXVec3d *n,FXVec3d *toSmooth,int numToSmooth);

            // Method to build flat surface normals
            void buildFlatNormals();

            // Method to build smooth surface normals
            void buildSmoothNormals(real smoothAngle);

public:
            // Constructor
            QMPolyhedron(ibool isConvex,int backfaceMethod,int orientation);

            // Destructor
    virtual ~QMPolyhedron();

            // Transform all vertices in the polyhedron
            void transform(const FXForm3d& xform);

            // Methods to render the object
    virtual void renderWireframe(Quick3d& dc);

            // Method to render the object
    virtual void renderShaded(Quick3d& dc);

            // Method to return the bounds of a primitive
    virtual void getBounds(FXPoint3d& min,FXPoint3d& max);

            // Get information about the object
            int getNumVertices()    { return numVertices; };
            int getNumPolys()       { return numPolys; };
            int getNumEdges()       { return numEdges; };

            // Method to change the surface normals to flat or smooth
            ibool makeFlat(QMPolyhedronCallback callback);
            ibool makeSmooth(real smoothAngle,QMPolyhedronCallback callback);

            // Method to change shading for object
            void setShading(ibool shade);

            // Method to set the polygon orientation
            void setOrientation(int o);

            // Method to set the backface handling method
            void setBackfaceMethod(int b)   { backfaceMethod = b; };
    };

#endif  // __QMODEL_POLYHEDR_HPP
