/*******************************************************************
 *
 *  ttinit.c
 *
 *    Client side loader for the SciTech Binary Portable DLL
 *
 *  Copyright 1999 by
 *  SciTech Software, Inc.
 *
 *  This file is part of the FreeType project, and may only be used
 *  modified and distributed under the terms of the FreeType project
 *  license, LICENSE.TXT.  By continuing to use, modify, or distribute
 *  this file you indicate that you have read the license and
 *  understand and accept it fully.
 *
 ******************************************************************/

#include "freetype.h"
#include "clib/modloadr.h"
#include <string.h>
#ifndef	_MAX_PATH
#define	_MAX_PATH	256
#endif

/*---------------------------- Global Variables ---------------------------*/

TT_exports	_VARAPI _TT_exports = {0};
static int			loaded = false;
static MOD_MODULE	*hModBPD = NULL;
static ulong		hModSize;

/*----------------------------- Implementation ----------------------------*/

#define	DLL_NAME 	"freetype.bpd"
/****************************************************************************
REMARKS:
Fatal error handler for non-exported GA_exports.
****************************************************************************/
static void _TT_fatalErrorHandler(void)
{
	PM_fatalError("Unsupported export function called! Please upgrade your copy of FreeType!\n");
}

/****************************************************************************
PARAMETERS:
shared	- True to load the driver into shared memory.

REMARKS:
Loads the binary portable DLL into memory and initilises it.
****************************************************************************/
static ibool LoadDriver(void)
{
	TT_initLibrary_t	TT_initLibrary;
	TT_exports			*ttExp;
	char				filename[_MAX_PATH];
	char				bpdpath[_MAX_PATH];
	FILE				*f;
	int					i,max;
	ulong 				*p;

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
	if ((TT_initLibrary = (TT_initLibrary_t)MOD_getProcAddress(hModBPD,MOD_FNPREFIX"TT_initLibrary")) == NULL)
		return false;
	if ((ttExp = TT_initLibrary()) == NULL)
		PM_fatalError("TT_initLibrary failed!\n");

	/* Initialize all default imports to point to fatal error handler
	 * for upwards compatibility, and copy the exported functions.
	 */
	max = sizeof(_TT_exports)/sizeof(TT_initLibrary_t);
	for (i = 0,p = (ulong*)&_TT_exports; i < max; i++)
		*p++ = (ulong)_TT_fatalErrorHandler;
	memcpy(&_TT_exports,ttExp,MIN(sizeof(_TT_exports),ttExp->dwSize));
	loaded = true;
	return true;
}

/* The following are stub entry points that the application calls to
 * initialise the FreeType loader library, and we use this to load our
 * driver DLL from disk and initialise the library using it.
 */

TT_Error PMAPI TT_Init_FreeType(TT_Engine* engine)
{
	if (LoadDriver())
		return _TT_exports.TT_Init_FreeType(engine);
	return -1;
}


TT_Error PMAPI TT_Done_FreeType(TT_Engine engine)
{
	TT_Error error = -1;

	if (loaded) {
		error = _TT_exports.TT_Done_FreeType(engine);
		MOD_freeLibrary(hModBPD);
		loaded = false;
		}
	return error;
}
