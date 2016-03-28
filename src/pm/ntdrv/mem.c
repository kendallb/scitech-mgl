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
* Environment:  32-bit Windows NT device drivers.
*
* Description:  Implementation for the NT driver memory management functions
*               for the PM library.
*
****************************************************************************/

#include "pmapi.h"
#include "clib/os/os.h"
#include "mtrr.h"
#include "oshdr.h"

/*--------------------------- Global variables ----------------------------*/

// Memory mapping cache

typedef struct {
    ulong   physical;
    void    *linear;
    ulong   length;
    ibool   isCached;
    int     refCount;
    } mmapping;

// Shared memory block cache

typedef struct {
    void    *linear;
    ulong   length;
    ibool   contiguous;
    } memblock;

static mmapping     *maps = NULL;
static int          mapsBlockSize = PAGE_SIZE / sizeof(mmapping);
static int          maxMaps = 0;
static int          numMaps = 0;
static memblock     *memblocks = NULL;
static int          memBlockSize = PAGE_SIZE / sizeof(memblock);
static int          maxMemBlocks = 0;
static int          numMemBlocks = 0;
static ulong        *pPDB;
ibool               _PM_globalAddressMapping = false;

/*----------------------------- Implementation ----------------------------*/

ulong   PMAPI _PM_getPDB(void);

// Page table entry flags

#define PAGE_FLAGS_PRESENT          0x00000001
#define PAGE_FLAGS_WRITEABLE        0x00000002
#define PAGE_FLAGS_USER             0x00000004
#define PAGE_FLAGS_WRITE_THROUGH    0x00000008
#define PAGE_FLAGS_CACHE_DISABLE    0x00000010
#define PAGE_FLAGS_ACCESSED         0x00000020
#define PAGE_FLAGS_DIRTY            0x00000040
#define PAGE_FLAGS_4MB              0x00000080
#define PAGE_LIMIT                  (PAGE_SIZE-1)
#define INVALID_MEMORY_MAPPING      ((void*)0xFFFFFFFF)

/****************************************************************************
PARAMETERS:
base        - Physical base address of the memory to maps in
limit       - Limit of physical memory to region to maps in

RETURNS:
Linear address of the newly mapped memory.

REMARKS:
Maps a physical memory range to a linear memory range.
****************************************************************************/
static void *_PM_mapPhysicalToLinear(
    ulong base,
    ulong limit,
    ibool isCached)
{
    ulong               length = limit+1;
    ulong               baseAddr,baseOfs,roundedLength;
    PHYSICAL_ADDRESS    paIoBase = {0};
    char                *linear;

    // Round the physical address to a 4Kb boundary to ensure that we
    // only request to map 4K pages from the kernel.
    baseOfs = base & 4095;
    baseAddr = base & ~4095;
    roundedLength = (length+baseOfs+4095) & ~4095;

    // Map IO space into Kernel
    paIoBase = RtlConvertUlongToLargeInteger(baseAddr);
    linear = MmMapIoSpace(paIoBase, roundedLength, isCached ? MmCached : MmNonCached);
    return linear + baseOfs;
}

/****************************************************************************
REMARKS:
This function sets up for accessing the page tables directly.
****************************************************************************/
static void _PM_beginPageTables(void)
{
    pPDB = _PM_mapPhysicalToLinear(_PM_getPDB(),PAGE_LIMIT,false);
    if (!pPDB)
        PM_fatalError("Unable to map page table directory!!");
}

/****************************************************************************
REMARKS:
This function cleans up after accessing the page tables directly.
****************************************************************************/
static void _PM_endPageTables(void)
{
    if (pPDB) {
        PM_flushTLB();
        MmUnmapIoSpace(pPDB,PAGE_SIZE);
        pPDB = NULL;
        }
}

