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
* Description:  Font enumeration routines. MGL_enumerateFonts will enumerate
*               all fonts in formats supported by MGL and present in
*               current working directory, $MGL_ROOT/fonts and _MGL_path/fonts.
*
****************************************************************************/

#include "mgl.h"

/*--------------------------- Global Variables ----------------------------*/

/* {secret} */
typedef struct fontEnumCache {
    struct fontEnumCache   *next;
    font_info_t            info;
    } fontEnumCache;

static fontEnumCache     *_MGL_fontEnumCache = NULL;

/*------------------------- Implementation --------------------------------*/

#define CACHE_FILE           "fntcache.inf"
#define CACHE_BLOCK_SIZE     32
#define CACHE_LINE_SIZE      512

/* {secret} */
typedef struct {
    char           *fileName;
    short          fontLibType;
    char           *familyName;
    ibool          isBold;
    ibool          isItalic;
    ibool          isFixed;
    ibool          exists;
    } fileCache;

/* {secret} */
typedef struct {
#if 0 // TODO: missing PM_getFileTime under Unix and DOS
    PM_time        timestamp;
#endif
    size_t         filesCnt;
    size_t         validCnt;
    size_t         filesBlocks;
    fileCache      *files;
    } dirCache;

static int comparFileCache(
    const void *i1,
    const void *i2)
{
    return strcmp(((fileCache*)i1)->fileName, ((fileCache*)i2)->fileName);
}

static int comparFileCacheByFamily(
    const void *i1,
    const void *i2)
{
    return strcmp(((fileCache*)i1)->familyName, ((fileCache*)i2)->familyName);
}

static int searchFileCache(
    const void *key,
    const void *item)
{
    return strcmp((const char*)key, ((fileCache*)item)->fileName);
}

/****************************************************************************
PARAMETERS:
cache   - cache to free

REMARKS:
Frees directory cache structure
{secret}
****************************************************************************/
static void freeFontCache(
    dirCache *cache)
{
    size_t i;

    for (i = 0; i < cache->filesCnt; i++) {
        PM_free(cache->files[i].fileName);
        PM_free(cache->files[i].familyName);
        }
    PM_free(cache->files);
}

