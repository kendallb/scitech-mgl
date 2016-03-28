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
*                   Portions copyright (C) Josh Vanderhoof
*
* Language:     ANSI C
* Environment:  Linux
*
* Description:  Implementation for the OS Portability Manager Library, which
*               contains functions to implement OS specific services in a
*               generic, cross platform API. Porting the OS Portability
*               Manager library is the first step to porting any SciTech
*               products to a new platform.
*
****************************************************************************/

#include "pmapi.h"
#include "pm_help.h"
#include "clib/os/os.h"
#include "oshdr.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/kd.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/vt.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/ipc.h>
#include <sys/user.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/utsname.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <syscall.h>
#include <signal.h>
#include <time.h>
#include <ctype.h>
#include <errno.h>
#include <linux/fb.h>
#include <linux/major.h>
#ifndef NO_PTHREAD
#include <pthread.h>
#endif
#if defined(__INTEL__) || defined(__X86_64__)
#include <sys/io.h>
#include <asm/types.h>
#ifdef ENABLE_MTRR
#include <asm/mtrr.h>
#endif
#else
#undef ENABLE_MTRR
#endif
#ifdef __INTEL__
#include <asm/vm86.h>
#endif
#ifdef __GLIBC__
#ifdef __PPC__
#include "asm/page.h"
#elif defined(__ALPHA__)
#elif defined(__MIPS__)
#else
#include <sys/perm.h>
#endif
#endif

/*--------------------------- Global variables ----------------------------*/

#define REAL_MEM_BASE               ((void *)0x10000)
#define REAL_MEM_SIZE               0x10000
#define REAL_MEM_BLOCKS             0x100
#define DEFAULT_VM86_FLAGS          (IF_MASK | IOPL_MASK)
#define DEFAULT_STACK_SIZE          0x1000
#define RETURN_TO_32_INT            255
#define DEFAULT_FRAMEBUFFER         "/dev/fb0"
#define GRAPHICS_BPD                "graphics.bpd"
#define TMP_DIR                     "/tmp"
#define PM_SHM_CHAR                 'P'
#define PM_GAM_CHAR                 'G'
#define PM_AAM_CHAR                 'A'
#define MAX_HEART_BEAT_CALLBACKS    10
#define HEART_BEAT_CALLBACK_TIME    (32 * 1000)

#if defined(__GNU_LIBRARY__) && !defined(_SEM_SEMUN_UNDEFINED)
    /* union semun is defined by including <sys/sem.h> */
#else
    /* according to X/OPEN we have to define it ourselves */
    union semun {
        int val;                  /* value for SETVAL */
        struct semid_ds *buf;     /* buffer for IPC_STAT, IPC_SET */
        unsigned short *array;    /* array for GETALL, SETALL */
                                  /* Linux specific part: */
        struct seminfo *__buf;    /* buffer for IPC_INFO */
        };
#endif

#ifdef __INTEL__
/* Quick and dirty fix for vm86() syscall. Note that we *must* save/restore
 * the GS register across this call, as the kernel call trashes this register
 * yet the Linux ;s library requires this register to keep track of
 * thread local storage!
 */
static int
vm86(struct vm86_struct *vm)
    {
    int r;
#ifdef __PIC__
    asm volatile (
     "pushl %%gs\n\t"
     "pushl %%ebx\n\t"
     "movl %2, %%ebx\n\t"
     "int $0x80\n\t"
     "popl %%ebx\n\t"
     "popl %%gs"
     : "=a" (r)
     : "0" (113), "r" (vm));
#else
    asm volatile (
     "pushl %%gs\n\t"
     "int $0x80\n\t"
     "popl %%gs"
     : "=a" (r)
     : "0" (113), "b" (vm));
#endif
    return r;
    }
#endif

#ifdef __INTEL__
static struct {
    int                 ready;
    unsigned short      ret_seg, ret_off;
    unsigned short      stack_seg, stack_off;
    struct vm86_struct  vm;
    } context = {0};
#endif

#if defined(__INTEL__) || defined(__X86_64__)
struct mem_block {
    unsigned int size : 20;
    unsigned int free : 1;
    };

static struct {
    int ready;
    int count;
    struct mem_block blocks[REAL_MEM_BLOCKS];
    } mem_info = {0};
#endif

/* Structure used to save the keyboard mode to disk. We save it to disk
 * so that we can properly restore the mode later if the program crashed.
 */

typedef struct {
    struct termios  termio;
    int             kb_mode;
    int             leds;
    int             flags;
    } keyboard_mode;

int                     _PM_haveConsole = false;
int                     _PM_console_fd = -1;
int                     _PM_leds = 0,_PM_modifiers = 0;
int                     _PM_oldFD0;
int                     _PM_oldFD1;
int                     _PM_oldFD2;
static ibool            _PM_haveBIOSAccess = -1;
static ibool            inited = false;
static int              console_count = 0;
static int              startup_vc;
static ibool            in_raw_mode = false;
static keyboard_mode    kbd_mode;
/* static */ int              fd_pmhelp = -1;
static int              fd_fbdev = -1;
#if ENABLE_MTRR
static int              fd_mtrr = 0;
#endif
static uint VESABuf_len = 1024;     /* Length of the VESABuf buffer     */
static void *VESABuf_ptr = NULL;    /* Near pointer to VESABuf          */
static uint VESABuf_rseg;           /* Real mode segment of VESABuf     */
static uint VESABuf_roff;           /* Real mode offset of VESABuf      */
#ifdef TRACE_IO
static ulong            traceAddr;
#endif
#if !defined(__INTEL__) && !defined(__X86_64__)
volatile void           *_PM_ioBase = NULL;
void                    *_PM_memBaseISA = NULL;
ulong                   _PM_memBase_phys = 0;
#ifndef __NR_pciconfig_iobase
#define __NR_pciconfig_iobase   200
#endif
#ifndef IOBASE_IO
#define IOBASE_BRIDGE_NUMBER    0
#define IOBASE_MEMORY           1
#define IOBASE_IO               2
#define IOBASE_ISA_IO           3
#define IOBASE_ISA_MEM          4
#endif
#endif

static void (PMAPIP fatalErrorCleanup)(void) = NULL;

/* Define the first virtual memory address and the first physical memory
 * address that we start using for shared memory. We start with this
 * address and work our way upwards in memory from there.
 */

#ifdef __64BIT__
#define SHARED_MEM_START    0x1F00000000
#define PHYS_MEM_START      0x1F80000000
#else
#define SHARED_MEM_START    0x0050000000
#define PHYS_MEM_START      0x0060000000
#endif

/* Define the maximum number of memory mappings to support, so that
 * it fits into a single shared memory page.
 */

#define MAX_MEMORY_MAPPINGS ((PAGE_SIZE - sizeof(_shared_info)) / sizeof(mmapping))

/* Define the minimum size of each shared memory pool. When we get
 * requests to allocate shared memory blocks, if the size is less than
 * this size, we allocate a pool of this size and maintain a free list
 * for the remaining data in the heap. This can be overridden to minimise
 * the amount of virtual memory reserved for shared memory blocks. Note that
 * because Linux demand pages the shared memory blocks, until the memory is
 * actually accessed, it is never committed to the process address space, just
 * reserved. Hence we can use a rather large initial value.
 */

#ifndef PM_MIN_SHARED_POOL_SIZE
#define PM_MIN_SHARED_POOL_SIZE ((4096 * 1024) + PAGE_SIZE)
#endif

/* Maximum number of simultaneous shared memory read locks */

#define MAX_LOCK_COUNT  200

/* Structure used to track all physical memory mappings, so that
 * we can properly re-map those mappings to the same location in all
 * connecting processes.
 */

typedef struct {
    ulong       physical;
    void        *linear;
    ulong       length;
    ibool       isCached;
    int         refCount;
    } mmapping;

/* Structure for the memory block headers that are used to track
 * allocated memory in a shared memory pool.
 */
typedef struct mem_header {
    int                 size;
    struct mem_header   *next;
    } mem_header;

/* Structure for the memory pool header used to track shared
 * memory blocks within each memory pool.
 */
typedef struct mem_pool {
    int             size;
    int             id;
    struct mem_pool *next;
    mem_header      *free;
    } mem_pool;

/* Structure for all shared information not including mapped memory
 */
typedef struct {
    int         numMaps;
    ulong       sharedMemTop;
    ulong       physMemTop;
    int         memid;
    mem_pool    *mempool;
    pid_t       pid_ignore;
    mmapping    *newmap;
    int         newid;
    mem_pool    *newpool;
    void        *gaexports;
    } _shared_info;

/* Structure for the main shared memory block that references all
 * shared memory blocks in the system.
 */
typedef struct {
    mmapping        maps[MAX_MEMORY_MAPPINGS];
    _shared_info    s;
    } shared_info;

static shared_info              *sharedInfo = NULL;
static char                     sharedBaseKey[PM_MAX_PATH];
static int                      semaphoreId[3] = {-1,-1,-1};
static int                      sharedId = 0;
static int                      isWriteLock[3] = {0,0,0};
#ifndef NO_PTHREAD
static pthread_t                tid_mainThread;
static pthread_t                tid_sharedMemThread;
static pthread_t                tid_heartBeatThread;
#else
static char                     sharedMemThread_stack[THREAD_STACK_SIZE];
static char                     heartBeatThread_stack[THREAD_STACK_SIZE];
#endif
static pid_t                    pid_sharedMemThread = -1;
static pid_t                    pid_heartBeatThread = -1;
static int                      numHeartBeatCallbacks = 0;
static PM_heartBeat_cb          heartBeat[MAX_HEART_BEAT_CALLBACKS];
static void *                   heartBeatData[MAX_HEART_BEAT_CALLBACKS];

/* Structure for maintaining address associations between kernel virtual
 * addresses and user virtual addresses for kernel module physical addresses.
 */
typedef struct {
    void    *kernelAddr;
    void    *userAddr;
    } kernel_mmapping;

#define                 MAX_KMAPS   1024

static int              numKernelMaps = 0;
static kernel_mmapping  kmap[MAX_KMAPS] = {{0,0}};

/*----------------------------- Implementation ----------------------------*/

#ifdef  TRACE_IO
extern void printk(char *msg,...);
#endif

#ifdef __INTEL__

static inline void port_out(int value, int port)
{
#ifdef TRACE_IO
    printk("%04X:%04X: outb.%04X <- %02X\n", traceAddr >> 16, traceAddr & 0xFFFF, (ushort)port, (uchar)value);
#endif
    asm volatile ("outb %0,%1"
          ::"a" ((unsigned char) value), "d"((unsigned short) port));
}

static inline void port_outw(int value, int port)
{
#ifdef TRACE_IO
    printk("%04X:%04X: outw.%04X <- %04X\n", traceAddr >> 16,traceAddr & 0xFFFF, (ushort)port, (ushort)value);
#endif
    asm volatile ("outw %0,%1"
         ::"a" ((unsigned short) value), "d"((unsigned short) port));
}

static inline void port_outl(int value, int port)
{
#ifdef TRACE_IO
    printk("%04X:%04X: outl.%04X <- %08X\n", traceAddr >> 16,traceAddr & 0xFFFF, (ushort)port, (ulong)value);
#endif
    asm volatile ("outl %0,%1"
         ::"a" ((unsigned long) value), "d"((unsigned short) port));
}

static inline unsigned int port_in(int port)
{
    unsigned char value;
    asm volatile ("inb %1,%0"
              :"=a" ((unsigned char)value)
              :"d"((unsigned short) port));
#ifdef TRACE_IO
    printk("%04X:%04X:  inb.%04X -> %02X\n", traceAddr >> 16,traceAddr & 0xFFFF, (ushort)port, (uchar)value);
#endif
    return value;
}

static inline unsigned int port_inw(int port)
{
    unsigned short value;
    asm volatile ("inw %1,%0"
              :"=a" ((unsigned short)value)
              :"d"((unsigned short) port));
#ifdef TRACE_IO
    printk("%04X:%04X:  inw.%04X -> %04X\n", traceAddr >> 16,traceAddr & 0xFFFF, (ushort)port, (ushort)value);
#endif
    return value;
}

static inline unsigned int port_inl(int port)
{
    unsigned long value;
    asm volatile ("inl %1,%0"
              :"=a" ((unsigned long)value)
              :"d"((unsigned short) port));
#ifdef TRACE_IO
    printk("%04X:%04X:  inl.%04X -> %08X\n", traceAddr >> 16,traceAddr & 0xFFFF, (ushort)port, (ulong)value);
#endif
    return value;
}

static void insert_block(int i)
{
    memmove(
        mem_info.blocks + i + 1,
        mem_info.blocks + i,
        (mem_info.count - i) * sizeof(struct mem_block));
    mem_info.count++;
}

static void delete_block(int i)
{
    mem_info.count--;

    memmove(
        mem_info.blocks + i,
        mem_info.blocks + i + 1,
        (mem_info.count - i) * sizeof(struct mem_block));
}

static inline void pm_set_bit(unsigned int bit, void *array)
{
    unsigned char *a = array;
    a[bit / 8] |= (1 << (bit % 8));
}

static inline unsigned int get_int_seg(int i)
{
    return *(unsigned short *)(i * 4 + 2);
}

static inline unsigned int get_int_off(int i)
{
    return *(unsigned short *)(i * 4);
}

static inline void pushw(unsigned short i)
{
    struct vm86_regs *r = &context.vm.regs;
    r->esp -= 2;
    *(unsigned short *)(((unsigned int)r->ss << 4) + r->esp) = i;
}

#endif /* __INTEL__*/

#if defined(__INTEL__) || defined(__X86_64__)

