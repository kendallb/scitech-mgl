/****************************************************************************
*
*                     SciTech SNAP Graphics Architecture
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
* Environment:  Any 32-bit protected mode environment
*
* Description:  Simple module to directly load the GA_options structure
*               from disk to avoid loading SciTech SNAP Graphics directly.
*
****************************************************************************/

#include "snap/graphics.h"
#if defined(__WIN32_VXD__) || defined(__NT_DRIVER__)
#include "sdd/sddhelp.h"
#else
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#endif

/*----------------------------- Implementation ----------------------------*/

#define DLL_NAME        "graphics.bpd"
#define OPTIONS_NAME    "options.dat"

/****************************************************************************
DESCRIPTION:
Reads the global options file directly from disk.

HEADER:
snap/graphics.h

PARAMETERS:
options - Place to store the returned options information

RETURNS:
False if no options file found.

REMARKS:
This function is similar to GA_getGlobalOptions, but it does not require
SciTech SNAP Graphics to be loaded at all in order to read the global options
file. This is useful for situations where the memory footprint of loading
SciTech SNAP Graphics needs to be avoided.

Note:   The dwSize member of the profile structure is intended for future
        compatibility, and must be set to the size of the structure before
        calling this function. Only the number of bytes set in the
        dwSize member will be copied into the callers structure.

SEE ALSO:
GA_getGlobalOptions, GA_setGlobalOptions, GA_saveGlobalOptions
****************************************************************************/
ibool NAPI GA_readGlobalOptions(
    GA_globalOptions *options)
{
    FILE        *f;
    char        filename[PM_MAX_PATH];
    N_uint32    size;

    /* Find the SciTech SNAP Graphics config path */
    if (!PM_findBPD(DLL_NAME,filename))
        return false;
    filename[strlen(filename)-1] = 0;
    if (strcmp(filename,PM_getSNAPPath()) == 0)
        strcpy(filename,PM_getSNAPConfigPath());
    else {
        PM_backslash(filename);
        strcat(filename,"config");
        }
    PM_backslash(filename);

    /* Read existing saved options from disk */
    strcat(filename,OPTIONS_NAME);
    if ((f = fopen(filename,"rb")) != NULL) {
        size = MIN(sizeof(*options),options->dwSize);
        fread((void *) options,1,size,f);
        fclose(f);
        if (options->dwSize >= size)
            options->dwSize = size;
        return true;
        }
    return false;
}

