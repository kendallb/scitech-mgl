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
* Environment:  Any
*
* Description:  Header file for the OS Portability Manager Library, which
*               contains functions to implement OS specific services in a
*               generic, cross platform API. Porting the OS Portability
*               Manager library is the first step to porting any SciTech
*               products to a new platform.
*
****************************************************************************/

#ifndef __PMAPI_H
#define __PMAPI_H

#include "scitech.h"
#include "pcilib.h"
#include "ztimerc.h"
#if defined(__XFREE86__)
#include <xf86_libc.h>
#elif !defined(__WIN32_VXD__) && !defined(__OS2_VDD__) && !defined(__NT_DRIVER__) && !defined(__KERNEL__)
#include <stdio.h>
#include <stdlib.h>
#endif

/*--------------------------- Macros and Typedefs -------------------------*/

/* You will need to define one of the following before you compile this
 * library for it to work correctly with the DOS extender that you are
 * using when compiling for extended DOS:
 *
 *      TNT         - Phar Lap TNT DOS Extender
 *      DOS4GW      - Rational DOS/4GW, DOS/4GW Pro, Causeway and PMODE/W
 *      DJGPP       - DJGPP port of GNU C++
 *
 * If none is specified, we will automatically determine which operating
 * system is being targetted and the following will be defined (provided by
 * scitech.h header file):
 *
 *      __MSDOS16__     - Default for 16 bit MSDOS mode
 *      __MSDOS32__     - Default for 32 bit MSDOS
 *      __WINDOWS16__   - Default for 16 bit Windows
 *      __WINDOWS32__   - Default for 32 bit Windows
 *
 * One of the following will be defined automatically for you to select
 * which memory model is in effect:
 *
 *      REALMODE    - 16 bit real mode (large memory model)
 *      PM286       - 16 protected mode (large memory model)
 *      PM386       - 32 protected mode (flat memory model)
 */

#if defined(TNT) || defined(DOSX) || defined(X32VM) || defined(DPMI32)      \
    || defined(DOS4GW) || defined(DJGPP) || defined(__WINDOWS32__)          \
    || defined(__MSDOS32__) || defined(__UNIX__) || defined(__WIN32_VXD__) \
    || defined(__32BIT__) || defined(__SMX32__) || defined(__RTTARGET__)
#define PM386
#elif defined(DPMI16) || defined(__WINDOWS16__)
#define PM286
#else
#define REALMODE
#endif

/* 'C' calling conventions always       */

#define PMAPI   _ASMAPI
#define PMAPIP  _ASMAPIP

/* Define a far pointer macro */

#ifdef __16BIT__
#define PM_FAR  far
#else
#define PM_FAR
#endif

/* Generic definitions we use internally */

#define PM_MAX_DRIVE                3
#define PM_MAX_PATH                 256
#define PM_FILE_INVALID             (void*)0xFFFFFFFF

/* Defines the size of an system memory page */

#define PM_PAGE_SIZE                4096

/* Define a bad physical address returned by map physical functions */

#define PM_BAD_PHYS_ADDRESS         0xFFFFFFFF

/* Macro to compute the byte offset of a field in a structure of type type */

#define PM_FIELD_OFFSET(type,field) ((long)&(((type*)0)->field))

/* Marcto to compute the address of the base of the structure given its type,
 * and an address of a field within the structure.
 */

#define PM_CONTAINING_RECORD(address, type, field)      \
    ((type*)(                                           \
    (char*)(address) -                                  \
    (char*)(&((type*)0)->field)))

/* Define a macro for creating physical base addresses from segment:offset */

#define MK_PHYS(s,o)  (((ulong)(s) << 4) + (ulong)(o))

/* Macros for extracting values from binary file structures in little
 * endian or big endian format. We define the following macros:
 *
 *  PM_getLEShort()     - Get a short in little endian format
 *  PM_getLELong()      - Get a long in little endian format
 *  PM_getBEShort()     - Get a short in big endian format
 *  PM_getBESHory()     - Get a long in big endian format
 *
 *  PM_putLEShort()     - Put a short in little endian format
 *  PM_putLELong()      - Put a long in little endian format
 *  PM_putBEShort()     - Put a short in big endian format
 *  PM_putBESHory()     - Put a long in big endian format
 *
 *  PM_convLEShort()    - Convert little endian short in place
 *  PM_convLELong()     - Convert little endian long in place
 *  PM_convBEShort()    - Convert big endian short in place
 *  PM_convBELong()     - Convert big endian long in place
 */

#define _PM_TOB(v)          ((u8*)&(v))
#define _PM_TOW(v)          ((u16*)&(v))
#define _PM_TOD(v)          ((u32*)&(v))
#define _PM_TOI(v,i,s)      (((u32)(_PM_TOB(v)[i])) << (s))

#ifdef  __BIG_ENDIAN__
#define PM_constBE_16(v)    (v)
#define PM_constBE_32(v)    (v)
#define PM_constLE_16(v)    (((v) >> 8) | (((v) & 0xff) << 8))
#define PM_constLE_32(v)    ((((((v)&0xff00)>>8)|(((v)&0xff)<<8))<<16)|(((((v)&0xff000000)>>8)|(((v)&0x00ff0000)<<8))>>16))
#define PM_getBE_16(v)      (*_PM_TOW(v))
#define PM_getBE_32(v)      (*_PM_TOD(v))
#define PM_putBE_16(p,v)    (*_PM_TOW(p)) = (v)
#define PM_putBE_32(p,v)    (*_PM_TOD(p)) = (v)
// CJC TODO - inline these
u32     _ASMAPI _swapw(u32 value);
u16     _ASMAPI _swaphw(u16 value);
#define PM_paramLE_16(v)    _swaphw(v)
#define PM_paramLE_32(v)    _swapw(v)
#define PM_getLE_16(v)      ((u16)(((v) & 0xFF) << 8) | (((((v) & 0xFF00) >> 8))))
#define PM_getLE_32(v)      ((u32)(_PM_TOI(v,3,24) | _PM_TOI(v,2,16) | _PM_TOI(v,1,8) | _PM_TOI(v,0,0)))
#define PM_putLE_16(p,v)    (_PM_TOB(p)[0] = (v) & 0xFF),(_PM_TOB(p)[1] = ((v) >> 8) & 0xFF)
#define PM_putLE_32(p,v)    (_PM_TOB(p)[0] = (v) & 0xFF),(_PM_TOB(p)[1] = ((v) >> 8) & 0xFF),(_PM_TOB(p)[2] = ((v) >> 16) & 0xFF),(_PM_TOB(p)[3] = ((v) >> 24) & 0xFF)
#define PM_convLE_16(v)     (v = PM_getLE_16(v))
#define PM_convLE_32(v)     (v = PM_getLE_32(v))
#define PM_convBE_16(v)     (v) = (v)
#define PM_convBE_32(v)     (v) = (v)
#else
#define PM_constLE_16(v)    (v)
#define PM_constLE_32(v)    (v)
#define PM_constBE_16(v)    (((v) >> 8) | (((v) & 0xff) << 8))
#define PM_constBE_32(v)    ((((((v)&0xff00)>>8)|(((v)&0xff)<<8))<<16)|(((((v)&0xff000000)>>8)|(((v)&0x00ff0000)<<8))>>16))
#define PM_getBE_16(v)      ((u16)(((v) & 0xFF) << 8) | (((((v) & 0xFF00) >> 8))))
#define PM_getBE_32(v)      ((u32)(_PM_TOI(v,3,0) | _PM_TOI(v,2,8) | _PM_TOI(v,1,16) | _PM_TOI(v,0,24)))
#define PM_putBE_16(p,v)    (_PM_TOB(p)[1] = (v) & 0xFF),(_PM_TOB(p)[0] = ((v) >> 8) & 0xFF)
#define PM_putBE_32(p,v)    (_PM_TOB(p)[3] = (v) & 0xFF),(_PM_TOB(p)[2] = ((v) >> 8) & 0xFF),(_PM_TOB(p)[1] = ((v) >> 16) & 0xFF),(_PM_TOB(p)[0] = ((v) >> 24) & 0xFF)
#define PM_getLE_16(v)      (*_PM_TOW(v))
#define PM_getLE_32(v)      (*_PM_TOD(v))
#define PM_putLE_16(p,v)    (*_PM_TOW(p)) = (v)
#define PM_putLE_32(p,v)    (*_PM_TOD(p)) = (v)
#define PM_convLE_16(v)     (v) = (v)
#define PM_convLE_32(v)     (v) = (v)
#define PM_convBE_16(v)     (v = PM_getBE_16(v))
#define PM_convBE_32(v)     (v = PM_getBE_32(v))
#endif

/* Macro to define CPU specific sub directory paths based on the compiler
 * target platorm.
 */

#if defined(__INTEL__)
#define PM_CPU_SUBPATH  "x86"
#elif defined(__ALPHA__)
#if defined(__BIG_ENDIAN__)
#define PM_CPU_SUBPATH  "axp-be"
#else
#define PM_CPU_SUBPATH  "axp-le"
#endif
#elif defined(__MIPS__)
#if defined(__BIG_ENDIAN__)
#define PM_CPU_SUBPATH  "mips-be"
#else
#define PM_CPU_SUBPATH  "mips-le"
#endif
#elif defined(__PPC__)
#if defined(__BIG_ENDIAN__)
#define PM_CPU_SUBPATH  "ppc-be"
#else
#define PM_CPU_SUBPATH  "ppc-le"
#endif
#elif defined(__X86_64__)
#define PM_CPU_SUBPATH  "x86-64"
#else
#error PM library not ported to this platform yet!
#endif