static int real_mem_init(void)
{
    void    *m;
    int     fd_zero;

    if (mem_info.ready)
        return 1;

    if ((fd_zero = open("/dev/zero", O_RDONLY)) == -1)
        PM_fatalError("You must have root privileges to run this program!");
    if ((m = mmap((void *)REAL_MEM_BASE, REAL_MEM_SIZE,
            PROT_READ | PROT_WRITE | PROT_EXEC,
            MAP_FIXED | MAP_PRIVATE, fd_zero, 0)) == (void *)-1) {
        close(fd_zero);
        PM_fatalError("You must have root privileges to run this program!");
        }
    mem_info.ready = 1;
    mem_info.count = 1;
    mem_info.blocks[0].size = REAL_MEM_SIZE;
    mem_info.blocks[0].free = 1;
    return 1;
}

#endif

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
    1   - SNAP Audio subsystem

****************************************************************************/
void PMAPI PM_lockSNAPAccess(
    int subsystem,
    ibool writeLock)
{
    struct sembuf   sops[1];
    int             ret;
    int             n = subsystem + 1;

    if (n < 0 || n > 2 || semaphoreId[n] == -1)
        PM_fatalError("PM_lockSNAPAccess invalid!");
    sops[0].sem_num = 0;
    sops[0].sem_flg = SEM_UNDO;
    if (writeLock != 0)
        sops[0].sem_op = -MAX_LOCK_COUNT;
    else
        sops[0].sem_op = -1;
    do {
        ret = semop(semaphoreId[n],sops,1);
        if (ret == -1 && errno != EINTR)
            PM_fatalError("PM_lockSNAPAccess failed!");
        } while (ret == -1 && errno == EINTR);
    isWriteLock[n] = writeLock;
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
    struct sembuf   sops[1];
    int             ret;
    int             n = subsystem + 1;

    if (n < 0 || n > 2 || semaphoreId[n] == -1)
        PM_fatalError("PM_unlockSNAPAccess invalid!");
    sops[0].sem_num = 0;
    sops[0].sem_flg = SEM_UNDO;
    if (isWriteLock[n])
        sops[0].sem_op = MAX_LOCK_COUNT;
    else
        sops[0].sem_op = 1;
    do {
        ret = semop(semaphoreId[n],sops,1);
        } while (ret == -1 && errno == EINTR);
}

/****************************************************************************
REMARKS:
This function locks access to the shared memory subsystem. To do this we
grab write locks on all the SNAP subsystems as a single atomic operation,
which then means we have exclusive access to the entire shared memory
subsystem. Note that this lock is not recursive, so you must only call this
once before calling the _PM_unlockEventQueue() function.
****************************************************************************/
int _PM_lockEventQueue(void)
{
    struct sembuf   sops[1];
    int             ret;

    sops[0].sem_num = 3;            /* Event subsystem */
    sops[0].sem_flg = SEM_UNDO;
    sops[0].sem_op = -1;
    do {
        ret = semop(semaphoreId[0],sops,1);
        if (ret == -1 && errno != EINTR)
            PM_fatalError("_PM_lockEventQueue failed!");
        } while (ret == -1 && errno == EINTR);
    return 1;
}

/****************************************************************************
REMARKS:
This function arbitrates access to the internal SNAP device driver code
for external applications and utilities, and is used to release mutually
exclusive access to the hardware.
****************************************************************************/
void _PM_unlockEventQueue(void)
{
    struct sembuf   sops[1];
    int             ret;

    sops[0].sem_num = 3;            /* Event subsystem */
    sops[0].sem_flg = SEM_UNDO;
    sops[0].sem_op = 1;
    do {
        ret = semop(semaphoreId[0],sops,1);
        if (ret == -1 && errno != EINTR)
            PM_fatalError("_PM_unlockEventQueue failed!");
        } while (ret == -1 && errno == EINTR);
}

/****************************************************************************
REMARKS:
Function to have the thread/process block and wait for the specified
semaphore to become zero.
****************************************************************************/
static ibool _PM_waitForZero(
    int semNum)
{
    struct sembuf   sops[1];
    int             ret;

    sops[0].sem_num = semNum;
    sops[0].sem_flg = SEM_UNDO;
    sops[0].sem_op = 0;
    do {
        ret = semop(semaphoreId[0],sops,1);
        if (ret == -1 && errno != EINTR)
            return false;
        } while (ret == -1 && errno == EINTR);
    return true;
}

/****************************************************************************
REMARKS:
Function to have the signal to all threads blocked on the semaphore waiting
for zero to wake up. We do not exit until all blocked threads have woken up.
****************************************************************************/
static void _PM_signalZero(
    int semNum)
{
    struct sembuf   sops[1];
    union semun     se ;
    int             ret;

    /* Wake up processes waiting for zero on the semaphore */
    sops[0].sem_num = semNum;
    sops[0].sem_flg = 0;
    sops[0].sem_op = -1;
    do {
        ret = semop(semaphoreId[0],sops,1);
        if (ret == -1 && errno != EINTR)
            PM_fatalError("SignalZero failed!");
        } while (ret == -1 && errno == EINTR);

    /* Pause until all blocked processes have woken up */
    se.val = 0 ;
    do {
        usleep(1);
        ret = semctl(semaphoreId[0], semNum, GETZCNT, se) ;
        } while ((ret == -1 && errno == EINTR) || ret > 0);
    if (ret == -1)
        PM_fatalError ("semctl GETZCNT failed!") ;

    /* Now restore semaphore to a value of 1. Note that for this to work
     * the processes should all now be blocked on a different semaphore
     * until this code has executed.
     */
    sops[0].sem_num = semNum;
    sops[0].sem_flg = 0;
    sops[0].sem_op = 1;
    do {
        ret = semop(semaphoreId[0],sops,1);
        if (ret == -1 && errno != EINTR)
            PM_fatalError("SignalZero failed!");
        } while (ret == -1 && errno == EINTR);
}

/****************************************************************************
DESCRIPTION:
Map a physical address to a linear address in the callers process.

PARAMETERS:
start       - Starting linear address to map memory to
base        - Physical base address of the memory to map
length      - Length for the mapped memory region
isCached    - True if the memory should be cached, false if not

RETURNS:
Pointer to the mapped memory, false on failure.

REMARKS:
This is an internal function to map physical memory into the process address
space. If 'start' is not NULL, the function attempts to map the physical
memory starting at the specified address, otherwise the OS chooses the
location to map the memory at.
****************************************************************************/
static void * _PM_mapPhysicalAddr(
    void *start,
    ulong base,
    ulong length,
    ibool isCached)
{
    uchar       *p;
    ulong       baseAddr,baseOfs;
    int         fd_mem;

    /* Round the physical address to a 4Kb boundary and the limit to a
     * 4Kb-1 boundary before passing the values to mmap. If we round the
     * physical address, then we also add an extra offset into the address
     * that we return.
     */
    baseOfs = base & 4095;
    baseAddr = base & ~4095;
    length = ((length+baseOfs+4095) & ~4095);
    if ((fd_mem = open("/dev/mem", O_RDWR | (isCached ? 0 : O_SYNC))) == -1)
        return NULL;
    p = mmap(start, length, PROT_READ | PROT_WRITE,
        (start ? MAP_FIXED : 0) | MAP_SHARED,
        fd_mem, baseAddr);
    close(fd_mem);
    if (p == (void *)-1)
        return NULL;
    return (void*)(p+baseOfs);
}

