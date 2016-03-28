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
* Environment:  Unix/X11
*
* Description:  Main 8 bit X11 device driver file. This file
*               will be linked in when a call to MGL_registerDriver() is
*               made to register this device driver for use.
*
****************************************************************************/

#include "mgldd.h"
#include "drivers/unix/xwin8.h"
#include <stdio.h>

/*---------------------------- Driver Tables -----------------------------*/

drivertype  _VARAPI XWIN8_driver = {
    IDENT_FLAG,
    MGL_XWIN8NAME,
    MGL_XWIN8NAME,
    "\r\n\r\nVersion " DRIVER_VERSION_STR " - " __DATE__ "\r\n"
    "Copyright (C) 1998 SciTech Software\r\n",
    DRIVER_MAJOR_VERSION,
    DRIVER_MINOR_VERSION,
    grXWIN,
    XWIN8_createInstance,
    XWIN8_detect,
    XWIN8_initDriver,
    XWIN8_destroyInstance,
};

PRIVATE drv_vec g_state;    /* Global state buffer pointer  */

/* Resolutions supported by this driver. Could be anything in fact for a X11 window... */
PRIVATE gmode_t XWIN8_modes[] = {
    {639,479,8,1,0xFF,0,0,1000,0,0},
    {799,599,8,1,0xFF,0,0,1000,0,0},
    {1023,767,8,1,0xFF,0,0,1000,0,0},
    {1151,863,8,1,0xFF,0,0,1000,0,0},
    {1279,959,8,1,0xFF,0,0,1000,0,0},
    {1279,1023,8,1,0xFF,0,0,1000,0,0},
    {1599,1199,8,1,0xFF,0,0,1000,0,0},
};


/*------------------------------ Implementation ------------------------------*/

ibool MGLAPI XWIN8_detect(void *data,int id,int force,int *driver,int *mode,modetab modes)
/****************************************************************************
*
* Function:     XWIN8_detect
* Parameters:   id      - Id of this driver for storing in mode table
*               force   - Highest performance driver to be used
*               driver  - Place to store detected driver id
*               mode    - Place to store recommended video mode
*               modes   - Place to store list of supported modes
* Returns:      TRUE if the device was found, FALSE if not.
*
* Description:  Determines if an X11 display is present, and
*               if so fills in the mode table with all of the modes
*               supported by this device driver.
*
*               If the value of 'driver' is less than the value of our
*               device driver number, then we return false since the user
*               has requested to ignore our devices modes.
*
****************************************************************************/
{
    if(force >= grXWIN && XWIN_detect() ) {
        XWIN_useMode(modes, grSVGA_640x480x256,  id,&XWIN8_modes[0]);
        XWIN_useMode(modes, grSVGA_800x600x256,  id,&XWIN8_modes[1]);
        XWIN_useMode(modes, grSVGA_1024x768x256, id,&XWIN8_modes[2]);
        XWIN_useMode(modes, grSVGA_1152x864x256, id,&XWIN8_modes[3]);
        XWIN_useMode(modes, grSVGA_1280x960x256, id,&XWIN8_modes[4]);
        XWIN_useMode(modes, grSVGA_1280x1024x256,id,&XWIN8_modes[5]);
        XWIN_useMode(modes, grSVGA_1600x1200x256,id,&XWIN8_modes[6]);
        return true;
    }
    return false;
}


void * MGLAPI  XWIN8_createInstance(void)
{
    return PM_malloc(sizeof(XWIN8_data));
}


void MGLAPI XWIN8_destroyInstance(void *data)
/****************************************************************************
*
* Function:     XWIN8_destroyInstance
*
* Description:  Frees the internal data structures allocated during the
*               detection process
*
****************************************************************************/
{
    if(data){
        //      XDestroyWindow(dpy, ((XWIN8_data*)data)->w);
        PM_free(data);
    }
}


ibool   MGLAPI XWIN8_initDriver(void *data,MGLDC *dc,int driverId,int modeId,ulong hwnd,
                                 int virtualX,int virtualY,int numBuffers,ibool stereo,int refreshRate)