/* Macro to safely copy a string using strncpy */

#define PM_strcpy(dst,src,size) (dst)[(size)-1] = 0, strncpy(dst,src,(size)-1)

#pragma pack(1)

/* Define the different types of modes supported. This is a global variable
 * that can be used to determine the type at runtime which will contain
 * one of these values. OBSOLETE NOW.
 * {secret}
 */
typedef enum {
    PM_realMode,
    PM_286,
    PM_386
    } PM_mode_enum;

/****************************************************************************
REMARKS:
This enumeration defines the type values passed to the PM_agpReservePhysical
function, to define how the physical memory mapping should be handled.

The PM_agpUncached type indicates that the memory should be allocated as
uncached memory.

The PM_agpWriteCombine type indicates that write combining should be enabled
for physical memory mapping. This is used for framebuffer write combing and
speeds up direct framebuffer writes to the memory.

The PM_agpIntelDCACHE type indicates that memory should come from the Intel
i81x Display Cache (or DCACHE) memory pool. This flag is specific to the
Intel i810 and i815 controllers, and should not be passed for any other
controller type.

HEADER:
pmapi.h

MEMBERS:
PM_agpUncached      - Indicates that the memory should be uncached
PM_agpWriteCombine  - Indicates that the memory should be write combined
PM_agpIntelDCACHE   - Indicates that the memory should come from DCACHE pool
****************************************************************************/
typedef enum {
    PM_agpUncached,
    PM_agpWriteCombine,
    PM_agpIntelDCACHE
    } PM_agpMemoryType;

/****************************************************************************
REMARKS:
Flags stored in the PM_findData structure, and also values passed to
PM_setFileAttr to change the file attributes. Flag names are self
explanatory.

HEADER:
pmapi.h
****************************************************************************/
typedef enum {
    PM_FILE_NORMAL          = 0x00000000,
    PM_FILE_READONLY        = 0x00000001,
    PM_FILE_DIRECTORY       = 0x00000002,
    PM_FILE_ARCHIVE         = 0x00000004,
    PM_FILE_HIDDEN          = 0x00000008,
    PM_FILE_SYSTEM          = 0x00000010
    } PMFileFlagsType;

/****************************************************************************
REMARKS:
Flags returned by the PM_splitpath function. Flag names are self explanatory.

HEADER:
pmapi.h
****************************************************************************/
typedef enum {
    PM_HAS_WILDCARDS        = 0x01,
    PM_HAS_EXTENSION        = 0x02,
    PM_HAS_FILENAME         = 0x04,
    PM_HAS_DIRECTORY        = 0x08,
    PM_HAS_DRIVE            = 0x10
    } PMSplitPathFlags;

/****************************************************************************
REMARKS:
Flags passed to PM_enableWriteCombine

HEADER:
pmapi.h

MEMBERS:
PM_MTRR_UNCACHABLE  - Make region uncacheable
PM_MTRR_WRCOMB      - Make region write combineable
PM_MTRR_WRTHROUGH   - Make region write through cached
PM_MTRR_WRPROT      - Make region write protected
PM_MTRR_WRBACK      - Make region write back cached
PM_MTRR_MAX         - Maximum value allowed
****************************************************************************/
typedef enum {
    PM_MTRR_UNCACHABLE      = 0,
    PM_MTRR_WRCOMB          = 1,
    PM_MTRR_WRTHROUGH       = 4,
    PM_MTRR_WRPROT          = 5,
    PM_MTRR_WRBACK          = 6,
    PM_MTRR_MAX             = 6
    } PMEnableWriteCombineFlags;

/****************************************************************************
REMARKS:
Error codes returned by PM_enableWriteCombine. Error code names are self
explanatory.

HEADER:
pmapi.h
****************************************************************************/
typedef enum {
    PM_MTRR_ERR_OK                  = 0,
    PM_MTRR_NOT_SUPPORTED           = -1,
    PM_MTRR_ERR_PARAMS              = -2,
    PM_MTRR_ERR_NOT_4KB_ALIGNED     = -3,
    PM_MTRR_ERR_BELOW_1MB           = -4,
    PM_MTRR_ERR_NOT_ALIGNED         = -5,
    PM_MTRR_ERR_OVERLAP             = -6,
    PM_MTRR_ERR_TYPE_MISMATCH       = -7,
    PM_MTRR_ERR_NONE_FREE           = -8,
    PM_MTRR_ERR_NOWRCOMB            = -9,
    PM_MTRR_ERR_NO_OS_SUPPORT       = -10
    } PMEnableWriteCombineErrors;

/****************************************************************************
REMARKS:
Values passed to the PM_DMACProgram function. Flag names are self
explanatory.

HEADER:
pmapi.h
{secret} - Only for DOS presently
****************************************************************************/
typedef enum {
    PM_DMA_READ_ONESHOT         = 0x44,
    PM_DMA_WRITE_ONESHOT        = 0x48,
    PM_DMA_READ_AUTOINIT        = 0x54,
    PM_DMA_WRITE_AUTOINIT       = 0x58
    } PMDMACProgramFlags;

/****************************************************************************
REMARKS:
Defines the suspend application callback flags, passed to the suspend
application callback registered with the PM library. This callback is called
when the user presses one of the system key sequences indicating that
they wish to change the active application. The PM library will catch these
events and if you have registered a callback, will call the callback to
save the state of the application so that it can be properly restored
when the user switches back to your application.

Note: Your application suspend callback may get called twice with the
      PM_DEACTIVATE flag in order to test whether the switch should
      occur.

Note: When your callback is called with the PM_DEACTIVATE flag, you
      cannot assume that you have access to the display memory surfaces
      as they may have been lost by the time your callback has been called.

HEADER:
pmapi.h

MEMBERS:
PM_DEACTIVATE  - Application losing active focus
PM_REACTIVATE  - Application regaining active focus
****************************************************************************/
typedef enum {
    PM_DEACTIVATE   = 0x0001,
    PM_REACTIVATE   = 0x0002
    } PM_suspendAppFlagsType;

/****************************************************************************
REMARKS:
Defines the return codes that the application can return from the suspend
application callback registered with the PM library. The default value to be
returned is PM_SUSPEND_APP and this will cause the application execution to
be suspended until the application is re-activated again by the user. During
this time the application will exist on the task bar under Windows 9x and
Windows NT/2000/XP in minimised form.

PM_NO_SUSPEND_APP can be used to tell the PM library to switch back to the
Windows desktop, but not to suspend the applications execution. This must
be used with care as the suspend application callback is then responsible
for setting a flag in the application that will /stop/ the application from
doing any rendering directly to the framebuffer while the application is
minimised on the task bar (since the application no lonter owns the screen!).
This return value is most useful for networked games that need to maintain
network connectivity while the user has temporarily switched back to the
Windows desktop. Hence you can ensure that you main loop continues to run,
including networking and AI code, but no drawing occurs to the screen.

Note: The PM library ensures that your application will /never/ be switched
      away from outside of a message processing loop. Hence as long as you do
      not process messages inside your drawing loops, you will never lose
      the active focus (and your surfaces) while drawing, but only during
      event processing. The exception to this is if the user hits
      Ctrl-Alt-Del under Windows NT/2000/XP which will always cause a switch
      away from the application immediately and force the surfaces to be
      lost.

HEADER:
pmapi.h

MEMBERS:
PM_SUSPEND_APP     - Suspend application execution until restored
PM_NO_SUSPEND_APP  - Don't suspend execution, but allow switch
****************************************************************************/
typedef enum {
    PM_SUSPEND_APP      = 0,
    PM_NO_SUSPEND_APP   = 1
    } PM_suspendAppCodesType;

/****************************************************************************
REMARKS:
Flags passed to PM_memProtect used to control access protection of
memory. Flag names are self explanatory.

HEADER:
pmapi.h
****************************************************************************/
typedef enum {
    PM_MPROT_NONE           = 0x00000000,
    PM_MPROT_READ           = 0x00000001,
    PM_MPROT_WRITE          = 0x00000002,
    PM_MPROT_EXEC           = 0x00000004
    } PMMemProtFlagsType;

/****************************************************************************
REMARKS:
Structure describing the 32-bit extended x86 CPU registers

HEADER:
pmapi.h

MEMBERS:
eax     - Value of the EAX register
ebx     - Value of the EBX register
ecx     - Value of the ECX register
edx     - Value of the EDX register
esi     - Value of the ESI register
edi     - Value of the EDI register
cflag   - Value of the carry flag
****************************************************************************/
typedef struct {
    u32     eax;
    u32     ebx;
    u32     ecx;
    u32     edx;
    u32     esi;
    u32     edi;
    u32     cflag;
    } PMDWORDREGS;

