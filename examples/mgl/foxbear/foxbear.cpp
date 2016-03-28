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
* Environment:  Any
*
* Description:  Sample port of the ATI Fox & Bear benchmark program to
*               C++ running under the MGL.
*
*               Note that all the code in here is completely independant
*               of what operating system or what mode the demo is running
*               in, and will work in DOS, fullscreen WinDirect and windowed
*               modes on the desktop equally well.
*
* Special Notices:
*
* All artwork, Copyright (c) 1996-95 ATI Technologies Inc.
* All Rights Reserved.
*
* All the artwork for the Fox & Bear demo is Copyright (c) 1996-95 ATI
* Technologies Inc and is being distributed by SciTech Software with
* written permission from ATI Technologies. You may use this demo and
* associated artwork for testing and development purposes only. You may not
* under any circumstances use any portion of this artwork in applications
* that you distribute, nor may you re-distribute modified copies of the
* artwork.
*
****************************************************************************/

#include "fbpch.hpp"
#include "ztimer.h"
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "pmapi.h"
#ifdef  ISV_LICENSE
#include "snap/graphics.h"
#include "isv.c"
#endif

/*---------------------------- Global Variables ---------------------------*/

GMDC                *gm;            // Game Framework object
int                 renderXRes;     // Resolution of rendering device
int                 renderYRes;
font_t              *font;          // Font used for displaying text
char                modeName[80];   // Desription of video mode
color_t             transClr;       // Transparent bitmap color to use
ulong               lastCount;      // Timer count for last fps update
int                 frameCount;     // Number of frames for timing
int                 fpsRate;        // Current frames per second rate
int                 bkgdHeight;     // Background plane height
int                 bkgdColor;      // Background plane color
int                 textColor;      // Color to draw text in
ibool               initialized = false;
SPR_bitmapManager   *sprMgr;        // Sprite manager instance
MGLDC               *convertDC;     // DC for converting bitmaps
MGL_waitVRTFlagType waitForRetrace = MGL_waitVRT;
GM_modeInfo         modeInfo = {640,480,8};
GM_driverOptions driverOpt = {
    true,           /* UseSNAP          */
    false,          /* UseHWOpenGL      */
    MGL_GL_AUTO,    /* OpenGLType       */
    GM_MODE_ALLBPP, /* modeFlags        */
    };

#ifdef  __ALPHA__
ibool               useRLE = false; // DEC Alpha is slower with RLE bitmaps
ibool               forceSysMem = true;
#else
ibool               useRLE = true;
ibool               forceSysMem = false;
#endif
SPR_Bitmap          *bitmapList[C_TILETOTAL + C_FBT + C_BBT];
FX_Sprite           *fox;           // Fox sprite
FX_Sprite           *bear;          // Bear sprite
FX_Sprite           *apple;         // Apple sprite
FX_SurfacePlane     *foreground;    // Foreground plane
FX_Plane            *midground;     // Midground plane
FX_Plane            *background;    // Background plane

#ifdef __UNIX__
#define DATAPATH    "data/"
#else
#define DATAPATH    "data\\"
#endif

/*------------------------------ Implementation ---------------------------*/

