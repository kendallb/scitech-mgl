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
* Description:  Program to build .FON font libraries using the Borland
*               Resource Compiler.
*
****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>
#include <dos.h>
#include <process.h>

/*------------------------- Implementation --------------------------------*/

int main(int argc,char *argv[])
{
    struct find_t   blk;
    char            basename[PM_MAX_PATH];
    char            suffix[PM_MAX_PATH] = "";
    char            findname[PM_MAX_PATH];
    char            findsuffix[PM_MAX_PATH];
    char            command[PM_MAX_PATH];
    int             id,result,suffixlen;
    FILE            *f;

    if (argc != 2 && argc != 3)
        return -1;
    strcpy(basename,argv[1]);
    if (argc > 2)
        strcpy(suffix,argv[2]);
    strcpy(findname,basename);
    strcat(findname,"*");
    strcat(findname,suffix);
    strcat(findname,".fnt");
    strcpy(findsuffix,suffix);
    strcat(findsuffix,".fnt");
    suffixlen = strlen(findsuffix);
    if ((f = fopen("font.rc","w")) == NULL)
        return -1;
    id = 1;
    result = _dos_findfirst(findname,_A_NORMAL,&blk);
    while (result == 0) {
        if (!(blk.attrib & _A_SUBDIR) &&
                strlen(blk.name) == strlen(findname)+1 &&
                stricmp(&blk.name[strlen(blk.name)-suffixlen],findsuffix) == 0) {
            fprintf(f,"%d FONT LOADONCALL MOVEABLE DISCARDABLE %s\n", id,blk.name);
            id++;
            }
        result = _dos_findnext(&blk);
        }
    fclose(f);

    sprintf(command,"cp empty.dll %s%s.fon", basename, suffix);
    system(command);

    sprintf(command,"c:\\c\\bc50\\bin\\brc -fe%s%s.fon font.rc", basename, suffix);
    system(command);

    unlink("font.rc");
    return 0;
}
