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
//      DOOM graphics stuff for MGL
//      doesn't contain any system-specific code!
//-----------------------------------------------------------------------------

static const char
rcsid[] = "$Id: i_x.c,v 1.6 1997/02/03 22:45:10 b1 Exp $";

#include <stdlib.h>
#include <ctype.h>

#include <stdarg.h>
//#include <sys/time.h>
#include <sys/types.h>
//#include <sys/socket.h>

//#include <netinet/in.h>
#include <signal.h>

#include "doomstat.h"
#include "i_system.h"
#include "v_video.h"
#include "m_argv.h"
#include "d_main.h"

#include "doomdef.h"

#include "mgraph.h"
#include "pmapi.h"

// Blocky mode,
// replace each 320x200 pixel with multiply*multiply pixels.
// According to Dave Taylor, it still is a bonehead thing
// to use ....
//static int      multiply=1;

MGLDC           *dc;     // MGL scren device context
MGLDC           *memDC;  // MGL memory device context
ibool           useLinear = true;
ibool           useDirectDraw = true;
ibool           useWinDirect = true;

int             mglMode = -1;

//
//  Translates the keyboard scan code currently in MGL event
//
int xlatescancode(int code)
{
   switch (code) {
      case KB_F1:          return KEY_F1;            
      case KB_F2:          return KEY_F2;            
      case KB_F3:          return KEY_F3;            
      case KB_F4:          return KEY_F4;            
      case KB_F5:          return KEY_F5;            
      case KB_F6:          return KEY_F6;            
      case KB_F7:          return KEY_F7;            
      case KB_F8:          return KEY_F8;            
      case KB_F9:          return KEY_F9;            
      case KB_F10:         return KEY_F10;           
      case KB_F11:         return KEY_F11;           
      case KB_F12:         return KEY_F12;           
      case KB_left:        return KEY_LEFTARROW;     
      case KB_right:       return KEY_RIGHTARROW;    
      case KB_down:        return KEY_DOWNARROW;     
      case KB_up:          return KEY_UPARROW;       
      case KB_esc:         return KEY_ESCAPE;        
      case KB_enter:       return KEY_ENTER;         
      case KB_tab:         return KEY_TAB;           

      case KB_backspace:
      case KB_delete:      return KEY_BACKSPACE;     

/*    case KB_pause:       return KEY_PAUSE;         */

      case KB_pageUp:      return KEY_EQUALS;        

      case KB_pageDown:    return KEY_MINUS;         

      case KB_leftShift:   return KEY_RSHIFT;        
      case KB_rightShift:  return KEY_RSHIFT;        

      case KB_leftCtrl:    return KEY_RCTRL;         
      case KB_rightCtrl:   return KEY_RCTRL;         

      case KB_leftAlt:     return KEY_RALT;          
      case KB_rightAlt:    return KEY_RALT;          

      case KB_space:       return ' ';                
   }
   return 0;
}

//
//  Translates the ASCII code currently in MGL event
//
int xlateascii(int code)
{
   switch (code) {
      case ASCII_esc:       return KEY_ESCAPE;        
      case ASCII_enter:     return KEY_ENTER;         
      case ASCII_tab:       return KEY_TAB;         
      case ASCII_backspace: return KEY_BACKSPACE;    
      case ASCII_space:     return ' ';
      case ASCII_minus:     return KEY_MINUS;
      case ASCII_plus:
      case ASCII_equals:    return KEY_EQUALS;        
   }
   if (isalnum(code))
      return code;   
   return 0;
}

void I_ShutdownGraphics(void)
{
   // Close MGL
   MGL_exit();
}



//
// I_StartFrame
//
void I_StartFrame (void)
{
    // er?

}

boolean         mousemoved = false;

void I_GetEvent(void)
{

    event_t_ event; /* DOOM event */
    event_t  mglev; /* MGL event */


    if (EVT_getNext(&mglev,EVT_EVERYEVT)) {
        switch (mglev.what) {
            case EVT_KEYDOWN:
                event.type = ev_keydown;
                if ((event.data1 = xlatescancode(EVT_scanCode(mglev.message))) == 0) 
                    event.data1 = xlateascii(EVT_asciiCode(mglev.message));
                D_PostEvent(&event);
                // fprintf(stderr, "kd");
                break;

            case EVT_KEYUP:
                event.type = ev_keyup;
                if ((event.data1 = xlatescancode(EVT_scanCode(mglev.message))) == 0) 
                    event.data1 = xlateascii(EVT_asciiCode(mglev.message));
                D_PostEvent(&event);
                // fprintf(stderr, "ku");
                break;

            case EVT_MOUSEDOWN:
               event.type = ev_mouse;
               event.data1 =
                   (mglev.modifiers & EVT_LEFTBUT ? 1 : 0)
                   | (mglev.modifiers & EVT_RIGHTBUT ? 2 : 0)
                   | (mglev.modifiers & EVT_MIDDLEBUT ? 4 : 0);
               event.data2 = event.data3 = 0;
               D_PostEvent(&event);
               // fprintf(stderr, "b");
               break;

            case EVT_MOUSEUP:
               event.type = ev_mouse;
               event.data1 =
                   (mglev.modifiers & EVT_LEFTBUT ? 1 : 0)
                   | (mglev.modifiers & EVT_RIGHTBUT ? 2 : 0)
                   | (mglev.modifiers & EVT_MIDDLEBUT ? 4 : 0);
               event.data2 = event.data3 = 0;
               D_PostEvent(&event);
               // fprintf(stderr, "b");
               break;

            case EVT_MOUSEMOVE:
               event.type = ev_mouse;
               event.data1 =
                   (mglev.modifiers & EVT_LEFTBUT ? 1 : 0)
                   | (mglev.modifiers & EVT_RIGHTBUT ? 2 : 0)
                   | (mglev.modifiers & EVT_MIDDLEBUT ? 4 : 0);
               event.data2 = mglev.relative_x << 2;
               event.data3 = -mglev.relative_y << 2;

               D_PostEvent(&event);
               // fprintf(stderr, "m");
               mousemoved = false;
               break;
        }
    }
}