bitmap_t *loadAndConvertBitmap(const char *filename)
/****************************************************************************
*
* Function:     FX_FoxBearBitmapManager::loadAndConvertBitmap
* Parameters:   filename    - Name of the bitmap to load
* Returns:      Pointer to loaded bitmap, or NULL on error.
*
* Description:  Attempts to load the bitmap file from disk and will
*               convert the bitmap file to the pixel format used by the
*               current display device context. By default all we do is
*               simply load a normal MGL bitmap. You should override this
*               routine to do extra bitmap conversion to handle your
*               particular application (such as converting to 15/16 bit
*               or scaling the bitmaps etc).
*
****************************************************************************/
{
    int             right,bottom,i;
    bitmap_t        *bmp;
    palette_t       orgPal[256];
    uchar           *p;

    // Stretch the bitmap to the device context which will also convert
    // to the device context pixel format at the same time.
    if ((bmp = MGL_loadBitmap(filename,true)) == NULL)
        return NULL;
    if (bmp->pal) {
        MGL_memcpy(orgPal,bmp->pal,256 * sizeof(palette_t));
        }
    else {
        MGL_getHalfTonePalette(orgPal);
        }
    MGL_setPalette(convertDC,orgPal,256,0);
    right = SCALEW(bmp->width);
    bottom = SCALEH(bmp->height);
    MGL_stretchBitmap(convertDC,0,0,right,bottom,bmp,MGL_REPLACE_MODE);

    // Free the old bitmap, and read the new bitmap from the device
    // context
    MGL_unloadBitmap(bmp);
    bmp = MGL_getBitmapFromDC(convertDC,0,0,right,bottom,false);
    if (!bmp)
        return NULL;

    // If this is an 8 bits per pixel bitmap, convert the bits to our
    // standard color palette, which is a simple 3:3:2 RGB palette (not
    // very nice but it works).
    if (bmp->bitsPerPixel == 8) {
        int         j,width = bmp->width,height = bmp->height;
        uchar       translate[256];
        palette_t   *pal = orgPal;
        uchar       *pstart = (uchar*)bmp->surface;

        // Build a translation vector for the bitmap palette.
        p = translate;
        for (i = 0; i < 256; i++, p++) {
            *p = (uchar)(((pal[i].blue  >> 0) & 0xE0) |
                         ((pal[i].green >> 3) & 0x1C) |
                         ((pal[i].red   >> 6) & 0x03));
            }

        // Remap transparent colors (marked as 255 in source bitmaps)
        // to 254 which we have reserved as our transparent color
        translate[255] = 254;

        // Translate all pixels in the bitmap surface
        for (j = height; j; j--, pstart += bmp->bytesPerLine) {
            for (p = pstart,i = width; i; i--, p++)
                *p = translate[*p];
            }
        }
    return bmp;
}

SPR_Bitmap *LoadBitmap(const char *fmt,int id,ibool isTransparent = true)
/****************************************************************************
*
* Function:     LoadBitmap
* Parameters:   fmt             - Format for filename
*               id              - Id used to build bitmap filename
*               isTransparent   - True if bitmap is transparent
* Returns:      Pointer to the loaded bitmap
*
* Description:  Loads a single bitmap from disk by creating the bitmap
*               filename and then loading the bitmap file.
*
****************************************************************************/
{
    char        filename[PM_MAX_PATH];
    bitmap_t    *bmp;

    sprintf(filename, fmt, id);
    bmp = loadAndConvertBitmap(filename);
    if (isTransparent)
        return SPR_mgrAddTransparentBitmap(sprMgr,bmp,transClr);
    else
        return SPR_mgrAddOpaqueBitmap(sprMgr,bmp);
}

int LoadBitmapList(int start,int end,const char *fmt)
/****************************************************************************
*
* Function:     LoadBitmapList
* Parameters:   start           - Start of bitmap list
*               end             - End of bitmap list
*               fmt             - Format for filenames
* Returns:      Index of last bitmap loaded
*
* Description:  Loads a list of transparent bitmaps from disk and stores
*               the pointers to the bitmaps in the global bitmap array.
*
****************************************************************************/
{
    int  i;

    for (i = start; i < end; i++)
        bitmapList[i] = LoadBitmap(fmt,i - start + 1);
    return i;
}

