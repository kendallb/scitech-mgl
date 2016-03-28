/****************************************************************************
*
*                   SciTech OS Portability Manager Library
*
*  ========================================================================
*
*   Copyright (C) 1991-2006 SciTech Software, Inc. All rights reserved.
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
* Environment:  QNX
*
* Description:  Implementation for the OS Portability Manager Library, which
*               contains functions to implement OS specific services in a
*               generic, cross platform API. Porting the OS Portability
*               Manager library is the first step to porting any SciTech
*               products to a new platform.
*
****************************************************************************/

#include "pmapi.h"
#include "clib/os/os.h"
#include "mtrr.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <malloc.h>
#include <sys/mman.h>
#include "qnx/vbios.h"
#ifndef __QNXNTO__
#include <sys/seginfo.h>
#include <sys/console.h>
#include <conio.h>
#include <i86.h>
#else
#include <sys/neutrino.h>
#include <sys/dcmd_chr.h>
#endif

/*--------------------------- Global variables ----------------------------*/

static uint VESABuf_len = 1024;     /* Length of the VESABuf buffer     */
static void *VESABuf_ptr = NULL;    /* Near pointer to VESABuf          */
static uint VESABuf_rseg;           /* Real mode segment of VESABuf     */
static uint VESABuf_roff;           /* Real mode offset of VESABuf      */
static VBIOSregs_t  *VRegs = NULL;  /* Pointer to VBIOS registers       */
static int raw_count = 0;
static struct _console_ctrl *cc = NULL;
static int console_count = 0;
static int rmbuf_inuse = 0;

static void (PMAPIP fatalErrorCleanup)(void) = NULL;

/* Page sized block cache */

#define PAGES_PER_BLOCK     100
#define FREELIST_NEXT(p)    (*(void**)(p))
typedef struct pageblock {
    struct pageblock    *next;
    struct pageblock    *prev;
    void                *freeListStart;
    void                *freeList;
    void                *freeListEnd;
    int                 freeCount;
    } pageblock;
static pageblock    *pageBlocks = NULL;

/*----------------------------- Implementation ----------------------------*/

void PMAPI PM_init(void)
{
    char *force;

    if (VRegs == NULL) {
#ifdef  __QNXNTO__
        ThreadCtl(_NTO_TCTL_IO, 0); /* Get IO privilidge */
#endif
        force = getenv("VBIOS_METHOD");
        VRegs = VBIOSinit(force ? atoi(force) : 0);
        }
    MTRR_init();
}

ibool PMAPI PM_haveBIOSAccess(void)
{ return VRegs != NULL; }

long PMAPI PM_getOSType(void)
{ return _OS_QNX; }

char * PMAPI PM_getOSName(void)
{ return "QNX"; }

int PMAPI PM_getModeType(void)
{ return PM_386; }

void PMAPI PM_backslash(char *s)
{
    uint pos = strlen(s);
    if (s[pos-1] != '/') {
        s[pos] = '/';
        s[pos+1] = '\0';
        }
}

void PMAPI PM_setFatalErrorCleanup(
    void (PMAPIP cleanup)(void))
{
    fatalErrorCleanup = cleanup;
}

void PMAPI PM_fatalError(const char *msg)
{
    if (fatalErrorCleanup)
        fatalErrorCleanup();
    fprintf(stderr,"%s\n", msg);
    exit(1);
}

static void ExitVBEBuf(void)
{
    if (VESABuf_ptr)
        PM_freeRealSeg(VESABuf_ptr);
    VESABuf_ptr = 0;
}

void * PMAPI PM_getVESABuf(uint *len,uint *rseg,uint *roff)
{
    if (!VESABuf_ptr) {
        /* Allocate a global buffer for communicating with the VESA VBE */
        if ((VESABuf_ptr = PM_allocRealSeg(VESABuf_len, &VESABuf_rseg, &VESABuf_roff)) == NULL)
            return NULL;
#ifndef __PHOTON__            
        atexit(ExitVBEBuf);
#endif        
        }
    *len = VESABuf_len;
    *rseg = VESABuf_rseg;
    *roff = VESABuf_roff;
    return VESABuf_ptr;
}