/****************************************************************************
REMARKS:
Structure describing the 16-bit x86 CPU registers

HEADER:
pmapi.h

MEMBERS:
ax      - Value of the AX register
bx      - Value of the BX register
cx      - Value of the CX register
dx      - Value of the DX register
si      - Value of the SI register
di      - Value of the DI register
cflag   - Value of the carry flag
****************************************************************************/
#ifdef __BIG_ENDIAN__
typedef struct {
    u16     ax_hi,ax;
    u16     bx_hi,bx;
    u16     cx_hi,cx;
    u16     dx_hi,dx;
    u16     si_hi,si;
    u16     di_hi,di;
    u16     cflag_hi,cflag;
    } PMWORDREGS;
#else
typedef struct {
    u16     ax,ax_hi;
    u16     bx,bx_hi;
    u16     cx,cx_hi;
    u16     dx,dx_hi;
    u16     si,si_hi;
    u16     di,di_hi;
    u16     cflag,cflag_hi;
    } PMWORDREGS;
#endif

/****************************************************************************
REMARKS:
Structure describing the 8-bit x86 CPU registers

HEADER:
pmapi.h

MEMBERS:
al      - Value of the AL register
ah      - Value of the AH register
bl      - Value of the BL register
bh      - Value of the BH register
cl      - Value of the CL register
ch      - Value of the CH register
dl      - Value of the DL register
dh      - Value of the DH register
****************************************************************************/
#ifdef __BIG_ENDIAN__
typedef struct {
    u16     ax_hi;
    u8      ah,al;
    u16     bx_hi;
    u8      bh,bl;
    u16     cx_hi;
    u8      ch,cl;
    u16     dx_hi;
    u8      dh,dl;
    } PMBYTEREGS;
#else
typedef struct {
    u8      al;
    u8      ah; ushort ax_hi;
    u8      bl;
    u8      bh; ushort bx_hi;
    u8      cl;
    u8      ch; ushort cx_hi;
    u8      dl;
    u8      dh; ushort dx_hi;
    } PMBYTEREGS;
#endif

/****************************************************************************
REMARKS:
Structure describing all the x86 CPU registers

HEADER:
pmapi.h

MEMBERS:
e   - Member to access registers as 32-bit values
x   - Member to access registers as 16-bit values
h   - Member to access registers as 8-bit values
****************************************************************************/
typedef union {
    PMDWORDREGS e;
    PMWORDREGS  x;
    PMBYTEREGS  h;
    } PMREGS;

/****************************************************************************
REMARKS:
Structure describing all the x86 segment registers

HEADER:
pmapi.h

MEMBERS:
es  - ES segment register
cs  - CS segment register
ss  - SS segment register
ds  - DS segment register
fs  - FS segment register
gs  - GS segment register
****************************************************************************/
typedef struct {
    u16     es;
    u16     cs;
    u16     ss;
    u16     ds;
    u16     fs;
    u16     gs;
    } PMSREGS;

/****************************************************************************
REMARKS:
Same as PMREGS. Please see PMREGS for more information.

HEADER:
pmapi.h
****************************************************************************/
typedef PMREGS  RMREGS;

/****************************************************************************
REMARKS:
Same as PMSREGS. Please see PMSREGS for more information.

HEADER:
pmapi.h
****************************************************************************/
typedef PMSREGS RMSREGS;

#ifdef  __MSDOS__
/* Register structure passed to PM_VxDCall function
 * {secret}
 */
typedef struct {
    ulong   eax;
    ulong   ebx;
    ulong   ecx;
    ulong   edx;
    ulong   esi;
    ulong   edi;
    ushort  ds,es;
    } VXD_regs;
#endif

/****************************************************************************
REMARKS:
Structure for generic directory traversal and management. Also the same
values are passed to PM_setFileAttr to change the file attributes.

Note:   The dwSize member is intended for future compatibility, and
        should be set to the size of the structure as defined in this
        header file. Future drivers will be compatible with older software
        by examiming this value.

HEADER:
pmapi.h

MEMBERS:
dwSize  - Set to size of structure in bytes
attrib  - Attributes for the file
sizeLo  - Size of the file (low 32-bits)
sizeHi  - Size of the file (high 32-bits)
name    - Name of the file
****************************************************************************/
typedef struct {
    ulong   dwSize;
    ulong   attrib;
    ulong   sizeLo;
    ulong   sizeHi;
    char    name[PM_MAX_PATH];
    } PM_findData;

/****************************************************************************
REMARKS:
Structure passed to the PM_setFileTime functions

HEADER:
pmapi.h

MEMBERS:
sec     - Seconds
min     - Minutes
hour    - Hour (0--23)
day     - Day of month (1--31)
mon     - Month (0--11)
year    - Year (calendar year minus 1900)
****************************************************************************/
typedef struct {
    short   sec;
    short   min;
    short   hour;
    short   day;
    short   mon;
    short   year;
    } PM_time;

/****************************************************************************
REMARKS:
Fundamental type definition for a physical memory address

HEADER:
pmapi.h
****************************************************************************/
typedef unsigned long PM_physAddr;

/****************************************************************************
REMARKS:
Type definition for the lock handle used for locking linear memory

HEADER:
pmapi.h

MEMBERS:
h   - Internal lock handle details
****************************************************************************/
typedef struct {
    ulong   h[3];
    } PM_lockHandle;

/* Type definitions for a window handle for console modes */

#if     defined(__DRIVER__) || defined(__WIN32_VXD__) || defined(__NT_DRIVER__) || defined(__WINCE__)
/****************************************************************************
REMARKS:
Fundamental type definition for a window handle. Note that the portable
version of this define is simply to make it a void pointer, but internally
it will be represented as a pointer to an internal operating system
window handle.

HEADER:
pmapi.h
****************************************************************************/
typedef void        *PM_HWND;

/****************************************************************************
REMARKS:
Fundamental type definition for a module handle. Note that the portable
version of this define is simply to make it a void pointer, but internally
it will be represented as a pointer to an internal operating system
module handle.

HEADER:
pmapi.h
****************************************************************************/
typedef void        *PM_MODULE;
#elif   defined(__WINDOWS__)
#ifdef  DECLARE_HANDLE
typedef HWND        PM_HWND;    /* Real window handle                   */
typedef HINSTANCE   PM_MODULE;  /* Win32 DLL handle                     */
typedef WNDPROC     PM_WNDPROC;
#else
typedef void        *PM_HWND;   /* Place holder if windows.h not included */
typedef void        *PM_MODULE; /* Place holder if windows.h not included */
typedef long        (__stdcall* PM_WNDPROC)(PM_HWND, uint, uint, long);
#endif
#elif   defined(__USE_X11__)
typedef struct {
    Window      *window;
    Display     *display;
    } PM_HWND;                  /* X11 window handle */
#elif   defined(__OS2__)
typedef void    *PM_HWND;
typedef void    *PM_MODULE;
#elif   defined(__LINUX__)
typedef long    PM_HWND;        /* Console id for fullscreen Linux */
typedef void    *PM_MODULE;
#elif   defined(__QNX__)
typedef int     PM_HWND;        /* Console id for fullscreen QNX */
typedef void    *PM_MODULE;
#elif   defined(__RTTARGET__)
typedef int     PM_HWND;        /* Placeholder for RTTarget-32 */
typedef void    *PM_MODULE;
#elif   defined(__REALDOS__)
typedef int     PM_HWND;        /* Placeholder for fullscreen DOS */
typedef void    *PM_MODULE;     /* Placeholder for fullscreen DOS */
#elif   defined(__SMX32__)
typedef int     PM_HWND;        /* Placeholder for fullscreen SMX */
typedef void    *PM_MODULE;
#elif   defined(__SNAP__)
typedef void    *PM_HWND;
typedef void    *PM_MODULE;
#elif   defined(__XFREE86__)
typedef void    *PM_HWND;
typedef void    *PM_MODULE;
#elif   defined(__UUU__)
typedef int     *PM_HWND;
typedef void    *PM_MODULE;
#else
#error  PM library not ported to this platform yet!
#endif


/****************************************************************************
REMARKS:
Type definition for a generic code pointer

HEADER:
pmapi.h
****************************************************************************/
#ifdef __GNUC__
typedef void (*__codePtr)(void);
#else
typedef void (*__codePtr)();
#endif

/****************************************************************************
REMARKS:
Type definition for a C based interrupt handler

HEADER:
pmapi.h
****************************************************************************/
typedef void (PMAPIP PM_intHandler)(void);

/****************************************************************************
REMARKS:
Type definition for a C based hardware interrupt handler

HEADER:
pmapi.h
****************************************************************************/
typedef ibool (PMAPIP PM_irqHandler)(
    void *context);

/****************************************************************************
REMARKS:
Type definition for a Hardware IRQ handle used to save and restore the
hardware IRQ handler.

HEADER:
pmapi.h
****************************************************************************/
typedef void *PM_IRQHandle;

/****************************************************************************
REMARKS:
Type definition for the fatal cleanup handler

HEADER:
pmapi.h
****************************************************************************/
typedef void (PMAPIP PM_fatalCleanupHandler)(void);

/****************************************************************************
REMARKS:
Type definition for suspend application callback function

HEADER:
pmapi.h
****************************************************************************/
typedef int (PMAPIP PM_suspendApp_cb)(
    int flags);

