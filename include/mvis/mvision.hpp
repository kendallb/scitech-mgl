/****************************************************************************
*
*                    MegaVision Application Framework
*
*      A C++ GUI Toolkit for the SciTech Multi-platform Graphics Library
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
* Description:  General header file for the class library.
*
****************************************************************************/

#ifndef __MVIS_MVISION_HPP
#define __MVIS_MVISION_HPP

#ifndef __SCITECH_H
#include "scitech.h"
#endif

#ifndef __STDIO_H
#include <stdio.h>
#endif

#ifndef __MGRAPH_HPP
#include "mgraph.hpp"
#endif

#ifndef __MVIS_MKEYS_HPP
#include "mvis/mkeys.hpp"
#endif

#ifndef __MVIS_MRECT_HPP
#include "mvis/mrect.hpp"
#endif

#define MV_VERSION_STR "1.0"

/*------------------------------ Constants --------------------------------*/

// Standard user interface component system colors

enum {
    scInvalid,                      // Invalid system color index
    scDesktopBackground,            // Desktop pattern background color
    scDesktopForeground,            // Desktop pattern foregound color
    scMenuInterior,                 // Menu interior color
    scMenuSelectedInterior,         // Menu selected interior color
    scMenuText,                     // Menu normal text
    scMenuDisabledText,             // Menu disabled text
    scMenuSelectedText,             // Menu selected text
    scHighlight,                    // System highlight color
    scMediumLight,                  // System medium light color
    scShadow,                       // System shadow color
    scDarkShadow,                   // System dark shadow color
    scFocusRect,                    // System focus rectangle color
    scFrame,                        // Window frame color
    scBackground,                   // Window background color
    scClientBackground,             // Window client background color
    scActiveTitleBar,               // Active title bar background
    scPassiveTitleBar,              // Inactive title bar background
    scActiveTitleText,              // Active title bar text
    scPassiveTitleText,             // Inactive title bar text
    scTitleBarIcons,                // Title bar icon colors
    scScrollBarInterior,            // Scroll bar interior color
    scActiveScrollArrow,            // Active scroll bar arrow color
    scInactiveScrollArrow,          // Inactive scroll bar arrow color
    scUnselectScrollBar,            // Unselected scroll interior color
    scSelectScrollBar,              // Selected scroll bar interior color
    scButtonFace,                   // Normal button face color
    scPressedButtonFace,            // Pressed button face color
    scButtonText,                   // Normal button text color
    scFocusedButtonText,            // Focused button text color
    scDisabledButtonText,           // Disabled button text color
    scDefaultButtonBorder,          // Default button border color
    scStaticText,                   // Default dialog static text color
    scInputInterior,                // Input line interior color
    scInputText,                    // Input line normal text color
    scInputSelectedText,            // Input line selected text color
    scInputSelection,               // Input line selection color
    scInputCursor,                  // Input line cursor color
    scInputSelCursor,               // Input line cursor color (selected)
    scListInterior,                 // List box interior color
    scListCell,                     // List box normal cell color
    scListSelectedCell,             // List box selected cell color
    scListHighlightCell,            // List box highlighted cell color
    scListCursor,                   // List box cursor color
    scMaxSystemColor                // Maximum system color index+1
    };

// System glyph indexes

enum {
    sgClose,                        // Close box
    sgRestore,                      // Restore box
    sgMaximise,                     // Maximise box
    sgMinimise,                     // Minimise box
    sgRight,                        // Right arrow
    sgLeft,                         // Left arrow
    sgDown,                         // Down arrow
    sgUp,                           // Up arrow
    sgMaxSysGlyph                   // Maximum system glyph index+1
    };

// System bitmap indexes

enum {
    sbRadio,                        // Radio button
    sbRadioSel,                     // Radio button selected
    sbRadioDown,                    // Radio button down bitmap
    sbRadioDownSel,                 // Radio button down selected
    sbRadioDisable,                 // Radio button disabled
    sbRadioDisableSel,              // Radio button disabled selected
    sbCheck,                        // Check box
    sbCheckSel,                     // Check box selected
    sbCheckDown,                    // Check box down bitmap
    sbCheckDownSel,                 // Check box down selected
    sbCheckDisable,                 // Check box disabled
    sbCheckDisableSel,              // Check box disabled selected
    sbMaxSysBitmap                  // Maximum system bitmap index+1
    };

const uint

// Standard system palette indices (Normal style palette - 8 system colors)

    scBlack             = 0,
    scWhite             = 1,
    scLightGray         = 2,
    scDarkGray          = 3,
    scBlue              = 4,
    scJade              = 5,
    scYellow            = 6,
    scLightYellow       = 7,
    scUser              = 8,        // First user color index

