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
* Environment:  Any
*
* Description:  Simple game framework sprite demonstration program.
*
****************************************************************************/

#include "gm/gm.h"
#include "gm/sprite.h"
#include <string.h>
#include <stdlib.h>
#ifdef  ISV_LICENSE
#include "snap/graphics.h"
#include "isv.c"
#endif

/*---------------------- Macros and type definitions ----------------------*/

#ifdef __UNIX__
#define DATAPATH    "data/"
#define KNIGHTPATH  "knight/"
#else
#define DATAPATH    "data\\"
#define KNIGHTPATH  "knight\\"
#endif
#define TILESIZE    160 // Background tiles are 160x160
#define numButtons  1   // How many buttons on the left side
#define TASKS       5   // How many different tasks are available

// Different tasks the knight can do
enum {
    HOVER,
    UP,
    DOWN,
    LEFT,
    RIGHT
    };

/* Structure reprensenting the knight object. We use a triple pointer
 * to the sprite bitmap data for convience:
 *
 *  ***1) pointer to a sprite as normal
 *  **2) pointer to list of sprites for animation ie the walking down animation
 *  *3) pointer to list of lists of animations ie walking, jumping, etc
 *
 * This allows a pointer to a standing animation to have only one frame
 * while a walking animation will have two frames and a fighting animation
 * may have 20 frames.
 */
struct {
    SPR_bitmap  ***sprite;      // Pointers to frames of animation
    SPR_bitmap  *currentsprite; // Sprite being displayed
    bitmap_t    *forceField;    // Pointer to the bubble around the knight
    int         xSize,ySize;    // Size of the sprite
    int         xTile;          // Where he is and how fast he is going
    int         yTile;
    int         xTileOffset;
    int         yTileOffset;
    int         xSpeed;
    int         ySpeed;
    int         currentFrame;   // Which frame of the animation its in
    int         frameCount;     // How long its been doing what its been doing
    int         animateSpeed;   // How fast it should animate
    int         animateTime;    // How soon it should change frames
    int         doing;          // What the knight is doing, ie walking left, HOVERing still. etc
    int         frames[TASKS];  // Number of frames in each animation sequence
    } knight;

/* Structure representing a simple graphical button */
struct {
    SPR_bitmap  *picture[2];    // Picture of the button in the up and down position
    int         x,y;            // Coordinates of the button
    ibool       down;           // State of the button
    void        (MGLAPIP func)();// pointer to the function of what the button does
    } button[numButtons];

/*---------------------------- Global Variables ---------------------------*/

GMDC        *gm;                // The game framework device context
MGLDC       *convertDC;         // Memory DC for converting bitmaps
int         waitForRetrace = MGL_waitVRT;
ibool       forceSysMem = false;
color_t     transColor;
ibool       doClip = false;
int         xSpeed = 2;         // Speed at which to scroll the map
int         ySpeed = 2;
char        text[1000];         // A scratchpad for error messages
SPR_bitmap  *backgroundTile[2]; // Pointer to the background tiles
SPR_bitmap  *mousePointer[1];   // Pointer to mouse to be used
int         mapX = 0;           // Position on the map in terms of tiles
int         mapY = 0;
int         mapOffsetX = 0;
int         mapOffsetY = 0;
SPR_bitmapManager *mgr;

/* Game framework driver options */

GM_driverOptions driverOpt = {
    true,           /* UseSNAP          */
    false,          /* UseHWOpenGL      */
    MGL_GL_AUTO,    /* OpenGLType       */
    GM_MODE_16BPP | GM_MODE_24BPP | GM_MODE_32BPP   /* modeFlags        */
    };

/* My map scrolling works by saying which tile you're sitting on, but since
 * scrolling whole tiles at a time is a really weak way of doing things.
 * I also have a pixel level offset from the current tile so that we can
 * scroll pixel by pixel and look professional!
 */
int map[6][8]={
    {1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1}
    }; // the map!

/*------------------------------ Implementation ---------------------------*/

