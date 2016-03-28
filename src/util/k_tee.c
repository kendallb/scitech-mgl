/****************************************************************************
*
*  ========================================================================
*
*   Copyright (C) 1991-2006 SciTech Software, Inc. All rights reserved.
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
* Environment:  any
*
* Description:  Program to send input to multiple outputs. Modeled after
*               the POSIX 'tee' utility.
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pmapi.h"


void usage(void)
{
    printf("Usage: k_tee [-a] <files>\n");
    exit(0);
}


#define BUFFER_SIZE     256
char    buffer[BUFFER_SIZE];

int main(int argc, char *argv[])
{

    FILE        **out_files;
    int         i;
    int         bytes_read;
    int         append = false;

    if (argc > 1 && stricmp(argv[1],"-a") == 0) {
        append = true;
        --argc;
        ++argv;
        }

    if (argc < 2)
        usage();

    --argc;             /* Skip first argument */
    ++argv;

    out_files = malloc(sizeof(FILE *) * argc);
    if (out_files == NULL)
        PM_fatalError("Not enough memory for file pointers!");

    /* Open output files */
    for (i = 0; i < argc; ++i) {
        out_files[i] = fopen(argv[i], append ? "a" : "w");
        if (out_files[i] == NULL)
            PM_fatalError("Unable to open output file!");
        }

    /* Do the tee-ing */
    for (;;) {
        bytes_read = fread(buffer, 1, BUFFER_SIZE, stdin);
        if (bytes_read < 0)
            PM_fatalError("Error reading (stdin)!");

        if (bytes_read == 0)
            break;
        if (fwrite(buffer, 1, bytes_read, stdout) != bytes_read)
            PM_fatalError("Error writing (stdout)!");

        for (i = 0; i < argc; ++i) {
            if (fwrite(buffer, 1, bytes_read, out_files[i]) != bytes_read)
                PM_fatalError("Error writing output file!");
            }
        }

    for (i = 0; i < argc; ++i)
        fclose(out_files[i]);

    return 0;
}
