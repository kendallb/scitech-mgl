/****************************************************************************
*
*                    SciTech SNAP Linux textmode program
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
* Environment:  Linux
*
* Description:  Program that uses the SNAP drivers to set extended
*               text modes on Linux VT's.
*               Portions (C) Eugene Crosser & Andries Brouwer
*
****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/tty.h>
#include <linux/kd.h>
#include <linux/vt.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include "pmapi.h"
#include "snap/graphics.h"
#include "snap/copyrigh.h"

/*---------------------------- Global Variables ---------------------------*/

GA_configInfo   info;
GA_initFuncs    init;
GA_modeInfo     mi;
int             xres,yres;
GA_devCtx       *dc;

// Default path where font files are usually located
#define FONTPATH "/usr/lib/kbd/consolefonts/"

/*-------------- Ripped from psf.h from the setfont utility ---------------*/

#define PSF_MAGIC1  0x36
#define PSF_MAGIC2  0x04

#define PSF_MODE512    0x01
#define PSF_MODEHASTAB 0x02
#define PSF_MAXMODE    0x03
#define PSF_SEPARATOR  0xFFFF

struct psf_header
{
  unsigned char magic1, magic2; /* Magic number */
  unsigned char mode;       /* PSF font mode */
  unsigned char charsize;   /* Character size */
};

#define PSF_MAGIC_OK(x) ((x).magic1 == PSF_MAGIC1 && (x).magic2 == PSF_MAGIC2)

/*---------------------------- Implementation -----------------------------*/

void printModes(
    FILE *fd)
{
    N_uint16 *modes;
    int i;

    fprintf(fd,"Text modes available on %s:\n\n", info.ChipsetName);
    for (modes = dc->AvailableModes; *modes != 0xFFFF; modes++) {
        mi.dwSize = sizeof(mi);
        if (init.GetVideoModeInfo(*modes,&mi) != 0)
            continue;
        if (!(mi.Attributes & gaIsTextMode))
            continue;
        fprintf(fd,"\t%d x %d, %dx%d character cells\t [ ",
                mi.XResolution/mi.XCharSize, mi.YResolution/mi.YCharSize,
                mi.XCharSize, mi.YCharSize);
        for (i=0; mi.RefreshRateList[i] != -1; i++) {
            N_int32 r = mi.RefreshRateList[i];
            if (r<0) {
                r = -r;
                fputc('i',fd);
                }
            fprintf(fd,"%dHz ", r);
            }
        fprintf(fd,"]\n");
        }
}

static void LoadFont(
    int console,
    const char *file)
{
    static char buf[16384], fn[256];
    struct psf_header psfhdr;
    struct consolefontdesc cfd;
    struct stat st;
    int fontsize;
    int hastable;
    int head, i, unit;

    // TODO: Add support for gzip'ed font

    FILE *fd;

    if (!stat(fn,&st))
        strcpy(fn,file);
    else {
        strcpy(fn,FONTPATH);
        strcat(fn,file);
        if (stat(fn,&st)<0) {
            printf("Unable to find font file.\n");
            return;
            }
        }
    fd = fopen(fn, "rb");
    if (!fd) {
        printf("Unable to open %s font file.\n", fn);
        return;
        }

    if (fread(&psfhdr, sizeof(struct psf_header), 1, fd) != 1) {
        perror("Error reading header input font");
        return;
        }
    if (PSF_MAGIC_OK(psfhdr)) {
        if (psfhdr.mode > PSF_MAXMODE) {
            printf("Unsupported psf file mode\n");
            exit(1);
            }
        fontsize = ((psfhdr.mode & PSF_MODE512) ? 512 : 256);
        hastable = (psfhdr.mode & PSF_MODEHASTAB);
        unit = psfhdr.charsize;
        head = sizeof(struct psf_header) + fontsize*unit;
        if (head > st.st_size || (!hastable && head != st.st_size)) {
            printf("Input file: bad length\n");
            return;
            }
        }
    else { /* Font not in PSF format */
        /* file with three code pages? */
        if (st.st_size == 9780) {
            unit = mi.YCharSize;
            printf("Choosing automatically 8x%d codepage.\n",unit);
            switch (unit) {
                case 8:
                    fseek(fd,7732,SEEK_SET); break;
                case 14:
                    fseek(fd,4142,SEEK_SET); break;
                case 16:
                    fseek(fd,40,SEEK_SET); break;
                }
            }
        else {
            if (st.st_size & 0377) {
                printf("Bad font input file size.\n");
                return;
                }
            else
                unit = st.st_size / 256;
            }
        fontsize = 256;
        }

    memset(buf,0,sizeof(buf));
    if (unit < 1 || unit > 32) {
        printf("Bad character size %d\n", unit);
        return;
        }
    for (i = 0; i < fontsize; i++) {
        if (fread(buf+(32*i), unit, 1, fd) != 1) {
            perror("Cannot read font from file");
            return;
            }
        }

    cfd.charcount = fontsize;
    cfd.charheight = unit;
    cfd.chardata = buf;

    if (ioctl(console,PIO_FONTX,&cfd)<0) {
        perror("ioctl(PIO_FONTX)");
        return;
        }
    printf("Console font %s loaded.\n", fn);
}