/****************************************************************************
REMARKS:
Type definition for heart beat driver callback function

HEADER:
pmapi.h
****************************************************************************/
typedef void (PMAPIP PM_heartBeat_cb)(
    void *data);

/****************************************************************************
REMARKS:
Type definition for enum write combined callback function

HEADER:
pmapi.h
****************************************************************************/
typedef void (PMAPIP PM_enumWriteCombine_t)(
    ulong base,
    ulong length,
    uint type);

/****************************************************************************
REMARKS:
Structure to define PM_int86 and friends that we can override from
within the SNAP binaries. This allows the BIOS emulator to be exported
from within the SNAP drivers to external client apps using the PM library
functions, which solves the problem of resetting VBE modes in VESA framebuffer
console drivers on non-x86 Linux platforms (such as x86-64).

HEADER:
pmapi.h
****************************************************************************/
typedef struct {
    ulong   dwSize;
    void *  (PMAPIP PM_mapRealPointer)(uint r_seg,uint r_off);
    void *  (PMAPIP PM_getVESABuf)(uint *len,uint *rseg,uint *roff);
    void    (PMAPIP PM_callRealMode)(uint seg,uint off, RMREGS *regs,RMSREGS *sregs);
    int     (PMAPIP PM_int86)(int intno, RMREGS *in, RMREGS *out);
    int     (PMAPIP PM_int86x)(int intno, RMREGS *in, RMREGS *out,RMSREGS *sregs);
    } PM_int86Funcs;

/* TODO: When we restructure all this code for the next generation loader
 *       we should change all this to have a single function imported into
 *       the drivers (QueryImports) and we can have groups of functions
 *       that are imported via ID similar to how QueryFunctions works for
 *       the driver exports. Right now this PM_imports object is getting
 *       a little out of hand!
 */

/* Structure defining all the PM API functions as exported to
 * the binary portable DLL's.
 * {secret}
 */
typedef struct {
    ulong   dwSize;
    int     (PMAPIP PM_getModeType)(void);
    void *  (PMAPIP PM_getBIOSPointer)(void);
    void *  (PMAPIP PM_getA0000Pointer)(void);
    void *  (PMAPIP PM_mapPhysicalAddr)(ulong base,ulong limit,ibool isCached);
    void *  (PMAPIP PM_mallocShared)(long size);
    void *  reserved0;
    void    (PMAPIP PM_freeShared)(void *ptr);
    void *  reserved1;
    void *  (PMAPIP PM_mapRealPointer)(uint r_seg,uint r_off);
    void *  (PMAPIP PM_allocRealSeg)(uint size,uint *r_seg,uint *r_off);
    void    (PMAPIP PM_freeRealSeg)(void *mem);
    void *  (PMAPIP PM_allocLockedMem)(uint size,ulong *physAddr,ibool contiguous,ibool below16Meg);
    void    (PMAPIP PM_freeLockedMem)(void *p,uint size,ibool contiguous);
    void    (PMAPIP PM_callRealMode)(uint seg,uint off, RMREGS *regs,RMSREGS *sregs);
    int     (PMAPIP PM_int86)(int intno, RMREGS *in, RMREGS *out);
    int     (PMAPIP PM_int86x)(int intno, RMREGS *in, RMREGS *out,RMSREGS *sregs);
    void *  reserved2;
    void *  reserved21;
    void *  (PMAPIP PM_getVESABuf)(uint *len,uint *rseg,uint *roff);
    long    (PMAPIP PM_getOSType)(void);
    void    (PMAPIP PM_fatalError)(const char *msg);
    void *  reserved3;
    void *  reserved4;
    void *  reserved5;
    char *  (PMAPIP PM_getCurrentPath)(char *path,int maxLen);
    void *  reserved6;
    const char * (PMAPIP PM_getSNAPPath)(void);
    const char * (PMAPIP PM_getSNAPConfigPath)(void);
    const char * (PMAPIP PM_getUniqueID)(void);
    const char * (PMAPIP PM_getMachineName)(void);
    void *  reserved7;
    void *  reserved8;
    void *  reserved9;
    PM_HWND (PMAPIP PM_openConsole)(PM_HWND hwndUser,int device,int xRes,int yRes,int bpp,ibool fullScreen);
    int     (PMAPIP PM_getConsoleStateSize)(void);
    void    (PMAPIP PM_saveConsoleState)(void *stateBuf,PM_HWND hwndConsole);
    void    (PMAPIP PM_restoreConsoleState)(const void *stateBuf,PM_HWND hwndConsole);
    void    (PMAPIP PM_closeConsole)(PM_HWND hwndConsole);
    void    (PMAPIP PM_setOSCursorLocation)(int x,int y);
    void    (PMAPIP PM_setOSScreenWidth)(int width,int height);
    int     (PMAPIP PM_enableWriteCombine)(ulong base,ulong length,uint type);
    void    (PMAPIP PM_backslash)(char *filename);
    int     (PMAPIP PM_lockDataPages)(void *p,uint len,PM_lockHandle *lockHandle);
    int     (PMAPIP PM_unlockDataPages)(void *p,uint len,PM_lockHandle *lockHandle);
    int     (PMAPIP PM_lockCodePages)(__codePtr p,uint len,PM_lockHandle *lockHandle);
    int     (PMAPIP PM_unlockCodePages)(__codePtr p,uint len,PM_lockHandle *lockHandle);
    ibool   (PMAPIP PM_setRealTimeClockHandler)(PM_intHandler ih,int frequency);
    void    (PMAPIP PM_setRealTimeClockFrequency)(int frequency);
    void    (PMAPIP PM_restoreRealTimeClockHandler)(void);
    void *  reserved10;
    char    (PMAPIP PM_getBootDrive)(void);
    void    (PMAPIP PM_freePhysicalAddr)(void *ptr,ulong limit);
    u8      (PMAPIP PM_inpb)(int port);
    u16     (PMAPIP PM_inpw)(int port);
    u32     (PMAPIP PM_inpd)(int port);
    void    (PMAPIP PM_outpb)(int port,u8 val);
    void    (PMAPIP PM_outpw)(int port,u16 val);
    void    (PMAPIP PM_outpd)(int port,u32 val);
    void *  reserved11;
    void    (PMAPIP PM_setSuspendAppCallback)(PM_suspendApp_cb saveState);
    ibool   (PMAPIP PM_haveBIOSAccess)(void);
    int     (PMAPIP PM_kbhit)(void);
    int     (PMAPIP PM_getch)(void);
    ibool   (PMAPIP PM_findBPD)(const char *dllname,char *bpdpath);
    ulong   (PMAPIP PM_getPhysicalAddr)(void *p);
    void    (PMAPIP PM_sleep)(ulong milliseconds);
    int     (PMAPIP PM_getCOMPort)(int port);
    int     (PMAPIP PM_getLPTPort)(int port);
    PM_MODULE (PMAPIP PM_loadLibrary)(const char *szDLLName);
    void *  (PMAPIP PM_getProcAddress)(PM_MODULE hModule,const char *szProcName);
    void    (PMAPIP PM_freeLibrary)(PM_MODULE hModule);
    int     (PMAPIP PCI_enumerate)(PCIDeviceInfo info[]);
    u32     (PMAPIP PCI_accessReg)(int index,u32 value,int func,PCIDeviceInfo *info);
    ibool   (PMAPIP PCI_setHardwareIRQ)(PCIDeviceInfo *info,uint intPin,uint IRQ);
    void    (PMAPIP PCI_generateSpecialCyle)(uint bus,u32 specialCycleData);
    void *  reserved12;
    u32     (PMAPIP PCIBIOS_getEntry)(void);
    uint    (PMAPIP CPU_getProcessorType)(void);
    ibool   (PMAPIP CPU_haveMMX)(void);
    ibool   (PMAPIP CPU_have3DNow)(void);
    ibool   (PMAPIP CPU_haveSSE)(void);
    ibool   (PMAPIP CPU_haveRDTSC)(void);
    ulong   (PMAPIP CPU_getProcessorSpeed)(ibool accurate);
    void    (PMAPIP ZTimerInit)(void);
    void    (PMAPIP LZTimerOn)(void);
    ulong   (PMAPIP LZTimerLap)(void);
    void    (PMAPIP LZTimerOff)(void);
    ulong   (PMAPIP LZTimerCount)(void);
    void    (PMAPIP LZTimerOnExt)(LZTimerObject *tm);
    ulong   (PMAPIP LZTimerLapExt)(LZTimerObject *tm);
    void    (PMAPIP LZTimerOffExt)(LZTimerObject *tm);
    ulong   (PMAPIP LZTimerCountExt)(LZTimerObject *tm);
    void    (PMAPIP ULZTimerOn)(void);
    ulong   (PMAPIP ULZTimerLap)(void);
    void    (PMAPIP ULZTimerOff)(void);
    ulong   (PMAPIP ULZTimerCount)(void);
    ulong   (PMAPIP ULZReadTime)(void);
    ulong   (PMAPIP ULZElapsedTime)(ulong start,ulong finish);
    void    (PMAPIP ULZTimerResolution)(ulong *resolution);
    void *  (PMAPIP PM_findFirstFile)(const char *filename,PM_findData *findData);
    ibool   (PMAPIP PM_findNextFile)(void *handle,PM_findData *findData);
    void    (PMAPIP PM_findClose)(void *handle);
    void    (PMAPIP PM_makepath)(char *p,const char *drive,const char *dir,const char *name,const char *ext);
    int     (PMAPIP PM_splitpath)(const char *fn,char *drive,char *dir,char *name,char *ext);
    void *  reserved13;
    void    (PMAPIP PM_getdcwd)(int drive,char *dir,int len);
    void    (PMAPIP PM_setFileAttr)(const char *filename,uint attrib);
    ibool   (PMAPIP PM_mkdir)(const char *filename);
    ibool   (PMAPIP PM_rmdir)(const char *filename);
    uint    (PMAPIP PM_getFileAttr)(const char *filename);
    ibool   (PMAPIP PM_getFileTime)(const char *filename,ibool gmtTime,PM_time *time);
    ibool   (PMAPIP PM_setFileTime)(const char *filename,ibool gmtTime,PM_time *time);
    char *  (PMAPIP CPU_getProcessorName)(void);
    int     (PMAPIP PM_getVGAStateSize)(void);
    void    (PMAPIP PM_saveVGAState)(void *stateBuf);
    void    (PMAPIP PM_restoreVGAState)(const void *stateBuf);
    void *  reserved14;
    void *  reserved15;
    void    (PMAPIP PM_blockUntilTimeout)(ulong milliseconds);
    void    (PMAPIP _PM_add64)(u32 a_low,s32 a_high,u32 b_low,s32 b_high,__i64 *result);
    void    (PMAPIP _PM_sub64)(u32 a_low,s32 a_high,u32 b_low,s32 b_high,__i64 *result);
    void    (PMAPIP _PM_mul64)(u32 a_low,s32 a_high,u32 b_low,s32 b_high,__i64 *result);
    void    (PMAPIP _PM_div64)(u32 a_low,s32 a_high,u32 b_low,s32 b_high,__i64 *result);
    void    (PMAPIP _PM_shr64)(u32 a_low,s32 a_high,s32 shift,__i64 *result);
    void    (PMAPIP _PM_sar64)(u32 a_low,s32 a_high,s32 shift,__i64 *result);
    void    (PMAPIP _PM_shl64)(u32 a_low,s32 a_high,s32 shift,__i64 *result);
    void    (PMAPIP _PM_neg64)(u32 a_low,s32 a_high,__i64 *result);
    void *  reserved16;
    void    (PMAPIP PCI_readRegBlock)(PCIDeviceInfo *info,int index,void *dst,int count);
    void    (PMAPIP PCI_writeRegBlock)(PCIDeviceInfo *info,int index,void *src,int count);
    void    (PMAPIP PM_flushTLB)(void);
    void    (PMAPIP PM_useLocalMalloc)(void * (*malloc)(size_t size),void * (*calloc)(size_t nelem,size_t size),void * (*realloc)(void *ptr,size_t size),void (*free)(void *p));
    void *  (PMAPIP PM_malloc)(size_t size);
    void *  (PMAPIP PM_calloc)(size_t nelem,size_t size);
    void *  (PMAPIP PM_realloc)(void *ptr,size_t size);
    void    (PMAPIP PM_free)(void *p);
    ibool   (PMAPIP PM_getPhysicalAddrRange)(void *p,ulong length,ulong *physAddress);
    void *  (PMAPIP PM_allocPage)(ibool locked);
    void    (PMAPIP PM_freePage)(void *p);
    ulong   (PMAPIP PM_agpInit)(void);
    void    (PMAPIP PM_agpExit)(void);
    ibool   (PMAPIP PM_agpReservePhysical)(ulong numPages,int type,void **physContext,PM_physAddr *physAddr);
    ibool   (PMAPIP PM_agpReleasePhysical)(void *physContext);
    ibool   (PMAPIP PM_agpCommitPhysical)(void *physContext,ulong numPages,ulong startOffset,PM_physAddr *physAddr);
    ibool   (PMAPIP PM_agpFreePhysical)(void *physContext,ulong numPages,ulong startOffset);
    int     (PMAPIP PCI_getNumDevices)(void);
    void    (PMAPIP PM_setLocalBPDPath)(const char *path);
    void *  (PMAPIP PM_loadDirectDraw)(int device);
    void    (PMAPIP PM_unloadDirectDraw)(int device);
    PM_HWND (PMAPIP PM_getDirectDrawWindow)(void);
    void    (PMAPIP PM_doSuspendApp)(void);
    ulong   (PMAPIP PM_setMaxThreadPriority)(void);
    void    (PMAPIP PM_restoreThreadPriority)(ulong oldPriority);
    char *  (PMAPIP PM_getOSName)(void);
    void    (PMAPIP _CHK_defaultFail)(int fatal,const char *msg,const char *cond,const char *file,int line);
    ibool   (PMAPIP PM_isSDDActive)(void);
    ibool   (PMAPIP PM_runningInAWindow)(void);
    void    (PMAPIP PM_stopRealTimeClock)(void);
    void    (PMAPIP PM_restartRealTimeClock)(int frequency);
    void    (PMAPIP PM_saveFPUState)(void);
    void    (PMAPIP PM_restoreFPUState)(void);
    void    (PMAPIP CPU_setDefaultProcessorSpeed)(ulong speed);
    ibool   (PMAPIP PM_getOSResolution)(int *width,int *height,int *bitsPerPixel);
    ibool   (PMAPIP PM_memProtect)(void *base, size_t length, int flags);
    PM_IRQHandle (PMAPIP PM_setIRQHandler)(int IRQ,PM_irqHandler ih,void *context);
    void    (PMAPIP PM_restoreIRQHandler)(PM_IRQHandle irqHandle);
    void    (PMAPIP PM_DMACEnable)(int channel);
    void    (PMAPIP PM_DMACDisable)(int channel);
    void    (PMAPIP PM_DMACProgram)(int channel,int mode,ulong bufferPhys,int count);
    ulong   (PMAPIP PM_DMACPosition)(int channel);
    void    (PMAPIP PM_lockSNAPAccess)(int subsystem,ibool writeLock);
    void    (PMAPIP PM_unlockSNAPAccess)(int subsystem);
    void    (PMAPIP PM_registerHeartBeatCallback)(PM_heartBeat_cb cb,void *data);
    void    (PMAPIP PM_unregisterHeartBeatCallback)(PM_heartBeat_cb cb);
    void    (PMAPIP PM_registerBIOSEmulator)(PM_int86Funcs *biosFuncs);
    } PM_imports;