void SPR_drawTranslucent(
    bitmap_t *sourceSprite,
    int xPos,
    int yPos,
    color_t transColor)
{
    uchar   r1, r2, g1, g2, b1, b2;
    long    *screenPtr32;
    long    *spritePtr32;
    short   *screenPtr16;
    short   *spritePtr16;
    char    *screenPtr8;
    char    *spritePtr8;
    int     screenLength, spriteLength;
    int     x, y, screenOffset = 0, spriteOffset = 0;
    pixel_format_t pf;
    int     xStart = 0, xEnd, yStart = 0, yEnd;

    // Make sure the sprite is of the correct colour depth
    pf = gm->dc->pf;
    if (memcmp(sourceSprite->pf, &pf, sizeof(pixel_format_t)))
        MGL_fatalError("pixel formats do not match for SPR_drawTranslucent");
    screenLength = gm->dc->mi.xRes + 1;
    spriteLength = sourceSprite->width;

    // Clipping
    xEnd = sourceSprite->width;
    yEnd = sourceSprite->height;
    if (xPos < 0)
        xStart = 0 - xPos;
    if (yPos < 0)
        yStart = 0 - yPos;
    if (xPos + sourceSprite->width > gm->dc->mi.xRes)
        xEnd = (gm->dc->mi.xRes + 1) - xPos;
    if (yPos + sourceSprite->height > gm->dc->mi.yRes)
        yEnd = (gm->dc->mi.yRes + 1) - yPos;
    if (gm->dc->mi.bitsPerPixel == 24) {
        // Triple the horizontal length of everything since we use 3
        // separate bytes instead of a single color
        xEnd++;
        xEnd *= 3;
        xEnd--;
        spriteLength *= 3;
        screenLength *= 3;
        }
    for (y = 0; y < yStart; y++) {
        spriteOffset += spriteLength;
        screenOffset += screenLength;
        }

    // Here is the acutal code
    if (gm->dc->mi.bitsPerPixel == 32) {
        // Compute the mem address of the screen where the top left of
        // the sprite goes
        MGL_beginDirectAccess();
        screenPtr32 = gm->dc->surface;
        screenPtr32 += xPos + screenLength * yPos;
        spritePtr32 = (long*)sourceSprite->surface;

        // For each pixel in the source sprite extract its red, green,
        // and blue values and do the same for the background it is drawing
        // over. Finally set the new color to the average of of the two
        // pixels
        for (y = yStart; y < yEnd; y++) {
            for (x = xStart; x < xEnd; x++) {
                if (*(spritePtr32 + x + spriteOffset) != transColor) {
                    MGL_unpackColor(&pf, *(spritePtr32 + x + spriteOffset), &r1, &g1, &b1);
                    MGL_unpackColor(&pf, *(screenPtr32 + x + screenOffset), &r2, &g2, &b2);
                    *(screenPtr32 + x + screenOffset) = MGL_packColorFast(&pf, (r1+r2)>>1, (g1+g2)>>1, (b1+b2)>>1);
                    }
                }
            spriteOffset += spriteLength;
            screenOffset += screenLength;
            }
        MGL_endDirectAccess();
        }
    else if (gm->dc->mi.bitsPerPixel == 24) {
        // Compute the mem address of the screen where the top left of the
        // sprite goes
        MGL_beginDirectAccess();
        screenPtr8 = gm->dc->surface;
        screenPtr8 += xPos + screenLength * yPos;
        spritePtr8 = sourceSprite->surface;

        // For each pixel in the source sprite extract its red, green,
        // and blue values and do the same for the background it is drawing
        // over. Finally set the new color to the average of of the two
        // pixels
        for (y = yStart; y < yEnd; y++) {
            for (x = xStart; x < xEnd; x++) {
                if ((*(spritePtr8 + x + spriteOffset)<<16) + (*(spritePtr8 + x + spriteOffset + 1)<<8) + *(spritePtr8 + x + spriteOffset + 2) != 0) {
                    *(screenPtr8 + x++ + screenOffset) = ((*(screenPtr8 + x + screenOffset)>>1) + (*(spritePtr8 + x + spriteOffset)>>1));
                    *(screenPtr8 + x++ + screenOffset) = ((*(screenPtr8 + x + screenOffset)>>1) + (*(spritePtr8 + x + spriteOffset)>>1));
                    *(screenPtr8 + x + screenOffset) = ((*(screenPtr8 + x + screenOffset)>>1) + (*(spritePtr8 + x + spriteOffset)>>1));
                    }
                else {
                    x += 2;
                    }
                }
            spriteOffset += spriteLength;
            screenOffset += screenLength;
            }
        MGL_endDirectAccess();
        }
    else if (gm->dc->mi.bitsPerPixel == 16 || gm->dc->mi.bitsPerPixel == 15) {
        // Compute the mem address of the screen where the top left of the
        // sprite goes
        MGL_beginDirectAccess();
        screenPtr16 = gm->dc->surface;
        screenPtr16 += xPos + screenLength * yPos;
        spritePtr16 = (short*)sourceSprite->surface;

        // For each pixel in the source sprite extract its red, green,
        // and blue values and do the same for the background it is drawing
        // over. Finally set the new color to the average of of the two
        // pixels
        for (y = yStart; y < yEnd; y++) {
            for (x = xStart; x < xEnd; x++) {
                if (*(spritePtr16 + x + spriteOffset) != transColor) {
                    MGL_unpackColor(&pf, *(spritePtr16 + x + spriteOffset), &r1, &g1, &b1);
                    MGL_unpackColor(&pf, *(screenPtr16 + x + screenOffset), &r2, &g2, &b2);
                    *(screenPtr16 + x + screenOffset) = MGL_packColorFast(&pf, (r1+r2)>>1, (g1+g2)>>1, (b1+b2)>>1);
                    }
                }
            spriteOffset += spriteLength;
            screenOffset += screenLength;
            }
        MGL_endDirectAccess();
        }
    else
        MGL_fatalError("invalid color depth");
}

