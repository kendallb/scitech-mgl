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
* Description:  Simple program to convert a binary file to C code as an
*               array of unsigned characters. The name of the structure
*               is based on the original file name.
*
****************************************************************************/

#include "pmapi.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*------------------------- Global variables ------------------------------*/

#define LINE_SIZE   14

/*-------------------------- Implementation -------------------------------*/

void help(void)
/****************************************************************************
*
* Function:     help
*
* Description:  Help provides usage information about our program if the
*               options do make any sense.
*
****************************************************************************/
{
    printf("Usage: bin2c <filename>\n");
    exit(1);
}

void main(int argc,char *argv[])
{
    FILE    *f;
    char    structName[PM_MAX_PATH],*p,*q;
    uchar   buf[LINE_SIZE];
    int     size,i;

    if (argc < 2)
        help();
    if ((f = fopen(argv[1],"rb")) == NULL)
        PM_fatalError("Unable to open input file!");
    for (p = structName, q = argv[1]; *q != 0; p++,q++) {
        if (*q == '.')
            *p = '_';
        else
            *p = *q;
        }
    printf("unsigned char %s[] = {\n", structName);
    while (!feof(f)) {
        size = fread(buf,1,sizeof(buf),f);
        printf("    ");
        for (i = 0; i < size; i++)
            printf("0x%02X,", buf[i]);
        printf("\n");
        }
    printf("    };\n");
    fclose(f);
}
