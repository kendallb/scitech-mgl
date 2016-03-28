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
* Description:  Demo showing how to use the text functions to draw text to
*               the screen.
*
****************************************************************************/

#include "mglsamp.h"
#include "pmapi.h"
#include <string.h>
#include <stdlib.h>

/* MGL SDK example TrueType font paths */
#ifdef  __UNIX__
#define WIN2X       "win2x/"
#define TRUETYPE    "truetype/"
#else
#define WIN2X       "win2x\\"
#define TRUETYPE    "truetype\\"
#endif

char *FontLibname[] = {
WIN2X "verdana.ttf",
TRUETYPE "flange-l.ttf",   TRUETYPE "1stgrade.ttf",
TRUETYPE "1979rg__.ttf",   TRUETYPE "finalnew.ttf",
TRUETYPE "39smooth.ttf",   TRUETYPE "fletcher.ttf",
TRUETYPE "7hours.ttf",     TRUETYPE "flora_bo.ttf",
TRUETYPE "aarco1.ttf",     TRUETYPE "flora_no.ttf",
TRUETYPE "abbey_m1.ttf",   TRUETYPE "florenc_.ttf",
TRUETYPE "abbey_m4.ttf",   TRUETYPE "florence.ttf",
TRUETYPE "abbey_me.ttf",   TRUETYPE "foxtrot.ttf",
TRUETYPE "abctec04.ttf",   TRUETYPE "frankenf.ttf",
TRUETYPE "ac.ttf",         TRUETYPE "freehan1.ttf",
TRUETYPE "adlib.ttf",      TRUETYPE "freehand.ttf",
TRUETYPE "adlib_ex.ttf",   TRUETYPE "freeport.ttf",
TRUETYPE "adlib_th.ttf",   TRUETYPE "froman.ttf",
TRUETYPE "adlib_wd.ttf",   TRUETYPE "fundrunk.ttf",
TRUETYPE "advert.ttf",     TRUETYPE "fusion.ttf",
TRUETYPE "african.ttf",    TRUETYPE "fusion_n.ttf",
TRUETYPE "agate_no.ttf",   TRUETYPE "g_cut1.ttf",
TRUETYPE "aircut3.ttf",    TRUETYPE "galleria.ttf",
TRUETYPE "aleia.ttf",      TRUETYPE "garton.ttf",
TRUETYPE "alex_ant.ttf",   TRUETYPE "ges_____.ttf",
TRUETYPE "alexandr.ttf",   TRUETYPE "gismonda.ttf",
TRUETYPE "alfr_ho.ttf",    TRUETYPE "gocmp___.ttf",
TRUETYPE "alfredo_.ttf",   TRUETYPE "gordon__.ttf",
TRUETYPE "alison__.ttf",   TRUETYPE "gustavus.ttf",
TRUETYPE "allegro.ttf",    TRUETYPE "hancock.ttf",
TRUETYPE "allheart.ttf",   TRUETYPE "handel.ttf",
TRUETYPE "alpine.ttf",     TRUETYPE "hangem.ttf",
TRUETYPE "ambrosia.ttf",   TRUETYPE "harri.ttf",
TRUETYPE "ametz___.ttf",   TRUETYPE "hasty.ttf",
TRUETYPE "amtyp__n.ttf",   TRUETYPE "hbo.ttf",
TRUETYPE "angular.ttf",    TRUETYPE "helna.ttf",
TRUETYPE "annifont.ttf",   TRUETYPE "hobo_ho.ttf",
TRUETYPE "annstone.ttf",   TRUETYPE "hotshot.ttf",
TRUETYPE "anson.ttf",      TRUETYPE "howard.ttf",
TRUETYPE "ant883.ttf",     TRUETYPE "iglesia_.ttf",
TRUETYPE "antique_.ttf",   TRUETYPE "igloocap.ttf",
TRUETYPE "antiquer.ttf",   TRUETYPE "imperium.ttf",
TRUETYPE "aquilla_.ttf",   TRUETYPE "impress.ttf",
TRUETYPE "arabian_.ttf",   TRUETYPE "informal.ttf",
TRUETYPE "architex.ttf",   TRUETYPE "italian.ttf",
TRUETYPE "architic.ttf",   TRUETYPE "japan.ttf",
TRUETYPE "architxb.ttf",   TRUETYPE "jazz.ttf",
TRUETYPE "architxi.ttf",   TRUETYPE "jester.ttf",
TRUETYPE "architxt.ttf",   TRUETYPE "jobber.ttf",
TRUETYPE "archtura.ttf",   TRUETYPE "junipr.ttf",
TRUETYPE "arctic2.ttf",    TRUETYPE "kabel.ttf",
TRUETYPE "arenski.ttf",    TRUETYPE "kennon.ttf",
TRUETYPE "ariston1.ttf",   TRUETYPE "klingon.ttf",
TRUETYPE "arroy.ttf",      TRUETYPE "lansbury.ttf",
TRUETYPE "asimov.ttf",     TRUETYPE "leftycas.ttf",
TRUETYPE "atlainln.ttf",   TRUETYPE "lichtnr.ttf",
TRUETYPE "aubauer.ttf",    TRUETYPE "light_bl.ttf",
TRUETYPE "aukoloss.ttf",   TRUETYPE "lightou.ttf",
TRUETYPE "aurora.ttf",     TRUETYPE "lithogrb.ttf",
TRUETYPE "author.ttf",     TRUETYPE "livingby.ttf",
TRUETYPE "avatar.ttf",     TRUETYPE "lucas.ttf",
TRUETYPE "avion.ttf",      TRUETYPE "m.ttf",
TRUETYPE "badfilms.ttf",   TRUETYPE "machum12.ttf",
TRUETYPE "balleten.ttf",   TRUETYPE "magde.ttf",
TRUETYPE "balloon1.ttf",   TRUETYPE "mardigr1.ttf",
TRUETYPE "baltar01.ttf",   TRUETYPE "mariage4.ttf",
TRUETYPE "basque_l.ttf",   TRUETYPE "mazam.ttf",
TRUETYPE "bear.ttf",       TRUETYPE "memo_.ttf",
TRUETYPE "beatsvil.ttf",   TRUETYPE "mengrav2.ttf",
TRUETYPE "bebl____.ttf",   TRUETYPE "merlinn.ttf",
TRUETYPE "bebop.ttf",      TRUETYPE "metalord.ttf",
TRUETYPE "becker_m.ttf",   TRUETYPE "mfigaro.ttf",
TRUETYPE "bed870.ttf",     TRUETYPE "mickey.ttf",
TRUETYPE "benguia1.ttf",   TRUETYPE "microtie.ttf",
TRUETYPE "benjami1.ttf",   TRUETYPE "minya.ttf",
TRUETYPE "benqu.ttf",      TRUETYPE "mira____.ttf",
TRUETYPE "benqu_b.ttf",    TRUETYPE "mlsgd.ttf",
TRUETYPE "berns.ttf",      TRUETYPE "mrkflthn.ttf",
TRUETYPE "bert____.ttf",   TRUETYPE "mrkfltwd.ttf",
TRUETYPE "bisque.ttf",     TRUETYPE "mystic.ttf",
TRUETYPE "blackcha.ttf",   TRUETYPE "n_e_b.ttf",
TRUETYPE "blippo-h.ttf",   TRUETYPE "n_e_b_b.ttf",
TRUETYPE "bodidlyb.ttf",   TRUETYPE "nas966.ttf",
TRUETYPE "bolide.ttf",     TRUETYPE "neil____.ttf",
TRUETYPE "bonzai.ttf",     TRUETYPE "neuvares.ttf",
TRUETYPE "bookjack.ttf",   TRUETYPE "ninjp___.ttf",
TRUETYPE "boomeran.ttf",   TRUETYPE "nitemare.ttf",
TRUETYPE "boomi___.ttf",   TRUETYPE "no.ttf",
TRUETYPE "borzoire.ttf",   TRUETYPE "nocontro.ttf",
TRUETYPE "bridge.ttf",     TRUETYPE "nuancei.ttf",
TRUETYPE "brook.ttf",      TRUETYPE "oregndr.ttf",
TRUETYPE "bucephtt.ttf",   TRUETYPE "oregnwe.ttf",
TRUETYPE "budangul.ttf",   TRUETYPE "oxfrd.ttf",
TRUETYPE "caligula.ttf",   TRUETYPE "paganini.ttf",
TRUETYPE "calligra.ttf",   TRUETYPE "pares.ttf",
TRUETYPE "camberic.ttf",   TRUETYPE "pares_o.ttf",
TRUETYPE "camelot_.ttf",   TRUETYPE "pcrypt.ttf",
TRUETYPE "canaith.ttf",    TRUETYPE "pepl_exp.ttf",
TRUETYPE "caraway_.ttf",   TRUETYPE "pepl_o.ttf",
TRUETYPE "carleton.ttf",   TRUETYPE "peplum.ttf",
TRUETYPE "cascad__.ttf",   TRUETYPE "phaedri_.ttf",
TRUETYPE "casscond.ttf",   TRUETYPE "phoenix_.ttf",
TRUETYPE "chaucer.ttf",    TRUETYPE "pompeii.ttf",
TRUETYPE "child.ttf",      TRUETYPE "postanti.ttf",
TRUETYPE "chltc_b.ttf",    TRUETYPE "premi.ttf",
TRUETYPE "chm_____.ttf",   TRUETYPE "presents.ttf",
TRUETYPE "chopinop.ttf",   TRUETYPE "prsent.ttf",
TRUETYPE "ciagoth.ttf",    TRUETYPE "quill_s.ttf",
TRUETYPE "civitype.ttf",   TRUETYPE "rabear.ttf",
TRUETYPE "clarendo.ttf",   TRUETYPE "rechtman.ttf",
TRUETYPE "clearfa2.ttf",   TRUETYPE "redfont.ttf",
TRUETYPE "cloister.ttf",   TRUETYPE "reed.ttf",
TRUETYPE "cobb.ttf",       TRUETYPE "ren&stim.ttf",
TRUETYPE "coronet-.ttf",   TRUETYPE "revival_.ttf",
TRUETYPE "croisant.ttf",   TRUETYPE "rhyol___.ttf",
TRUETYPE "csd_chal.ttf",   TRUETYPE "richardm.ttf",
TRUETYPE "cuckoo.ttf",     TRUETYPE "roissy.ttf",
TRUETYPE "cuneifor.ttf",   TRUETYPE "roundinf.ttf",
TRUETYPE "dali____.ttf",   TRUETYPE "rsgraphi.ttf",
TRUETYPE "debussy.ttf",    TRUETYPE "rsjacks.ttf",
TRUETYPE "dicken.ttf",     TRUETYPE "sacc_o.ttf",
TRUETYPE "dingdong.ttf",   TRUETYPE "saccule.ttf",
TRUETYPE "dixon.ttf",      TRUETYPE "sadh_o.ttf",
TRUETYPE "domcasua.ttf",   TRUETYPE "sadhu.ttf",
TRUETYPE "dorcla__.ttf",   TRUETYPE "saff_too.ttf",
TRUETYPE "dow_____.ttf",   TRUETYPE "sanlite.ttf",
TRUETYPE "drg.ttf",        TRUETYPE "sketch.ttf",
TRUETYPE "dubiel.ttf",     TRUETYPE "slogan_n.ttf",
TRUETYPE "eastside.ttf",   TRUETYPE "squire.ttf",
TRUETYPE "eccentri.ttf",   TRUETYPE "steelwlf.ttf",
TRUETYPE "eckmann_.ttf",   TRUETYPE "stonehen.ttf",
TRUETYPE "edition_.ttf",   TRUETYPE "stymiel1.ttf",
TRUETYPE "eklekti3.ttf",   TRUETYPE "styrofoa.ttf",
TRUETYPE "ekletic.ttf",    TRUETYPE "terminat.ttf",
TRUETYPE "elastic.ttf",    TRUETYPE "tgscript.ttf",
TRUETYPE "eldbjtt.ttf",    TRUETYPE "thomas.ttf",
TRUETYPE "elzevier.ttf",   TRUETYPE "toonzscr.ttf",
TRUETYPE "english1.ttf",   TRUETYPE "tt0626m_.ttf",
TRUETYPE "english_.ttf",   TRUETYPE "tt0727m_.ttf",
TRUETYPE "englisht.ttf",   TRUETYPE "unicorn.ttf",
TRUETYPE "engross.ttf",    TRUETYPE "upwest.ttf",
TRUETYPE "enp_____.ttf",   TRUETYPE "vietfont.ttf",
TRUETYPE "erie_li1.ttf",   TRUETYPE "vikingn.ttf",
TRUETYPE "erie_lig.ttf",   TRUETYPE "vireofn.ttf",
TRUETYPE "eterna.ttf",     TRUETYPE "vivala.ttf",
TRUETYPE "ex1472.ttf",     TRUETYPE "vogue_no.ttf",
TRUETYPE "exlogo.ttf",     TRUETYPE "walrod__.ttf",
TRUETYPE "featherl.ttf",   TRUETYPE "westend.ttf",
TRUETYPE "fecm____.ttf",   TRUETYPE "wildwst.ttf",
TRUETYPE "feltmari.ttf",   TRUETYPE "wilhlmk.ttf",
TRUETYPE "feltmark.ttf",   TRUETYPE "zirkle.ttf",
TRUETYPE "fettefra.ttf",
    };
