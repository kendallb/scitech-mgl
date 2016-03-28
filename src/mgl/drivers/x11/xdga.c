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
* Environment:  Unix/X11/XFree86
*
* Description:  Main XFree86 DGA X11 fullscreen device driver file. This file
*               will be linked in when a call to MGL_registerDriver() is
*               made to register this device driver for use.
*
****************************************************************************/

#include "mgldd.h"
#include "drivers/unix/xdga.h"
#include "drivers/packed/packed8.h"
#include "drivers/packed/packed16.h"
#include "drivers/packed/packed24.h"
#include "drivers/packed/packed32.h"
#include <X11/extensions/xf86dga.h>
#include <X11/extensions/xf86vmode.h>
#include <stdio.h>

/*---------------------------- Driver Tables -----------------------------*/

drivertype  _VARAPI XDGA_driver = {
    IDENT_FLAG,
    MGL_XDGANAME,
    MGL_XDGANAME,
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

/* Resolutions supported by this driver. */
PRIVATE gmode_t XDGA_modes[] = {
    {319,199,8,1,0xFF,0,0,1200,0,0x4F02},
    {319,239,8,1,0xFF,0,0,1000,0,0x4F02},
    {319,399,8,1,0xFF,0,0,600,0,0x4F02},
    {319,479,8,1,0xFF,0,0,500,0,0x4F02},
    {399,299,8,1,0xFF,0,0,1000,0,0x4F02},
    {511,383,8,1,0xFF,0,0,1000,0,0x4F02},
    {639,349,8,1,0xFF,0,0,1371,0,0x4F02},
    {639,399,8,1,0xFF,0,0,1200,0,0x4F02},
    {639,479,8,1,0xFF,0,0,1000,0,0x4F02},
    {799,599,8,1,0xFF,0,0,1000,0,0x4F02},
    {1023,767,8,1,0xFF,0,0,1000,0,0x4F02},
    {1151,863,8,1,0xFF,0,0,1000,0,0x4F02},
    {1279,959,8,1,0xFF,0,0,1000,0,0x4F02},
    {1279,1023,8,1,0xFF,0,0,938,0,0x4F02},
    {1599,1199,8,1,0xFF,0,0,1000,0,0x4F02},
};


/*------------------------------ Implementation ------------------------------*/

static ibool findMode(XDGA_data *inst,gmode_t *mi,ibool linear)
/****************************************************************************
*
* Function:     findMode
* Parameters:   mi      - Mode information block to fill in
* Returns:      True if valid mode was found
*
* Description:  Calls the SVGA VBE BIOS to determine the information for
*               the specified video mode. We search for a video mode that
*               matches our specifications of pixels resolution and depth,
*               and fill in the remainder of the modeInfo block with the
*               values returned by the VBE.
*
****************************************************************************/
{
    XF86VidModeModeInfo *modes;
    int count, i;

    if (!XF86VidModeQueryExtension(x_disp, &EventBase, &ErrorBase)){
        fprintf(stderr,"MGL Warning: Unable to query VidMode extension.\n");

    }else{
        XF86VidModeGetAllModeLines(inst->dpy,inst->scr, &count, &modes);
        if(inst->depth != mi->bitsPerPixel)
            return false; /* The VidMode extension does not allow us to dynamically change bit depths */
        for(i=0; i<count; i++){
            if(modes[i].hdisplay==mi->xRes && modes[i].vdisplay==mi->yRes)
                break;
        }
        if(i==count)
            return false;
    }
    if (inst->depth <= 8) {
        mi->redMaskSize         = 8;
        mi->redFieldPosition    = 16;
        mi->greenMaskSize       = 8;
        mi->greenFieldPosition  = 8;
        mi->blueMaskSize        = 8;
        mi->blueFieldPosition   = 0;
        mi->rsvdMaskSize        = 0;
        mi->rsvdFieldPosition   = 0;
        }
    else {
        // TODO: This !
        mi->redMaskSize = md->LinRedMaskSize;
        mi->redFieldPosition = md->LinRedFieldPosition;
        mi->greenMaskSize = md->LinGreenMaskSize;
        mi->greenFieldPosition = md->LinGreenFieldPosition;
        mi->blueMaskSize = md->LinBlueMaskSize;
        mi->blueFieldPosition = md->LinBlueFieldPosition;
        mi->rsvdMaskSize = md->LinRsvdMaskSize;
        mi->rsvdFieldPosition = md->LinRsvdFieldPosition;
        }


    return true;
}

void MGLAPI XDGA_useMode(modetab modes,int mode,int id,gmode_t *mi)
/****************************************************************************
*
* Function:     XDGA_useMode
* Parameters:   modes       - Video mode/device driver association table
*               mode        - Index into mode association table
*               id          - Driver id to store in table
*               mi          - Mode information block to fill in
*
* Description:  Calls the XF86 VidMode extension to determine the information for
*               the specified video mode. We search for a video mode that
*               matches our specifications of pixels resolution and depth,
*               and fill in the remainder of the modeInfo block with the
*               values returned by the VBE.
*
****************************************************************************/
{
    ulong   flags;

    if (findMode(inst,mi,true,&flags) ) {
        if (md->ModeAttributes & vbeMdLinear)
            mi->scratch2 |= vbeLinearBuffer;
        else if (useVirtual && inst->svga.useVFlat)
            mi->scratch2 |= VBE_VIRTUAL_LINEAR;
        else
            return;
        DRV_useMode(modes,mode,id,mi->maxPage+1,flags);
        }
}

ibool MGLAPI XDGA_detect(void *data,int id,int force,int *driver,int *mode,modetab modes)
/****************************************************************************
*
* Function:     XDGA_detect
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
    if(force >= grXDGA && XWIN_detect() ) {
        XDGA_data *inst = (XDGA_data*) data;
        int dga_flags;

        if (geteuid()) /* Must have root privileges for DGA */
            return false;

        XF86DGAQueryDirectVideo(inst->dpy, inst->screen, &dga_flags);
        if (! dga_flags & XF86DGADirectPresent)
            return false;

        memcpy(inst->modes,XDGA_defModes,sizeof(inst->modes));
        XDGA_useMode(modes,grSVGA_320x200x256,id,&inst->modes[0],true);
        XDGA_useMode(modes,grSVGA_320x240x256,id,&inst->modes[1],true);
        XDGA_useMode(modes,grSVGA_320x400x256,id,&inst->modes[2],true);
        XDGA_useMode(modes,grSVGA_320x480x256,id,&inst->modes[3],true);
        XDGA_useMode(modes,grSVGA_400x300x256,id,&inst->modes[4],true);
        XDGA_useMode(modes,grSVGA_512x384x256,id,&inst->modes[5],true);
        XDGA_useMode(modes,grSVGA_640x350x256,id,&inst->modes[6],true);
        XDGA_useMode(modes,grSVGA_640x400x256,id,&inst->modes[7],true);
        XDGA_useMode(modes,grSVGA_640x480x256,id,&inst->modes[8],true);
        XDGA_useMode(modes,grSVGA_800x600x256,id,&inst->modes[9],true);
        XDGA_useMode(modes,grSVGA_1024x768x256,id,&inst->modes[10],true);
        XDGA_useMode(modes,grSVGA_1152x864x256,id,&inst->modes[11],true);
        XDGA_useMode(modes,grSVGA_1280x960x256,id,&inst->modes[12],true);
        XDGA_useMode(modes,grSVGA_1280x1024x256,id,&inst->modes[13],true);
        XDGA_useMode(modes,grSVGA_1600x1200x256,id,&inst->modes[14],true);
        return true;
    }
    return false;
}


