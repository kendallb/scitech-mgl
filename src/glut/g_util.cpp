/****************************************************************************
*
*                   SciTech Multi-platform Graphics Library
*
*  ========================================================================
*
*    The contents of this file are subject to the SciTech MGL Public
*    License Version 1.0 (the "License"); you may not use this file
*    except in compliance with the License. You may obtain a copy of
*    the License at http://www.scitechsoft.com/mgl-license.txt
*
*    Software distributed under the License is distributed on an
*    "AS IS" basis, WITHOUT WARRANTY OF ANY KIND, either express or
*    implied. See the License for the specific language governing
*    rights and limitations under the License.
*
*    The Original Code is Copyright (C) 1991-2004 SciTech Software, Inc.
*
*    The Initial Developer of the Original Code is SciTech Software, Inc.
*    All Rights Reserved.
*
*  ========================================================================
*
*                Portions Copyright (c) Mark J. Kilgard, 1994.
*
* Language:     ANSI C
* Environment:  Any
*
* Description:  SciTech MGL minimal GLUT implementation for fullscreen
*               and windowed operation.
*
****************************************************************************/

#include <stdarg.h>
#include "glutint.h"

/*------------------------- Implementation --------------------------------*/

void __glutWarning(char *format,...)
{
    va_list args;
    FILE    *f;

    if ((f = fopen("mgl_glut.log","a+t")) == NULL)
        exit(1);
    va_start(args, format);
    fprintf(f, "GLUT: Warning in %s: ",
        __glutProgramName ? __glutProgramName : "(unamed)");
    vfprintf(f, format, args);
    va_end(args);
    putc('\n', f);
    fclose(f);
}

void __glutFatalError(char *format,...)
{
    va_list args;
    char    buf1[128],buf2[128];

    va_start(args, format);
    sprintf(buf1, "GLUT: Fatal Error in %s: ",
        __glutProgramName ? __glutProgramName : "(unamed)");
    vsprintf(buf2, format, args);
    va_end(args);
    strcat(buf1,buf2);
    MGL_fatalError(buf1);
}

void __glutFatalUsage(char *format,...)
{
    va_list args;
    char    buf1[128],buf2[128];

    va_start(args, format);
    sprintf(buf1, "GLUT: Fatal API Usage in %s: ",
        __glutProgramName ? __glutProgramName : "(unamed)");
    vsprintf(buf2, format, args);
    va_end(args);
    strcat(buf1,buf2);
    MGL_fatalError(buf1);
}


