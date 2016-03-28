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
* Description:  Main module to implement I/O port access functions
*               on non-x86 platforms.
*
****************************************************************************/

#include "pmapi.h"
#include <asm/io.h>

#if defined(__PPC__)

#include <sys/syscall.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

extern volatile void           *_PM_ioBase ;

#define eieio() __asm__ __volatile__ ("eieio" ::: "memory")

static __inline__ uchar _PM_readMmio8(
    __volatile__ void *base,
    const ulong offset)
{
    register uchar val;
    __asm__ __volatile__(
        "lbzx %0,%1,%2\n\t"
        "eieio"
        : "=r" (val)
        : "b" (base), "r" (offset),
        "m" (*((volatile uchar *)base+offset)));
    return val;
}

static __inline__ ushort _PM_readMmio16Be(
    __volatile__ void *base,
    const ulong offset)
{
    register ushort val;
    __asm__ __volatile__(
        "lhzx %0,%1,%2\n\t"
        "eieio"
        : "=r" (val)
        : "b" (base), "r" (offset),
        "m" (*((volatile uchar *)base+offset)));
    return val;
}

static __inline__ ushort _PM_readMmio16Le(
    __volatile__ void *base,
    const ulong offset)
{
    register ushort val;
    __asm__ __volatile__(
        "lhbrx %0,%1,%2\n\t"
        "eieio"
        : "=r" (val)
        : "b" (base), "r" (offset),
        "m" (*((volatile uchar *)base+offset)));
    return val;
}

static __inline__ uint _PM_readMmio32Be(
    __volatile__ void *base,
    const ulong offset)
{
    register uint val;
    __asm__ __volatile__(
        "lwzx %0,%1,%2\n\t"
        "eieio"
        : "=r" (val)
        : "b" (base), "r" (offset),
        "m" (*((volatile uchar *)base+offset)));
    return val;
}

static __inline__ uint _PM_readMmio32Le(
    __volatile__ void *base,
    const ulong offset)
{
    register uint val;
    __asm__ __volatile__(
        "lwbrx %0,%1,%2\n\t"
        "eieio"
        : "=r" (val)
        : "b" (base), "r" (offset),
        "m" (*((volatile uchar *)base+offset)));
    return val;
}

static __inline__ void _PM_writeMmioNB8(
    __volatile__ void *base,
    const ulong offset,
        const uchar val)
{
    __asm__ __volatile__(
        "stbx %1,%2,%3\n\t"
        : "=m" (*((volatile uchar *)base+offset))
        : "r" (val), "b" (base), "r" (offset));
}

static __inline__ void _PM_writeMmioNB16Le(
    __volatile__ void *base,
    const ulong offset,
        const ushort val)
{
    __asm__ __volatile__(
        "sthbrx %1,%2,%3\n\t"
        : "=m" (*((volatile uchar *)base+offset))
        : "r" (val), "b" (base), "r" (offset));
}

static __inline__ void _PM_writeMmioNB16Be(
    __volatile__ void *base,
    const ulong offset,
        const ushort val)
{
    __asm__ __volatile__(
        "sthx %1,%2,%3\n\t"
        : "=m" (*((volatile uchar *)base+offset))
        : "r" (val), "b" (base), "r" (offset));
}

static __inline__ void _PM_writeMmioNB32Le(
    __volatile__ void *base,
    const ulong offset,
        const uint val)
{
    __asm__ __volatile__(
        "stwbrx %1,%2,%3\n\t"
        : "=m" (*((volatile uchar *)base+offset))
        : "r" (val), "b" (base), "r" (offset));
}

static __inline__ void _PM_writeMmioNB32Be(
    __volatile__ void *base,
    const ulong offset,
        const uint val)
{
    __asm__ __volatile__(
        "stwx %1,%2,%3\n\t"
        : "=m" (*((volatile uchar *)base+offset))
        : "r" (val), "b" (base), "r" (offset));
}

static __inline__ void _PM_writeMmio8(
    __volatile__ void *base,
    const ulong offset,
    const uchar val)
{
    _PM_writeMmioNB8(base, offset, val);
    eieio();
}

static __inline__ void _PM_writeMmio16Le(
    __volatile__ void *base,
    const ulong offset,
    const ushort val)
{
    _PM_writeMmioNB16Le(base, offset, val);
    eieio();
}

static __inline__ void _PM_writeMmio16Be(
    __volatile__ void *base,
    const ulong offset,
    const ushort val)
{
    _PM_writeMmioNB16Be(base, offset, val);
    eieio();
}

static __inline__ void _PM_writeMmio32Le(
    __volatile__ void *base,
    const ulong offset,
    const uint val)
{
    _PM_writeMmioNB32Le(base, offset, val);
    eieio();
}

static __inline__ void _PM_writeMmio32Be(
    __volatile__ void *base,
    const ulong offset,
    const uint val)
{
    _PM_writeMmioNB32Be(base, offset, val);
    eieio();
}

/* TODO: We probably need macros to handle this for non-x86 platforms! These
 *       macros should end up in the SNAP header files though, not in the PM
 *       library since the PM library does not deal with MMIO support.
#   define mem_barrier()        eieio()
#   define write_mem_barrier()  eieio()
 */

#elif defined(__X86_64__)

// Inline assembler functions for GCC

static inline uchar _inpb(u32 port)
{
    uchar _v;
    __asm__ __volatile__ ("inb %w1,%0" : "=a" (_v) : "Nd" (port) );
    return _v;
}

static inline u16 _inpw(u32 port)
{
    u16 _v;
    __asm__ __volatile__ ("inw %w1,%0" : "=a" (_v) : "Nd" (port) );
    return _v;
}

static inline u32 _inpd(u32 port)
{
    u32 _v;
    __asm__ __volatile__ ("inl %w1,%0" : "=a" (_v) : "Nd" (port) );
    return _v;
}

static inline void _outpb(u32 port, uchar value) 
{
    __asm__ __volatile__ ("outb %b0,%w1" : : "a" (value), "Nd" (port));
}

static inline void _outpw(u32 port, u16 value)
{
    __asm__ __volatile__ ("outw %w0,%w1" : : "a" (value), "Nd" (port));
} 

static inline void _outpd(u32 port, u32 value)
{
    __asm__ __volatile__ ("outl %0,%w1" : : "a" (value), "Nd" (port));
}
 
#endif

void PMAPI PM_outpb(int port,uchar val)
{
#ifdef __PPC__
    _PM_writeMmio8((void *)_PM_ioBase, port, val);
#else
    _outpb(port, val);
#endif
}

void PMAPI PM_outpw(int port,ushort val)
{
#ifdef __PPC__
    _PM_writeMmio16Le((void *)_PM_ioBase, port, val);
#else
    _outpw(port, val);
#endif
}

void PMAPI PM_outpd(int port,u32 val)
{
#ifdef __PPC__
    _PM_writeMmio32Le((void *)_PM_ioBase, port, val);
#else
    _outpd(port, val);
#endif
}

uchar PMAPI PM_inpb(int port)
{
#ifdef __PPC__
    return _PM_readMmio8((void *)_PM_ioBase, port);
#else
    return _inpb(port);
#endif
}

ushort PMAPI PM_inpw(int port)
{
#ifdef __PPC__
    return _PM_readMmio16Le((void *)_PM_ioBase, port);
#else
    return _inpw(port);
#endif
}

u32 PMAPI PM_inpd(int port)
{
#ifdef __PPC__
    return _PM_readMmio32Le((void *)_PM_ioBase, port);
#else
    return _inpd(port);
#endif
}