/* This routine loads a 15, 16, 24, or 32bit colour bitmap and draws it
 * to a temp device then grabs the bitmap that is being "displayed" in
 * that device.  This allows any 15, 16, 24, or 32bit bitmap to be loaded
 * and the easilly be converted to the current colour depth.
 */
bitmap_t* LoadBMP(
    char *filename,
    ibool loadPalette)
{
    int             width,height,bitsPerPixel;
    pixel_format_t  pf;

    if (MGL_getBitmapSize(filename, &width, &height, &bitsPerPixel, &pf)) {
        if (!MGL_loadBitmapIntoDC(convertDC, filename, 0, 0, loadPalette)) {
            return NULL;
            }
        return MGL_getBitmapFromDC(convertDC,0,0,width,height,false);
        }
    sprintf(text,"Unable to load %s",filename);
    MGL_fatalError(text);
    return NULL;
}

// Initialisation function that sets up the internal's of the demo
// after a mode set.
void Begin(void)
{
    pixel_format_t  pf;
    int             bits;

    // Check to ensure we have enough colors
    if (MGL_getBitsPerPixel(gm->dc) < 15)
        MGL_fatalError("You must run this in at least 15bit color!");

    // Set the transparent color to match the mouse background
    transColor = MGL_rgbColor(gm->dc, 200, 0, 100);
    MGL_setBackColor(transColor);

    // Initialise the sprite manager
    if ((mgr = SPR_mgrInit(gm->dc)) == NULL)
        MGL_fatalError("Unable to initialize sprite manager!");

    // Create the memory device context for converting the bitmap data
    // to our screen style. 640x480 is big enough for any sprites I can
    // imagine in a game!
    bits = MGL_getBitsPerPixel(gm->dc);
    MGL_getPixelFormat(gm->dc,&pf);
    if (!(convertDC = MGL_createMemoryDC(640,480, bits, &pf)))
        MGL_fatalError("Unable to create memory DC!");

    // Load the knight force field bitmap
    if (!(knight.forceField = LoadBMP(DATAPATH "ball.bmp", false)))
        MGL_fatalError("unable to load and allocate space for knight's force field");

    // Add the mouse as a normal transparent sprite to the sprite manager
    if (!(mousePointer[0] = SPR_mgrAddTransparentBitmap(mgr,LoadBMP(DATAPATH "mouse.bmp", false),transColor)))
        MGL_fatalError("Unable to load mouse pointer");

    // Add the background tiles as solid bitmaps to the sprite manager
    if (!(backgroundTile[0] = SPR_mgrAddOpaqueBitmap(mgr,LoadBMP(DATAPATH "water.bmp", false))))
        MGL_fatalError("Unable to add water to sprite manager");
    if (!(backgroundTile[1] = SPR_mgrAddOpaqueBitmap(mgr,LoadBMP(DATAPATH "stone.bmp", false))))
        MGL_fatalError("Unable to add stone to sprite manager");

    // Initialise our exit button
    button[0].down = 0;
    button[0].x = 0;
    button[0].y = 0;
    button[0].func = GM_exit;

    // Add the button as a solid bitmap to the sprite manager
    if (!(button[0].picture[0] = SPR_mgrAddOpaqueBitmap(mgr,LoadBMP(DATAPATH "quitu.bmp", false))))
        MGL_fatalError("Unable to load Quit up button");
    if (!(button[0].picture[1] = SPR_mgrAddOpaqueBitmap(mgr,LoadBMP(DATAPATH "quitd.bmp", false))))
        MGL_fatalError("Unable to load Quit down button");

    // Create the knight object
    knight.xSize = 32;
    knight.ySize = 32;
    knight.xTile = 0;
    knight.yTile = 0;
    knight.xTileOffset = 0;
    knight.yTileOffset = 0;
    knight.xSpeed = 1;
    knight.ySpeed = 1;
    knight.currentFrame = 0;
    knight.frameCount = 0;
    knight.animateSpeed = 100; // how fast it should animate
    knight.animateTime = 1000; // how soon it should change frames
    knight.frames[HOVER] = 1;
    knight.frames[UP] = 2;
    knight.frames[DOWN] = 2;
    knight.frames[LEFT] = 2;
    knight.frames[RIGHT] = 2;
    transColor = MGL_rgbColor(gm->dc, 0, 0, 0);
    knight.sprite = (SPR_bitmap***)malloc(TASKS * sizeof(SPR_bitmap**));
    *(knight.sprite + HOVER) = (SPR_bitmap**)malloc(knight.frames[HOVER] * sizeof(SPR_bitmap*));
    *(knight.sprite + UP) = (SPR_bitmap**)malloc(knight.frames[UP] * sizeof(SPR_bitmap*));
    *(knight.sprite + DOWN) = (SPR_bitmap**)malloc(knight.frames[DOWN] * sizeof(SPR_bitmap*));
    *(knight.sprite + LEFT) = (SPR_bitmap**)malloc(knight.frames[LEFT] * sizeof(SPR_bitmap*));
    *(knight.sprite + RIGHT) = (SPR_bitmap**)malloc(knight.frames[RIGHT] * sizeof(SPR_bitmap*));
    if (!(*(*(knight.sprite + HOVER) + 0) = SPR_mgrAddTransparentBitmap(mgr,LoadBMP(KNIGHTPATH "knitss.bmp", false),transColor)))
        MGL_fatalError("Unable to load knight standing still");
    if (!(*(*(knight.sprite + UP) + 0) = SPR_mgrAddTransparentBitmap(mgr,LoadBMP(KNIGHTPATH "knitwu1.bmp", false),transColor)))
        MGL_fatalError("Unable to load knight walking up 1");
    if (!(*(*(knight.sprite + UP) + 1) = SPR_mgrAddTransparentBitmap(mgr,LoadBMP(KNIGHTPATH "knitwu2.bmp", false),transColor)))
        MGL_fatalError("Unable to load knight walking up 2");
    if (!(*(*(knight.sprite + DOWN) + 0) = SPR_mgrAddTransparentBitmap(mgr,LoadBMP(KNIGHTPATH "knitwd1.bmp", false),transColor)))
        MGL_fatalError("Unable to load knight walking down 1");
    if (!(*(*(knight.sprite + DOWN) + 1) = SPR_mgrAddTransparentBitmap(mgr,LoadBMP(KNIGHTPATH "knitwd2.bmp", false),transColor)))
        MGL_fatalError("Unable to load knight walking down 2");
    if (!(*(*(knight.sprite + LEFT) + 0) = SPR_mgrAddTransparentBitmap(mgr,LoadBMP(KNIGHTPATH "knitwl1.bmp", false),transColor)))
        MGL_fatalError("Unable to load knight walking left 1");
    if (!(*(*(knight.sprite + LEFT) + 1) = SPR_mgrAddTransparentBitmap(mgr,LoadBMP(KNIGHTPATH "knitwl2.bmp", false),transColor)))
        MGL_fatalError("Unable to load knight walking left 2");
    if (!(*(*(knight.sprite + RIGHT) + 0) = SPR_mgrAddTransparentBitmap(mgr,LoadBMP(KNIGHTPATH "knitwr1.bmp", false),transColor)))
        MGL_fatalError("Unable to load knight walking right 1");
    if (!(*(*(knight.sprite + RIGHT) + 1) = SPR_mgrAddTransparentBitmap(mgr,LoadBMP(KNIGHTPATH "knitwr2.bmp", false),transColor)))
        MGL_fatalError("Unable to load knight walking right 2");
    knight.doing = HOVER;
    knight.currentsprite = *(*(knight.sprite + knight.doing) + knight.currentFrame);
}

