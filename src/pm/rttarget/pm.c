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
* Environment:  RTTarget-32
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
#include <conio.h>
#include <stdlib.h>
#include <string.h>
#include <direct.h>
#define WIN32_LEAN_AND_MEAN
#define STRICT
#include <windows.h>
#ifdef  __BORLANDC__
#pragma warn -par
#endif
#ifdef  USE_RTT32DLL
#define RTT32DLL
#endif
#include <rttarget.h>
#ifdef  USE_RTKERNEL32
#include <rtkernel.h>
#include <rtkeybrd.h>
#endif
#ifdef  USE_RTFILES32
#include <rtfiles.h>
#endif

/*--------------------------- Global variables ----------------------------*/

static void             (PMAPIP fatalErrorCleanup)(void) = NULL;
static uchar *          lowMem = NULL;
static PM_intHandler    savedHandler = NULL;
static PM_intHandler    _PM_rtcHandler = NULL;
static uchar            _PM_oldCMOSRegA = 0;
static uchar            _PM_oldCMOSRegB = 0;
static ulong            PDB = 0;
static ulong *          pPDB = NULL;

/* Memory mapping cache */

#define MAX_MEMORY_MAPPINGS 1024    /* 4G address space / 4K page size = 1K page table entries */
typedef struct {
    ulong   physical;
    ulong   linear;
    ulong   limit;
    } mmapping;
static mmapping     maps[MAX_MEMORY_MAPPINGS] = {{0}};
static int          numMaps = 0;

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

/* External assembler functions */

ulong   _ASMAPI _PM_getPDB(void);
int     _ASMAPI _PM_pagingEnabled(void);

/* Use the RTOS-32 provided functions to access CMOS registers */

#define _PM_readCMOS    RTCMOSRead
#define _PM_writeCMOS   RTCMOSWrite

/* Optional RTFiles-32 filesystem support requires SDK-like Init module. */
#ifdef USE_RTFILES32
#include "init.c"
#endif

/****************************************************************************
REMARKS:
Initialize the RTTarget32 run-time startup. This optional callback function
must be called at startup time to be effective. It is called by PM_init()
only to ensure that the function is exported by the PM library. It typically
would be referenced by an "Init" statement in an RTTarget config file, so
a developer has the choice of using their own run-time startup Init()
function, like the one included with the RTTarget SDK's filesystem examples.
****************************************************************************/
#ifdef _MSC_VER
__declspec(dllexport) void _PM_initRTTargetStartup(void)
#else
void __export __cdecl _PM_initRTTargetStartup(void)
#endif
{
    /* Extend heap to full memory size reported by CMOS BIOS */
    RTCMOSExtendHeap();
}

/****************************************************************************
REMARKS:
Initialize the PM library.
****************************************************************************/
void PMAPI PM_init(void)
{
    static ibool    inited = false;

    /* Exit if we are already inited */
    if (inited)
        return;

    /* Set system time from CMOS clock */
    RTCMOSSetSystemTime();

#ifdef USE_RTFILES32
    /* RTFiles-32 SDK-like Init module called here in order to be exported. */
    Init();
#else
    /* Extend heap to full memory size reported by CMOS BIOS */
    _PM_initRTTargetStartup();
#endif

#ifdef USE_RTKERNEL32
    /* Start RTKernel services, such as keyboard handler */
    RTKernelInit(0);
    KBInit();
#endif

    /* Attempt to init Memory Type Range Registers on installed CPU */
    MTRR_init();

    /* Map the first Mb of physical memory into lowMem */
    if ((lowMem = PM_mapPhysicalAddr(0,0xFFFFF,true)) == NULL)
        PM_fatalError("Unable to map first Mb physical memory!");
    inited = true;
}

/****************************************************************************
REMARKS:
Return the operating system type identifier.
****************************************************************************/
long PMAPI PM_getOSType(void)
{
    return _OS_RTTARGET;
}

/****************************************************************************
REMARKS:
Return the name of the runtime environment.
****************************************************************************/
char * PMAPI PM_getOSName(void)
{
    return "RTTarget";
}

/****************************************************************************
REMARKS:
Return the runtime type identifier.
****************************************************************************/
int PMAPI PM_getModeType(void)
{
    return PM_386;
}

/****************************************************************************
REMARKS:
Add a file directory separator to the end of the filename.
****************************************************************************/
void PMAPI PM_backslash(
    char *s)
{
    uint pos = strlen(s);
    if (s[pos-1] != '\\') {
        s[pos] = '\\';
        s[pos+1] = '\0';
        }
}

/****************************************************************************
REMARKS:
Add a user defined PM_fatalError cleanup function.
****************************************************************************/
void PMAPI PM_setFatalErrorCleanup(
    void (PMAPIP cleanup)(void))
{
    fatalErrorCleanup = cleanup;
}

