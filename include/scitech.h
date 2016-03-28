/****************************************************************************
*
*                      SciTech Common Header File
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
* Description:  General header file for operating system portable code.
*
****************************************************************************/

#ifndef __SCITECH_H
#define __SCITECH_H

/* We have the following defines to identify the compilation environment:
 *
 *  __16BIT__       Compiling for 16 bit code (any environment)
 *  __32BIT__       Compiling for 32 bit code (any environment)
 *  __64BIT__       Compiling for 64 bit code (any environment)
 *  __MSDOS__       Compiling for MS-DOS (includes __WINDOWS16__, __WIN386__)
 *  __REALDOS__     Compiling for MS-DOS (excludes __WINDOWS16__)
 *  __MSDOS16__     Compiling for 16 bit MS-DOS
 *  __MSDOS32__     Compiling for 32 bit MS-DOS
 *  __WINDOWS__     Compiling for Windows
 *  __WINDOWS16__   Compiling for 16 bit Windows (__MSDOS__ also defined)
 *  __WINDOWS32__   Compiling for 32 bit Windows
 *  __WIN32_VXD__   Compiling for a 32-bit C based VxD
 *  __NT_DRIVER__   Compiling for a 32-bit C based NT device driver
 *  __WINCE__       Compiling for the Windows CE 32-bit embedded OS
 *  __OS2__         Compiling for OS/2
 *  __OS2_16__      Compiling for 16 bit OS/2
 *  __OS2_32__      Compiling for 32 bit OS/2
 *  __UUU__         Compiling for the Unununium OS
 *  __UNIX__        Compiling for Unix
 *  __QNX__         Compiling for the QNX realtime OS (Unix compatible)
 *  __LINUX__       Compiling for the Linux OS (Unix compatible)
 *  __FREEBSD__     Compiling for the FreeBSD OS (Unix compatible)
 *  __BEOS__        Compiling for the BeOS (Unix compatible)
 *  __SMX32__       Compiling for the SMX 32-bit Real Time OS
 *  __ENEA_OSE__    Compiling for the OSE embedded OS
 *  __RTTARGET__    Compiling for the RTTarget 32-bit embedded OS
 *  __MACOS__       Compiling for the MacOS platform (PowerPC)
 *  __DRIVER__      Compiling for a 32-bit binary compatible driver
 *  __CONSOLE__     Compiling for a fullscreen OS console mode
 *  __SNAP__        Compiling as a Snap executeable or dynamic library
 *  __XFREE86__     Compiling for XFree86 driver module
 *
 *  __INTEL__       Compiling for Intel IA32 CPU's
 *  __ALPHA__       Compiling for DEC Alpha CPU's
 *  __MIPS__        Compiling for MIPS CPU's
 *  __PPC__         Compiling for PowerPC CPU's
 *  __MC68K__       Compiling for Motorola 680x0
 *  __X86_64__      Compiling for AMD64/X86-64
 *
 *  __BIG_ENDIAN__  Compiling for a big endian processor
 *
 */

#ifdef  __SC__
#if     __INTSIZE == 4
#define __SC386__
#endif
#endif

/* Determine the CPU type that we are compiling for */

#if     defined(__M_ALPHA) || defined(__ALPHA_) || defined(__ALPHA) || defined(__alpha)
#ifndef __ALPHA__
#define __ALPHA__
#endif
#elif   defined(__M_PPC) || defined(__POWERC) || defined(powerpc)
#ifndef __PPC__
#define __PPC__
#endif
#elif   defined(__M_MRX000) || defined(__mips__)
#ifndef __MIPS__
#define __MIPS__
#endif
#elif   defined(__X86_64__) || defined(__AMD64__) || defined(__x86_64__)
#define __64BIT__
#ifndef __X86_64__
#define __X86_64__
#endif
#else
#ifndef __INTEL__
#define __INTEL__               /* Assume Intel if nothing found */
#ifndef __i386__
#define __i386__                /* Compatible with Linux kernel  */
#endif
#endif
#endif

/* Determine some things that are compiler specific */

