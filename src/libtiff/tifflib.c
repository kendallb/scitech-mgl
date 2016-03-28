/****************************************************************************
*
* Language:     ANSI C
* Environment:  Any
*
* Description:  This file contains the code specific to initialising the
*               SciTech MGL Binary Portable DLL.
*
****************************************************************************/

#include "mgraph.h"
#include "tiffio.h"
#include "clib/peloader.h"
#ifndef _MAX_PATH
#define _MAX_PATH   256
#endif

/*---------------------------- Global Variables ---------------------------*/

TIFF_exports _VARAPI    _TIFF_exports;
static int              loaded = false;
static PE_MODULE        *hModBPD = NULL;
static ulong            hModSize;

TIFF_imports _VARAPI _TIFF_imports = {
    sizeof(TIFF_imports),
    MGL_fread,
    MGL_fwrite,
    };

extern PM_imports _VARAPI _PM_imports;

/*----------------------------- Implementation ----------------------------*/

#define DLL_NAME    "tiff.bpd"

/****************************************************************************
REMARKS:
Fatal error handler for non-exported TIFF_exports.
****************************************************************************/
static void _TIFF_fatalErrorHandler(void)
{
    PM_fatalError("Unsupported export function called! Please upgrade your copy of TIFF!\n");
}

/****************************************************************************
PARAMETERS:
shared  - True to load the driver into shared memory.

REMARKS:
Loads the binary portable DLL into memory and initilises it.
****************************************************************************/
static ibool LoadDriver(void)
{
    TIFF_initLibrary_t  TIFF_initLibrary;
    TIFF_exports        *jpegExp;
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
    hModBPD = PE_loadLibraryExt(f,0,&hModSize,false);
    fclose(f);
    if (!hModBPD)
        return false;
    if ((TIFF_initLibrary = (TIFF_initLibrary_t)PE_getProcAddress(hModBPD,"_TIFF_initLibrary")) == NULL)
        return false;
    if ((jpegExp = TIFF_initLibrary(&_PM_imports,&_TIFF_imports)) == NULL)
        PM_fatalError("TIFF_initLibrary failed!\n");

    /* Initialize all default imports to point to fatal error handler
     * for upwards compatibility, and copy the exported functions.
     */
    max = sizeof(_TIFF_exports)/sizeof(TIFF_initLibrary_t);
    for (i = 0,p = (ulong*)&_TIFF_exports; i < max; i++)
        *p++ = (ulong)_TIFF_fatalErrorHandler;
    memcpy(&_TIFF_exports,jpegExp,MIN(sizeof(_TIFF_exports),jpegExp->dwSize));
    loaded = true;
    return true;
}

struct TIFF_error_mgr * PMAPI TIFF_std_error(
    struct TIFF_error_mgr * err)
{
    if (LoadDriver())
        return _TIFF_exports.TIFF_std_error(err);
    return NULL;
}