/****************************************************************************
REMARKS:
Report a fatal error condition and halt the program.
****************************************************************************/
void PMAPI PM_fatalError(
    const char *msg)
{
    if (fatalErrorCleanup)
        fatalErrorCleanup();
    RTDisplayString(msg);
    exit(1);
}

/****************************************************************************
REMARKS:
Allocate the real mode VESA transfer buffer for communicating with the BIOS.
****************************************************************************/
void * PMAPI PM_getVESABuf(
    uint *len,
    uint *rseg,
    uint *roff)
{
    /* No BIOS access for the RTTarget */
    return NULL;
}

/****************************************************************************
REMARKS:
Check if a key has been pressed.
****************************************************************************/
int PMAPI PM_kbhit(void)
{
    /* Need to check if a key is waiting on the keyboard queue */
    PM_init();
#ifdef USE_RTKERNEL32
    return KBKeyPressed();
#else
    return kbhit();
#endif
}

/****************************************************************************
REMARKS:
Wait for and return the next keypress.
****************************************************************************/
int PMAPI PM_getch(void)
{
    /* Need to obtain the next keypress, and block until one is hit */
    PM_init();
#ifdef USE_RTKERNEL32
    return KBGetCh();
#else
    return getch();
#endif
}

/****************************************************************************
REMARKS:
Set the location of the OS console cursor.
****************************************************************************/
void PM_setOSCursorLocation(
    int x,
    int y)
{
    /* Nothing to do for RTTarget-32 */
}

/****************************************************************************
REMARKS:
Set the width of the OS console.
****************************************************************************/
void PM_setOSScreenWidth(
    int width,
    int height)
{
    /* Nothing to do for RTTarget-32 */
}

/****************************************************************************
REMARKS:
Real time clock interrupt handler, which calls the user registered C code.
****************************************************************************/
static void RTTAPI RTCInt_Handler(void)
{
    /* Clear priority interrupt controller and re-enable interrupts so we
     * dont lock things up for long.
     */
    RTIRQEnd(8);

    /* Clear real-time clock timeout */
    _PM_readCMOS(0x0C);

    /* Now call the C based interrupt handler. We always call the user
     * code with interrupts *disabled* and they must remain off for the
     * duration of the handler.
     */
    _PM_rtcHandler();
}

/****************************************************************************
REMARKS:
Set the real time clock handler (used for software stereo modes).
****************************************************************************/
ibool PMAPI PM_setRealTimeClockHandler(
    PM_intHandler ih,
    int frequency)
{
    PM_init();

    /* Save the old CMOS real time clock values */
    _PM_oldCMOSRegA = _PM_readCMOS(0x0A);
    _PM_oldCMOSRegB = _PM_readCMOS(0x0B);

    /* Install real time clock handler on IRQ8 */
    _PM_rtcHandler = savedHandler = ih;
    RTInstallSharedIRQHandler(8,RTCInt_Handler);

    /* Program the real time clock default frequency */
    PM_setRealTimeClockFrequency(frequency);

    /* Unmask IRQ8 in the PIC */
    RTEnableIRQ(8);
    return true;
}