#ifdef  __GNUC__
#undef  __cdecl
#undef  __stdcall
#if !defined(__INTEL__)
#   define __cdecl          /* Non-Intel compiler ignores both attributes   */
#   define __stdcall        /* __cdecl and __stdcall                        */
#   define __export         /* No need to explicitly export symbols         */
#else
#if defined(OLD_GPLUSPLUS_COMPILER)
#   define __cdecl          /* Older G++ compilers screws this up      */
#   define __stdcall
#else
#   define __cdecl     __attribute__ ((cdecl))
#   define __stdcall   __attribute__ ((stdcall))
#endif
#endif
#define __FLAT__            /* GCC is always 32 bit flat model          */
#define __HAS_BOOL__        /* Latest GNU C++ has bool type             */
#ifndef __STRICT_ANSI__
#define __HAS_LONG_LONG__   /* GNU C supports long long type            */
#endif
#if !defined(XFree86Module) && !defined(__KERNEL__)
#include <stdio.h>          /* Bring in for definition of NULL          */
#endif
#endif

#ifdef  __BORLANDC__
#if (__BORLANDC__ >= 0x500) || defined(CLASSLIB_DEFS_H)
#define __HAS_BOOL__        /* Borland C++ 5.0 defines bool type        */
#endif
#if (__BORLANDC__ >= 0x502) && !defined(VTOOLSD) && !defined(__SMX32__)
#define __HAS_INT64__       /* Borland C++ 5.02 supports __int64 type   */
#endif
#endif

#if defined(_MSC_VER) && !defined(__SC__) && !defined(VTOOLSD) && !defined(__SMX32__)
#define __HAS_INT64__       /* Visual C++ supports __int64 type         */
#endif

#if defined(__WATCOMC__) && (__WATCOMC__ >= 1100) && !defined(VTOOLSD) && !defined(__SMX32__)
#define __HAS_INT64__       /* Watcom C++ 11.0 supports __int64 type    */
#define __HAS_BOOL__        /* Watcom C++ 11.0 has bool type            */
#endif

/*---------------------------------------------------------------------------
 * Determine the compile time environment. This must be done for each
 * supported platform so that we can determine at compile time the target
 * environment, hopefully without requiring #define's from the user.
 *-------------------------------------------------------------------------*/

/* 32-bit binary compatible driver. Compiled as Win32, but as OS neutral */
#ifdef  __DRIVER__
#ifndef __32BIT__
#define __32BIT__
#endif
#undef  __WINDOWS__
#undef  _WIN32
#undef  __WIN32__
#undef  __NT__

/* 32-bit Snap exe or dll. Compiled as Win32, but as OS neutral */
#elif   defined(__SNAP__)
#ifndef __32BIT__
#define __32BIT__
#endif
#undef  __WINDOWS__
#undef  _WIN32
#undef  __WIN32__
#undef  __NT__

/* 32-bit Windows VxD compile environment */
#elif   defined(__vtoolsd_h_) || defined(VTOOLSD)
#include <vtoolsc.h>
#define __WIN32_VXD__
#ifndef __32BIT__
#define __32BIT__
#endif
#undef __WINDOWS32__

/* 32-bit Windows NT driver compile environment */
#elif   defined(__NT_DRIVER__)
#include "ntdriver.h"
#ifndef __32BIT__
#define __32BIT__
#endif
#undef __WINDOWS32__

/* 32-bit Windows CE driver compile environment */
#elif   defined(__WINCE__)
#ifndef __32BIT__
#define __32BIT__
#endif

/* 32-bit SMX compile environment */
#elif   defined(__SMX32__)
#ifndef __MSDOS__
#define __MSDOS__
#endif
#ifndef __32BIT__
#define __32BIT__
#endif
#ifndef __CONSOLE__
#define __CONSOLE__
#endif

/* 32-bit Enea OSE environment */
#elif   defined(__ENEA_OSE__)
#ifndef __32BIT__
#define __32BIT__
#endif
#ifndef __CONSOLE__
#define __CONSOLE__
#endif

/* 32-bit RTTarget-32 environment */
#elif   defined(__RTTARGET__)
#ifndef __32BIT__
#define __32BIT__
#endif
#ifndef __CONSOLE__
#define __CONSOLE__
#endif

