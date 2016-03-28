/****************************************************************************
*
*                   SciTech OS Portability Manager Library
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
* Environment:  Linux
*
* Description:  Program to restore the console state state from a previously
*               saved state if the program crashed while the console
*               was in graphics mode.
*
****************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/kd.h>
#include <sys/ioctl.h>
#include "pmapi.h"

void setVideoMode(int mode)
{
    RMREGS r;

    r.x.ax = mode;
    PM_int86(0x10, &r, &r);
}

int main(void)
{
    PM_HWND         hwndConsole;
    ulong           stateSize;
    void            *stateBuf;
    void            *badStateBuf;
    FILE            *f;
    int             fd;
    struct termios  termio;

    /* Forcibly restore the current console keyboard state */
    if ((fd = open("/dev/tty0", O_RDWR)) < 0) {
        printf("Unable to open /dev/tty0!\n");
        return -1;
        }
    ioctl(fd, KDSETMODE, KD_TEXT);
    ioctl(fd, KDSKBMODE, K_XLATE);
    tcgetattr(fd, &termio);
    termio.c_iflag = 0x500;
    termio.c_cflag = 0x4BF;
    termio.c_lflag = 0x8A3B;
    tcsetattr(fd, TCSANOW, &termio);
    close(fd);

    /* Read the saved console state buffer from disk */
    if ((f = fopen("/etc/pmsave.dat","rb")) == NULL) {
        printf("Unable to open /etc/pmsave.dat for reading!\n");
        return -1;
        }
    fread(&stateSize,1,sizeof(stateSize),f);
    if (stateSize != PM_getConsoleStateSize()) {
        printf("Size mismatch in /etc/pmsave.dat!\n");
        return -1;
        }
    if ((stateBuf = PM_malloc(stateSize)) == NULL) {
        printf("Unable to allocate console state buffer!\n");
        return -1;
        }
    fread(stateBuf,1,stateSize,f);
    fclose(f);

    /* Open the console */
    hwndConsole = PM_openConsole(0,0,0,0,0,true);

    /* Allocate a buffer to save console state and save the state */
    stateSize = PM_getConsoleStateSize();
    if ((badStateBuf = PM_malloc(stateSize)) == NULL) {
        PM_closeConsole(hwndConsole);
        printf("Unable to allocate console state buffer!\n");
        return -1;
        }
    PM_saveConsoleState(badStateBuf,0);

    /* Forcibly set 80x25 text mode using the BIOS */
    setVideoMode(0x3);

    /* Restore the previous console state */
    PM_restoreConsoleState(stateBuf,0);
    PM_closeConsole(hwndConsole);
    PM_free(stateBuf);
    printf("Console state successfully restored from /etc/pmsave.dat\n");
    return 0;
}