/* Cleans up and exits */
void End(void)
{
    MGL_unloadBitmap(knight.forceField);
    SPR_mgrExit(mgr);
    free(*(knight.sprite + HOVER));
    free(*(knight.sprite + UP));
    free(*(knight.sprite + DOWN));
    free(*(knight.sprite + LEFT));
    free(*(knight.sprite + RIGHT));
    free(knight.sprite);
}

/* Main draw callback handler that draws the current frame to the device
 * context and then swaps the display buffers. When the game is
 * minimized on the task bar in fullscreen modes this function will not
 * be called to avoid drawing to memory that we do not own.
 */
void Draw(void)
{
    int screenX, screenY,i;
    int tilesX,tilesY;
    point_t mousePos;
    rect_t  oldClipRect;

    // Conversion routine for when running on the desktop
    MS_getPos(&mousePos.x, &mousePos.y);
#if defined(__WINDOWS__) && !defined(__CONSOLE__)
// TODO! Fix this!
//  ScreenToClient(gm->mainWindow,(LPPOINT)&mousePos);
#endif

    // We need enough tiles to cover the whole screen, plus a one tile
    // border for when the user is "between" tiles.
    tilesY=MGL_maxy()/TILESIZE+2;
    tilesX=MGL_maxx()/TILESIZE+2;

    // Set clip rectangle to test sprite clipping
    if (doClip) {
        rect_t clipRect;

        MGL_clearDevice();
        clipRect.left = MGL_maxx()/4;
        clipRect.right = 3*(MGL_maxx()/4);
        clipRect.top = MGL_maxy()/4;
        clipRect.bottom = 3*(MGL_maxy()/4);
        MGL_getClipRect(&oldClipRect);
        MGL_setClipRect(clipRect);
        }

    // Draw all the visible and partially visible tiles.
    for (screenY = 0; screenY < tilesY; screenY++)
        for (screenX = 0; screenX < tilesX; screenX++)
            SPR_draw(backgroundTile[map[screenY + mapY][screenX + mapX]], screenX * TILESIZE - mapOffsetX, screenY * TILESIZE - mapOffsetY);

    // Draw all the knight
    SPR_draw(knight.currentsprite, TILESIZE * (knight.xTile - mapX) + (knight.xTileOffset - mapOffsetX), TILESIZE * (knight.yTile - mapY) + (knight.yTileOffset - mapOffsetY));
    if (knight.doing == HOVER)
        SPR_drawTranslucent(knight.forceField,  TILESIZE * (knight.xTile - mapX) + (knight.xTileOffset - mapOffsetX) - 16, TILESIZE * (knight.yTile - mapY) + (knight.yTileOffset - mapOffsetY) - 16, 0);

    // Draw all the buttons down the left side
    for (i = 0; i < numButtons; i++)
        SPR_draw(button[i].picture[button[i].down],button[i].x,button[i].y);

    // Draw the mouse
    SPR_draw(mousePointer[0],mousePos.x,mousePos.y);

    // Reset clip rectangle
    if (doClip)
       MGL_setClipRect(oldClipRect);

    // Show the finished picture
    GM_swapBuffers(waitForRetrace);
}