/* 32-bit extended DOS compile environment */
#elif   defined(__MSDOS__) || defined(__MSDOS32__) || defined(__DOS__) || defined(__DPMI32__) || (defined(M_I86) && (!defined(__SC386__) && !defined(M_I386))) || defined(TNT)
#ifndef __MSDOS__
#define __MSDOS__
#endif
#if     defined(__MSDOS32__) || defined(__386__) || defined(__FLAT__) || defined(__NT__) || defined(__SC386__)
#ifndef __MSDOS32__
#define __MSDOS32__
#endif
#ifndef __32BIT__
#define __32BIT__
#endif
#ifndef __REALDOS__
#define __REALDOS__
#endif
#ifndef __CONSOLE__
#define __CONSOLE__
#endif

/* 16-bit Windows compile environment */
#elif   (defined(_Windows) || defined(_WINDOWS)) && !defined(__DPMI16__)
#ifndef __16BIT__
#define __16BIT__
#endif
#ifndef __WINDOWS16__
#define __WINDOWS16__
#endif
#ifndef __WINDOWS__
#define __WINDOWS__
#endif
#ifndef __MSDOS__
#define __MSDOS__
#endif

/* 16-bit DOS compile environment */
#else
#ifndef __16BIT__
#define __16BIT__
#endif
#ifndef __MSDOS16__
#define __MSDOS16__
#endif
#ifndef __REALDOS__
#define __REALDOS__
#endif
#ifndef __CONSOLE__
#define __CONSOLE__
#endif
#endif

/* 32-bit Windows compile environment */
#elif   defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#ifndef __32BIT__
#define __32BIT__
#endif
#ifndef __WINDOWS32__
#define __WINDOWS32__
#endif
#ifndef _WIN32
#define _WIN32                  /* Microsoft Win32 SDK headers use _WIN32 */
#endif
#ifndef WIN32
#define WIN32                   /* OpenGL headers use WIN32 */
#endif
#ifndef __WINDOWS__
#define __WINDOWS__
#endif

/* 32-bit OS/2 VDD compile environment */
/* We're assuming (for now) that CL386 must be used */
#elif   defined(MSDOS) && defined(M_I386)
/* fixes necessary to compile with CL386 */
#define __cdecl  _cdecl
typedef unsigned int size_t;

#include <mvdm.h>

/* This should probably be somewhere else...                 */
/* Inline eligible functions (we have no CRT libs for CL386) */
#pragma intrinsic (strcpy, strcmp, strlen, strcat)
#pragma intrinsic (memcmp, memcpy, memset)

#define __OS2_VDD__
#ifndef __32BIT__
#define __32BIT__
#endif
#define CCHMAXPATH  256
#ifndef __OS2__
#define __OS2__
#endif
#ifndef __OS2_32__
#define __OS2_32__
#endif

/* 16-bit OS/2 compile environment */
#elif   defined(__OS2_16__)
#ifndef __OS2__
#define __OS2__
#endif
#ifndef __16BIT__
#define __16BIT__
#endif
#ifndef __OS2_PM__
#ifndef __CONSOLE__
#define __CONSOLE__
#endif
#endif

/* 32-bit OS/2 compile environment */
#elif   defined(__OS2__) || defined(__OS2_32__)
#ifndef __OS2__
#define __OS2__
#endif
#ifndef __OS2_32__
#define __OS2_32__
#endif
#ifndef __32BIT__
#define __32BIT__
#endif
#ifndef __OS2_PM__
#ifndef __CONSOLE__
#define __CONSOLE__
#endif
#endif

/* 32-bit QNX compile environment */
#elif   defined(__QNX__)
#ifndef __32BIT__
#define __32BIT__
#endif
#ifndef __UNIX__
#define __UNIX__
#endif
#ifdef  __GNUC__
#define stricmp strcasecmp
#endif
#if !defined(__PHOTON__) && !defined(__X11__)
#ifndef __CONSOLE__
#define __CONSOLE__
#endif
#endif

/* XFree86 driver module */

#elif   defined(XFree86Module)
#ifndef __XFREE86__
#define __XFREE86__
#endif
#ifndef __32BIT__
#define __32BIT__
#endif

/* Unununium Compile Environment */
#elif   defined(__UUU__)
#ifndef __32BIT__
#define __32BIT__
#endif
#ifndef __UNIX__
#define __UNIX__
#endif
#ifdef  __GNUC__
#ifndef stricmp
#define stricmp strcasecmp
#endif
#ifndef strnicmp
#define strnicmp strncasecmp
#endif
#endif