/****************************************************************************
REMARKS:
Thread to handle shared memory mapping requests for the process
****************************************************************************/
#ifndef NO_PTHREAD
static void *_PM_sharedMemThread(
#else
static int _PM_sharedMemThread(
#endif
    void *arg)
{
    mmapping    *map;
    mem_pool    *pool;

#ifndef NO_PTHREAD
    pid_sharedMemThread = getpid();
#endif
    for (;;) {
        /* Block on semaphore #1, waiting until we need to process a
         * request.
         */
        if (!_PM_waitForZero(1))
            return 0;

        /* Now perform the specific request unless this thread should
         * ignore the request (ie: our process submitted the request).
         */
        if (getpid() != sharedInfo->s.pid_ignore) {
            if ((map = sharedInfo->s.newmap) != NULL) {
                /* Map the new memory mapping into the process */
                _PM_mapPhysicalAddr(map->linear,map->physical,map->length,map->isCached);
                }
            else if ((pool = sharedInfo->s.newpool) != NULL) {
                /* Map the new shared memory block into the process */
                if (shmat(sharedInfo->s.newid, (void*)pool, 0) != (void*)pool)
                    PM_fatalError("_PM_sharedMemThread: Unable to map shared memory!");
                PM_memProtect(pool, pool->size, PM_MPROT_READ | PM_MPROT_WRITE | PM_MPROT_EXEC);
                }
            }

        /* Block on semaphore #2 until all processes have completed the
         * request.
         */
        if (!_PM_waitForZero(2))
            return 0;
        }
    return 0;
}

/****************************************************************************
REMARKS:
Thread to handle shared memory mapping requests for the process
****************************************************************************/
#ifndef NO_PTHREAD
static void *_PM_heartBeatThread(
#else
static int _PM_heartBeatThread(
#endif
    void *arg)
{
    int i;

#ifndef NO_PTHREAD
    pid_heartBeatThread = getpid();
#endif
    /* Sleep for a while while everything starts up */
    usleep(2000000);
    for (;;) {
        /* Sleep until we need to process our list */
        usleep(HEART_BEAT_CALLBACK_TIME);

        /* Now lock the SNAP subsystem and process all the registers callbacks */
        PM_lockSNAPAccess(-1,true);
        for (i = 0; i < numHeartBeatCallbacks; i++)
            (*heartBeat[i])(heartBeatData[i]);
        PM_unlockSNAPAccess(-1);
        }
    return 0;
}

/****************************************************************************
REMARKS:
Called to signal the shared memory threads to process the specified command
packet.
****************************************************************************/
static void _PM_signalThreads(
    mmapping    *newmap,
    int         newid,
    mem_pool    *newpool)
{
    /* Setup the parameters for the request to be performed by the worker threads */
    sharedInfo->s.pid_ignore = pid_sharedMemThread;
    sharedInfo->s.newmap = newmap;
    sharedInfo->s.newid = newid;
    sharedInfo->s.newpool = newpool;

    /* Signal all threads to wake up and process the request */
    _PM_signalZero(1);

    /* Signal all threads that processing is now done and go back to sleep */
    _PM_signalZero(2);
}

/****************************************************************************
REMARKS:
Called upon process exit to clean up the shared memory subsystem.
****************************************************************************/
void PM_sharedMemoryCleanup(void)
{
    int             id;
    mem_pool        *pool,*nextpool;
    struct shmid_ds shm;

    /* Kill the shared memory thread first */
#if defined(NO_PTHREAD)
    kill(pid_sharedMemThread,SIGKILL);
    kill(pid_heartBeatThread,SIGKILL);
#endif
    usleep(100000);

    /* Now clean up the shared memory subsystem */
    PM_lockSNAPAccess(-1,true);
    shmctl(sharedId, IPC_STAT, &shm);
    if (shm.shm_nattch == 1) {
        union semun se;

        /* This is the last process, so clean up all shared memory blocks */
        id = sharedInfo->s.memid;
        for (pool = sharedInfo->s.mempool; pool; pool = nextpool) {
            shmctl(id, IPC_RMID, NULL);
            id = pool->id;
            nextpool = pool->next;
            shmdt(pool);
            }

        /* Detach and destroy the main shared memory block */
        shmctl(sharedId, IPC_RMID, NULL);
        shmdt(sharedInfo);

        /* This is the last process, so remove the semaphore set.
         * Buggy versions of C runtime crash without the fourth argument
         */
        se.val = 0;
        semctl(semaphoreId[0], 0, IPC_RMID, &se);
        semctl(semaphoreId[1], 0, IPC_RMID, &se);
        semctl(semaphoreId[2], 0, IPC_RMID, &se);
        /* No semaphores left to unlock */
        return;
        }
    else
        shmdt(sharedInfo);
    PM_unlockSNAPAccess(-1);
}

/****************************************************************************
REMARKS:
Initialised the shared memory subsystem. Note that we only initialsie the
shared memory subsystem if we find a copy of the SNAP graphics drivers on the
system. If we do not find the SNAP graphics drivers, then we do not initialise
the shared memory subsystem.
****************************************************************************/
static void PM_initSharedMem(void)
{
    int             i,id;
    key_t           key[3];
    mem_pool        *pool;
    mmapping        *maps;
#ifndef NO_PTHREAD
    pthread_attr_t  attr;
#endif

    /* Find the named shared memory block used by the SNAP daemon or any other
     * PM based processes running in the system, so that we can map the shared
     * memory blocks for all those processes in the system. If we don't have
     * a SNAP driver in the system, use a default key for the semaphores so
     * that programs can use the PM library without needing SNAP installed.
     */
    if (!PM_findBPD(GRAPHICS_BPD,sharedBaseKey))
        strcpy(sharedBaseKey,"/usr/lib/snap/default");

    /* Generate the key used for shared memory blocks and our semaphores */
    key[0] = ftok(sharedBaseKey, PM_SHM_CHAR);
    key[1] = ftok(sharedBaseKey, PM_GAM_CHAR);
    key[2] = ftok(sharedBaseKey, PM_AAM_CHAR);

    /* Allocate the set of semaphores that we need. First we try to connect
     * to the existing semaphore set, but if that fails we create the semaphore
     * set and initialise it.
     */
    for (i = 0; i < 3; i++) {
        if ((semaphoreId[i] = semget(key[i], 0, 0)) == -1) {
            ushort          values[4] = {MAX_LOCK_COUNT,1,1,1};
            union semun     se;

            if ((semaphoreId[i] = semget(key[i], 4, IPC_CREAT | IPC_EXCL | 0666)) == -1)
                PM_fatalError("Unable to create process semaphores!");
            se.array = values;
            if (semctl(semaphoreId[i], 0, SETALL, se) != 0)
                PM_fatalError("Unable to initialize process semaphores!");
            }
        }

    /* Get exclusive access to shared memory subsystem */
    PM_lockSNAPAccess(-1,true);

    /* Now allocate the shared memory */
    if ((sharedId = shmget(key[0], 0, 0)) != -1) {
        /* Another process has already started, so map all the shared memory
         * blocks and addresses created by the other process
         */
        if ((sharedInfo = shmat(sharedId, 0, 0)) == (void*)-1) {
            PM_unlockSNAPAccess(-1);
            PM_fatalError("Unable to map shared memory block!");
            }

        /* Map in all shared memory pools */
        id = sharedInfo->s.memid;
        for (pool = sharedInfo->s.mempool; pool; pool = pool->next) {
            if (shmat(id, pool, 0) != pool) {
                PM_unlockSNAPAccess(-1);
                PM_fatalError("Unable to map shared memory block!");
                }
            PM_memProtect(pool, pool->size, PM_MPROT_READ | PM_MPROT_WRITE | PM_MPROT_EXEC);
            id = pool->id;
            }

        /* Map in all existing physical memory mappings for this process */
        maps = sharedInfo->maps;
        for (i = 0; i < sharedInfo->s.numMaps; i++)
            _PM_mapPhysicalAddr(maps[i].linear,maps[i].physical,maps[i].length,maps[i].isCached);
        }
    else {
        /* This is the first process in the system, so create and initialise
         * the shared memory system.
         */
        if ((sharedId = shmget(key[0], sizeof(shared_info), IPC_CREAT | IPC_EXCL | 0666)) == -1) {
            PM_unlockSNAPAccess(-1);
            PM_fatalError("Unable to create shared memory block!");
            }
        if ((sharedInfo = shmat(sharedId, 0, 0)) == (void*)-1) {
            PM_unlockSNAPAccess(-1);
            PM_fatalError("Unable to map shared memory block!");
            }
        memset(sharedInfo,0,sizeof(*sharedInfo));
        sharedInfo->s.sharedMemTop = SHARED_MEM_START;
        sharedInfo->s.physMemTop = PHYS_MEM_START;
        }

    /* Release exclusive access to shared memory subsystem */
    PM_unlockSNAPAccess(-1);

    /* Create the shared memory thread used to map shared memory blocks allocated
     * in other processes into this process address space.
     */
#ifndef NO_PTHREAD
    pid_sharedMemThread = -1;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_create(&tid_sharedMemThread, &attr, _PM_sharedMemThread, NULL);
    while (pid_sharedMemThread == -1)
        sleep(1);
#else
    pid_sharedMemThread = clone(_PM_sharedMemThread, &sharedMemThread_stack[THREAD_STACK_SIZE], CLONE_FS | CLONE_FILES | CLONE_VM, NULL);
#endif

    /* Ensure the list of driver heart beat callbacks is empty */
    numHeartBeatCallbacks = 0;

    /* Register our cleanup function */
    atexit(PM_sharedMemoryCleanup);
}

/****************************************************************************
REMARKS:
Function to register a driver heart beat callback function. The first
function that is called sets the interval for all the callback functions
and they will be called in the order they were registered. This function
will implement this mechanism in whatever way is appropriate for the
device driver environment. For Linux since we are running entirely within
user space, this function simply starts a thread that goes to sleep for
a certain period of time and then wakes up to process the callbacks on
a regular basis.
****************************************************************************/
void PMAPI PM_registerHeartBeatCallback(
    PM_heartBeat_cb cb,
    void *data)
{
#ifndef NO_PTHREAD
    pthread_attr_t  attr;
#endif

    if (pid_heartBeatThread == -1) {
        /* This is the first callback, so start the heart beat thread */
#ifndef NO_PTHREAD
        pid_heartBeatThread = -1;
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
        pthread_create(&tid_heartBeatThread, &attr, _PM_heartBeatThread, NULL);
        while (pid_heartBeatThread == -1)
            sleep(1);
#else
        pid_heartBeatThread = clone(_PM_heartBeatThread, &heartBeatThread_stack[THREAD_STACK_SIZE], CLONE_FS | CLONE_FILES | CLONE_VM, NULL);
#endif
        }

    /* Now register this callback with the thread */
    PM_lockSNAPAccess(-1,true);
    if (numHeartBeatCallbacks < MAX_HEART_BEAT_CALLBACKS) {
        heartBeat[numHeartBeatCallbacks] = cb;
        heartBeatData[numHeartBeatCallbacks] = data;
        numHeartBeatCallbacks++;
        }
    PM_unlockSNAPAccess(-1);
}

/****************************************************************************
REMARKS:
Function to unregister a driver heart beat callback with the PM library.
****************************************************************************/
void PMAPI PM_unregisterHeartBeatCallback(
    PM_heartBeat_cb cb)
{
    int i;

    for (i = 0; i < numHeartBeatCallbacks; i++) {
        if (heartBeat[i] == cb) {
            PM_lockSNAPAccess(-1,true);
            if (i < numHeartBeatCallbacks-1) {
                memmove(&heartBeat[i],&heartBeat[i+1], sizeof(heartBeat[i]) * (numHeartBeatCallbacks - 1 - i));
                memmove(&heartBeatData[i],&heartBeatData[i+1],sizeof(heartBeatData[i]) * (numHeartBeatCallbacks - 1 - i));
                }
            numHeartBeatCallbacks--;
            PM_unlockSNAPAccess(-1);
            }
        }
}

/****************************************************************************
REMARKS:
This internal function copies the global SNAP Graphics driver exports into
the shared memory block.
****************************************************************************/
void PMAPI PM_setGASharedExports(
    void *gaexports,
    int size)
{
    sharedInfo->s.gaexports = PM_mallocShared(size);
    memcpy(sharedInfo->s.gaexports,gaexports,size);
}

/****************************************************************************
REMARKS:
Return the shared exports structure for the shared SNAP drivers that are
loaded. If SNAP was loaded via the daemon, then the SNAP drivers we
connect to are the ones that were loaded in the daemon.
****************************************************************************/
void * PMAPI PM_getGASharedExports(void)
{
    PM_init();
    return sharedInfo ? sharedInfo->s.gaexports : NULL;
}

/****************************************************************************
REMARKS:
Main entry point to initialise the PM library.
****************************************************************************/
void PMAPI PM_init(void)
{
    int     fd_mem;
#ifdef __INTEL__
    uint    r_seg,r_off;
#endif
#if defined(__INTEL__) || defined(__X86_64__)
    void    *m;
#else
    ulong   _PM_ioBase_phys;
    ulong   _PM_memBaseISA_phys;
    char    *p;
#endif

    if (inited)
        return;

    /* Save off the main thread handle */
#ifndef NO_PTHREAD
    tid_mainThread = pthread_self();
#endif

    /* Map the Interrupt Vectors (0x0 - 0x400) + BIOS data (0x400 - 0x502)
     * and the physical framebuffer and ROM images from (0xa0000 - 0x100000)
     */
#if defined(__INTEL__) || defined(__X86_64__)
    real_mem_init();
#endif
    if ((fd_mem = open("/dev/mem", O_RDWR)) == -1) {
        PM_fatalError("You must have root privileges to run this program!");
        }
#if defined(__INTEL__) || defined(__X86_64__)
    if ((m = mmap((void *)0, 0x502,
            PROT_READ | PROT_WRITE | PROT_EXEC,
            MAP_FIXED | MAP_PRIVATE, fd_mem, 0)) == (void *)-1) {
        PM_fatalError("You must have root privileges to run this program!");
        }
    if ((m = mmap((void *)0xA0000, 0xC0000 - 0xA0000,
            PROT_READ | PROT_WRITE,
            MAP_FIXED | MAP_SHARED, fd_mem, 0xA0000)) == (void *)-1) {
        PM_fatalError("You must have root privileges to run this program!");
        }
    if ((m = mmap((void *)0xC0000, 0xD0000 - 0xC0000,
            PROT_READ | PROT_WRITE | PROT_EXEC,
            MAP_FIXED | MAP_PRIVATE, fd_mem, 0xC0000)) == (void *)-1) {
        PM_fatalError("You must have root privileges to run this program!");
        }
    if ((m = mmap((void *)0xD0000, 0x100000 - 0xD0000,
            PROT_READ | PROT_WRITE,
            MAP_FIXED | MAP_SHARED, fd_mem, 0xD0000)) == (void *)-1) {
        PM_fatalError("You must have root privileges to run this program!");
        }
#endif
    inited = 1;

    /* Initialise shared memory manager */
    PM_initSharedMem();

#ifdef __INTEL__
    /* Allocate a stack */
    m = PM_allocRealSeg(DEFAULT_STACK_SIZE,&r_seg,&r_off);
    context.stack_seg = r_seg;
    context.stack_off = r_off+DEFAULT_STACK_SIZE;

    /* Allocate the return to 32 bit routine */
    m = PM_allocRealSeg(2,&r_seg,&r_off);
    context.ret_seg = r_seg;
    context.ret_off = r_off;
    ((uchar*)m)[0] = 0xCD;         /* int opcode */
    ((uchar*)m)[1] = RETURN_TO_32_INT;
    memset(&context.vm, 0, sizeof(context.vm));

    /* Enable kernel emulation of all ints except RETURN_TO_32_INT */
    memset(&context.vm.int_revectored, 0, sizeof(context.vm.int_revectored));
    pm_set_bit(RETURN_TO_32_INT, &context.vm.int_revectored);
    context.ready = 1;
#endif

    /* Open device I/O channel to PM kernel module */
    fd_pmhelp = open("/dev/pmhelp", O_RDWR);

#ifdef ENABLE_MTRR
    fd_mtrr =  open("/dev/cpu/mtrr", O_RDWR, 0);
    if (fd_mtrr < 0)
       fd_mtrr =  open("/proc/mtrr", O_RDWR, 0);
#endif

#ifdef __PPC__
    /* Enable I/O port access for PowerPC based systems. For this we simply
     * make a syscall to the kernel to find out the base address of the I/O
     * port map for accessing the hardware, and then map it in.
     */
    if ((_PM_ioBase_phys = syscall(__NR_pciconfig_iobase, IOBASE_IO, 0, 0)) == 0) {
        /* Didn't work, try the ISA I/O base next */
        if ((_PM_ioBase_phys = syscall(__NR_pciconfig_iobase, IOBASE_ISA_IO, 0, 0)) == 0) {
            if ((p = getenv("SNAP_ISA_IOBASE")) == NULL)
                PM_fatalError("Unable to map I/O space!");
            _PM_ioBase_phys = strtoul(p,NULL,16);
            }
        }
    if ((_PM_ioBase = mmap((void *)0, 0x20000,
            PROT_READ | PROT_WRITE,
            MAP_SHARED, fd_mem, _PM_ioBase_phys)) == (void *)-1) {
        PM_fatalError("You must have root privileges to run this program!");
        }

    /* Map ISA memory (used for access to VGA framebuffer). This is not
     * supported on all systems/kernels, hence failure to map the memory
     * must not be a fatal error. However we do allow the user to override
     * the value returned by the kernel for incorrectly built kernels with
     * an environment variable. Ideally the kernel should be fixed, but this
     * will get the code up and running in leiu of a properly written kernel.
     */
    _PM_memBaseISA_phys = syscall(__NR_pciconfig_iobase, IOBASE_ISA_MEM, 0, 0);
    if ((p = getenv("SNAP_ISA_MEMBASE")) != NULL)
        _PM_memBaseISA_phys = strtoul(p,NULL,16);
    if (_PM_memBaseISA_phys != 0) {
        /* Map the first megabyte of ISA memory. If we can't map this memory we
         * leave the pointer set to NULL so our code will avoid using ISA
         * memory.
         */
        if ((_PM_memBaseISA = mmap((void *)0, 0x100000,
                PROT_READ | PROT_WRITE,
                MAP_SHARED, fd_mem, _PM_memBaseISA_phys)) == (void *)-1) {
            _PM_memBaseISA = NULL;
            }
        }

    /* Find the PCI bus memory offset which is used for access to PCI device memory.
     * Many machines map PCI bus memory 1:1 with the CPU memory above 2G or so may
     * return this value as zero which is valid, but some machines will remap
     * it differently (ie: older PREP machines).
     */
    _PM_memBase_phys = syscall(__NR_pciconfig_iobase, IOBASE_MEMORY, 0, 0);
#elif defined __MIPS__
    if ((p = getenv("SNAP_ISA_IOBASE")) != NULL)
        _PM_ioBase_phys = strtoul(p,NULL,16);
    else
        _PM_ioBase_phys = -1;
    if ((_PM_ioBase = mmap((void *)0, 0x20000,
            PROT_READ | PROT_WRITE,
            MAP_SHARED, fd_mem, _PM_ioBase_phys)) == (void *)-1) {
        PM_fatalError("MMIO unavailable (you must have root privileges to run this program!)");
        }

    if ((p = getenv("SNAP_ISA_MEMBASE")) != NULL)
        _PM_memBaseISA_phys = strtoul(p,NULL,16);
    else
        _PM_memBaseISA_phys = 0;
    if (_PM_memBaseISA_phys != 0) {
        /* Map the first megabyte of ISA memory. If we can't map this memory we
         * leave the pointer set to NULL so our code will avoid using ISA
         * memory.
         */
        if ((_PM_memBaseISA = mmap((void *)0, 0x100000,
                PROT_READ | PROT_WRITE,
                MAP_SHARED, fd_mem, _PM_memBaseISA_phys)) == (void *)-1) {
            _PM_memBaseISA = NULL;
            }
        }
#else
    /* Enable I/O permissions to directly access I/O ports. We break the
     * allocation into two parts, one for the ports from 0-0x3FF and
     * another for the remaining ports up to 0xFFFF. Standard Linux kernels
     * only allow the first 0x400 ports to be enabled, so to enable all
     * 65536 ports you need a patched kernel that will enable the full
     * 8Kb I/O permissions bitmap.
     */
#ifndef TRACE_IO
    ioperm(0x0,0x400,1);
    ioperm(0x400,0x10000-0x400,1);
#endif
    iopl(3);
#endif
    close(fd_mem);

    /* Determine if we have BIOS access */
    _PM_haveBIOSAccess = PM_haveBIOSAccess();
}

ibool PMAPI PM_haveBIOSAccess(void)
{
#ifdef __INTEL__
    struct utsname  buf;

    if (_PM_haveBIOSAccess == -1) {
        /* Allow the BIOS emulator to be forced on via an environment
         * variable for testing purposes.
         */
        _PM_haveBIOSAccess = true;
        if (getenv("SNAP_FORCE_BIOSEMU") != NULL)
            _PM_haveBIOSAccess = false;

        /* If we are running on x86-64, disable BIOS access because
         * vm86() services are not supported when running in 64-bit
         * mode.
         */
        if (uname(&buf) == 0 && strcmp(buf.machine,"x86_64") == 0)
            _PM_haveBIOSAccess = false;
        }
    return _PM_haveBIOSAccess;
#else
    /* Non x86 processors never have native BIOS support */
    return false;
#endif
}

long PMAPI PM_getOSType(void)
{
    return _OS_LINUX;
}

char * PMAPI PM_getOSName(void)
{
    return "Linux";
}

int PMAPI PM_getModeType(void)
{ return PM_386; }

void PMAPI PM_backslash(char *s)
{
    uint pos = strlen(s);
    if (pos && (s[pos-1] != '/')) {
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
    void (PMAPIP _fatalErrorCleanup)(void) = fatalErrorCleanup ;

    if (_fatalErrorCleanup) {
        /*
         * Prevent infinite loops by setting this to NULL.
         * This commonly happens on PPC when cleanup code
         * attempts to set mode 0x100, which doesn't exist.
         */
        fatalErrorCleanup = NULL ;
        _fatalErrorCleanup();
    }
    fprintf(stderr,"%s\n", msg);
    fflush(stderr);
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
    if (__PM_biosFuncs.PM_getVESABuf)
        return __PM_biosFuncs.PM_getVESABuf(len,rseg,roff);
    if (!VESABuf_ptr) {
        /* Allocate a global buffer for communicating with the VESA VBE */
        if ((VESABuf_ptr = PM_allocRealSeg(VESABuf_len, &VESABuf_rseg, &VESABuf_roff)) == NULL)
            return NULL;
        atexit(ExitVBEBuf);
        }
    *len = VESABuf_len;
    *rseg = VESABuf_rseg;
    *roff = VESABuf_roff;
    return VESABuf_ptr;
}

/* New raw console based getch and kbhit functions */

#define KB_CAPS     LED_CAP /* 4 */
#define KB_NUMLOCK  LED_NUM /* 2 */
#define KB_SCROLL   LED_SCR /* 1 */
#define KB_SHIFT    8
#define KB_CONTROL  16
#define KB_ALT      32

/****************************************************************************
REMARKS:
Restore the keyboard to normal mode
****************************************************************************/
void _PM_restore_kb_mode(void)
{
    if (_PM_console_fd != -1 && in_raw_mode) {
        if (_PM_haveConsole) {
            ioctl(_PM_console_fd, KDSETLED, kbd_mode.leds);
            ioctl(_PM_console_fd, KDSKBMODE, kbd_mode.kb_mode);
            }
        tcsetattr(_PM_console_fd, TCSANOW, &kbd_mode.termio);
        fcntl(_PM_console_fd,F_SETFL,kbd_mode.flags);
        in_raw_mode = false;
        }
}

/****************************************************************************
REMARKS:
Safely abort the event module upon catching a fatal error.
****************************************************************************/
void _PM_abort(
    int signo)
{
    char    buf[80];

    /* Ignore any signals except for the primary thread */
    if (pthread_equal(tid_mainThread,pthread_self())) {
        sprintf(buf,"Terminating on signal %d",signo);
        _PM_restore_kb_mode();
        PM_fatalError(buf);
        }
}

/****************************************************************************
REMARKS:
Put the keyboard into raw mode
****************************************************************************/
void _PM_keyboard_rawmode(void)
{
    struct termios  termio;
    int             i;
    static int sig_list[] = {
        SIGHUP,
        SIGINT,
        SIGQUIT,
        SIGILL,
        SIGTRAP,
        SIGABRT,
        SIGIOT,
        SIGBUS,
        SIGKILL,
        SIGSEGV,
        SIGTERM,
        };

    /* Bail if we are already in raw mode */
    if (in_raw_mode)
        return;

    /* Attempt to put the console into raw mode */
    if (_PM_haveConsole) {
        ioctl(_PM_console_fd, KDGKBMODE, &kbd_mode.kb_mode);
        ioctl(_PM_console_fd, KDGETLED, &kbd_mode.leds);
        if (ioctl(_PM_console_fd, KDSKBMODE, K_MEDIUMRAW))
            perror("KDSKBMODE");
        _PM_leds = kbd_mode.leds & 0xF;
        _PM_modifiers = 0;
        }
    tcgetattr(_PM_console_fd, &kbd_mode.termio);
    termio = kbd_mode.termio;
    termio.c_iflag = (IGNPAR | IGNBRK) & (~PARMRK) & (~ISTRIP);
    termio.c_cflag = CREAD | CS8;
    termio.c_lflag = 0;
    termio.c_cc[VTIME] = 0;
    termio.c_cc[VMIN] = 1;
    tcsetattr(_PM_console_fd, TCSANOW, &termio);
    kbd_mode.flags = fcntl(_PM_console_fd,F_GETFL);
    fcntl(_PM_console_fd,F_SETFL,kbd_mode.flags & ~O_NONBLOCK);
    atexit(_PM_restore_kb_mode);
    for (i = 0; i < sizeof(sig_list)/sizeof(sig_list[0]); i++)
        signal(sig_list[i], _PM_abort);
    in_raw_mode = true;
}

int PMAPI PM_kbhit(void)
{
    fd_set          s;
    struct timeval  tv = { 0, 1 };

    if (console_count == 0)
        PM_fatalError("You *must* open a console before using PM_kbhit!");
    _PM_keyboard_rawmode();
    FD_ZERO(&s);
    FD_SET(_PM_console_fd, &s);
    return select(_PM_console_fd+1, &s, NULL, NULL, &tv) > 0;
}

int PMAPI PM_getch(void)
{
    static uchar            c;
    int                     release;
    static struct kbentry   ke;

    if (console_count == 0)
        PM_fatalError("You *must* open a console before using PM_getch!");
    _PM_keyboard_rawmode();
    if (_PM_haveConsole) {
        /* We have console support, so process the keyboard in raw mode */
        while (read(_PM_console_fd, &c, 1) > 0) {
            release = c & 0x80;
            c &= 0x7F;
            if (release) {
                switch(c){
                    case 42: case 54: /* Shift */
                        _PM_modifiers &= ~KB_SHIFT;
                        break;
                    case 29: case 97: /* Control */
                        _PM_modifiers &= ~KB_CONTROL;
                        break;
                    case 56: case 100: /* Alt / AltGr */
                        _PM_modifiers &= ~KB_ALT;
                        break;
                    }
                continue;
                }
            switch (c) {
                case 42: case 54: /* Shift */
                    _PM_modifiers |= KB_SHIFT;
                     break;
                case 29: case 97: /* Control */
                    _PM_modifiers |= KB_CONTROL;
                    break;
                case 56: case 100: /* Alt / AltGr */
                    _PM_modifiers |= KB_ALT;
                    break;
                case 58: /* Caps Lock */
                    _PM_modifiers ^= KB_CAPS;
                    ioctl(_PM_console_fd, KDSETLED, _PM_modifiers & 7);
                    break;
                case 69: /* Num Lock */
                    _PM_modifiers ^= KB_NUMLOCK;
                    ioctl(_PM_console_fd, KDSETLED, _PM_modifiers & 7);
                    break;
                case 70: /* Scroll Lock */
                    _PM_modifiers ^= KB_SCROLL;
                    ioctl(_PM_console_fd, KDSETLED, _PM_modifiers & 7);
                    break;
                case 28:
                    return 0x1C;
                default:
                    ke.kb_index = c;
                    ke.kb_table = 0;
                    if ((_PM_modifiers & KB_SHIFT) || (_PM_modifiers & KB_CAPS))
                        ke.kb_table |= K_SHIFTTAB;
                    if (_PM_modifiers & KB_ALT)
                        ke.kb_table |= K_ALTTAB;
                    ioctl(_PM_console_fd, KDGKBENT, (ulong)&ke);
                    c = ke.kb_value & 0xFF;
                    return c;
                }
            }
        }
    else {
        /* We are probably running over a serial/SSH terminal without
         * native console support, so just read from stdin.
         */
        if (read(_PM_console_fd, &c, 1) > 0)
            return c;
        }
    return 0;
}

/****************************************************************************
REMARKS:
Sleep until the virtual terminal is active
****************************************************************************/
static void wait_vt_active(
    int _PM_console_fd,
    int tty_vc)
{
    while (ioctl(_PM_console_fd, VT_WAITACTIVE, tty_vc) < 0) {
        if ((errno != EAGAIN) && (errno != EINTR)) {
            perror("ioctl(VT_WAITACTIVE)");
            exit(1);
            }
        usleep(150000);
        }
}

/****************************************************************************
REMARKS:
Opens up the console device for output by finding an appropriate virtual
console that we can run on.
****************************************************************************/
PM_HWND PMAPI PM_openConsole(
    PM_HWND hwndUser,
    int device,
    int xRes,
    int yRes,
    int bpp,
    ibool fullScreen)
{
    struct vt_stat  vts;
    struct stat     sbuf;
    char            fname[30];
    int             tty_vc = -1;

    /* Check if we have already opened the console */
    PM_init();
    if (console_count++)
        return _PM_console_fd;

    /* Now, it would be great if we could use /dev/tty and see what it is
     * connected to. Alas, we cannot find out reliably what VC /dev/tty is
     * bound to. Thus we parse stdin through stderr for a reliable VC.
     */
    startup_vc = 0;
    for (_PM_console_fd = 0; _PM_console_fd < 3; _PM_console_fd++) {
        if (fstat(_PM_console_fd, &sbuf) < 0)
            continue;
        if (ioctl(_PM_console_fd, VT_GETSTATE, &vts) < 0)
            continue;
        if (MAJOR(sbuf.st_rdev) != TTYAUX_MAJOR) {
            if (MAJOR(sbuf.st_rdev) != TTY_MAJOR)
                continue;
            if (MINOR(sbuf.st_rdev) == 0)
                continue;
            }
        else if (MINOR(sbuf.st_rdev) != 1)
            continue;

        /* Found VC support on one of our input streams. This means
         * that the user ran our program from one of the virtual
         * consoles, so we want to send all output to and get all input
         * from the currently active console. This may not be the
         * console our program started from if the user changed it with
         * 'chvt' inside a script.
         */
        tty_vc = vts.v_active;
        break;
        }

    /* If we could not find VT support attached to one of our standard
     * streams, try to create one that will work for us instead.
     */
    if (tty_vc == -1) {
        if ((_PM_console_fd = open("/dev/console", O_RDWR)) < 0)
            PM_fatalError("open_dev_console: can't open /dev/console\n");
        if (ioctl(_PM_console_fd, VT_OPENQRY, &tty_vc) < 0) {
            /* If we are running over a serial port or SSH session on a
             * embedded system without console support, we want to get all our
             * keyboard input from stdin. In this case we don't have to worry
             * about saving/restoring the console, since there is no console
             * support in the kernel. Hence we can early out of here.
             *
             * Regular desktop systems will however have VT switching support,
             * so when run from an X terminal we should not end up in here.
             */
            close(_PM_console_fd);
            _PM_haveConsole = false;
            return _PM_console_fd = fileno(stdin);
            }
        close(_PM_console_fd);
        if (tty_vc <= 0) {
            console_count = 0;
            PM_fatalError(
                "Not running in a graphics capable console,\n"
                "and unable to create one.\n");
            return -1;
            }
        }

    /* Change our control terminal */
    setsid();

    /* We must use RDWR to allow for output... */
    sprintf(fname, "/dev/tty%d", tty_vc);
    if ((_PM_console_fd = open(fname, O_RDWR)) < 0) {
        /* Try current console - /dev/tty0 */
        strcpy(fname, "/dev/tty0");
        if ((_PM_console_fd = open(fname, O_RDWR)) < 0)
            PM_fatalError("Unable to open correct /dev/tty!\n");
        }

    /* Get the current console state */
    if (ioctl(_PM_console_fd, VT_GETSTATE, &vts) < 0)
        PM_fatalError("Unable to get console state!\n");

    /* Switch to the new virtual terminal that we want to use */
    if (tty_vc != vts.v_active) {
        startup_vc = vts.v_active;

        /* Clear the console state before running */
        #define CLR_STR "\033[H\033[J"
        write(_PM_console_fd,CLR_STR,strlen(CLR_STR));

        ioctl(_PM_console_fd, VT_ACTIVATE, 1);
        ioctl(_PM_console_fd, VT_ACTIVATE, tty_vc);
        wait_vt_active(_PM_console_fd, tty_vc);
        }

    /* Redirect standard input to our new console */
    fflush(stdin);
    _PM_oldFD0 = dup(0);
    dup2(_PM_console_fd,0);
    stdin = fdopen(0, "rt");

    /* Redirect stdout to our new console, but only do this if
     * it is connected to a tty. If the output has been redirected
     * we do not wish to do this.
     */
    _PM_oldFD1 = -1;
    if (isatty(1)) {
        fflush(stdout);
        _PM_oldFD1 = dup(1);
        dup2(_PM_console_fd,1);
        stdout = fdopen(1, "wt");
        }

    /* Redirect stderr to our new console, but only do this if
     * it is connected to a tty. If the output has been redirected
     * we do not wish to do this.
     */
    _PM_oldFD2 = -1;
    if (isatty(2)) {
        fflush(stderr);
        _PM_oldFD2 = dup(2);
        dup2(_PM_console_fd,2);
        stderr = fdopen(2, "wt");
        }
    _PM_haveConsole = true;
    return _PM_console_fd;
}

/* 256 8-byte color palette entries */
#define PAL_C   2048

/****************************************************************************
REMARKS:
Returns the size of the console state buffer.
****************************************************************************/
int PMAPI PM_getConsoleStateSize(void)
{
    if (!inited)
        PM_init();
    return PM_getVGAStateSize();
}

/****************************************************************************
REMARKS:
Save the state of the Linux framebuffer console so we can restore it later.
****************************************************************************/
ibool PMAPI PM_saveFBConsoleState(
    int fd,
    void *stateBuf)
{
    struct fb_fix_screeninfo    *fix;
    struct fb_var_screeninfo    *var;
    struct fb_cmap              *cmap;
    u16                         *vbe_mode;
    u16                         *dac_width;
    u16                         *pal_start;
    RMREGS                      regs;

    /* Get pointers to the buffers to save the info in */
    CHECK(sizeof(struct fb_fix_screeninfo) + sizeof(struct fb_var_screeninfo) + 4 + PAL_C < PM_getVGAStateSize());
    fix = (struct fb_fix_screeninfo*)stateBuf;
    var = (struct fb_var_screeninfo*)((uchar*)fix + sizeof(struct fb_fix_screeninfo));
    cmap = (struct fb_cmap*)((uchar*)var + sizeof(struct fb_var_screeninfo));
    vbe_mode = (u16*)((uchar*)cmap + sizeof(struct fb_cmap));
    dac_width = (u16*)((uchar*)vbe_mode + sizeof(u16));
    pal_start = (u16*)((uchar*)dac_width + sizeof(u16));

    /* Save the console state */
    if (ioctl(fd, FBIOGET_FSCREENINFO, fix))
        perror("ioctl(FBIOGET_FSCREENINFO) failed");
    if (ioctl(fd, FBIOGET_VSCREENINFO, var))
        perror("ioctl(FBIOGET_VSCREENINFO) failed");

    /* Save the color map (palette) */
    cmap->start     = 0;
    cmap->len       = (var->bits_per_pixel >= 8) ? 256 : 16;
    cmap->red       = (pal_start + 0);
    cmap->green     = (pal_start + 256);
    cmap->blue      = (pal_start + 512);
    cmap->transp    = (pal_start + 768);
    if (ioctl(fd, FBIOGETCMAP, cmap))
        perror("ioctl(FBIOGETCMAP) failed");

    /* Save the VESA VBE mode if using the VESA driver */
    if (strncmp(fix->id,"VESA",4) == 0) {
        regs.x.ax = 0x4F03;
        PM_int86(0x10,&regs,&regs);
        *vbe_mode = regs.x.bx;
        regs.x.ax = 0x4F08;
        regs.h.bl = 0x01;
        PM_int86(0x10,&regs,&regs);
        *dac_width = regs.h.bh;
        }
    return true;
}

/****************************************************************************
REMARKS:
Restore the state of the Linux framebuffer console.
****************************************************************************/
void PMAPI PM_restoreFBConsoleState(
    int fd,
    const void *stateBuf)
{
    struct fb_fix_screeninfo    *fix;
    struct fb_var_screeninfo    *var;
    struct fb_cmap              *cmap;
    u16                         *vbe_mode;
    u16                         *dac_width;
    u16                         *pal_start;
    RMREGS                      regs;

    /* Get pointers to the buffers to save the info in */
    CHECK(sizeof(struct fb_fix_screeninfo) + sizeof(struct fb_var_screeninfo) + PAL_C < PM_getVGAStateSize());
    fix = (struct fb_fix_screeninfo*)stateBuf;
    var = (struct fb_var_screeninfo*)((uchar*)fix + sizeof(struct fb_fix_screeninfo));
    cmap = (struct fb_cmap*)((uchar*)var + sizeof(struct fb_var_screeninfo));
    vbe_mode = (u16*)((uchar*)cmap + sizeof(struct fb_cmap));
    dac_width = (u16*)((uchar*)vbe_mode + sizeof(u16));
    pal_start = (u16*)((uchar*)dac_width + sizeof(u16));

    /* Restore the console state */
    if (strncmp(fix->id,"VESA",4) == 0) {
        /* Restore the mode using the VESA BIOS, since the vesafb driver
         * does not support setting the display modes via the BIOS.
         */
        regs.x.ax = 0x4F02;
        regs.x.bx = *vbe_mode & ~0x8000;
        PM_int86(0x10,&regs,&regs);
        regs.x.ax = 0x4F08;
        regs.h.bl = 0x00;
        regs.h.bh = (uchar)*dac_width;
        PM_int86(0x10,&regs,&regs);
        }
    else if (ioctl(fd, FBIOPUT_VSCREENINFO, var))
        perror("ioctl(FBIOGET_VSCREENINFO) failed");

    /* Restore the color map (palette) */
    cmap->start     = 0;
    cmap->len       = (var->bits_per_pixel >= 8) ? 256 : 16;
    cmap->red       = (pal_start + 0);
    cmap->green     = (pal_start + 256);
    cmap->blue      = (pal_start + 512);
    cmap->transp    = (pal_start + 768);
    if (ioctl(fd, FBIOPUTCMAP, cmap))
        perror("ioctl(FBIOPUTCMAP) failed");
}

/****************************************************************************
REMARKS:
Save the state of the Linux console.
****************************************************************************/
void PMAPI PM_saveConsoleState(
    void *stateBuf,
    PM_HWND console_id)
{
    /* Bail early if we do not have console support as there is nothing to do */
    if (!_PM_haveConsole)
        return;

    /* Save the state of the framebuffer console if active */
    if ((fd_fbdev = open(DEFAULT_FRAMEBUFFER, O_RDONLY)) != -1) {
        if (!PM_saveFBConsoleState(fd_fbdev,stateBuf))
            fd_fbdev = -1;
        }

    /* Inform the Linux console that we are going into graphics mode */
    if (ioctl(console_id, KDSETMODE, KD_GRAPHICS) < 0)
        perror("ioctl(KDSETMODE)");

    /* Save the state of the VGA hardware if we did not save the
     * framebuffer console state earlier.
     */
    if (fd_fbdev == -1)
        PM_saveVGAState(stateBuf);
}

void PMAPI PM_setSuspendAppCallback(
    int (_ASMAPIP saveState)(
        int flags))
{
    /* TODO: Implement support for allowing console switching! */
}

/****************************************************************************
REMARKS:
Restore the state of the Linux console.
****************************************************************************/
void PMAPI PM_restoreConsoleState(
    const void *stateBuf,
    PM_HWND console_id)
{
    /* Bail early if we do not have console support as there is nothing to do */
    if (!_PM_haveConsole)
        return;

    /* Restore the state of the framebuffer console or the VGA hardware
     * registers, depending on what the previous state was.
     */
    if (fd_fbdev == -1)
        PM_restoreVGAState(stateBuf);
    else {
        PM_restoreFBConsoleState(fd_fbdev,stateBuf);
        close(fd_fbdev);
        fd_fbdev = -1;
        }

    /* Inform the Linux console that we are back from graphics modes */
    if (ioctl(console_id, KDSETMODE, KD_TEXT) < 0)
        perror("ioctl(KDSETMODE) failed");
}

/****************************************************************************
REMARKS:
Close the Linux console and put it back to normal.
****************************************************************************/
void PMAPI PM_closeConsole(
    PM_HWND _PM_console_fd)
{
    /* Restore console to normal operation */
    if (--console_count == 0) {
        /* Make sure keyboard is restored first */
        _PM_restore_kb_mode();

        if (startup_vc > 0) {
            /* Re-activate the original virtual console */
            ioctl(_PM_console_fd, VT_ACTIVATE, startup_vc);

            /* Restore standard file descriptors */
            dup2(_PM_oldFD0,0);
            if (_PM_oldFD1 != -1)
                dup2(_PM_oldFD1,1);
            if (_PM_oldFD2 != -1)
                dup2(_PM_oldFD2,2);
            }

        /* Close the console file descriptor */
        if (_PM_console_fd > 2)
            close(_PM_console_fd);
        _PM_console_fd = -1;
        }
}

void PM_setOSCursorLocation(
    int x,
    int y)
{
    /* Nothing to do in here */
}

/****************************************************************************
REMARKS:
Set the screen width and height for the Linux console.
****************************************************************************/
void PM_setOSScreenWidth(
    int width,
    int height)
{
    struct winsize  ws;
    struct vt_sizes vs;

    if (_PM_haveConsole) {
        /* Resize the software terminal */
        ws.ws_col = width;
        ws.ws_row = height;
        ioctl(_PM_console_fd, TIOCSWINSZ, &ws);

        /* And the hardware */
        vs.v_rows = height;
        vs.v_cols = width;
        vs.v_scrollsize = 0;
        ioctl(_PM_console_fd, VT_RESIZE, &vs);
        }
}

ibool PMAPI PM_setRealTimeClockHandler(PM_intHandler ih, int frequency)
{
    /* Not supported */
    return false;
}

void PMAPI PM_setRealTimeClockFrequency(int frequency)
{
    /* Not supported */
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
    /* Not supported */
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
#ifdef __64BIT__
    return env ? env : "/usr/lib64/snap";
#else
    return env ? env : "/usr/lib/snap";
#endif
}

const char * PMAPI PM_getSNAPConfigPath(void)
{
    static char path[256];
    char        *env;

    if ((env = getenv("SNAP_CONFIG_PATH")) != NULL)
        return env;
    strcpy(path,PM_getSNAPPath());
    PM_backslash(path);
    strcat(path,"config");
    return path;
}

const char * PMAPI PM_getUniqueID(void)
{
    return PM_getMachineName();
}

const char * PMAPI PM_getMachineName(void)
{
    static char buf[128];
    gethostname(buf, 128);
    return buf;
}

void * PMAPI PM_getBIOSPointer(void)
{
#ifdef __INTEL__
    static uchar *zeroPtr = NULL;
    if (!zeroPtr)
        zeroPtr = PM_mapPhysicalAddr(0,0xFFFFF,true);
    return (void*)(zeroPtr + 0x400);
#else
    /* On non-x86 platforms, the BIOS Data Area doesn't exist. */
    return NULL;
#endif
}

void * PMAPI PM_getA0000Pointer(void)
{
    /* PM_init maps in the 0xA0000 framebuffer region 1:1 with our
     * address mapping, so we can return the address here.
     */
    if (!inited)
        PM_init();
#if defined(__INTEL__) || defined(__X86_64__)
    return (void*)0xA0000;
#else
    if (_PM_memBaseISA != NULL)
        return _PM_memBaseISA + 0xA0000;
    return NULL;
#endif
}

/****************************************************************************
DESCRIPTION:
Map a physical address to a linear address in the callers process.

HEADER:
pmapi.h

PARAMETERS:
base        - Physical base address of the memory to map
limit       - Limit for the mapped memory region (length-1)
isCached    - True if the memory should be cached, false if not

RETURNS:
Pointer to the mapped memory, false on failure.

REMARKS:
This function is used to obtain a pointer to the any physical memory
location in the computer, mapped into the linear address space of the
calling process. If the isCached parameter is set to true, caching will
be enabled for this region. If this parameter is off, caching will be
disabled. Caching must always be disabled when accessing memory mapped
registers, as they cannot be cached. Note that this does not enable
write combing for the region; for that you need to call the
PM_enableWriteCombine function (however caching must be enabled before
the write combining will work!).

SEE ALSO:
PM_freePhysicalAddr, PM_getPhysicalAddr
****************************************************************************/
void * PMAPI PM_mapPhysicalAddr(
    ulong base,
    ulong limit,
    ibool isCached)
{
    void        *p = NULL,*baseAddr = NULL;
    int         i;
    ulong       length = limit+1;
    mmapping    *maps,*map = NULL;

    /* Initialise the library and handle special cases */
    if (!inited)
        PM_init();
#if defined(__INTEL__) || defined(__X86_64__)
    if (base >= 0xA0000 && base < 0x100000)
        return (void*)base;
#else
    /* Map ISA memory below 1Meg */
    if (base >= 0xA0000 && base < 0x100000) {
        if (_PM_memBaseISA != NULL)
            return (void*)((ulong)_PM_memBaseISA + base);
        return NULL;
        }

    /* Map physical bus memory by adding in the offset needed */
    base += _PM_memBase_phys;
#endif

    /* Get exclusive access to shared memory subsystem */
    PM_lockSNAPAccess(-1,true);

    /* Search table of existing mappings to see if we have already mapped
     * a region of memory that will serve this purpose.
     */
    if (sharedInfo) {
        maps = sharedInfo->maps;
        for (i = 0; i < sharedInfo->s.numMaps; i++) {
            if (maps[i].physical == base && maps[i].length == length) {
                maps[i].refCount++;
                p = maps[i].linear;
                PM_unlockSNAPAccess(-1);
                return p;
                }
            }

        /* Now find a free slot in our memory mapping table */
        for (i = 0; i < sharedInfo->s.numMaps; i++) {
            if (maps[i].length == 0)
                break;
            }
        if (i == sharedInfo->s.numMaps) {
            if (sharedInfo->s.numMaps == (MAX_MEMORY_MAPPINGS-1)) {
                PM_unlockSNAPAccess(-1);
                return NULL;
                }
            sharedInfo->s.numMaps++;
            }
        map = &maps[i];

        /* Now find the next base address in our shared memory arena
         * that we can use for this mapping. We then extend the top of
         * our physical memory mapping area to start at the next 4K
         * page past this mapping plus one more page as a guard page.
         */
        baseAddr = (void*)sharedInfo->s.physMemTop;
        sharedInfo->s.physMemTop += ((length+4095) & ~4095) + PAGE_SIZE;
        }

    /* Now map the physical memory location */
    p = _PM_mapPhysicalAddr(baseAddr,base,length,isCached);

    /* Store the mapped memory information away in shared memory for
     * other processes to accesss if the shared memory manager is up.
     */
    if (map && p) {
        map->physical = base;
        map->linear = p;
        map->length = length;
        map->isCached = isCached;
        map->refCount = 1;

        /* Signal all other processes that a new memory mapping has been
         * created and to map that memory into all those processes as well.
         */
        _PM_signalThreads(map,0,NULL);
        }

    /* Release exclusive access to shared memory subsystem */
    PM_unlockSNAPAccess(-1);
    return p;
}

/****************************************************************************
DESCRIPTION:
Free a physical address mapping allocated by PM_mapPhysicalAddr.

HEADER:
pmapi.h

PARAMETERS:
ptr     - Linear address of the address to free
limit   - Limit for the mapped memory region (length-1)

REMARKS:
This function is used to free an address mapping previously allocated
with the PM_mapPhysicalAddr function.

SEE ALSO:
PM_mapPhysicalAddr
****************************************************************************/
void PMAPI PM_freePhysicalAddr(
    void *ptr,
    ulong limit)
{
    int         i;
    ulong       length = limit+1;
    mmapping    *maps;

    /* Handle special cases */
    if ((ulong)ptr < 0x100000)
        return;

    /* First unmap the memory */
    munmap(ptr,length);

    /* Get exclusive access to shared memory subsystem */
    PM_lockSNAPAccess(-1,true);

    /* Search table of existing mappings and clear that mapping in the
     * table if it is found.
     */
    if (sharedInfo) {
        maps = sharedInfo->maps;
        for (i = 0; i < sharedInfo->s.numMaps; i++) {
            if (maps[i].linear == ptr && maps[i].length == length) {
                if (--maps[i].refCount == 0)
                    memset(&maps[i],0,sizeof(maps[i]));
                break;
                }
            }
        }

    /* Release exclusive access to shared memory subsystem */
    PM_unlockSNAPAccess(-1);
}

/****************************************************************************
REMARKS:
Add kernel memory address and user linear address to kernel mmap list.
****************************************************************************/
void _PM_addKernelAddr(
    void *kernelAddr,
    void *userAddr)
{
    int i;
    for (i = 0; i < MAX_KMAPS; i++) {
        if (kmap[i].kernelAddr == NULL) {
            kmap[i].kernelAddr = kernelAddr;
            kmap[i].userAddr = userAddr;
            numKernelMaps++;
            return;
            }
        }
}

/****************************************************************************
REMARKS:
Remove kernel memory address and user linear address from kernel mmap list.
****************************************************************************/
void _PM_removeKernelAddr(
    void *userAddr)
{
    int i;
    for (i = 0; i < MAX_KMAPS; i++) {
        if (kmap[i].userAddr == userAddr) {
            kmap[i].kernelAddr = NULL;
            kmap[i].userAddr = NULL;
            numKernelMaps--;
            return;
            }
        }
}

/****************************************************************************
REMARKS:
Get kernel memory address corresponding to user linear address.
****************************************************************************/
void * _PM_getKernelAddr(
    void *userAddr)
{
    int i;
    for (i = 0; i < numKernelMaps; ) {
        if (kmap[i].userAddr == NULL)
            continue;
        if (kmap[i].userAddr == userAddr)
            return kmap[i].kernelAddr;
        i++;
        }
    return userAddr;
}

/****************************************************************************
REMARKS:
Get physical memory address via kernel module service.
****************************************************************************/
ulong PMAPI PM_getPhysicalAddr(
    void *p)
{
    ulong buf[2];

    if (fd_pmhelp == -1)
    return 0xFFFFFFFFUL;
    buf[0] = (ulong)_PM_getKernelAddr(p);
    buf[1] = 0xFFFFFFFFUL;
    ioctl(fd_pmhelp,PMHELP_GETPHYSICALADDR,&buf);
    return (ulong)buf[1];
}

/****************************************************************************
REMARKS:
Get physical memory address range.
****************************************************************************/
ibool PMAPI PM_getPhysicalAddrRange(
    void *p,
    ulong length,
    ulong *physAddress)
{
    // We have to call the PM service repetitively for each address
    // since the I/O call has no way of knowing how many user space
    // parameters would have to be accessed in variable list.
    int     i;
    ulong   linear = (ulong)p & PAGE_MASK;

    for (i = (length + PAGE_SIZE-1) >> PAGE_SHIFT; i > 0; i--) {
        if ((*physAddress++ = PM_getPhysicalAddr((void*)linear)) == 0xFFFFFFFF)
    return false;
        linear += PAGE_SIZE;
        }
    return true;
}

/****************************************************************************
REMARKS:
Put process thread to sleep for so many milliseconds.
****************************************************************************/
void PMAPI PM_sleep(ulong milliseconds)
{
    if (milliseconds)
        usleep(milliseconds * 1000);
    else
        sched_yield();
}

int PMAPI PM_getCOMPort(int port)
{
    /* Not supported on Linux */
    return 0;
}

int PMAPI PM_getLPTPort(int port)
{
    /* Not supported on Linux */
    return 0;
}

/****************************************************************************
DESCRIPTION:
Allocate a block of system global shared memory

HEADER:
pmapi.h

PARAMETERS:
size    - Size of the shared memory block to allocate

RETURNS:
Pointer to the shared memory block, NULL on failure.

REMARKS:
This function is used to allocate a block of shared memory, such that the
linear address returned for this shared memory is /identical/ for all \
processes in the system using the PM library. If this cannot be provided,
this function will return NULL.

Note that this function is not as efficient as it could be, since we always
do a linear search for the first free block to use. However in practice
this function is only used to allocate large blocks of memory from the
system (usually >= 64K) at a time, so it will be efficient enough for the
purposes that we need it for.
****************************************************************************/
void * PMAPI PM_mallocShared(
    long size)
{
    int             id,poolsize;
    mem_pool        *pool;
    mem_header      *mem,**prevmem;
    struct shmid_ds shm;
    int             *ptr;

    /* Get exclusive access to shared memory subsystem */
    PM_lockSNAPAccess(-1,true);

    /* Go through the list of memory pools to see if we can find one
     * that has a block on the free list big enough to fit.
     */
    size = MAX((size + sizeof(int)),sizeof(mem_header));
    for (pool = sharedInfo->s.mempool, mem = NULL; pool; pool = pool->next) {
        prevmem = &pool->free;
        for (mem = pool->free; mem; mem = mem->next) {
            if (mem->size >= size)
                break;
            prevmem = &mem->next;
            }
        if (mem)
            break;
        }

    /* If we did not find a pool big enough for what we need,
     * allocate a new pool big enough for this block and link it to
     * the start of the memory pool list.
     */
    if (!pool) {
        poolsize = MAX(size + sizeof(mem_pool),PM_MIN_SHARED_POOL_SIZE);
        if ((id = shmget(IPC_PRIVATE, poolsize, IPC_CREAT | IPC_EXCL | 0666)) == -1) {
            PM_unlockSNAPAccess(-1);
            PM_fatalError("PM_mallocShared: Unable to create shared memory block!");
            }

        /* Now find the next base address in our shared memory arena
         * that we can use for this memory block. We then extend the top of
         * our shared memory area to start at the next 4K page past this
         * mapping plus one more page as a guard page.
         */
        if ((pool = shmat(id, (void*)sharedInfo->s.sharedMemTop, 0)) == (void*)-1) {
            PM_unlockSNAPAccess(-1);
            PM_fatalError("PM_mallocShared: Unable to map shared memory!");
            }

        /* Hook the pool onto the head of the pool list */
        if (shmctl(id, IPC_STAT, &shm) == -1) {
            perror("shmctl");
            PM_unlockSNAPAccess(-1);
            PM_fatalError("PM_mallocShared: Unable to access shared memory!");
            }
        pool->size = shm.shm_segsz;
        pool->id = sharedInfo->s.memid;
        pool->next = sharedInfo->s.mempool;
        sharedInfo->s.memid = id;
        sharedInfo->s.mempool = pool;
        sharedInfo->s.sharedMemTop += ((shm.shm_segsz+4095) & ~4095) + PAGE_SIZE;

        /* Create the free list for the pool. Note that we get the actual size of the
         * shared memory block, since the real size may be rounded up to a page
         * boundary.
         */
        pool->free = mem = (mem_header*)(pool + 1);
        mem->next = NULL;
        mem->size = pool->size - sizeof(mem_pool);
        prevmem = &pool->free;

        /* Signal other processes to map the new memory block into the process
         * address space.
         */
        _PM_signalThreads(NULL,id,pool);
        }

    /* Now sub-allocate the memory from the pool we found */
    if (size < mem->size) {
        /* New block is smaller than free list block, so pare it off
         * and leave the shrunk block on the free list.
         */
        mem->size -= size;
        ptr = (int*)((uchar*)mem + mem->size);
        }
    else {
        /* Block is the same size, so remove this block from the free
         * list.
         */
        *prevmem = mem->next;
        ptr = (int*)mem;
        }

    /* Store the size of the block at the start of the block in memory */
    *ptr = size;

    /* Release exclusive access to shared memory subsystem */
    PM_unlockSNAPAccess(-1);
    return (void*)(ptr+1);
}

/****************************************************************************
DESCRIPTION:
Frees a block of global shared memory.

HEADER:
pmapi.h

PARAMETERS:
ptr - Shared memory block to free

REMARKS:
This function is used to free a block of global shared memory previously
allocated with the PM_mallocShared function.
****************************************************************************/
void PMAPI PM_freeShared(
    void *ptr)
{
    mem_pool    *pool;
    mem_header  *before,*after,*m,*mem = (mem_header*)(((int*)ptr) - 1);
    mem_header  **prevm,**prevAfter=NULL,**prevBefore=NULL;
    int         size = *((int*)mem);

    /* Get exclusive access to shared memory subsystem */
    PM_lockSNAPAccess(-1,true);

    /* Go through the list of memory pools looking for the one that
     * contains this block, and then add it to the free list for that
     * pool.
     */
    for (pool = sharedInfo->s.mempool; pool; pool = pool->next) {
        if ((uchar*)pool < (uchar*)mem && (uchar*)mem < (uchar*)pool + pool->size)
            break;
        }
    if (!pool) {
        PM_unlockSNAPAccess(-1);
        PM_fatalError("PM_freeShared: Unable to find matching memory block pool!");
        }

#ifdef CHECKED
    /* Go through the free list to make sure we are not getting called to
     * free the same memory block twice.
     */
    for (m = pool->free; m; m = m->next) {
        if ((uchar*)m <= (uchar*)mem && (uchar*)mem < (uchar*)m + m->size) {
            PM_unlockSNAPAccess(-1);
            PM_fatalError("PM_freeShared: Attempt to free memory block twice!");
            return;
            }
        }
#endif

    /* Combine adjacent memory blocks on the free list */
    mem->size = size;
    before = after = NULL;
    prevm = &pool->free;
    for (m = pool->free; m; m = m->next) {
        if (((uchar*)m + m->size) == (uchar*)mem) {
            /* This block is immediately prior to the block being freed */
            before = m;
            prevBefore = prevm;
            }
        if (((uchar*)mem + mem->size) == (uchar*)m) {
            /* This block is immediately after the block being freed */
            after = m;
            prevAfter = prevm;
            }
        prevm = &m->next;
        }

    /* Merge the blocks before and after this block if necessary */
    if (before) {
        /* There is a block immediately prior to the block being freed,
         * so remove it from the list and expand the before block
         * to consume the block we are freeing.
         */
        *prevBefore = before->next;
        before->size += mem->size;
        mem = before;
        if (after && (prevAfter == &before->next))
            prevAfter = prevBefore;
        }
    if (after) {
        /* There is a block immediately after the block being freed,
         * so remove it from the list and expand the after block
         * to consume the after block.
         */
        *prevAfter = after->next;
        mem->size += after->size;
        }

    /* Now add the block onto the free list */
    mem->next = pool->free;
    pool->free = mem;

    /* Release exclusive access to shared memory subsystem */
    PM_unlockSNAPAccess(-1);
}

void * PMAPI PM_mapRealPointer(uint r_seg,uint r_off)
{
#ifdef __INTEL__
    /* PM_init maps in the 0xA0000-0x100000 region 1:1 with our
     * address mapping, as well as all memory blocks in a 1:1 address
     * mapping so we can simply return the physical address in here.
     */
    if (!inited)
        PM_init();
    if (__PM_biosFuncs.PM_mapRealPointer)
        return __PM_biosFuncs.PM_mapRealPointer(r_seg,r_off);
    return (void*)MK_PHYS(r_seg,r_off);
#else
    if (__PM_biosFuncs.PM_mapRealPointer)
        return __PM_biosFuncs.PM_mapRealPointer(r_seg,r_off);
    return NULL;
#endif
}

void * PMAPI PM_allocRealSeg(uint size,uint *r_seg,uint *r_off)
{
#ifdef __INTEL__
    int     i;
    char    *r = (char *)REAL_MEM_BASE;

    if (!inited)
        PM_init();
    if (!mem_info.ready)
        return NULL;
    if (mem_info.count == REAL_MEM_BLOCKS)
        return NULL;
    size = (size + 15) & ~15;
    for (i = 0; i < mem_info.count; i++) {
        if (mem_info.blocks[i].free && size < mem_info.blocks[i].size) {
            insert_block(i);
            mem_info.blocks[i].size = size;
            mem_info.blocks[i].free = 0;
            mem_info.blocks[i + 1].size -= size;
            *r_seg = (uint)(r) >> 4;
            *r_off = (uint)(r) & 0xF;
            return (void *)r;
            }
        r += mem_info.blocks[i].size;
        }
#endif
    return NULL;
}

void PMAPI PM_freeRealSeg(void *mem)
{
#ifdef __INTEL__
    int     i;
    char    *r = (char *)REAL_MEM_BASE;

    if (!mem_info.ready)
        return;
    i = 0;
    while (mem != (void *)r) {
        r += mem_info.blocks[i].size;
        i++;
        if (i == mem_info.count)
            return;
        }
    mem_info.blocks[i].free = 1;
    if (i + 1 < mem_info.count && mem_info.blocks[i + 1].free) {
        mem_info.blocks[i].size += mem_info.blocks[i + 1].size;
        delete_block(i + 1);
        }
    if (i - 1 >= 0 && mem_info.blocks[i - 1].free) {
        mem_info.blocks[i - 1].size += mem_info.blocks[i].size;
        delete_block(i);
        }
#endif
}

#ifdef __INTEL__

#define DIRECTION_FLAG  (1 << 10)

static void em_ins(int size)
{
    unsigned int edx, edi;

    edx = context.vm.regs.edx & 0xffff;
    edi = context.vm.regs.edi & 0xffff;
    edi += (unsigned int)context.vm.regs.ds << 4;
    if (context.vm.regs.eflags & DIRECTION_FLAG) {
        if (size == 4)
            asm volatile ("std; insl; cld"
             : "=D" (edi) : "d" (edx), "0" (edi));
        else if (size == 2)
            asm volatile ("std; insw; cld"
             : "=D" (edi) : "d" (edx), "0" (edi));
        else
            asm volatile ("std; insb; cld"
             : "=D" (edi) : "d" (edx), "0" (edi));
        }
    else {
        if (size == 4)
            asm volatile ("cld; insl"
             : "=D" (edi) : "d" (edx), "0" (edi));
        else if (size == 2)
            asm volatile ("cld; insw"
             : "=D" (edi) : "d" (edx), "0" (edi));
        else
            asm volatile ("cld; insb"
             : "=D" (edi) : "d" (edx), "0" (edi));
        }
    edi -= (unsigned int)context.vm.regs.ds << 4;
    context.vm.regs.edi &= 0xffff0000;
    context.vm.regs.edi |= edi & 0xffff;
}

static void em_rep_ins(int size)
{
    unsigned int ecx, edx, edi;

    ecx = context.vm.regs.ecx & 0xffff;
    edx = context.vm.regs.edx & 0xffff;
    edi = context.vm.regs.edi & 0xffff;
    edi += (unsigned int)context.vm.regs.ds << 4;
    if (context.vm.regs.eflags & DIRECTION_FLAG) {
        if (size == 4)
            asm volatile ("std; rep; insl; cld"
             : "=D" (edi), "=c" (ecx)
             : "d" (edx), "0" (edi), "1" (ecx));
        else if (size == 2)
            asm volatile ("std; rep; insw; cld"
             : "=D" (edi), "=c" (ecx)
             : "d" (edx), "0" (edi), "1" (ecx));
        else
            asm volatile ("std; rep; insb; cld"
             : "=D" (edi), "=c" (ecx)
             : "d" (edx), "0" (edi), "1" (ecx));
        }
    else {
        if (size == 4)
            asm volatile ("cld; rep; insl"
             : "=D" (edi), "=c" (ecx)
             : "d" (edx), "0" (edi), "1" (ecx));
        else if (size == 2)
            asm volatile ("cld; rep; insw"
             : "=D" (edi), "=c" (ecx)
             : "d" (edx), "0" (edi), "1" (ecx));
        else
            asm volatile ("cld; rep; insb"
             : "=D" (edi), "=c" (ecx)
             : "d" (edx), "0" (edi), "1" (ecx));
        }

    edi -= (unsigned int)context.vm.regs.ds << 4;
    context.vm.regs.edi &= 0xffff0000;
    context.vm.regs.edi |= edi & 0xffff;
    context.vm.regs.ecx &= 0xffff0000;
    context.vm.regs.ecx |= ecx & 0xffff;
}

static void em_outs(int size)
{
    unsigned int edx, esi;

    edx = context.vm.regs.edx & 0xffff;
    esi = context.vm.regs.esi & 0xffff;
    esi += (unsigned int)context.vm.regs.ds << 4;
    if (context.vm.regs.eflags & DIRECTION_FLAG) {
        if (size == 4)
            asm volatile ("std; outsl; cld"
             : "=S" (esi) : "d" (edx), "0" (esi));
        else if (size == 2)
            asm volatile ("std; outsw; cld"
             : "=S" (esi) : "d" (edx), "0" (esi));
        else
            asm volatile ("std; outsb; cld"
             : "=S" (esi) : "d" (edx), "0" (esi));
        }
    else {
        if (size == 4)
            asm volatile ("cld; outsl"
             : "=S" (esi) : "d" (edx), "0" (esi));
        else if (size == 2)
            asm volatile ("cld; outsw"
             : "=S" (esi) : "d" (edx), "0" (esi));
        else
            asm volatile ("cld; outsb"
             : "=S" (esi) : "d" (edx), "0" (esi));
        }

    esi -= (unsigned int)context.vm.regs.ds << 4;
    context.vm.regs.esi &= 0xffff0000;
    context.vm.regs.esi |= esi & 0xffff;
}

static void em_rep_outs(int size)
{
    unsigned int ecx, edx, esi;

    ecx = context.vm.regs.ecx & 0xffff;
    edx = context.vm.regs.edx & 0xffff;
    esi = context.vm.regs.esi & 0xffff;
    esi += (unsigned int)context.vm.regs.ds << 4;
    if (context.vm.regs.eflags & DIRECTION_FLAG) {
        if (size == 4)
            asm volatile ("std; rep; outsl; cld"
             : "=S" (esi), "=c" (ecx)
             : "d" (edx), "0" (esi), "1" (ecx));
        else if (size == 2)
            asm volatile ("std; rep; outsw; cld"
             : "=S" (esi), "=c" (ecx)
             : "d" (edx), "0" (esi), "1" (ecx));
        else
            asm volatile ("std; rep; outsb; cld"
             : "=S" (esi), "=c" (ecx)
             : "d" (edx), "0" (esi), "1" (ecx));
        }
    else {
        if (size == 4)
            asm volatile ("cld; rep; outsl"
             : "=S" (esi), "=c" (ecx)
             : "d" (edx), "0" (esi), "1" (ecx));
        else if (size == 2)
            asm volatile ("cld; rep; outsw"
             : "=S" (esi), "=c" (ecx)
             : "d" (edx), "0" (esi), "1" (ecx));
        else
            asm volatile ("cld; rep; outsb"
             : "=S" (esi), "=c" (ecx)
             : "d" (edx), "0" (esi), "1" (ecx));
        }

    esi -= (unsigned int)context.vm.regs.ds << 4;
    context.vm.regs.esi &= 0xffff0000;
    context.vm.regs.esi |= esi & 0xffff;
    context.vm.regs.ecx &= 0xffff0000;
    context.vm.regs.ecx |= ecx & 0xffff;
}

static int emulate(void)
{
    unsigned char *insn;
    struct {
        unsigned int size : 1;
        unsigned int rep : 1;
        } prefix = { 0, 0 };
    int i = 0;

    insn = (unsigned char *)((unsigned int)context.vm.regs.cs << 4);
    insn += context.vm.regs.eip;

    while (1) {
#ifdef TRACE_IO
        traceAddr = ((ulong)context.vm.regs.cs << 16) + context.vm.regs.eip + i;
#endif
        if (insn[i] == 0x66) {
            prefix.size = 1 - prefix.size;
            i++;
            }
        else if (insn[i] == 0xf3) {
            prefix.rep = 1;
            i++;
            }
        else if (insn[i] == 0xf0 || insn[i] == 0xf2
             || insn[i] == 0x26 || insn[i] == 0x2e
             || insn[i] == 0x36 || insn[i] == 0x3e
             || insn[i] == 0x64 || insn[i] == 0x65
             || insn[i] == 0x67) {
            /* these prefixes are just ignored */
            i++;
            }
        else if (insn[i] == 0x6c) {
            if (prefix.rep)
                em_rep_ins(1);
            else
                em_ins(1);
            i++;
            break;
            }
        else if (insn[i] == 0x6d) {
            if (prefix.rep) {
                if (prefix.size)
                    em_rep_ins(4);
                else
                    em_rep_ins(2);
                }
            else {
                if (prefix.size)
                    em_ins(4);
                else
                    em_ins(2);
                }
            i++;
            break;
            }
        else if (insn[i] == 0x6e) {
            if (prefix.rep)
                em_rep_outs(1);
            else
                em_outs(1);
            i++;
            break;
            }
        else if (insn[i] == 0x6f) {
            if (prefix.rep) {
                if (prefix.size)
                    em_rep_outs(4);
                else
                    em_rep_outs(2);
                }
            else {
                if (prefix.size)
                    em_outs(4);
                else
                    em_outs(2);
                }
            i++;
            break;
            }
        else if (insn[i] == 0xec) {
            *((uchar*)&context.vm.regs.eax) = port_in(context.vm.regs.edx);
            i++;
            break;
            }
        else if (insn[i] == 0xed) {
            if (prefix.size)
                *((ulong*)&context.vm.regs.eax) = port_inl(context.vm.regs.edx);
            else
                *((ushort*)&context.vm.regs.eax) = port_inw(context.vm.regs.edx);
            i++;
            break;
            }
        else if (insn[i] == 0xee) {
            port_out(context.vm.regs.eax,context.vm.regs.edx);
            i++;
            break;
            }
        else if (insn[i] == 0xef) {
            if (prefix.size)
                port_outl(context.vm.regs.eax,context.vm.regs.edx);
            else
                port_outw(context.vm.regs.eax,context.vm.regs.edx);
            i++;
            break;
            }
        else {
            fprintf(stderr, "Unknown emulated instruction: %2x, i = %d\n", insn[i], i);
            return 0;
            }
        }

    context.vm.regs.eip += i;
    return 1;
}

static void debug_info(int vret)
{
    int i;
    unsigned char *p;

    fputs("vm86() failed\n", stderr);
    fprintf(stderr, "return = 0x%x\n", vret);
    fprintf(stderr, "eax = 0x%08lx\n", context.vm.regs.eax);
    fprintf(stderr, "ebx = 0x%08lx\n", context.vm.regs.ebx);
    fprintf(stderr, "ecx = 0x%08lx\n", context.vm.regs.ecx);
    fprintf(stderr, "edx = 0x%08lx\n", context.vm.regs.edx);
    fprintf(stderr, "esi = 0x%08lx\n", context.vm.regs.esi);
    fprintf(stderr, "edi = 0x%08lx\n", context.vm.regs.edi);
    fprintf(stderr, "ebp = 0x%08lx\n", context.vm.regs.ebp);
    fprintf(stderr, "eip = 0x%08lx\n", context.vm.regs.eip);
    fprintf(stderr, "cs  = 0x%04x\n", context.vm.regs.cs);
    fprintf(stderr, "esp = 0x%08lx\n", context.vm.regs.esp);
    fprintf(stderr, "ss  = 0x%04x\n", context.vm.regs.ss);
    fprintf(stderr, "ds  = 0x%04x\n", context.vm.regs.ds);
    fprintf(stderr, "es  = 0x%04x\n", context.vm.regs.es);
    fprintf(stderr, "fs  = 0x%04x\n", context.vm.regs.fs);
    fprintf(stderr, "gs  = 0x%04x\n", context.vm.regs.gs);
    fprintf(stderr, "eflags  = 0x%08lx\n", context.vm.regs.eflags);
    fputs("cs:ip = [ ", stderr);
    p = (unsigned char *)((context.vm.regs.cs << 4) + (context.vm.regs.eip & 0xffff));
    for (i = 0; i < 16; ++i)
            fprintf(stderr, "%02x ", (unsigned int)p[i]);
    fputs("]\n", stderr);
    fflush(stderr);
}

static int run_vm86(void)
{
    unsigned int    vret;
    sigset_t        set;
    sigset_t        oldSet;

    /* Make sure I/O permissions are set to on for the vm86() task so
     * it can properly do direct I/O access to low ports. This allows us
     * to avoid emulating the low I/O port access functions. For some reason
     * on some XFree86 versions, the ioperm() setting is not correct when
     * returning from a virtual console mode, so we set it here just to be
     * sure.
     */
    ioperm(0x0,0x400,1);
    sigfillset(&set);
    for (;;) {
        /* Block all signals across vm86() calls, to avoid compatibility
         * problems.
         */
        sigprocmask (SIG_BLOCK, &set, &oldSet);
        vret = vm86(&context.vm);
        sigprocmask (SIG_SETMASK, &oldSet, NULL);
        if (VM86_TYPE(vret) == VM86_INTx) {
            unsigned int v = VM86_ARG(vret);
            if (v == RETURN_TO_32_INT)
                return 1;
            pushw(context.vm.regs.eflags);
            pushw(context.vm.regs.cs);
            pushw(context.vm.regs.eip);
            context.vm.regs.cs = get_int_seg(v);
            context.vm.regs.eip = get_int_off(v);
            context.vm.regs.eflags &= ~(VIF_MASK | TF_MASK);
            continue;
            }
        if (VM86_TYPE(vret) == VM86_SIGNAL)
            continue;
        if (VM86_TYPE(vret) != VM86_UNKNOWN)
            break;
        if (!emulate())
            break;
        }
    debug_info(vret);
    return 0;
}
#endif  /* !__INTEL__ */

#define IN(ereg) context.vm.regs.ereg = in->e.ereg
#define OUT(ereg) out->e.ereg = context.vm.regs.ereg

int PMAPI PM_int86(
    int intno,
    RMREGS *in,
    RMREGS *out)
{
#ifdef __INTEL__
    if (!inited)
        PM_init();
    if (__PM_biosFuncs.PM_int86)
        return __PM_biosFuncs.PM_int86(intno,in,out);
    if (_PM_haveBIOSAccess) {
        memset(&context.vm.regs, 0, sizeof(context.vm.regs));
        IN(eax); IN(ebx); IN(ecx); IN(edx); IN(esi); IN(edi);
        context.vm.regs.eflags = DEFAULT_VM86_FLAGS;
        context.vm.regs.cs = get_int_seg(intno);
        context.vm.regs.eip = get_int_off(intno);
        context.vm.regs.ss = context.stack_seg;
        context.vm.regs.esp = context.stack_off;
        pushw(DEFAULT_VM86_FLAGS);
        pushw(context.ret_seg);
        pushw(context.ret_off);
        run_vm86();
        OUT(eax); OUT(ebx); OUT(ecx); OUT(edx); OUT(esi); OUT(edi);
        out->x.cflag = context.vm.regs.eflags & 1;
        }
    else {
        *out = *in;
        }
#endif
    if (__PM_biosFuncs.PM_int86)
        return __PM_biosFuncs.PM_int86(intno,in,out);
    *out = *in;
    return out->x.ax;
}

int PMAPI PM_int86x(
    int intno,
    RMREGS *in,
    RMREGS *out,
    RMSREGS *sregs)
{
#ifdef __INTEL__
    if (!inited)
        PM_init();
    if (intno == 0x21) {
        time_t today = time(NULL);
        struct tm *t;
        t = localtime(&today);
        out->x.cx = t->tm_year + 1900;
        out->h.dh = t->tm_mon + 1;
        out->h.dl = t->tm_mday;
        return out->e.eax;
        }
    if (__PM_biosFuncs.PM_int86x)
        return __PM_biosFuncs.PM_int86x(intno,in,out,sregs);
    if (_PM_haveBIOSAccess) {
        unsigned int seg, off;
        seg = get_int_seg(intno);
        off = get_int_off(intno);
        memset(&context.vm.regs, 0, sizeof(context.vm.regs));
        IN(eax); IN(ebx); IN(ecx); IN(edx); IN(esi); IN(edi);
        context.vm.regs.eflags = DEFAULT_VM86_FLAGS;
        context.vm.regs.cs = seg;
        context.vm.regs.eip = off;
        context.vm.regs.es = sregs->es;
        context.vm.regs.ds = sregs->ds;
        context.vm.regs.fs = sregs->fs;
        context.vm.regs.gs = sregs->gs;
        context.vm.regs.ss = context.stack_seg;
        context.vm.regs.esp = context.stack_off;
        pushw(DEFAULT_VM86_FLAGS);
        pushw(context.ret_seg);
        pushw(context.ret_off);
        run_vm86();
        OUT(eax); OUT(ebx); OUT(ecx); OUT(edx); OUT(esi); OUT(edi);
        sregs->es = context.vm.regs.es;
        sregs->ds = context.vm.regs.ds;
        sregs->fs = context.vm.regs.fs;
        sregs->gs = context.vm.regs.gs;
        out->x.cflag = context.vm.regs.eflags & 1;
        }
    else {
        *out = *in;
        }
#endif
    if (__PM_biosFuncs.PM_int86x)
        return __PM_biosFuncs.PM_int86x(intno,in,out,sregs);
    *out = *in;
    return out->e.eax;
}

#define OUTR(ereg) in->e.ereg = context.vm.regs.ereg

void PMAPI PM_callRealMode(
    uint seg,
    uint off,
    RMREGS *in,
    RMSREGS *sregs)
{
#ifdef __INTEL__
    if (!inited)
        PM_init();
    if (__PM_biosFuncs.PM_callRealMode)
        __PM_biosFuncs.PM_callRealMode(seg,off,in,sregs);
    if (_PM_haveBIOSAccess) {
        memset(&context.vm.regs, 0, sizeof(context.vm.regs));
        IN(eax); IN(ebx); IN(ecx); IN(edx); IN(esi); IN(edi);
        context.vm.regs.eflags = DEFAULT_VM86_FLAGS;
        context.vm.regs.cs = seg;
        context.vm.regs.eip = off;
        context.vm.regs.ss = context.stack_seg;
        context.vm.regs.esp = context.stack_off;
        context.vm.regs.es = sregs->es;
        context.vm.regs.ds = sregs->ds;
        context.vm.regs.fs = sregs->fs;
        context.vm.regs.gs = sregs->gs;
        pushw(DEFAULT_VM86_FLAGS);
        pushw(context.ret_seg);
        pushw(context.ret_off);
        run_vm86();
        OUTR(eax); OUTR(ebx); OUTR(ecx); OUTR(edx); OUTR(esi); OUTR(edi);
        sregs->es = context.vm.regs.es;
        sregs->ds = context.vm.regs.ds;
        sregs->fs = context.vm.regs.fs;
        sregs->gs = context.vm.regs.gs;
        in->x.cflag = context.vm.regs.eflags & 1;
        }
#endif
    if (__PM_biosFuncs.PM_callRealMode)
        __PM_biosFuncs.PM_callRealMode(seg,off,in,sregs);
}

/****************************************************************************
REMARKS:
Allocate locked physical memory via kernel module service.
****************************************************************************/
void * PMAPI PM_allocLockedMem(
    uint size,
    ulong *physAddr,
    ibool contiguous,
    ibool below16M)
{
    ulong   buf[6];
    ulong   physical;
    void*   linear;
    void*   kernel;

    if (!inited)
        PM_init();
    if (fd_pmhelp == -1)
        return NULL;
    // Memory must be page-aligned for mmap() to work
    size += PAGE_SIZE-1;
    size &= PAGE_MASK;
    buf[0] = (ulong)size;
    buf[1] = (ulong)physAddr;
    buf[2] = (ulong)contiguous;
    buf[3] = (ulong)below16M;
    buf[4] = 0;
    buf[5] = 0;
    ioctl(fd_pmhelp,PMHELP_ALLOCLOCKED,&buf);
    // I/O service cannot update user space physAddr param directly
    // nor make its virtual address accessible to user process
    kernel = (void*)buf[4];
    physical = (ulong)buf[5];
    if (kernel == NULL)
        return NULL;
    // Use driver mmap() callback for mapping physical memory to user space
    linear = mmap(kernel,size,PROT_READ | PROT_WRITE,MAP_SHARED,fd_pmhelp,physical);
    if (linear == (void*)-1) {
        perror("PM_allocLockedMem mmap");
        return NULL;
        }
    // Add pair of kernel and user virtual addresses to kernel mmap list
    _PM_addKernelAddr(kernel, linear);
    *physAddr = physical;
    return linear;
}

/****************************************************************************
REMARKS:
Free locked memory allocated above via kernel module service.
****************************************************************************/
void PMAPI PM_freeLockedMem(
    void *p,
    uint size,
    ibool contiguous)
{
    ulong buf[3];

    if (fd_pmhelp == -1)
        return;
    // Unmap user space mapping before releasing memory
    munmap(p,size);
    buf[0] = (ulong)_PM_getKernelAddr(p);
    buf[1] = (ulong)size;
    buf[2] = (ulong)contiguous;
    ioctl(fd_pmhelp,PMHELP_FREELOCKED,&buf);
    // Remove kernel and user addresses from kernel mmap list
    _PM_removeKernelAddr(p);
}

/****************************************************************************
REMARKS:
Allocate page of memory.
****************************************************************************/
void * PMAPI PM_allocPage(
    ibool locked)
{
    // TODO: Implement this for Linux
    return NULL;
}

/****************************************************************************
REMARKS:
Free memory page allocated above.
****************************************************************************/
void PMAPI PM_freePage(
    void *p)
{
    // TODO: Implement this for Linux
}

/****************************************************************************
REMARKS:
Enable MTRR write combining.
****************************************************************************/
int PMAPI PM_enableWriteCombine(ulong base,ulong length,uint type)
{
#ifdef ENABLE_MTRR
    struct mtrr_sentry sentry;

    if (fd_mtrr < 0)
        return PM_MTRR_ERR_NO_OS_SUPPORT;
    sentry.base = base;
    sentry.size = length;
    sentry.type = type;
    if (ioctl(fd_mtrr, MTRRIOC_ADD_ENTRY, &sentry) == -1) {
        // TODO: Need to decode MTRR error codes!!
        return PM_MTRR_NOT_SUPPORTED;
        }
    return PM_MTRR_ERR_OK;
#else
    return PM_MTRR_ERR_NO_OS_SUPPORT;
#endif
}

/****************************************************************************
PARAMETERS:
callback    - Function to callback with write combine information

REMARKS:
Function to enumerate all write combine regions currently enabled for the
processor.
****************************************************************************/
int PMAPI PM_enumWriteCombine(
    PM_enumWriteCombine_t callback)
{
#ifdef ENABLE_MTRR
    struct mtrr_gentry gentry;

    if (fd_mtrr < 0)
        return PM_MTRR_ERR_NO_OS_SUPPORT;
    for (gentry.regnum = 0; ioctl (fd_mtrr, MTRRIOC_GET_ENTRY, &gentry) == 0; ++gentry.regnum) {
        if (gentry.size > 0) {
            /* WARNING: This code assumes that the types in pmapi.h match the ones
             * in the Linux kernel (mtrr.h)
             */
            callback(gentry.base, gentry.size, gentry.type);
            }
        }
    return PM_MTRR_ERR_OK;
#else
    return PM_MTRR_ERR_NO_OS_SUPPORT;
#endif
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
    // TODO: Move the IOPL switching into this function!!
    return level;
}

void PMAPI PM_flushTLB(void)
{
    /* Do nothing on Linux */
}

/****************************************************************************
REMARKS:
Do nothing for this OS.
****************************************************************************/
ulong PMAPI PM_setMaxThreadPriority(void)
{
    // TODO: Implement this on Linux!
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
    /* TODO: It would be nice to use this function to determine if a Linux
     *       console app is running under X11 or not...
     */
    return false;
}

/****************************************************************************
REMARKS:
Read a machine status register for the CPU. Executes privileged instruction
RDMSR via ring-0 service.
****************************************************************************/
void _ASMAPI PM_readMSR(
    ulong reg,
    ulong *eax,
    ulong *edx)
{
    /* TODO: We will need this eventually to support Geode chips on Linux! */
    (void)reg;
    (void)eax;
    (void)edx;
}

/****************************************************************************
REMARKS:
Write a machine status register for the CPU. Executes privileged instruction
WRMSR via ring-0 service.
****************************************************************************/
void _ASMAPI PM_writeMSR(
    ulong reg,
    ulong eax,
    ulong edx)
{
    /* TODO: We will need this eventually to support Geode chips on Linux! */
    (void)reg;
    (void)eax;
    (void)edx;
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
    struct fb_var_screeninfo    var;

    /* Return false if we are not running a framebuffer console */
    if (fd_fbdev == -1) {
        if ((fd_fbdev = open(DEFAULT_FRAMEBUFFER, O_RDONLY)) == -1) {
            return false;
            }
        }

    /* Detect the console state */
    if (ioctl(fd_fbdev, FBIOGET_VSCREENINFO, &var)) {
        perror("ioctl(FBIOGET_VSCREENINFO) failed");
        return false;
        }

    *width = var.xres;
    *height = var.yres;
    *bitsPerPixel = var.bits_per_pixel;
    return true;
}

/****************************************************************************
REMARKS:
Function to set access protection flags on a block of memory.
****************************************************************************/
ibool PMAPI PM_memProtect(void *addr, size_t len, int flags)
{
    int mflags = 0;

    if (flags & PM_MPROT_READ)
        mflags |= PROT_READ;
    if (flags & PM_MPROT_WRITE)
        mflags |= PROT_WRITE;
    if (flags & PM_MPROT_EXEC)
        mflags |= PROT_EXEC;
    if (mprotect(addr, len, mflags) != 0 )
        return false;
    return true;
}
