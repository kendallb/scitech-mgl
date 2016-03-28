/****************************************************************************
*
*                         SciTech Display Doctor
*
*               Copyright (C) 1991-2004 SciTech Software, Inc.
*                            All rights reserved.
*
*  ======================================================================
*  |REMOVAL OR MODIFICATION OF THIS HEADER IS STRICTLY PROHIBITED BY LAW|
*  |                                                                    |
*  |This copyrighted computer code is a proprietary trade secret of     |
*  |SciTech Software, Inc., located at 505 Wall Street, Chico, CA 95928 |
*  |USA (www.scitechsoft.com).  ANY UNAUTHORIZED POSSESSION, USE,       |
*  |VIEWING, COPYING, MODIFICATION OR DISSEMINATION OF THIS CODE IS     |
*  |STRICTLY PROHIBITED BY LAW.  Unless you have current, express       |
*  |written authorization from SciTech to possess or use this code, you |
*  |may be subject to civil and/or criminal penalties.                  |
*  |                                                                    |
*  |If you received this code in error or you would like to report      |
*  |improper use, please immediately contact SciTech Software, Inc. at  |
*  |530-894-8400.                                                       |
*  |                                                                    |
*  |REMOVAL OR MODIFICATION OF THIS HEADER IS STRICTLY PROHIBITED BY LAW|
*  ======================================================================
*
* Language:     ANSI C
* Environment:  IBM PC (Linux/i386)
*
* Description:  SVGAlib mode selection functions.
*
****************************************************************************/

/*                                                                 */
/* This library is free software; you can redistribute it and/or   */
/* modify it without any restrictions. This library is distributed */
/* in the hope that it will be useful, but without any warranty.   */

/* Multi-chipset support Copyright 1993 Harm Hanemaayer */
/* partially copyrighted (C) 1993 by Hartmut Schirmer */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "vga.h"
#include "libvga.h"


#define MODENAME_LENGTH 20

/* This is set by vga.c if there's a 'default_mode' line in the config */
extern int __svgalib_default_mode;

static struct info infotable[] =
{
    {80, 25, 16, 160, 0},	/* VGAlib VGA modes */
    {320, 200, 16, 40, 0},
    {640, 200, 16, 80, 0},
    {640, 350, 16, 80, 0},
    {640, 480, 16, 80, 0},
    {320, 200, 256, 320, 1},
    {320, 240, 256, 80, 0},
    {320, 400, 256, 80, 0},
    {360, 480, 256, 90, 0},
    {640, 480, 2, 80, 0},

    {640, 480, 256, 640, 1},	/* VGAlib SVGA modes */
    {800, 600, 256, 800, 1},
    {1024, 768, 256, 1024, 1},
    {1280, 1024, 256, 1280, 1},

    {320, 200, 1 << 15, 640, 2},	/* Hicolor/truecolor modes */
    {320, 200, 1 << 16, 640, 2},
    {320, 200, 1 << 24, 320 * 3, 3},
    {640, 480, 1 << 15, 640 * 2, 2},
    {640, 480, 1 << 16, 640 * 2, 2},
    {640, 480, 1 << 24, 640 * 3, 3},
    {800, 600, 1 << 15, 800 * 2, 2},
    {800, 600, 1 << 16, 800 * 2, 2},
    {800, 600, 1 << 24, 800 * 3, 3},
    {1024, 768, 1 << 15, 1024 * 2, 2},
    {1024, 768, 1 << 16, 1024 * 2, 2},
    {1024, 768, 1 << 24, 1024 * 3, 3},
    {1280, 1024, 1 << 15, 1280 * 2, 2},
    {1280, 1024, 1 << 16, 1280 * 2, 2},
    {1280, 1024, 1 << 24, 1280 * 3, 3},

    {800, 600, 16, 100, 0},	/* SVGA 16-color modes */
    {1024, 768, 16, 128, 0},
    {1280, 1024, 16, 160, 0},

    {720, 348, 2, 90, 0},	/* Hercules emulation mode */

    {320, 200, 1 << 24, 320 * 4, 4},
    {640, 480, 1 << 24, 640 * 4, 4},
    {800, 600, 1 << 24, 800 * 4, 4},
    {1024, 768, 1 << 24, 1024 * 4, 4},
    {1280, 1024, 1 << 24, 1280 * 4, 4},

    {1152, 864, 16, 144, 0},
    {1152, 864, 256, 1152, 1},
    {1152, 864, 1 << 15, 1152 * 2, 2},
    {1152, 864, 1 << 16, 1152 * 2, 2},
    {1152, 864, 1 << 24, 1152 * 3, 3},
    {1152, 864, 1 << 24, 1152 * 4, 4},

    {1600, 1200, 16, 200, 0},
    {1600, 1200, 256, 1600, 1},
    {1600, 1200, 1 << 15, 1600 * 2, 2},
    {1600, 1200, 1 << 16, 1600 * 2, 2},
    {1600, 1200, 1 << 24, 1600 * 3, 3},
    {1600, 1200, 1 << 24, 1600 * 4, 4},

    {0, 0, 0, 0, 0},		/* 16 user definable modes */
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0}
};

#define MAX_MODES (sizeof(infotable) / sizeof(struct info))


/* This one won't type an error message ... */
int __svgalib_name2number(char *m)
{
    int i;

    for (i = G320x200x16; i <= GLASTMODE; i++) {
	if (strcasecmp(m, vga_getmodename(i)) == 0)	/* check name */
	    return i;
    }
    return -1;
}

int vga_getmodenumber(char *m)
{
    int i;
    char s[3];

    i = __svgalib_name2number(m);
    if (i > 0)
	return i;

    for (i = G320x200x16; i <= GLASTMODE; i++) {
	sprintf(s, "%d", i);
	if (strcasecmp(m, s) == 0)	/* check number */
	    return i;
    }
    if (strcasecmp(m, "PROMPT") == 0)
	return -1;

    fprintf(stderr, "Invalid graphics mode \'%s\'.\n", m);
    return -1;
}

char *
 vga_getmodename(int m)
{
    static char modename[MODENAME_LENGTH];
    int x, y, c;

    if (m <= TEXT || m > GLASTMODE)
	return "";
    x = __svgalib_infotable[m].xdim;
    y = __svgalib_infotable[m].ydim;
    switch (c = __svgalib_infotable[m].colors) {
    case 1 << 15:
	sprintf(modename, "G%dx%dx32K", x, y);
	break;
    case 1 << 16:
	sprintf(modename, "G%dx%dx64K", x, y);
	break;
    case 1 << 24:
	sprintf(modename, (__svgalib_infotable[m].bytesperpixel == 3) ?
		"G%dx%dx16M" : "G%dx%dx16M32", x, y);
	break;
    default:
	sprintf(modename, "G%dx%dx%d", x, y, c);
	break;
    }
    return modename;
}

int vga_getdefaultmode(void) {
    char *stmp = getenv("SVGALIB_DEFAULT_MODE");

/* Process env var first so mode might be overridden by it. */
    if (stmp != NULL && strcmp(stmp, "") != 0) {
      int mode;
      if ( (mode = vga_getmodenumber(stmp)) != -1)
	return mode;
    } else if (__svgalib_default_mode) {
      return __svgalib_default_mode;
    }
    return -1;	/* Not defined */
}
