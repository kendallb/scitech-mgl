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
* Description:  Header file for the QMOFFObject classes.
*
****************************************************************************/

#ifndef __QMODEL_OFFOBJEC_HPP
#define __QMODEL_OFFOBJEC_HPP

#ifndef __QMODEL_POLYHEDR_HPP
#include "qmodel/polyhedr.hpp"
#endif

/*-------------------------- Class Definitions ----------------------------*/

//---------------------------------------------------------------------------
// Class for loading OFF Object files as QMPolyhedron objects. Defaults
// to flat shaded mode.
//---------------------------------------------------------------------------

class QMOFFObject : public QMPolyhedron {
public:
    enum {  Ok,             // Object was loaded into memory correctly
            FileNotFound,   // Could not locate object file
            ParseError,     // Parse error reading object file
            OutOfMemory,    // Not enough memory to load object file
            };

protected:
    int     _status;        // Status of the object file

public:
            // Constructor
            QMOFFObject(const char *filename,ibool isConvex,
                int backfaceMethod,int orientation);

            // Return true if the object initialisation failed
            ibool fail() const  { return (_status != Ok); };

            // Return the status of the object
            int status() const  { return _status; };
    };

#endif  // __QMODEL_OFFOBJEC_HPP
