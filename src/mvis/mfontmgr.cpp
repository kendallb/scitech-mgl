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
* Description:  Module to implement the loadable font manager routines.
*
****************************************************************************/

#include "mvis/mvision.hpp"
#include "mvis/mfontmgr.hpp"
#include "mvis/mprogram.hpp"
#include "pmapi.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef  __MSDOS__
#ifdef  __GNUC__
#define mode_t  _dj_mode_t
#include <io.h>
#undef  mode_t
#else
#include <io.h>
#endif
#endif

/*---------------------------- Global Variables ---------------------------*/

const int   MAXFONTS = 100;     // Maximum number of registered fonts

// Table of all currently registered font families

struct FontRegEntry {
    uint        family;         // Family of the font
    const char  *fileprefix;    // Filename prefix for the font
    };

FontRegEntry registeredFonts[MAXFONTS] = {
    { fmVecAstrology,           "astrolog"  },
    { fmVecCyrillic,            "cyrillic"  },
    { fmVecGothicEnglish,       "gotheng"   },
    { fmVecGothicGerman,        "gothger"   },
    { fmVecGothicItalian,       "gothita"   },
    { fmVecGreekComplex,        "greekc"    },
    { fmVecGreekComplexSmall,   "greekcs"   },
    { fmVecGreekSimplex,        "greeks"    },
    { fmVecItalicComplex,       "italicc"   },
    { fmVecItalicComplexSmall,  "italiccs"  },
    { fmVecItalicTriplex,       "italict"   },
    { fmVecJapanese,            "japanese"  },
    { fmVecLittle,              "little"    },
    { fmVecMathLarge,           "mathbig"   },
    { fmVecMathSmall,           "mathsmal"  },
    { fmVecMusical,             "musical"   },
    { fmVecRomanComplex,        "romanc"    },
    { fmVecRomanComplexSmall,   "romancs"   },
    { fmVecRomanDuplex,         "romand"    },
    { fmVecRomanSimplex,        "romans"    },
    { fmVecRomanTriplex,        "romant"    },
    { fmVecScriptComplex,       "scriptc"   },
    { fmVecScriptSimplex,       "scripts"   },
    { fmVecSymbol,              "symbol"    },

    { fmCharter,                "char"      },
    { fmCourier,                "cour"      },
    { fmHelvetica,              "helv"      },
    { fmLucidaBright,           "lucb"      },
    { fmLucidaSans,             "lucs"      },
    { fmLucidaTypewriter,       "luct"      },
    { fmNewCenturySchoolbook,   "ncen"      },
    { fmSymbol,                 "symb"      },
    { fmTimes,                  "tms"       },
    { fmFixed,                  "fix"       },
    { fmPC,                     "pc"        },

    { fmUserFont,               NULL        },
    };

MVFontManager   MV_fontManager;

/*----------------------------- Implementation ----------------------------*/

MVFontManager::MVFontManager()
/****************************************************************************
*
* Function:     MVFontManager::MVFontManager
*
* Description:  Constructor for the font manager class. Here we setup the
*               initial global variables etc, but do not load any fonts.
*               The system font is set to the built in 8x8 bitmap font,
*               and can be re-set to something else after the graphics
*               system has been initialised.
*
****************************************************************************/
{
    currentFont = systemFont = MVFontRecord(0xFFFF);
    lastRegistered = 0;
    while (registeredFonts[lastRegistered].family != fmUserFont)
        lastRegistered++;
    lastRegistered--;
    lastUserFont = fmUserFont-1;
    fonts = NULL;
    setMaximumFonts(fmDefaultMaximum);
}

MVFontManager::~MVFontManager()
/****************************************************************************
*
* Function:     MVFontManager::~MVFontManager
*
* Description:  Destructor for the font manager class. Purge all of the
*               currently loaded fonts and delete the font table.
*
****************************************************************************/
{
    shutDown();
    delete [] fonts;
}

void MVFontManager::shutDown()
/****************************************************************************
*
* Function:     MVFontManager::shutDown
*
* Description:  Shuts down the font manager, by resetting the system font
*               and purging all loaded fonts.
*
****************************************************************************/
{
    currentFont = systemFont = MVFontRecord(0xFFFF);
    for (int i = 0; i < maximumFonts; i++)
        if (fonts[i].fptr != NULL)
            purgeFont(fonts[i].rec);
}

void MVFontManager::setMaximumFonts(uint max)
/****************************************************************************
*
* Function:     MVFontManager::setMaximumFonts
* Parameters:   max - New maximum number of fonts
*
* Description:  Sets a new maximum number of loaded fonts. All the currently
*               loaded fonts will be purged, and will require reloading
*               again.
*
****************************************************************************/
{
    if (fonts) {
        while (purgeLRUFont() != -1)
            ;
        delete [] fonts;
        }

    fonts = new FontEntry[maximumFonts = max];

    for (int i = 0; i < maximumFonts; i++) {
        fonts[i].LRU = 0;
        fonts[i].fptr = NULL;
        }
}

