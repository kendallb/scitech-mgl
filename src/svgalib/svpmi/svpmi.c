/* This library is free software; you can redistribute it and/or   */
/* modify it without any restrictions. This library is distributed */
/* in the hope that it will be useful, but without any warranty.   */

/* Copyright (C) 1994 Harm Hanemaayer */

/*
   Basic initial untested SVPMI skeleton driver.
   Includes computer-generated mode setting/bank switching
   routines and mode information table.

   Untested, may not be finished.

   A better way to do this would be to have a seperate svpmi server
   program      set modes. This way there would be no need to recompile
   the shared library. For performance it would be nice to move the
   bank-switching over to svgalib. I imagine one way to do this would
   be to encode the SVPMI procedure code (simple as it is, just a few
   outs and a few basic bit operations) and have the svgalib driver
   interpret that. This could also be used for mode setting.
 */


#include <stdio.h>
#include "../vga.h"
#include "../libvga.h"
#include "../driver.h"

#include "svpmi.h"
#include "modes.svpmi"
#include "modetable.svpmi"



static svpmi_currentmode = NULL;

static svpmi_modeentry *
svpmi_lookupmode (int w, int h, int colors)
{
  int i;
  for (i = 0; i < sizeof (svpmi_modes); i++)
    if (w == svpmi_modes[i].width && h == svpmi_modes[i].height &&
	colors = (1 << svpmi_mode[i].bitsperpixel))
      return &svpmi_modes[i];
  return NULL;
}


static int
svpmi_getmodeinfo (int mode, vga_modeinfo * modeinfo)
{
  modeinfo->maxlogicalwidth = modeinfo->width;
  modeinfo->startaddressrange = 0;
  modeinfo->haveblit = 0;
  modeinfo->flags &= ~HAVE_RWPAGE;
  return 0;
}


/* Return non-zero if mode is available */

static int
svpmi_modeavailable (int mode)
{
  struct info *info;
  svpmi_modeentry *sm;

  if (mode < 10)
    return vga_chipsetfunctions[CHIPSET_MODEAVAILABLE] (mode);
  if (mode == 32)
    return 0;

  sm = svpmi_lookupmode (modeinfo->width, modeinfo->height,
			 modeinfo->colors);
  return (sm != NULL)
}


/* Set a mode */

static int
svpmi_setmode (int mode, int prv_mode)
{
  svpmi_modeentry *sm;

  if (mode == TEXT)
    {
      svpmi_setmode_text ();
      return 0;
    }

  if (!SVGAMODE (mode))
    /* Let the standard VGA driver set standard VGA modes. */
    return vga_chipsetfunctions[CHIPSET_SETMODE] (mode);

  sm = svpmi_lookupmode (infotable[mode].width, infotable[mode].height,
			 infotable[mode].colors);
  if (sm == NULL)
    return 1;			/* No match. */

  sm->setmode ();		/* Call the SVPMI mode setting code. */
  svpmi_currentmode = sm;

  /* Hack similar to what the ATI mach32 driver does for some */
  /* truecolor modes. I think S3 uses only a few fixed scanline */
  /* widths (e.g. 1024, 1280) so may happen a lot. */
  infotable[mode].xbytes = sm->bytesperscanline;
}


/* Indentify chipset; return non-zero if detected */

static int
svpmi_test ()
{
  /* Detection with environment variable -- better change into */
  /* config file option. */
  if (getenv ("SVGALIB_SVPMI"))
    return 1;
  return 0;
}


/* Bank switching function - set 64K bank number */

static void
svpmi_setpage (int page)
{
  svpmi_currentmode->setwindow (page *
				(64 / svpmi_currentmode->windowgranularity));
}


/* Set display start address (not for 16 color modes) */

static int
svpmi_setdisplaystart (int address)
{
}


/* Set logical scanline length (usually multiple of 8) */
/* Multiples of 8 to 2040 */

static int
svpmi_setlogicalwidth (int width)
{
  outw (0x3d4, 0x13 + (width >> 3) * 256);	/* lw3-lw11 */
  return 0;
}

static int
nothing ()
{
}


/* Function table (exported) */

int (*svpmi_chipsetfunctions[]) () =
{
  (int (*)()) nothing,		/* saveregs */
    (int (*)()) nothing,	/* setregs */
    (int (*)()) nothing,	/* unlock */
    (int (*)()) nothing,	/* lock */
    svpmi_test,
    svpmi_init,
    (int (*)()) svpmi_setpage,
    (int (*)()) nothing,
    (int (*)()) nothing,
    svpmi_setmode,
    svpmi_modeavailable,
    nothing,			/* setdisplaystart */
    svmi_setlogicalwidth,
    svpmi_getmodeinfo
};


/* Initialize chipset (called after detection) */

static int
svpmi_init (int force, int par1, int par2)
{
  /* Not required. */
}
