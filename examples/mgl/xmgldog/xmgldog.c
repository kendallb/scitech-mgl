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
* Language:     ANSI C
* Environment:  Unix / X11
*
* Description:  X11 version of MGLDOG sample app that runs in a window
*               on the X display. This shows how you can build a
*               normal X-Window application and use the MGL for high
*               performance rendering to a window.
*
*
****************************************************************************/

#define STRICT
#include <stdio.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/xpm.h>

#include "mgraph.h"
#include "mgldog.xpm"

/*--------------------------- Global Variables ----------------------------*/

char        szAppName[] = "MGLDOG: MGL Sprite Demo";
Window      wnd;
color_t     transparent;
int         sizex,sizey,width,height;
MGLDC       *winDC = NULL,*dogDC = NULL,*memDC = NULL;
Display     *dpy;
long scr;

/*------------------------------ Implementation ---------------------------*/

void GetMemoryBitmapDepth(MGLDC *dc,int *bits,pixel_format_t *pf)
/****************************************************************************
*
* Function:     GetMemoryBitmapDepth
* Parameters:   dc      - Window DC
*               bits    - Place to store pixel depth
*               pf      - Place to store pixel format
* Returns:      Pointer to valid memory DC with bitmap loaded
*
* Description:  Under X11 we can blt bitmaps of any color depth, so we create
*               our memory DC with the same depth and format as the current
*               X11 display mode.
*
****************************************************************************/
{
    *bits = MGL_getBitsPerPixel(dc);
    MGL_getPixelFormat(dc,pf);
}

MGLDC *LoadBitmapIntoMemDC(MGLDC *dc,char *bitmapName)
/****************************************************************************
*
* Function:     LoadBitmapIntoMemDC
* Parameters:   dc          - Display dc
*               bitmapName  - Name of bitmap file to load
* Returns:      Pointer to valid memory DC with bitmap loaded
*
* Description:  Loads the specified bitmap into a memory DC with the
*               same dimensions as the bitmap on disk, but with the same
*               pixel depth and format used by the display DC (for maximum
*               blt performance). The MGL automatically handles pixel
*               format conversion for us when we load the bitmap into our
*               memory DC, and the bitmap's palette is loaded into the
*               memory DC's palette.
*
****************************************************************************/
{
    int             width,height,bits;
    pixel_format_t  pf;
    MGLDC           *memDC;

    /* Get dimensions of bitmap image on disk */
    if (!MGL_getBitmapSize(bitmapName,&width,&height,&bits,&pf))
        MGL_fatalError("Cant locate bitmap file!");

    /* Get bitmap depth and pixel format for memory DC */
    GetMemoryBitmapDepth(dc,&bits,&pf);

    /* Create the memory DC and load the bitmap file into it */
    if ((memDC = MGL_createMemoryDC(width,height,bits,&pf)) == NULL)
        MGL_fatalError("Not enough memory to load bitmap!");
    if (!MGL_loadBitmapIntoDC(memDC,bitmapName,0,0,true))
        MGL_fatalError("Cant locate bitmap file!");
    return memDC;
}