void LoadBitmaps()
/****************************************************************************
*
* Function:     LoadBitmaps
* Returns:      Pointer to array of all loaded bitmaps
*
* Description:  Allocates memory for an array of bitmap pointers and loads
*               all bitmaps.
*
****************************************************************************/
{
    int     i,n;
    ibool    isTransparent[C_TILETOTAL];
    FILE    *f;

    // Read list of transparent tile names
    memset(isTransparent,0,sizeof(isTransparent));
    if ((f = fopen(DATAPATH "trans.dat", "r")) == NULL)
        MGL_fatalError("Unable to open TRANS.DAT!");
    while (!feof(f)) {
        if (fscanf(f,"%d\n",&i) == 1)
            isTransparent[i-1] = true;
        }
    fclose(f);

    // Load all background, midground and foreground plane tiles
    for (i = 0; i < C_TILETOTAL; i++)
        bitmapList[i] = LoadBitmap(DATAPATH "%03ld.bmp",i + 1,isTransparent[i]);
    n = C_TILETOTAL;

    // Load all fox sprite bitmaps
    n = LoadBitmapList(n, n + C_FOXWALK,        DATAPATH "fw%02ldr.bmp");
    n = LoadBitmapList(n, n + C_FOXWALK,        DATAPATH "fw%02ldl.bmp");
    n = LoadBitmapList(n, n + C_FOXRUN,         DATAPATH "fr%02ldr.bmp");
    n = LoadBitmapList(n, n + C_FOXRUN,         DATAPATH "fr%02ldl.bmp");
    n = LoadBitmapList(n, n + C_FOXSTILL,       DATAPATH "fs%1ldr.bmp");
    n = LoadBitmapList(n, n + C_FOXSTILL,       DATAPATH "fs%1ldl.bmp");
    n = LoadBitmapList(n, n + C_FOXSTUNNED,     DATAPATH "fk%1ldr.bmp");
    n = LoadBitmapList(n, n + C_FOXSTUNNED,     DATAPATH "fk%1ldl.bmp");
    n = LoadBitmapList(n, n + C_FOXCROUCH,      DATAPATH "fc%1ldr.bmp");
    n = LoadBitmapList(n, n + C_FOXCROUCH,      DATAPATH "fc%1ldl.bmp");
    n = LoadBitmapList(n, n + C_FOXSTOP,        DATAPATH "fcd%1ldr.bmp");
    n = LoadBitmapList(n, n + C_FOXSTOP,        DATAPATH "fcd%1ldl.bmp");
    n = LoadBitmapList(n, n + C_FOXTHROW,       DATAPATH "ft%1ldr.bmp");
    n = LoadBitmapList(n, n + C_FOXTHROW,       DATAPATH "ft%1ldl.bmp");
    n = LoadBitmapList(n, n + C_FOXJUMPTHROW,   DATAPATH "fjt%1ldr.bmp");
    n = LoadBitmapList(n, n + C_FOXJUMPTHROW,   DATAPATH "fjt%1ldl.bmp");
    n = LoadBitmapList(n, n + C_FOXJUMP,        DATAPATH "fj%1ldr.bmp");
    n = LoadBitmapList(n, n + C_FOXJUMP,        DATAPATH "fj%1ldl.bmp");
    n = LoadBitmapList(n, n + C_FOXCROUCHWALK,  DATAPATH "fcw%02ldr.bmp");
    n = LoadBitmapList(n, n + C_FOXCROUCHWALK,  DATAPATH "fcw%02ldl.bmp");
    n = LoadBitmapList(n, n + C_FOXBLURR,       DATAPATH "fb%02ldr.bmp");
    n = LoadBitmapList(n, n + C_FOXBLURR,       DATAPATH "fb%02ldl.bmp");

    // Load all bear bitmaps
    n = LoadBitmapList(n, n + C_BEARMISS,       DATAPATH "bm%1ldl.bmp");
    n = LoadBitmapList(n, n + C_BEARSTRIKE,     DATAPATH "bs%02ldl.bmp");
        LoadBitmapList(n, n + C_BEARWALK,       DATAPATH "bw%02ldl.bmp");
}

ibool ActionKeyMap[6]; // {left, up, right, down, space, t}