#pragma pack()

/*---------------------------- Global variables ---------------------------*/

#ifdef  __cplusplus
extern "C" {            /* Use "C" linkage when in C++ mode */
#endif

#ifdef  __WIN32_VXD__
#define VESA_BUF_SIZE 1024
/* {secret} */
extern uchar *_PM_rmBufAddr;
#endif

/* {secret} Pointer to global exports structure.
 * Should not be used by application programs.
 */
extern PM_imports _VARAPI _PM_imports;

/* {secret} */
extern void * (*__PM_malloc)(size_t size);
/* {secret} */
extern void * (*__PM_calloc)(size_t nelem,size_t size);
/* {secret} */
extern void * (*__PM_realloc)(void *ptr,size_t size);
/* {secret} */
extern void (*__PM_free)(void *p);

#ifdef __MSDOS__
/* {secret} */
extern uint _VARAPI    _PM_VXD_off;
/* {secret} */
extern uint _VARAPI    _PM_VXD_sel;
/* {secret} */
void    _ASMAPI _PM_VxDCall(VXD_regs *regs,uint off,uint sel);
#endif

/*--------------------------- Function Prototypes -------------------------*/

/* Routine to initialise the host side PM library. Note used from DLL's */

void    PMAPI PM_init(void);

/* Routine to return a selector to the BIOS data area at segment 0x40 */

void *  PMAPI PM_getBIOSPointer(void);

/* Routine to return a linear pointer to the VGA frame buffer memory */

void *  PMAPI PM_getA0000Pointer(void);

/* Routines to map/free physical memory into the current DS segment. In
 * some environments (32-bit DOS is one), after the mapping has been
 * allocated, it cannot be freed. Hence you should only allocate the
 * mapping once and cache the value for use by other parts of your
 * application. If the mapping cannot be createed, this function will
 * return a NULL pointer.
 *
 * This routine will also work for memory addresses below 1Mb, but the
 * mapped address cannot cross the 1Mb boundary.
 */

void *  PMAPI PM_mapPhysicalAddr(ulong base,ulong limit,ibool isCached);
void    PMAPI PM_freePhysicalAddr(void *ptr,ulong limit);

/* Routine to determine the physical address of a linear address. It is
 * up to the caller to ensure the entire address range for a linear
 * block of memory is page aligned if that is required.
 */

ulong   PMAPI PM_getPhysicalAddr(void *p);
ibool   PMAPI PM_getPhysicalAddrRange(void *p,ulong length,ulong *physAddress);

/* Routines for memory allocation. By default these functions use the regular
 * C runtime library malloc/free functions, but you can use the
 * PM_useLocalMalloc function to override the default memory allocator with
 * your own memory allocator. This will ensure that all memory allocation
 * used by SciTech products will use your overridden memory allocator
 * functions.
 *
 * Note that BPD files automatically map the C runtime library
 * malloc/calloc/realloc/free calls from inside the BPD to the PM library
 * versions by default.
 */

void    PMAPI PM_useLocalMalloc(void * (*malloc)(size_t size),void * (*calloc)(size_t nelem,size_t size),void * (*realloc)(void *ptr,size_t size),void (*free)(void *p));
void *  PMAPI PM_malloc(size_t size);
void *  PMAPI PM_calloc(size_t nelem,size_t size);
void *  PMAPI PM_realloc(void *ptr,size_t size);
void    PMAPI PM_free(void *p);

/* Routine to allocate a memory block in the global shared region that
 * is common to all tasks and accessible from ring 0 code.
 */

void *  PMAPI PM_mallocShared(long size);

/* Routine to free the allocated shared memory block */

void    PMAPI PM_freeShared(void *ptr);

/* Macros to extract byte, word and long values from a char pointer */

#define PM_getByte(p)       *((volatile uchar*)(p))
#define PM_getWord(p)       *((volatile ushort*)(p))
#define PM_getLong(p)       *((volatile ulong*)(p))
#define PM_setByte(p,v)     PM_getByte(p) = (v)
#define PM_setWord(p,v)     PM_getWord(p) = (v)
#define PM_setLong(p,v)     PM_getLong(p) = (v)

/* Routine for accessing a low 1Mb memory block. You dont need to free this
 * pointer, but in 16 bit protected mode the selector allocated will be
 * re-used the next time this routine is called.
 */

void *  PMAPI PM_mapRealPointer(uint r_seg,uint r_off);

/* Routine to allocate a block of conventional memory below the 1Mb
 * limit so that it can be accessed from real mode. Ensure that you free
 * the segment when you are done with it.
 *
 * This routine returns a selector and offset to the segment that has been
 * allocated, and also returns the real mode segment and offset which can
 * be passed to real mode routines. Will return 0 if memory could not be
 * allocated.
 *
 * Please note that with some DOS extenders, memory allocated with the
 * following function cannot be freed, hence it will be allocated for the
 * life of your program. Thus if you need to call a bunch of different
 * real-mode routines in your program, allocate a single large buffer at
 * program startup that can be re-used throughout the program execution.
 */

void *  PMAPI PM_allocRealSeg(uint size,uint *r_seg,uint *r_off);
void    PMAPI PM_freeRealSeg(void *mem);

/* Routine to allocate a block of locked memory, and return both the
 * linear and physical addresses of the memory. You should always
 * allocate locked memory blocks in page sized chunks (ie: 4K on IA32).
 * If the memory is not contiguous, you will need to use the
 * PM_getPhysicalAddr function to get the physical address of linear
 * pages within the memory block (the returned physical address will be
 * for the first address in the memory block only).
 */

void *  PMAPI PM_allocLockedMem(uint size,ulong *physAddr,ibool contiguous,ibool below16Meg);
void    PMAPI PM_freeLockedMem(void *p,uint size,ibool contiguous);

/* Routine to allocate and free paged sized blocks of shared memory.
 * Addressable from all processes, but not from a ring 0 context
 * under OS/2. Note that under OS/2 PM_mapSharedPages must be called
 * to map the memory blocks into the shared memory address space
 * of each connecting process.
 */

void *  PMAPI PM_allocPage(ibool locked);
void    PMAPI PM_freePage(void *p);
#ifdef __OS2__
/* {secret} */
void    PMAPI PM_mapSharedPages(void);
#endif

/* Routine to change page level protection on a block of memory */
ibool   PMAPI PM_memProtect(void *base, size_t length, int flags);

/* Routine to return true if we have access to the BIOS on the host OS */

ibool   PMAPI PM_haveBIOSAccess(void);

/* Routine to call a real mode assembly language procedure. Register
 * values are passed in and out in the 'regs' and 'sregs' structures. We
 * do not provide any method of copying data from the protected mode stack
 * to the real mode stack, so if you need to pass data to real mode, you will
 * need to write a real mode assembly language hook to recieve the values
 * in registers, and to pass the data through a real mode block allocated
 * with the PM_allocRealSeg() routine.
 */

void    PMAPI PM_callRealMode(uint seg,uint off, RMREGS *regs,RMSREGS *sregs);

/* Routines to generate real mode interrupts using the same interface that
 * is used by int86() and int86x() in realmode. This routine is need to
 * call certain BIOS and DOS functions that are not supported by some
 * DOS extenders. No translation is done on any of the register values,
 * so they must be correctly set up and translated by the calling program.
 *
 * Normally the DOS extenders will allow you to use the normal int86()
 * function directly and will pass on unhandled calls to real mode to be
 * handled by the real mode handler. However calls to int86x() with real
 * mode segment values to be loaded will cause a GPF if used with the
 * standard int86x(), so you should use these routines if you know you
 * want to call a real mode handler.
 */

int     PMAPI PM_int86(int intno, RMREGS *in, RMREGS *out);
int     PMAPI PM_int86x(int intno, RMREGS *in, RMREGS *out,RMSREGS *sregs);

/* Return the address of a global VESA real mode transfer buffer for use
 * by applications.
 */

void *  PMAPI PM_getVESABuf(uint *len,uint *rseg,uint *roff);

/* Function to register BIOS emulator override functions */

void    PMAPI PM_registerBIOSEmulator(PM_int86Funcs *biosFuncs);

/* Handle fatal error conditions */

void    PMAPI PM_fatalError(const char *msg);

/* Function to set a cleanup error handler called when PM_fatalError
 * is called. This allows us to the console back into a normal state
 * if we get a failure from deep inside a BPD file. This function is
 * not exported to BPD files, and is only used by code compiled for the
 * OS.
 */

void    PMAPI PM_setFatalErrorCleanup(PM_fatalCleanupHandler cleanup);

/* Return the OS type flag as defined in <clib/os/os.h> */

long    PMAPI PM_getOSType(void);

/* Return the name of the OS as a string */

char *  PMAPI PM_getOSName(void);

/* Return the current working directory */

char *  PMAPI PM_getCurrentPath(char *path,int maxLen);

/* Return paths to the SciTech SNAP directories */

const char * PMAPI PM_getSNAPPath(void);
const char * PMAPI PM_getSNAPConfigPath(void);

/* Find the path to a binary portable DLL */

void    PMAPI PM_setLocalBPDPath(const char *path);
ibool   PMAPI PM_findBPD(const char *dllname,char *bpdpath);

/* Returns the drive letter of the boot drive for DOS, OS/2 and Windows */

char    PMAPI PM_getBootDrive(void);

/* Return a network unique machine identifier as a string */

const char * PMAPI PM_getUniqueID(void);

/* Return the network machine name as a string */

const char * PMAPI PM_getMachineName(void);

/* Functions to wait for a keypress and read a key for command line
 * environments such as DOS, Win32 console and Unix.
 */

int     PMAPI PM_kbhit(void);
int     PMAPI PM_getch(void);

/* Functions to create either a fullscreen or windowed console on the
 * desktop, and to allow the resolution of fullscreen consoles to be
 * changed on the fly without closing the console. For non-windowed
 * environments (such as a Linux or OS/2 fullscreen console), these
 * functions enable console graphics mode and restore console text mode.
 *
 * The suspend application callback is used to allow the application to
 * save the state of the fullscreen console mode to allow temporary
 * switching to another console or back to the regular GUI desktop. It
 * is also called to restore the fullscreen graphics state after the
 * fullscreen console regains the focus.
 *
 * The device parameter allows for the console to be opened on a different
 * display controllers (0 is always the primary controller).
 */

PM_HWND PMAPI PM_openConsole(PM_HWND hwndUser,int device,int xRes,int yRes,int bpp,ibool fullScreen);
int     PMAPI PM_getConsoleStateSize(void);
void    PMAPI PM_saveConsoleState(void *stateBuf,PM_HWND hwndConsole);
void    PMAPI PM_setSuspendAppCallback(PM_suspendApp_cb saveState);
void    PMAPI PM_restoreConsoleState(const void *stateBuf,PM_HWND hwndConsole);
void    PMAPI PM_closeConsole(PM_HWND hwndConsole);

/* Windows specific function to register a user window event filter */

#if defined(__WINDOWS32__)
void    PMAPI PM_registerEventProc(PM_WNDPROC userWndProc);
#endif

/* Functions to modify OS console information */

void    PMAPI PM_setOSCursorLocation(int x,int y);
void    PMAPI PM_setOSScreenWidth(int width,int height);

/* Function to emable Intel PPro/PII write combining */

int     PMAPI PM_enableWriteCombine(ulong base,ulong length,uint type);
int     PMAPI PM_enumWriteCombine(PM_enumWriteCombine_t callback);

/* Function to add a path separator to the end of a filename (if not present) */

void    PMAPI PM_backslash(char *filename);

/* Routines to lock and unlock regions of memory under a virtual memory
 * environment. These routines _must_ be used to lock all hardware
 * and mouse interrupt handlers installed, _AND_ any global data that
 * these handler manipulate, so that they will always be present in memory
 * to handle the incoming interrupts.
 *
 * Note that it is important to call the correct routine depending on
 * whether the area being locked is code or data, so that under 32 bit
 * PM we will get the selector value correct.
 */

int     PMAPI PM_lockDataPages(void *p,uint len,PM_lockHandle *lockHandle);
int     PMAPI PM_unlockDataPages(void *p,uint len,PM_lockHandle *lockHandle);
int     PMAPI PM_lockCodePages(__codePtr p,uint len,PM_lockHandle *lockHandle);
int     PMAPI PM_unlockCodePages(__codePtr p,uint len,PM_lockHandle *lockHandle);

/* Routines to install and remove Real Time Clock interrupt handlers. The
 * frequency of the real time clock can be changed by calling
 * PM_setRealTimeClockFrequeny, and the value can be any power of 2 value
 * from 2Hz to 8192Hz.
 *
 * Note that you _must_ lock the memory containing the interrupt
 * handlers with the PM_lockPages() function otherwise you may encounter
 * problems in virtual memory environments.
 *
 * NOTE: User space versions of the PM library should fail these functions.
 */

ibool   PMAPI PM_setRealTimeClockHandler(PM_intHandler ih,int frequency);
void    PMAPI PM_setRealTimeClockFrequency(int frequency);
void    PMAPI PM_stopRealTimeClock(void);
void    PMAPI PM_restartRealTimeClock(int frequency);
void    PMAPI PM_restoreRealTimeClockHandler(void);

/* Routines to install and remove hardware interrupt handlers. Allows a
 * context pointer to be passed to the interrupt handler for each
 * interrupt that is hooked.
 *
 * Note that you _must_ lock the memory containing the interrupt
 * handlers with the PM_lockPages() function otherwise you may encounter
 * problems in virtual memory environments.
 *
 * NOTE: User space versions of the PM library should fail these functions.
 */

/* {secret} - Only for DOS presently */
PM_IRQHandle PMAPI PM_setIRQHandler(int IRQ,PM_irqHandler ih,void *context);
/* {secret} - Only for DOS presently */
void    PMAPI PM_restoreIRQHandler(PM_IRQHandle irqHandle);

/* Functions to program DMA using the legacy ISA DMA controller */

/* {secret} - Only for DOS presently */
void    PMAPI PM_DMACEnable(int channel);
/* {secret} - Only for DOS presently */
void    PMAPI PM_DMACDisable(int channel);
/* {secret} - Only for DOS presently */
void    PMAPI PM_DMACProgram(int channel,int mode,ulong bufferPhys,int count);
/* {secret} - Only for DOS presently */
ulong   PMAPI PM_DMACPosition(int channel);

/* Function to init the AGP functions and return the AGP aperture size in MB */

ulong   PMAPI PM_agpInit(void);
void    PMAPI PM_agpExit(void);

/* Functions to reserve and release physical AGP memory ranges */

ibool   PMAPI PM_agpReservePhysical(ulong numPages,int type,void **physContext,PM_physAddr *physAddr);
ibool   PMAPI PM_agpReleasePhysical(void *physContext);

/* Functions to commit and free physical AGP memory ranges */

ibool   PMAPI PM_agpCommitPhysical(void *physContext,ulong numPages,ulong startOffset,PM_physAddr *physAddr);
ibool   PMAPI PM_agpFreePhysical(void *physContext,ulong numPages,ulong startOffset);

/* Function to register a heart beat callback function */
void    PMAPI PM_registerHeartBeatCallback(PM_heartBeat_cb cb,void *data);
void    PMAPI PM_unregisterHeartBeatCallback(PM_heartBeat_cb cb);

/* Functions to do I/O port manipulation directly from C code. These
 * functions are portable and will work on any processor architecture
 * to access I/O space registers on PCI devices.
 */

u8      PMAPI PM_inpb(int port);
u16     PMAPI PM_inpw(int port);
u32     PMAPI PM_inpd(int port);
void    PMAPI PM_outpb(int port,u8 val);
void    PMAPI PM_outpw(int port,u16 val);
void    PMAPI PM_outpd(int port,u32 val);

/* Functions to determine the I/O port locations for COM and LPT ports.
 * The functions are zero based, so for COM1 or LPT1 pass in a value of 0,
 * for COM2 or LPT2 pass in a value of 1 etc.
 */

int     PMAPI PM_getCOMPort(int port);
int     PMAPI PM_getLPTPort(int port);

/* Internal functions that need prototypes */

/* {secret} */
void    PMAPI _PM_getRMvect(int intno, long *realisr);
/* {secret} */
void    PMAPI _PM_setRMvect(int intno, long realisr);
/* {secret} */
void    PMAPI _PM_freeMemoryMappings(void);

/* Function to override the default debug log file location */

void    PMAPI PM_setDebugLog(const char *logFilePath);

/* Function to put the process to sleep for the specified milliseconds */

void    PMAPI PM_sleep(ulong milliseconds);

/* Functions to set and restore thread priority. This function will
 * increase the priority of the current process/thread to the maximum
 * allowed and is used for short periods of time for short code that
 * needs to do calibration loops etc.
 */

ulong   PMAPI PM_setMaxThreadPriority(void);
void    PMAPI PM_restoreThreadPriority(ulong oldPriority);

/* Function to block until 'milliseconds' have passed since last call */

void    PMAPI PM_blockUntilTimeout(ulong milliseconds);

/* Functions for directory traversal and management */

void *  PMAPI PM_findFirstFile(const char *filename,PM_findData *findData);
ibool   PMAPI PM_findNextFile(void *handle,PM_findData *findData);
void    PMAPI PM_findClose(void *handle);
void    PMAPI PM_makepath(char *p,const char *drive,const char *dir,const char *name,const char *ext);
int     PMAPI PM_splitpath(const char *fn,char *drive,char *dir,char *name,char *ext);
void    PMAPI PM_getdcwd(int drive,char *dir,int len);
uint    PMAPI PM_getFileAttr(const char *filename);
void    PMAPI PM_setFileAttr(const char *filename,uint attrib);
ibool   PMAPI PM_getFileTime(const char *filename,ibool gmTime,PM_time *time);
ibool   PMAPI PM_setFileTime(const char *filename,ibool gmTime,PM_time *time);
ibool   PMAPI PM_mkdir(const char *filename);
ibool   PMAPI PM_rmdir(const char *filename);

/* Functions to handle loading OS specific shared libraries */

PM_MODULE PMAPI PM_loadLibrary(const char *szDLLName);
void *  PMAPI PM_getProcAddress(PM_MODULE hModule,const char *szProcName);
void    PMAPI PM_freeLibrary(PM_MODULE hModule);

/* Functions and macros for 64-bit arithmetic */

/* {secret} */
void    PMAPI _PM_add64(u32 a_low,s32 a_high,u32 b_low,s32 b_high,__i64 *result);
/* {secret} */
void    PMAPI _PM_sub64(u32 a_low,s32 a_high,u32 b_low,s32 b_high,__i64 *result);
/* {secret} */
void    PMAPI _PM_mul64(u32 a_low,s32 a_high,u32 b_low,s32 b_high,__i64 *result);
/* {secret} */
void    PMAPI _PM_div64(u32 a_low,s32 a_high,u32 b_low,s32 b_high,__i64 *result);
/* {secret} */
void    PMAPI _PM_shr64(u32 a_low,s32 a_high,s32 shift,__i64 *result);
/* {secret} */
void    PMAPI _PM_sar64(u32 a_low,s32 a_high,s32 shift,__i64 *result);
/* {secret} */
void    PMAPI _PM_shl64(u32 a_low,s32 a_high,s32 shift,__i64 *result);
/* {secret} */
void    PMAPI _PM_neg64(u32 a_low,s32 a_high,__i64 *result);
#ifdef __NATIVE_INT64__
#define PM_add64(r,a,b)     (r) = (a) + (b)
#define PM_add64_32(r,a,b)  (r) = (a) + (b)
#define PM_sub64(r,a,b)     (r) = (a) - (b)
#define PM_sub64_32(r,a,b)  (r) = (a) - (b)
#define PM_mul64(r,a,b)     (r) = (a) * (b)
#define PM_mul64_32(r,a,b)  (r) = (a) * (b)
#define PM_div64(r,a,b)     (r) = (a) / (b)
#define PM_div64_32(r,a,b)  (r) = (a) / (b)
#define PM_shr64(r,a,s)     (r) = (a) >> (s)
#define PM_sar64(r,a,s)     (r) = ((s64)(a)) >> (s)
#define PM_shl64(r,a,s)     (r) = (u64)(a) << (s)
#define PM_neg64(r,a,s)     (r) = -(a)
#define PM_not64(r,a,s)     (r) = ~(a)
#define PM_eq64(a,b)        (a) == (b)
#define PM_gt64(a,b)        (a) > (b)
#define PM_lt64(a,b)        (a) < (b)
#define PM_geq64(a,b)       (a) >= (b)
#define PM_leq64(a,b)       (a) <= (b)
#define PM_eq64_32(a,b)     (a) == (b)
#define PM_gt64_32(a,b)     (a) > (b)
#define PM_lt64_32(a,b)     (a) < (b)
#define PM_geq64_32(a,b)    (a) >= (b)
#define PM_leq64_32(a,b)    (a) <= (b)
#define PM_64to32(a)        (u32)(a)
#define PM_64tos32(a)       (s32)(a)
#define PM_set64(a,b,c)     (a) = ((u64)(b) << 32) + (c)
#define PM_set64_32(a,b)    (a) = (b)
#define PM_getlo32(a)       (u32)(a)
#define PM_gethi32(a)       (u32)(a >> 32)
#else
#define PM_add64(r,a,b)     _PM_add64((a).low,(a).high,(b).low,(b).high,&(r))
#define PM_add64_32(r,a,b)  _PM_add64((a).low,(a).high,b,0,&(r))
#define PM_sub64(r,a,b)     _PM_sub64((a).low,(a).high,(b).low,(b).high,&(r))
#define PM_sub64_32(r,a,b)  _PM_sub64((a).low,(a).high,b,0,&(r))
#define PM_mul64(r,a,b)     _PM_mul64((a).low,(a).high,(b).low,(b).high,&(r))
#define PM_mul64_32(r,a,b)  _PM_mul64((a).low,(a).high,b,0,&(r))
#define PM_div64(r,a,b)     _PM_div64((a).low,(a).high,(b).low,(b).high,&(r))
#define PM_div64_32(r,a,b)  _PM_div64((a).low,(a).high,b,0,&(r))
#define PM_shr64(r,a,s)     _PM_shr64((a).low,(a).high,s,&(r))
#define PM_sar64(r,a,s)     _PM_sar64((a).low,(a).high,s,&(r))
#define PM_shl64(r,a,s)     _PM_shl64((a).low,(a).high,s,&(r))
#define PM_neg64(r,a,s)     _PM_neg64((a).low,(a).high,&(r))
#define PM_not64(r,a,s)     (r).low = ~(a).low, (r).high = ~(a).high
#define PM_eq64(a,b)        ((a).low == (b).low && (a).high == (b).high)
#define PM_gt64(a,b)        (((a).high > (b).high) || ((a).high == (b).high && (a).low > (b).low))
#define PM_lt64(a,b)        (((a).high < (b).high) || ((a).high == (b).high && (a).low < (b).low))
#define PM_geq64(a,b)       (PM_eq64(a,b) || PM_gt64(a,b))
#define PM_leq64(a,b)       (PM_eq64(a,b) || PM_lt64(a,b))
#define PM_eq64_32(a,b)     ((a).high == 0 && (a).low == (b))
#define PM_gt64_32(a,b)     (((a).high > 0) || ((a).high == 0 && (a).low > (b)))
#define PM_lt64_32(a,b)     ((a).high == 0 && (a).low < (b))
#define PM_geq64_32(a,b)    (PM_eq64_32(a,b) || PM_gt64_32(a,b))
#define PM_leq64_32(a,b)    (PM_eq64_32(a,b) || PM_lt64_32(a,b))
#define PM_64to32(a)        (u32)(a.low)
#define PM_64tos32(a)       (((a).high < 0) ? -(a).low : (a).low)
#define PM_set64(a,b,c)     (a).high = (b), (a).low = (c)
#define PM_set64_32(a,b)    (a).high = 0, (a).low = (b)
#define PM_getlo32(a)       (u32)(a.low)
#define PM_gethi32(a)       (u32)(a.high)
#endif

/* Function to enable IOPL access if required */

int     PMAPI PM_setIOPL(int iopl);

/* Function to get current IOPL level (callable from any ring) */

int     PMAPI PM_getIOPL(void);

/* Function to flush the TLB and CPU caches */

void    PMAPI PM_flushTLB(void);

/* DOS specific fucntions */

#ifdef  __MSDOS__
/* {secret} */
uint    PMAPI PMHELP_getVersion(void);
/* {secret} */
void    PMAPI PM_VxDCall(VXD_regs *regs);
#endif

/* Functions to save and restore the VGA hardware state */

int     PMAPI PM_getVGAStateSize(void);
void    PMAPI PM_saveVGAState(void *stateBuf);
void    PMAPI PM_restoreVGAState(const void *stateBuf);

/* Functions to save and restore the Linux framebuffer console hardware state */

#if defined(__LINUX__) || defined(__XFREE86__)
ibool   PMAPI PM_saveFBConsoleState(int fd,void *stateBuf);
void    PMAPI PM_restoreFBConsoleState(int fd,const void *stateBuf);
#endif

/* Functions to load and unload DirectDraw libraries. Only used on
 * Windows platforms.
 */

void *  PMAPI PM_loadDirectDraw(int device);
void    PMAPI PM_unloadDirectDraw(int device);
PM_HWND PMAPI PM_getDirectDrawWindow(void);
void    PMAPI PM_doSuspendApp(void);

/* Functions to install, start, stop and remove NT services. Valid only
 * for Win32 apps running on Windows NT.
 */

#ifdef __WINDOWS32__
ulong   PMAPI PM_installServiceExt(const char *szDriverName,const char *szServiceName,const char *szLoadGroup,ulong dwServiceType,ulong dwStartType);
ulong   PMAPI PM_installService(const char *szDriverName,const char *szServiceName,const char *szLoadGroup,ulong dwServiceType);
ulong   PMAPI PM_startService(const char *szServiceName);
ulong   PMAPI PM_queryService(const char *szServiceName);
ulong   PMAPI PM_stopService(const char *szServiceName);
ulong   PMAPI PM_removeService(const char *szServiceName);
#endif

/* Internal functions to register the shared exports table pointer with
 * the PM library shared memory sub-system.
 */
/* {secret} */
void    PMAPI PM_setGASharedExports(void *gaexports,int size);
/* {secret} */
void *  PMAPI PM_getGASharedExports(void);

/* Function to enable/disable global memory mapping (default is disabled).
 * Valid only for Win32 apps on Windows NT (on Windows 9x it does nothing).
 */

#if defined(__WINDOWS32__) || defined(__WIN32_VXD__) || defined(__NT_DRIVER__)
/* {secret} */
void    PMAPI PM_enableGlobalAddressMapping(ibool enable);
#endif

/* Function to enable/disable using SDDHELP.VXD/SYS on Win32 */

#if defined(__WINDOWS32__)
/* {secret} */
void    PMAPI PM_useSDDHELP(ibool enable);
/* {secret} */
ibool   PMAPI PM_isSDDHELPActive(void);
/* {secret} */
void    PMAPI PM_rebuildSDDModeList(void);
#endif

/* Function to determine if SDD is the active display driver in the system */
ibool   PMAPI PM_isSDDActive(void);

/* Function to determine if the app is running in a window on the desktop */
ibool   PMAPI PM_runningInAWindow(void);

/* Functions to lock and unlock exclusive access to a specific SNAP subsystem */
void    PMAPI PM_lockSNAPAccess(int subsystem,ibool writeLock);
void    PMAPI PM_unlockSNAPAccess(int subsystem);

/* Functions to save/restore the floating point state */
void    PMAPI PM_saveFPUState(void);
void    PMAPI PM_restoreFPUState(void);

/* Function to get the OS console or GUI current graphics resolution info */
ibool   PMAPI PM_getOSResolution(int *width,int *height,int *bitsPerPixel);

/* Function to enable/disable file filter driver on WinNT */

#if defined(__WINDOWS32__)
/* {secret} */
void    PMAPI PM_enableFileFilter(ibool enable);
#endif

/* Functions to read/write MSR registers */
/* {secret} */
void    PMAPI PM_readMSR(ulong reg, ulong PM_FAR *eax, ulong PM_FAR *edx);
/* {secret} */
void    PMAPI PM_writeMSR(ulong reg, ulong eax, ulong edx);

/* Routines to generate native interrupts (ie: protected mode interrupts
 * for protected mode apps) using an interface the same as that use by
 * int86() and int86x() in realmode. These routines are required because
 * many 32 bit compilers use different register structures and different
 * functions causing major portability headaches. Thus we provide our
 * own and solve it all in one fell swoop, and we also get a routine to
 * put stuff into 32 bit registers from real mode ;-)
 */

/* {secret} */
void    PMAPI PM_segread(PMSREGS *sregs);
/* {secret} */
int     PMAPI PM_int386(int intno, PMREGS *in, PMREGS *out);
/* {secret} */
int     PMAPI PM_int386x(int intno, PMREGS *in, PMREGS *out,PMSREGS *sregs);

/* Call the X86 emulator or the real BIOS in our test harness */

#if defined(TEST_HARNESS) && !defined(PMLIB)
#define PM_mapRealPointer(r_seg,r_off)      _PM_imports.PM_mapRealPointer(r_seg,r_off)
#define PM_getVESABuf(len,rseg,roff)        _PM_imports.PM_getVESABuf(len,rseg,roff)
#define PM_callRealMode(seg,off,regs,sregs) _PM_imports.PM_callRealMode(seg,off,regs,sregs)
#define PM_int86(intno,in,out)              _PM_imports.PM_int86(intno,in,out)
#define PM_int86x(intno,in,out,sregs)       _PM_imports.PM_int86x(intno,in,out,sregs)
#endif

/* Obsolete functions */
/* {secret} */
int     PMAPI PM_getModeType(void);

#ifdef  __cplusplus
}                       /* End of "C" linkage for C++   */
#endif

/* Include OS extensions for interrupt handling */

#if defined(__REALDOS__) || defined(__SMX32__)
#include "pmint.h"
#endif

#endif /* __PMAPI_H */

