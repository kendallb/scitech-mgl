/****************************************************************************
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
* Environment:  MSDOS
*
* Description:  Quick and dirty hack to process an input file listing
*               function names and to generate a table in RTF format that
*               can be imported into Word.
*
****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/*-------------------------- Implementation -------------------------------*/

const char *docHeader = "\
{\\rtf1\\ansi\\ansicpg1252\\uc1 \\deff0\\deflang1033\\deflangfe1033{\\fonttbl{\\f0\\froman\\fcharset0\\fprq2{\\*\\panose 02020603050405020304}Times New Roman;}{\\f1\\fswiss\\fcharset0\\fprq2{\\*\\panose 020b0604020202020204}Arial;}\n\
{\\f2\\fmodern\\fcharset0\\fprq1{\\*\\panose 02070309020205020404}Courier New;}{\\f3\\froman\\fcharset2\\fprq2{\\*\\panose 05050102010706020507}Symbol;}{\\f4\\froman\\fcharset0\\fprq2{\\*\\panose 02020603050405020304}Times;}\n\
{\\f9\\fswiss\\fcharset0\\fprq2{\\*\\panose 00000000000000000000}Helv{\\*\\falt Arial};}{\\f28\\froman\\fcharset0\\fprq2{\\*\\panose 02040602050305030304}Book Antiqua;}{\\f91\\froman\\fcharset238\\fprq2 Times New Roman CE;}\n\
{\\f92\\froman\\fcharset204\\fprq2 Times New Roman Cyr;}{\\f94\\froman\\fcharset161\\fprq2 Times New Roman Greek;}{\\f95\\froman\\fcharset162\\fprq2 Times New Roman Tur;}{\\f96\\froman\\fcharset177\\fprq2 Times New Roman (Hebrew);}\n\
{\\f97\\froman\\fcharset178\\fprq2 Times New Roman (Arabic);}{\\f98\\froman\\fcharset186\\fprq2 Times New Roman Baltic;}{\\f99\\fswiss\\fcharset238\\fprq2 Arial CE;}{\\f100\\fswiss\\fcharset204\\fprq2 Arial Cyr;}{\\f102\\fswiss\\fcharset161\\fprq2 Arial Greek;}\n\
{\\f103\\fswiss\\fcharset162\\fprq2 Arial Tur;}{\\f104\\fswiss\\fcharset177\\fprq2 Arial (Hebrew);}{\\f105\\fswiss\\fcharset178\\fprq2 Arial (Arabic);}{\\f106\\fswiss\\fcharset186\\fprq2 Arial Baltic;}{\\f107\\fmodern\\fcharset238\\fprq1 Courier New CE;}\n\
{\\f108\\fmodern\\fcharset204\\fprq1 Courier New Cyr;}{\\f110\\fmodern\\fcharset161\\fprq1 Courier New Greek;}{\\f111\\fmodern\\fcharset162\\fprq1 Courier New Tur;}{\\f112\\fmodern\\fcharset177\\fprq1 Courier New (Hebrew);}\n\
{\\f113\\fmodern\\fcharset178\\fprq1 Courier New (Arabic);}{\\f114\\fmodern\\fcharset186\\fprq1 Courier New Baltic;}{\\f123\\froman\\fcharset238\\fprq2 Times CE;}{\\f124\\froman\\fcharset204\\fprq2 Times Cyr;}{\\f126\\froman\\fcharset161\\fprq2 Times Greek;}\n\
{\\f127\\froman\\fcharset162\\fprq2 Times Tur;}{\\f128\\froman\\fcharset177\\fprq2 Times (Hebrew);}{\\f129\\froman\\fcharset178\\fprq2 Times (Arabic);}{\\f130\\froman\\fcharset186\\fprq2 Times Baltic;}{\\f315\\froman\\fcharset238\\fprq2 Book Antiqua CE;}\n\
{\\f316\\froman\\fcharset204\\fprq2 Book Antiqua Cyr;}{\\f318\\froman\\fcharset161\\fprq2 Book Antiqua Greek;}{\\f319\\froman\\fcharset162\\fprq2 Book Antiqua Tur;}{\\f322\\froman\\fcharset186\\fprq2 Book Antiqua Baltic;}}{\\colortbl;\\red0\\green0\\blue0;\n\
\\red0\\green0\\blue255;\\red0\\green255\\blue255;\\red0\\green255\\blue0;\\red255\\green0\\blue255;\\red255\\green0\\blue0;\\red255\\green255\\blue0;\\red255\\green255\\blue255;\\red0\\green0\\blue128;\\red0\\green128\\blue128;\\red0\\green128\\blue0;\\red128\\green0\\blue128;\n\
\\red128\\green0\\blue0;\\red128\\green128\\blue0;\\red128\\green128\\blue128;\\red192\\green192\\blue192;}{\\stylesheet{\\ql \\li0\\ri0\\widctlpar\\aspalpha\\aspnum\\faauto\\adjustright\\rin0\\lin0\\itap0 \\fs24\\lang1033\\langfe1033\\cgrid\\langnp1033\\langfenp1033 \\snext0 Normal;}{\n\
\\s1\\qr \\li0\\ri0\\sb480\\sa480\\keepn\\pagebb\\widctlpar\\brdrb\\brdrdb\\brdrw15\\brsp20 \\nooverflow\\faroman\\outlinelevel0\\rin0\\lin0\\itap0 \\i\\f1\\fs48\\lang1033\\langfe1033\\kerning28\\cgrid\\langnp1033\\langfenp1033 \\sbasedon0 \\snext0 heading 1;}{\n\
\\s2\\ql \\li0\\ri0\\sa220\\keepn\\widctlpar\\brdrb\\brdrs\\brdrw15\\brsp20 \\nooverflow\\faroman\\outlinelevel1\\rin0\\lin0\\itap0 \\f1\\fs28\\lang1033\\langfe1033\\cgrid\\langnp1033\\langfenp1033 \\sbasedon0 \\snext0 heading 2;}{\n\
\\s3\\ql \\li0\\ri0\\sa60\\keepn\\widctlpar\\nooverflow\\faroman\\outlinelevel2\\rin0\\lin0\\itap0 \\b\\f1\\fs24\\lang1033\\langfe1033\\cgrid\\langnp1033\\langfenp1033 \\sbasedon0 \\snext0 heading 3;}{\n\
\\s4\\ql \\li0\\ri0\\keepn\\widctlpar\\aspalpha\\aspnum\\faauto\\adjustright\\rin0\\lin0\\itap0 \\b\\i\\f1\\fs42\\lang1033\\langfe1033\\cgrid\\langnp1033\\langfenp1033 \\sbasedon0 \\snext0 heading 4;}{\n\
\\s5\\ql \\li0\\ri0\\keepn\\widctlpar\\aspalpha\\aspnum\\faauto\\adjustright\\rin0\\lin0\\itap0 \\b\\i\\f1\\fs24\\lang1033\\langfe1033\\cgrid\\langnp1033\\langfenp1033 \\sbasedon0 \\snext0 heading 5;}{\n\
\\s9\\qc \\li0\\ri0\\sa220\\keepn\\widctlpar\\nooverflow\\faroman\\outlinelevel8\\rin0\\lin0\\itap0 \\b\\f28\\fs28\\lang1033\\langfe1033\\cgrid\\langnp1033\\langfenp1033 \\sbasedon0 \\snext0 heading 9;}{\\*\\cs10 \\additive Default Paragraph Font;}{\n\
\\s15\\ql \\fi-220\\li440\\ri0\\widctlpar\\tqr\\tx3960\\nooverflow\\faroman\\rin0\\lin440\\itap0 \\fs18\\lang1033\\langfe1033\\cgrid\\langnp1033\\langfenp1033 \\sbasedon0 \\snext0 \\sautoupd index 2;}{\\s16\\ql \\li360\\ri0\\sa240\\widctlpar\\nooverflow\\faroman\\rin0\\lin360\\itap0\n\
\\f28\\fs22\\lang1033\\langfe1033\\cgrid\\langnp1033\\langfenp1033 \\sbasedon0 \\snext16 Body Text;}{\\s17\\ql \\fi-360\\li720\\ri0\\sa220\\widctlpar\\jclisttab\\tx720{\\*\\pn \\pnlvlbody\\ilvl0\\ls1\\pnrnot0\\pndec }\\nooverflow\\faroman\\ls1\\rin0\\lin720\\itap0\n\
\\f28\\fs22\\lang1033\\langfe1033\\cgrid\\langnp1033\\langfenp1033 \\sbasedon0 \\snext17 \\sautoupd List Bullet 2;}{\\s18\\ql \\fi-360\\li1080\\ri0\\sa220\\widctlpar\\jclisttab\\tx1080{\\*\\pn \\pnlvlbody\\ilvl0\\ls2\\pnrnot0\\pndec }\\nooverflow\\faroman\\ls2\\rin0\\lin1080\\itap0\n\
\\f28\\fs22\\lang1033\\langfe1033\\cgrid\\langnp1033\\langfenp1033 \\sbasedon0 \\snext18 \\sautoupd List Bullet 3;}{\\s19\\ql \\fi-360\\li1440\\ri0\\sa220\\widctlpar\\jclisttab\\tx1440{\\*\\pn \\pnlvlbody\\ilvl0\\ls3\\pnrnot0\\pndec }\\nooverflow\\faroman\\ls3\\rin0\\lin1440\\itap0\n\
\\f28\\fs22\\lang1033\\langfe1033\\cgrid\\langnp1033\\langfenp1033 \\sbasedon0 \\snext19 \\sautoupd List Bullet 4;}{\\s20\\ql \\fi-360\\li1800\\ri0\\sa220\\widctlpar\\jclisttab\\tx1800{\\*\\pn \\pnlvlbody\\ilvl0\\ls4\\pnrnot0\\pndec }\\nooverflow\\faroman\\ls4\\rin0\\lin1800\\itap0\n\
\\f28\\fs22\\lang1033\\langfe1033\\cgrid\\langnp1033\\langfenp1033 \\sbasedon0 \\snext20 \\sautoupd List Bullet 5;}{\\s21\\ql \\fi-360\\li360\\ri0\\sa220\\widctlpar\\jclisttab\\tx360{\\*\\pn \\pnlvlbody\\ilvl0\\ls5\\pnrnot0\\pndec }\\nooverflow\\faroman\\ls5\\rin0\\lin360\\itap0\n\
\\f28\\fs22\\lang1033\\langfe1033\\cgrid\\langnp1033\\langfenp1033 \\sbasedon0 \\snext21 List Number;}{\\s22\\ql \\fi-360\\li720\\ri0\\sa220\\widctlpar\\jclisttab\\tx720{\\*\\pn \\pnlvlbody\\ilvl0\\ls6\\pnrnot0\\pndec }\\nooverflow\\faroman\\ls6\\rin0\\lin720\\itap0\n\
\\f28\\fs22\\lang1033\\langfe1033\\cgrid\\langnp1033\\langfenp1033 \\sbasedon0 \\snext22 List Number 2;}{\\s23\\ql \\fi-360\\li1080\\ri0\\sa220\\widctlpar\\jclisttab\\tx1080{\\*\\pn \\pnlvlbody\\ilvl0\\ls7\\pnrnot0\\pndec }\\nooverflow\\faroman\\ls7\\rin0\\lin1080\\itap0\n\
\\f28\\fs22\\lang1033\\langfe1033\\cgrid\\langnp1033\\langfenp1033 \\sbasedon0 \\snext23 List Number 3;}{\\s24\\ql \\fi-360\\li1440\\ri0\\sa220\\widctlpar\\jclisttab\\tx1440{\\*\\pn \\pnlvlbody\\ilvl0\\ls8\\pnrnot0\\pndec }\\nooverflow\\faroman\\ls8\\rin0\\lin1440\\itap0\n\
\\f28\\fs22\\lang1033\\langfe1033\\cgrid\\langnp1033\\langfenp1033 \\sbasedon0 \\snext24 List Number 4;}{\\s25\\ql \\fi-360\\li1800\\ri0\\sa220\\widctlpar\\jclisttab\\tx1800{\\*\\pn \\pnlvlbody\\ilvl0\\ls9\\pnrnot0\\pndec }\\nooverflow\\faroman\\ls9\\rin0\\lin1800\\itap0\n\
\\f28\\fs22\\lang1033\\langfe1033\\cgrid\\langnp1033\\langfenp1033 \\sbasedon0 \\snext25 List Number 5;}{\\s26\\ql \\li0\\ri0\\widctlpar\\nooverflow\\faroman\\rin0\\lin0\\itap0 \\f28\\fs22\\lang1033\\langfe1033\\cgrid\\langnp1033\\langfenp1033 \\sbasedon0 \\snext26 SingleSpace;}{\n\
\\s27\\ql \\fi-220\\li1100\\ri0\\widctlpar\\tqr\\tx3960\\nooverflow\\faroman\\rin0\\lin1100\\itap0 \\fs18\\lang1033\\langfe1033\\cgrid\\langnp1033\\langfenp1033 \\sbasedon0 \\snext0 \\sautoupd index 5;}{\\s28\\ql \\fi-283\\li1003\\ri0\\widctlpar{\\*\\pn \\pnlvlblt\\ilvl10\\ls2047\\pnrnot0\n\
\\pnf4\\pnstart1\\pnindent283\\pnhang{\\pntxtb ?}}\\nooverflow\\faroman\\ls2047\\ilvl10\\rin0\\lin1003\\itap0 \\f28\\fs22\\lang1033\\langfe1033\\cgrid\\langnp1033\\langfenp1033 \\sbasedon0 \\snext28 \\sautoupd List Bullet;}{\n\
\\s29\\ql \\li0\\ri0\\widctlpar\\nooverflow\\faroman\\rin0\\lin0\\itap0 \\f1\\fs20\\lang1033\\langfe1033\\cgrid\\langnp1033\\langfenp1033 \\sbasedon0 \\snext29 AddressText;}{\\s30\\ql \\li360\\ri0\\widctlpar\\nooverflow\\faroman\\rin0\\lin360\\itap0\n\
\\b\\f1\\fs22\\lang1033\\langfe1033\\cgrid\\langnp1033\\langfenp1033 \\sbasedon0 \\snext16 Method Section;}{\\s31\\ql \\li0\\ri0\\widctlpar\\tqc\\tx4320\\tqr\\tx8208\\nooverflow\\faroman\\rin0\\lin0\\itap0 \\i\\f1\\fs22\\lang1033\\langfe1033\\cgrid\\langnp1033\\langfenp1033\n\
\\sbasedon0 \\snext31 EvenHeader;}{\\s32\\ql \\li0\\ri0\\sb120\\widctlpar\\tqr\\tx9000\\nooverflow\\faroman\\rin0\\lin0\\itap0 \\i\\f1\\fs22\\lang1033\\langfe1033\\cgrid\\langnp1033\\langfenp1033 \\sbasedon0 \\snext32 footer;}{\\s33\\qr \\li0\\ri0\\sb480\\sa480\\keepn\\pagebb\\widctlpar\n\
\\brdrb\\brdrdb\\brdrw15\\brsp20 \\nooverflow\\faroman\\outlinelevel1\\rin0\\lin0\\itap0 \\i\\f1\\fs48\\lang1033\\langfe1033\\kerning28\\cgrid\\langnp1033\\langfenp1033 \\sbasedon1 \\snext33 Func Heading 2;}{\\s34\\ql \\li0\\ri0\\widctlpar\n\
\\tqr\\tldot\\tx9000\\nooverflow\\faroman\\rin0\\lin0\\itap0 \\b\\f1\\fs24\\lang1024\\langfe1024\\cgrid\\noproof\\langnp1033\\langfenp1033 \\sbasedon0 \\snext0 \\sautoupd toc 1;}{\\s35\\ql \\li230\\ri0\\widctlpar\\tqr\\tldot\\tx9000\\nooverflow\\faroman\\rin0\\lin230\\itap0\n\
\\f28\\fs22\\lang1024\\langfe1024\\cgrid\\noproof\\langnp1033\\langfenp1033 \\sbasedon0 \\snext0 \\sautoupd toc 2;}{\\s36\\ql \\li446\\ri0\\widctlpar\\tqr\\tldot\\tx9000\\nooverflow\\faroman\\rin0\\lin446\\itap0\n\
\\i\\f28\\fs20\\lang1024\\langfe1024\\cgrid\\noproof\\langnp1033\\langfenp1033 \\sbasedon0 \\snext0 \\sautoupd toc 3;}{\\s37\\ql \\li0\\ri0\\sa40\\keepn\\widctlpar\\nooverflow\\faroman\\rin0\\lin0\\itap0 \\f28\\fs22\\lang1033\\langfe1033\\cgrid\\langnp1033\\langfenp1033\n\
\\sbasedon0 \\snext37 TableText;}{\\s38\\ql \\li360\\ri0\\sa240\\widctlpar\\nooverflow\\faroman\\rin0\\lin360\\itap0 \\v\\f28\\fs22\\lang1033\\langfe1033\\cgrid\\langnp1033\\langfenp1033 \\sbasedon16 \\snext38 Help Only Body Text;}{\n\
\\s39\\ql \\fi-1080\\li1080\\ri0\\sa240\\keepn\\pagebb\\widctlpar\\brdrt\\brdrs\\brdrw15\\brsp60 \\brdrl\\brdrs\\brdrw15\\brsp60 \\brdrb\\brdrs\\brdrw15\\brsp60 \\brdrr\\brdrs\\brdrw15\\brsp60 \\nooverflow\\faroman\\outlinelevel2\\rin0\\lin1080\\itap0 \\shading5000\n\
\\b\\f1\\fs28\\cf8\\lang1033\\langfe1033\\cgrid\\langnp1033\\langfenp1033 \\sbasedon0 \\snext16 Func Heading 3;}{\\s40\\ql \\fi-634\\li994\\ri0\\sa240\\keep\\nowidctlpar\\brdrt\\brdrs\\brdrw15\\brsp20 \\brdrb\\brdrs\\brdrw15\\brsp20\n\
\\tx1368\\tx1728\\tx2088\\tx2448\\nooverflow\\faroman\\rin0\\lin994\\itap0 \\i\\f28\\fs22\\lang1033\\langfe1033\\cgrid\\langnp1033\\langfenp1033 \\sbasedon0 \\snext0 Note;}{\\s41\\ql \\li360\\ri0\\sa240\\widctlpar\\nooverflow\\faroman\\rin0\\lin360\\itap0\n\
\\f2\\fs22\\cf1\\lang1033\\langfe1033\\cgrid\\langnp1033\\langfenp1033 \\sbasedon0 \\snext41 Preformatted;}{\\*\\cs42 \\additive \\ul\\cf2 \\sbasedon10 Hyperlink;}{\\s43\\ql \\fi-220\\li220\\ri0\\widctlpar\\tqr\\tx3960\\nooverflow\\faroman\\rin0\\lin220\\itap0\n\
\\fs18\\lang1033\\langfe1033\\cgrid\\langnp1033\\langfenp1033 \\sbasedon0 \\snext0 \\sautoupd index 1;}{\\s44\\ql \\li140\\ri0\\sb240\\sa120\\widctlpar\\nooverflow\\faroman\\rin0\\lin140\\itap0 \\b\\f1\\fs28\\lang1033\\langfe1033\\cgrid\\langnp1033\\langfenp1033 \\sbasedon0 \\snext43\n\
index heading;}{\\s45\\ql \\fi-360\\li1080\\ri0\\sa240\\widctlpar{\\*\\pn \\pnlvlbody\\ilvl0\\ls23\\pnrnot0\\pndec }\\nooverflow\\faroman\\ls23\\rin0\\lin1080\\itap0 \\f28\\fs22\\lang1033\\langfe1033\\cgrid\\langnp1033\\langfenp1033 \\sbasedon16 \\snext45 Numbered Text;}{\n\
\\s46\\ql \\fi-1080\\li1080\\ri0\\sa240\\keepn\\pagebb\\widctlpar\\brdrt\\brdrs\\brdrw15\\brsp60 \\brdrl\\brdrs\\brdrw15\\brsp60 \\brdrb\\brdrs\\brdrw15\\brsp60 \\brdrr\\brdrs\\brdrw15\\brsp60 \\nooverflow\\faroman\\outlinelevel2\\rin0\\lin1080\\itap0 \\shading5000\n\
\\b\\f1\\fs28\\cf8\\lang1033\\langfe1033\\cgrid\\langnp1033\\langfenp1033 \\sbasedon39 \\snext16 Func Heading 4;}{\\s47\\qr \\li0\\ri0\\sb480\\sa480\\keepn\\pagebb\\widctlpar\\brdrb\\brdrdb\\brdrw15\\brsp20 \\nooverflow\\faroman\\outlinelevel0\\rin0\\lin0\\itap0\n\
\\i\\v\\f1\\fs48\\lang1033\\langfe1033\\kerning28\\cgrid\\langnp1033\\langfenp1033 \\sbasedon1 \\snext47 Help Only Heading 1;}{\\s48\\qr \\li0\\ri0\\widctlpar\\nooverflow\\faroman\\rin0\\lin0\\itap0 \\i\\f1\\fs22\\lang1024\\langfe1024\\cgrid\\noproof\\langnp1033\\langfenp1033\n\
\\sbasedon0 \\snext48 OddHeader;}{\\*\\cs49 \\additive \\sbasedon10 page number;}{\\s50\\ql \\li0\\ri0\\widctlpar\\nooverflow\\faroman\\rin0\\lin0\\itap0 \\f28\\fs22\\lang1033\\langfe1033\\cgrid\\langnp1033\\langfenp1033 \\sbasedon0 \\snext16 Parameter;}{\n\
\\s51\\ql \\li0\\ri0\\widctlpar\\nooverflow\\faroman\\rin0\\lin0\\itap0 \\b\\i\\f28\\fs22\\lang1033\\langfe1033\\cgrid\\langnp1033\\langfenp1033 \\sbasedon0 \\snext16 Parameter Name;}{\\*\\cs52 \\additive \\i Parameter Names;}{\n\
\\s53\\ql \\li0\\ri0\\sa220\\widctlpar\\nooverflow\\faroman\\rin0\\lin0\\itap0 \\b\\f28\\fs22\\lang1033\\langfe1033\\cgrid\\langnp1033\\langfenp1033 \\sbasedon0 \\snext53 StrucTableHeading;}{\\s54\\ql \\li0\\ri0\\sa40\\keepn\\widctlpar\\nooverflow\\faroman\\rin0\\lin0\\itap0\n\
\\b\\f28\\fs22\\lang1033\\langfe1033\\cgrid\\langnp1033\\langfenp1033 \\sbasedon37 \\snext54 TableTextHeading;}{\\s55\\ql \\li662\\ri0\\widctlpar\\tqr\\tldot\\tx9000\\nooverflow\\faroman\\rin0\\lin662\\itap0 \\i\\f28\\fs20\\lang1024\\langfe1024\\cgrid\\noproof\\langnp1033\\langfenp1033\n\
\\sbasedon36 \\snext0 \\sautoupd toc 4;}{\\s56\\ql \\li454\\ri0\\sa220\\widctlpar\\tqr\\tldot\\tx9000\\nooverflow\\faroman\\rin0\\lin454\\itap0 \\i\\f28\\fs20\\lang1033\\langfe1033\\cgrid\\langnp1033\\langfenp1033 \\sbasedon0 \\snext0 \\sautoupd toc 5;}{\n\
\\s57\\qr \\li0\\ri0\\sb240\\sa720\\keep\\keepn\\pagebb\\widctlpar\\brdrb\\brdrs\\brdrw30\\brsp20 \\nooverflow\\faroman\\rin0\\lin0\\itap0 \\b\\f1\\fs36\\lang1033\\langfe1033\\cgrid\\langnp1033\\langfenp1033 \\sbasedon1 \\snext57 TOC Heading;}{\\*\\cs58 \\additive \\f2 Typedef Names;}{\n\
\\s59\\ql \\li0\\ri0\\widctlpar\\aspalpha\\aspnum\\faauto\\adjustright\\rin0\\lin0\\itap0 \\fs24\\lang1033\\langfe1033\\cgrid\\langnp1033\\langfenp1033 \\sbasedon0 \\snext16 Class Derivation;}{\\s60\\ql \\li0\\ri0\\sb120\\nowidctlpar\n\
\\tx360\\tx720\\tx1080\\tx1440\\faauto\\rin0\\lin0\\itap0 \\b\\i\\fs20\\lang1033\\langfe1033\\cgrid\\langnp1033\\langfenp1033 \\sbasedon0 \\snext16 Class Section;}{\\s61\\ql \\fi-360\\li360\\ri0\\sb120\\nowidctlpar\\tx360\\tx720\\tx1080\\tx1440\\faauto\\rin0\\lin360\\itap0\n\
\\fs20\\lang1033\\langfe1033\\cgrid\\langnp1033\\langfenp1033 \\sbasedon0 \\snext16 Bullet;}{\\s62\\ql \\li0\\ri0\\sb120\\nowidctlpar\\tx360\\tx720\\tx1080\\tx1440\\faauto\\rin0\\lin0\\itap0 \\f1\\fs20\\lang1033\\langfe1033\\cgrid\\langnp1033\\langfenp1033 \\sbasedon0 \\snext16\n\
Class Hierarchy;}{\\s63\\ql \\li0\\ri0\\sb120\\nowidctlpar\\tx360\\tx720\\tx1080\\tx1440\\faauto\\rin0\\lin0\\itap0 \\fs20\\lang1033\\langfe1033\\cgrid\\langnp1033\\langfenp1033 \\sbasedon0 \\snext16 Definition;}{\\s64\\ql \\li0\\ri0\\sb120\\nowidctlpar\n\
\\tx360\\tx720\\tx1080\\tx1440\\faauto\\rin0\\lin0\\itap0 \\fs20\\lang1033\\langfe1033\\cgrid\\langnp1033\\langfenp1033 \\sbasedon0 \\snext16 Enumeration Constant;}{\\s65\\ql \\li0\\ri0\\sb120\\nowidctlpar\\tx360\\tx720\\tx1080\\tx1440\\faauto\\rin0\\lin0\\itap0\n\
\\i\\fs30\\ul\\lang1033\\langfe1033\\cgrid\\langnp1033\\langfenp1033 \\sbasedon0 \\snext16 Glossary Word;}{\\s66\\ql \\li0\\ri0\\sb120\\nowidctlpar\\tx360\\tx720\\tx1080\\tx1440\\faauto\\rin0\\lin0\\itap0 \\b\\fs20\\lang1033\\langfe1033\\cgrid\\langnp1033\\langfenp1033\n\
\\sbasedon0 \\snext16 Include;}{\\s67\\ql \\li0\\ri0\\sb120\\nowidctlpar\\tx360\\tx720\\tx1080\\tx1440\\faauto\\rin0\\lin0\\itap0 \\b\\fs30\\lang1033\\langfe1033\\cgrid\\langnp1033\\langfenp1033 \\sbasedon0 \\snext16 IndexHead 1;}{\\s68\\ql \\li360\\ri0\\sb120\\nowidctlpar\n\
\\tx720\\tx1080\\tx1440\\tx1800\\faauto\\rin0\\lin360\\itap0 \\b\\fs20\\lang1033\\langfe1033\\cgrid\\langnp1033\\langfenp1033 \\sbasedon0 \\snext16 IndexHead 2;}{\\s69\\ql \\li720\\ri0\\sb120\\nowidctlpar\\tx1080\\tx1440\\tx1800\\tx2160\\faauto\\rin0\\lin720\\itap0\n\
\\b\\fs20\\lang1033\\langfe1033\\cgrid\\langnp1033\\langfenp1033 \\sbasedon0 \\snext16 IndexHead 3;}{\\s70\\ql \\li360\\ri0\\sb120\\nowidctlpar\\tx720\\tx1080\\tx1440\\tx1800\\faauto\\rin0\\lin360\\itap0 \\fs20\\lang1033\\langfe1033\\cgrid\\langnp1033\\langfenp1033\n\
\\sbasedon0 \\snext16 IndexItem 1;}{\\s71\\ql \\li720\\ri0\\sb120\\nowidctlpar\\tx1080\\tx1440\\tx1800\\tx2160\\faauto\\rin0\\lin720\\itap0 \\fs20\\lang1033\\langfe1033\\cgrid\\langnp1033\\langfenp1033 \\sbasedon0 \\snext16 IndexItem 2;}{\\s72\\ql \\li1080\\ri0\\sb120\\nowidctlpar\n\
\\tx1440\\tx1800\\tx2160\\tx2520\\faauto\\rin0\\lin1080\\itap0 \\fs20\\lang1033\\langfe1033\\cgrid\\langnp1033\\langfenp1033 \\sbasedon0 \\snext16 IndexItem 3;}{\\s73\\ql \\li0\\ri0\\sb120\\nowidctlpar\\tx360\\tx720\\tx1080\\tx1440\\faauto\\rin0\\lin0\\itap0\n\
\\b\\fs20\\ul\\lang1033\\langfe1033\\cgrid\\langnp1033\\langfenp1033 \\sbasedon0 \\snext16 Method Group;}{\\s74\\ql \\li0\\ri0\\sb120\\nowidctlpar\\tx360\\tx720\\tx1080\\tx1440\\faauto\\rin0\\lin0\\itap0 \\b\\fs20\\lang1033\\langfe1033\\cgrid\\langnp1033\\langfenp1033\n\
\\sbasedon0 \\snext16 Method Header;}{\\s75\\ql \\li0\\ri0\\sb120\\nowidctlpar\\tx360\\tx720\\tx1080\\tx1440\\faauto\\rin0\\lin0\\itap0 \\b\\i\\fs20\\lang1033\\langfe1033\\cgrid\\langnp1033\\langfenp1033 \\sbasedon0 \\snext16 Object Section;}{\n\
\\s76\\qc \\li0\\ri0\\sb120\\nowidctlpar\\faauto\\rin0\\lin0\\itap0 \\fs20\\lang1033\\langfe1033\\cgrid\\langnp1033\\langfenp1033 \\sbasedon0 \\snext16 Screen Shot;}{\\s77\\ql \\li0\\ri0\\sb120\\nowidctlpar\\tx360\\tx720\\tx1080\\tx1440\\faauto\\rin0\\lin0\\itap0\n\
\\fs20\\lang1033\\langfe1033\\cgrid\\langnp1033\\langfenp1033 \\sbasedon0 \\snext16 Structure Member;}{\\s78\\ql \\fi720\\li0\\ri0\\sb120\\nowidctlpar\\tx360\\tx720\\tx1080\\tx1440\\faauto\\rin0\\lin0\\itap0 \\fs20\\lang1033\\langfe1033\\cgrid\\langnp1033\\langfenp1033\n\
\\sbasedon0 \\snext16 Style;}{\\s79\\ql \\li0\\ri0\\sb120\\nowidctlpar\\tx360\\tx720\\tx1080\\tx1440\\faauto\\rin0\\lin0\\itap0 \\i\\fs20\\ul\\lang1033\\langfe1033\\cgrid\\langnp1033\\langfenp1033 \\sbasedon0 \\snext16 SubSubheading;}{\\s80\\ql \\li0\\ri0\\sb120\\nowidctlpar\n\
\\tx360\\tx720\\tx1080\\tx1440\\faauto\\rin0\\lin0\\itap0 \\b\\fs20\\lang1033\\langfe1033\\cgrid\\langnp1033\\langfenp1033 \\sbasedon0 \\snext16 Subheading;}{\\s81\\ql \\fi-1080\\li1080\\ri0\\sb120\\nowidctlpar\\tx1080\\tx1440\\tx1800\\tx2160\\faauto\\rin0\\lin1080\\itap0\n\
\\fs20\\cf9\\lang1033\\langfe1033\\cgrid\\langnp1033\\langfenp1033 \\sbasedon0 \\snext16 ToDo;}{\\*\\cs82 \\additive \\b\\f0 Class Names;}{\\*\\cs83 \\additive \\f0\\ul Variable Names;}{\\*\\cs84 \\additive \\f1 Function Names;}{\\*\\cs85 \\additive \\b\\f1 Enumeration Constants;}\n\
{\\*\\cs86 \\additive \\b\\f2 Structure Members;}}{\\*\\listtable{\\list\\listtemplateid152581844\\listsimple{\\listlevel\\levelnfc0\\levelnfcn0\\leveljc0\\leveljcn0\\levelfollow0\\levelstartat1\\levelspace0\\levelindent0{\\leveltext\\'02\\'00.;}{\\levelnumbers\\'01;}\\chbrdr\n\
\\brdrnone\\brdrcf1 \\chshdng0\\chcfpat1\\chcbpat1 \\s25\\fi-360\\li1800\\jclisttab\\tx1800 }{\\listname ;}\\listid-132}{\\list\\listtemplateid-1111967826\\listsimple{\\listlevel\\levelnfc0\\levelnfcn0\\leveljc0\\leveljcn0\\levelfollow0\\levelstartat1\\levelspace0\\levelindent0\n\
{\\leveltext\\'02\\'00.;}{\\levelnumbers\\'01;}\\chbrdr\\brdrnone\\brdrcf1 \\chshdng0\\chcfpat1\\chcbpat1 \\s24\\fi-360\\li1440\\jclisttab\\tx1440 }{\\listname ;}\\listid-131}{\\list\\listtemplateid-690590426\\listsimple{\\listlevel\\levelnfc0\\levelnfcn0\\leveljc0\\leveljcn0\n\
\\levelfollow0\\levelstartat1\\levelspace0\\levelindent0{\\leveltext\\'02\\'00.;}{\\levelnumbers\\'01;}\\chbrdr\\brdrnone\\brdrcf1 \\chshdng0\\chcfpat1\\chcbpat1 \\s23\\fi-360\\li1080\\jclisttab\\tx1080 }{\\listname ;}\\listid-130}{\\list\\listtemplateid231661234\\listsimple\n\
{\\listlevel\\levelnfc0\\levelnfcn0\\leveljc0\\leveljcn0\\levelfollow0\\levelstartat1\\levelspace0\\levelindent0{\\leveltext\\'02\\'00.;}{\\levelnumbers\\'01;}\\chbrdr\\brdrnone\\brdrcf1 \\chshdng0\\chcfpat1\\chcbpat1 \\s22\\fi-360\\li720\\jclisttab\\tx720 }{\\listname\n\
;}\\listid-129}{\\list\\listtemplateid1257803914\\listsimple{\\listlevel\\levelnfc23\\levelnfcn23\\leveljc0\\leveljcn0\\levelfollow0\\levelstartat1\\levelspace0\\levelindent0{\\leveltext\\'01\\u-3913 ?;}{\\levelnumbers;}\\f3\\chbrdr\\brdrnone\\brdrcf1\n\
\\chshdng0\\chcfpat1\\chcbpat1\\fbias0 \\s20\\fi-360\\li1800\\jclisttab\\tx1800 }{\\listname ;}\\listid-128}{\\list\\listtemplateid1475507504\\listsimple{\\listlevel\\levelnfc23\\levelnfcn23\\leveljc0\\leveljcn0\\levelfollow0\\levelstartat1\\levelspace0\\levelindent0{\\leveltext\n\
\\'01\\u-3913 ?;}{\\levelnumbers;}\\f3\\chbrdr\\brdrnone\\brdrcf1 \\chshdng0\\chcfpat1\\chcbpat1\\fbias0 \\s19\\fi-360\\li1440\\jclisttab\\tx1440 }{\\listname ;}\\listid-127}{\\list\\listtemplateid-580894488\\listsimple{\\listlevel\\levelnfc23\\levelnfcn23\\leveljc0\\leveljcn0\n\
\\levelfollow0\\levelstartat1\\levelspace0\\levelindent0{\\leveltext\\'01\\u-3913 ?;}{\\levelnumbers;}\\f3\\chbrdr\\brdrnone\\brdrcf1 \\chshdng0\\chcfpat1\\chcbpat1\\fbias0 \\s18\\fi-360\\li1080\\jclisttab\\tx1080 }{\\listname ;}\\listid-126}{\\list\\listtemplateid979509174\n\
\\listsimple{\\listlevel\\levelnfc23\\levelnfcn23\\leveljc0\\leveljcn0\\levelfollow0\\levelstartat1\\levelspace0\\levelindent0{\\leveltext\\'01\\u-3913 ?;}{\\levelnumbers;}\\f3\\chbrdr\\brdrnone\\brdrcf1 \\chshdng0\\chcfpat1\\chcbpat1\\fbias0 \\s17\\fi-360\\li720\\jclisttab\\tx720\n\
}{\\listname ;}\\listid-125}{\\list\\listtemplateid1887232722\\listsimple{\\listlevel\\levelnfc0\\levelnfcn0\\leveljc0\\leveljcn0\\levelfollow0\\levelstartat1\\levelspace0\\levelindent0{\\leveltext\\'02\\'00.;}{\\levelnumbers\\'01;}\\chbrdr\\brdrnone\\brdrcf1\n\
\\chshdng0\\chcfpat1\\chcbpat1 \\s21\\fi-360\\li360\\jclisttab\\tx360 }{\\listname ;}\\listid-120}{\\list\\listtemplateid172149842\\listsimple{\\listlevel\\levelnfc23\\levelnfcn23\\leveljc0\\leveljcn0\\levelfollow0\\levelstartat1\\levelspace0\\levelindent0{\\leveltext\n\
\\'01\\u-3913 ?;}{\\levelnumbers;}\\f3\\chbrdr\\brdrnone\\brdrcf1 \\chshdng0\\chcfpat1\\chcbpat1\\fbias0 \\fi-360\\li360\\jclisttab\\tx360 }{\\listname ;}\\listid-119}{\\list\\listtemplateid153660052\\listsimple{\\listlevel\\levelnfc0\\levelnfcn0\\leveljc0\\leveljcn0\\levelfollow0\n\
\\levelstartat0\\levelspace0\\levelindent0{\\leveltext\\'01*;}{\\levelnumbers;}\\chbrdr\\brdrnone\\brdrcf1 \\chshdng0\\chcfpat1\\chcbpat1 }{\\listname ;}\\listid-2}{\\list\\listtemplateid-1800895756\\listhybrid{\\listlevel\\levelnfc0\\levelnfcn0\\leveljc0\\leveljcn0\n\
\\levelfollow0\\levelstartat1\\levelspace360\\levelindent0{\\leveltext\\leveltemplateid67698703\\'02\\'00.;}{\\levelnumbers\\'01;}\\chbrdr\\brdrnone\\brdrcf1 \\chshdng0\\chcfpat1\\chcbpat1 \\fi-360\\li1080\\jclisttab\\tx1080 }{\\listlevel\\levelnfc4\\levelnfcn4\\leveljc0\n\
\\leveljcn0\\levelfollow0\\levelstartat1\\levelspace360\\levelindent0{\\leveltext\\leveltemplateid67698713\\'02\\'01.;}{\\levelnumbers\\'01;}\\chbrdr\\brdrnone\\brdrcf1 \\chshdng0\\chcfpat1\\chcbpat1 \\fi-360\\li1800\\jclisttab\\tx1800 }{\\listlevel\\levelnfc2\\levelnfcn2\n\
\\leveljc2\\leveljcn2\\levelfollow0\\levelstartat1\\levelspace360\\levelindent0{\\leveltext\\leveltemplateid67698715\\'02\\'02.;}{\\levelnumbers\\'01;}\\chbrdr\\brdrnone\\brdrcf1 \\chshdng0\\chcfpat1\\chcbpat1 \\fi-180\\li2520\\jclisttab\\tx2520 }{\\listlevel\\levelnfc0\n\
\\levelnfcn0\\leveljc0\\leveljcn0\\levelfollow0\\levelstartat1\\levelspace360\\levelindent0{\\leveltext\\leveltemplateid67698703\\'02\\'03.;}{\\levelnumbers\\'01;}\\chbrdr\\brdrnone\\brdrcf1 \\chshdng0\\chcfpat1\\chcbpat1 \\fi-360\\li3240\\jclisttab\\tx3240 }{\\listlevel\n\
\\levelnfc4\\levelnfcn4\\leveljc0\\leveljcn0\\levelfollow0\\levelstartat1\\levelspace360\\levelindent0{\\leveltext\\leveltemplateid67698713\\'02\\'04.;}{\\levelnumbers\\'01;}\\chbrdr\\brdrnone\\brdrcf1 \\chshdng0\\chcfpat1\\chcbpat1 \\fi-360\\li3960\\jclisttab\\tx3960 }\n\
{\\listlevel\\levelnfc2\\levelnfcn2\\leveljc2\\leveljcn2\\levelfollow0\\levelstartat1\\levelspace360\\levelindent0{\\leveltext\\leveltemplateid67698715\\'02\\'05.;}{\\levelnumbers\\'01;}\\chbrdr\\brdrnone\\brdrcf1 \\chshdng0\\chcfpat1\\chcbpat1 \\fi-180\\li4680\n\
\\jclisttab\\tx4680 }{\\listlevel\\levelnfc0\\levelnfcn0\\leveljc0\\leveljcn0\\levelfollow0\\levelstartat1\\levelspace360\\levelindent0{\\leveltext\\leveltemplateid67698703\\'02\\'06.;}{\\levelnumbers\\'01;}\\chbrdr\\brdrnone\\brdrcf1 \\chshdng0\\chcfpat1\\chcbpat1\n\
\\fi-360\\li5400\\jclisttab\\tx5400 }{\\listlevel\\levelnfc4\\levelnfcn4\\leveljc0\\leveljcn0\\levelfollow0\\levelstartat1\\levelspace360\\levelindent0{\\leveltext\\leveltemplateid67698713\\'02\\'07.;}{\\levelnumbers\\'01;}\\chbrdr\\brdrnone\\brdrcf1\n\
\\chshdng0\\chcfpat1\\chcbpat1 \\fi-360\\li6120\\jclisttab\\tx6120 }{\\listlevel\\levelnfc2\\levelnfcn2\\leveljc2\\leveljcn2\\levelfollow0\\levelstartat1\\levelspace360\\levelindent0{\\leveltext\\leveltemplateid67698715\\'02\\'08.;}{\\levelnumbers\\'01;}\\chbrdr\n\
\\brdrnone\\brdrcf1 \\chshdng0\\chcfpat1\\chcbpat1 \\fi-180\\li6840\\jclisttab\\tx6840 }{\\listname ;}\\listid39135856}{\\list\\listtemplateid-551676362\\listsimple{\\listlevel\\levelnfc0\\levelnfcn0\\leveljc0\\leveljcn0\\levelfollow0\\levelstartat1\\levelold\\levelspace0\n\
\\levelindent360{\\leveltext\\'02\\'00.;}{\\levelnumbers\\'01;}\\chbrdr\\brdrnone\\brdrcf1 \\chshdng0\\chcfpat1\\chcbpat1 \\s45\\fi-360\\li1080 }{\\listname ;}\\listid273291241}{\\list\\listtemplateid868123898\\listsimple{\\listlevel\\levelnfc255\\levelnfcn255\\leveljc0\n\
\\leveljcn0\\levelfollow0\\levelstartat1\\levelold\\levelspace0\\levelindent360{\\leveltext\\'05Note:;}{\\levelnumbers;}\\b\\i0\\chbrdr\\brdrnone\\brdrcf1 \\chshdng0\\chcfpat1\\chcbpat1 }{\\listname ;}\\listid560600786}{\\list\\listtemplateid-1882543998\\listsimple{\\listlevel\n\
\\levelnfc0\\levelnfcn0\\leveljc0\\leveljcn0\\levelfollow0\\levelstartat1\\levelold\\levelspace0\\levelindent360{\\leveltext\\'02\\'00.;}{\\levelnumbers\\'01;}\\chbrdr\\brdrnone\\brdrcf1 \\chshdng0\\chcfpat1\\chcbpat1 \\fi-360\\li1080 }{\\listname ;}\\listid575434854}\n\
{\\list\\listtemplateid-1882543998\\listsimple{\\listlevel\\levelnfc0\\levelnfcn0\\leveljc0\\leveljcn0\\levelfollow0\\levelstartat1\\levelold\\levelspace0\\levelindent360{\\leveltext\\'02\\'00.;}{\\levelnumbers\\'01;}\\chbrdr\\brdrnone\\brdrcf1 \\chshdng0\\chcfpat1\\chcbpat1\n\
\\fi-360\\li1080 }{\\listname ;}\\listid717165899}{\\list\\listtemplateid-1882543998\\listsimple{\\listlevel\\levelnfc0\\levelnfcn0\\leveljc0\\leveljcn0\\levelfollow0\\levelstartat1\\levelold\\levelspace0\\levelindent360{\\leveltext\\'02\\'00.;}{\\levelnumbers\\'01;}\\chbrdr\n\
\\brdrnone\\brdrcf1 \\chshdng0\\chcfpat1\\chcbpat1 \\fi-360\\li1080 }{\\listname ;}\\listid1341545372}{\\list\\listtemplateid841668904\\listsimple{\\listlevel\\levelnfc255\\levelnfcn255\\leveljc0\\leveljcn0\\levelfollow0\\levelstartat1\\levelold\\levelspace0\\levelindent360\n\
{\\leveltext\\'05Note:;}{\\levelnumbers;}\\b\\i0\\chbrdr\\brdrnone\\brdrcf1 \\chshdng0\\chcfpat1\\chcbpat1 }{\\listname ;}\\listid1550410408}{\\list\\listtemplateid-98929936\\listsimple{\\listlevel\\levelnfc255\\levelnfcn255\\leveljc0\\leveljcn0\\levelfollow0\\levelstartat1\n\
\\levelold\\levelspace0\\levelindent360{\\leveltext\\'05Note:;}{\\levelnumbers;}\\b\\i0\\chbrdr\\brdrnone\\brdrcf1 \\chshdng0\\chcfpat1\\chcbpat1 }{\\listname ;}\\listid1715734938}{\\list\\listtemplateid-1447280102\\listsimple{\\listlevel\\levelnfc255\\levelnfcn255\\leveljc0\n\
\\leveljcn0\\levelfollow0\\levelstartat1\\levelold\\levelspace0\\levelindent360{\\leveltext\\'05Note:;}{\\levelnumbers;}\\b\\i0\\chbrdr\\brdrnone\\brdrcf1 \\chshdng0\\chcfpat1\\chcbpat1 }{\\listname ;}\\listid2009672104}}{\\*\\listoverridetable{\\listoverride\\listid-125\n\
\\listoverridecount0\\ls1}{\\listoverride\\listid-126\\listoverridecount0\\ls2}{\\listoverride\\listid-127\\listoverridecount0\\ls3}{\\listoverride\\listid-128\\listoverridecount0\\ls4}{\\listoverride\\listid-120\\listoverridecount0\\ls5}{\\listoverride\\listid-129\n\
\\listoverridecount0\\ls6}{\\listoverride\\listid-130\\listoverridecount0\\ls7}{\\listoverride\\listid-131\\listoverridecount0\\ls8}{\\listoverride\\listid-132\\listoverridecount0\\ls9}{\\listoverride\\listid-119\\listoverridecount0\\ls10}{\\listoverride\\listid-125\n\
\\listoverridecount0\\ls11}{\\listoverride\\listid-126\\listoverridecount0\\ls12}{\\listoverride\\listid-127\\listoverridecount0\\ls13}{\\listoverride\\listid-128\\listoverridecount0\\ls14}{\\listoverride\\listid-120\\listoverridecount0\\ls15}{\\listoverride\\listid-129\n\
\\listoverridecount0\\ls16}{\\listoverride\\listid-130\\listoverridecount0\\ls17}{\\listoverride\\listid-131\\listoverridecount0\\ls18}{\\listoverride\\listid-132\\listoverridecount0\\ls19}{\\listoverride\\listid-119\\listoverridecount0\\ls20}{\\listoverride\\listid-2\n\
\\listoverridecount1{\\lfolevel\\listoverrideformat{\\listlevel\\levelnfc23\\levelnfcn23\\leveljc0\\leveljcn0\\levelfollow0\\levelstartat1\\levelold\\levelspace0\\levelindent283{\\leveltext\\'01\\u-3913 ?;}{\\levelnumbers;}\\f3\\chbrdr\\brdrnone\\brdrcf1\n\
\\chshdng0\\chcfpat1\\chcbpat1\\fbias0 \\fi-283\\li1003 }}\\ls21}{\\listoverride\\listid1341545372\\listoverridecount0\\ls22}{\\listoverride\\listid273291241\\listoverridecount0\\ls23}{\\listoverride\\listid575434854\\listoverridecount0\\ls24}{\\listoverride\\listid717165899\n\
\\listoverridecount0\\ls25}{\\listoverride\\listid-2\\listoverridecount1{\\lfolevel\\listoverrideformat{\\listlevel\\levelnfc23\\levelnfcn23\\leveljc0\\leveljcn0\\levelfollow0\\levelstartat1\\levelold\\levelspace0\\levelindent360{\\leveltext\\'01\\u-3913 ?;}{\\levelnumbers;}\n\
\\f3\\chbrdr\\brdrnone\\brdrcf1 \\chshdng0\\chcfpat1\\chcbpat1\\fbias0 \\fi-360\\li1800 }}\\ls26}{\\listoverride\\listid-2\\listoverridecount1{\\lfolevel\\listoverrideformat{\\listlevel\\levelnfc23\\levelnfcn23\\leveljc0\\leveljcn0\\levelfollow0\\levelstartat1\\levelold\n\
\\levelspace0\\levelindent283{\\leveltext\\'01\\u-3913 ?;}{\\levelnumbers;}\\f9\\chbrdr\\brdrnone\\brdrcf1 \\chshdng0\\chcfpat1\\chcbpat1\\fbias0 \\fi-283\\li1003 }}\\ls27}{\\listoverride\\listid39135856\\listoverridecount0\\ls28}{\\listoverride\\listid-125\n\
\\listoverridecount0\\ls29}{\\listoverride\\listid-126\\listoverridecount0\\ls30}{\\listoverride\\listid-127\\listoverridecount0\\ls31}{\\listoverride\\listid-128\\listoverridecount0\\ls32}{\\listoverride\\listid-120\\listoverridecount0\\ls33}{\\listoverride\\listid-129\n\
\\listoverridecount0\\ls34}{\\listoverride\\listid-130\\listoverridecount0\\ls35}{\\listoverride\\listid-131\\listoverridecount0\\ls36}{\\listoverride\\listid-132\\listoverridecount0\\ls37}{\\listoverride\\listid273291241\\listoverridecount0\\ls38}\n\
{\\listoverride\\listid2009672104\\listoverridecount0\\ls39}{\\listoverride\\listid1550410408\\listoverridecount0\\ls40}{\\listoverride\\listid1715734938\\listoverridecount0\\ls41}{\\listoverride\\listid560600786\\listoverridecount0\\ls42}}{\\info{\\title SciTech MGL}\n\
{\\author Kendall Bennett}{\\operator Kendall Bennett}{\\creatim\\yr2003\\mo1\\dy3\\hr12\\min29}{\\revtim\\yr2003\\mo1\\dy3\\hr12\\min41}{\\version3}{\\edmins2}{\\nofpages1}{\\nofwords26}{\\nofchars149}{\\*\\company SciTech Software, Inc.}{\\nofcharsws0}{\\vern8203}}\n\
\\margl1440\\margr1440\\gutter360 \\widowctrl\\ftnbj\\aenddoc\\noxlattoyen\\expshrtn\\noultrlspc\\dntblnsbdb\\nospaceforul\\linkstyles\\hyphcaps0\\formshade\\horzdoc\\dgmargin\\dghspace187\\dgvspace127\\dghorigin1800\\dgvorigin1440\\dghshow1\\dgvshow2\n\
\\jexpand\\viewkind4\\viewscale100\\pgbrdrhead\\pgbrdrfoot\\splytwnine\\ftnlytwnine\\htmautsp\\nolnhtadjtbl\\useltbaln\\alntblind\\lytcalctblwd\\lyttblrtgr\\lnbrkrule \\fet0{\\*\\template C:\\\\private\\\\docs\\\\mgl\\\\refguide.dot}\\sectd\n\
\\psz1\\sbkodd\\pgnrestart\\linex0\\headery360\\footery360\\endnhere\\sectdefaultcl {\\*\\pnseclvl1\\pnucrm\\pnstart1\\pnindent720\\pnhang{\\pntxta .}}{\\*\\pnseclvl2\\pnucltr\\pnstart1\\pnindent720\\pnhang{\\pntxta .}}{\\*\\pnseclvl3\\pndec\\pnstart1\\pnindent720\\pnhang\n\
{\\pntxta .}}{\\*\\pnseclvl4\\pnlcltr\\pnstart1\\pnindent720\\pnhang{\\pntxta )}}{\\*\\pnseclvl5\\pndec\\pnstart1\\pnindent720\\pnhang{\\pntxtb (}{\\pntxta )}}{\\*\\pnseclvl6\\pnlcltr\\pnstart1\\pnindent720\\pnhang{\\pntxtb (}{\\pntxta )}}{\\*\\pnseclvl7\n\
\\pnlcrm\\pnstart1\\pnindent720\\pnhang{\\pntxtb (}{\\pntxta )}}{\\*\\pnseclvl8\\pnlcltr\\pnstart1\\pnindent720\\pnhang{\\pntxtb (}{\\pntxta )}}{\\*\\pnseclvl9\\pnlcrm\\pnstart1\\pnindent720\\pnhang{\\pntxtb (}{\\pntxta )}}\\pard\\plain\n\
\\ql \\li0\\ri0\\widctlpar\\aspalpha\\aspnum\\faauto\\adjustright\\rin0\\lin0\\itap0 \\fs24\\lang1033\\langfe1033\\cgrid\\langnp1033\\langfenp1033\n\
";