void HandleMovement()
/****************************************************************************
*
* Function:     ProcessInput
* Returns:      True to contiue or false to quit demo
*
* Description:  Processes keyboard input and updates the state of all the
*               objects in the demo.
*
****************************************************************************/
{
    long            foxSpeedX = fox->VelocityX();
    FB_ACTION       foxAction = fox->Action();
    FB_DIRECTION    foxDir    = fox->Direction();

    // Nothing to do if fox is stopped or not active
    if (!fox->Active() || foxAction == FB_STOP)
        return;

    if (ActionKeyMap[3]) { /* down button is held */
        if (foxAction == FB_STILL)
                fox->SetAction(FB_CROUCH, FB_SAME);
            else if (foxAction == FB_WALK)
                fox->SetAction(FB_CROUCHWALK, FB_SAME);
        }
    if (ActionKeyMap[0]) { /* left button is held */
            if (foxSpeedX == 0) {
                if (foxDir == FB_RIGHT) {
                    fox->ChangeDirection();
                    foreground->SetSlideX(-C_BOUNDDIF, FB_RELATIVE);
                    midground->SetSlideX(-C_BOUNDDIF, FB_RELATIVE);
                    background->SetSlideX(-C_BOUNDDIF, FB_RELATIVE);
                    foreground->SetIncremX(C_BOUNDINCREM, FB_ABSOLUTE);
                    background->SetIncremX(C_BOUNDINCREM, FB_ABSOLUTE);
                    midground->SetIncremX(C_BOUNDINCREM, FB_ABSOLUTE);
                    }
                else if (foxAction == FB_STILL) {
                    fox->SetAction(FB_WALK, FB_LEFT);
                    fox->SetSwitch(C_FOX_WALKSWITCH, FB_ABSOLUTE);
                    fox->SetVelocityX(-C_FOX_XMOVE, FB_RELATIVE);
                    }
                else if (foxAction == FB_CROUCH) {
                    fox->SetAction(FB_CROUCHWALK, FB_LEFT);
                    fox->SetSwitch(C_FOX_WALKSWITCH, FB_ABSOLUTE);
                    fox->SetVelocityX(-C_FOX_XMOVE, FB_RELATIVE);
                    }
                else fox->SetVelocityX(-C_FOX_XMOVE, FB_RELATIVE);
                }
            else fox->SetVelocityX(-C_FOX_XMOVE, FB_RELATIVE);
        }
    if (ActionKeyMap[2]) {
            if (foxSpeedX == 0) {
                if (foxDir == FB_LEFT) {
                    fox->ChangeDirection();
                    foreground->SetSlideX(C_BOUNDDIF, FB_RELATIVE);
                    midground->SetSlideX(C_BOUNDDIF, FB_RELATIVE);
                    background->SetSlideX(C_BOUNDDIF, FB_RELATIVE);
                    foreground->SetIncremX(C_BOUNDINCREM, FB_ABSOLUTE);
                    background->SetIncremX(C_BOUNDINCREM, FB_ABSOLUTE);
                    midground->SetIncremX(C_BOUNDINCREM, FB_ABSOLUTE);
                    }
                else if (foxAction == FB_STILL) {
                    fox->SetAction(FB_WALK, FB_RIGHT );
                    fox->SetSwitch(C_FOX_WALKSWITCH, FB_ABSOLUTE );
                    fox->SetVelocityX(C_FOX_XMOVE, FB_RELATIVE );
                    }
                else if (foxAction == FB_CROUCH) {
                    fox->SetAction(FB_CROUCHWALK, FB_RIGHT );
                    fox->SetSwitch(C_FOX_WALKSWITCH, FB_ABSOLUTE );
                    fox->SetVelocityX(C_FOX_XMOVE, FB_RELATIVE );
                    }
                else fox->SetVelocityX(C_FOX_XMOVE, FB_RELATIVE);
                }
            else fox->SetVelocityX(C_FOX_XMOVE, FB_RELATIVE);
        }
    if (ActionKeyMap[1]) {
            if (foxAction == FB_CROUCH)
                fox->SetAction(FB_STILL, FB_SAME);
            else if (foxAction == FB_CROUCHWALK)
                fox->SetAction(FB_WALK, FB_SAME);
            ActionKeyMap[0] = false;
        }
    if (ActionKeyMap[5]) {
            if ((foxAction == FB_STILL) || (foxAction == FB_WALK)
                    || (foxAction == FB_RUN) || (foxAction == FB_CROUCH)
                    || (foxAction == FB_CROUCHWALK)) {
                fox->SetAction(FB_THROW, FB_SAME);
                fox->SetSwitch(C_FOX_THROWSWITCH, FB_ABSOLUTE);
                fox->SetVelocityX(0, FB_ABSOLUTE);
                fox->SetSwitchType(FB_TIME);
                }
            else if (foxAction == FB_JUMP) {
                fox->SetAccelerationY(0, FB_ABSOLUTE);
                fox->SetSwitch(C_FOX_THROWSWITCH, FB_ABSOLUTE);
                fox->SetAction(FB_JUMPTHROW, FB_SAME);
                fox->SetVelocityY(0, FB_ABSOLUTE);
                fox->SetSwitchDone(FALSE);
                fox->SetSwitchForward(TRUE);
                }
        }
    if (ActionKeyMap[4]) {
            if ((foxAction == FB_STILL) || (foxAction == FB_WALK)
                    || (foxAction == FB_RUN) || (foxAction == FB_CROUCH)
                    || (foxAction == FB_CROUCHWALK)) {
#ifdef  USE_DWSTK
                sound_PlaySound(sound_JUMP);
#endif
                fox->SetAction(FB_JUMP, FB_SAME);
                fox->SetSwitchType(FB_TIME);
                fox->SetSwitch(C_FOX_JUMPSWITCH, FB_ABSOLUTE);
                fox->SetVelocityY(-C_FOX_JUMPMOVE, FB_ABSOLUTE);
                fox->SetAccelerationY(C_UNIT / 2, FB_ABSOLUTE);
                }
        }
}