/****************************************************************************
REMARKS:
Adjust the page table caching bits directly. Requires ring 0 access and
only works with DOS4GW and compatible extenders (CauseWay also works since
it has direct support for the ring 0 instructions we need from ring 3). Will
not work in a DOS box, but we call into the ring 0 helper VxD so we should
never get here in a DOS box anyway (assuming the VxD is present). If we
do get here and we are in windows, this code will be skipped.
****************************************************************************/
static void _PM_adjustPageTables(
    ulong linear,
    ulong limit,
    ibool isGlobal,
    ibool isCached)
{
    int     startPDB,endPDB,iPDB,startPage,endPage,start,end,iPage;
    ulong   pageTable,*pPageTable;
    ulong   mask = 0xFFFFFFFF;
    ulong   bits = 0x00000000;

    // Enable user level access for page table entry
    if (isGlobal) {
        mask &= ~PAGE_FLAGS_USER;
        bits |= PAGE_FLAGS_USER;
        }

    // Disable PCD bit if page table entry should be uncached
    if (!isCached) {
        mask &= ~(PAGE_FLAGS_CACHE_DISABLE | PAGE_FLAGS_WRITE_THROUGH);
        bits |= (PAGE_FLAGS_CACHE_DISABLE | PAGE_FLAGS_WRITE_THROUGH);
        }

    if (pPDB) {
        startPDB = (linear >> 22) & 0x3FF;
        startPage = (linear >> 12) & 0x3FF;
        endPDB = ((linear+limit) >> 22) & 0x3FF;
        endPage = ((linear+limit) >> 12) & 0x3FF;
        for (iPDB = startPDB; iPDB <= endPDB; iPDB++) {
            // Set the bits in the page directory entry - required as per
            // Pentium 4 manual. This also takes care of the 4MB page entries
            pPDB[iPDB] = (pPDB[iPDB] & mask) | bits;
            if (!(pPDB[iPDB] & PAGE_FLAGS_4MB)) {
                // If we are dealing with 4KB pages then we need to iterate
                // through each of the page table entries
                pageTable = pPDB[iPDB] & ~PAGE_LIMIT;
                pPageTable = _PM_mapPhysicalToLinear(pageTable,PAGE_LIMIT,false);
                start = (iPDB == startPDB) ? startPage : 0;
                end = (iPDB == endPDB) ? endPage : 0x3FF;
                for (iPage = start; iPage <= end; iPage++) {
                    pPageTable[iPage] = (pPageTable[iPage] & mask) | bits;
                    }
                MmUnmapIoSpace(pPageTable,PAGE_SIZE);
                }
            }
        }
}

/****************************************************************************
RETURNS:
True on success, false on failure.

REMARKS:
This function re-sizes the memory block table when we run out of space
****************************************************************************/
static ibool ResizeMemBlockTable(void)
{
    ulong   oldSize = maxMemBlocks * sizeof(memblock);
    ulong   size = (maxMemBlocks += memBlockSize) * sizeof(memblock);
    void    *newmemblocks;

    if ((newmemblocks = ExAllocatePool(PagedPool,size)) == NULL)
        return false;
    memset(newmemblocks,0,size);
    if (memblocks) {
        memcpy(newmemblocks,memblocks,oldSize);
        ExFreePool(memblocks);
        }
    memblocks = newmemblocks;
    return true;
}

/****************************************************************************
PARAMETERS:
size        - Size of the memory block to allocate
flags       - Flags to pass to ExAllocPool to allocate the memory block
pBelowAddr  - Optionally allocate below this physical address

RETURNS:
Linear address of the newly allocated memory block.

REMARKS:
This function allocates a block of memory in the global memory arena that
is is optionally accessible from ring 0 and well as in the ring 3 process
address space. We keep track of all allocated memory blocks so that we can
manage mapping them all in and out of the ring 3 address space as needed.

NOTE:   If pBelowAddr is not NULL, the memory block is allocated as a
        contigous block of memory below the specified physical address.
****************************************************************************/
static void *AllocMemBlock(
    long size,
    ulong flags,
    PHYSICAL_ADDRESS *pBelowAddr)
{
    int     i;
    void    *linear;

    // Now find a free slot in our memblocks memory table
    for (i = 0; i < numMemBlocks; i++) {
        if (memblocks[i].linear == NULL)
            break;
        }
    if (i == numMemBlocks) {
        // Resize the memblocks memory table if necessary
        i = numMemBlocks;
        if (numMemBlocks == maxMemBlocks && !ResizeMemBlockTable())
            return NULL;
        numMemBlocks++;
        }

    // Make sure the memory is a muliple of page sized blocks
    size = (size + 0xFFF) & ~0xFFF;

    // Allocate the memory block, optionally as contiguous memory
    if (pBelowAddr) {
        if ((linear = MmAllocateContiguousMemory(size, *pBelowAddr)) == NULL)
            return NULL;
        }
    else {
        if ((linear = ExAllocatePool(flags,size)) == NULL)
            return NULL;
        }

    // Adjust the memory block page table flags
    if (_PM_globalAddressMapping) {
        _PM_beginPageTables();
        _PM_adjustPageTables((ulong)linear,size-1,true,true);
        _PM_endPageTables();
        }
    memblocks[i].length = size;
    memblocks[i].linear = linear;
    memblocks[i].contiguous = (pBelowAddr != NULL);
    return linear;
}

