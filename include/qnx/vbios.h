

#ifndef __VBIOS_H_INCLUDED
#define __VBIOS_H_INCLUDED

#ifndef __TYPES_H_INCLUDED
#include <sys/types.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include <qnx/_pack1.h>

#ifndef VB_EXT
#define VB_EXT  extern
#define VB_INIT(x)
#else
#define VB_INIT(x)  = { x }
#endif

#define VBIOS_SizeByte      0x1000
#define VBIOS_SizeByteS     0x3000
#define VBIOS_SizeWord      0x2000
#define VBIOS_SizeLong      0x4000
#define VBIOS_SizeMask      0xF000u

#define VBIOS_WriteValue    0x10000ul

typedef struct {
    unsigned short      f386;
    unsigned short      ds, ss, cs, es;
    unsigned short      fs, gs;
    unsigned long       eip;
    unsigned long       eflags;
    unsigned long       edi, esi, ebp, esp;
    unsigned long       ebx, edx, ecx, eax;
} __VBIOSlregs_t;

typedef struct {
    unsigned short      f386;
    unsigned short      ds, ss, cs, es;
    unsigned short      fs, gs;
    unsigned short      ip, __wTopIP;
    unsigned short      flags,  __wTopFlags;        
    unsigned short      di,     __wTopDI;
    unsigned short      si,     __wTopSI;
    unsigned short      bp,     __wTopBP;
    unsigned short      sp,     __wTopSP;
    unsigned short      bx,     __wTopBX;
    unsigned short      dx,     __wTopDX;
    unsigned short      cx,     __wTopCX;
    unsigned short      ax,     __wTopAX;
} __VBIOSwregs_t;

typedef struct {
    unsigned short      f386;
    unsigned short      ds, ss, cs, es;
    unsigned short      fs, gs;
    unsigned short      ip, __bTopIP;
    unsigned char       flagl, flagh;
    unsigned short      __bTopFlags;        
    unsigned long       edi, esi, ebp, esp;
    unsigned char       bl, bh;     
    unsigned short      __bTopBX;
    unsigned char       dl, dh;
    unsigned short      __bTopDX;
    unsigned char       cl, ch;
    unsigned short      __bTopCX;
    unsigned char       al, ah;
    unsigned short      __bTopAX;
} __VBIOSbregs_t;

typedef union {
    __VBIOSlregs_t  l;
    __VBIOSwregs_t  w;
    __VBIOSbregs_t  b;
} VBIOSregs_t;

VB_EXT int VBIOSverbose;
extern int _VBVER;
extern int _VBREV;

#define VBIOSforce_emu86    1
#define VBIOSforce_vm86     2
#define VBIOSforce_qnx4_v86 4
VBIOSregs_t *VBIOSinit( unsigned ForceType );
VBIOSregs_t *VBIOSinit_vm86( unsigned ForceType );
VBIOSregs_t *VBIOSinit_emu86( ulong_t, ulong_t );

int VBIOSint( unsigned inum, VBIOSregs_t *regs, int dsize );
ulong_t VBIOSintABCD( unsigned inum, ulong_t A, ulong_t B, ulong_t C, ulong_t D, VBIOSregs_t * );
int VBIOScall( unsigned CS, unsigned IP, VBIOSregs_t *regs, int dsize );
int VBIOSsetmembank( int bnum, void *ptr, VBIOSregs_t * );
int VBIOSsetmemrange( int bnum, void *ptr, unsigned nbytes, VBIOSregs_t * );
void *VBIOSgetmembank( int bnum, VBIOSregs_t * );
void *VBIOSgetmemptr( unsigned seg, unsigned off, VBIOSregs_t * );
int VBIOSaddporttrap( unsigned port, int len, unsigned(*FAddr)(unsigned, unsigned, unsigned), VBIOSregs_t * );
int VBIOSaddcalltrap( unsigned vaddr, int (*FAddr)( unsigned, VBIOSregs_t * ), unsigned q, VBIOSregs_t * );

unsigned VBIOS_TransBufVSeg( VBIOSregs_t * );
unsigned VBIOS_TransBufVOff( VBIOSregs_t * );
uchar_t *VBIOS_TransBufPtr( VBIOSregs_t * );
unsigned VBIOS_TransBufSize( VBIOSregs_t * );

int VBIOSprintregs( VBIOSregs_t *regs );

void *PhysicalMMap( unsigned long addr, unsigned long size );


#include <qnx/_packpop.h>

#ifdef __cplusplus
};
#endif

#endif