void ProcessKeyDownInput(char scanCode)
/****************************************************************************
*
* Function:     ProcessInput
* Returns:      True to contiue or false to quit demo
*
* Description:  Processes keyboard input and updates the state of all the
*               objects in the demo.
*
****************************************************************************/
{
    long            foxSpeedX = fox->VelocityX();
    FB_ACTION       foxAction = fox->Action();

    // Nothing to do if fox is stopped or not active
    if (!fox->Active() || foxAction == FB_STOP)
        return;

    switch (scanCode) {
        case KB_down:
            ActionKeyMap[3] = true;
            break;
        case KB_left:
            ActionKeyMap[0] = true;
            break;
        case KB_right:
            ActionKeyMap[2] = true;
            break;
        case KB_up:
            ActionKeyMap[1] = true;
            break;
        case KB_5:      // Stop
            if ((foxAction == FB_RUN) || (foxAction == FB_BLURR)) {
                fox->SetAction(FB_STOP, FB_SAME);
                fox->SetAccelerationX(-foxSpeedX / 25, FB_ABSOLUTE);
                }
            else fox->SetVelocityX(0, FB_ABSOLUTE);
            break;
        case KB_T:      // Throw
            ActionKeyMap[5] = true;
            break;
        case KB_space:  // Jump
            ActionKeyMap[4] = true;
            break;
        }
}

void ProcessKeyUpInput(char scanCode)
/****************************************************************************
*
* Function:     ProcessInput
* Returns:      True to contiue or false to quit demo
*
* Description:  Processes keyboard input and updates the state of all the
*               objects in the demo.
*
****************************************************************************/
{
    FB_ACTION       foxAction = fox->Action();

    // Nothing to do if fox is stopped or not active
    if (!fox->Active() || foxAction == FB_STOP)
        return;

    switch (scanCode) {
        case KB_down:
            ActionKeyMap[3] = false;
            break;
        case KB_left:
            ActionKeyMap[0] = false;
            break;
        case KB_right:
            ActionKeyMap[2] = false;
            break;
        case KB_up:
            ActionKeyMap[1] = false;
            break;
        case KB_T:      // Throw
            ActionKeyMap[5] = false;
            break;
        case KB_space:  // Jump
            ActionKeyMap[4] = false;
            break;
        }
}

void KeyUp(event_t *evt)
{
    ProcessKeyUpInput(EVT_scanCode(evt->message));
}

void KeyDown(event_t *evt)
/****************************************************************************
*
* Function:     KeyDown
* Parameters:   evt - Keyboard event to handle
*
* Description:  Handles keyboard events and passes them to ProcessInput.
*
****************************************************************************/
{
    switch (EVT_asciiCode(evt->message)) {
        case ASCII_esc:
            GM_exit();
            break;
        default:
            ProcessKeyDownInput(EVT_scanCode(evt->message));
            break;
        }

    switch (EVT_scanCode(evt->message)) {
        case KB_V:
            if (waitForRetrace == MGL_waitVRT)
                waitForRetrace = MGL_dontWait;
            else
                waitForRetrace = MGL_waitVRT;
            break;
#if 0
        // TODO: Add these back in again!
        case KB_S:
            if (!MGL_isWindowedDC(gm->dispdc)) {
                forceSysMem = !forceSysMem;
                SwitchFullscreen();
                }
            break;
        case KB_A:
            if (!MGL_isWindowedDC(gm->dispdc)) {
                driverOpt.useAccel = !driverOpt.useAccel;
                SwitchFullscreen();
                }
            break;
        case KB_D:
            if (!MGL_isWindowedDC(gm->dispdc)) {
                driverOpt.useWinDirect = !driverOpt.useWinDirect;
                driverOpt.useDirectDraw = !driverOpt.useWinDirect;
                SwitchFullscreen();
                }
            break;
        case KB_grayMinus:
            // Descrease resolution
            if (curMode > 0) {
                curMode--;
                restart = true;
                }
            break;
        case KB_grayPlus:
            // Increase resolution
            if (curMode < curMaxMode) {
                curMode++;
                restart = true;
                }
            break;
        case KB_grayDivide:
            // Decrease color depth
            if (curDepth > 0) {
                curDepth--;
                //ChangeColorDepth();
                restart = true;
                }
            break;
        case KB_grayTimes:
            // Increase color depth
            if (curDepth < curMaxDepth) {
                curDepth++;
                //ChangeColorDepth();
                restart = true;
                }
            break;
#endif
        }
}