/****************************************************************************
PARAMETERS:
dir   - path to look for fntcache.inf in
cache - cache structure to save loaded data to

REMARKS:
Initializes dirCache structure and tries to load fntcache.inf file from
given directory. Silently fails if such file does not exist.

fntcache.inf is text file in the following format: first line contains
number of entries in decimal format, which indicates the number of file lines
to follow. Each file line contains three column delimined by '\t'. Font file
name is in the first column. Second column has fixed "T-F-B-I" format where
T is '0', '1' or '2' (as numerical values of MGL_fontLibType are), F is
'f' for fixed size fonts and 'p' for proportional, B is
'b' for bold face and 'n' otherwise and I is 'i' for italic face and 'n'
otherwise. Third column contains family name.

The cache is sorted alphabetically by file name.

Example file:
9
1979rg__.ttf    1-p-n-n 1979
1stgrade.ttf    1-p-n-n FirstGrader-Normal
39smooth.ttf    1-p-n-n 39 Smooth
7hours.ttf      1-p-n-n 7 hours
Scot000.ttf     1-p-n-n Scott
verdana.ttf     1-p-n-n Verdana
verdanab.ttf    1-p-b-n Verdana
verdanai.ttf    1-p-n-i Verdana
verdanaz.ttf    1-p-b-i Verdana
{secret}
****************************************************************************/
static void loadFontCache(
    const char *dir,
    dirCache *cache)
{
    char       filename[PM_MAX_PATH];
    char       buf[CACHE_LINE_SIZE];
    char       *c1, *c2;
    char       bufType, bufFixed, bufBold, bufItalic;
    fileCache  *file;
    FILE       *f;
    size_t     i;

    /* empty cache: */
    cache->filesCnt = 0;
    cache->validCnt = 0;
    cache->filesBlocks = 0;
    cache->files = NULL;

    /* try to load fonts cache file from directory, silently fail
       if it doesn't exist: */
    strcpy(filename, dir);
    PM_backslash(filename);
    strcat(filename, CACHE_FILE);
    f = __MGL_fopen(filename, "rt");
    if (f == NULL)
        return;
#if 0 // TODO: missing PM_getFileTime under Unix and DOS
    PM_getFileTime(filename, true, &cache->timestamp);
#endif
    if (fgets(buf, CACHE_LINE_SIZE, f) == NULL) {
        __MGL_fclose(f);
        return;
        }

    /* get number of items in the cache:  */
    if (sscanf(buf, "%li", &cache->filesCnt) != 1 ||
        cache->filesCnt == 0) {
        __MGL_fclose(f);
        return;
        }

    cache->filesBlocks = ((cache->filesCnt - 1) / CACHE_BLOCK_SIZE) + 1;
    cache->files =
        PM_malloc(sizeof(fileCache) * cache->filesBlocks * CACHE_BLOCK_SIZE);
    if (cache->files == NULL)
        FATALERROR(grNoMem);

    /* read entries. Each font file has one entry, there's one entry per
       line. The cache is sorted alphabetically by filename. */
    for (i = 0; i < cache->filesCnt; i++) {
        file = cache->files + i;
        file->exists = false;
        if (fgets(buf, CACHE_LINE_SIZE, f) == NULL) {
            cache->filesCnt = i;
            __MGL_fclose(f);
            return;
            }

        c1 = buf;
        for (c2 = c1; *c2 != '\0' && *c2 != '\t'; c2++) {}
        *c2 = '\0';
        if ((file->fileName = PM_malloc(strlen(c1) + 1)) == NULL)
            FATALERROR(grNoMem);
        strcpy(file->fileName, c1);

        c1 = c2 + 1;
        for (c2 = c1; *c2 != '\0' && *c2 != '\t'; c2++) {}
        *c2 = '\0';
        if (sscanf(c1, "%c-%c-%c-%c",
                   &bufType, &bufFixed, &bufBold, &bufItalic) != 4)
            continue;
        file->isBold = (bufBold == 'b');
        file->isItalic = (bufItalic == 'i');
        file->isFixed = (bufFixed == 'f');
        file->fontLibType = bufType - '0';

        c1 = c2 + 1;
        for (c2 = c1; *c2 != '\0'; c2++)
            if (*c2 == '\n' || *c2 == '\r') *c2 = '\0';
        if ((file->familyName = PM_malloc(strlen(c1) + 1)) == NULL)
            FATALERROR(grNoMem);
        strcpy(file->familyName, c1);
        }
    __MGL_fclose(f);
}


/****************************************************************************
PARAMETERS:
dir   - path to store fntcache.inf in
cache - cache structure to save

REMARKS:
Attempt to save fonts cache information to file fntcache.inf in the directory.
Silently fails if the directory is not writeable.

SEE ALSO:
loadFontCache
{secret}
****************************************************************************/
static void saveFontCache(
    const char *dir,
    dirCache *cache)
{
    char       filename[PM_MAX_PATH];
    FILE       *f;
    size_t     i;

    if (cache->validCnt == 0)
        return;
    strcpy(filename, dir);
    PM_backslash(filename);
    strcat(filename, CACHE_FILE);
    f = __MGL_fopen(filename, "wt");
    if (f == NULL)
        return;

    qsort(cache->files, cache->filesCnt, sizeof(fileCache), comparFileCache);

    fprintf(f, "%li\n", cache->validCnt);
    for (i = 0; i < cache->filesCnt; i++) {
        if (!cache->files[i].exists)
            continue;
        fprintf(f, "%s\t%c-%c-%c-%c\t%s\n",
                    cache->files[i].fileName,
                    cache->files[i].fontLibType + '0',
                    cache->files[i].isFixed ? 'f' : 'p',
                    cache->files[i].isBold ? 'b' : 'n',
                    cache->files[i].isItalic ? 'i' : 'n',
                    cache->files[i].familyName);
        }
    __MGL_fclose(f);
}