void SetMode(
    int console,
    N_uint16 mode,
    int refreshRate,
    char *font)
{
    struct winsize      ws, old_ws;
    struct vt_consize   cs;
    N_int32             dummy = -1, bpl=-1, maxMem = -1;
    GA_CRTCInfo         crtc;

    // Resize the software terminal
    ws.ws_col = xres;
    ws.ws_row = yres;

    // Resize the hardware terminal
    cs.v_cols = xres;
    cs.v_rows = yres;
    cs.v_vlin = mi.YResolution;
    cs.v_vcol = mi.XResolution;
    cs.v_clin = mi.YCharSize;
    cs.v_ccol = mi.XCharSize;
    if (ioctl(console, TIOCGWINSZ, &old_ws)<0)
        perror("ioctl(TIOCGWINSZ)");
    if (ioctl(console, VT_RESIZEX, &cs)<0)
        perror("ioctl(VT_RESIZEX)");
    if (ioctl(console, TIOCSWINSZ, &ws)<0)
        perror("ioctl(TIOCSWINSZ)");

    // Set the SNAP mode
    if (init.SetVideoMode(mode,&dummy,&dummy,&bpl,&maxMem,refreshRate,NULL) == 0) {
        printf("Previous terminal size: %dx%d\n", old_ws.ws_col, old_ws.ws_row);
        init.GetCRTCTimings(&crtc);
        if (refreshRate)
            printf("%dx%d text mode set with %dHz refresh rate.\n", xres, yres, refreshRate);
        else
            printf("%dx%d text mode set with default %dHz refresh rate.\n", xres, yres, crtc.RefreshRate/100);
        if (font)
            LoadFont(console,font);
        init.SetCRTCTimings(&crtc);
        if (refreshRate)
            GA_saveCRTCTimings(dc,init.GetActiveHead());
        }
    else {
        if (ioctl(console, TIOCSWINSZ, &old_ws)<0)
            perror("ioctl(TIOCSWINSZ)");
        printf("Failed to set %dx%d text mode.\n", xres, yres);
        }
}

static void PrintUsage(
    char *n)
{
    printf( GRAPHICS_PRODUCT" Textmode setting program for Linux.\n"
            SCITECH_COPYRIGHT_MSG_SHORT"\n\n"
            "Usage: %s xres yres [-][refresh] [font] [device]\n"
            "    or %s -l  (get the list of available text modes)\n"
            "\trefresh = Refresh rate to set (0 = default, < 0 = interlaced)\n"
            "\t          This becomes the default refresh rate for this mode.\n"
            "\tfont    = Name of console font file to set\n"
            "\tdevice  = Device index for multiple controllers\n",
            n,n);
}

/*************** Program entry point *******************/

int main(
    int argc,
    char *argv[])
{
    int             deviceIndex = 0, numDevices;
    int             refresh = 0;
    N_uint16        *modes;
    char            *font = NULL;
    PM_HWND         console;
    void            *stateBuf;

    // Print usage for invalid command line
    if (argc!=2 && (argc<3 || argc>6)) {
        PrintUsage(argv[0]);
        return 1;
        }

    // Open the console for output. If the system is stuck in
    // KD_GRAPHICS mode, this function will restore it back to
    // the regular mode of operation.
    console = PM_openConsole(0, 0, 0,0,0, true);
    if ((stateBuf = malloc(PM_getConsoleStateSize())) == NULL)
        PM_fatalError("Out of memory!");
    PM_saveConsoleState(stateBuf,console);

    if (argc==6)
        deviceIndex = atoi(argv[5]);

    numDevices = GA_enumerateDevices(false);
    if (deviceIndex >= numDevices)
        PM_fatalError("Device not found!");

    // Load the device driver for this device
    PM_lockSNAPAccess(0, true);
    GA_setActiveDevice(deviceIndex);
    PM_unlockSNAPAccess(0);
    if ((dc = GA_loadDriver(deviceIndex,false)) == NULL)
        PM_fatalError(GA_errorMsg(GA_status()));
    PM_lockSNAPAccess(0, true);
    GA_setActiveDevice(0);
    PM_unlockSNAPAccess(0);
    init.dwSize = sizeof(init);
    if (!GA_queryFunctions(dc,GA_GET_INITFUNCS,&init))
        PM_fatalError("Unable to get device driver functions!");
    info.dwSize = sizeof(info);
    init.GetConfigInfo(&info);

    // Restore the console *before* we change the SNAP text mode!
    PM_restoreConsoleState(stateBuf,console);

    if (argc == 2) {
        if (strcmp("-l", argv[1])) {
            PrintUsage(argv[0]);
            }
        else {
            printModes(stdout);
            }
        }
    else {
        // Find the SNAP text mode and set it
        xres = atoi(argv[1]);
        yres = atoi(argv[2]);
        if (argc>3)
            refresh = atoi(argv[3]);
        if (argc>4)
            font = argv[4];
        for (modes = dc->AvailableModes; *modes != 0xFFFF; modes++) {
            mi.dwSize = sizeof(mi);
            if (init.GetVideoModeInfo(*modes,&mi) != 0)
                continue;
            if (!(mi.Attributes & gaIsTextMode))
                continue;
            if (xres == (mi.XResolution/mi.XCharSize) && yres == (mi.YResolution/mi.YCharSize)) {
                SetMode(console, *modes, refresh, font);
                break;
                }
            }
        if (*modes == 0xFFFF) {
            printf("%dx%d text mode not supported by this driver.\n", xres,yres);
            printModes(stdout);
            }
        }

    // Unload the SNAP device driver
    GA_unloadDriver(dc);

    // Close the console and exit
    PM_closeConsole(console);
    return 0;
}

