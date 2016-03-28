/****************************************************************************
*
*                           SciTech SNAP Graphics
*
*               Copyright (C) 1991-2003 SciTech Software, Inc.
*                            All rights reserved.
*
*  ======================================================================
*  |REMOVAL OR MODIFICATION OF THIS HEADER IS STRICTLY PROHIBITED BY LAW|
*  |                                                                    |
*  |This copyrighted computer code is a proprietary trade secret of     |
*  |SciTech Software, Inc., located at 505 Wall Street, Chico, CA 95928 |
*  |USA (www.scitechsoft.com).  ANY UNAUTHORIZED POSSESSION, USE,       |
*  |VIEWING, COPYING, MODIFICATION OR DISSEMINATION OF THIS CODE IS     |
*  |STRICTLY PROHIBITED BY LAW.  Unless you have current, express       |
*  |written authorization from SciTech to possess or use this code, you |
*  |may be subject to civil and/or criminal penalties.                  |
*  |                                                                    |
*  |If you received this code in error or you would like to report      |
*  |improper use, please immediately contact SciTech Software, Inc. at  |
*  |530-894-8400.                                                       |
*  |                                                                    |
*  |REMOVAL OR MODIFICATION OF THIS HEADER IS STRICTLY PROHIBITED BY LAW|
*  ======================================================================
*
* Language:     ANSI C
* Environment:  Any
*
* Description:  Simple main test program to load a Portable Binary module
*               and run the test programs contained within it.
*
****************************************************************************/

#include <clib/modloadr.h>
#include <stdlib.h>
#include <stdio.h>

/*------------------------- Implementation --------------------------------*/

void RunTest(MOD_MODULE *hMod, const char *name)
{
    void    (SNAP_CDECL *pFunc)(void);

    if ((pFunc = MOD_getProcAddress(hMod, name)) == NULL) {
        printf("Unable to get procedure address\n");
        exit(-1);
        }
    pFunc();
}

void RunDLLTests(MOD_MODULE *hMod)
{
    RunTest(hMod, MOD_FNPREFIX"HelloWorld");
    RunTest(hMod, MOD_FNPREFIX"TestMalloc");
    RunTest(hMod, MOD_FNPREFIX"TestMath");
    RunTest(hMod, MOD_FNPREFIX"TestLongjmp");
//  RunTest(hMod, MOD_FNPREFIX"TestStdlib");
//  RunTest(hMod, MOD_FNPREFIX"TestStdio");
    fflush(stdout);
}

int main(int argc, char **argv)
{
    MOD_MODULE   *hMod;

    if (argc < 2) {
        printf("Usage: %s <module>\n", argv[0]);
        return 1;
        }
    if ((hMod = MOD_loadLibrary(argv[1], false)) == NULL) {
        printf("Unable to load %s\n", argv[1]);
        return 2;
        }

    RunDLLTests(hMod);

    MOD_freeLibrary(hMod);
    return 0;
}