// System palette indices (Windows style palette - bottom 8 system colors)

    scWinBlack          = 0,
    scWinDarkRed        = 1,
    scWinDarkGreen      = 2,
    scWinDarkYellow     = 3,
    scWinDarkBlue       = 4,
    scWinDarkMagenta    = 5,
    scWinDarkCyan       = 6,
    scWinLightGray      = 7,

// Codes for selecting either Normal or Windows system palettes

    ptNormalPalette     = 0,        // Normal palette (8 system colors)
    ptWindowsPalette    = 1,        // Windows palette (16 system colors)

// Standard command codes

    cmValid             = 0,        // Command to check if view is valid
    cmQuit              = 1,        // Command to kill the program
    cmError             = 2,        // Unsupported command message
    cmMenu              = 3,        // Starts a keyboard menu selection
    cmClose             = 4,        // Close the currently active window
    cmMove              = 5,        // Move the currently active window
    cmResize            = 6,        // Resize the currently active window
    cmMinimise          = 7,        // Minimise the current window
    cmMaximise          = 8,        // Maximise the current window
    cmNext              = 9,        // Move to next active window
    cmPrev              = 10,       // Move to previous active window
    cmHelp              = 11,       // Activate the help mechanism

// MVDialog standard commands

    cmOk                = 12,       // The OK button was pressed
    cmCancel            = 13,       // The Cancel button was pressed
    cmYes               = 14,       // The Yes button was pressed
    cmNo                = 15,       // The No button was pressed
    cmDefault           = 16,       // Default button selection (enter key)

// Standard application commands

    cmCut               = 21,       // Cut selected item to clipboard
    cmCopy              = 22,       // Copy selected item to clipboard
    cmPaste             = 23,       // Paste item from clipboard
    cmUndo              = 24,       // Undo last command
    cmRedo              = 25,       // Redo last command
    cmClear             = 26,       // Clear selected item

// MVScrollBar messages

    cmScrollBarChanged  = 50,       // Scroll bar thumb has moved
    cmScrollBarClicked  = 51,       // Part of scroll bar was activated

// MVListViewer messages

    cmListItemSelected  = 52,       // Item in list was selected
    cmListCursorChanged = 53,       // Cursor in list was moved

// MVFontManager error commands

    cmInvalidFont       = 256,      // Invalid font number used
    cmLowFontMem        = 257,      // Couldn't load font (no memory)
    cmBadFontFile       = 258,      // File loaded was not a font file
    cmFontNotFound      = 259,      // Font file was not found

// Standard system commands

    cmRestart           = 260,      // The application should re-start
    cmGrabModal         = 261,      // View has grabbed the modal view
    cmReleaseModal      = 262,      // View has released the model view
    cmReceivedFocus     = 263,      // View recieved the current focus
    cmReleasedFocus     = 264,      // View released the current focus
    cmCommandSetChanged = 265,      // Command set for the view changed
    cmRecordHistory     = 266,      // Views should record current history
    cmBeginEvents       = 267,      // About to begin event processing
    cmEndEvents         = 268,      // About to end event processing
    cmMoveToFront       = 269,      // Move window to the front and select

// MVButton messages

    cmGrabDefault       = 270,      // Button has become the default
    cmReleaseDefault    = 271,      // But is no longer the default
    cmButtonClicked     = 272,      // Button has been clicked
    cmRadioSelected     = 273,      // Radio button has been selected

// Standard file dialog commands

    cmFileOpen          = 274,      // Open button was hit
    cmFileSave          = 275,      // Save button was hit

// First user defined command

    cmUser              = 1000,     // User commands start here

// Pre-defined help context numbers

    hcNoContext         = 0;        // No help context relevant

// All icons that we deal with are 32x32 in size

#define MV_ICON_SIZE    32

/* Pass and extract points in a message */

#define MV_PASS_POINT(p)                        \
    (void *)(((ulong)p.y << 16) | (ushort)p.x)

#define MV_GET_POINT(l,p)                       \
    (p).x = (short)l;                           \
    (p).y = (short)((ulong)l >> 16);

/*-------------------------- Global Variables -----------------------------*/

extern MGLCursor        *MV_ARROW_CURSOR;
extern MGLCursor        *MV_WAIT_CURSOR;
extern MGLCursor        *MV_IBEAM_CURSOR;
extern MGLCursor        *MV_MOVE_CURSOR;
extern MGLCursor        *MV_SIZENESW_CURSOR;
extern MGLCursor        *MV_SIZENS_CURSOR;
extern MGLCursor        *MV_SIZENWSE_CURSOR;
extern MGLCursor        *MV_SIZEWE_CURSOR;
extern MGLCursor        *MV_currentCursor;
extern MGLIcon          *MV_ICON_WARNING;
extern MGLIcon          *MV_ICON_ERROR;
extern MGLIcon          *MV_ICON_INFORMATION;
extern MGLIcon          *MV_ICON_CONFIRM;
extern font_t           *MV_SYS_GLYPHS;
extern bitmap_t         *MV_SYS_BITMAPS;
extern MVRect           *MV_SYS_GLYPH_RECTS;
extern MVRect           *MV_SYS_BITMAP_RECTS;
extern int              _MV_sysScrollBarWidth;  // Default scroll bar width
extern int              _MV_systemFont;         // System font family
extern int              _MV_systemFontSize;     // System font size
extern int              _MV_iconOffset;         // Offset of icons in msgbox
extern MVHotKey         _MV_noHotKey;
extern MVRect           _MV_emptyRect;          // TRect(0,0,0,0)
extern MVRect           _MV_pointRect;          // TRect(0,0,1,1)

