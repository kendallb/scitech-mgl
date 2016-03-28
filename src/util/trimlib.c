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
* Environment:  Any
*
* Description:  Simple program to process the response file for the Watcom
*               linker wlink, to remove the empty 'LIBR' statement that
*               our makefiles generate (cant seem to find a way to get
*               DMAKE to automatically remove this if there are no libraries
*               to be linked with the executable file).
*
*               Note that we can also optionally import a REF file containing
*               all info necessay to add the EXPORT directives to the linker
*               script to export the functions with the proper names.
*
****************************************************************************/

#include "scitech.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int isupperstr(const char *s)
{
    while (*s != '\0') {
        if (islower(*s))
            return false;
        ++s;
        }

    return true;
}

// TODO: Add support for IMPORT directives as well as EXPORT directives.
//       We will also need to handle translating entire DEF files including
//       the segment options in order to get complex stuff like OS/2 DLL's
//       building correctly.

int main(int argc, char *argv[])
{
    int     foundExports,ordinal;
    char    buf[255],name[255],stdcallName[255],intName[255];
    char    *p,*q,*r;
    FILE    *infile,*outfile,*reffile;

    if (argc != 3 && argc != 4) {
        fprintf(stderr, "Usage: trimlib <infile> <outfile> [reffile]\n");
        return -1;
        }

    if ((infile = fopen(argv[1], "r")) == NULL) {
        printf("Unable to open input file!\n");
        exit(1);
        }
    if ((outfile = fopen(argv[2], "w")) == NULL) {
        printf("Unable to open output file!\n");
        exit(1);
        }

    while (fgets(buf,255,infile)) {
        if (strncmp(buf,"LIBR", 4) == 0) {
            p = &buf[4];
            while (isspace(*p) && *p != '\n' && *p != '\0')
                p++;
            if (*p == '\n' || *p == '\0')
                continue;
            while (*p != '\n' && *p != '\0')
                p++;
            p--;
            if (*p == ',')
                *p = '\n';
            }

        fputs(buf,outfile);
        }
    fputs("\n",outfile);

    if (argc == 4) {
        /* Exports directives can all be added at the start of the
         * linker script, so we do that first. The directives need
         * to be of the following form:
         *
         *   EXP InitReg.1='_InitReg'
         *
         * for __cdecl functions and:
         *
         *   EXP InitReg.1='_InitReg@0'
         *
         * for __stdcall functions. Note that the leading underscore
         * is always removed from the function names, as that is how
         * the latest Visual C++ compilers work. We do this to maintain
         * compatibility with that compiler, even though I hate it
         * passionately!
         * For _Pascal exports, which are all uppercase, no underscores
         * are added.
         */
        if ((reffile = fopen(argv[3], "r")) == NULL) {
            printf("Unable to open ref file!\n");
            exit(1);
            }
        foundExports = false;
        while (fgets(buf,255,reffile)) {
            if (strncmp(buf,"EXPORTS", 7) == 0) {
                foundExports = true;
                break;
                }
            }
        if (!foundExports) {
            printf("Did not find EXPORTS keyword!\n");
            exit(1);
            }
        ordinal = 1;
        while (fgets(buf,255,reffile)) {
            intName[0] = '\0';
            p = buf;
            if (*p == ';')
                continue;
            while (isspace(*p) && *p != '\n' && *p != '\0')
                p++;
            if (*p == '\n' || *p == '\0')
                continue;

            /* Parse export name */
            q = stdcallName;
            while (!isspace(*p) && *p != '\n' && *p != '\0')
                *q++ = *p++;
            *q = '\0';
            strcpy(name,stdcallName);
            if ((r = strchr(name,'@')) != NULL)
                *r = '\0';
            else
                stdcallName[0] = '\0';

            /* Look for optional internal name */
            while (isspace(*p) && *p != '\n' && *p != '\0')
                p++;
            if (*p == '=') {
                ++p;
                while (isspace(*p) && *p != '\n' && *p != '\0')
                    p++;
                q = intName;
                while (!isspace(*p) && *p != '\n' && *p != '\0')
                    *q++ = *p++;
                *q = '\0';
                }

            /* Look for optional ordinal */
            while (isspace(*p) && *p != '\n' && *p != '\0')
                p++;
            r = p;
            if (*r == '@') {
                ++r;
                ordinal = atoi(r);
                }
            if ((p = strchr(name, '@')) != NULL)
                *p = '\0';

            /* Emit linker directive */
            if (stdcallName[0])
                fprintf(outfile,"EXP %s.%d='_%s'\n", name,ordinal,stdcallName);
            else if (intName[0])
                fprintf(outfile,"EXP %s.%d='%s'\n", name,ordinal,intName);
            else
                fprintf(outfile,"EXP %s.%d\n", name,ordinal);

            ordinal++;
            }
        fclose(reffile);
        }

    fclose(infile);
    fclose(outfile);
    return 0;
}