static int term_raw(void)
{
    struct termios  termios_p;

    if (raw_count++ > 0)
        return 0;

    /* Go into "raw" input mode */
    if (tcgetattr(STDIN_FILENO, &termios_p))
        return -1;

    termios_p.c_cc[VMIN] =  1;
    termios_p.c_cc[VTIME] =  0;
    termios_p.c_lflag &= ~( ECHO|ICANON|ISIG|ECHOE|ECHOK|ECHONL);
    tcsetattr(STDIN_FILENO, TCSADRAIN, &termios_p);
    return 0;
}

static void term_restore(void)
{
    struct termios  termios_p;

    if (raw_count-- != 1)
        return;

    tcgetattr(STDIN_FILENO, &termios_p);
    termios_p.c_lflag |= (ECHO|ICANON|ISIG|ECHOE|ECHOK|ECHONL);
    termios_p.c_oflag |= (OPOST);
    tcsetattr(STDIN_FILENO, TCSADRAIN, &termios_p);
}

int PMAPI PM_kbhit(void)
{
    int blocking, c;

    if (term_raw() == -1)
        return 0;

    /* Go into non blocking mode */
    blocking = fcntl(STDIN_FILENO, F_GETFL) | O_NONBLOCK;
    fcntl(STDIN_FILENO, F_SETFL, blocking);
    c = getc(stdin);

    /* restore blocking mode */
    fcntl(STDIN_FILENO, F_SETFL, blocking & ~O_NONBLOCK);
    term_restore();
    if (c != EOF) {
        ungetc(c, stdin);
        return c;
        }
    clearerr(stdin);
    return 0;
}

int PMAPI PM_getch(void)
{
    int c;

    if (term_raw() == -1)
        return (0);
    c = getc(stdin);
#if defined(__QNX__) && !defined(__QNXNTO__)
    if (c == 0xA)
        c = 0x0D;
    else if (c == 0x7F)
        c = 0x08;
#endif
    term_restore();
    return c;
}

PM_HWND PMAPI PM_openConsole(
    PM_HWND hwndUser,
    int device,
    int xRes,
    int yRes,
    int bpp,
    ibool fullScreen)
{
#ifndef __QNXNTO__
    int fd;

    if (console_count++)
        return 0;
    if ((fd = open("/dev/con1", O_RDWR)) == -1)
        return -1;
    cc = console_open(fd, O_RDWR);
    close(fd);
    if (cc == NULL)
        return -1;
#endif
    return 1;
}

int PMAPI PM_getConsoleStateSize(void)
{
    return PM_getVGAStateSize() + sizeof(int) * 3;
}

void PMAPI PM_saveConsoleState(void *stateBuf,int console_id)
{
#ifdef __QNXNTO__
    int     fd;
    int     flags;

    if ((fd = open("/dev/con1", O_RDWR)) == -1)
        return;
    flags = _CONCTL_INVISIBLE_CHG | _CONCTL_INVISIBLE;
    devctl(fd, DCMD_CHR_SERCTL, &flags, sizeof flags, 0);
    close(fd);
#else
    uchar   *buf = &((uchar*)stateBuf)[PM_getVGAStateSize()];

    /* Save QNX 4 console state */
    console_read(cc, -1, 0, NULL, 0,
        (int *)buf+1, (int *)buf+2, NULL);
    *(int *)buf = console_ctrl(cc, -1,
        CONSOLE_NORESIZE | CONSOLE_NOSWITCH | CONSOLE_INVISIBLE,
        CONSOLE_NORESIZE | CONSOLE_NOSWITCH | CONSOLE_INVISIBLE);

    /* Save state of VGA registers */
    PM_saveVGAState(stateBuf);
#endif
}

void PMAPI PM_setSuspendAppCallback(int (_ASMAPIP saveState)(int flags))
{
    /* TODO: Implement support for console switching if possible */
}