/* 32-bit Linux compile environment */
#elif   defined(__LINUX__) || defined(linux)
#ifndef __LINUX__
#define __LINUX__
#endif
#ifndef __32BIT__
#define __32BIT__
#endif
#ifndef __UNIX__
#define __UNIX__
#endif
#ifdef  __GNUC__
#ifndef stricmp
#define stricmp strcasecmp
#endif
#ifndef strnicmp
#define strnicmp strncasecmp
#endif
#endif
#ifndef __X11__
#ifndef __CONSOLE__
#define __CONSOLE__
#endif
#endif

/* 32-bit FreeBSD compile environment */
#elif   defined(__FREEBSD__)
#ifndef __FREEBSD__
#define __FREEBSD__
#endif
#ifndef __32BIT__
#define __32BIT__
#endif
#ifndef __UNIX__
#define __UNIX__
#endif
#ifdef  __GNUC__
#define stricmp strcasecmp
#endif
#ifndef __X11__
#ifndef __CONSOLE__
#define __CONSOLE__
#endif
#endif

/* 32-bit BeOS compile environment */
#elif   defined(__BEOS__)
#ifndef __32BIT__
#define __32BIT__
#endif
#ifndef __UNIX__
#define __UNIX__
#endif
#ifdef  __GNUC__
#define stricmp strcasecmp
#endif

/* Unsupported OS! */
#else
#error  This platform is not currently supported!
#endif

/* We have the following defines to define the calling conventions for
 * publicly accesible functions:
 *
 *  _PUBAPI  - Compiler default calling conventions for all public 'C' functions
 *  _ASMAPI  - Calling conventions for all public assembler functions
 *  _VARAPI  - Modifiers for variables; Watcom C++ mangles C++ globals
 *  _STDCALL - Win32 __stdcall where possible, __cdecl if not supported
 */

#if defined(_MSC_VER) && defined(_WIN32) && !defined(__SC__)
#define __PASCAL    __stdcall
#else
#define __PASCAL    __pascal
#endif

#if defined(NO_STDCALL)
#define _STDCALL    __cdecl
#else
#define _STDCALL    __stdcall
#endif

#ifdef  __WATCOMC__
#if (__WATCOMC__ >= 1050)
#define _VARAPI     __cdecl
#else
#define _VARAPI
#endif
#else
#define _VARAPI
#endif

#if defined(__IBMC__) || defined(__IBMCPP__)
#define PTR_DECL_IN_FRONT
#endif

/* Define the calling conventions for all public functions. For simplicity
 * we define all public functions as __cdecl calling conventions, so that
 * they are the same across all compilers and runtime DLL's.
 */

#define _PUBAPI __cdecl
#define _ASMAPI __cdecl

/* Determine the syntax for declaring a function pointer with a
 * calling conventions override. Most compilers require the calling
 * convention to be declared in front of the '*', but others require
 * it to be declared after the '*'. We handle both in here depending
 * on what the compiler requires.
 */

#ifdef  PTR_DECL_IN_FRONT
#define _PUBAPIP    * _PUBAPI
#define _ASMAPIP    * _ASMAPI
#else
#define _PUBAPIP    _PUBAPI *
#define _ASMAPIP    _ASMAPI *
#endif

/* Useful macros */

#define PRIVATE static
#define PUBLIC

/* This HAS to be 0L for 16-bit real mode code to work!!! */

#ifndef NULL
#       define _NULL 0L
#       define NULL _NULL
#endif

#ifndef MAX
#       define MAX(a,b) ( ((a) > (b)) ? (a) : (b))
#endif
#ifndef MIN
#       define MIN(a,b) ( ((a) < (b)) ? (a) : (b))
#endif
#ifndef ABS
#       define ABS(a)   ((a) >= 0 ? (a) : -(a))
#endif
#ifndef SIGN
#       define SIGN(a)  ((a) > 0 ? 1 : -1)
#endif

/* General typedefs */