/* Main game logic callback handler that lets you scroll around the map
 * on the screen. This function is called once per main loop by the
 * Game Framework, and you should do all non-drawing related game logic
 * type stuff in here. This is where you would do all your sound processing,
 * network processing etc before drawing the next frame.
 */
void GameLogic()
{
    point_t     mousePos;

    // Find the current mouse position
    MS_getPos(&mousePos.x, &mousePos.y);
#ifdef  __WINDOWS32__
// TODO: Fix this!
//  ScreenToClient(gm->mainWindow,&mousePos);
#endif

    // Do animation
    knight.frameCount += knight.animateSpeed;
    if (knight.frameCount > knight.animateTime) {
        knight.currentFrame++;
        if (knight.currentFrame >= knight.frames[knight.doing])
            knight.currentFrame = 0;
        knight.frameCount = 0;
        knight.currentsprite = *(*(knight.sprite + knight.doing) + knight.currentFrame);
        }

    // Move knight
    knight.xTileOffset += knight.xSpeed;
    knight.yTileOffset += knight.ySpeed;
    if (knight.xTileOffset >= TILESIZE) {
        knight.xTile++;
        knight.xTileOffset -= TILESIZE;
        }
    if (knight.xTileOffset < 0) {
        knight.xTile--;
        knight.xTileOffset += TILESIZE;
        }
    if (knight.yTileOffset >= TILESIZE) {
        knight.yTile++;
        knight.yTileOffset -= TILESIZE;
        }
    if (knight.yTileOffset < 0) {
        knight.yTile--;
        knight.yTileOffset += TILESIZE;
        }

    // Make knight "pace" around the screen
    if (knight.xTile * TILESIZE + knight.xTileOffset > 1248) {
        knight.xSpeed = 0;
        knight.ySpeed = 1;
        knight.xTileOffset--;
        knight.doing = DOWN;
        }
    if (knight.xTile * TILESIZE + knight.xTileOffset < 0) {
        knight.xSpeed = 0;
        knight.ySpeed = -1;
        knight.xTileOffset++;
        knight.doing = UP;
        }
    if (knight.yTile * TILESIZE + knight.yTileOffset > 288) {
        knight.xSpeed = -1;
        knight.ySpeed = 0;
        knight.yTileOffset--;
        knight.doing = LEFT;
        }
    if (knight.yTile * TILESIZE + knight.yTileOffset < 0) {
        knight.xSpeed = 1;
        knight.ySpeed = 0;
        knight.yTileOffset++;
        knight.doing = RIGHT;
        }

    // Scroll pixel at a time and once we get past the last pixel (159)
    // we move to the next tile and move the offset to zero
    if (EVT_isKeyDown(KB_right) || mousePos.x >= MGL_maxx()) {
        mapOffsetX+=xSpeed;
        if (mapOffsetX > 159) {
            mapX++;
            if (mapX > 8-(MGL_maxx()/TILESIZE+2)) {
                mapX--;
                mapOffsetX-=xSpeed;
                }
            else {
                mapOffsetX -= TILESIZE;
                }
            }
        }
    if (EVT_isKeyDown(KB_left) || mousePos.x <= 0) {
        mapOffsetX-=xSpeed;
        if (mapOffsetX < 0) {
            mapX--;
            if (mapX < 0) {
                mapX++;
                mapOffsetX+=xSpeed;
                }
            else {
                mapOffsetX += TILESIZE;
                }
            }
        }
    if (EVT_isKeyDown(KB_down) || mousePos.y >= MGL_maxy()) {
        mapOffsetY+=ySpeed;
        if (mapOffsetY > 159) {
            mapY++;
            if (mapY > 6-(MGL_maxy()/TILESIZE+2)) {
                mapY--;
                mapOffsetY -= ySpeed;
                }
            else {
                mapOffsetY -= TILESIZE;
                }
            }
        }
    if (EVT_isKeyDown(KB_up) || mousePos.y <= 0) {
        mapOffsetY-=ySpeed;
        if (mapOffsetY < 0) {
            mapY--;
            if (mapY < 0) {
                mapOffsetY+=ySpeed;
                mapY++;
                }
            else {
                mapOffsetY += TILESIZE;
                }
            }
        }
}