#define NUM_FontLibS   (sizeof(FontLibname) / sizeof(FontLibname[0]))

/*----------------------------- Global Variables --------------------------*/

char demoName[] = "truetype";

/*------------------------------ Implementation ---------------------------*/

#define MYRED MGL_rgbColor(dc, 255, 0, 0)
#define MYBLUE MGL_rgbColor(dc, 0, 0, 255)
#define MYGREEN MGL_rgbColor(dc, 0, 255, 0)
#define MYWHITE MGL_rgbColor(dc, 255, 255, 255)
#define MYBLACK MGL_rgbColor(dc, 0, 0, 0)

ibool textLibDump(MGLDC *dc)
/****************************************************************************
*
* Function:     textDump
* Parameters:   dc  - Device context
*
* Description:  Displays all the characters in each of the fonts in
*               the distribution.
*
****************************************************************************/
{
    int         i,j,maxx,pointSize = 24;
    font_t      *font;
    font_lib_t  *fontLib;
    char        buf[PM_MAX_PATH];

    mainWindow(dc,"Loading font...");
    maxx = MGL_maxx();
    for (i = 0; i < NUM_FontLibS; i++) {   /* For each available font      */
        /* Attempt to load the font file from disk. If it cannot be
         * found, then go onto the next one.
         */
        strcpy(buf,FontLibname[i]);
        if ((fontLib = MGL_openFontLib(buf)) == NULL)
            continue;
        if ((font = MGL_loadFontInstance(fontLib,pointSize,0,0,true)) == NULL) {
            MGL_closeFontLib(fontLib);
            continue;
            }
        switch (fontLib->fontLibType) {
            case MGL_BITMAPFONT_LIB:
                sprintf(buf,"Bitmap Font Library: %s - %d Point", fontLib->name, font->pointSize);
                break;
            case MGL_TRUETYPEFONT_LIB:
                sprintf(buf,"TrueType Font Library: %s - %d Point", fontLib->name, font->pointSize);
                break;
            case MGL_TYPE1FONT_LIB:
                sprintf(buf,"Type1 Font Library: %s - %d Point", fontLib->name, font->pointSize);
                break;
            }
        mainWindow(dc,buf);

        /* Display all characters on the screen */
        MGL_useFont(font);
        MGL_setFontBlendMode(MGL_AA_RGBBLEND);
        if (MGL_getBitsPerPixel(dc) <= 8) {
            MGL_setFontAntiAliasPalette(
                MGL_rgbColor(dc, 255, 255, 255),
                MGL_rgbColor(dc, 192, 192, 192),
                MGL_rgbColor(dc, 128, 128, 128),
                MGL_rgbColor(dc, 64, 64, 64),
                MGL_rgbColor(dc, 0, 0, 0));
            }
        MGL_setColor(MYWHITE);
        MGL_setBackColor(MYBLACK);
        buf[1] = '\0';
        for (j = ' '; j < 256; j++) {
            buf[0] = j;
            MGL_drawStr(buf);
            if (MGL_getX() + MGL_maxCharWidth() > maxx) {
                MGL_moveToCoord(0,MGL_getY() + MGL_textHeight());
                }
            }

        /* Unload the font from memory */
        MGL_useFont(defFont);
        MGL_unloadFont(font);
        MGL_closeFontLib(fontLib);
        if (!pause())
            return false;
        mainWindow(dc,"Loading font...");
        }
    defaultAttributes(dc);
    return true;
}