/****************************************************************************
REMARKS:
Gets character's width in given font
{secret}
****************************************************************************/
static int getWidthOfChar(
    font_t *font,
    int ch)
{
    int     glyph;

    if (font == NULL) return 0;
    switch (font->fontType & MGL_FONTTYPEMASK) {
        case MGL_FIXEDFONT:
            return font->fontWidth;
        case MGL_VECTORFONT:
            if (VEC_ISMISSING(font,ch))
                return VECFONT(font)->width[VEC_MISSINGSYMBOL(font)];
            else
                return VECFONT(font)->width[ch];
        case MGL_PROPFONT:
            glyph = ch - BITFONT(font)->startGlyph;
            if (!BITFONT(font)->valid[glyph] && (glyph < BITFONT(font)->numGlyphs))
                _MGL_generateGlyph(glyph,BITFONT(font));
            if ((BIT_ISMISSING(font,glyph)) ||
                (ch < BITFONT(font)->startGlyph) ||
                (glyph >= BITFONT(font)->numGlyphs))
                return BITFONT(font)->width[BIT_MISSINGSYMBOL(font)];
            else
                return BITFONT(font)->width[glyph];
        default:
            return 0;
        }
}

/****************************************************************************
REMARKS:
Checks if the font is fixed width or proportional by comparing
widths of 'i' and 'm'.
{secret}
****************************************************************************/
static ibool fontIsFixed(
    font_lib_t *fontLib)
{
    font_t  *font;
    ibool   fixed;

    font = MGL_loadFontInstance(fontLib, 12, 0.0, 0.0, false);
    if (!font) return false;
    fixed = (getWidthOfChar(font, 'i') == getWidthOfChar(font, 'm'));
    MGL_unloadFontInstance(font);
    return fixed;
}

/****************************************************************************
PARAMETERS:
path        - directory with font file
filename    - filename of the font file
cache       - local cache for one directory

REMARKS:
Updates cache entry for given font file. If such file is not in the
cache, loads it with MGL_openFontLib and extracts information about font
library type, family and face from it.

A font is marked as bold if its name ends with " Bold", as italic if it ends
with " Italic". Suffixes are cumulative. Family name is face name with
recognized suffixes stripped off. For example, cache entry for font
"Arial Bold Italic" will read "family is Arial, face is bold and italic".
{secret}
****************************************************************************/
static void enumFontFile(
    const char *path,
    const char *filename,
    dirCache *cache)
{
    char          fullname[PM_MAX_PATH];
    font_lib_t    *fontLib;
    fileCache     *entry;
    ibool         isNewer;
    ibool         foundSuffix;
#if 0 // TODO: missing PM_getFileTime under Unix and DOS
    PM_time       time;
    long          ytimef, ytimec;
#endif
    char          *buf;
    size_t        buflen;

    strcpy(fullname, path);
    PM_backslash(fullname);
    strcat(fullname, filename);

    /* Try to find entry in cache: */
    entry = (fileCache*) bsearch(filename, cache->files, cache->filesCnt,
                                    sizeof(fileCache), searchFileCache);

    isNewer = (entry == NULL);

#if 0 // TODO: missing PM_getFileTime under Unix and DOS
    if (!isNewer && PM_getFileTime(fullname, true, &time)) {
        ytimef = time.sec + 60 * (time.min + 60 * (time.hour + 24 * (time.day +
                    31 * time.mon)));
        ytimec = cache->timestamp.sec + 60 * (cache->timestamp.min + 60 * (
                    cache->timestamp.hour + 24 * (cache->timestamp.day +
                    31 * cache->timestamp.mon)));
        isNewer = time.year > cache->timestamp.year ||
                    (time.year == cache->timestamp.year && ytimef > ytimec);
        }
#endif

    if (!isNewer) {
        cache->validCnt++;
        entry->exists = true;
        return;
        }

    /* Nothing in cache, we have to open the font: */
    fontLib = MGL_openFontLib(fullname);
    if (fontLib == NULL)
        return;

    if (entry == NULL) {
        if (cache->filesCnt % CACHE_BLOCK_SIZE == 0) {
            cache->filesBlocks++;
            cache->files = PM_realloc(cache->files,
                    sizeof(fileCache) * cache->filesBlocks * CACHE_BLOCK_SIZE);
            }
        entry = cache->files + cache->filesCnt;
        cache->filesCnt++;
        }
    else {
        PM_free(entry->fileName);
        PM_free(entry->familyName);
        }

    /* Try to determine family name and face type from facename: */
    if ((entry->fileName = PM_malloc(strlen(filename) + 1)) == NULL)
        FATALERROR(grNoMem);
    strcpy(entry->fileName, filename);
    entry->fontLibType = fontLib->fontLibType;
    buflen = strlen(fontLib->name);
    if ((buf = PM_malloc(buflen + 1)) == NULL)
        FATALERROR(grNoMem);
    strcpy(buf, fontLib->name);
    entry->isBold = entry->isItalic = false;
    do {
        foundSuffix = false;
        if (buflen >= 5 && strcmp(buf + buflen - 5, " Bold") == 0) {
            foundSuffix = true;
            entry->isBold = true;
            buf[buflen - 5] = '\0';
            buflen -= 5;
            }
        if (buflen >= 7 && strcmp(buf + buflen - 7, " Italic") == 0) {
            foundSuffix = true;
            entry->isItalic = true;
            buf[buflen - 7] = '\0';
            buflen -= 7;
            }
        } while (buflen > 0 && foundSuffix);
    if (buflen == 0) {
        if ((entry->familyName = PM_malloc(strlen(filename) + 1)) == NULL)
            FATALERROR(grNoMem);
        strcpy(entry->familyName, filename);
        }
    else {
        if ((entry->familyName = PM_malloc(strlen(buf) + 1)) == NULL)
            FATALERROR(grNoMem);
        strcpy(entry->familyName, buf);
        }
    PM_free(buf);

    /* Check if the font is fixed width or proportional. In absence of better
       method, we do it by comparing average and maximum character width: */
    entry->isFixed = fontIsFixed(fontLib);

    MGL_closeFontLib(fontLib);

    cache->validCnt++;
    entry->exists = true;
}