void GameLogic()
/****************************************************************************
*
* Function:     GameLogic
*
* Description:  Update all object positions for the next frame
*
****************************************************************************/
{
    // Update sprite actions etc after processing input
    HandleMovement();

    UpdateFox();
    UpdateBear();
    UpdateApple();

    // Update sprite locations
    fox->SetX(0, FB_AUTOMATIC);
    fox->SetY(0, FB_AUTOMATIC);
    bear->SetX(0, FB_AUTOMATIC);
    apple->SetX(0, FB_AUTOMATIC);
    apple->SetY(0, FB_AUTOMATIC);

    // Update plane locations
    background->SetVelocityX(fox->VelocityX(), FB_ABSOLUTE);
    midground->SetVelocityX(fox->VelocityX(), FB_ABSOLUTE);
    foreground->SetVelocityX(fox->VelocityX(), FB_ABSOLUTE);
    background->SetX(0, FB_AUTOMATIC);
    midground->SetX(0, FB_AUTOMATIC);
    foreground->SetX(0, FB_AUTOMATIC);
}

void DrawFrame()
/****************************************************************************
*
* Function:     DrawFrame
*
* Description:  Update all object positions and draw the next frame
*
****************************************************************************/
{
    // Draw the current frame.
    MGL_setColor(bkgdColor);
    MGL_fillRectCoord(0,0,renderXRes,bkgdHeight);
    background->Display();
    midground->Display();
    foreground->Display();
    fox->Display(foreground->X());
    bear->Display(foreground->X());
    apple->Display(foreground->X());

    // Update the frames per second count if we have gone past at least
    // a quarter of a second since the last update.
    ulong newCount = LZTimerLap();
    frameCount++;
    if ((newCount - lastCount) > 100000L) {
        fpsRate = (int)(10000000L / (newCount - lastCount)) * frameCount;
        lastCount = newCount;
        frameCount = 0;
        }
    char buf[20];
    sprintf(buf,"%3d.%d fps", fpsRate / 10, fpsRate % 10);
    MGL_setColor(textColor);
    MGL_setTextJustify(MGL_CENTER_TEXT,MGL_TOP_TEXT);
    MGL_drawStrXY(renderXRes / 2,0,buf);
    MGL_setColor(textColor);
    MGL_setTextJustify(MGL_LEFT_TEXT,MGL_BOTTOM_TEXT);
    MGL_drawStrXY(0,renderYRes-1,modeName);

    // Swap the display buffers
    GM_swapBuffers(waitForRetrace);
}