const char *MVFontManager::fontFilename(const MVFontRecord& fontRec)
/****************************************************************************
*
* Function:     MVFontManager::fontFilename
* Parameters:   fontRec - Font record for the font
* Returns:      Pointer to the font's filename, or NULL if invalid.
*
* Description:  Builds the name of the font file to be loaded in a static
*               buffer. If the font record is invalid, we return NULL.
*
****************************************************************************/
{
    int i;

    for (i = 0; i <= lastRegistered; i++) {
        if (registeredFonts[i].family == fontRec.family)
            break;
        }
    if (i > lastRegistered)
        return NULL;

    // Build the name of the font file to load, from the font's file prefix
    // and the size and attributes of the font.
    static char name[PM_MAX_PATH];
    if (fontRec.sizey == -1) {
        sprintf(name,"%s%s",
            registeredFonts[i].fileprefix,
            FMGR_FONTEXT);
        }
    else if (fontRec.sizex == -1) {
        sprintf(name,"%s%d%s%s%s",
            registeredFonts[i].fileprefix,
            fontRec.sizey,
            (fontRec.attributes & fmBold) ? "b" : "",
            (fontRec.attributes & fmItalic) ? "i" : "",
            FMGR_FONTEXT);
        }
    else {
        sprintf(name,"%s%dx%d%s%s%s",
            registeredFonts[i].fileprefix,
            fontRec.sizex,
            fontRec.sizey,
            (fontRec.attributes & fmBold) ? "b" : "",
            (fontRec.attributes & fmItalic) ? "i" : "",
            FMGR_FONTEXT);
        }
    return name;
}

int MVFontManager::loadFont(const MVFontRecord& fontRec,ibool purgeable)
/****************************************************************************
*
* Function:     MVFontManager::loadFont
* Parameters:   fontRec     - Font record for font to load
*               purgeable   - True if font should be purgeable
* Returns:      Index of loaded font in font table, or -1 on error.
*
* Description:  Attempts to load the specified font. We first check the
*               font table to see if it is already loaded, otherwise we
*               attempt to load it.
*
****************************************************************************/
{
    int     i,freeSlot,errorCode;

    // Search the table of loaded fonts to see if it is already loaded,
    // also finding the first available slot.
    for (i = 0,freeSlot = maximumFonts; i < maximumFonts; i++) {
        if (fonts[i].fptr == NULL)
            freeSlot = MIN(i,freeSlot);
        else if (fonts[i].rec == fontRec) {
            if (fontRec != systemFont) {
                fonts[i].rec.attributes &= ~fmNonPurgeable;
                fonts[i].rec.attributes |= (purgeable ? 0 : fmNonPurgeable);
                }
            return i;
            }
        }

    // The font is not currently loaded, so attempt to load it in.
    const char *name = fontFilename(fontRec);
    if (name == NULL) {
        errorCode = cmInvalidFont;
        goto error;
        }

    if (freeSlot == maximumFonts)
        freeSlot = purgeLRUFont();
    fonts[freeSlot].rec = fontRec;
    fonts[freeSlot].rec.attributes &= ~fmNonPurgeable;
    fonts[freeSlot].rec.attributes |= (purgeable ? 0 : fmNonPurgeable);

    while (fonts[freeSlot].fptr == NULL) {
        fonts[freeSlot].fptr = MGL_loadFont(name);
        switch (MGL_result()) {
            case grLoadMem:
                if (purgeLRUFont() == -1) {
                    errorCode = cmLowFontMem;
                    goto error;
                    }
                break;
            case grBadFontFile:
                errorCode = cmBadFontFile;
                goto error;
            case grFontNotFound:
                errorCode = cmFontNotFound;
                goto error;
            }
        }
    return freeSlot;

error:
    MVEvent event;
    event.what = evCommand;
    event.message.infoPtr = NULL;
    event.message.command = errorCode;
    MVProgram::deskTop->putEvent(event);
    return -1;
}