void * MGLAPI  XDGA_createInstance(void)
{
    return PM_malloc(sizeof(XDGA_data));
}


void MGLAPI XDGA_destroyInstance(void *data)
/****************************************************************************
*
* Function:     XDGA_destroyInstance
*
* Description:  Frees the internal data structures allocated during the
*               detection process
*
****************************************************************************/
{
    if(data)
        PM_free(data);
}

static void MGLAPI destroyDC(MGLDC *dc)
/****************************************************************************
*
* Function:     destroyDC
* Parameters:   dc  - Device context
*
* Description:  Destroys all internal memory allocated by the device
*               context.
*
****************************************************************************/
{
    if (--dc->v->d.refCount == 0) {
        /* Nothing to do in here */
        }
}

static void startMode(XDGA_data *inst, gmode_t *mi)
/****************************************************************************
*
* Function:     startMode
* Parameters:   mi  - Mode information
*
* Description:  Switches the X server to the desired resolution with
*               the XFree86 VidMode extension, and starts DirectVideo.
*
****************************************************************************/
{
    XF86VidModeModeInfo *modes;
    int count, i;
    Window root;

    XF86VidModeGetAllModeLines(inst->dpy,inst->scr, &count, &modes);
    for(i=0; i<count; i++){
        if(modes[i].hdisplay==mi->xRes && modes[i].vdisplay==mi->yRes)
            break;
    }
    if(i==count)
        MGL_fatalError("VidMode: Could not find suitable video mode.");

    /* Switch to the display mode */
    inst->oldmode = *modes;
    XF86VidModeSwitchToMode(inst->dpy, inst->scr, &modes[i]);
    root = XRootWindow(inst->dpy, inst->scr);
    XGrabKeyboard(inst->dpy, root, True, GrabModeAsync, GrabModeAsync, CurrentTime);
    XGrabPointer(inst->dpy, root, True, PointerMotionMask|ButtonPressMask|ButtonReleaseMask,
                 GrabModeAsync, GrabModeAsync, None, None, CurrentTime);
    XF86DGAGetVideo(inst->dpy, inst->scr, &inst->dga_addr, &inst->dga_linewidth, &inst->dga_banksize, &inst->dga_memsize);
    inst->dga_memsize *= 1024; /* Convert to bytes */

    if(inst->dga_banksize < inst->dga_memsize)
        fprintf(stderr, "MGLDGA: Using banked framebuffer.\n");
    else
        fprintf(stderr, "MGLDGA: Using linear framebuffer.\n");
    setuid(getuid());  /* Give up root privileges */

    XF86DGAGetViewPortSize(inst->dpy, inst->scr, &inst->dga_width, &inst->dga_height);
    /* let's do a security fork */
    if (XF86DGAForkApp(inst->scr) != 0)
        MGL_fatalError("XF86DGAForkApp error");
    else{
        XF86DGADirectVideo(inst->dpy, inst->scr, XF86DGADirectGraphics|XF86DGADirectMouse|XF86DGADirectKeyb);
        /* find space for page flipping */
        inst->dga_pages = 0;
        for (int x=0; x+inst->dga_width <= inst->dga_linewidth; x+=inst->dga_width)
            for (int y=0; y+inst->dga_height <= inst->dga_banksize/(inst->dga_linewidth*inst->byte_per_pixel); y+=inst->dga_height)
                {
                    inst->dga_page[inst->dga_pages].x = x;
                    inst->dga_page[inst->dga_pages].y = y;
                    inst->dga_page[inst->dga_pages].addr = inst->dga_addr+(y*inst->dga_linewidth+x)*inst->byte_per_pixel;
                    inst->dga_page[inst->dga_pages].bytes_per_line = inst->dga_linewidth*inst->byte_per_pixel;
                    if (++inst->dga_pages == MAXPAGES)
                        goto l;
                }
    l:
        /*  All pages should fit into the first bank (??) */
        XF86DGASetVidPage(inst->dpy, inst->scr, 0);
        /* clear screen */
        memset(inst->dga_addr, 0, inst->dga_banksize);
    }
}