const char *tableTitle = "{%s \\par }\n";

const char *tableHeader = "\
\\trowd \\trgaph108\\trleft720\\trftsWidth1\\trpaddl108\\trpaddr108\\trpaddfl3\\trpaddfr3 \\clvertalt\\cltxlrtb\\clftsWidth3\\clwWidth3798 \\cellx4518\\clvertalt\\cltxlrtb\\clftsWidth3\\clwWidth3798 \\cellx8316\\pard\\plain\n\
\\s37\\ql \\li0\\ri0\\sa40\\keepn\\widctlpar\\intbl\\nooverflow\\faroman\\rin0\\lin0 \\f28\\fs22\\lang1033\\langfe1033\\cgrid\\langnp1033\\langfenp1033\n\
";

const char *tableEntry = "{\\field{\\*\\fldinst { REF %s \\\\* MERGEFORMAT}}{\\fldrslt {%s}}}{\\cell }\n";
const char *emptyTableEntry = "{\\cell }\n";

const char *tableSeparator = "\
\\pard\\plain \\ql \\li0\\ri0\\widctlpar\\intbl\\aspalpha\\aspnum\\faauto\\adjustright\\rin0\\lin0 \\fs24\\lang1033\\langfe1033\\cgrid\\langnp1033\\langfenp1033 {\\trowd\n\
\\trgaph108\\trleft720\\trftsWidth1\\trpaddl108\\trpaddr108\\trpaddfl3\\trpaddfr3 \\clvertalt\\cltxlrtb\\clftsWidth3\\clwWidth3798 \\cellx4518\\clvertalt\\cltxlrtb\\clftsWidth3\\clwWidth3798 \\cellx8316\\row }\\trowd\n\
\\trgaph108\\trleft720\\trftsWidth1\\trpaddl108\\trpaddr108\\trpaddfl3\\trpaddfr3 \\clvertalt\\cltxlrtb\\clftsWidth3\\clwWidth3798 \\cellx4518\\clvertalt\\cltxlrtb\\clftsWidth3\\clwWidth3798 \\cellx8316\\pard\\plain\n\
\\s37\\ql \\li0\\ri0\\sa40\\keepn\\widctlpar\\intbl\\nooverflow\\faroman\\rin0\\lin0 \\f28\\fs22\\lang1033\\langfe1033\\cgrid\\langnp1033\\langfenp1033\n\
";