font_t *MVFontManager::useFont(const MVFontRecord& fontRec)
/****************************************************************************
*
* Function:     MVFontManager::useFont
* Parameters:   fontRec - Font record of the font to use
* Returns:      Pointer to the font loaded and ready for use
*
* Description:  Attempts to use the specified font. This may fail for
*               a number of reasons:
*
*                   1. The font record is invalid
*                   2. There is not enough memory to load the font
*                   3. The font file does not exist
*
*               In either case, a cmInvalidFont or cmLowFontMem message
*               will be posted to the desktop, and the routine will
*               return false. The currently active font will be set to
*               the system font.
*
****************************************************************************/
{
    int             i,index;
    MVFontRecord    rec = fontRec;

    if (rec.family == fmSystemFont)
        rec = systemFont;

    if (rec != currentFont) {
        currentFont = rec;
        if ((index = loadFont(rec)) == -1)
            return useFont(fmSystemFont);
        CHECK(index < maximumFonts);
        currentFontp = fonts[index].fptr;
        fonts[index].LRU = 0xFFFF;
        }

    // Adjust the LRU counts for all loaded fonts.
    for (i = 0; i < maximumFonts; i++) {
        if (fonts[i].LRU-- == 0)
            fonts[i].LRU = 0;
        }
    return currentFontp;
}

void MVFontManager::purgeFont(const MVFontRecord& fontRec)
/****************************************************************************
*
* Function:     MVFontManager::purgeFont
* Parameters:   fontRec - Font record for the font to purge
*
* Description:  Forcibly purges a specified font. The system font cannot
*               be forcibly purged.
*
****************************************************************************/
{
    int i;

    if (fontRec.family == fmSystemFont || fontRec.family == 0xFFFF)
        return;

    for (i = 0; i < maximumFonts; i++) {
        if (fonts[i].rec == fontRec)
            break;
        }
    if (i == maximumFonts)
        return;

    if (fonts[i].rec == systemFont)
        return;

    MGL_unloadFont(fonts[i].fptr);
    fonts[i].fptr = NULL;
    fonts[i].LRU = 0;
    if (fontRec == currentFont)
        useFont(fmSystemFont);
}

int MVFontManager::purgeLRUFont()
/****************************************************************************
*
* Function:     MVFontManager::purgeLRUFont
* Returns:      Index of the purged font, or -1 if not more purgeable fonts.
*
* Description:  Finds and purges the least recently used font. If there
*               are no more purgeable fonts, we return -1.
*
****************************************************************************/
{
    int     index = -1;
    uint    minLRU = 0xFFFF;

    for (int i = 0; i < maximumFonts; i++) {
        if (fonts[i].fptr != NULL && !(fonts[i].rec.attributes & fmNonPurgeable)) {
            if (fonts[i].LRU < minLRU) {
                index = i;
                minLRU = fonts[i].LRU;
                }
            }
        }
    if (index != -1)
        purgeFont(fonts[index].rec);
    return index;
}

uint MVFontManager::registerFont(const char *fileprefix)
/****************************************************************************
*
* Function:     MVFontManager::registerFont
* Parameters:   fileprefix  - Filename prefix for the font file.
* Returns:      Font family for the registered font, 0xFFFF if no more room.
*
* Description:  Registers a user font family for use with the font manager.
*
****************************************************************************/
{
    if (++lastRegistered == MAXFONTS)
        return 0xFFFF;
    registeredFonts[lastRegistered].family = ++lastUserFont;
    registeredFonts[lastRegistered].fileprefix = fileprefix;
    return lastUserFont;
}

int MVFontManager::availableFont(const MVFontRecord& fontRec)
/****************************************************************************
*
* Function:     MVFontManager::availableFont
* Parameters:   fontRec - Font record for font to check for
* Returns:      0 if available, error code if not.
*
* Description:  Checks to see if the specified font is available for use.
*               Returns one of the following:
*
*                   0               - Font is available for use
*                   cmInvalidFont   - Font record is invalid
*                   cmFontNotFound  - Font file is unavailable
*
****************************************************************************/
{
    const char *name = fontFilename(fontRec);
    if (name == NULL)
        return cmInvalidFont;
    if (!MGL_availableFont(name))
        return cmFontNotFound;
    return 0;
}

ibool MVFontManager::setSystemFont(const MVFontRecord& fontRec)
/****************************************************************************
*
* Function:     MVFontManager::setSystemFont
* Parameters:   fontRec - Font record of the new system font
* Returns:      True if system font was changed, false if font number invalid
*
* Description:  Attempts the change the system font setting to a new font.
*               When the font manager starts, there is no default system
*               font loaded, so the system font must be initially loaded
*               (this is done in the TProgram class after graphics are
*               initialised).
*
****************************************************************************/
{
    int i;

    if (systemFont == fontRec)
        return true;

    for (i = 0; i < lastRegistered; i++)
        if (registeredFonts[i].family == fontRec.family)
            break;
    if (i != lastRegistered && availableFont(fontRec) == 0) {
        shutDown();
        if (loadFont(systemFont = fontRec,false) == -1)
            return false;
        useFont(systemFont);
        return true;
        }
    return false;
}