/* Main key down event handler to allow you to process key down events. The
 * Game Framework allows you to register key repeat and key up event handlers
 * as well (you can also use the same handler if you wish).
 */
void KeyDown(
    event_t *evt)
{
    switch (EVT_asciiCode(evt->message)) {
        case 'c':
        case 'C':
            doClip ^= 1;
            break;
        case 0x1B:
            GM_exit();
            break;
        }
}

/* Main mouse down event handler to allow you to process mouse down events.
 * The Game Framework allows you to register mouse up and mouse movement
 * event handlers as well (you can also use the same handler if you wish).
 */
void MouseDown(
    event_t *evt)
{
    int i;

    switch (evt->modifiers) {
        case EVT_LEFTBUT:
            // Only bother checking if a button is hit if the X pos is
            // correct
            if (evt->where_x < 64) {
                for (i = 0; i < numButtons; i++) {
                    if (evt->where_y >= 32*i && evt->where_y < 32*(i+1))
                        button[i].down = 1; // the user has clicked on button i
                    else
                        button[i].down = 0;
                    }
                }
            else {
                // All the buttons must be up
                for (i = 0; i < numButtons; i++)
                    button[i].down = 0;
                }
            break;
        }
}

void MouseUp(
    event_t *evt)
{
    point_t mousePos;
    int     i;

    mousePos.x = evt->where_x;
    mousePos.y = evt->where_y;
    switch (evt->message) {
        case EVT_LEFTBMASK:
            // Check for buttons being hit
            if (mousePos.x < 64) {
                for (i = 0; i < numButtons; i++) {
                    if (button[i].down) {
                        button[i].down = 0;
                        if (mousePos.y >= 32*i && mousePos.y < 32*(i+1))
                            button[0].func(); //don't do a mouse click action until the button goes up
                        break; // no sense test more buttons since you can only click buttons at a time
                        }
                    }
                }
            break;
        }
}