/****************************************************************************
PARAMETERS:
ptr - Linear pointer to the memory to free

REMARKS:
This function frees a block of global memory.
****************************************************************************/
static void FreeMemBlock(
    void *ptr)
{
    int i;

    // Find the memblocks memory block in our table and free it
    for (i = 0; i < numMemBlocks; i++) {
        if (memblocks[i].linear != ptr)
            continue;
        if (memblocks[i].contiguous)
            MmFreeContiguousMemory(memblocks[i].linear);
        else
            ExFreePool(memblocks[i].linear);
        memblocks[i].linear = 0;
        memblocks[i].length = 0;
        memblocks[i].contiguous = false;
        return;
        }
}

/****************************************************************************
REMARKS:
Allocate a block of shared memory. For NT we allocate shared memory
as locked, global memory that is accessible from any memory context
(including interrupt time context), which allows us to load our important
data structure and code such that we can access it directly from a ring
0 interrupt context.
****************************************************************************/
void * PMAPI PM_mallocShared(
    long size)
{
    // Allocate the memory from the non-paged pool. Note that by default
    // the memory is *not* mapped into the user address space at all,
    // but rather that option is left for a later date. If the application
    // needs to run user land code that directly calls SNAP functions,
    // direct user space access to the SNAP functions can be toggled on
    // and off as needed by hitting all the memory pages. Once turned on
    // all new memory allocations are global. When turned off all memory
    // allocations are returned to their previous state.
    return AllocMemBlock(size,NonPagedPool,NULL);
}

/****************************************************************************
REMARKS:
Free a block of shared memory
****************************************************************************/
void PMAPI PM_freeShared(
    void *p)
{
    FreeMemBlock(p);
}

/****************************************************************************
RETURNS:
True on success, false on failure.

REMARKS:
This function re-sizes the memory mapping table when we run out of space
****************************************************************************/
static ibool ResizeMemoryMapTable(void)
{
    ulong   oldSize = maxMaps * sizeof(mmapping);
    ulong   size = (maxMaps += mapsBlockSize) * sizeof(mmapping);
    void    *newmaps;

    if ((newmaps = ExAllocatePool(PagedPool,size)) == NULL)
        return false;
    memset(newmaps,0,size);
    if (maps) {
        memcpy(newmaps,maps,oldSize);
        ExFreePool(maps);
        }
    maps = newmaps;
    return true;
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
    void    *linear;
    ulong   length = limit+1;
    int     i;

    // Search table of existing mappings to see if we have already mapped
    // a region of memory that will serve this purpose.
    for (i = 0; i < numMaps; i++) {
        if (maps[i].physical == base && maps[i].length == length && maps[i].isCached == isCached) {
            _PM_beginPageTables();
            _PM_adjustPageTables((ulong)maps[i].linear, maps[i].length-1, _PM_globalAddressMapping, isCached);
            _PM_endPageTables();
            maps[i].refCount++;
            return (void*)maps[i].linear;
            }
        }

    // Now find a free slot in our memory mapping table
    for (i = 0; i < numMaps; i++) {
        if (maps[i].linear == NULL)
            break;
        }
    if (i == numMaps) {
        // Resize the memory mapping table if necessary
        i = numMaps;
        if (numMaps == maxMaps && !ResizeMemoryMapTable())
            return NULL;
        numMaps++;
        }

    // We did not find any previously mapped memory region, so maps it in.
    if ((linear = _PM_mapPhysicalToLinear(base,limit,isCached)) == INVALID_MEMORY_MAPPING)
        return NULL;
    _PM_beginPageTables();
    _PM_adjustPageTables((ulong)linear, length-1, _PM_globalAddressMapping, isCached);
    _PM_endPageTables();
    maps[i].physical = base;
    maps[i].length = length;
    maps[i].linear = linear;
    maps[i].isCached = isCached;
    maps[i].refCount = 1;
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
    int i;

    // Find the physical memory mapping in our table and free it. Note that
    // if the memory mapping is memblocks, we don't actually free it until the
    // last instance of it is freed.
    for (i = 0; i < numMaps; i++) {
        if (maps[i].linear != ptr)
            continue;
        if (--maps[i].refCount == 0) {
            MmUnmapIoSpace(maps[i].linear,limit+1);
            maps[i].physical = 0;
            maps[i].linear = NULL;
            maps[i].length = 0;
            maps[i].refCount = 0;
            }
        break;
        }
}

/****************************************************************************
REMARKS:
Called when the device driver unloads to free all the page table mappings!
****************************************************************************/
void PMAPI _PM_freeMemoryMappings(void)
{
    int i;

    // Free any memory mappings that were not explicitly freed.
    for (i = 0; i < numMaps; i++) {
        if (maps[i].linear != NULL)
            MmUnmapIoSpace((void *)maps[i].linear,maps[i].length);
        }
}

