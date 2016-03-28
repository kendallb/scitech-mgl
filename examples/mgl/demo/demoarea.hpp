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
* Language:     C++ 3.0
* Environment:  any
*
* Description:  Header file for the class DemoArea, a class for
*               performing the demos in a double buffered window.
*
****************************************************************************/

#ifndef __DEMOAREA_HPP
#define __DEMOAREA_HPP

#include "mvis/mrendera.hpp"

/*-------------------------- Class definitions ----------------------------*/

//---------------------------------------------------------------------------
// The DemoArea class is a class used to represent an area for peforming
// the double buffered demonstrations.
//---------------------------------------------------------------------------

class DemoArea : public MVRenderArea {
protected:
    ibool       inDemo;             // True if currently running demo
    MGLDevCtx   background;         // Background bitmap for window
    int         defPalSize;         // Size of default palette
    palette_t   defPal[256];        // Buffer for default palette

            // Methods to begin and end the demos
            void beginDemo();
            void endDemo();

            // Method to run each of the specific demonstrations
            void displayStatusInfo();
            void lineDemo();
            void lineSpeedDemo();
            void lineNormalDemo();
            void ellipseDemo();
            void arcDemo();
            void polygonDemo();
            void polyDemo();
            void convexPolyDemo();
            void cShadedPolyDemo();
            void rgbShadedPolyDemo();
            void solidRectangleDemo();
            void opaquePattRectangleDemo();
            void transPattRectangleDemo();
            void colorPattRectangleDemo();
            void outlineRectangleDemo();
            void rectangleDemo();
            void fontDemo();
            void colorDemo();
            void patternDemo();
            void boundaryFillDemo();
            void interiorFillDemo();
            void animationDemo();

            // Run the specified demo
            void runDemo(int demo);

            // Method to set the demonstration title name
            void setDemoTitle(char *title);

            // Method to set the information window text
            void setInfoText(char *text);

public:
            // Constructor
            DemoArea(MGLDevCtx& dc,const MVRect& bounds);

            // Method handle interaction with the rendering
    virtual void handleEvent(MVEvent& event,phaseType phase = phFocused);

            // Method to render the scene
    virtual void render();
    };

#endif  // __RENDERA_HPP