void PMAPI PM_restoreConsoleState(const void *stateBuf,PM_HWND hwndConsole)
{
#ifdef __QNXNTO__
    int     fd;
    int     flags;

    if ((fd = open("/dev/con1", O_RDWR)) == -1)
        return;
    flags = _CONCTL_INVISIBLE_CHG;
    devctl(fd, DCMD_CHR_SERCTL, &flags, sizeof flags, 0);
    close(fd);
#else
    uchar   *buf = &((uchar*)stateBuf)[PM_getVGAStateSize()];

    /* Restore the state of the VGA compatible registers */
    PM_restoreVGAState(stateBuf);

    /* Restore QNX 4 console state */
    console_ctrl(cc, -1, *(int *)buf,
        CONSOLE_NORESIZE | CONSOLE_NOSWITCH | CONSOLE_INVISIBLE);
    console_write(cc, -1, 0, NULL, 0,
        (int *)buf+1, (int *)buf+2, NULL);
#endif
}

void PMAPI PM_closeConsole(PM_HWND hwndConsole)
{
#ifndef __QNXNTO__
    if (--console_count == 0) {
        console_close(cc);
        cc = NULL;
        }
#endif
}

void PM_setOSCursorLocation(int x,int y)
{
    if (!cc)
        return;
#ifndef __QNXNTO__
    console_write(cc, -1, 0, NULL, 0, &y, &x, NULL);
#endif
}

void PM_setOSScreenWidth(int width,int height)
{
}

ibool PMAPI PM_setRealTimeClockHandler(PM_intHandler ih, int frequency)
{
    // TODO: Implement this for QNX
    return false;
}

void PMAPI PM_setRealTimeClockFrequency(int frequency)
{
    // TODO: Implement this for QNX
}

/****************************************************************************
REMARKS:
Stops the real time clock from ticking. Note that when we are actually
using IRQ0 instead, this functions does nothing (unlike calling
PM_setRealTimeClockFrequency directly).
****************************************************************************/
void PMAPI PM_stopRealTimeClock(void)
{
    PM_setRealTimeClockFrequency(0);
}

/****************************************************************************
REMARKS:
Restarts the real time clock ticking. Note that when we are actually using
IRQ0 instead, this functions does nothing.
****************************************************************************/
void PMAPI PM_restartRealTimeClock(
    int frequency)
{
    PM_setRealTimeClockFrequency(frequency);
}

void PMAPI PM_restoreRealTimeClockHandler(void)
{
    // TODO: Implement this for QNX
}

char * PMAPI PM_getCurrentPath(
    char *path,
    int maxLen)
{
    return getcwd(path,maxLen);
}

char PMAPI PM_getBootDrive(void)
{ return '/'; }

const char * PMAPI PM_getSNAPPath(void)
{
    char *env = getenv("SNAP_PATH");
#ifdef __QNXNTO__
    return env ? env : "/opt/scitech/drivers";
#else   /* QNX 4 */
    return env ? env : "/qnx4/scitech/bin";
#endif
}

const char * PMAPI PM_getSNAPConfigPath(void)
{
    static char path[512];
    char        *env;
#ifdef __QNXNTO__
    char temp[64];
    gethostname(temp, sizeof (temp));
    temp[sizeof (temp) - 1] = '\0';     /* Paranoid */
    sprintf(path,"/etc/config/scitech/%s/config", temp);
#else
    sprintf(path,"/etc/config/scitech/%d/config", getnid());
#endif
    if ((env = getenv("SNAP_CONFIG_PATH")) != NULL)
        return env;
    else if ((env = getenv("SNAP_PATH")) != NULL) {
        strcpy(path,env);
        PM_backslash(path);
        strcat(path,"config");
        }
    return path;
}

const char * PMAPI PM_getUniqueID(void)
{
    return PM_getMachineName();
}

const char * PMAPI PM_getMachineName(void)
{
    static char buf[128];
#ifdef __QNXNTO__
    gethostname(buf, sizeof (buf));
#else
    sprintf(buf,"node%d", getnid());
#endif
    return buf;
}

void * PMAPI PM_getBIOSPointer(void)
{
    return PM_mapRealPointer(0, 0x400);
}