/****************************************************************************
REMARKS:
Find the physical address of a linear memory address in current process.
****************************************************************************/
ulong PMAPI PM_getPhysicalAddr(
    void *p)
{
    PHYSICAL_ADDRESS    paOurAddress;

    paOurAddress = MmGetPhysicalAddress(p);
    return paOurAddress.LowPart;
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
    ulong   linear = (ulong)p & ~PAGE_LIMIT;

    for (i = (length + PAGE_LIMIT) >> 12; i > 0; i--) {
        if ((*physAddress++ = PM_getPhysicalAddr((void*)linear)) == 0xFFFFFFFF)
            return false;
        linear += PAGE_SIZE;
        }
    return true;
}

/****************************************************************************
REMARKS:
Allocates a block of locked physical memory.
****************************************************************************/
void * PMAPI PM_allocLockedMem(
    uint size,
    ulong *physAddr,
    ibool contiguous,
    ibool below16M)
{
    PHYSICAL_ADDRESS    paOurAddress;
    void                *linearAddr;

    // HighestAcceptableAddress - Specifies the highest valid physical address
    // the driver can use. For example, if a device can only reference physical
    // memory in the lower 16MB, this value would be set to 0x00000000FFFFFF.
    paOurAddress.HighPart = 0;
    if (below16M)
        paOurAddress.LowPart = 0x00FFFFFF;
    else
        paOurAddress.LowPart = 0xFFFFFFFF;
    linearAddr = AllocMemBlock(size,NonPagedPoolCacheAligned,contiguous ? &paOurAddress : NULL);
    // If allocation was successful, get the physical address for that block
    if (linearAddr)
        *physAddr = PM_getPhysicalAddr(linearAddr);
    return linearAddr;
}

/****************************************************************************
REMARKS:
Frees a block of locked physical memory.
****************************************************************************/
void PMAPI PM_freeLockedMem(
    void *p,
    uint size,
    ibool contiguous)
{
    (void)size;
    (void)contiguous;
    FreeMemBlock(p);
}

/****************************************************************************
REMARKS:
Allocates a page aligned and page sized block of memory
****************************************************************************/
void * PMAPI PM_allocPage(
    ibool locked)
{
    // Allocate the memory from the non-paged pool if we want the memory
    // to be locked.
    return ExAllocatePool(locked ? NonPagedPoolCacheAligned : PagedPoolCacheAligned,PAGE_SIZE);
}

/****************************************************************************
REMARKS:
Free a page aligned and page sized block of memory
****************************************************************************/
void PMAPI PM_freePage(
    void *p)
{
    if (p) ExFreePool(p);
}

/****************************************************************************
REMARKS:
Lock linear memory so it won't be paged.
****************************************************************************/
int PMAPI PM_lockDataPages(
    void *p,
    uint len,
    PM_lockHandle *lh)
{
    // Nothing to do since all memory is locked anyway!
    return true;
}

/****************************************************************************
REMARKS:
Unlock linear memory so it won't be paged.
****************************************************************************/
int PMAPI PM_unlockDataPages(
    void *p,
    uint len,
    PM_lockHandle *lh)
{
    // Nothing to do since all memory is locked anyway!
    return true;
}

/****************************************************************************
REMARKS:
Lock linear memory so it won't be paged.
****************************************************************************/
int PMAPI PM_lockCodePages(
    void (*p)(),
    uint len,
    PM_lockHandle *lh)
{
    // Nothing to do since all memory is locked anyway!
    return true;
}

/****************************************************************************
REMARKS:
Unlock linear memory so it won't be paged.
****************************************************************************/
int PMAPI PM_unlockCodePages(
    void (*p)(),
    uint len,
    PM_lockHandle *lh)
{
    // Nothing to do since all memory is locked anyway!
    return true;
}

/****************************************************************************
REMARKS:
Function to enable/disable global address mapping on an as needed basis.
If global address mapping is enabled, all PM shared memory, locked memory
and physical memory mapping addresses are opened up to user space programs.
All future memory allocations will also be enabled for user space access.
Once this is turned off, we restore everything back to kernal space access
only.
****************************************************************************/
void PMAPI PM_enableGlobalAddressMapping(
    ibool enable)
{
    int         i;

    // Set the value of the global address mapping flag
    _PM_globalAddressMapping = enable;

    // Re-adjust page tables for all shared memory mappings
    _PM_beginPageTables();
    for (i = 0; i < numMemBlocks; i++) {
        if (memblocks[i].linear != 0)
            _PM_adjustPageTables((ulong)memblocks[i].linear, memblocks[i].length-1, enable, true);
        }

    // Re-adjust page tables for all physical memory mappings
    for (i = 0; i < numMaps; i++) {
        if (maps[i].linear != NULL)
            _PM_adjustPageTables((ulong)maps[i].linear, maps[i].length-1, _PM_globalAddressMapping, maps[i].isCached);
        }
    _PM_endPageTables();
}