ibool textDir(MGLDC *dc)
/****************************************************************************
*
* Function:     textDir
* Parameters:   dc  - Device context
*
* Description:  Display text in varying directions on the screen.
*
****************************************************************************/
{
    int         maxx,maxy,i;
    int         horiz,vert;
    char        *str;
    font_t      *font = NULL;
    font_lib_t  *fontLib;
    char        buf[PM_MAX_PATH];
    ibool       cont = true;

    maxx = MGL_maxx();
    maxy = MGL_maxy();

    /* Search for the first font that exists on disk */
    for (i = 0; i < NUM_FontLibS; i++) {   /* For each available font      */
        strcpy(buf,FontLibname[i]);
        if ((fontLib = MGL_openFontLib(buf)) == NULL)
            continue;
        if ((font = MGL_loadFontInstance(fontLib,18,0,0,true)) == NULL) {
            MGL_closeFontLib(fontLib);
            continue;
            }
        break;
        }
    /* No fonts found on disk */
    if (font == NULL)
        return false;

    sprintf(buf,"Character set (%s)", font->name);
    MGL_clearDevice();
    mainWindow(dc,buf);

    MGL_useFont(font);
    for (horiz = MGL_LEFT_TEXT; horiz <= MGL_RIGHT_TEXT && cont; horiz++)
        for (vert = MGL_TOP_TEXT; vert <= MGL_BASELINE_TEXT; vert++) {
            MGL_clearDevice();
            mainWindow(dc,buf);

            MGL_useFont(font);
            MGL_setColor(MYRED);
            MGL_lineCoord(0,maxy/2,maxx,maxy/2);
            MGL_lineCoord(maxx/2,0,maxx/2,maxy);
            MGL_setTextJustify(MGL_LEFT_TEXT,MGL_TOP_TEXT);
            MGL_setTextDirection(MGL_RIGHT_DIR);
            MGL_setColor(MYGREEN);
            MGL_setFontBlendMode(MGL_AA_RGBBLEND);
            if (MGL_getBitsPerPixel(dc) <= 8) {
                MGL_setFontAntiAliasPalette(
                    MGL_rgbColor(dc, 0, 255, 0),
                    MGL_rgbColor(dc, 0, 192, 0),
                    MGL_rgbColor(dc, 0, 128, 0),
                    MGL_rgbColor(dc, 0, 64, 0),
                    MGL_rgbColor(dc, 0, 0, 0));
                }
            MGL_setBackColor(MYBLACK);

            MGL_moveToCoord(0,0);
            MGL_drawStr("horiz: ");
            switch (horiz) {
                case MGL_LEFT_TEXT:
                    str = "LEFT";
                    break;
                case MGL_CENTER_TEXT:
                    str = "CENTER";
                    break;
                case MGL_RIGHT_TEXT:
                    str = "RIGHT";
                    break;
                default:
                    str = "UNKNOWN";
                }
            MGL_drawStr(str);
            MGL_moveToCoord(0,MGL_textHeight());
            MGL_drawStr("vert:  ");
            switch (vert) {
                case MGL_TOP_TEXT:
                    str = "TOP";
                    break;
                case MGL_CENTER_TEXT:
                    str = "CENTER";
                    break;
                case MGL_BOTTOM_TEXT:
                    str = "BOTTOM";
                    break;
                case MGL_BASELINE_TEXT:
                    str = "BASELINE";
                    break;
                default:
                    str = "UNKNOWN";
                }
            MGL_drawStr(str);
            MGL_makeCurrentDC(dc);

            MGL_setTextJustify(horiz,vert);
            MGL_useFont(font);

            MGL_setColor(MYBLUE);
            MGL_lineCoord(0,maxy/2+MGL_textHeight()-1,maxx,maxy/2+MGL_textHeight()-1);
            MGL_lineCoord(maxx/2+MGL_textHeight()-1,0,maxx/2+MGL_textHeight()-1,maxy);

            MGL_setColor(MYWHITE);
            MGL_setBackColor(MYBLACK);

            if (MGL_getBitsPerPixel(dc) <= 8) {
                MGL_setFontAntiAliasPalette(
                    MGL_rgbColor(dc, 255, 255, 255),
                    MGL_rgbColor(dc, 192, 192, 192),
                    MGL_rgbColor(dc, 128, 128, 128),
                    MGL_rgbColor(dc, 64, 64, 64),
                    MGL_rgbColor(dc, 0, 0, 0));
                }

            MGL_setTextDirection(MGL_LEFT_DIR);
            MGL_drawStrXY(maxx/2,maxy/2,"This text goes left");

            MGL_setTextDirection(MGL_DOWN_DIR);
            MGL_drawStrXY(maxx/2,maxy/2,"This text goes down");

            MGL_setTextDirection(MGL_UP_DIR);
            MGL_drawStrXY(maxx/2,maxy/2,"This text goes up");

            MGL_setTextDirection(MGL_RIGHT_DIR);
            MGL_drawStrXY(maxx/2,maxy/2,"This text goes right");
            if (!pause()) {
                cont = false;
                break;
                }
            }

    /* Unload the font from memory */
    MGL_useFont(defFont);
    MGL_unloadFont(font);
    MGL_closeFontLib(fontLib);
    defaultAttributes(dc);
    return cont;
}