void * PMAPI PM_getA0000Pointer(void)
{
    static void *ptr = NULL;
    void *freeptr;
    unsigned offset, i, maplen;

    if (ptr != NULL)
        return ptr;

    /* Some trickery is required to get the linear address 64K aligned */
    for (i = 0; i < 5; i++) {
        ptr = PM_mapPhysicalAddr(0xA0000,0xFFFF,true);
        offset = 0x10000 - ((unsigned)ptr % 0x10000);
        if (!offset)
            break;
        munmap(ptr, 0x10000);
        maplen = 0x10000 + offset;
        freeptr = PM_mapPhysicalAddr(0xA0000-offset, maplen-1,true);
        ptr = (void *)(offset + (unsigned)freeptr);
        if (0x10000 - ((unsigned)ptr % 0x10000))
            break;
        munmap(freeptr, maplen);
        }
    if (i == 5) {
        printf("Could not get a 64K aligned linear address for A0000 region\n");
        exit(1);
        }
    return ptr;
}

void * PMAPI PM_mapPhysicalAddr(ulong base,ulong limit,ibool isCached)
{
    uchar_t *p;
    unsigned o;
    unsigned prot = PROT_READ|PROT_WRITE|(isCached?0:PROT_NOCACHE);
#ifdef __PAGESIZE
    int pagesize = __PAGESIZE;
#else
    int pagesize = 4096;
#endif
    int rounddown = base % pagesize;
#ifndef __QNXNTO__
    static int __VidFD = -1;
#endif

    if (rounddown) {
        if (base < rounddown)
            return NULL;
        base -= rounddown;
        limit += rounddown;
        }

#ifndef __QNXNTO__
    if (__VidFD < 0) {
        if ((__VidFD = shm_open( "Physical", O_RDWR, 0777 )) == -1) {
            perror( "Cannot open Physical memory" );
            exit(1);
            }
        }
    o = base & 0xFFF;
    limit = (limit + o + 0xFFF) & ~0xFFF;
    if ((int)(p = mmap( 0, limit, prot, MAP_SHARED,
            __VidFD, base )) == -1 ) {
        return NULL;
        }
    p += o;
#else
    if ((p = mmap(0, limit, prot, MAP_PHYS | MAP_SHARED,
            NOFD, base)) == MAP_FAILED) {
        return (void *)-1;
        }
#endif
    return (p + rounddown);
}

void PMAPI PM_freePhysicalAddr(void *ptr,ulong limit)
{
    munmap(ptr,limit+1);
}

ulong PMAPI PM_getPhysicalAddr(void *p)
{
#ifdef __QNXNTO__
    off_t buf;

    if (mem_offset(p,NOFD,1,&buf,0) == -1) {
        perror("PM_getPhysicalAddr");
        return 0xFFFFFFFFUL;
        }
    return buf;
#else
    /* TODO: not currently implemented on QNX4 */
    return 0xFFFFFFFFUL;
#endif
}

ibool PMAPI PM_getPhysicalAddrRange(
    void *p,
    ulong length,
    ulong *physAddress)
{
    // TODO: Implement this!
    return false;
}

void PMAPI PM_sleep(ulong milliseconds)
{
    // TODO: Put the process to sleep for milliseconds
}

int PMAPI PM_getCOMPort(int port)
{
    // TODO: Re-code this to determine real values using the Plug and Play
    //       manager for the OS.
    switch (port) {
        case 0: return 0x3F8;
        case 1: return 0x2F8;
        }
    return 0;
}

int PMAPI PM_getLPTPort(int port)
{
    // TODO: Re-code this to determine real values using the Plug and Play
    //       manager for the OS.
    switch (port) {
        case 0: return 0x3BC;
        case 1: return 0x378;
        case 2: return 0x278;
        }
    return 0;
}

void * PMAPI PM_mallocShared(long size)
{
    return PM_malloc(size);
}

void PMAPI PM_freeShared(void *ptr)
{
    PM_free(ptr);
}

void * PMAPI PM_mapRealPointer(uint r_seg,uint r_off)
{
    void *p;

    PM_init();

    if ((p = VBIOSgetmemptr(r_seg, r_off, VRegs)) == (void *)-1)
        return NULL;
    return p;
}

void * PMAPI PM_allocRealSeg(uint size,uint *r_seg,uint *r_off)
{
    if (size > 1024) {
        printf("PM_allocRealSeg: can't handle %d bytes\n", size);
        return 0;
        }
    if (rmbuf_inuse != 0) {
        printf("PM_allocRealSeg: transfer area already in use\n");
        return 0;
        }
    PM_init();
    rmbuf_inuse = 1;
    *r_seg = VBIOS_TransBufVSeg(VRegs);
    *r_off = VBIOS_TransBufVOff(VRegs);
    return (void*)VBIOS_TransBufPtr(VRegs);
}