void Begin()
/****************************************************************************
*
* Function:     Begin
* Parameters:   dc              - Display device context to use
*               useSystemBuffer - True if we should always render to sys mem
*               max8Bit         - True if maximum depth is 8 bit
*               SwapBuffers     - Pointer to buffer swap function
*
* Description:  Initialises the system by loading all bitmaps and building
*               all the scrolling planes and sprites required. If we
*               only have a single video page available, then we create
*               a memory device context for our output buffer and render
*               to that.
*
****************************************************************************/
{
    FILE            *f;
    palette_t       pal[256];
    int             bits;
    pixel_format_t  pf;

    // clear the input action status
    for (int i =0; i < 6; i++)
        ActionKeyMap[i] = false;

    // Load color palette from disk into global array
    if ((f = fopen(DATAPATH "palette.dat", "rb")) == NULL)
        MGL_fatalError("Unable to open palette data!");
    fread(pal,1,sizeof(pal),f);
    fclose(f);
    GM_setPalette(pal,256,0);
    GM_realizePalette(256,0,true);

    // Find the render buffer resolution
    renderXRes = MGL_sizex(gm->dc)+1;
    renderYRes = MGL_sizey(gm->dc)+1;

    // Get transparent color for demo
    if (MGL_getBitsPerPixel(gm->dc) == 8)
        transClr = 0xFE;
    else
        transClr = MGL_rgbColor(gm->dc,0xFF,0xFF,0xFF);
    MGL_setBackColor(transClr);

    // Build a string describing the video mode
    if (MGL_isWindowedDC(gm->dispdc)) {
        sprintf(modeName,"%dx%dx%d Windowed", MGL_sizex(gm->dc)+1, MGL_sizey(gm->dc)+1,
            MGL_getBitsPerPixel(gm->dc));
        }
    else {
        sprintf(modeName,"%dx%dx%d ", modeInfo.xRes, modeInfo.yRes, modeInfo.bitsPerPixel);
        if (forceSysMem)
            strcat(modeName,"SysMem");
        else
            strcat(modeName,MGL_modeDriverName(modeInfo.mode));
        }

    // Display loading data message on display
    font = MGL_loadFont("helv29bi.fnt");
    MGL_useFont(font);
    MGL_setTextJustify(MGL_CENTER_TEXT,MGL_CENTER_TEXT);
    MGL_setColor(textColor = MGL_realColor(gm->dc,255));
    MGL_drawStrXY(renderXRes / 2,(renderYRes - MGL_textHeight())/2,"Loading Data");
    MGL_drawStrXY(renderXRes / 2,(renderYRes + MGL_textHeight())/2,"Please wait...");

    // Display the ATI logo and artwork copyright notice
    bitmap_t *bmpart = MGL_loadBitmap(DATAPATH "ati.bmp",true);
    bitmap_t *bmpnote = MGL_loadBitmap(DATAPATH "atinote.bmp",true);
    MGLRect rArt(0,0,bmpart->width,bmpart->height);
    MGLRect rNote(0,0,bmpnote->width,bmpnote->height);
    if (MGL_sizex(gm->dc) < 360) {
        rArt.right() /= 2;
        rArt.bottom() /= 2;
        }
    MGL_checkIdentityPalette(true);
    if (MGL_getBitsPerPixel(gm->dc) > 8) {
        GM_setPalette(bmpart->pal,256,0);
        GM_realizePalette(256,0,false);
        }
    MGL_stretchBitmap(gm->dc,rArt.left(),rArt.top(),rArt.right(),rArt.bottom(),bmpart,MGL_REPLACE_MODE);
    if (MGL_getBitsPerPixel(gm->dc) > 8) {
        GM_setPalette(bmpnote->pal,256,0);
        GM_realizePalette(256,0,false);
        }
    if (MGL_sizex(gm->dc) > 360) {
        MGL_stretchBitmap(gm->dc,
            rNote.left()+rArt.right(),rNote.top(),
            rNote.right()+rArt.right(),rNote.bottom(),bmpnote,MGL_REPLACE_MODE);
        }
    MGL_checkIdentityPalette(false);
    MGL_setTextJustify(MGL_LEFT_TEXT,MGL_BOTTOM_TEXT);
    MGL_drawStrXY(0,renderYRes-1,modeName);
    GM_swapBuffers(MGL_waitVRT);

    // Create our bitmap manager and load all bitmaps
    if ((sprMgr = SPR_mgrInit(gm->dc)) == NULL)
        MGL_fatalError("Unable to initialize sprite manager!");

    // Create the memory device context for converting our bitmap data
    bits = MGL_getBitsPerPixel(gm->dc);
    MGL_getPixelFormat(gm->dc,&pf);
    if ((convertDC = MGL_createMemoryDC(SCALEX(BMP_MAXX+1),SCALEY(BMP_MAXY+1),
            bits,&pf)) == NULL)
        MGL_fatalError("Unable to create memory DC!");

    // Load all the bitmaps
    LoadBitmaps();

    // Create our scrolling planes
    foreground  = new FX_SurfacePlane(C_FORE_W, C_FORE_H, C_FORE_DENOM,
        DATAPATH "forelist.dat", bitmapList, DATAPATH "surflist.dat");
    midground   = new FX_Plane(C_MID_W, C_MID_H, C_MID_DENOM,
        DATAPATH "midlist.dat", bitmapList);
    // We are not using destination transparency, so we can optimise
    // the code to draw all solid bitmaps as solid rectangles which is
    // faster than a blt operation. Since nearly all the background
    // plane is one color, we draw the entire background plane as a
    // single rectangle.
    bkgdHeight = SCALEY(C_BACK_RECT_H * C_TILE_H);
    if (gm->dc->mi.bitsPerPixel > 8)
        bkgdColor = MGL_rgbColor(gm->dc,0,152,245);
    else
        bkgdColor = MGL_realColor(gm->dc,240);
    background  = new FX_Plane(C_BACK_W, C_BACK_H, C_BACK_DENOM,
        DATAPATH "backlst2.dat", bitmapList);

    // Create our sprites
    CreateFox();
    CreateBear();
    CreateApple();

#ifdef  USE_DWSTK
    // Start music right after sprites are created
    sound_PlaySound(sound_SONG);
#endif

    // Initialise the Zen Timer for precision timing and start it
    ZTimerInit();
    LZTimerOn();
    lastCount = 0;
    frameCount = 0;
    fpsRate = 0;

    // Reset the palette used for the main game logic
    GM_setPalette(pal,256,0);
    GM_realizePalette(256,0,true);
    initialized = true;
}

