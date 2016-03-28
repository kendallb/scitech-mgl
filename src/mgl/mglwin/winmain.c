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
* Environment:  Win32
*
* Description:  Simple module to call the regular C style main() function
*               for Windows programs.
*
****************************************************************************/

#include "gm/gm.h"
#include "cmdline.h"

#ifdef  __WINDOWS__
#undef  WINGDIAPI
#undef  APIENTRY
#undef  STRICT
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

/*--------------------------- Global Variables ----------------------------*/

#define MAX_ARGV    20
static  char            *argv[MAX_ARGV];
static  int             argc;
extern  ibool           __glutDone;

/*------------------------- Implementation --------------------------------*/

extern int main(int argc,char *argv[]);

int PASCAL WinMain(HINSTANCE hInst,HINSTANCE hPrev,LPSTR szCmdLine,int sw)
{
    char        szModuleName[260];
    GetModuleFileName(hInst,szModuleName,sizeof(szModuleName));
    parse_commandline(szModuleName,szCmdLine,&argc,argv,MAX_ARGV);
    return main(argc,argv);
}

#endif  /* __WINDOWS__ */