void PMAPI PM_freeRealSeg(void *mem)
{
    if (rmbuf_inuse == 0) {
        printf("PM_freeRealSeg: nothing was allocated\n");
        return;
        }
    rmbuf_inuse = 0;
}

int PMAPI PM_int86(int intno, RMREGS *in, RMREGS *out)
{
    PM_init();
    if (VRegs == NULL)
        return 0;

    VRegs->l.eax = in->e.eax;
    VRegs->l.ebx = in->e.ebx;
    VRegs->l.ecx = in->e.ecx;
    VRegs->l.edx = in->e.edx;
    VRegs->l.esi = in->e.esi;
    VRegs->l.edi = in->e.edi;

    VBIOSint(intno, VRegs, 1024);

    out->e.eax = VRegs->l.eax;
    out->e.ebx = VRegs->l.ebx;
    out->e.ecx = VRegs->l.ecx;
    out->e.edx = VRegs->l.edx;
    out->e.esi = VRegs->l.esi;
    out->e.edi = VRegs->l.edi;
    out->x.cflag = VRegs->w.flags & 0x1;

    return out->x.ax;
}

int PMAPI PM_int86x(int intno, RMREGS *in, RMREGS *out,
    RMSREGS *sregs)
{
    PM_init();
    if (VRegs == NULL)
        return 0;

    if (intno == 0x21) {
        time_t today = time(NULL);
        struct tm *t;
        t = localtime(&today);
        out->x.cx = t->tm_year + 1900;
        out->h.dh = t->tm_mon + 1;
        out->h.dl = t->tm_mday;
        return 0;
        }
    else {
        VRegs->l.eax = in->e.eax;
        VRegs->l.ebx = in->e.ebx;
        VRegs->l.ecx = in->e.ecx;
        VRegs->l.edx = in->e.edx;
        VRegs->l.esi = in->e.esi;
        VRegs->l.edi = in->e.edi;
        VRegs->w.es = sregs->es;
        VRegs->w.ds = sregs->ds;

        VBIOSint(intno, VRegs, 1024);

        out->e.eax = VRegs->l.eax;
        out->e.ebx = VRegs->l.ebx;
        out->e.ecx = VRegs->l.ecx;
        out->e.edx = VRegs->l.edx;
        out->e.esi = VRegs->l.esi;
        out->e.edi = VRegs->l.edi;
        out->x.cflag = VRegs->w.flags & 0x1;
        sregs->es = VRegs->w.es;
        sregs->ds = VRegs->w.ds;

        return out->x.ax;
        }
}

void PMAPI PM_callRealMode(uint seg,uint off, RMREGS *in,
    RMSREGS *sregs)
{
    PM_init();
    if (VRegs == NULL)
        return;

    VRegs->l.eax = in->e.eax;
    VRegs->l.ebx = in->e.ebx;
    VRegs->l.ecx = in->e.ecx;
    VRegs->l.edx = in->e.edx;
    VRegs->l.esi = in->e.esi;
    VRegs->l.edi = in->e.edi;
    VRegs->w.es = sregs->es;
    VRegs->w.ds = sregs->ds;

    VBIOScall(seg, off, VRegs, 1024);

    in->e.eax = VRegs->l.eax;
    in->e.ebx = VRegs->l.ebx;
    in->e.ecx = VRegs->l.ecx;
    in->e.edx = VRegs->l.edx;
    in->e.esi = VRegs->l.esi;
    in->e.edi = VRegs->l.edi;
    in->x.cflag = VRegs->w.flags & 0x1;
    sregs->es = VRegs->w.es;
    sregs->ds = VRegs->w.ds;
}

