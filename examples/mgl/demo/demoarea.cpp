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
*
* Language:     C++ 3.0
* Environment:  any
*
* Description:  Member functions for the class DemoArea, a class for
*               performing demos in a double buffered rendering area.
*
*               Note that the rendering area does not use the standard
*               MegaVision color model, but uses the native color model of
*               the current MGL video mode (color mapped or RGB).
*
****************************************************************************/

#include "demo.hpp"
#include "demoarea.hpp"
#include <stdio.h>
#include <stdarg.h>

/*----------------------------- Implementation ----------------------------*/

// Define the text displayed in the Explanation window during the demos

char welcomeText[] = "Welcome to the SciTech MGL Graphics Library, a high "
                     "performance graphics library for PC's. This program "
                     "will give you an idea of the kind of primitives that "
                     "the library supports, and the sort of things you can "
                     "do with it.";

char lineText1[] =   "The MGL can draw lines - blindingly fast! "
                     "Click to continue...";
char lineText2[] =   "Of course, you are not restricted to drawing lines "
                     "that are a single pixel wide. You can use an "
                     "arbitrary pen width, and an arbitrary pen pattern "
                     "if you so desire. Click to continue...";

char ellipseText[] = "The MGL can draw ellipses any way you like. They can "
                     "be outlined in an arbitrary rectangular pen, or they "
                     "can of course be filled. Click to continue...";

char arcText[] =     "The MGL can draw elliptical arc's any way you like. "
                     "They can be outlined, or they can of course be filled. "
                     "Click to continue...";

char rectText1[] =   "The MGL can draw solid rectangles very fast. "
                     "Click to continue...";
char rectText2[] =   "The MGL can draw opaque mono pattern filled rectangles "
                     "very fast. "
                     "Click to continue...";
char rectText3[] =   "The MGL can draw transparent mono pattern filled rectangles "
                     "very fast. "
                     "Click to continue...";
char rectText4[] =   "The MGL can draw color pattern filled rectangles "
                     "very fast. "
                     "Click to continue...";
char rectText5[] =   "And of course the MGL can draw outlined rectangles "
                     "any way you like, in any of the previous fill styles. "
                     "Click to continue...";

char polyText1[] =   "The MGL can draw arbitrary polgyons in any pattern "
                     "you like. Click to continue...";
char polyText2[] =   "However the MGL can also render convex polygons "
                     "- blindingly fast! Click to continue...";
char polyText3[] =   "But that is not all, how about some smooth shaded "
                     "polygons? Click to continue...";
char polyText4[] =   "Or some RGB shaded polygons? Click to continue...";

char colorText1[] =  "The MGL works with 24 bit color values internally "
                     "and in color mapped modes like this you can assign "
                     "a color value to any index you like. This mode can "
                     "handle sixteen simulataneous colors at once. ";
char colorText2[] =  "The MGL works with 24 bit color values internally "
                     "and in color mapped modes like this you can assign "
                     "a color value to any index you like. This mode can "
                     "handle 256 simulataneous colors at once. Click to "
                     "continue...";
char colorText3[] =  "The MGL can also do high speed palette rotations. "
                     "Click to continue...";
char colorText4[] =  "Of course you can rotate the other direction also. "
                     "Click to continue...";
char colorText5[] =  "And naturally you can perform the classic palette "
                     "fade in and out!";
char colorText6[] =  "The MGL works with 24 bit color values internally "
                     "and in TrueColor (or direct color) modes like this "
                     "you can specify each color value directly.";

char patText[] =     "The MGL can fill with any arbitrary pattern that "
                     "you can think up. Naturally it comes with a good "
                     "selection of patterns that you can use right away.";

char floodText1[] =  "The MGL can also perform very fast flood fills, both "
                     "boundary fills and interior fills. Click to perform "
                     "a boundary fill...";
char floodText2[] =  "Click to continue...";
char floodText3[] =  "Click to perform an interior fill...";

char animText1[] =   "The MGL can also perform smooth animation using "
                     "double buffering techniques. Click to continue...";
char animText2[] =   "Unfortunately this video mode does not support "
                     "double buffering, so the animation is unavailable.";

#define SWAP(a,b)   { a^=b; b^=a; a^=b; }
#define MAXNUM  1000

color_t         colorMask;
int             backColor = scJade;
int             maxx,maxy,val;
attributes_t    attr;
color_t         maxcolor;
MVPoint         p[MAXNUM+1];

// Array of useful monochrome patterns

#define NUMPATS (sizeof(bitpat) / sizeof(bitpat[0]))

