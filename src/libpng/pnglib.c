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
#include "png.h"
#include "clib/modloadr.h"
#ifndef _MAX_PATH
#define _MAX_PATH   256
#endif

/*---------------------------- Global Variables ---------------------------*/

PNG_exports _VARAPI     _PNG_exports;
static int              loaded = false;
static MOD_MODULE       *hModBPD = NULL;
static ulong            hModSize;

PNG_imports _VARAPI _PNG_imports = {
    sizeof(PNG_imports),
    };

extern PM_imports _VARAPI _PM_imports;

/*----------------------------- Implementation ----------------------------*/

#define DLL_NAME    "png.bpd"

/****************************************************************************
REMARKS:
Fatal error handler for non-exported PNG_exports.
****************************************************************************/
static void _PNG_fatalErrorHandler(void)
{
    PM_fatalError("Unsupported export function called! Please upgrade your copy of PNG!\n");
}

/****************************************************************************
PARAMETERS:
shared  - True to load the driver into shared memory.

REMARKS:
Loads the binary portable DLL into memory and initilises it.
****************************************************************************/
static ibool LoadDriver(void)
{
    PNG_initLibrary_t   PNG_initLibrary;
    PNG_exports         *pngExp;
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
    if ((PNG_initLibrary = (PNG_initLibrary_t)MOD_getProcAddress(hModBPD,MOD_FNPREFIX"PNG_initLibrary")) == NULL)
        return false;
    if ((pngExp = PNG_initLibrary(&_PM_imports,&_PNG_imports)) == NULL)
        PM_fatalError("PNG_initLibrary failed!\n");

    /* Initialize all default imports to point to fatal error handler
     * for upwards compatibility, and copy the exported functions.
     */
    max = sizeof(_PNG_exports)/sizeof(PNG_initLibrary_t);
    for (i = 0,p = (ulong*)&_PNG_exports; i < max; i++)
        *p++ = (ulong)_PNG_fatalErrorHandler;
    memcpy(&_PNG_exports,pngExp,MIN(sizeof(_PNG_exports),pngExp->dwSize));
    loaded = true;
    return true;
}

// Small stub entry points that are the first functions ever called. These
// stubs check to see if the library needs to be dynamically loaded.

int PNGAPI png_sig_cmp(
    png_bytep sig,
    png_size_t start,
    png_size_t num_to_check)
{
    if (!LoadDriver())
        return 0;
    return _PNG_exports.png_sig_cmp(sig,start,num_to_check);
}

png_structp PNGAPI png_create_read_struct(
    png_const_charp user_png_ver,
    voidp error_ptr,
    png_error_ptr error_fn,
    png_error_ptr warn_fn)
{
    if (!LoadDriver())
        return NULL;
    return _PNG_exports.png_create_read_struct(user_png_ver,error_ptr,error_fn,warn_fn);
}

png_structp PNGAPI png_create_write_struct(
    png_const_charp user_png_ver,
    voidp error_ptr,
    png_error_ptr error_fn,
    png_error_ptr warn_fn)
{
    if (!LoadDriver())
        return NULL;
    return _PNG_exports.png_create_write_struct(user_png_ver,error_ptr,error_fn,warn_fn);
}

