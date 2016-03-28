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
* Environment:  any
*
* Description:  Program to count the number of lines of code in a specified
*               file. It will determine the total number of lines in the
*               file, along with the number of comment lines and number of
*               blank lines in the file.
*
****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "scitech.h"

#define LINE_SIZE   255         /* Maximum length of a line             */

ibool   asm_file = false;       /* True if file is an assembler file    */
ibool   inside_comment = false; /* True if inside a 'C' comment         */

ibool comment_line(const char *line)
/****************************************************************************
*
* Function:     comment_line
* Parameters:   line    - Line to check for comment
* Returns:      True if the line is a comment line, false if not.
*
* Description:  Automatically handle the case of 'C' comments that span
*               multiple lines.
*
****************************************************************************/
{
    if (asm_file) {
        if (line[0] == ';')
            return true;
        while (*line != '\n') {
            if (!isspace(*line) && *line != ';')
                return false;
            if (*line == ';')
                return true;
            line++;
            }
        return false;
        }
    else if (inside_comment) {
        while (*line != '\n') {
            if (*line == '*' && *(line+1) == '/') {
                inside_comment = false;
                return true;
                }
            line++;
            }
        return true;
        }
    else {
        while (*line != '\n') {
            if (*line == '/' && *(line+1) == '*') {
                inside_comment = true;
                return comment_line(line+1);
                }
            if (!isspace(*line))
                return false;
            line++;
            }
        return false;
        }
}

ibool blank_line(const char *line)
/****************************************************************************
*
* Function:     blank_line
* Parameters:   lines   - Line to check if blank
* Returns:      True if the line is blank.
*
****************************************************************************/
{
    if (line[0] == '\n')
        return true;

    while (*line != '\n')
        if (*line++ != ' ')
            return false;
    return true;
}

int main(int argc,char *argv[])
{
    FILE    *in;
    long    total_lines,comment_lines,blank_lines;
    char    line[LINE_SIZE],*p;

    if (argc != 2) {
        printf("Usage: loc <filename>\n");
        exit(1);
        }

    if ((in = fopen(argv[1],"r")) == NULL) {
        printf("Unable to open input file\n");
        exit(1);
        }

    total_lines = comment_lines = blank_lines = 0;

    p = argv[1];
    while (*p) {
        if (*p == '.' && stricmp(p,".asm") == 0) {
            asm_file = true;
            break;
            }
        p++;
        }

    while (!feof(in)) {
        fgets(line,LINE_SIZE,in);
        if (comment_line(line))
            comment_lines++;
        else if (blank_line(line))
            blank_lines++;
        total_lines++;
        }

    printf("Total lines:   %ld\n", total_lines);
    printf("Comment lines: %ld\n", comment_lines);
    printf("Blank lines:   %ld\n", blank_lines);

    fclose(in);
    return 0;
}
