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
* Environment:  Win32 Console
*
* Description:  Simple program to set global user environment variables
*               for Windows NT systems.
*
****************************************************************************/

#define STRICT
#define WIN32_LEAN_AND_MEAN
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <windows.h>

/*------------------------------ Implementation ---------------------------*/

/****************************************************************************
PARAMETERS:
hKey        - Key to open registry key to set data in
szValue     - Value to get information for
value       - New value to store in registry
****************************************************************************/
void REG_setStringEx(
    HKEY hKey,
    const char *szValue,
    const char *value)
{
    RegSetValueEx(hKey,(PCHAR)szValue,0,REG_SZ,(PUCHAR)value,strlen(value));
}

/****************************************************************************
PARAMETERS:
szKey       - Key to query (can contain version number formatting)
szValue     - Value to get information for
value       - New value to store in registry
****************************************************************************/
void REG_setString(
    HKEY hKeyRoot,
    const char *szKey,
    const char *szValue,
    const char *value)
{
    HKEY    hKey;
    if (RegOpenKey(hKeyRoot,(PCHAR)szKey,&hKey) == ERROR_SUCCESS) {
        RegSetValueEx(hKey,(PCHAR)szValue,0,REG_SZ,(PUCHAR)value,strlen(value));
        RegCloseKey(hKey);
        }
}

/****************************************************************************
REMARKS:
Safely copies a string to a destination without overrunning the length. Also
ensures the copied string is correctly terminated.
****************************************************************************/
char *safe_strncpy(
    char *dst,
    const char *src,
    int maxlen)
{
    strncpy(dst,src,maxlen);
    dst[maxlen-1] = 0;
    return dst;
}

/****************************************************************************
REMARKS:
Display command line usage.
****************************************************************************/
void help(void)
{
    printf("Usage: ntset <variable>=<value>\n");
    exit(-1);
}

int main(int argc, char *argv[])
{
    char    variable[256];
    char    *value,*p;
    DWORD   dwReturnValue;

    // Parse the command line
    if (argc < 2)
        help();
    safe_strncpy(variable,argv[1],sizeof(variable));
    if ((p = strchr(variable,'=')) == NULL)
        help();
    value = p+1;
    *p = 0;
    while (isspace(*(p-1)))
        *--p = 0;
    while (isspace(*value) && *value != 0)
        value++;

    // Now set the environment variable for the current user
    REG_setString(HKEY_CURRENT_USER,"Environment",variable,value);

    // Now propagate the changes to the system
    SendMessageTimeout(HWND_BROADCAST,WM_SETTINGCHANGE,0,(LPARAM)"Environment",SMTO_ABORTIFHUNG, 5000, &dwReturnValue);
    return 0;
}