const char *tableFooter = "\
\\pard\\plain \\ql \\li0\\ri0\\widctlpar\\intbl\\aspalpha\\aspnum\\faauto\\adjustright\\rin0\\lin0 \\fs24\\lang1033\\langfe1033\\cgrid\\langnp1033\\langfenp1033 {\\trowd\n\
\\trgaph108\\trleft720\\trftsWidth1\\trpaddl108\\trpaddr108\\trpaddfl3\\trpaddfr3 \\clvertalt\\cltxlrtb\\clftsWidth3\\clwWidth3798 \\cellx4518\\clvertalt\\cltxlrtb\\clftsWidth3\\clwWidth3798 \\cellx8316\\row }\\pard\n\
\\ql \\li0\\ri0\\widctlpar\\aspalpha\\aspnum\\faauto\\adjustright\\rin0\\lin0\\itap0\n\
";

const char *docFooter = "\
\\f28\\fs22\\lang1033\\langfe1033\\cgrid\\langnp1033\\langfenp1033 {\\par }}\n";

char    buf[2048];

char *skipwhite(char *s)
{
    while (*s && isspace(*s))
        s++;
    return *s ? s : NULL;
}

int main(int argc, char *argv[])
{
    int     count,eof;
    char    *p,*q;
    FILE    *in;

    /* Display usage information */
    if (argc != 2) {
        fprintf(stderr, "Usage: rtftable <file>\n");
        return -1;
        }

    /* Read all lines from input file, generating RTF to standard output */
    if ((in = fopen(argv[1], "r")) == NULL) {
        printf("Unable to open input file!\n");
        return -1;
        }

    /* Write document header */
    printf(docHeader);

    /* Scan for the table header comment */
    eof = (fgets(buf,sizeof(buf),in) == NULL);
    while (!eof) {
        p = skipwhite(buf);
        if (p != NULL && p[0] == '/' && p[1] == '*') {
            /* Find the table title text to write */
            if ((p = skipwhite(p+2)) != NULL) {
                q = p;
                while (*q && (q[0] != '*' || q[1] != '/'))
                    q++;
                *q = 0;
                printf(tableTitle, p);
                }
            printf(tableHeader);

            /* Now process all the table entries until we find the
             * next comment line or the end of the file
             */
            count = 0;
            while (fgets(buf,sizeof(buf),in)) {
                if ((p = skipwhite(buf)) == NULL)
                    continue;
                if (p[0] == '/' && p[1] == '*')
                    break;

                /* Extract the function name */
                q = p;
                while (*q && !isspace(*q))
                    q++;
                *q = 0;
                if (count && !(count & 1))
                    printf(tableSeparator);
                printf(tableEntry, p, p);
                count++;
                }
            if (!count) {
                printf("Error: did not find table entries!\n");
                return -1;
                }
            if (count & 1)
                printf(emptyTableEntry);
            printf(tableFooter);
            }
        else
            eof = (fgets(buf,sizeof(buf),in) == NULL);
        }

    /* Write document footer */
    printf(docFooter);

    /* Clean up and exit */
    fclose(in);
    return 0;
}