/****************************************************************************
PARAMETERS:
path        - directory to scan for fonts
wildcard    - wildcard to match.
cache       - local cache for one directory

REMARKS:
Enumerates fonts in path matching wildard and puts information about them into
directory cache cache.
{secret}
****************************************************************************/
static void enumFontsByWildcard(
    const char *path,
    const char *wildcard,
    dirCache *cache)
{
    char          fullwild[PM_MAX_PATH];
    PM_findData   findData;
    void          *handle;

    strcpy(fullwild, path);
    PM_backslash(fullwild);
    strcat(fullwild, wildcard);

    findData.dwSize = sizeof(findData);
    handle = PM_findFirstFile(fullwild, &findData);
    if (handle == PM_FILE_INVALID)
        return;

    do {
        enumFontFile(path, findData.name, cache);
        } while (PM_findNextFile(handle, &findData));

    PM_findClose(handle);
}

/****************************************************************************
PARAMETERS:
dcache  - local cache for one directory
cache   - pointer to variable that holds list of found families

REMARKS:
Extracts information from dcache and translates it to cache.

This involves finding fonts with same family name and merging them into
one entry in cache.

Called exclusively by enumFontsInDir.
{secret}
****************************************************************************/
static void buildFontEnumCache(
    dirCache *dcache,
    fontEnumCache **cache)
{
    fontEnumCache  *ecache, *tail;
    size_t         i;
    fileCache      *f;

    if (dcache->filesCnt == 0)
        return;
    qsort(dcache->files, dcache->filesCnt, sizeof(fileCache),
          comparFileCacheByFamily);

    tail = ecache = NULL;
    for (i = 0, f = dcache->files; i < dcache->filesCnt; i++, f++) {
        /* new family: */
        if (tail == NULL ||
                    strcmp(tail->info.familyName, f->familyName) != 0) {
            if (tail == NULL) {
                if ((tail = ecache = PM_malloc(sizeof(fontEnumCache))) == NULL)
                    FATALERROR(grNoMem);
                }
            else {
                if ((tail->next = PM_malloc(sizeof(fontEnumCache))) == NULL)
                    FATALERROR(grNoMem);
                tail = tail->next;
                }
            tail->next = NULL;
            strcpy(tail->info.familyName, f->familyName);
            tail->info.fontLibType = f->fontLibType;
            tail->info.isFixed = f->isFixed;
            tail->info.regularFace[0] = '\0';
            tail->info.boldFace[0] = '\0';
            tail->info.italicFace[0] = '\0';
            tail->info.boldItalicFace[0] = '\0';
            }
        /* faces lookup: */
        if (!f->isBold && !f->isItalic)
            strcpy(tail->info.regularFace, f->fileName);
        else if (f->isBold && !f->isItalic)
            strcpy(tail->info.boldFace, f->fileName);
        else if (!f->isBold && f->isItalic)
            strcpy(tail->info.italicFace, f->fileName);
        else if (f->isBold && f->isItalic)
            strcpy(tail->info.boldItalicFace, f->fileName);
        }

    tail->next = *cache;
    *cache = ecache;
}