void End()
/****************************************************************************
*
* Function:     End
*
* Description:  Cleans up at the end by destroying all our structures
*
****************************************************************************/
{
    if (initialized) {
        // Turn off the Zen Timer
        LZTimerOff();

    #ifdef  USE_DWSTK
        // Kill all sounds
        sound_ClearAll();
    #endif

        // Destroy our sprites and planes
        delete fox;
        delete bear;
        delete apple;
        delete foreground;
        delete midground;
        delete background;

        // Destroy the sprite manager and device context for bitmap conversion
        SPR_mgrExit(sprMgr);
        MGL_destroyDC(convertDC);
        initialized = false;
        }
}

ibool PreModeSwitch(GM_modeInfo *,ibool)
/****************************************************************************
*
* Function:     PreModeSwitch
*
* Description:  Main callback to switch between fullscreen and windowed
*               graphics modes. This function is called by the game framework
*               when the user requests to switch to fullscreen modes either
*               by typing 'Alt-Enter' or by clicking on the maximum button
*               when in windowed modes.
*
****************************************************************************/
{
    End();
    return true;
}

void SwitchModes(GM_modeInfo *info,ibool)
/****************************************************************************
*
* Function:     SwitchModes
*
* Description:  Main callback to switch between fullscreen and windowed
*               graphics modes. This function is called by the game framework
*               when the user requests to switch to fullscreen modes either
*               by typing 'Alt-Enter' or by clicking on the maximum button
*               when in windowed modes.
*
****************************************************************************/
{
    modeInfo = *info;
    Begin();
}

int _ASMAPI SuspendApp(MGLDC*,int flags)
/****************************************************************************
*
* Function:     SuspendApp
* Parameters:   dc      - Active display DC to save/restore state from
*               flags   - Flags indicating what to do
* Returns:      True if we can switch, false if not.
*
* Description:  Re-downloads all the offscreen bitmap data to the hardware
*               when we switch back from GDI mode to our fullscreen app.
*
*****************************************************************************/
{
    if (flags == MGL_REACTIVATE) {
        // Nothing to do in here
        }
    return MGL_NO_SUSPEND_APP;
}

int main(int argc,char *argv[])
{
    ibool   startWindowed = false;

    /* Register the ISV license file if desired */
#ifdef  ISV_LICENSE
    GA_registerLicense(OemLicense,false);
#endif
    while (argc > 1) {
        if (stricmp(argv[1],"-nosnap") == 0)
            driverOpt.useSNAP = false;
        else if (stricmp(argv[1],"-usesysmem") == 0)
            forceSysMem = true;
        else if (argc >= 5 && stricmp(argv[1],"-mode") == 0) {
            modeInfo.xRes = atoi(argv[2]);
            modeInfo.yRes = atoi(argv[3]);
            modeInfo.bitsPerPixel = atoi(argv[4]);
            argc -= 3;
            argv += 3;
            }
#ifdef __CONSOLE__
        else {
            printf("Valid arguments are:\n");
            printf("    -nosnap\n");
            printf("    -nodirectdraw\n");
            printf("    -usesysmem\n");
            printf("    -usesysmem\n");
            printf("    -userect\n");
            printf("    -notransblt\n");
            printf("    -mode [xres] [yres] [bits]\n");
            exit(1);
            }
#endif
        argc--;
        argv++;
        }
    GM_setDriverOptions(&driverOpt);
    GM_initSysPalNoStatic(true);
    if ((gm = GM_init("FoxBear Demo")) == NULL)
        MGL_fatalError(MGL_errorMsg(MGL_result()));
    GM_setGameLogicFunc(GameLogic);
    GM_setDrawFunc(DrawFrame);
    GM_setKeyDownFunc(KeyDown);
    GM_setKeyRepeatFunc(KeyDown);
    GM_setKeyUpFunc(KeyUp);
    GM_setPreModeSwitchFunc(PreModeSwitch);
    GM_setModeSwitchFunc(SwitchModes);
    GM_setSuspendAppCallback(SuspendApp);
    GM_setExitFunc(End);

    /* Continue choosing modes until the user is done */
    while (GM_chooseMode(&modeInfo,&startWindowed)) {
        if (!GM_setMode(&modeInfo,startWindowed,2,forceSysMem))
            MGL_fatalError(MGL_errorMsg(MGL_result()));
        Begin();
        GM_mainLoop();
        }
    return 0;
}