void MouseMove(
    event_t *evt)
{
    switch (evt->modifiers) {
        case EVT_LEFTBUT:
            MouseDown(evt); // incase you move the mouse while holding the button
            break;
        }
}

/* Main callback to switch between fullscreen and windowed
 * graphics modes. This function is called by the game framework
 * when the user requests to switch to fullscreen modes either
 * by typing 'Alt-Enter' or by clicking on the maximum button
 * when in windowed modes.
 */
ibool PreModeSwitch(
    GM_modeInfo *mode,
    ibool windowed)
{
    End();
    return true;
}

/* Main callback to switch between fullscreen and windowed graphics modes. This
 * function is called by the game framework when the user requests to switch to
 * fullscreen modes either by typing 'Alt-Enter' or by clicking on the maximum
 * button when in windowed modes.
 */
void SwitchModes(
    GM_modeInfo *mode,
    ibool windowed)
{
    Begin();
}

/* This function gets called when our application is activated and
 * de-activated. If you have other components in your game that need
 * to be disabled while your are not active (such as sound or CD-ROM
 * Audio) you should do it here.
 */
void ActivateApp(
    ibool active)
{
}

/* This function gets called when the user switches away from our app
 * back to the desktop via an Alt-Tab or some other activation key. This
 * function can be used to suspend stuff like CD-ROM Audio while suspended.
 * You can also return MGL_NO_DEACTIVATE from this function and the MGL
 * will ignore the request to switch back to the desktop (but make sure this
 * is an option that is off by default!).
 */