void InitMGL(void)
/****************************************************************************
*
* Function:     InitMGL
*
* Description:  Initialise the MGL and load all bitmap resources required by
*               the application. This is where the application should allocate
*               space for all MGL memory DC's used for storing sprites etc
*               and load the sprites from disk.
*
****************************************************************************/
{
    MGLDC           *winDC;
    int             bits;
    pixel_format_t  pf;

    /* Register the MGL packed pixel drivers we want to use. For this demo
     * we need all color depths >= 8 bits per pixel, since we will be
     * automatically converting our sprite and offscreen bitmap buffer to
     * the same pixel format as the X11 video mode.
     */
    MGL_registerDriver(MGL_PACKED8NAME,PACKED8_driver);
    MGL_registerDriver(MGL_PACKED16NAME,PACKED16_driver);
    MGL_registerDriver(MGL_PACKED24NAME,PACKED24_driver);
    MGL_registerDriver(MGL_PACKED32NAME,PACKED32_driver);

    MGL_registerDriver(MGL_XWINDCNAME,XWINDC_driver);

    /* Initialise the MGL for Windowed output only */
    MGL_initWindowed("../../../");

    /* Create an MGL Windowed DC for the Windows Desktop to obtain pixel depth
     * and pixel format of the Windows display driver video mode. We can then
     * check that we are in the correct video mode and also load the bitmaps
     * in the appropriate format.
     */
    MGL_setX11Display(dpy);
    if ((winDC = MGL_createWindowedDC(wnd)) == NULL)
        MGL_fatalError("Unable to create Windowed DC!");
    if ((bits = MGL_getBitsPerPixel(winDC)) < 8)
        MGL_fatalError("This program requires 256 or more colors!");
    MGL_getPixelFormat(winDC,&pf);

    /* Load the sprite bitmap into the dogDC */
    dogDC = LoadBitmapIntoMemDC(winDC,"doggie2.bmp");

    /* Get transparent color from pixel (0,0) and dimensions of sprite */
    MGL_makeCurrentDC(dogDC);
    transparent = MGL_getPixelCoord(0,0);
    width = MGL_sizex(dogDC)+1;
    height = MGL_sizey(dogDC)+1;
    MGL_makeCurrentDC(NULL);

    /* Destroy the temporary DC */
    MGL_destroyDC(winDC);
}

void CreateMGLDeviceContexts(Window wnd)
/****************************************************************************
*
* Function:     CreateMGLDeviceContexts
* Parameters:   hwnd    - Handle to Window used for output
*
* Description:  Sets up the MGL for output to the client region of the
*               specified window. This function will destroy any previously
*               existing MGL device contexts, and will re-build all the
*               DC's for the Window. Hence this routine is always called
*               from the Windows WM_SIZE message handler, which will always
*               be called before the first WM_PAINT message is received,
*               allowing the MGL device contexts to be created with the
*               appropriate size information.
*
****************************************************************************/
{
    int             bits;
    pixel_format_t  pf;
    palette_t       pal[256];

    /* Destroy the previous DC's if they existed */
    if (winDC)
        MGL_destroyDC(winDC);
    if (memDC)
        MGL_destroyDC(memDC);

    MGL_setX11Display(dpy);
    /* Create the main MGL Windowed DC for interfacing our main window */
    if ((winDC = MGL_createWindowedDC(wnd)) == NULL)
        MGL_fatalError("Unable to create Windowed DC!");
    GetMemoryBitmapDepth(winDC,&bits,&pf);
    sizex = MGL_sizex(winDC)+1;
    sizey = MGL_sizey(winDC)+1;

    /* Create the main backbuffer DC with same pixel format as Window DC */
    if ((memDC = MGL_createMemoryDC(sizex,sizey,bits,&pf)) == NULL)
        MGL_fatalError("Unable to create Memory DC!");

    /* Copy the palette from the sprite and store in the offscreen bitmap
     * and the Window DC. The MGL automatically takes care of creating
     * identity palettes when we realize the palette for the memory DC and
     * the Window DC, but we must make sure that both of them get realized.
     */
    MGL_getPalette(dogDC,pal,MGL_getPaletteSize(dogDC),0);
    MGL_setPalette(memDC,pal,MGL_getPaletteSize(memDC),0);
    MGL_realizePalette(memDC,MGL_getPaletteSize(memDC),0,false);
    MGL_setPalette(winDC,pal,MGL_getPaletteSize(winDC),0);
    MGL_realizePalette(winDC,MGL_getPaletteSize(winDC),0,false);

    /* Clear the backbuffer and draw the dog in the middle */
    MGL_makeCurrentDC(memDC);
    MGL_clearDevice();
    MGL_transBltCoord(memDC,dogDC,0,0,width,height,(sizex - width)/2,
        (sizey - height)/2,transparent,true);
    MGL_makeCurrentDC(NULL);
}

