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
* Environment:  32-bit Windows NT driver
*
* Description:  C library compatible stdlib.h functions for use within a
*               Windows NT driver.
*
****************************************************************************/

#include "pmapi.h"
#include "oshdr.h"

/*------------------------ Main Code Implementation -----------------------*/

/****************************************************************************
REMARKS:
PM_malloc override function for SNAP drivers loaded in NT drivers's.
****************************************************************************/
void * malloc(
    size_t size)
{
    return PM_mallocShared(size);
}

/****************************************************************************
REMARKS:
calloc library function for SNAP drivers loaded in NT drivers's.
****************************************************************************/
void * calloc(
    size_t nelem,
    size_t size)
{
    void *p = PM_mallocShared(nelem * size);
    if (p)
        memset(p,0,nelem * size);
    return p;
}

/****************************************************************************
REMARKS:
PM_realloc override function for SNAP drivers loaded in VxD's.
****************************************************************************/
void * realloc(
    void *ptr,
    size_t size)
{
    void *p = PM_mallocShared(size);
    if (p) {
        memcpy(p,ptr,size);
        PM_freeShared(ptr);
        }
    return p;
}

/****************************************************************************
REMARKS:
PM_free override function for SNAP drivers loaded in VxD's.
****************************************************************************/
void free(
    void *p)
{
    PM_freeShared(p);
}

/****************************************************************************
PARAMETERS:
cstr    - C style ANSI string to convert

RETURNS:
Pointer to the UniCode string structure or NULL on failure to allocate memory

REMARKS:
Converts a C style string to a UniCode string structure that can be passed
directly to NT kernel functions.
****************************************************************************/
UNICODE_STRING *_PM_CStringToUnicodeString(
    const char *cstr)
{
    int             length;
    ANSI_STRING     ansiStr;
    UNICODE_STRING  *uniStr;

    // Allocate memory for the string structure
    if ((uniStr = ExAllocatePool(NonPagedPool, sizeof(UNICODE_STRING))) == NULL)
        return NULL;

    // Allocate memory for the wide string itself
    length = (strlen(cstr) * sizeof(WCHAR)) + sizeof(WCHAR);
    if ((uniStr->Buffer = ExAllocatePool(NonPagedPool, length)) == NULL) {
        ExFreePool(uniStr);
        return NULL;
        }
    RtlZeroMemory(uniStr->Buffer, length);
    uniStr->Length = 0;
    uniStr->MaximumLength = (USHORT)length;

    // Convert filename string to ansi string and then to UniCode string
    RtlInitAnsiString(&ansiStr, cstr);
    RtlAnsiStringToUnicodeString(uniStr, &ansiStr, FALSE);
    return uniStr;
}

/****************************************************************************
PARAMETERS:
uniStr  - UniCode string structure to free

REMARKS:
Frees a string allocated by the above _PM_CStringToUnicodeString function.
****************************************************************************/
void _PM_FreeUnicodeString(
    UNICODE_STRING *uniStr)
{
    if (uniStr) {
        ExFreePool(uniStr->Buffer);
        ExFreePool(uniStr);
        }
}