//
// I_StartTic
//
void I_StartTic (void)
{

    I_GetEvent();
    mousemoved = false;

}


//
// I_UpdateNoBlit
//
void I_UpdateNoBlit (void)
{
    // what is this?
}

//
// I_FinishUpdate
//
void I_FinishUpdate (void)
{
    static int  lasttic;
    int         tics;
    int         i;
    // UNUSED static unsigned char *bigscreen=0;

    // draws little dots on the bottom of the screen
    if (devparm)
    {

        i = I_GetTime();
        tics = i - lasttic;
        lasttic = i;
        if (tics > 20) tics = 20;

        for (i=0 ; i<tics*2 ; i+=2)
            screens[0][ (SCREENHEIGHT-1)*SCREENWIDTH + i] = 0xff;
        for ( ; i<20*2 ; i+=2)
            screens[0][ (SCREENHEIGHT-1)*SCREENWIDTH + i] = 0x0;

    }

    {
        // draw the image through MGL
        MGL_bitBltCoord(dc,memDC,0,0,SCREENWIDTH,SCREENHEIGHT,0,0,
                        MGL_REPLACE_MODE);
        MGL_swapBuffers(dc,true);
    }

}


//
// I_ReadScreen
//
void I_ReadScreen (byte* scr)
{
    memcpy (scr, screens[0], SCREENWIDTH*SCREENHEIGHT);
}


//
// Palette stuff.
//
palette_t    pal[256];

void UploadNewPalette(byte *palette)
{

    register int        i;
    register unsigned   c;
    static boolean      firstcall = true;

    // initialize the palette
    if (firstcall)
    {
        firstcall = false;
        for (i = 0; i < 256; i++)
        {
            pal[i].alpha = 0;
        }
    }
    // set the MGL palette entries
    for (i=0 ; i<256 ; i++)
    {
        c = gammatable[usegamma][*palette++];
        pal[i].red   = c;
        c = gammatable[usegamma][*palette++];
        pal[i].green = c;
        c = gammatable[usegamma][*palette++];
        pal[i].blue  = c;
    }

    // store the colors to the current colormap
    // Program palette using MGL
    if (MGL_getBitsPerPixel(dc) == 8) {
        MGL_setPalette(dc,pal,256,0);
        MGL_realizePalette(dc,256,0,true);
    }
}

//
// I_SetPalette
//
void I_SetPalette (byte* palette)
{
    UploadNewPalette(palette);
}

extern void initFatalError(void)
{
    char    buf[80];
    sprintf(buf,"Graphics error: %s\n",MGL_errorMsg(MGL_result()));
    MGL_fatalError(buf);
}

int _ASMAPI doSuspendApp(
    MGLDC *dc,
    int flags)
{
    if (flags == MGL_REACTIVATE) {
        MGLDC *oldDC = MGL_makeCurrentDC(dc);
        MGL_clearDevice();
        MGL_makeCurrentDC(oldDC);
        }
    return MGL_NO_SUSPEND_APP;
}


//
// I_InitGraphics
//
void I_InitGraphics(void)
{
    pixel_format_t    pf;
    static  int       firsttime = 1;

    if (!firsttime)
        return;
    firsttime = 0;

    signal(SIGINT, (void (*)(int)) I_Quit);

    /* Create a MGL display device context */
    if ((dc = MGL_createDisplayDC(mglMode, MGL_availablePages(mglMode), MGL_DEFAULT_REFRESH)) == NULL)
        initFatalError();
        MGL_makeCurrentDC(dc);

    /* Turn off identity palette checking for maximum speed */
    MGL_checkIdentityPalette(false);

    // allocate MGL image buffer ?
    if ((memDC = MGL_createMemoryDC(SCREENWIDTH, SCREENHEIGHT, 8, &pf)) == NULL)
            MGL_fatalError("Can't allocate offscreen buffer!\n");

    // replace the screens[0] pointer so that we can get rid of a memcpy()
    screens[0] = memDC->surface;

    if (!MGL_doubleBuffer(dc))
        MGL_fatalError("Double buffereing not available!");

    MGL_makeCurrentDC(dc);
}