ibool textBlend(MGLDC *dc)
/****************************************************************************
*
* Function:     textBlend
* Parameters:   dc  - Device context
*
* Description:  Blend text on the screen.
*
****************************************************************************/
{
    int         maxx,maxy,i;
    font_t      *font = NULL;
    font_lib_t  *fontLib;
    char        buf[PM_MAX_PATH];

    maxx = MGL_maxx();
    maxy = MGL_maxy();

    /* Search for the first font that exists on disk */
    for (i = 0; i < NUM_FontLibS; i++) {   /* For each available font      */
        strcpy(buf,FontLibname[i]);
        if ((fontLib = MGL_openFontLib(buf)) == NULL)
            continue;
        if ((font = MGL_loadFontInstance(fontLib,22,0,0,true)) == NULL) {
            MGL_closeFontLib(fontLib);
            continue;
            }
        break;
        }
    /* No fonts found on disk */
    if (font == NULL)
        return false;

    sprintf(buf,"Character set (%s)", font->name);
    MGL_clearDevice();
    mainWindow(dc,buf);

    MGL_useFont(font);

    MGL_setTextJustify(MGL_LEFT_TEXT,MGL_TOP_TEXT);
    MGL_setTextDirection(MGL_RIGHT_DIR);
    MGL_setColor(MYGREEN);
    MGL_loadPNGIntoDC(dc, "cow-24.png", (maxx/2)-160, (maxy/2)-120, false);

    MGL_setTextJustify(MGL_CENTER_TEXT,MGL_CENTER_TEXT);
    MGL_setFontBlendMode(MGL_AA_RGBBLEND);

    if (MGL_getBitsPerPixel(dc) <= 8) {
        MGL_setFontAntiAliasPalette(
            MGL_rgbColor(dc, 0, 255, 0),
            MGL_rgbColor(dc, 0, 192, 0),
            MGL_rgbColor(dc, 0, 128, 0),
            MGL_rgbColor(dc, 0, 64, 0),
            MGL_rgbColor(dc, 0, 0, 0));
        }

    MGL_setColor(MGL_rgbColor(dc, 255, 255, 255));
    MGL_moveToCoord(maxx/2-1, (maxy/2)-(MGL_textHeight()*2)-1);
    MGL_drawStr("This is a test of anti-aliased font blending.");

    MGL_setColor(MGL_rgbColor(dc, 0, 100, 0));
    MGL_moveToCoord(maxx/2+1, (maxy/2)-(MGL_textHeight()*2)+1);
    MGL_drawStr("This is a test of anti-aliased font blending.");

    MGL_setColor(MGL_rgbColor(dc, 0, 220, 0));
    MGL_moveToCoord(maxx/2, (maxy/2)-(MGL_textHeight()*2));
    MGL_drawStr("This is a test of anti-aliased font blending.");


    MGL_setColor(MGL_rgbColor(dc, 255, 0, 255));
    MGL_moveToCoord(maxx/2, (maxy/2)-(MGL_textHeight()));

    MGL_drawStr("Note: this test will not draw with");
    MGL_setColor(MGL_rgbColor(dc, 128, 128, 128));
    MGL_moveToCoord(maxx/2, maxy/2);
    MGL_drawStr("blending in 8 bit or lower modes");


    MGL_setColor(MGL_rgbColor(dc, 255, 255, 255));
    MGL_moveToCoord(maxx/2, (maxy/2)+(MGL_textHeight()));
    MGL_drawStr("press any key to continue");

    MGL_setColor(MGL_rgbColor(dc, 128, 128, 255));
    MGL_moveToCoord(maxx/2-1, (maxy/2)+(MGL_textHeight()*2)-1);
    MGL_drawStr("**************");

    MGL_setColor(MGL_rgbColor(dc, 0, 0, 128));
    MGL_moveToCoord(maxx/2+1, (maxy/2)+(MGL_textHeight()*2)+1);
    MGL_drawStr("**************");

    MGL_setColor(MGL_rgbColor(dc, 0, 0, 0));
    MGL_moveToCoord(maxx/2, (maxy/2)+(MGL_textHeight()*2));
    MGL_drawStr("**************");

    pause();

    /* Unload the font from memory */
    MGL_useFont(defFont);
    MGL_unloadFont(font);
    MGL_closeFontLib(fontLib);
    defaultAttributes(dc);
    return true;
}

void demo(MGLDC *dc)
{
    if (!textBlend(dc)) return;
    if (!textDir(dc)) return;
    if (!textLibDump(dc)) return;
}