#ifndef __GENDEFS
#define __GENDEFS
#if defined(__BEOS__)
#include <SupportDefs.h>
#else
#ifdef __LINUX__
#ifndef __KERNEL__
#include <sys/types.h>
#else
#include <linux/types.h>
#define __USE_MISC
#endif
/* the linux sys/types.h file will typedef ushort etc unless __USE_MISC is not defined. */
#ifndef __USE_MISC
typedef unsigned short      ushort;
typedef unsigned long       ulong;
typedef unsigned int        uint;
#endif
#ifdef  __KERNEL__
#define __GENDEFS_2
#define __GENDEFS_3
#endif
#else
#if !(defined(__QNXNTO__) && defined(GENERAL_STRUCT))
typedef unsigned short      ushort;
typedef unsigned long       ulong;
#endif
typedef unsigned int        uint;
#endif
typedef unsigned char       uchar;
#endif
typedef int                 ibool;      /* Integer boolean type         */
#ifdef  USE_BOOL                        /* Only for older code          */
#ifndef __cplusplus
#define bool                ibool       /* Standard C                   */
#else
#ifndef __HAS_BOOL__
#define bool                ibool       /* Older C++ compilers          */
#endif
#endif  /* __cplusplus */
#endif  /* USE_BOOL */
#endif  /* __GENDEFS */

/* More general typedefs compatible with Linux kernel code */

#ifndef __GENDEFS_2
#define __GENDEFS_2
typedef char                s8;
typedef unsigned char       u8;
typedef short               s16;
typedef unsigned short      u16;
#ifdef  __16BIT__
typedef long                s32;
typedef unsigned long       u32;
#else
typedef int                 s32;
typedef unsigned int        u32;
#endif
#endif  /* __GENDEFS_2 */

typedef struct {
    u32 low;
    s32 high;
    } __i64;

#ifndef __GENDEFS_3
#define __GENDEFS_3
#ifdef  __HAS_LONG_LONG__
#define __NATIVE_INT64__
typedef long long           s64;
typedef unsigned long long  u64;
#elif   defined(__HAS_INT64__) && !defined(__16BIT__)
#define __NATIVE_INT64__
typedef __int64             s64;
typedef unsigned __int64    u64;
#else
typedef __i64               s64;
typedef __i64               u64;
#endif
#endif  /* __GENDEFS_3 */

/* Integer type to hold a pointer value */
typedef unsigned long       __ui_ptr;

/* Boolean truth values */

#undef  NO
#undef  YES
#undef  FALSE
#undef  TRUE
#define NO          0
#define YES         1
#define FALSE       0
#define TRUE        1

/* Newer C++ compiler have built in types for true and false that we do not want
 * to change or it will lose track of value being of the type 'bool'.
 */

#if !defined(__cplusplus) || !defined(__HAS_BOOL__)
#undef  false
#undef  true
#define false       0
#define true        1
#endif

/* Inline debugger interrupts for Watcom C++ and Borland C++ */

#ifdef  __WATCOMC__
void DebugInt(void);
#pragma aux DebugInt =              \
    "int    3";
void DebugVxD(void);
#pragma aux DebugVxD =              \
    "int    1";
#elif   defined(__BORLANDC__)
#define DebugInt()  __emit__(0xCC)
#define DebugVxD()  {__emit__(0xCD); __emit__(0x01);}
#elif   defined(_MSC_VER)
#define DebugInt()  _asm {int 0x3}
#define DebugVxD()  _asm {int 0x1}
#elif   defined(__GNUC__)
#ifdef __INTEL__
#define DebugInt()  __asm__ volatile ("int $0x3")
#define DebugVxD()  __asm__ volatile ("int $0x1")
#elif defined __PPC__
#define DebugInt() __asm__ volatile ("trap")
#define DebugVxD()
#else
#define DebugInt()
#define DebugVxD()
#endif
#else
void _ASMAPI DebugInt(void);
void _ASMAPI DebugVxD(void);
#endif

/* Macros to break once and never break again */

#define DebugIntOnce()              \
{                                   \
    static ibool firstTime = true;  \
    if (firstTime) {                \
        firstTime = false;          \
        DebugInt();                 \
        }                           \
}

#define DebugVxDOnce()              \
{                                   \
    static ibool firstTime = true;  \
    if (firstTime) {                \
        firstTime = false;          \
        DebugVxD();                 \
        }                           \
}

/* Macros for NT driver string compatibility functions */

#if defined(__NT_DRIVER__) || defined(__WINCE__)
#define stricmp _stricmp
#define strnicmp _strnicmp
#endif

/* Get rid of some helaciously annoying Visual C++ warnings! */