pattern_t bitpat[] = {
    {{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
    {{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}},
    {{0x7F, 0xFF, 0xF7, 0xFF, 0x7F, 0xFF, 0xF7, 0xFF}},
    {{0xDD, 0x77, 0xDD, 0x77, 0xDD, 0x77, 0xDD, 0x77}},
    {{0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55}},
    {{0x55, 0xFF, 0x55, 0xFF, 0x55, 0xFF, 0x55, 0xFF}},
    {{0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA}},
    {{0xEE, 0xDD, 0xBB, 0x77, 0xEE, 0xDD, 0xBB, 0x77}},
    {{0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88}},
    {{0xB1, 0x30, 0x03, 0x1B, 0xD8, 0xC0, 0x0C, 0x8D}},
    {{0x80, 0x10, 0x02, 0x20, 0x01, 0x08, 0x40, 0x04}},
    {{0xFF, 0x88, 0x88, 0x88, 0xFF, 0x88, 0x88, 0x88}},
    {{0xFF, 0x80, 0x80, 0x80, 0xFF, 0x08, 0x08, 0x08}},
    {{0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
    {{0x80, 0x40, 0x20, 0x00, 0x02, 0x04, 0x08, 0x00}},
    {{0x82, 0x44, 0x39, 0x44, 0x82, 0x01, 0x01, 0x01}},
    {{0xF8, 0x74, 0x22, 0x47, 0x8F, 0x17, 0x22, 0x71}},
    {{0x55, 0xA0, 0x40, 0x40, 0x55, 0x0A, 0x04, 0x04}},
    {{0x20, 0x50, 0x88, 0x88, 0x88, 0x88, 0x05, 0x02}},
    {{0xBF, 0x00, 0xBF, 0xBF, 0xB0, 0xB0, 0xB0, 0xB0}},

    {{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
    {{0x80, 0x00, 0x08, 0x00, 0x80, 0x00, 0x08, 0x00}},
    {{0x88, 0x00, 0x22, 0x00, 0x88, 0x00, 0x22, 0x00}},
    {{0x88, 0x22, 0x88, 0x22, 0x88, 0x22, 0x88, 0x22}},
    {{0xAA, 0x00, 0xAA, 0x00, 0xAA, 0x00, 0xAA, 0x00}},
    {{0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00}},
    {{0x11, 0x22, 0x44, 0x88, 0x11, 0x22, 0x44, 0x88}},
    {{0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00}},
    {{0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80}},
    {{0xAA, 0x00, 0x80, 0x00, 0x88, 0x00, 0x80, 0x00}},
    {{0xFF, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80}},
    {{0x08, 0x1C, 0x22, 0xC1, 0x80, 0x01, 0x02, 0x04}},
    {{0x88, 0x14, 0x22, 0x41, 0x88, 0x00, 0xAA, 0x00}},
    {{0x40, 0xA0, 0x00, 0x00, 0x04, 0x0A, 0x00, 0x00}},
    {{0x03, 0x84, 0x48, 0x30, 0x0C, 0x02, 0x01, 0x01}},
    {{0x80, 0x80, 0x41, 0x3E, 0x08, 0x08, 0x14, 0xE3}},
    {{0x10, 0x20, 0x54, 0xAA, 0xFF, 0x02, 0x04, 0x08}},
    {{0x77, 0x89, 0x8F, 0x8F, 0x77, 0x98, 0xF8, 0xF8}},
    {{0x00, 0x08, 0x14, 0x2A, 0x55, 0x2A, 0x14, 0x08}},
    {{0x00, 0x08, 0x14, 0x2A, 0x55, 0x2A, 0x14, 0x08}},

    {{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
    {{0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
    {{0x80, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00}},
    {{0x88, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00}},
    {{0x88, 0x00, 0x00, 0x00, 0x88, 0x00, 0x00, 0x00}},
    {{0x88, 0x00, 0x20, 0x00, 0x88, 0x00, 0x00, 0x00}},
    {{0x88, 0x00, 0x20, 0x00, 0x88, 0x00, 0x02, 0x00}},
    {{0x88, 0x00, 0x22, 0x00, 0x88, 0x00, 0x02, 0x00}},
    {{0x88, 0x00, 0x22, 0x00, 0x88, 0x00, 0x22, 0x00}},
    {{0xA8, 0x00, 0x22, 0x00, 0x88, 0x00, 0x22, 0x00}},
    {{0xA8, 0x00, 0x22, 0x00, 0x8A, 0x00, 0x22, 0x00}},
    {{0xAA, 0x00, 0x22, 0x00, 0x8A, 0x00, 0x22, 0x00}},
    {{0xAA, 0x00, 0x22, 0x00, 0xAA, 0x00, 0x22, 0x00}},
    {{0xAA, 0x00, 0xA2, 0x00, 0xAA, 0x00, 0x22, 0x00}},
    {{0xAA, 0x00, 0xA2, 0x00, 0xAA, 0x00, 0x2A, 0x00}},
    {{0xAA, 0x00, 0xAA, 0x00, 0xAA, 0x00, 0x2A, 0x00}},
    {{0xAA, 0x00, 0xAA, 0x00, 0xAA, 0x00, 0xAA, 0x00}},
    {{0xAA, 0x40, 0xAA, 0x00, 0xAA, 0x00, 0xAA, 0x00}},
    {{0xAA, 0x40, 0xAA, 0x00, 0xAA, 0x04, 0xAA, 0x00}},
    {{0xAA, 0x44, 0xAA, 0x00, 0xAA, 0x04, 0xAA, 0x00}},
    {{0xAA, 0x44, 0xAA, 0x00, 0xAA, 0x44, 0xAA, 0x00}},
    {{0xAA, 0x44, 0xAA, 0x10, 0xAA, 0x44, 0xAA, 0x00}},
    {{0xAA, 0x44, 0xAA, 0x10, 0xAA, 0x44, 0xAA, 0x01}},
    {{0xAA, 0x44, 0xAA, 0x11, 0xAA, 0x44, 0xAA, 0x01}},
    {{0xAA, 0x44, 0xAA, 0x11, 0xAA, 0x44, 0xAA, 0x11}},
    {{0xAA, 0x54, 0xAA, 0x11, 0xAA, 0x44, 0xAA, 0x11}},
    {{0xAA, 0x54, 0xAA, 0x11, 0xAA, 0x45, 0xAA, 0x11}},
    {{0xAA, 0x55, 0xAA, 0x11, 0xAA, 0x45, 0xAA, 0x11}},
    {{0xAA, 0x55, 0xAA, 0x11, 0xAA, 0x55, 0xAA, 0x11}},
    {{0xAA, 0x55, 0xAA, 0x51, 0xAA, 0x55, 0xAA, 0x11}},
    {{0xAA, 0x55, 0xAA, 0x51, 0xAA, 0x55, 0xAA, 0x15}},
    {{0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x15}},
    {{0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55}},
    {{0xEA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55}},
    {{0xEA, 0x55, 0xAA, 0x55, 0xAE, 0x55, 0xAA, 0x55}},
    {{0xEE, 0x55, 0xAA, 0x55, 0xAE, 0x55, 0xAA, 0x55}},
    {{0xEE, 0x55, 0xAA, 0x55, 0xEE, 0x55, 0xAA, 0x55}},
    {{0xEE, 0x55, 0xBA, 0x55, 0xEE, 0x55, 0xAA, 0x55}},
    {{0xEE, 0x55, 0xBA, 0x55, 0xEE, 0x55, 0xAB, 0x55}},
    {{0xEE, 0x55, 0xBB, 0x55, 0xEE, 0x55, 0xAB, 0x55}},
    {{0xEE, 0x55, 0xBB, 0x55, 0xEE, 0x55, 0xBB, 0x55}},
    {{0xFE, 0x55, 0xBB, 0x55, 0xEE, 0x55, 0xBB, 0x55}},
    {{0xFE, 0x55, 0xBB, 0x55, 0xEF, 0x55, 0xBB, 0x55}},
    {{0xFF, 0x55, 0xBB, 0x55, 0xEF, 0x55, 0xBB, 0x55}},
    {{0xFF, 0x55, 0xBB, 0x55, 0xFF, 0x55, 0xBB, 0x55}},
    {{0xFF, 0x55, 0xFB, 0x55, 0xFF, 0x55, 0xBB, 0x55}},
    {{0xFF, 0x55, 0xFB, 0x55, 0xFF, 0x55, 0xBF, 0x55}},
    {{0xFF, 0x55, 0xFF, 0x55, 0xFF, 0x55, 0xBF, 0x55}},
    {{0xFF, 0x55, 0xFF, 0x55, 0xFF, 0x55, 0xFF, 0x55}},
    {{0xFF, 0xD5, 0xFF, 0x55, 0xFF, 0x55, 0xFF, 0x55}},
    {{0xFF, 0xD5, 0xFF, 0x55, 0xFF, 0x5D, 0xFF, 0x55}},
    {{0xFF, 0xDD, 0xFF, 0x55, 0xFF, 0x5D, 0xFF, 0x55}},
    {{0xFF, 0xDD, 0xFF, 0x55, 0xFF, 0xDD, 0xFF, 0x55}},
    {{0xFF, 0xDD, 0xFF, 0x75, 0xFF, 0xDD, 0xFF, 0x55}},
    {{0xFF, 0xDD, 0xFF, 0x75, 0xFF, 0xDD, 0xFF, 0x57}},
    {{0xFF, 0xDD, 0xFF, 0x77, 0xFF, 0xDD, 0xFF, 0x57}},
    {{0xFF, 0xDD, 0xFF, 0x77, 0xFF, 0xDD, 0xFF, 0x77}},
    {{0xFF, 0xFD, 0xFF, 0x77, 0xFF, 0xDD, 0xFF, 0x77}},
    {{0xFF, 0xFD, 0xFF, 0x77, 0xFF, 0xDF, 0xFF, 0x77}},
    {{0xFF, 0xFF, 0xFF, 0x77, 0xFF, 0xDF, 0xFF, 0x77}},
    {{0xFF, 0xFF, 0xFF, 0x77, 0xFF, 0xFF, 0xFF, 0x77}},
    {{0xFF, 0xFF, 0xFF, 0xF7, 0xFF, 0xFF, 0xFF, 0x77}},
    {{0xFF, 0xFF, 0xFF, 0xF7, 0xFF, 0xFF, 0xFF, 0x7F}},
    {{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F}},
    {{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}}
    };

// Array of useful color patterns

#define NUMPIXPATS (sizeof(pixpat) / sizeof(pixpat[0]))

pixpattern_t pixpat[10];

ibool done(void)
/****************************************************************************
*
* Function:     done
* Returns:      True if a mouse click or key was pressed.
*
****************************************************************************/
{
    MVEvent evt;

    if (MV_eventQueue.getNext(evt) && (evt.what & (evKeyDown | evMouseUp)))
        return true;
    return false;
}

void halt(void)
/****************************************************************************
*
* Function:     halt
*
* Description:  Halts until a key is pressed or the mouse is clicked.
*
****************************************************************************/
{
    MV_eventQueue.flush();
    while (!done())
        ;
}

color_t randomColor(void)
/****************************************************************************
*
* Function:     randomColor
*
* Description:  Computes a random color value and returns it. To do this
*               so it works properly in RGB modes with all pixel formats,
*               we need to generate a random 32 bit number and then mask
*               it to the current pixel format. In some systems the alpha
*               channel information in 16 bit and 32 bit pixel modes is
*               active, and hence *must* be set to zero for proper
*               operation (the ATI Mach64 is one such card).
*
****************************************************************************/
{
    return MGL_randoml(0xFFFFFFFFUL) & colorMask;
}

void clearView(MGLDevCtx& dc)
{
    dc.setColor(dc.realColor(backColor));
    dc.fillRect(0,0,dc.maxx()+1,dc.maxy()+1);
}

DemoArea::DemoArea(MGLDevCtx& dc,const MVRect& bounds)
    : MVRenderArea(dc,bounds), background(NULL)
/****************************************************************************
*
* Function:     DemoArea::DemoArea
* Parameters:   bounds      - Bounding box for the rendering area
*
* Description:  Constructor for the DemoArea class.
*
****************************************************************************/
{
    defPalSize = dc.getPaletteSize();
    dc.getPalette(defPal,defPalSize,0);
}

void DemoArea::beginDemo()
/****************************************************************************
*
* Function:     beginDemo
*
* Description:  Begin the next demo.
*
****************************************************************************/
{
    maxx = dc.maxx();
    maxy = dc.maxy();
    maxcolor = dc.maxColor();
    dc.getAttributes(attr);
    dc.setPalette(defPal,defPalSize,0);
    dc.realizePalette(defPalSize,0,false);
    clearView(dc);
    MV_eventQueue.flush();
}

void DemoArea::endDemo()
/****************************************************************************
*
* Function:     endDemo
*
* Description:  Ends the demonstration
*
****************************************************************************/
{
    dc.restoreAttributes(attr);
}

void DemoArea::handleEvent(MVEvent& event,phaseType)
/****************************************************************************
*
* Function:     DemoArea::handleEvent
* Parameters:   event   - Event to handle
*               phase   - Current phase for the event (pre,focus,post)
*
* Description:  Event handling routine for the DemoArea class.
*
****************************************************************************/
{
    if (event.what == evBroadcast) {
        switch (event.message.command) {
            case cmStatusDemo:
            case cmLineDemo:
            case cmEllipseDemo:
            case cmArcDemo:
            case cmPolygonDemo:
            case cmRectangleDemo:
            case cmColorDemo:
            case cmPatternDemo:
            case cmAnimationDemo:
                setupViewport();
                runDemo(event.message.command);
                resetViewport();
                clearEvent(event);
                break;
            }
        }
}

static int gprintf(MGLDevCtx& dc,char *fmt, ... )
/****************************************************************************
*
* Function:     gprintf
* Parameters:   fmt     - Format string
*               ...     - Standard printf style parameters
* Returns:      Number of items converted successfully.
*
* Description:  Simple printf style output routine for sending text to
*               the current viewport. It begins drawing the string at
*               the current CP location, and move the CP to the start of
*               the next logical line.
*
****************************************************************************/
{
    va_list argptr;                 /* Argument list pointer            */
    char    buf[255];               /* Buffer to build sting into       */
    int     cnt;                    /* Result of SPRINTF for return     */
    MVPoint CP;

    va_start(argptr,fmt);

    cnt = vsprintf(buf,fmt,argptr);
    dc.getCP(CP);
    dc.drawStr(buf);                /* Display the string               */
    CP.y += dc.textHeight();        /* Advance to next line             */
    dc.moveTo(CP);

    va_end(argptr);

    return cnt;                     /* Return the conversion count      */
}

void DemoArea::displayStatusInfo()
/****************************************************************************
*
* Function:     DemoArea::displayStatusInfo
*
* Description:  Displays status information about the current video mode.
*               This is used to paint the demonstration window when no
*               demonstration is currently running.
*
****************************************************************************/
{
    setDemoTitle("Status Information");
    setInfoText(welcomeText);

    dc.moveTo(10,10);

    /* Get required information */

    dc.setColor(dc.realColor(scWhite));

    gprintf(dc,"Graphics driver: %s", MGL_modeDriverName(mode));
    gprintf(dc,"Graphics mode: %d x %d x %d", MGL_sizex(dc)+1, MGL_sizey(dc)+1, MGL_getBitsPerPixel(dc));
    gprintf(dc,"Screen resolution: ( 0, 0, %d, %d )",dc.sizex(),dc.sizey());
    gprintf(dc,"Colors available: %ld", dc.maxColor()+1);
    gprintf(dc,"Maximum Page Index: %d", dc.maxPage());

    gprintf(dc,"Current font: %s", dc.getFont()->name);
}

void DemoArea::lineSpeedDemo()
{
    int     i,j;

    beginDemo();

    for (i = 0; i < MAXNUM; i++) {
        p[i].x = MGL_random((ushort)maxx);
        p[i].y = MGL_random((ushort)maxy);
        }

    for (j = 0; j < 10; j++) {
        dc.setColor(dc.realColor(j+1));
        for (i = 0; i < MAXNUM; i++)
            dc.line(p[i],p[i+1]);
        }
    endDemo();
}

void DemoArea::lineNormalDemo()
{
    MVPoint     p1,p2;

    beginDemo();
    while (!done()) {
        p1.x = MGL_random((ushort)maxx);
        p1.y = MGL_random((ushort)maxy);
        p2.x = MGL_random((ushort)maxx);
        p2.y = MGL_random((ushort)maxy);

        dc.setColor(randomColor());
        dc.setBackColor(randomColor());
        dc.setPenSize(MGL_random(5)+1,MGL_random(5)+1);

        if ((val = MGL_random(5)) == 0) {
            dc.setPenStyle(MGL_BITMAP_TRANSPARENT);
            dc.setPenBitmapPattern(0,&bitpat[MGL_random(NUMPATS)+1]);
            dc.usePenBitmapPattern(0);
            }
        else if (val == 1) {
            dc.setPenStyle(MGL_BITMAP_OPAQUE);
            dc.setPenBitmapPattern(0,&bitpat[MGL_random(NUMPATS)+1]);
            dc.usePenBitmapPattern(0);
            }
        else
            dc.setPenStyle(MGL_BITMAP_SOLID);

        dc.line(p1,p2);
        }
    endDemo();
}

void DemoArea::lineDemo()
{
    setDemoTitle("Line Demonstration");
    setInfoText(lineText1);
    halt();
    setInfoText("");
    lineSpeedDemo();
    setInfoText(lineText2);
    lineNormalDemo();
}

void DemoArea::ellipseDemo()
{
    setDemoTitle("Ellipse Demonstration");
    setInfoText(ellipseText);
    beginDemo();

    MVRect  r;

    while (!done()) {
        r.left() = MGL_random((ushort)(maxx-100));
        r.top() = MGL_random((ushort)(maxy-100));
        r.right() = r.left() + MGL_random(100);
        r.bottom() = r.top() + MGL_random(100);

        dc.setColor(randomColor());
        dc.setBackColor(randomColor());

        dc.setPenSize(MGL_random(5)+1,MGL_random(5)+1);

        if ((val = MGL_random(5)) == 1) {
            dc.setPenStyle(MGL_BITMAP_TRANSPARENT);
            dc.setPenBitmapPattern(0,&bitpat[MGL_random(NUMPATS)+1]);
            dc.usePenBitmapPattern(0);
            }
        else if (val == 2) {
            dc.setPenStyle(MGL_BITMAP_OPAQUE);
            dc.setPenBitmapPattern(0,&bitpat[MGL_random(NUMPATS)+1]);
            dc.usePenBitmapPattern(0);
            }
        else
            dc.setPenStyle(MGL_BITMAP_SOLID);

        if (MGL_random(3) == 0)
            dc.fillEllipse(r);
        else
            dc.ellipse(r);
        }

    endDemo();
}

void DemoArea::arcDemo()
{
    setDemoTitle("Elliptical Arc Demonstration");
    setInfoText(arcText);
    beginDemo();

    int     startAngle,endAngle;
    MVRect  r;

    beginDemo();
    while (!done()) {
        r.left() = MGL_random((ushort)(maxx-100));
        r.top() = MGL_random((ushort)(maxy-100));
        r.right() = r.left() + MGL_random(100);
        r.bottom() = r.top() + MGL_random(100);
        startAngle = MGL_random(360);
        endAngle = MGL_random(360);

        dc.setColor(randomColor());
        dc.setBackColor(randomColor());

        dc.setPenSize(MGL_random(5)+1,MGL_random(5)+1);

        if ((val = MGL_random(5)) == 1) {
            dc.setPenStyle(MGL_BITMAP_TRANSPARENT);
            dc.setPenBitmapPattern(0,&bitpat[MGL_random(NUMPATS)+1]);
            dc.usePenBitmapPattern(0);
            }
        else if (val == 2) {
            dc.setPenStyle(MGL_BITMAP_OPAQUE);
            dc.setPenBitmapPattern(0,&bitpat[MGL_random(NUMPATS)+1]);
            dc.usePenBitmapPattern(0);
            }
        else
            dc.setPenStyle(MGL_BITMAP_SOLID);

        if (MGL_random(3) == 0)
            dc.fillEllipseArc(r,startAngle,endAngle);
        else
            dc.ellipseArc(r,startAngle,endAngle);
        }
    endDemo();
}

void DemoArea::solidRectangleDemo()
{
    beginDemo();

    MVRect  r;

    dc.setPenStyle(MGL_BITMAP_SOLID);
    while (!done()) {
        // Find random rectangle dimensions
        r.left() = MGL_random((ushort)maxx);
        r.right() = MGL_random((ushort)maxx);
        r.top() = MGL_random((ushort)maxy);
        r.bottom() = MGL_random((ushort)maxy);

        // Fix the rectangle so it is not empty
        if (r.right() < r.left())
            SWAP(r.left(),r.right());
        if (r.bottom() < r.top())
            SWAP(r.top(),r.bottom());

        // Draw the rectangle
        dc.setColor(randomColor());
        dc.fillRect(r);
        }

    endDemo();
}

void DemoArea::opaquePattRectangleDemo()
{
    beginDemo();

    MVRect  r;

    dc.setPenStyle(MGL_BITMAP_OPAQUE);
    while (!done()) {
        // Find random rectangle dimensions
        r.left() = MGL_random((ushort)maxx);
        r.right() = MGL_random((ushort)maxx);
        r.top() = MGL_random((ushort)maxy);
        r.bottom() = MGL_random((ushort)maxy);

        // Fix the rectangle so it is not empty
        if (r.right() < r.left())
            SWAP(r.left(),r.right());
        if (r.bottom() < r.top())
            SWAP(r.top(),r.bottom());

        dc.setColor(randomColor());
        dc.setBackColor(randomColor());
        dc.setPenBitmapPattern(0,&bitpat[MGL_random(NUMPATS)+1]);
        dc.usePenBitmapPattern(0);
        dc.fillRect(r);
        }

    endDemo();
}

void DemoArea::transPattRectangleDemo()
{
    beginDemo();

    MVRect  r;

    dc.setPenStyle(MGL_BITMAP_TRANSPARENT);
    while (!done()) {
        // Find random rectangle dimensions
        r.left() = MGL_random((ushort)maxx);
        r.right() = MGL_random((ushort)maxx);
        r.top() = MGL_random((ushort)maxy);
        r.bottom() = MGL_random((ushort)maxy);

        // Fix the rectangle so it is not empty
        if (r.right() < r.left())
            SWAP(r.left(),r.right());
        if (r.bottom() < r.top())
            SWAP(r.top(),r.bottom());

        dc.setColor(randomColor());
        dc.setPenBitmapPattern(0,&bitpat[MGL_random(NUMPATS)+1]);
        dc.usePenBitmapPattern(0);
        dc.fillRect(r);
        }

    endDemo();
}

void DemoArea::colorPattRectangleDemo()
{
    beginDemo();

    MVRect  r;

    // Build our array of color patterns from the first entries in the
    // mono pattern array with random foreground and background colors.
    // This is a quick and dirty way to generate some color patterns that
    // look reasonable (we could also do some dithered patterns or
    // something to show the fact you can have any colors you like in them).
    for (uint i = 0; i < NUMPIXPATS; i++) {
        color_t foreColor = randomColor();
        color_t backColor = randomColor();
        uchar   *patt = (uchar*)&bitpat[i];
        color_t *p = (color_t*)&pixpat[i];
        for (int y = 0; y < 8; y++) {
            *p++ = (*patt & 0x01) ? foreColor : backColor;
            *p++ = (*patt & 0x02) ? foreColor : backColor;
            *p++ = (*patt & 0x04) ? foreColor : backColor;
            *p++ = (*patt & 0x08) ? foreColor : backColor;
            *p++ = (*patt & 0x10) ? foreColor : backColor;
            *p++ = (*patt & 0x20) ? foreColor : backColor;
            *p++ = (*patt & 0x40) ? foreColor : backColor;
            *p++ = (*patt & 0x80) ? foreColor : backColor;
            patt++;
            }
        }

    // Now display the rectangles
    dc.setPenStyle(MGL_PIXMAP);
    while (!done()) {
        // Find random rectangle dimensions
        r.left() = MGL_random((ushort)maxx);
        r.right() = MGL_random((ushort)maxx);
        r.top() = MGL_random((ushort)maxy);
        r.bottom() = MGL_random((ushort)maxy);

        // Fix the rectangle so it is not empty
        if (r.right() < r.left())
            SWAP(r.left(),r.right());
        if (r.bottom() < r.top())
            SWAP(r.top(),r.bottom());

        dc.setPenPixmapPattern(0,&pixpat[MGL_random(NUMPIXPATS)+1]);
        dc.usePenPixmapPattern(0);
        dc.fillRect(r);
        }

    endDemo();
}

void DemoArea::outlineRectangleDemo()
{
    beginDemo();

    MVRect  r;

    dc.setPenStyle(MGL_PIXMAP);
    while (!done()) {
        // Find random rectangle dimensions
        r.left() = MGL_random((ushort)maxx);
        r.right() = MGL_random((ushort)maxx);
        r.top() = MGL_random((ushort)maxy);
        r.bottom() = MGL_random((ushort)maxy);

        // Fix the rectangle so it is not empty
        if (r.right() < r.left())
            SWAP(r.left(),r.right());
        if (r.bottom() < r.top())
            SWAP(r.top(),r.bottom());

        dc.setColor(randomColor());
        dc.setBackColor(randomColor());
        dc.setPenSize(MGL_random(5)+1,MGL_random(5)+1);
        if ((val = MGL_random(5)) == 1) {
            dc.setPenStyle(MGL_BITMAP_TRANSPARENT);
            dc.setPenBitmapPattern(0,&bitpat[MGL_random(NUMPATS)+1]);
            dc.usePenBitmapPattern(0);
            }
        else if (val == 2) {
            dc.setPenStyle(MGL_BITMAP_OPAQUE);
            dc.setPenBitmapPattern(0,&bitpat[MGL_random(NUMPATS)+1]);
            dc.usePenBitmapPattern(0);
            }
        else
            dc.setPenStyle(MGL_BITMAP_SOLID);
        dc.rect(r);
        }

    endDemo();
}

void DemoArea::rectangleDemo()
{
    setDemoTitle("Rectangle Demonstration");
    setInfoText(rectText1);
    solidRectangleDemo();
    setInfoText(rectText2);
    opaquePattRectangleDemo();
    setInfoText(rectText3);
    transPattRectangleDemo();
    setInfoText(rectText4);
    colorPattRectangleDemo();
    setInfoText(rectText5);
    outlineRectangleDemo();
}

#define MaxPts      6               // Maximum # of pts in polygon

void DemoArea::polyDemo()
{
    int         i;
    fxpoint_t   poly[MaxPts];       // Space to hold polygon data

    beginDemo();
    while (!done()) {
        for (i = 0; i < MaxPts; i++) {
            poly[i].x = MGL_randoml(MGL_TOFIX(maxx));
            poly[i].y = MGL_randoml(MGL_TOFIX(maxy));
            }

        dc.setColor(randomColor());
        dc.setBackColor(randomColor());

        if ((val = MGL_random(3)) == 0) {
            dc.setPenStyle(MGL_BITMAP_TRANSPARENT);
            dc.setPenBitmapPattern(0,&bitpat[MGL_random(NUMPATS)+1]);
            dc.usePenBitmapPattern(0);
            }
        else if (val == 1) {
            dc.setPenStyle(MGL_BITMAP_OPAQUE);
            dc.setPenBitmapPattern(0,&bitpat[MGL_random(NUMPATS)+1]);
            dc.usePenBitmapPattern(0);
            }
        else
            dc.setPenStyle(MGL_BITMAP_SOLID);

        dc.fillPolygon(MaxPts,poly,sizeof(fxpoint_t),0,0);
        }

    endDemo();
}

void DemoArea::convexPolyDemo()
{
    fxpoint_t   poly[3];

    beginDemo();
    dc.setPolygonType(MGL_CONVEX_POLYGON);
    while (!done()) {
        poly[0].x = MGL_randoml(MGL_TOFIX(maxx));
        poly[0].y = MGL_randoml(MGL_TOFIX(maxy));
        poly[1].x = MGL_randoml(MGL_TOFIX(maxx));
        poly[1].y = MGL_randoml(MGL_TOFIX(maxy));
        poly[2].x = MGL_randoml(MGL_TOFIX(maxx));
        poly[2].y = MGL_randoml(MGL_TOFIX(maxy));

        dc.setColor(randomColor());
        dc.fillPolygon(3,poly,sizeof(fxpoint_t),0,0);
        }
    endDemo();
}

void DemoArea::polygonDemo()
{
    setDemoTitle("Polygon Demonstration");
    setInfoText(polyText1);
    polyDemo();
    setInfoText(polyText2);
    convexPolyDemo();
}

void DemoArea::colorDemo()
{
    int         color,width,height,x,y,i,j,top,bottom,start,palsize;
    palette_t   pal[256];

    setDemoTitle("Color Demonstration");

    beginDemo();
    if (maxcolor <= 15) {
        // Simple color demonstration for 16 color displays

        setInfoText(colorText1);
        width = 2 * ((dc.maxx()+1) / 16);
        height = 2 * ((dc.maxy()-10)  / 10);

        x = width / 2;
        y = height / 2;
        color = 1;
        for (j = 0; j < 3; j++) {
            for (i = 0; i < 5; i++) {
                dc.setColor(color++);
                dc.fillRect(x,y,x+width,y+height);
                x += (width/2) * 3;
                }
            y += (height / 2) * 3;
            x = width / 2;
            }
        }
    else if (maxcolor == 255) {
        // Color demonstration for color mapped displays with 255 colors

        setInfoText(colorText2);
        width = 2 * ((dc.maxx()+1) / 47);
        height = 2 * ((dc.maxy()-10)  / 48);
        palsize = maxcolor - scUser + 1;

        x = width / 2;
        y = height / 2;
        color = scUser;
        for (j = 0; j < (int)(maxcolor-scUser+1); j++) {
            dc.setColor(color++);
            dc.fillRect(x,y,x+width,y+height);
            x += (width/2) * 3;
            if (((j+1) % 16) == 0) {
                y += (height / 2) * 3;
                x = width / 2;
                }
            }

        halt();
        setInfoText(colorText3);

        // Palette rotations

        while (!done()) {
            dc.rotatePalette(palsize,scUser,MGL_ROTATE_UP);
            dc.realizePalette(palsize,scUser,true);
            }
        setInfoText(colorText4);
        while (!done()) {
            dc.rotatePalette(palsize,scUser,MGL_ROTATE_DOWN);
            dc.realizePalette(palsize,scUser,true);
            }
        setInfoText(colorText5);

        // Palette fade out

        dc.getPalette(pal,dc.getPaletteSize(),0);
        for (i = 63; i >= 0; i--) {
            dc.fadePalette(pal+1,dc.getPaletteSize()-1,1,(uchar)(i*4));
            dc.realizePalette(dc.getPaletteSize()-1,1,true);
            }

        // Palette fade in

        for (i = 0; i <= 63; i++) {
            dc.fadePalette(pal+1,dc.getPaletteSize()-1,1,(uchar)(i*4));
            dc.realizePalette(dc.getPaletteSize()-1,1,true);
            }
        }
    else {
        // Color demonstration for HiColor and TrueColor modes

        setInfoText(colorText6);

        start = dc.maxx()/8;
        width = dc.maxx() - start*2;

        top = dc.maxy()/8;
        bottom = dc.maxy()/8 + dc.maxy()/5;
        for (x = 0; x < width; x++) {
            dc.setColor(dc.packColor((uchar)((x * 255L) / width),0,0));
            dc.line(start+x,top,start+x,bottom);
            }

        top += dc.maxy()/5;
        bottom += dc.maxy()/5;
        for (x = 0; x < width; x++) {
            dc.setColor(dc.packColor(0,(uchar)((x * 255L) / width),0));
            dc.line(start+x,top,start+x,bottom);
            }

        top += dc.maxy()/5;
        bottom += dc.maxy()/5;
        for (x = 0; x < width; x++) {
            dc.setColor(dc.packColor(0,0,(uchar)((x * 255L) / width)));
            dc.line(start+x,top,start+x,bottom);
            }

        top += dc.maxy()/5;
        bottom += dc.maxy()/5;
        for (x = 0; x < width/2; x++) {
            dc.setColor(dc.packColor((uchar)((((width/2)-x) * 255L) / (width/2)),
                (uchar)((x * 255L) / (width/2)),0));
            dc.line(start+x,top,start+x,bottom);
            }
        for (; x < width; x++) {
            dc.setColor(dc.packColor(0,
                (uchar)(( ((width/2) - (x - (width/2))) * 255L) / (width/2)),
                (uchar)(((x-width/2) * 255L) / (width/2) )));
            dc.line(start+x,top,start+x,bottom);
            }
        }
    endDemo();
}

#define POINTS  10

int     data1[POINTS] = { 1, 3, 5, 4, 3, 2, 1, 5, 4, 2 };
int     data2[POINTS] = { 4, 6, 10, 2, 6, 4, 8, 10, 6, 2 };
int     data3[POINTS] = { 1, 3, 2, 5, 7, 9, 5, 4, 5, 8 };

void DemoArea::patternDemo()
{
    int     i,j,x,y,height,width,space;

    setDemoTitle("Pattern Demonstration");
    setInfoText(patText);

    beginDemo();
    width  = (maxx+1) / 20;
    height = (maxy+1) / 9;
    if (dc.sizey() < 479)
        space = 3;
    else space = 10;

    x = y = space;
    dc.setColor(dc.realColor(scWhite));
    dc.setPenStyle(MGL_BITMAP_TRANSPARENT);

    for (j = 0; j < 7; j++) {           /* For 7 rows of boxes      */
        for (i = 0; i < 15; i++) {      /* For 15 columns of boxes      */
            dc.setPenBitmapPattern(0,&bitpat[j*15 + i]);
            dc.usePenBitmapPattern(0);
            dc.fillRect(x,y,x+width+1,y+height+1);
            x += width + space;         /* Advance to next col          */
            }
                 x = space;
        y += height + space;
        }
    endDemo();
}

int fixAngle(int angle)
{
    while (angle < 0)
        angle += 360;
    while (angle >= 360)
        angle -= 360;
    return angle;
}

void DemoArea::animationDemo()
{
    int     stepx,stepy,secAngle,minAngle;
    MVRect  extent,dirtyRect;

    setDemoTitle("Animation Demonstration");
    if (dc.maxPage() > 0) {
        setInfoText(animText1);
        beginDemo();
        doubleBufferOn();

        extent.left() = extent.top() = 0;
        extent.right() = maxx/5;
        extent.bottom() = ((long)extent.right() * 1000) / dc.getAspectRatio();
        dirtyRect = extent;

        stepx = 1;
        stepy = 1;
        secAngle = minAngle = 90;

        while (!done()) {
            dc.setColor(dc.realColor(backColor));
            dc.fillRect(dirtyRect);
            dc.setColor(dc.realColor(MGL_LIGHTRED));
            dc.fillEllipse(extent);
            dc.setColor(dc.realColor(scWhite));
            dc.fillEllipseArc(extent,secAngle-5,secAngle);
            dc.fillEllipseArc(extent,minAngle-5,minAngle);

            swapBuffers();          // Swap the display buffers

            // Bounce the clock off the walls

            dirtyRect = extent;
            dirtyRect.inset(-ABS(stepx),-ABS(stepy));
            if (extent.left() + stepx < 0)
                stepx = -stepx;
            if (extent.right() + stepx > maxx)
                stepx = -stepx;

            if (extent.top() + stepy < 0)
                stepy = -stepy;
            if (extent.bottom() + stepy > maxy)
                stepy = -stepy;

            extent.offset(stepx,stepy);

            // Update the hand movement

            secAngle = fixAngle(secAngle - 5);
            if (secAngle == 90)
                minAngle = fixAngle(minAngle - 5);
            }

        doubleBufferOff();
        endDemo();
        }
    else setInfoText(animText2);
}

void DemoArea::setDemoTitle(char *title)
/****************************************************************************
*
* Function:     DemoArea::setDemoTitle
* Parameters:   title   - New title for the demonstration
*
* Description:  Sets the demonstration title information, and forces a
*               repaint event.
*
****************************************************************************/
{
    MVEvent e;

    MV_message(MVProgram::deskTop,evBroadcast,cmSetDemoTitle,title);
    getEvent(e,evRepaint);
}

void DemoArea::setInfoText(char *text)
/****************************************************************************
*
* Function:     DemoArea::setInfoText
* Parameters:   text    - Text for the new window.
*
* Description:  Sets the explanation information in the information
*               window.
*
****************************************************************************/
{
    MVEvent e;

    MV_message(MVProgram::deskTop,evBroadcast,cmSetExplanation,text);
    getEvent(e,evRepaint);
}

void DemoArea::runDemo(int demo)
/****************************************************************************
*
* Function:     DemoArea::runDemo
*
* Description:  Runs the specified demo, and saves the final state of the
*               demo area to a system memory device context.
*
****************************************************************************/
{
    MS_obscure();
    dc.setColor(dc.realColor(backColor));
    dc.fillRect(0,0,size.x+1,size.y+1);

    if (dc.getBitsPerPixel() <= 8)
        colorMask = dc.maxColor();
    else colorMask = dc.packColor(0xFF,0xFF,0xFF);

    // Allocate memory to store background bitmap
    if (!background.isValid()) {
        pixel_format_t pf;
        int bits = dc.getBitsPerPixel();
        dc.getPixelFormat(pf);
        background.setDC(MGL_createMemoryDC(size.x+1,size.y+1,bits,&pf));
        }

    // Run the demo
    inDemo = true;
    switch (demo) {
        case cmLineDemo:
            lineDemo();
            break;
        case cmEllipseDemo:
            ellipseDemo();
            break;
        case cmArcDemo:
            arcDemo();
            break;
        case cmPolygonDemo:
            polygonDemo();
            break;
        case cmRectangleDemo:
            rectangleDemo();
            break;
        case cmColorDemo:
            colorDemo();
            break;
        case cmPatternDemo:
            patternDemo();
            break;
        case cmAnimationDemo:
            animationDemo();
            break;
        case cmStatusDemo:
            displayStatusInfo();
            break;
        }
    inDemo = false;

    // Save background bitmap so we can restore it quickly. Note that we
    // do not want palette translations so we disable them and re-enable
    // them when we are done.
    if (background.isValid()) {
        ibool oldIdPal = MGL_checkIdentityPalette(false);
        background.bitBlt(dc,0,0,size.x+1,size.y+1,0,0,MGL_REPLACE_MODE);
        MGL_checkIdentityPalette(oldIdPal);
        }
    MS_show();
}

void DemoArea::render()
/****************************************************************************
*
* Function:     DemoArea::render
*
* Description:  Renders the data in the demo area. All we really do in
*               here is simply draw the saved bitmap for the background
*               if we have enough memory, or just a solid rectangle if
*               we dont.
*
****************************************************************************/
{
    if (!inDemo && background.isValid()) {
        ibool oldIdPal = MGL_checkIdentityPalette(false);
        dc.bitBlt(background,0,0,size.x+1,size.y+1,0,0,MGL_REPLACE_MODE);
        MGL_checkIdentityPalette(oldIdPal);
        }
    else {
        dc.setColor(dc.realColor(backColor));
        dc.fillRect(0,0,size.x+1,size.y+1);
        }
}