ibool   MGLAPI XDGA_initDriver(void *data,MGLDC *dc,int driverId,int modeId,ulong hwnd,
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
    XDGA_data *inst = (XDGA_data*) data;

    dc->mi = XDGA_modes[modeId - grSVGA_320x200x256];
    dc->deviceType = MGL_DISPLAY_DEVICE;

    g_state.d.hardwareCursor = false;

    dc->v = &g_state;
    dc->wm.fulldc.dpy = inst->dpy;
    dc->wm.fulldc.scr = scr = DefaultScreenOfDisplay(inst->dpy);
    dc->wm.fulldc.gc  = DefaultGCOfScreen(scr);
    d->refCount++;

    xswa.background_pixel = BlackPixel(globalDisplay,XScreenNumberOfScreen(scr));

    /* Set up the colormap */
    dc->wm.xwindc.hpal = xswa.colormap = XCreateColormap(globalDisplay, wnd, DefaultVisualOfScreen(scr), AllocAll);

    startMode(inst, &dc->mi);
    dc->wm.fulldc.dga_pages = inst->dga_pages;
    dc->wm.fulldc.pages = inst->pages;

    dc->surface = inst->pages[0].addr;

    XWIN_initInternal(dc);
    dc->r.realizePalette = XDGA_realizePalette;
    dc->r.getDefaultPalette = XWIN_getDefaultPalette;
    switch(dc->wm.fulldc.depth){
    case 8:
        dc->r.getPixel = PACKED8_getPixel;
        dc->r.putPixel = PACKED8_putPixel;
        dc->r.getScanLine = PACKED8_getScanLine;
        dc->r.putScanLine = PACKED8_putScanLine;
        break;
    case 15:
    case 16:
        dc->r.getPixel = PACKED16_getPixel;
        dc->r.putPixel = PACKED16_putPixel;
        dc->r.getScanLine = PACKED16_getScanLine;
        dc->r.putScanLine = PACKED16_putScanLine;
        break;
    case 24:
        dc->r.getPixel = PACKED24_getPixel;
        dc->r.putPixel = PACKED24_putPixel;
        dc->r.getScanLine = PACKED24_getScanLine;
        dc->r.putScanLine = PACKED24_putScanLine;
        break;
    case 32:
        dc->r.getPixel = PACKED32_getPixel;
        dc->r.putPixel = PACKED32_putPixel;
        dc->r.getScanLine = PACKED32_getScanLine;
        dc->r.putScanLine = PACKED32_putScanLine;
        break;
    default:
        MGL_fatalError("MGL DGA: Unsupported color depth");
    }
    return true;
}

void    MGLAPI XDGA_realizePalette(MGLDC *dc,palette_t *pal,int num,int index,int waitVRT)
/****************************************************************************
*
* Function:     XDGA_realizePalette
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
    XF86DGAInstallColormap(v->dpy, v->scr, v->hpal);
    XFlush(v->dpy);
    PM_free(cols);
}

void  MGLAPI XDGA_setActivePage(MGLDC *dc, int page)
{
    dc->surface = dc->wm.fulldc.pages[page].addr;
}

void  MGLAPI XDGA_setVisualPage(MGLDC *dc, int page, int waitVRT)
{
    fulldc_vars *v = &dc->wm.fulldc;

    while (!XF86DGAViewPortChanged(v->dpy, v->scr, v->dga_pages))
        ;  /* unfortunately we must waste some time */
    XF86DGASetViewPort(v->dpy, v->scr, v->pages[page].x, v->pages[page].y);
}
