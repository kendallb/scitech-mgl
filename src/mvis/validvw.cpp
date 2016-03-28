/****************************************************************************
*
*                    MegaVision Application Framework
*
*      A C++ GUI Toolkit for the SciTech Multi-platform Graphics Library
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
* Environment:  Any
*
* Description:  Implementation for the validView function.
*
****************************************************************************/

#include "mvis/mvision.hpp"
#include "mvis/mprogram.hpp"

/*----------------------------- Implementation ----------------------------*/

MVView *MV_validView(MVView *view)
/****************************************************************************
*
* Function:     MV_validView
* Parameters:   view    - View to check for validity
* Returns:      Pointer to the view if valid, NULL if not.
*
* Description:  Checks to see if the safety pool has been hit during
*               memory allocation, or the view was not validly constructed.
*               If either is the case, the view is destroyed and the
*               appropriate action is taken (returning NULL for the
*               view pointer).
*
****************************************************************************/
{
    if (view) {
        if (MV_lowMemory()) {
            delete view;

            // Only call the outOfMemory() routine _iff_ the application
            // instance has been successfully initialised. This will
            // be true only if the application pointer is not NULL.
            if (MVProgram::application)
                MVProgram::application->outOfMemory();
            return NULL;
            }
        if (!view->valid(cmValid)) {
            delete view;
            return NULL;
            }
        }
    return view;
}
