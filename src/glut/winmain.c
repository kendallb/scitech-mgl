/****************************************************************************
*
*                       SciTech MGL Graphics Library
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
*                Portions Copyright (c) Mark J. Kilgard, 1994.
*
*
* Language:     ANSI C
* Environment:  IBM PC (MS DOS)
*
* Description:  SciTech MGL minimal GLUT implementation for fullscreen
*               and windowed operation.
*
*
****************************************************************************/

#include "scitech.h"
#ifdef  __WINDOWS__
#define STRICT
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "GL/glut.h"
#include "cmdline.h"

/*--------------------------- Global Variables ----------------------------*/

#define MAX_ARGV    20
PRIVATE char            *argv[MAX_ARGV];
PRIVATE int             argc;
extern  ibool           __glutDone;

/*------------------------- Implementation --------------------------------*/

extern int main(int argc,char *argv[]);

int PASCAL WinMain(HINSTANCE hInst,HINSTANCE hPrev,LPSTR szCmdLine,int sw)
{
    char        szModuleName[260];
    GetModuleFileName(hInst,szModuleName,sizeof(szModuleName));
    while (!__glutDone) {
        parse_commandline(szModuleName,szCmdLine,&argc,argv,MAX_ARGV);
        main(argc,argv);
        }
    return 0;
}

#endif  /* __WINDOWS__ */


