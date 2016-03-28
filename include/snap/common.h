/****************************************************************************
*
*                     SciTech SNAP Graphics Architecture
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
* Environment:  Any 32-bit protected mode environment
*
* Description:  Common header file for the SciTech SNAP Driver
*               Architecture, shared by all device driver level layers.
*
*               All SciTech SNAP drivers are loaded by the appropriate loader
*               library for the desired type of device support. All loader
*               libraries are Binary Portable DLL's that export an API
*               to load and unload drivers.
*
*               All SciTech SNAP device driver neutral API's, macros and
*               functions all start with an 'N_' or 'n' to indicate they
*               are part of the SciTech SNAP Neutral API.
*
****************************************************************************/

#ifndef __SNAP_COMMON_H
#define __SNAP_COMMON_H

#ifndef __WINDOWS16__
#include "pmapi.h"
#else
/* {secret} */
typedef void        *PM_HWND;
/* {secret} */
typedef void        *PM_MODULE;
#endif

/*---------------------- Macros and type definitions ----------------------*/

/* Define the calling conventions for the code in this module */

#ifdef  __16BIT__
#define _FAR_   far
#else
#define _FAR_
#endif

#define NAPI    _ASMAPI             /* 'C' calling conventions always   */
#ifdef  PTR_DECL_IN_FRONT
#define NAPIP   * _ASMAPI _FAR_
#else
#define NAPIP   _ASMAPI _FAR_ *
#endif

/* Name of SNAP environment variable */

#define SNAP_PATH           "SNAP_PATH"

/* Macros to convert between integer and 32 bit fixed point format */

#define N_FIX_1             0x10000L
#define N_FIX_2             0x20000L
#define N_FIX_HALF          0x08000L
#define N_TOFIX(i)          ((long)(i) << 16)
#define N_FIXTOINT(f)       ((N_int32)((f) >> 16))
#define N_FIXROUND(f)       ((N_int32)(((f) + N_FIX_HALF) >> 16))
#define N_FIXCONST(num)     (long)((num) * 65536.0 + 0.5)
#define N_FLTTOFIX(num)     (long)((num) * 65536.0 + 0.5)
#define N_FIXTOFLT(f)       ((float)((f) / 65536.0))
#define N_FLOOR(f)          ((f) & 0xFFFF0000)
#define N_CEIL(f)           N_FLOOR((f) + N_FIX_HALF)

/****************************************************************************
REMARKS:
Fundamental type definition for an 8-bit signed value.

HEADER:
snap/common.h
****************************************************************************/
typedef char N_int8;

/****************************************************************************
REMARKS:
Fundamental type definition for a 16-bit signed value.

HEADER:
snap/common.h
****************************************************************************/
typedef short N_int16;

/****************************************************************************
REMARKS:
Fundamental type definition for a 32-bit signed value.

HEADER:
snap/common.h
****************************************************************************/
#ifdef  __16BIT__
typedef long N_int32;
#else
typedef int  N_int32;
#endif

/****************************************************************************
REMARKS:
Fundamental type definition for an 8-bit unsigned value.

HEADER:
snap/common.h
****************************************************************************/
typedef unsigned char N_uint8;

/****************************************************************************
REMARKS:
Fundamental type definition for a 16-bit unsigned value.

HEADER:
snap/common.h
****************************************************************************/
typedef unsigned short N_uint16;

/****************************************************************************
REMARKS:
Fundamental type definition for a 32-bit unsigned value.

HEADER:
snap/common.h
****************************************************************************/
#ifdef  __16BIT__
typedef unsigned long N_uint32;
#else
typedef unsigned int N_uint32;
#endif

/****************************************************************************
REMARKS:
Fundamental type definition for a 64-bit unsigned value. Only supported
where native 64-bit integer support is available in a compiler.

HEADER:
snap/common.h
****************************************************************************/
#ifdef __HAS_LONG_LONG__
typedef unsigned long long N_uint64;
#endif

/****************************************************************************
REMARKS:
Fundamental type definition for a 64-bit signed value. Only supported
where native 64-bit integer support is available in a compiler.

HEADER:
snap/common.h
****************************************************************************/
#ifdef __HAS_LONG_LONG__
typedef signed long long N_int64;
#endif

/****************************************************************************
REMARKS:
Fundamental type definition for an unsigned integer that can hold a pointer.

HEADER:
snap/common.h
****************************************************************************/
typedef unsigned long N_uintptr;

/****************************************************************************
REMARKS:
Fundamental type definition for a system physical address

HEADER:
snap/common.h
****************************************************************************/
typedef unsigned long N_physAddr;

/****************************************************************************
REMARKS:
Fundamental type definition for a 32-bit fixed point value. The fixed point
value is interpreted as a 16.16 fixed point number, with 16 integral bits
and 16 fractional bits.

HEADER:
snap/common.h
****************************************************************************/
#ifdef  __16BIT__
typedef long N_fix32;
#else
typedef int N_fix32;
#endif

/****************************************************************************
REMARKS:
Fundamental type definition for a 32-bit floating point number. The number
is stored as an IEEE 754 floating point number with 1 sign bit, 8 exponent
bits and 23 mantissa bits.

HEADER:
snap/common.h
****************************************************************************/
typedef float N_flt32;