#if defined(_MSC_VER) && !defined(__MWERKS__) && !defined(__SC__)
#pragma warning(disable:4761)   /* integral size mismatch in argument; conversion supplied */
#pragma warning(disable:4244)   /* conversion from 'unsigned short ' to 'unsigned char ', possible loss of data */
#pragma warning(disable:4018)   /* '<' : signed/unsigned mismatch */
#pragma warning(disable:4305)   /* 'initializing' : truncation from 'const double' to 'float' */
#endif

/*---------------------------------------------------------------------------
 * Set of debugging macros used by the libraries. If the debug flag is
 * set, they are turned on depending on the setting of the flag. User code
 * can override the default functions called when a check fails, and the
 * MGL does this so it can restore the system from graphics mode to display
 * an error message. These functions also log information to the
 * scitech.log file in the root directory of the hard drive when problems
 * show up.
 *
 * If you set the value of CHECKED to be 2, it will also enable code to
 * insert hard coded debugger interrupt into the source code at the line of
 * code where the check fail. This is useful if you run the code under a
 * debugger as it will break inside the debugger before exiting with a
 * failure condition.
 *
 * Also for code compiled to run under Windows, we also call the
 * OutputDebugString function to send the message to the system debugger
 * such as Soft-ICE or WDEB386. Hence if you get any non-fatal warnings you
 * will see those on the debugger terminal as well as in the log file.
 *-------------------------------------------------------------------------*/

#ifdef  __cplusplus
extern "C" {            /* Use "C" linkage when in C++ mode */
#endif

extern void _ASMAPI _CHK_defaultFail(int fatal,const char *msg,const char *cond,const char *file,int line);
#ifdef __NT_DRIVER__
#define _CHK_debugLog   DbgPrint
#else
extern void _ASMAPI _CHK_debugLog(const char *msg,...);
#endif
#if defined(__NT_DRIVER__) || defined(__WIN32_VXD__) || defined(__DRIVER__)
#define _CHK_fail       _CHK_defaultFail
#else
extern void (_ASMAPIP _CHK_fail)(int fatal,const char *msg,const char *cond,const char *file,int line);
#endif

#ifdef  CHECKED
#       define  _CHK(x) x
#if     CHECKED > 1
#       define  _CHECK(p)                                            \
        ((p) ? (void)0 : DebugInt(),                                \
            _CHK_fail(1,"Check failed: '%s', file %s, line %d\n",   \
            #p, __FILE__, __LINE__))
#       define  _WARN(p)                                             \
        ((p) ? (void)0 : DebugInt(),                                \
            _CHK_fail(0,"Warning: '%s', file %s, line %d\n",        \
            #p, __FILE__, __LINE__))
#else
#       define  _CHECK(p)                                            \
        ((p) ? (void)0 :                                            \
            _CHK_fail(1,"Check failed: '%s', file %s, line %d\n",   \
            #p, __FILE__, __LINE__))
#       define  _WARN(p)                                             \
        ((p) ? (void)0 :                                            \
            _CHK_fail(0,"Warning: '%s', file %s, line %d\n",        \
            #p, __FILE__, __LINE__))
#endif
#       define  _LOGFATAL(msg)                                       \
            _CHK_fail(1,"Fatal error: '%s', file %s, line %d\n",    \
            msg, __FILE__, __LINE__)
#       define  _LOGWARN(msg)                                        \
            _CHK_fail(0,"Warning: '%s', file %s, line %d\n",        \
            msg, __FILE__, __LINE__)
#       define  _LOGMSG(paramsInParentheses)                         \
            _CHK_debugLog paramsInParentheses
#else
#       define  _CHK(x)
#       define  _CHECK(p)       ((void)0)
#       define  _WARN(p)        ((void)0)
#       define  _LOGFATAL(msg)  ((void)0)
#       define  _LOGWARN(msg)   ((void)0)
#       define  _LOGMSG(p)      ((void)0)
#endif
#       define  CHK(x)          _CHK(x)
#       define  CHECK(p)        _CHECK(p)
#       define  WARN(p)         _WARN(p)
#       define  LOGFATAL(msg)   _LOGFATAL(msg)
#       define  LOGWARN(msg)    _LOGWARN(msg)
#       define  LOGMSG(p)       _LOGMSG(p)

#ifdef  __cplusplus
}                       /* End of "C" linkage for C++   */
#endif

#endif  /* __SCITECH_H */