void ProcessEvent(Window wnd, XEvent *msg)
/****************************************************************************
*
* Function:     ProcessEvent
*
* Description:  Processes a X11 event
*
****************************************************************************/
{
    static ibool   leftDown, rightDown, flag = false;
    int         x,y;

    printf("Event: %d\n", msg->type);
    switch (msg->type) {
    case ConfigureNotify:
        printf("Configure\n");
        CreateMGLDeviceContexts(wnd);
        break;
    case ButtonPress:
        switch(msg->xbutton.button){
        case 1:
            leftDown = true;
            break;
        case 3:
            rightDown = true;
            break;
        }
        x = msg->xbutton.x;
        y = msg->xbutton.y;
        flag = true;
        break;
    case ButtonRelease:
        switch(msg->xbutton.button){
        case 1:
            leftDown = false;
            break;
        case 3:
            rightDown = false;
            break;
        }
        x = msg->xbutton.x;
        y = msg->xbutton.y;
        flag = true;
        break;
    case MotionNotify:
        x = msg->xmotion.x;
        y = msg->xmotion.y;
        flag = true;
        break;
    case DestroyNotify:
        exit(0);
        break;
    case Expose:
        printf("Expose\n");
        MGL_bitBltCoord(winDC,memDC,0,0,sizex,sizey,0,0,MGL_REPLACE_MODE);
        break;
    }
    if(flag){
        x -= width>>1;
        y -= height>>1;
        printf("(%d,%d)\n", x,y);
        if (!leftDown && !rightDown)
            return;
        if (leftDown)
            MGL_transBltCoord(memDC,dogDC,0,0,width,height,x,y,transparent,true);
        else if (rightDown)
            MGL_bitBltCoord(memDC,dogDC,0,0,width,height,x,y,MGL_REPLACE_MODE);
        // MGL_bitBltCoord(winDC,memDC,x,y,x+width,y+height,x,y,MGL_REPLACE_MODE);
        MGL_bitBltCoord(winDC,memDC,0,0,sizex,sizey,0,0,MGL_REPLACE_MODE);
    }
}

int main(int argc, char **argv)
{
    XEvent   msg;
    XSetWindowAttributes xswa;
    XWMHints wmhints;
    Pixmap icon;
    const long evt_mask = ExposureMask | ButtonPressMask | ButtonReleaseMask | StructureNotifyMask | PointerMotionMask;

    dpy=XOpenDisplay(NULL);
    if (!dpy) {
        fprintf(stderr, "Could not open X11 display.\n");
        return 1;
        }

    scr = DefaultScreen(dpy);

    /* Create the main window and display it */
    wnd = XCreateSimpleWindow(dpy, RootWindow(dpy,scr), 0,0,400,400,0,
                              WhitePixel(dpy,scr), BlackPixel(dpy,scr));
    xswa.backing_store = Always;
    //  XChangeWindowAttributes(dpy, wnd, CWBackingStore, &xswa);

    if(!wnd){
        fprintf(stderr, "Window creation failed !\n");
        return 1;
        }

    XpmCreatePixmapFromData(dpy, wnd, mgldog_xpm, &icon, NULL, NULL);
    wmhints.icon_pixmap = icon;
    wmhints.flags = IconPixmapHint;
    XSetWMHints(dpy, wnd, &wmhints);

    /* Initialise the MGL */
    InitMGL();

    XSelectInput(dpy, wnd, evt_mask);
    XMapRaised(dpy,wnd);
    XStoreName(dpy,wnd,szAppName);
    XSync(dpy, false);


    MGL_bitBltCoord(winDC,memDC,0,0,sizex,sizey,0,0,MGL_REPLACE_MODE);
    /* Polling messages from event queue */
    for(;;) {
        XNextEvent(dpy, &msg);
        // XWindowEvent(dpy, wnd, evt_mask, &msg);
        ProcessEvent(wnd, &msg);
        }

    /* Clean up the MGL before exit */
    MGL_exit();
    XCloseDisplay(dpy);
    return 0;
}