void * PMAPI PM_allocLockedMem(
    uint size,
    ulong *physAddr,
    ibool contiguous,
    ibool below16M)
{
    void*   linear;

    /* Allocate a physically contiguous buffer */
#ifdef  __QNXNTO__
    if ((linear = mmap(NULL,size,PROT_READ|PROT_WRITE,MAP_PHYS|MAP_ANON,NOFD,0)) == MAP_FAILED) {
#else
    if ((linear = mmap(NULL,size,PROT_READ|PROT_WRITE,MAP_ANON,NOFD,0)) == -1) {
#endif
        perror("PM_allocLockedMem mmap");
        return NULL;
        }
    *physAddr = PM_getPhysicalAddr(linear);
    return linear;
}

void PMAPI PM_freeLockedMem(
    void *p,
    uint size,
    ibool contiguous)
{
    if (munmap(p,size) == -1)
        perror("PM_freeLockedMem munmap");
}

/****************************************************************************
REMARKS:
Allocates a new block of pages for the page block manager.
****************************************************************************/
static pageblock *PM_addNewPageBlock(void)
{
    int         i,size;
    pageblock   *newBlock;
    char        *p,*next;

    /* Allocate memory for the new page block, and add to head of list */
    size = PAGES_PER_BLOCK * PM_PAGE_SIZE + (PM_PAGE_SIZE-1) + sizeof(pageblock);
    if ((newBlock = PM_malloc(size)) == NULL)
        return NULL;
    newBlock->prev = NULL;
    newBlock->next = pageBlocks;
    if (pageBlocks)
        pageBlocks->prev = newBlock;
    pageBlocks = newBlock;

    /* Initialise the page aligned free list for the page block */
    newBlock->freeCount = PAGES_PER_BLOCK;
    newBlock->freeList = p = (char*)(((ulong)(newBlock + 1) + (PM_PAGE_SIZE-1)) & ~(PM_PAGE_SIZE-1));
    newBlock->freeListStart = newBlock->freeList;
    newBlock->freeListEnd = p + (PAGES_PER_BLOCK-1) * PM_PAGE_SIZE;
    for (i = 0; i < PAGES_PER_BLOCK; i++,p = next)
        FREELIST_NEXT(p) = next = p + PM_PAGE_SIZE;
    FREELIST_NEXT(p - PM_PAGE_SIZE) = NULL;
    return newBlock;
}

/****************************************************************************
REMARKS:
Allocates a page aligned and page sized block of memory
****************************************************************************/
void * PMAPI PM_allocPage(
    ibool locked)
{
    pageblock       *block;
    void            *p;
    PM_lockHandle   lh;

    /* Scan the block list looking for any free blocks. Allocate a new
     * page block if no free blocks are found. */
    for (block = pageBlocks; block != NULL; block = block->next) {
        if (block->freeCount)
            break;
        }
    if (block == NULL && (block = PM_addNewPageBlock()) == NULL)
        return NULL;
    block->freeCount--;
    p = block->freeList;
    block->freeList = FREELIST_NEXT(p);
    if (locked)
        PM_lockDataPages(p,PM_PAGE_SIZE,&lh);
    return p;
}

/****************************************************************************
REMARKS:
Free a page aligned and page sized block of memory
****************************************************************************/
void PMAPI PM_freePage(
    void *p)
{
    pageblock   *block;

    /* First find the page block that this page belongs to */
    for (block = pageBlocks; block != NULL; block = block->next) {
        if (p >= block->freeListStart && p <= block->freeListEnd)
            break;
        }
    CHECK(block != NULL);

    /* Now free the block by adding it to the free list */
    FREELIST_NEXT(p) = block->freeList;
    block->freeList = p;
    if (++block->freeCount == PAGES_PER_BLOCK) {
        /* If all pages in the page block are now free, free the entire
         * page block itself.
         */
        if (block == pageBlocks) {
            /* Delete from head */
            pageBlocks = block->next;
            if (block->next)
                block->next->prev = NULL;
            }
        else {
            /* Delete from middle of list */
            CHECK(block->prev != NULL);
            block->prev->next = block->next;
            if (block->next)
                block->next->prev = block->prev;
            }
        PM_free(block);
        }
}

int PMAPI PM_lockDataPages(void *p,uint len,PM_lockHandle *lh)
{
    p = p;  len = len;
    return 1;
}

int PMAPI PM_unlockDataPages(void *p,uint len,PM_lockHandle *lh)
{
    p = p;  len = len;
    return 1;
}

int PMAPI PM_lockCodePages(void (*p)(),uint len,PM_lockHandle *lh)
{
    p = p;  len = len;
    return 1;
}

int PMAPI PM_unlockCodePages(void (*p)(),uint len,PM_lockHandle *lh)
{
    p = p;  len = len;
    return 1;
}

PM_MODULE PMAPI PM_loadLibrary(
    const char *szDLLName)
{
    // TODO: Implement this to load shared libraries!
    (void)szDLLName;
    return NULL;
}

void * PMAPI PM_getProcAddress(
    PM_MODULE hModule,
    const char *szProcName)
{
    // TODO: Implement this!
    (void)hModule;
    (void)szProcName;
    return NULL;
}

void PMAPI PM_freeLibrary(
    PM_MODULE hModule)
{
    // TODO: Implement this!
    (void)hModule;
}

int PMAPI PM_setIOPL(
    int level)
{
    // QNX handles IOPL selection at the program link level.
    return level;
}

/****************************************************************************
PARAMETERS:
base    - The starting physical base address of the region
size    - The size in bytes of the region
type    - Type to place into the MTRR register

RETURNS:
Error code describing the result.

REMARKS:
Function to enable write combining for the specified region of memory.
****************************************************************************/
int PMAPI PM_enableWriteCombine(
    ulong base,
    ulong size,
    uint type)
{
    return MTRR_enableWriteCombine(base,size,type);
}

/****************************************************************************
REMARKS:
Function to enumerate all write combine regions currently enabled for the
processor.
****************************************************************************/
int PMAPI PM_enumWriteCombine(
    PM_enumWriteCombine_t callback)
{
    return MTRR_enumWriteCombine(callback);
}

/****************************************************************************
REMARKS:
Do nothing for this OS.
****************************************************************************/
ulong PMAPI PM_setMaxThreadPriority(void)
{
    // TODO: Implement this on QNX!
    return 0;
}

/****************************************************************************
REMARKS:
Do nothing for this OS.
****************************************************************************/
void PMAPI PM_restoreThreadPriority(
    ulong oldPriority)
{
    (void)oldPriority;
}

/****************************************************************************
REMARKS:
Returns true if SDD is the active display driver in the system.
****************************************************************************/
ibool PMAPI PM_isSDDActive(void)
{
    return false;
}

/****************************************************************************
REMARKS:
This is not relevant to this OS.
****************************************************************************/
ibool PMAPI PM_runningInAWindow(void)
{
    return false;
}

/****************************************************************************
REMARKS:
Function get the OS resolution from the system. If we return false, it means
we don't know how to detect the OS resolution.
****************************************************************************/
ibool PMAPI PM_getOSResolution(
    int *width,
    int *height,
    int *bitsPerPixel)
{
    return false;
}

/****************************************************************************
REMARKS:
Function to set access protection flags on a block of memory.
****************************************************************************/
ibool PMAPI PM_memProtect(void *addr, size_t len, int flags)
{
    return true;
}

/****************************************************************************
PARAMETERS:
subsystem   - Subsystem to request access to
write       - True to enable a write lock, false for a read lock

REMARKS:
This function locks access to the internal SNAP device driver code. Locks can
either be read or write. If a lock is a read lock, attempts by other
processes to obtain a read lock will succeed, and write attempts will
block until the lock is unlocked. If locked as a write lock, all attempts
to lock by other processes will block until the lock is unlocked.

If the subsystem parameter is -1, a global lock will be taken for all
subsystems at the same time. The current subsystems supported are:

    0   - SNAP Graphics subsystem

****************************************************************************/
void PMAPI PM_lockSNAPAccess(
    int subsystem,
    ibool writeLock)
{
    /* Nothing to do on QNX */
    (void)subsystem;
    (void)writeLock;
}

/****************************************************************************
REMARKS:
This function arbitrates access to the internal SNAP device driver code
for external applications and utilities, and is used to release mutually
exclusive access to the hardware.
****************************************************************************/
void PMAPI PM_unlockSNAPAccess(
    int subsystem)
{
    /* Nothing to do on QNX */
    (void)subsystem;
}

/****************************************************************************
REMARKS:
This function cleans up any outstanding PM library allocations. Used for 
Photon graphics driver shared library to avoid async atexit() callbacks.
****************************************************************************/
void PMAPI PM_exit(void)
{
    ExitVBEBuf();
}