/****************************************************************************
REMARKS:
Set the real time clock frequency (for stereo modes).
****************************************************************************/
void PMAPI PM_setRealTimeClockFrequency(
    int frequency)
{
    static short convert[] = {
        8192,
        4096,
        2048,
        1024,
        512,
        256,
        128,
        64,
        32,
        16,
        8,
        4,
        2,
        -1,
        };
    int i;

    /* First clear any pending RTC timeout if not cleared */
    _PM_readCMOS(0x0C);
    if (frequency == 0) {
        /* Disable RTC timout */
        _PM_writeCMOS(0x0A,_PM_oldCMOSRegA);
        _PM_writeCMOS(0x0B,_PM_oldCMOSRegB & 0x0F);
        }
    else {
        /* Convert frequency value to RTC clock indexes */
        for (i = 0; convert[i] != -1; i++) {
            if (convert[i] == frequency)
                break;
            }

        /* Set RTC timout value and enable timeout */
        _PM_writeCMOS(0x0A,0x20 | (i+3));
        _PM_writeCMOS(0x0B,(_PM_oldCMOSRegB & 0x0F) | 0x40);
        }
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

/****************************************************************************
REMARKS:
Restore the original real time clock handler.
****************************************************************************/
void PMAPI PM_restoreRealTimeClockHandler(void)
{
    /* Restore real time clock handler on IRQ8 */
    if (_PM_rtcHandler) {
        RTRemoveSharedIRQHandler(8,RTCInt_Handler);
        _PM_rtcHandler = NULL;

        /* Restore CMOS registers and mask RTC clock */
        _PM_writeCMOS(0x0A,_PM_oldCMOSRegA);
        _PM_writeCMOS(0x0B,_PM_oldCMOSRegB);
        }
}

/****************************************************************************
REMARKS:
Return the current operating system path or working directory.
****************************************************************************/
char * PMAPI PM_getCurrentPath(
    char *path,
    int maxLen)
{
    return getcwd(path,maxLen);
}

/****************************************************************************
REMARKS:
Return the drive letter for the boot drive.
****************************************************************************/
char PMAPI PM_getBootDrive(void)
{
    return 'c';
}

/****************************************************************************
REMARKS:
Return the path to the SNAP driver files.
****************************************************************************/
const char * PMAPI PM_getSNAPPath(void)
{
    static char env[256];

    /* Possible to set environment vars in RTTarget config file */
    if (GetEnvironmentVariable("SNAP_PATH",env,sizeof(env)))
        return env;
    return "c:\\snap";
}

/****************************************************************************
REMARKS:
Return the path to the SNAP configuration files.
****************************************************************************/
const char * PMAPI PM_getSNAPConfigPath(void)
{
    static char path[256], env[256];

    if (GetEnvironmentVariable("SNAP_CONFIG_PATH",env,sizeof(env)))
        return env;
    strcpy(path,PM_getSNAPPath());
    PM_backslash(path);
    strcat(path,"config");
    return path;
}

/****************************************************************************
REMARKS:
Return a unique identifier for the machine if possible.
****************************************************************************/
const char * PMAPI PM_getUniqueID(void)
{
    return PM_getMachineName();
}

/****************************************************************************
REMARKS:
Get the name of the machine on the network.
****************************************************************************/
const char * PMAPI PM_getMachineName(void)
{
    /* Not necessary for RTTarget-32 */
    return "UNKNOWN";
}

/****************************************************************************
REMARKS:
Return a pointer to the real mode BIOS data area.
****************************************************************************/
void * PMAPI PM_getBIOSPointer(void)
{
    PM_init();
    return lowMem + 0x400;
}

/****************************************************************************
REMARKS:
Return a pointer to 0xA0000 physical VGA graphics framebuffer.
****************************************************************************/
void * PMAPI PM_getA0000Pointer(void)
{
    PM_init();
    return lowMem + 0xA0000;
}

/****************************************************************************
REMARKS:
Maps a range of physical memory (DPMI function 0x0800).
****************************************************************************/
static ulong PMAPI _PM_mapPhysicalToLinear(
    ulong physAddr,
    ulong limit)
{
    int     i;
    ulong   baseAddr,baseOfs,roundedLimit;
    ulong   linear;

    /* Search table of existing mappings to see if we have already mapped
     * a region of memory that will serve this purpose. We do this because
     * DPMI 0.9 does not allow us to free physical memory mappings, and if
     * the mappings get re-used in the program we want to avoid allocating
     * more mappings than necessary.
     */
    for (i = 0; i < numMaps; i++) {
        if (maps[i].physical == physAddr && maps[i].limit == limit)
            return maps[i].linear;
        }

    /* Find a free slot in our physical memory mapping table */
    for (i = 0; i < numMaps; i++) {
        if (maps[i].limit == 0)
            break;
        }
    if (i == numMaps) {
        i = numMaps++;
        if (i == MAX_MEMORY_MAPPINGS)
            return NULL;
        }

    /* Round the physical address to a 4Kb boundary and the limit to a
     * 4Kb-1 boundary before passing the values to DPMI as some extenders
     * will fail the calls unless this is the case. If we round the
     * physical address, then we also add an extra offset into the address
     * that we return.
     */
    baseOfs = physAddr & 4095;
    baseAddr = physAddr & ~4095;
    roundedLimit = ((limit+baseOfs+1+4095) & ~4095)-1;

    /* Get some address space for the rounded 4K boundary */
    linear = baseAddr;
    if (RTReserveVirtualAddress((void**)&linear, roundedLimit, 0) != RT_MAP_SUCCESS)
        return 0xFFFFFFFFUL;

    /* Map the memory */
    if (RTMapMem((void*)baseAddr, (void*)linear, roundedLimit, RT_PG_USERREADWRITE) != RT_MAP_SUCCESS)
        return 0xFFFFFFFFUL;

    /* Save the values and return the mapped address */
    maps[i].physical = physAddr;
    maps[i].limit = limit;
    maps[i].linear = linear + baseOfs;
    return maps[i].linear;
}

/****************************************************************************
REMARKS:
Adjust the page table caching bits directly. Requires ring 0 access which
is handled on RTTarget-32 via callgates. (Based on DOS version PM library.)
****************************************************************************/
static void PM_adjustPageTables(
    ulong linear,
    ulong limit,
    ibool isCached)
{
    int     startPDB,endPDB,iPDB,startPage,endPage,start,end,iPage;
    ulong   andMask,orMask,pageTable,*pPageTable;

    /* Using the page table directory base address obtained via ring-0
     * callgate, map each of the page table entries so we can access them
     * directly at ring-3.
     */
    andMask = ~0x18;
    orMask = (isCached) ? 0x00 : 0x18;
    if (_PM_pagingEnabled() == 1 && (PDB = _PM_getPDB()) != 0) {
        /* 4K page size * 1K page table entries = 4G address space */
        pPDB = (ulong*)_PM_mapPhysicalToLinear(PDB,0xFFF); /* 4K-1 */
        if (pPDB && (ulong)pPDB != 0xFFFFFFFFUL) {
            startPDB = (linear >> 22) & 0x3FF;              /* 1K-1 */
            startPage = (linear >> 12) & 0x3FF;
            endPDB = ((linear+limit) >> 22) & 0x3FF;
            endPage = ((linear+limit) >> 12) & 0x3FF;
            for (iPDB = startPDB; iPDB <= endPDB; iPDB++) {
                pageTable = pPDB[iPDB] & ~0xFFF;
                pPageTable = (ulong*)_PM_mapPhysicalToLinear(pageTable,0xFFF);
                start = (iPDB == startPDB) ? startPage : 0;
                end = (iPDB == endPDB) ? endPage : 0x3FF;
                for (iPage = start; iPage <= end; iPage++)
                    pPageTable[iPage] = (pPageTable[iPage] & andMask) | orMask;
                }
            }
        PM_flushTLB();
        }
}

/****************************************************************************
REMARKS:
Map a physical address to a linear address in the callers process.
****************************************************************************/
void * PMAPI PM_mapPhysicalAddr(
    ulong base,
    ulong limit,
    ibool isCached)
{
    ulong   linear;

    /* Map the memory */
    if ((linear = _PM_mapPhysicalToLinear(base,limit)) == 0xFFFFFFFF)
        return NULL;

    /* Adjust the page tables for the memory mapping we just created */
    PM_adjustPageTables((ulong)linear, limit, isCached);
    return (void*)linear;
}

/****************************************************************************
REMARKS:
Free a physical address mapping allocated by PM_mapPhysicalAddr.
****************************************************************************/
void PMAPI PM_freePhysicalAddr(
    void *ptr,
    ulong limit)
{
    RTReleaseVirtualAddress(ptr, limit);
}

/****************************************************************************
REMARKS:
Find the physical address of a linear memory address in current process.
****************************************************************************/
ulong PMAPI PM_getPhysicalAddr(
    void *p)
{
    return (ulong)RTGetPhysAddress(p);
}

/****************************************************************************
REMARKS:
Find the physical address of a linear memory address in current process.
****************************************************************************/
ibool PMAPI PM_getPhysicalAddrRange(
    void *p,
    ulong length,
    ulong *physAddress)
{
    int     i;
    ulong   linear = (ulong)p & ~0xFFF;

    for (i = (length + 0xFFF) >> 12; i > 0; i--) {
        if ((*physAddress++ = PM_getPhysicalAddr((void*)linear)) == 0xFFFFFFFF)
            return false;
        linear += 4096;
        }
    return true;
}

/****************************************************************************
REMARKS:
Sleep for the specified number of milliseconds.
****************************************************************************/
void PMAPI PM_sleep(
    ulong milliseconds)
{
    Sleep(milliseconds);
}

/****************************************************************************
REMARKS:
Return the base I/O port for the specified COM port.
****************************************************************************/
int PMAPI PM_getCOMPort(
    int port)
{
    switch (port) {
        case 0: return 0x3F8;
        case 1: return 0x2F8;
        }
    return 0;
}

/****************************************************************************
REMARKS:
Return the base I/O port for the specified LPT port.
****************************************************************************/
int PMAPI PM_getLPTPort(
    int port)
{
    switch (port) {
        case 0: return 0x3BC;
        case 1: return 0x378;
        case 2: return 0x278;
        }
    return 0;
}

/****************************************************************************
REMARKS:
Allocate a block of shared memory.
****************************************************************************/
void * PMAPI PM_mallocShared(
    long size)
{
    /* Until we need shared memory on RTTarget, this function should fail */
    return NULL;
}

/****************************************************************************
REMARKS:
Free a block of shared memory.
****************************************************************************/
void PMAPI PM_freeShared(
    void *ptr)
{
}

/****************************************************************************
REMARKS:
Map a real mode pointer to a protected mode pointer.
****************************************************************************/
void * PMAPI PM_mapRealPointer(
    uint r_seg,
    uint r_off)
{
    return lowMem + MK_PHYS(r_seg,r_off);
}

/****************************************************************************
REMARKS:
Allocate a block of real mode memory
****************************************************************************/
void * PMAPI PM_allocRealSeg(
    uint size,
    uint *r_seg,
    uint *r_off)
{
    /* No real mode access under RTTarget-32 */
    return NULL;
}

/****************************************************************************
REMARKS:
Free a block of real mode memory.
****************************************************************************/
void PMAPI PM_freeRealSeg(
    void *mem)
{
    /* No real mode access under RTTarget-32 */
}

/****************************************************************************
REMARKS:
Issue a real mode interrupt.
****************************************************************************/
int PMAPI PM_int86(
    int intno,
    RMREGS *in,
    RMREGS *out)
{
    /* No real mode access under RTTarget-32 */
    return 0;
}

/****************************************************************************
REMARKS:
Issue a real mode interrupt.
****************************************************************************/
int PMAPI PM_int86x(
    int intno,
    RMREGS *in,
    RMREGS *out,
    RMSREGS *sregs)
{
    /* No real mode access under RTTarget-32 */
    return 0;
}

/****************************************************************************
REMARKS:
Call a real mode far function.
****************************************************************************/
void PMAPI PM_callRealMode(
    uint seg,
    uint off,
    RMREGS *in,
    RMSREGS *sregs)
{
    /* No real mode access under RTTarget-32 */
}

/****************************************************************************
REMARKS:
Allocate a block of locked, physical memory for DMA operations.
****************************************************************************/
void * PMAPI PM_allocLockedMem(
    uint size,
    ulong *physAddr,
    ibool contiguous,
    ibool below16M)
{
    ulong physical;
    ulong linear;

    /* Allocated physical memory is always contiguous on RTTarget. */
    if ((physical = (ulong)RTAllocPhysPageAligned(size)) == NULL)
        return NULL;
    if ((linear = _PM_mapPhysicalToLinear(physical,size)) == 0xFFFFFFFF)
        return NULL;
    *physAddr = physical;
    return (void*)linear;
}

/****************************************************************************
REMARKS:
Free a block of locked physical memory.
****************************************************************************/
void PMAPI PM_freeLockedMem(
    void *p,
    uint size,
    ibool contiguous)
{
    /* Allocated physical memory cannot be released on RTTarget. */
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

    /* Scan the block list looking for any free blocks. Allocate a new
     * page block if no free blocks are found.
     */
    for (block = pageBlocks; block != NULL; block = block->next) {
        if (block->freeCount)
            break;
        }
    if (block == NULL && (block = PM_addNewPageBlock()) == NULL)
        return NULL;
    block->freeCount--;
    p = block->freeList;
    block->freeList = FREELIST_NEXT(p);

    /* Memory will never be paged to disk under RTTarget */
    (void)locked;
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

/****************************************************************************
REMARKS:
Return true if we have real BIOS access, but since we do not under RTTarget,
we return false.
****************************************************************************/
ibool PMAPI PM_haveBIOSAccess(void)
{
    return false;
}

/****************************************************************************
DESCRIPTION:
Load an OS specific shared library or DLL.

HEADER:
pmapi.h

PARAMETERS:
szDLLName   - Name of the OS specific library to load

RETURNS:
Pointer to the loaded module handle, NULL on failure.

REMARKS:
This function is used to load an operating system specific shared library
or DLL. This is mostly used by binary portable code that needs to directly
interface to operating system specific shared library code.

If the OS does not support shared libraries, this function simply returns
NULL.

SEE ALSO:
PM_getProcAddress, PM_freeLibrary
****************************************************************************/
PM_MODULE PMAPI PM_loadLibrary(
    const char *szDLLName)
{
    return (PM_MODULE)LoadLibrary(szDLLName);
}

/****************************************************************************
DESCRIPTION:
Get the address of a named procedure from a shared library.

HEADER:
pmapi.h

PARAMETERS:
hModule     - Handle to the module to get procedure from
szProcName  - Name of the procedure to get address of

RETURNS:
Pointer to the start of the function in the shared library

REMARKS:
This function is used to get the address of a named function in a shared
library that was loaded with the PM_loadLibrary.

SEE ALSO:
PM_loadLibrary, PM_freeLibrary
****************************************************************************/
void * PMAPI PM_getProcAddress(
    PM_MODULE hModule,
    const char *szProcName)
{
    return (void*)GetProcAddress((HINSTANCE)hModule,szProcName);
}

/****************************************************************************
DESCRIPTION:
Unload a shared library.

HEADER:
pmapi.h

PARAMETERS:
hModule     - Handle to the module to unload

REMARKS:
This function is used to unload a shared library previously loaded with the
PM_loadLibrary function.

SEE ALSO:
PM_loadLibrary, PM_getProcAddress
****************************************************************************/
void PMAPI PM_freeLibrary(
    PM_MODULE hModule)
{
    FreeLibrary((HINSTANCE)hModule);
}

/****************************************************************************
REMARKS:
Internal function to convert the find data to the generic interface.
****************************************************************************/
static void convertFindData(
    PM_findData *findData,
    WIN32_FIND_DATA *blk)
{
    ulong   dwSize = findData->dwSize;

    memset(findData,0,findData->dwSize);
    findData->dwSize = dwSize;
    if (blk->dwFileAttributes & FILE_ATTRIBUTE_READONLY)
        findData->attrib |= PM_FILE_READONLY;
    if (blk->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        findData->attrib |= PM_FILE_DIRECTORY;
    if (blk->dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE)
        findData->attrib |= PM_FILE_ARCHIVE;
    if (blk->dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)
        findData->attrib |= PM_FILE_HIDDEN;
    if (blk->dwFileAttributes & FILE_ATTRIBUTE_SYSTEM)
        findData->attrib |= PM_FILE_SYSTEM;
    findData->sizeLo = blk->nFileSizeLow;
    findData->sizeHi = blk->nFileSizeHigh;
    strncpy(findData->name,blk->cFileName,PM_MAX_PATH);
    findData->name[PM_MAX_PATH-1] = 0;
}

/****************************************************************************
DESCRIPTION:
Function to find the first file matching a search criteria in a directory.

HEADER:
pmapi.h

PARAMETERS:
filename    - Filename mask to see the search with
findData    - Place to return the found file data

RETURNS:
Pointer to the find handle created, PM_FILE_INVALID if no more files.

REMARKS:
This function is used to find the first file matching a search criteria in a
directory. Once you have found the first file, you can then call
PM_findNextFile to find the next file matching the same search criteria.
When you are done, make sure you call PM_findClose to free the handle
returned by this function.

SEE ALSO:
PM_findNextFile, PM_findClose
****************************************************************************/
void * PMAPI PM_findFirstFile(
    const char *filename,
    PM_findData *findData)
{
    WIN32_FIND_DATA blk;
    HANDLE          hfile;

    if ((hfile = FindFirstFile(filename,&blk)) != INVALID_HANDLE_VALUE) {
        convertFindData(findData,&blk);
        return (void*)hfile;
        }
    return PM_FILE_INVALID;
}

/****************************************************************************
DESCRIPTION:
Function to find the next file matching a search criteria in a directory.

HEADER:
pmapi.h

PARAMETERS:
handle      - Handle return from PM_findFirstFile
findData    - Place to return the found file data

RETURNS:
True if another file is found, false if not.

REMARKS:
This function is used to find the next file matching the same search criteria
passed to PM_findFirstFile. You can keep calling PM_findNextFile to find
each file that matches until this function returns false, indicating there
are no more files that match. When you are done, make sure you call
PM_findClose to free the handle returned by the PM_findFirstFile function.

SEE ALSO:
PM_findFirstFile, PM_findClose
****************************************************************************/
ibool PMAPI PM_findNextFile(
    void *handle,
    PM_findData *findData)
{
    WIN32_FIND_DATA blk;

    if (FindNextFile((HANDLE)handle,&blk)) {
        convertFindData(findData,&blk);
        return true;
        }
    return false;
}

/****************************************************************************
DESCRIPTION:
Function to close the find process

HEADER:
pmapi.h

PARAMETERS:
handle      - Handle return from PM_findFirstFile

REMARKS:
This function is used to close the search handle returned by the
PM_findFirstFile function.

SEE ALSO:
PM_findFirstFile, PM_findNextFile
****************************************************************************/
void PMAPI PM_findClose(
    void *handle)
{
    FindClose((HANDLE)handle);
}

/****************************************************************************
DESCRIPTION:
Function to change the file attributes for a specific file.

HEADER:
pmapi.h

PARAMETERS:
filename    - Full path to filename for file to change
attrib      - New attributes for the file (PMFileFlagsType)

REMARKS:
This function is used to file attributes for a specific file to the values
passed in the attribute parameter (a combination of flags defined in
PMFileFlagsType). Under Unix system some of these flags are igonred,
such as the hidden and system attributes.

SEE ALSO:
PM_getFileAttr
****************************************************************************/
void PMAPI PM_setFileAttr(
    const char *filename,
    uint attrib)
{
    DWORD attr = 0;

    if (attrib & PM_FILE_READONLY)
        attr |= FILE_ATTRIBUTE_READONLY;
    if (attrib & PM_FILE_ARCHIVE)
        attr |= FILE_ATTRIBUTE_ARCHIVE;
    if (attrib & PM_FILE_HIDDEN)
        attr |= FILE_ATTRIBUTE_HIDDEN;
    if (attrib & PM_FILE_SYSTEM)
        attr |= FILE_ATTRIBUTE_SYSTEM;
    SetFileAttributes((LPSTR)filename, attr);
}

/****************************************************************************
DESCRIPTION:
Function to get the file attributes for a specific file.

HEADER:
pmapi.h

PARAMETERS:
filename    - Full path to filename for file to get attributes from

RETURNS:
Current attributes for the file (PMFileFlagsType)

REMARKS:
This function is used to retrieve the current file attributes for a specific
file.

SEE ALSO:
PM_setFileAttr
****************************************************************************/
uint PMAPI PM_getFileAttr(
    const char *filename)
{
    DWORD   attr = GetFileAttributes(filename);
    uint    attrib = 0;

    if (attr & FILE_ATTRIBUTE_READONLY)
        attrib |= PM_FILE_READONLY;
    if (attr & FILE_ATTRIBUTE_ARCHIVE)
        attrib |= PM_FILE_ARCHIVE;
    if (attr & FILE_ATTRIBUTE_HIDDEN)
        attrib |= PM_FILE_HIDDEN;
    if (attr & FILE_ATTRIBUTE_SYSTEM)
        attrib |= PM_FILE_SYSTEM;
    return attrib;
}

/****************************************************************************
DESCRIPTION:
Function to get the file time and date for a specific file.

HEADER:
pmapi.h

PARAMETERS:
filename    - Full path to filename for file to get date and time from
gmTime      - True if time should be in the GMT timezone
time        - Place to store the file time for the file

RETURNS:
True on success, false on failure.

REMARKS:
This function is used to obtain the file date and time stamp for a specific
file. If the gmTime parameter is true, the time is returned in the GMT
time zone, otherwise it is in the local machine time zone.

SEE ALSO:
PM_setFileTime
****************************************************************************/
ibool PMAPI PM_getFileTime(
    const char *filename,
    ibool gmTime,
    PM_time *time)
{
    HANDLE      f;
    OFSTRUCT    of;
    FILETIME    utcTime,localTime;
    SYSTEMTIME  sysTime;
    ibool       status = false;

    of.cBytes = sizeof(of);
    if ((f = CreateFile(filename,GENERIC_READ,0,NULL,OPEN_EXISTING,0,NULL)) == INVALID_HANDLE_VALUE)
        return false;
    if (!GetFileTime((HANDLE)f,NULL,NULL,&utcTime))
        goto Exit;
    if (!gmTime) {
        if (!FileTimeToLocalFileTime(&utcTime,&localTime))
            goto Exit;
        }
    else
        localTime = utcTime;
    if (!FileTimeToSystemTime(&localTime,&sysTime))
        goto Exit;
    time->year = sysTime.wYear;
    time->mon = sysTime.wMonth-1;
    time->day = sysTime.wYear;
    time->hour = sysTime.wHour;
    time->min = sysTime.wMinute;
    time->sec = sysTime.wSecond;
    status = true;

Exit:
    CloseHandle((HANDLE)f);
    return status;
}

/****************************************************************************
DESCRIPTION:
Function to set the file time and date for a specific file.

HEADER:
pmapi.h

PARAMETERS:
filename    - Full path to filename for file to set date and time for
gmTime      - True if time should be in the GMT timezone
time        - Time to set for the file

RETURNS:
True on success, false on failure.

REMARKS:
This function is used to set the file date and time stamp for a specific
file. If the gmTime parameter is true, the time passed in should be in

SEE ALSO:
PM_getFileTime
****************************************************************************/
ibool PMAPI PM_setFileTime(
    const char *filename,
    ibool gmTime,
    PM_time *time)
{
    HANDLE      f;
    OFSTRUCT    of;
    FILETIME    utcTime,localTime;
    SYSTEMTIME  sysTime;
    ibool       status = false;

    of.cBytes = sizeof(of);
    if ((f = CreateFile(filename,GENERIC_WRITE,0,NULL,OPEN_EXISTING,0,NULL)) == INVALID_HANDLE_VALUE)
        return false;
    sysTime.wYear = time->year;
    sysTime.wMonth = time->mon+1;
    sysTime.wYear = time->day;
    sysTime.wHour = time->hour;
    sysTime.wMinute = time->min;
    sysTime.wSecond = time->sec;
    if (!SystemTimeToFileTime(&sysTime,&localTime))
        goto Exit;
    if (!gmTime) {
        if (!LocalFileTimeToFileTime(&localTime,&utcTime))
            goto Exit;
        }
    else
        utcTime = localTime;
    if (!SetFileTime((HANDLE)f,NULL,NULL,&utcTime))
        goto Exit;
    status = true;

Exit:
    CloseHandle((HANDLE)f);
    return status;
}

/****************************************************************************
REMARKS:
Function to get the current working directory for the specififed drive.
Under Unix this will always return the current working directory regardless
of what the value of 'drive' is.
****************************************************************************/
void PMAPI PM_getdcwd(
    int drive,
    char *dir,
    int len)
{
    (void)drive;
    getcwd(dir,len);
}

/****************************************************************************
REMARKS:
Function to create a directory.
****************************************************************************/
ibool PMAPI PM_mkdir(
    const char *filename)
{
    return mkdir(filename) == 0;
}

/****************************************************************************
REMARKS:
Function to remove a directory.
****************************************************************************/
ibool PMAPI PM_rmdir(
    const char *filename)
{
    return rmdir(filename) == 0;
}

/****************************************************************************
REMARKS:
Increase the thread priority to maximum, if possible.
****************************************************************************/
ulong PMAPI PM_setMaxThreadPriority(void)
{
    /* No need for this under RTTarget, since no other processes will be
     * running.
     */
    return 0;
}

/****************************************************************************
REMARKS:
Restore the original thread priority.
****************************************************************************/
void PMAPI PM_restoreThreadPriority(
    ulong oldPriority)
{
    /* No need for this under RTTarget, since no other processes will be
     * running.
     */
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
Set the IOPL level to enable I/O port access. Since we always have IOPL on
RTTarget, this function doesn't have to do anything.
****************************************************************************/
int PMAPI PM_setIOPL(
    int level)
{
    return level;
}

/****************************************************************************
REMARKS:
Open a fullscreen console for output to the screen. This requires that
the application be a fullscreen VIO program.
****************************************************************************/
PM_HWND PMAPI PM_openConsole(
    PM_HWND hwndUser,
    int device,
    int xRes,
    int yRes,
    int bpp,
    ibool fullScreen)
{
    /* Not need for RTTarget since we are the only app */
    (void)hwndUser;
    (void)device;
    (void)xRes;
    (void)yRes;
    (void)bpp;
    (void)fullScreen;
    return 0;
}

/****************************************************************************
REMARKS:
Find the size of the console state buffer.
****************************************************************************/
int PMAPI PM_getConsoleStateSize(void)
{
    /* Not need for RTTarget since we are the only app */
    return 1;
}

/****************************************************************************
REMARKS:
Save the state of the console.
****************************************************************************/
void PMAPI PM_saveConsoleState(
    void *stateBuf,
    PM_HWND hwndConsole)
{
    /* Not need for RTTarget since we are the only app */
    (void)stateBuf;
    (void)hwndConsole;
}

/****************************************************************************
REMARKS:
Set the suspend application callback for the fullscreen console.
****************************************************************************/
void PMAPI PM_setSuspendAppCallback(
    PM_suspendApp_cb saveState)
{
    /* Not need for RTTarget since we are the only app */
    (void)saveState;
}

/****************************************************************************
REMARKS:
Restore the console state.
****************************************************************************/
void PMAPI PM_restoreConsoleState(
    const void *stateBuf,
    PM_HWND hwndConsole)
{
    /* Not need for RTTarget since we are the only app */
    (void)stateBuf;
    (void)hwndConsole;
}

/****************************************************************************
REMARKS:
Close the fullscreen console.
****************************************************************************/
void PMAPI PM_closeConsole(
    PM_HWND hwndConsole)
{
    /* Not need for RTTarget since we are the only app */
    (void)hwndConsole;
}

/****************************************************************************
REMARKS:
Enumertae all the write combine regions defined for the processor
****************************************************************************/
int PMAPI PM_enumWriteCombine(
    PM_enumWriteCombine_t callback)
{
    /* Enumerate write combining for RTTarget */
    return MTRR_enumWriteCombine(callback);
}

/****************************************************************************
REMARKS:
Enable write combining for a specific memory region
****************************************************************************/
int PMAPI PM_enableWriteCombine(
    ulong base,
    ulong length,
    uint type)
{
    /* Enable write combining for RTTarget */
    return MTRR_enableWriteCombine(base, length, type);
}

/****************************************************************************
REMARKS:
Lock data memory so it will not be paged to disk
****************************************************************************/
int PMAPI PM_lockDataPages(
    void *p,
    uint len,
    PM_lockHandle *lockHandle)
{
    /* Memory is never paged to disk under RTTarget */
    (void)p;
    (void)len;
    (void)lockHandle;
    return 1;
}

/****************************************************************************
REMARKS:
Unlock data memory
****************************************************************************/
int PMAPI PM_unlockDataPages(
    void *p,
    uint len,
    PM_lockHandle *lockHandle)
{
    /* Memory is never paged to disk under RTTarget */
    (void)p;
    (void)len;
    (void)lockHandle;
    return 1;
}

/****************************************************************************
REMARKS:
Lock executeable code so it will not be paged to disk
****************************************************************************/
int PMAPI PM_lockCodePages(
    __codePtr p,
    uint len,
    PM_lockHandle *lockHandle)
{
    /* Memory is never paged to disk under RTTarget */
    (void)p;
    (void)len;
    (void)lockHandle;
    return 1;
}

/****************************************************************************
REMARKS:
Unlock code memory
****************************************************************************/
int PMAPI PM_unlockCodePages(
    __codePtr p,
    uint len,
    PM_lockHandle *lockHandle)
{
    /* Memory is never paged to disk under RTTarget */
    (void)p;
    (void)len;
    (void)lockHandle;
    return 1;
}

/****************************************************************************
REMARKS:
Install a break keyboard handler.
****************************************************************************/
void PMAPI PM_installBreakHandler(void)
{
    /* Nothing to do on RTTarget32 */
}

/****************************************************************************
REMARKS:
Restore the break keyboard handler.
****************************************************************************/
void PMAPI PM_restoreBreakHandler(void)
{
    /* Nothing to do on RTTarget32 */
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
    (void)subsystem;
}

