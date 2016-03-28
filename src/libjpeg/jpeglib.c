/*
 * jpeginit.c
 *
 * Copyright (C) 1994-1996, Thomas G. Lane.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying README file.
 *
 * This file contains the code specific to loading a SciTech MGL
 * Binary Portable DLL.
 */

#include "mgraph.h"
#include "jpeglib.h"
#include "clib/modloadr.h"
#include <string.h>
#ifndef _MAX_PATH
#define _MAX_PATH   256
#endif

/*---------------------------- Global Variables ---------------------------*/

JPEG_exports _VARAPI    _JPEG_exports;
static int              loaded = false;
static MOD_MODULE       *hModBPD = NULL;
static ulong            hModSize;

JPEG_imports _VARAPI _JPEG_imports = {
    sizeof(JPEG_imports),
    MGL_fread,
    MGL_fwrite,
    };

extern PM_imports _VARAPI _PM_imports;

/*----------------------------- Implementation ----------------------------*/

#define DLL_NAME    "jpeg.bpd"

/****************************************************************************
REMARKS:
Fatal error handler for non-exported JPEG_exports.
****************************************************************************/
static void _JPEG_fatalErrorHandler(void)
{
    PM_fatalError("Unsupported export function called! Please upgrade your copy of JPEG!\n");
}

/****************************************************************************
PARAMETERS:
shared  - True to load the driver into shared memory.

REMARKS:
Loads the binary portable DLL into memory and initilises it.
****************************************************************************/
static ibool LoadDriver(void)
{
    JPEG_initLibrary_t  JPEG_initLibrary;
    JPEG_exports        *jpegExp;
    char                filename[_MAX_PATH];
    char                bpdpath[_MAX_PATH];
    FILE                *f;
    int                 i,max;
    ulong               *p;

    /* Check if we have already loaded the driver */
    if (loaded)
        return true;

    /* Open the BPD file */
    if (!PM_findBPD(DLL_NAME,bpdpath))
        return false;
    strcpy(filename,bpdpath);
    strcat(filename,DLL_NAME);
    if ((f = fopen(filename,"rb")) == NULL)
        return false;
    hModBPD = MOD_loadLibraryExt(f,0,&hModSize,false);
    fclose(f);
    if (!hModBPD)
        return false;
    if ((JPEG_initLibrary = (JPEG_initLibrary_t)MOD_getProcAddress(hModBPD,MOD_FNPREFIX"JPEG_initLibrary")) == NULL)
        return false;
    if ((jpegExp = JPEG_initLibrary(&_PM_imports,&_JPEG_imports)) == NULL)
        PM_fatalError("JPEG_initLibrary failed!\n");

    /* Initialize all default imports to point to fatal error handler
     * for upwards compatibility, and copy the exported functions.
     */
    max = sizeof(_JPEG_exports)/sizeof(JPEG_initLibrary_t);
    for (i = 0,p = (ulong*)&_JPEG_exports; i < max; i++)
        *p++ = (ulong)_JPEG_fatalErrorHandler;
    memcpy(&_JPEG_exports,jpegExp,MIN(sizeof(_JPEG_exports),jpegExp->dwSize));
    loaded = true;
    return true;
}

struct jpeg_error_mgr * PMAPI jpeg_std_error(
    struct jpeg_error_mgr * err)
{
    if (LoadDriver())
        return _JPEG_exports.jpeg_std_error(err);
    return NULL;
}
