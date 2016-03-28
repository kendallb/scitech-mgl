/****************************************************************************
*
*                          Console Output Library
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
* Environment:  IBM PC (MS DOS)
*
* Description:  Test program for the console output library.
*
****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "console.h"
#include "pmapi.h"

/*---------------------------- Implementation -----------------------------*/

void test_CON_printf(void)
{
    int     i;

    CON_printf("This is a test of CON_printf() \"%s\" '%c' %d\n",
        "String", 'C', 10);
    for (i = 0; i < 50; i++)
        CON_printf("CON_printf() line number %d ... \n", i);
}

void test_CON_puts(void)
{
    int     i;

    for (i = 0; i < 50; i++)
        CON_puts("CON_puts() for fast Strings! ");
}

void test_CON_puts_word_wrap(void)
{
    int     i;

    CON_puts("\n\n");
    CON_setLineWrap(CON_WORD_WRAP);
    for (i = 0; i < 50; i++)
        CON_puts("CON_puts() for fast Strings! ");
    CON_setLineWrap(CON_LINE_WRAP);
}

void test_CON_putc(void)
{
    int     i;

    for (i = 0; i < 100; i++)
        CON_putc('c');
}

void test_CON_writec(void)
{
    int     i;

    for (i = 0; i < 80; i++) {
        CON_writec(i, 0, CON_makeAttr(CON_CYAN, CON_BLUE), '°');
        CON_writec(i, 1, CON_makeAttr(CON_CYAN, CON_BLUE), '±');
        }
}

void test_CON_write(void)
{
    int     i;

    for (i = 2; i < CON_maxy() + 1; i++)
        CON_write(0, i, CON_makeAttr(CON_RED, CON_BLUE),
            "°°°°°°°°±±±±±±±±±±±²²²²²²²²²²²²²");
}

void test_CON_clreol(void)
{
    CON_getch();
    CON_gotoxy(0, 1);
    CON_clreol();
}

void test_CON_delline(void)
{
    CON_getch();
    CON_delline();
}

void test_CON_moveText(void)
{
    int     i;

    CON_getch();
    for (i = 0; i < 40; i++)
        CON_moveText(i, i, i+10, i+10, i+1,i+1);
    CON_getch();
    for (i = 39; i > 0; i--)
        CON_moveText(i, i, i+10, i+10, i-1,i-1);
    CON_getch();
    for (i = 0; i < 70; i++)
        CON_moveText(i, 0, i+10, 10, i+1, 0);
}

void test_CON_saveText(void)
{
    char    *savebuf;

    CON_getch();
    savebuf = malloc(CON_bufSize(CON_maxy()+1,CON_maxx()+1));

    CON_saveText(0, 0, CON_maxx(), CON_maxy(), savebuf);
    CON_clrscr();
    CON_getch();
    CON_restoreText(0, 0, CON_maxx(), CON_maxy(), savebuf);

    free(savebuf);
}

void test_CON_scroll(void)
{
    int i;

    CON_getch();
    CON_setBackground(CON_makeAttr(CON_CYAN, CON_BLUE), '°');
    for (i = 0; i < CON_maxy() + 1; i++)
        CON_scroll(CON_SCROLL_UP, 1);

    CON_getch();
    CON_setBackground(CON_makeAttr(CON_CYAN, CON_BLUE), '±');
    for (i = 0; i < CON_maxy() + 1; i++)
        CON_scroll(CON_SCROLL_DOWN, 1);

    CON_getch();
    CON_setBackground(CON_makeAttr(CON_RED, CON_BLUE), '±');
    for (i = 0; i < CON_maxx() + 1; i++)
        CON_scroll(CON_SCROLL_RIGHT, 1);

    CON_getch();
    CON_setBackground(CON_makeAttr(CON_RED, CON_BLUE), '°');
    for (i = 0; i < CON_maxx() + 1; i++)
        CON_scroll(CON_SCROLL_LEFT, 1);
}

void test_CON_fillText(void)
{
    CON_getch();
    CON_fillText(0, 0, CON_maxx(), CON_maxy(),
        CON_makeAttr(CON_CYAN, CON_BLUE), '°');
}

void test_CON_fillAttr(void)
{
    CON_getch();
    CON_fillAttr(0, 0, CON_maxx(), CON_maxy(),
        CON_makeAttr(CON_RED, CON_BLUE));
}

void do_tests(void)
{
    int oldCursor;

    test_CON_printf();
    test_CON_puts();
    test_CON_puts_word_wrap();
    test_CON_putc();
    CON_getch();
    CON_clrscr();
    oldCursor = CON_getCursor();
    CON_cursorOff();
    test_CON_writec();
    test_CON_write();
    test_CON_clreol();
    test_CON_delline();
    test_CON_moveText();
    test_CON_saveText();
    test_CON_scroll();
    test_CON_fillText();
    test_CON_fillAttr();
    CON_restoreCursor(oldCursor);
}

void do_tests_in_window(void)
{
    CON_setBackground(CON_makeAttr(CON_LIGHTGRAY, CON_BLACK), ' ');
    CON_setWindow(10, 10, CON_screenWidth()-10, CON_screenHeight()-10);
    CON_getch();
    CON_setForeColor(CON_RED);
    CON_setBackColor(CON_BLUE);
    CON_setAttr(CON_makeAttr(CON_RED, CON_BLUE));
    do_tests();
}

void main(int argc,char *argv[])
{
    PM_HWND hwndConsole;
    int     stateSize;
    void    *stateBuf;

    /* Initialise the console output routines */
    hwndConsole = PM_openConsole(0,0,0,0,0,true);
    stateSize = PM_getConsoleStateSize();
    if ((stateBuf = malloc(stateSize)) == NULL)
        PM_fatalError("Out of memory!\n");
    PM_saveConsoleState(stateBuf,hwndConsole);
    PM_init();
    CON_init();
    if (argc > 1) {
        if (atoi(argv[1]) == 25)
            CON_set25LineMode();
        else if (atoi(argv[1]) == 43)
            CON_set43LineMode();
        else if (atoi(argv[1]) == 50)
            CON_set50LineMode();
        }
    do_tests();
    do_tests_in_window();
    CON_getch();
    CON_restoreMode();
    PM_restoreConsoleState(stateBuf,hwndConsole);
    PM_closeConsole(hwndConsole);
    free(stateBuf);
}