/****************************************************************************
REMARKS:
Fundamental type definition for a spin lock variable

HEADER:
snap/common.h
****************************************************************************/
#ifdef __PPC__
typedef N_uint32 N_spinlock;
#else
typedef char N_spinlock;
#endif

/****************************************************************************
REMARKS:
Error codes returned by N_status to indicate the driver load status if
loading the device driver failed.

HEADER:
snap/common.h

MEMBERS:
nOK                 - No error
nNotDetected        - Hardware not detected
nNotPOSTed          - Hardware has not been POSTed
nDriverNotFound     - Driver file not found
nCorruptDriver      - File loaded not a driver file
nLoadMem            - Not enough memory to load driver
nOldVersion         - Driver file is an older version
nMemMapError        - Could not map physical memory areas
nIOError            - General I/O error
nIRQHookFailed      - Could not hook required hardware IRQ
nNotCertified       - Driver is not certified
nInternalError      - Internal device driver error
nOutOfMemory        - Not enough memory to complete operation
nOutOfResources     - Not enough spare resources to complete operation
nInvalidParameter   - Invalid parameter passed to function
nNoAGPSupport       - No AGP support services found
nInvalidLicense     - License is invalid
nNotLicensed        - Feature is not licensed
****************************************************************************/
typedef enum {
    nOK,
    nNotDetected,
    nNotPOSTed,
    nDriverNotFound,
    nCorruptDriver,
    nLoadMem,
    nOldVersion,
    nMemMapError,
    nIOError,
    nIRQHookFailed,
    nNotCertified,
    nInternalError,
    nOutOfMemory,
    nOutOfResources,
    nInvalidParameter,
    nNoAGPSupport,
    nInvalidLicense,
    nNotLicensed
    } N_errorType;

#ifndef __WINDOWS16__
/****************************************************************************
REMARKS:
Structure defining all the SciTech SNAP Generic functions as imported into
the Binary Portable DLL.
{secret}
****************************************************************************/
typedef struct {
    N_uintptr   dwSize;
    void        (NAPIP _OS_delay)(N_uint32 microSeconds);
    } N_imports;
#endif

/*-------------------------- Function Prototypes --------------------------*/

#ifdef  __cplusplus
extern "C" {                        /* Use "C" linkage when in C++ mode */
#endif

/* Generic SciTech SNAP helper functions. These functions are implemented
 * in the OS specific modules and exported to the Binary Portable DLL.
 */

/* {secret} */
const char *    NAPI N_getLogName(void);
/* {secret} */
void            NAPI N_log(const char *fmt,...);
/* {secret} */
void            NAPI N_openLog(ibool safeLogging);
/* {secret} */
void            NAPI N_closeLog(void);
/* {secret} */
void            NAPI _OS_delay(N_uint32 microSeconds);
/* {secret} */
const char *    N_errorMsg(N_int32 status);

/* Watcom C++ specific fixed point multiplication/divide routines */

#if defined(__WATCOMC__) && defined(__386__) && !defined(NOINLINEASM)
/* {secret} */
N_fix32 N_FixMul(N_fix32 a,N_fix32 b);
#pragma aux N_FixMul =          \
    "imul   edx"                    \
    "add    eax,8000h"              \
    "adc    edx,0"                  \
    "shrd   eax,edx,16"             \
    parm [eax] [edx]                \
    value [eax]                     \
    modify exact [eax edx];

/* {secret} */
N_fix32 N_FixDiv(N_fix32 a,N_fix32 b);
#pragma aux N_FixDiv =          \
    "xor    eax,eax"                \
    "shrd   eax,edx,16"             \
    "sar    edx,16"                 \
    "idiv   ebx"                    \
    parm [edx] [ebx]                \
    value [eax]                     \
    modify exact [eax edx];

/* {secret} */
N_fix32 N_FixMulDiv(N_fix32 a,N_fix32 b,N_fix32 c);
#pragma aux N_FixMulDiv =           \
    "imul   ebx"                    \
    "idiv   ecx"                    \
    parm [eax] [ebx] [ecx]          \
    value [eax]                     \
    modify exact [eax edx];
#endif

#if defined(TEST_HARNESS) && defined(__OS2__)
/* When compiling in OS/2 test harness mode, we need to re-direct all
 * internal malloc calls to SDDPMI_malloc to ensure all the
 * memory is shared. A macro is used so that all other
 * code that is not related to SciTech SNAP will use the regular malloc
 * functions from the C library.
 */
/* {secret} */
void * SDDPMI_malloc(size_t size);
/* {secret} */
void * SDDPMI_calloc(size_t nelem,size_t size);
/* {secret} */
void * SDDPMI_realloc(void *ptr,size_t size);
/* {secret} */
void SDDPMI_free(void *p);
#define malloc(size)        SDDPMI_malloc(size)
#define calloc(nelem,size)  SDDPMI_calloc(nelem,size)
#define realloc(ptr,size)   SDDPMI_realloc(ptr,size)
#define free(p)             SDDPMI_free(p)
#endif

#ifdef  __cplusplus
}                                   /* End of "C" linkage for C++       */
#endif

#endif  /* __SNAP_COMMON_H */