/****************************************************************************
*
* Function:     WDCI8_initDriver
* Parameters:   dc  - Device context.
* Returns:      True if the device was correctly initialised.
*
* Description:  Initialises the device driver, and starts the specified
*               graphics mode. This is also where we fill in all of the
*               vectors in the device context to initialise our device
*               context properly.
*
****************************************************************************/
{
    Screen *scr;
    Window wnd;
    char *buf;
    XSetWindowAttributes xswa;

    dc->mi = XWIN8_modes[modeId - grSVGA_640x480x256];
    dc->deviceType = MGL_WINDOWED_DEVICE;

    g_state.d.hardwareCursor = false;

    dc->v = &g_state;
    dc->wm.xwindc.dpy = globalDisplay;
    dc->wm.xwindc.scr = scr = DefaultScreenOfDisplay(globalDisplay);
    dc->wm.xwindc.gc  = DefaultGCOfScreen(scr);

    xswa.background_pixel = BlackPixel(globalDisplay,XScreenNumberOfScreen(scr));
    xswa.backing_store = Always;

    dc->wm.xwindc.wnd = wnd = XCreateWindow(globalDisplay, RootWindowOfScreen(scr), 0,0, dc->mi.xRes+1, dc->mi.yRes+1,
                                            0, CopyFromParent, CopyFromParent, CopyFromParent,
                                            CWBackPixel | CWBackingStore, &xswa
                                            );
    buf = PM_malloc((dc->mi.xRes+1) * (dc->mi.yRes+1));
    dc->wm.xwindc.img = XCreateImage(globalDisplay, DefaultVisualOfScreen(scr),
                                     8, ZPixmap, 0, NULL, dc->mi.xRes+1, dc->mi.yRes+1, 8, 0);
    dc->wm.xwindc.img->data = buf;
    XInitImage(dc->wm.xwindc.img);

    // Sets up the private colormap
    dc->wm.xwindc.hpal = xswa.colormap = XCreateColormap(globalDisplay, wnd, DefaultVisualOfScreen(scr), AllocAll);
    XChangeWindowAttributes(globalDisplay, wnd, CWColormap, &xswa);

    XMapRaised(globalDisplay, wnd);
    XClearWindow(globalDisplay, wnd);
    XStoreName(globalDisplay, wnd, "MGL [Unix/X11 8 bit Display]");

    XWIN_initInternal(dc);
    dc->r.realizePalette = XWIN8_realizePalette;
    dc->r.getDefaultPalette = XWIN8_getDefaultPalette;
    dc->r.putImage = XWIN8_putImage;
    return true;
}

void *  MGLAPI XWIN8_getDefaultPalette(MGLDC *dc)
{
    XColor cols[256];
    static palette_t default_pal[256];
    int i;
    xwindc_vars *v = &dc->wm.xwindc;
    /*
    for(i=0; i<256; i++){
        cols[i].pixel = i;
        cols[i].flags = DoGreen | DoBlue | DoRed;
    }
    XQueryColors(v->dpy, v->hpal, cols, 256);
    for(i=0; i<256; i++){
        default_pal[i].green = cols[i].green >> 8;
        default_pal[i].blue = cols[i].blue >> 8;
        default_pal[i].red = cols[i].red >> 8;
    }
    */
    for(i=0; i<256; i++){
        default_pal[i].green =  default_pal[i].blue = default_pal[i].red = i;
    }

    return default_pal;
}

void    MGLAPI XWIN8_realizePalette(MGLDC *dc,palette_t *pal,int num,int index,int waitVRT)
/****************************************************************************
*
* Function:     XWIN8_realizePalette
* Parameters:   dc      - Device context
*               pal     - Palette of values to program
*               num     - Number of entries to program
*               index   - Index to start programming at
*
* Description:  Program the X11 colormap. First we need to translate the
*               values from the MGL internal format into the 16 bit RGB
*               format used by X11.
*
****************************************************************************/
{
    int i;
    XColor *cols = PM_malloc(sizeof(XColor) * num);
    xwindc_vars *v = &dc->wm.xwindc;

    for(i=0; i<num; i++){
        cols[i].flags = DoRed | DoGreen | DoBlue;
        cols[i].pixel = i+index;
        cols[i].red   = pal[i].red << 8;
        cols[i].green = pal[i].green << 8;
        cols[i].blue  = pal[i].blue << 8;
    }
    XStoreColors(v->dpy, v->hpal, cols, num);
    XFlush(v->dpy);
    PM_free(cols);
}

void MGLAPI XWIN8_putImage(MGLDC *dc,int left,int top,int right,
    int bottom,int dstLeft,int dstTop,int op,void *surface,
    int bytesPerLine,MGLDC *src)
/****************************************************************************
*
* Function:     XWIN8_putImage
* Parameters:   dc      - Device context
*
* Description:  Blt's a system memory buffer DC of the same pixel depth
*               as the display device context to the display DC.
*
****************************************************************************/
{
    MGL_HDC gc = dc->wm.xwindc.gc;
    Window wnd = dc->wm.xwindc.wnd;
    Display *dpy = dc->wm.xwindc.dpy;
    xwindc_vars *v = &dc->wm.xwindc;

    //  XImage  *im = src->wm.memdc.img;
    XImage *im; // = dc->wm.xwindc.img;
    int func;

    switch(op){
    case MGL_REPLACE_MODE:
        func = GXcopy;
        break;
    case MGL_AND_MODE:
        func = GXand;
        break;
    case MGL_OR_MODE:
        func = GXor;
        break;
    case MGL_XOR_MODE:
        func = GXxor;
        break;
    }
    XSetFunction(dpy,gc,func);
    //  memcpy(im->data, surface, (src->mi.xRes+1) * (src->mi.yRes+1));
    //  memcpy(im->data, surface, (right-left+1) * (bottom-top+1));
    im = XCreateImage(dpy, DefaultVisualOfScreen(v->scr), 8, ZPixmap, 0, surface, right-left, bottom-top, 8, 0);
    XInitImage(im);
    XPutImage(dpy, wnd, gc, im, 0,0, dstLeft, dstTop, right-left,bottom-top);
    XDestroyImage(im);
}
