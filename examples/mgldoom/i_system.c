// Emacs style mode select   -*- C++ -*-
//-----------------------------------------------------------------------------
//
// $Id:$
//
// Copyright (C) 1993-1996 by id Software, Inc.
//
// This source is available for distribution and/or modification
// only under the terms of the DOOM Source Code License as
// published by id Software. All rights reserved.
//
// The source is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// FITNESS FOR A PARTICULAR PURPOSE. See the DOOM Source Code License
// for more details.
//
// $Log:$
//
// DESCRIPTION:
//
//-----------------------------------------------------------------------------

static const char
rcsid[] = "$Id: m_bbox.c,v 1.1 1997/02/03 22:45:10 b1 Exp $";


#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <stdarg.h>
//#include <sys/time.h>

#include "doomdef.h"
#include "m_misc.h"
#include "i_video.h"
#include "i_sound.h"

#include "d_net.h"
#include "g_game.h"

#ifdef __GNUG__
#pragma implementation "i_system.h"
#endif
#include "i_system.h"

#include "ztimer.h"
#include "mgraph.h"

int     mb_used = 6;


void
I_Tactile
( int   on,
  int   off,
  int   total )
{
  // UNUSED.
  on = off = total = 0;
}

ticcmd_t        emptycmd;
ticcmd_t*       I_BaseTiccmd(void)
{
    return &emptycmd;
}


int  I_GetHeapSize (void)
{
    return mb_used*1024*1024;
}

byte* I_ZoneBase (int*  size)
{
    *size = mb_used*1024*1024;
    return (byte *) malloc (*size);
}



//
// I_GetTime
// returns time in 1/35th second tics
//
int  I_GetTime (void)
{
    unsigned long        temp;
    int                  newtics;
    static unsigned int  basetime = 0xFFFFFFFF;

    /* prevent timer rollover */
    if (basetime > 0x07FFFFFF) {
       LZTimerOff();
       LZTimerOn();
       basetime = LZTimerLap();
    }

    temp = LZTimerLap();
    newtics = ((temp - basetime) * TICRATE) / 1000000;
    return newtics;
}


extern void initFatalError(void);
extern ibool    useLinear;
extern ibool    useDirectDraw;
extern ibool    useWinDirect;
extern int      driver;
extern int      mglMode;

#ifdef ISV_LICENSE
#include "snap/graphics.h"
#include "isv.c"
#endif

//
// I_Init
//
void I_Init (void)
{
    ZTimerInit();
    I_InitSound();

#ifdef ISV_LICENSE
    GA_registerLicense(OemLicense,false);
#endif

    /* Start the MGL */
    if (!MGL_init(".", NULL))
            MGL_fatalError("MGL init failed");

    MGL_enableAllDrivers();
    if ((mglMode = MGL_findMode(SCREENWIDTH, SCREENHEIGHT, 8)) == -1)
       MGL_fatalError("Graphics mode not found");

//    I_InitGraphics();
}

//
// I_Quit
//
void I_Quit (void)
{
    D_QuitNetGame ();
    I_ShutdownSound();
    I_ShutdownMusic();
    M_SaveDefaults ();
    I_ShutdownGraphics();
    LZTimerOff();
    exit(0);
}

void I_WaitVBL(int count)
{
   // Not sure what should be here
   //DosSleep(0);
}

void I_BeginRead(void)
{
}

void I_EndRead(void)
{
}

byte*   I_AllocLow(int length)
{
    byte*       mem;

    mem = (byte *)malloc (length);
    memset (mem,0,length);
    return mem;
}


//
// I_Error
//
extern boolean demorecording;

void I_Error (char *error, ...)
{
    va_list     argptr;

    // Message first.
    va_start (argptr,error);
    fprintf (stderr, "Error: ");
    vfprintf (stderr,error,argptr);
    fprintf (stderr, "\n");
    va_end (argptr);

    fflush( stderr );

    LZTimerOff();

    // Shutdown. Here might be other errors.
    if (demorecording)
        G_CheckDemoStatus();

    D_QuitNetGame ();
    I_ShutdownGraphics();

    exit(-1);
}