int MGLAPI SuspendApp(
    MGLDC *dc,
    int flags)
{
    if (flags == MGL_DEACTIVATE) {
        /* We are switching back to GDI mode, so put code in here to disable
         * stuff when switched away from fullscreen mode. Note that this
         * function may get called once to determine if the switch should occur,
         * and again when the switch actually happens.
         */
        }
    else if (flags == MGL_REACTIVATE) {
        /* We are now back in fullscreen mode, so put code in here to renable
         * stuff for fullscreen modes.
         */
        }
    return MGL_NO_SUSPEND_APP;
}

int main(int argc, char *argv[])
{
    ibool       startWindowed = false;
    int         pages = 2; //# of pages for buffering
    ibool       choose = false; //give the MGL choose gm mode screen to the user
    int         colorDepth = 16; //default color depth
    int         screenResX = 640,screenResY = 480; //preferred size of the screen
    GM_modeInfo info;

    /* Register the ISV license file if desired */
#ifdef  ISV_LICENSE
    GA_registerLicense(OemLicense,false);
#endif
    while (argc > 1) {
        if (stricmp(argv[1],"-nosnap") == 0)
            driverOpt.useSNAP = false;
        else if (stricmp(argv[1],"-usesysmem") == 0)
            forceSysMem = true;
        else if (stricmp(argv[1],"-noretrace") == 0)
            waitForRetrace = MGL_dontWait;
        else if (stricmp(argv[1],"-notfullscreen") == 0)
            startWindowed = true;
        else if (stricmp(argv[1],"-double") == 0)
            pages = 2;
        else if (stricmp(argv[1],"-triple") == 0)
            pages = 3;
        else if (stricmp(argv[1],"-choosemode") == 0)
            choose = true;
        else if (stricmp(argv[1],"-15bit") == 0)
            colorDepth = 15;
        else if (stricmp(argv[1],"-16bit") == 0)
            colorDepth = 16;
        else if (stricmp(argv[1],"-24bit") == 0)
            colorDepth = 24;
        else if (stricmp(argv[1],"-32bit") == 0)
            colorDepth = 32;
        else {
            printf("Accepted parameters are:\n");
            printf("    -novga\n");
            printf("    -nomodex\n");
            printf("    -nolfb\n");
            printf("    -novbeaf\n");
            printf("    -nodib\n");
            printf("    -nowindirect\n");
            printf("    -nodirectdraw\n");
            printf("    -norle\n");
            printf("    -usesysmem\n");
            printf("    -noretrace\n");
            printf("    -notfullscreen\n");
            printf("    -double\n");
            printf("    -triple\n");
            printf("    -choosemode\n");
            printf("    -15bit\n");
            printf("    -16bit\n");
            printf("    -24bit\n");
            printf("    -32bit\n");
            exit(1);
            }
        argc--;
        argv++;
        }

    GM_setDriverOptions(&driverOpt);
    if ((gm = GM_init("Simple sprite demo")) == NULL)
        MGL_fatalError(MGL_errorMsg(MGL_result()));
    GM_setGameLogicFunc(GameLogic);
    GM_setDrawFunc(Draw);
    GM_setKeyDownFunc(KeyDown);
    GM_setMouseDownFunc(MouseDown);
    GM_setMouseUpFunc(MouseUp);
    GM_setMouseMoveFunc(MouseMove);
    GM_setAppActivate(ActivateApp);
    GM_setPreModeSwitchFunc(PreModeSwitch);
    GM_setModeSwitchFunc(SwitchModes);
    GM_setSuspendAppCallback(SuspendApp);
    GM_setExitFunc(End);

    /* Search for 640x480x15bit as the default mode */
    if (choose) {
        if (!(GM_chooseMode(&info, &startWindowed)))
            return 1; //user clicked cancel
        }
    else {
        if (!GM_findMode(&info, screenResX, screenResY, colorDepth)) {
            sprintf(text,"Unable to find %ix%ix%ibit graphics mode!  Please get Display Doctor from http://www.scitechsoft.com/ ", screenResX, screenResY, colorDepth);
            MGL_fatalError(text);
            }
        }
    if (!GM_setMode(&info,startWindowed,pages,forceSysMem))
        MGL_fatalError(MGL_errorMsg(MGL_result()));

    Begin();
    GM_mainLoop();

    return 0;
}