// System palette indices (Windows style palette - top 8 system colors)

extern uint             scWinDarkGray;
extern uint             scWinLightRed;
extern uint             scWinLightGreen;
extern uint             scWinLightYellow;
extern uint             scWinLightBlue;
extern uint             scWinLightMagenta;
extern uint             scWinLightCyan;
extern uint             scWinWhite;

/*-------------------------- Class definitions ----------------------------*/

//---------------------------------------------------------------------------
// Class for manipulating text justifications.
//---------------------------------------------------------------------------

class MVTextJust {
protected:
    int     horiz_just;         // Horizontal justification
    int     vert_just;          // Vertical justification

public:
            // Constructors
            MVTextJust() {};
            MVTextJust(int hJust,int vJust)
                { setJustification(hJust,vJust); };

            // Method to change the justification
            void setJustification(int hJust,int vJust)
                { horiz_just = hJust; vert_just = vJust; };

            // Method to use the current settings
            void use(MGLDevCtx dc) { dc.setTextJustify(horiz_just,vert_just); };

            // Method to save the current settings
            void save(MGLDevCtx dc) { dc.getTextJustify(horiz_just,vert_just); };

            // Method to return the justifications
            int hjust() { return horiz_just; };
            int vjust() { return vert_just; };
    };

/*------------------------- Function Prototypes ---------------------------*/

class MVView;

// Memory management routines

void MV_majorConsumer();
void MV_normalConsumer();
ibool MV_lowMemory();

// Miscellaneous stuff

void MV_loadSystemResources(MGLDevCtx& dc,int systemFont,int paletteType);
void MV_unloadSystemResources();
int MV_getSystemColor(int index);
void MV_setSystemColor(int index,int color);
MVView *MV_validView(MVView *view);
MVView *MV_message(MVView *view,ulong what,ulong command,void *info = NULL);
void MV_postMessage(MVView *view,ulong what,ulong command,void *info = NULL);
char *MV_newStr(const char *str);
char *MV_newHotStr(const char *str,char& hotChar,uint& hotIndex);
int MV_countLines(MGLDevCtx& dc,const char *msg,int maxWidth,int& maxTextWidth,ibool wrap);
void MV_beep(void);
void MV_pushViewport(MGLDevCtx& dc);
void MV_popViewport(MGLDevCtx& dc);
void MV_pushClipRect(MGLDevCtx& dc);
void MV_popClipRect(MGLDevCtx& dc);
void MV_getSysGlyphBounds(int index,MVRect& bounds);
void MV_putSysGlyph(int x,int y,int index);
void MV_putSysBitmap(MGLDC *dc,int x,int y,int index);
void MV_getSysBitmapBounds(int index,MVRect& bounds);
void MV_setBorderColors(color_t bright,color_t dark);
void MV_drawBorderCoord(int left,int top,int right,int bottom,int style,int thickness);
inline void MV_drawBorder(const MVRect& r,int style,int thickness)
{ MV_drawBorderCoord(r.left(),r.top(),r.right(),r.bottom(),style,thickness); }

typedef enum {
    MV_BDR_INSET,
    MV_BDR_OUTSET,
    MV_BDR_OUTLINE
    } MGL_bdrStyleType;

/*--------------------------- Inline functions ----------------------------*/

#ifdef  PC_HEADERS

// Pre-compile all of the headers used by the library

#include "mvis/mpoint.hpp"
#include "mvis/mevent.hpp"
#include "mvis/mview.hpp"
#include "mvis/mgroup.hpp"
#include "mvis/mframe.hpp"
#include "mvis/mwindow.hpp"
#include "mvis/mmenu.hpp"
#include "mvis/mmenubar.hpp"
#include "mvis/mbackgnd.hpp"
#include "mvis/mdesktop.hpp"
#include "mvis/mprogram.hpp"
#include "mvis/mbutton.hpp"
#include "mvis/mdialog.hpp"
#include "mvis/msttext.hpp"
#include "mvis/mfontmgr.hpp"
#include "mvis/minputl.hpp"
#include "mvis/mfiledlg.hpp"
#include "mvis/mscrollb.hpp"
#include "mvis/msgbox.hpp"

#endif  // PC_HEADERS

#endif  // __MVIS_MVISION_HPP