/****************************************************************************
PARAMETERS:
path    - directory to scan for fonts
cache   - pointer to variable that holds list of found families

REMARKS:
Scans given directory for fonts, taking advantage of fntcache.inf if present
and updating it if the directory is writeable. Updates linked list pointed
to by cache argument by prepending information about fonts in path to it.
{secret}
****************************************************************************/
static void enumFontsInDir(
    const char *path,
    fontEnumCache **cache)
{
    dirCache       dcache;

    /* get list of all fonts in directory: */
    loadFontCache(path, &dcache);
    enumFontsByWildcard(path, "*.ttf", &dcache);
    enumFontsByWildcard(path, "*.fon", &dcache);

    /* save the cache back (if directory is writeable): */
    saveFontCache(path, &dcache);

    /* Build font enum cache from dir one: */
    buildFontEnumCache(&dcache, cache);

    freeFontCache(&dcache);
}

/****************************************************************************
PARAMETERS:
c       - structure to free

REMARKS:
Recursively frees fontEnumCache data structure.
{secret}
****************************************************************************/
static void freeFontEnumCache(
    fontEnumCache *c)
{
    if (c == NULL)
        return;
    freeFontEnumCache(c->next);
    PM_free(c);
}

/****************************************************************************
DESCRIPTION:
Initializes font information cache.

REMARKS:
Scans standard locations for fonts and stores all found font families to
_MGL_fontEnumCache global variable. Called by MGL_enumerateFonts upon first
invocation.
{secret}
****************************************************************************/
void _MGL_initFontEnumCache(void)
{
    char path[PM_MAX_PATH];

    enumFontsInDir(".", &_MGL_fontEnumCache);
    path[0] = '\0';
    if (_MGL_path[0] != '\0') {
        strcpy(path, _MGL_path);
        PM_backslash(path);
        }
    strcat(path, MGL_FONTS);
    enumFontsInDir(path, &_MGL_fontEnumCache);
    if (getenv(MGL_ROOT)) {
        strcpy(path, getenv(MGL_ROOT));
        PM_backslash(path);
        strcat(path, MGL_FONTS);
        enumFontsInDir(path, &_MGL_fontEnumCache);
    }
}

/****************************************************************************
DESCRIPTION:
Destroys font information cache.

REMARKS:
Frees memory occupied by font information cache. Called from MGL_exit.
{secret}
****************************************************************************/
void _MGL_destroyFontEnumCache(void)
{
    if (_MGL_fontEnumCache) {
        freeFontEnumCache(_MGL_fontEnumCache);
        _MGL_fontEnumCache = NULL;
    }
}

/****************************************************************************
DESCRIPTION:
Enumerates all available font families.

HEADER:
mgraph.h

PARAMETERS:
callback    - function that will be called for each font family
cookie      - pointer to user data that will be passed to callback function

REMARKS:
This function finds all fonts in current directory and standard MGL
locations and calls the callback function for every font family it has found.
If the callback returns false, MGL_enumerateFonts immediately returns, even if
it hasn't yet iterated over all available fonts.

The cookie argument is useful in multithreaded environment where it is
neccessary to distinguish between several concurrently running enumerations.

This function will scan directories for fonts when called for the first time.
All subsequent calls are more efficient, because MGL_enumerateFonts will use
font cache stored in memory.

MGL_enumerateFonts will attempt to create file named fntcache.inf in
directories it scans for fonts. This file contains information about all
fonts in the directory and will subsequently be used to further speed up
fonts enumeration.

SEE ALSO:
font_info_t
****************************************************************************/
void MGLAPI MGL_enumerateFonts(
    enumfntcallback_t callback,
    void *cookie)
{
    fontEnumCache *c;

    if (_MGL_fontEnumCache == NULL)
        _MGL_initFontEnumCache();

    c = _MGL_fontEnumCache;
    while (c) {
        if (!callback(&c->info, cookie))
            return;
        c = c->next;
        }
}

