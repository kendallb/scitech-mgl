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
* Environment:  WinCE
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

// Include any OS specific headers here!
#include "oshdr.h"
#include <pkfuncs.h>
#include <windev.h>
#include <ceddk.h>

/*--------------------------- Global variables ----------------------------*/

static void *           PM_lowMemPtr = NULL;

static void (PMAPIP fatalErrorCleanup)(void) = NULL;

void    PMAPI _PM_flushTLB(void);
void    PMAPI _PM_saveFPUState(void);
void    PMAPI _PM_restoreFPUState(void);

/*----------------------------- Implementation ----------------------------*/

/****************************************************************************
REMARKS:
WinCE does not support environment variables
****************************************************************************/
char* getenv(const char* __name)
{
    return NULL;
}

/****************************************************************************
REMARKS:
Initialise the PM library.
****************************************************************************/
void PMAPI PM_init(void)
{
    static ibool        initialized = false;
    static SYSTEM_INFO  si;
    BOOL                kmode;

    if (initialized)
        return;

    // Switch to kernel mode for MTRR ring-0 execution
    kmode = SetKMode(TRUE);
    MTRR_init();
    SetKMode(kmode);

    // Map the first Mb of physical memory into PM_lowMemPtr
    if ((PM_lowMemPtr = PM_mapPhysicalAddr(0,0xFFFFF,true)) == NULL)
        PM_fatalError("Unable to map first Mb physical memory!");

    initialized = true;
}

/****************************************************************************
REMARKS:
Return the operating system type identifier.
****************************************************************************/
long PMAPI PM_getOSType(void)
{
    return _OS_WINCE;
}

/****************************************************************************
REMARKS:
Return the name of the runtime environment.
****************************************************************************/
char * PMAPI PM_getOSName(void)
{
    return "Windows CE";
}

/****************************************************************************
REMARKS:
Return the runtime type identifier (always PM_386 for protected mode)
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
    TCHAR szMsg[PM_MAX_PATH];
#ifdef UNICODE
    MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,msg,-1,szMsg,PM_MAX_PATH);
#else
    strcpy(szMsg,msg);
#endif
    if (fatalErrorCleanup)
        fatalErrorCleanup();
    OutputDebugString(szMsg);
    // Message boxes can only be displayed after window manager is running
    if (IsAPIReady(SH_WMGR))
        MessageBox(NULL,szMsg,_T("PM Fatal Error"),MB_OK | MB_ICONWARNING);
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
    // No VESA BIOS access on WinCE
    return NULL;
}

/****************************************************************************
REMARKS:
Check if a key has been pressed.
****************************************************************************/
int PMAPI PM_kbhit(void)
{
    // TODO: This function checks if a key is available to be read. This
    //       should be implemented, but is mostly used by the test programs
    //       these days.
    return true;
}

/****************************************************************************
REMARKS:
Wait for and return the next keypress.
****************************************************************************/
int PMAPI PM_getch(void)
{
    // TODO: This returns the ASCII code of the key pressed. This
    //       should be implemented, but is mostly used by the test programs
    //       these days.
    return 0xD;
}

/****************************************************************************
REMARKS:
Open a fullscreen console mode for output.
****************************************************************************/
PM_HWND PMAPI PM_openConsole(
    PM_HWND hwndUser,
    int device,
    int xRes,
    int yRes,
    int bpp,
    ibool fullScreen)
{
    // TODO: Opens up a fullscreen console for graphics output. If your
    //       console does not have graphics/text modes, this can be left
    //       empty. The main purpose of this is to disable console switching
    //       when in graphics modes if you can switch away from fullscreen
    //       consoles (if you want to allow switching, this can be done
    //       elsewhere with a full save/restore state of the graphics mode).
    return 0;
}

/****************************************************************************
REMARKS:
Return the size of the state buffer used to save the console state.
****************************************************************************/
int PMAPI PM_getConsoleStateSize(void)
{
    // TODO: Returns the size of the console state buffer used to save the
    //       state of the console before going into graphics mode. This is
    //       used to restore the console back to normal when we are done.
    return 1;
}

/****************************************************************************
REMARKS:
Save the state of the console into the state buffer.
****************************************************************************/
void PMAPI PM_saveConsoleState(
    void *stateBuf,
    PM_HWND hwndConsole)
{
    // TODO: Saves the state of the console into the state buffer. This is
    //       used to restore the console back to normal when we are done.
    //       We will always restore 80x25 text mode after being in graphics
    //       mode, so if restoring text mode is all you need to do this can
    //       be left empty.
}

/****************************************************************************
REMARKS:
Set the suspend application callback for the fullscreen console.
****************************************************************************/
void PMAPI PM_setSuspendAppCallback(
    PM_suspendApp_cb saveState)
{
    // TODO:
    (void)saveState;
}

/****************************************************************************
REMARKS:
Restore the state of the console from the state buffer.
****************************************************************************/
void PMAPI PM_restoreConsoleState(
    const void *stateBuf,
    PM_HWND hwndConsole)
{
    // TODO: Restore the state of the console from the state buffer. This is
    //       used to restore the console back to normal when we are done.
    //       We will always restore 80x25 text mode after being in graphics
    //       mode, so if restoring text mode is all you need to do this can
    //       be left empty.
}

/****************************************************************************
REMARKS:
Close the console and return to non-fullscreen console mode.
****************************************************************************/
void PMAPI PM_closeConsole(
    PM_HWND hwndConsole)
{
    // TODO: Close the console when we are done, going back to text mode.
}

/****************************************************************************
REMARKS:
Set the location of the OS console cursor.
****************************************************************************/
void PM_setOSCursorLocation(
    int x,
    int y)
{
    // TODO: Set the OS console cursor location to the new value. This is
    //       generally used for new OS ports (used mostly for DOS).
}

/****************************************************************************
REMARKS:
Set the width of the OS console.
****************************************************************************/
void PM_setOSScreenWidth(
    int width,
    int height)
{
    // TODO: Set the OS console screen width. This is generally unused for
    //       new OS ports.
}

/****************************************************************************
REMARKS:
Set the real time clock handler (used for software stereo modes).
****************************************************************************/
ibool PMAPI PM_setRealTimeClockHandler(
    PM_intHandler ih,
    int frequency)
{
    // TODO: Install a real time clock interrupt handler. Normally this
    //       will not be supported from most OS'es in user land, so an
    //       alternative mechanism is needed to enable software stereo.
    //       Hence leave this unimplemented unless you have a high priority
    //       mechanism to call the 32-bit callback when the real time clock
    //       interrupt fires.
    return false;
}

/****************************************************************************
REMARKS:
Set the real time clock frequency (for stereo modes).
****************************************************************************/
void PMAPI PM_setRealTimeClockFrequency(
    int frequency)
{
    // TODO: Set the real time clock interrupt frequency. Used for stereo
    //       LC shutter glasses when doing software stereo. Usually sets
    //       the frequency to around 2048 Hz.
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
    // TODO: Restores the real time clock handler.
}

/****************************************************************************
REMARKS:
Return the current operating system path or working directory.
****************************************************************************/
char * PMAPI PM_getCurrentPath(
    char *path,
    int maxLen)
{
    // TODO: Need to use shell services for navigation? (no GetCurrentDirectory)
    strcpy(path,".");
    return path;
}

/****************************************************************************
REMARKS:
Return the drive letter for the boot drive.
****************************************************************************/
char PMAPI PM_getBootDrive(void)
{
    // No drive designators on WinCE
    return '\\';
}

/****************************************************************************
REMARKS:
Return the path to the SNAP driver files.
****************************************************************************/
const char * PMAPI PM_getSNAPPath(void)
{
    // Default location for *all* binaries on WinCE including drivers
    return "\\windows";
}

/****************************************************************************
REMARKS:
Return the path to the SNAP configuration files.
****************************************************************************/
const char * PMAPI PM_getSNAPConfigPath(void)
{
    static char path[256];

    // Must use Default drivers location on WinCE since there does not
    // seem to be a way to specify subdirectories in the target image.
    strcpy(path,PM_getSNAPPath());
#if 0
    PM_backslash(path);
    strcat(path,"config");
#endif
    return path;
}

/****************************************************************************
REMARKS:
Return a unique identifier for the machine if possible.
****************************************************************************/
const char * PMAPI PM_getUniqueID(void)
{
    // Win32 gethostname() requires WinSock services on WinCE
    return "UNKNOWN";
}

/****************************************************************************
REMARKS:
Get the name of the machine on the network.
****************************************************************************/
const char * PMAPI PM_getMachineName(void)
{
    // Win32 gethostname() requires WinSock services on WinCE
    return "UNKNOWN";
}

/****************************************************************************
REMARKS:
Return a pointer to the real mode BIOS data area.
****************************************************************************/
void * PMAPI PM_getBIOSPointer(void)
{
    // TODO: This returns a pointer to the real mode BIOS data area. If you
    //       do not support BIOS access, you can simply return NULL here.
    PM_init();
    return (void*)((ulong)PM_lowMemPtr + 0x400);
}

/****************************************************************************
REMARKS:
Return a pointer to 0xA0000 physical VGA graphics framebuffer.
****************************************************************************/
void * PMAPI PM_getA0000Pointer(void)
{
    PM_init();
    return (void*)((ulong)PM_lowMemPtr + 0xA0000);
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
    // NOTE: This function *must* be able to handle any phsyical base
    //       address, and hence you will have to handle rounding of
    //       the physical base address to a page boundary (ie: 4Kb on
    //       x86 CPU's) to be able to properly map in the memory
    //       region.

    // NOTE: If possible the isCached bit should be used to ensure that
    //       the PCD (Page Cache Disable) and PWT (Page Write Through)
    //       bits are set to disable caching for a memory mapping used
    //       for MMIO register access. We also disable caching using
    //       the MTRR registers for Pentium Pro and later chipsets so if
    //       MTRR support is enabled for your OS then you can safely ignore
    //       the isCached flag and always enable caching in the page
    //       tables.

    // The WinCE DDK way to map physical memory
    // Mappings can only be up to 32 Megs at a time
    #define MEG32   0x02000000  // (32*1024*1024)
    DWORD   dwLinearAddr = 0;
    DWORD   dwLinearAddr0 = 0;
    DWORD   dwPhysicalAddr = base & ~4095;
    DWORD   dwBlockSize;
    DWORD   baseOfs = base & 4095;

    // Round the mapping limit up to 4K boundary in order to encompass all the
    // physical address region. The physical base address is rounded down to
    // 4K boundary, so the rounded offset needs to be added back to the returned
    // mapped linear address.
    limit = ((limit+baseOfs+1+4095) & ~4095)-1;

    // If the physical memory region size is more than 32 Megs, WinCE allows
    // mapping the region in consecutive 32 Meg blocks. The returned linear
    // addresses are contiguous for corresponding physical addresses, and this
    // technique is used in other WinCE DDK display drivers for graphics
    // boards with frame buffers larger than 32 Megs.
    while (limit) {
        // Limit mapping block size to 32 Megs maximum
        dwBlockSize = (limit > MEG32) ? MEG32 : limit;

        // Allocate virtual memory in process space to hold the mapped address range
        dwLinearAddr = (DWORD)VirtualAlloc(NULL,dwBlockSize,MEM_RESERVE,PAGE_NOACCESS);
        if (dwLinearAddr == 0)
            return NULL;

        // Bind physical memory to virtual address range
        VirtualCopy(
            (LPVOID)dwLinearAddr,
            (LPVOID)(dwPhysicalAddr >> 8),    // = physical memory / 256 (top 32 of 40 bits)
            dwBlockSize,
            (isCached) ? PAGE_READWRITE | PAGE_PHYSICAL
                       : PAGE_READWRITE | PAGE_NOCACHE | PAGE_PHYSICAL);

        // Adjust the remaining physical memory address region and mapping limit
        dwPhysicalAddr += dwBlockSize;
        limit -= dwBlockSize;

        // Need to save the base linear address if more than one mapping
        if (dwLinearAddr0 == 0)
            dwLinearAddr0 = dwLinearAddr;
        }

    return (void*)(dwLinearAddr0+baseOfs);
}

/****************************************************************************
REMARKS:
Free a physical address mapping allocated by PM_mapPhysicalAddr.
****************************************************************************/
void PMAPI PM_freePhysicalAddr(
    void *ptr,
    ulong limit)
{
    // WinCE needs only physical base address of previous mapping(s).
    // Note that if the mapping size is greater than WinCE 32 Meg limit,
    // we release multiple mappings for consecutive physical address
    // regions similar to manner used above.
    DWORD dwBlockSize;

    while (limit) {
        dwBlockSize = (limit > MEG32) ? MEG32 : limit;
        VirtualFree(ptr,0,MEM_RELEASE);
        // Adjust physical address if more than one 32 Meg block
        ptr = (void*)((DWORD)ptr + dwBlockSize);
        limit -= dwBlockSize;
        }
}

/****************************************************************************
REMARKS:
Find the physical address of a linear memory address in current process.
****************************************************************************/
ulong PMAPI PM_getPhysicalAddr(void *p)
{
    MEMORY_BASIC_INFORMATION    mbi;

    if (VirtualQuery(p,&mbi,sizeof(mbi)))
        return (ulong)mbi.BaseAddress;
    return 0xFFFFFFFFUL;
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

    // dwPageSize == 4096 on Intel x86
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
void PMAPI PM_sleep(ulong milliseconds)
{
    Sleep(milliseconds);
}

/****************************************************************************
REMARKS:
Return the base I/O port for the specified COM port.
****************************************************************************/
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

/****************************************************************************
REMARKS:
Return the base I/O port for the specified LPT port.
****************************************************************************/
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

/****************************************************************************
REMARKS:
Allocate a block of (unnamed) shared memory.
****************************************************************************/
void * PMAPI PM_mallocShared(
    long size)
{
    // The WinCE DDK way to use shared memory via memory-mapped file objects
    HANDLE  hMapObject = NULL;
    DWORD   dwLinearAddr = 0;

    // Create a generic mapping object for the memory (not a file)
    hMapObject = (PUCHAR)CreateFileMapping (
        INVALID_HANDLE_VALUE,   // = physical memory (not a file)
        NULL,
        PAGE_READWRITE,
        0,
        size,
        NULL);                  // optional name string for map object
    if (hMapObject == NULL)
        return NULL;
    
    // Map the memory into the process space
    dwLinearAddr = (DWORD)MapViewOfFile(
        hMapObject,
        FILE_MAP_WRITE,         // = read/write
        0, 0,                   // = offset
        0);                     // = all
    if (dwLinearAddr == 0)
        return NULL;

    return (void*)dwLinearAddr;
}

/****************************************************************************
REMARKS:
Free a block of shared memory.
****************************************************************************/
void PMAPI PM_freeShared(
    void *ptr)
{
    // Release the mapping from MapViewOfFile()
    UnmapViewOfFile(ptr);
}

/****************************************************************************
REMARKS:
Map a real mode pointer to a protected mode pointer.
****************************************************************************/
void * PMAPI PM_mapRealPointer(
    uint r_seg,
    uint r_off)
{
    // TODO: This function maps a real mode memory pointer into the
    //       calling processes address space as a 32-bit near pointer. If
    //       you do not support BIOS access, simply return NULL here.
    return (void*)((ulong)PM_lowMemPtr + MK_PHYS(r_seg,r_off));
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
    // TODO: This function allocates a block of real mode memory for the
    //       calling process used to communicate with real mode BIOS
    //       functions. If you do not support BIOS access, simply return
    //       NULL here.
    return NULL;
}

/****************************************************************************
REMARKS:
Free a block of real mode memory.
****************************************************************************/
void PMAPI PM_freeRealSeg(
    void *mem)
{
    // TODO: Frees a previously allocated real mode memory block. If you
    //       do not support BIOS access, this function should be empty.
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
    // TODO: This function calls the real mode BIOS using the passed in
    //       register structure. If you do not support real mode BIOS
    //       access, this function should return 0.
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
    // TODO: This function calls the real mode BIOS using the passed in
    //       register structure. If you do not support real mode BIOS
    //       access, this function should return 0.
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
    // TODO: This function calls a real mode far function with a far call.
    //       If you do not support BIOS access, this function should be
    //       empty.
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
    // TODO: Allocate a block of locked, physical memory of the specified
    //       size. This is used for bus master operations. If this is not
    //       supported by the OS, return NULL and bus mastering will not
    //       be used.
    // TODO: AllocPhysMem()
    return NULL;
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
    // TODO: Free a memory block allocated with PM_allocLockedMem.
    // TODO: FreePhysMem()
}

/****************************************************************************
REMARKS:
Allocates a page aligned and page sized block of memory
****************************************************************************/
void * PMAPI PM_allocPage(
    ibool locked)
{
    // TODO: VirtualAlloc()
    (void)locked;
    return NULL;
}

/****************************************************************************
REMARKS:
Free a page aligned and page sized block of memory
****************************************************************************/
void PMAPI PM_freePage(
    void *p)
{
    // TODO: VirtualFree()
}

/****************************************************************************
REMARKS:
Return true if we have real BIOS access, otherwise false.
****************************************************************************/
ibool PMAPI PM_haveBIOSAccess(void)
{
    // No BIOS access on WinCE
    return false;
}

/****************************************************************************
REMARKS:
Enable write combining for the memory region.
****************************************************************************/
ibool PMAPI PM_enableWriteCombine(
    ulong base,
    ulong length,
    uint type)
{
    // TODO: MTRR routine returns disabled because we are switched to ring-1
    //       kernel mode, not ring-0.

    // Switch to kernel mode for MTRR ring-0 execution
    BOOL kmode = SetKMode(TRUE);
    ibool retval = MTRR_enableWriteCombine(base,length,type);
    SetKMode(kmode);
    return retval;
}

/****************************************************************************
REMARKS:
Function to enumerate all write combine regions currently enabled for the
processor.
****************************************************************************/
int PMAPI PM_enumWriteCombine(
    PM_enumWriteCombine_t callback)
{
    // Switch to kernel mode for MTRR ring-0 execution
    BOOL kmode = SetKMode(TRUE);
    int  retval = MTRR_enumWriteCombine(callback);
    SetKMode(kmode);
    return retval;
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
    // Array of physical page addresses returned at no extra charge
    DWORD dwPhysicalPages[1024];
    (void)lockHandle;
    return LockPages(p,len,&dwPhysicalPages[0],LOCKFLAG_READ | LOCKFLAG_WRITE);
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
    (void)lockHandle;
    return UnlockPages(p,len);
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
    // Array of physical page addresses returned at no extra charge
    DWORD dwPhysicalPages[1024];
    (void)lockHandle;
    return LockPages(p,len,&dwPhysicalPages[0],LOCKFLAG_READ);
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
    (void)lockHandle;
    return UnlockPages(p,len);
}

/****************************************************************************
REMARKS:
Load an OS specific shared library or DLL. If the OS does not support
shared libraries, simply return NULL.
****************************************************************************/
PM_MODULE PMAPI PM_loadLibrary(
    const char *szDLLName)
{
#ifdef UNICODE
    WCHAR wszName[PM_MAX_PATH];
    MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,szDLLName,-1,wszName,PM_MAX_PATH);
    return LoadLibrary(wszName);
#else
    return LoadLibrary(szDLLName);
#endif
}

/****************************************************************************
REMARKS:
Get the address of a named procedure from a shared library.
****************************************************************************/
void * PMAPI PM_getProcAddress(
    PM_MODULE hModule,
    const char *szProcName)
{
#ifdef UNICODE
    WCHAR wszName[PM_MAX_PATH];
    MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,szProcName,-1,wszName,PM_MAX_PATH);
    return GetProcAddress(hModule, wszName);
#else
    return GetProcAddress(hModule, szProcName);
#endif
}

/****************************************************************************
REMARKS:
Unload a shared library.
****************************************************************************/
void PMAPI PM_freeLibrary(
    PM_MODULE hModule)
{
    FreeLibrary(hModule);
}

/****************************************************************************
REMARKS:
Enable requested I/O privledge level (usually only to set to a value of
3, and then restore it back again). If the OS is protected this function
must be implemented in order to enable I/O port access for ring 3
applications. The function should return the IOPL level active before
the switch occurred so it can be properly restored.
****************************************************************************/
int PMAPI PM_setIOPL(
    int level)
{
    // TODO: This function should enable IOPL for the task (if IOPL is
    //       not always enabled for the app through some other means).
    return level;
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
#ifdef UNICODE
    WCHAR   wszFilename[PM_MAX_PATH];
#endif

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
#ifdef UNICODE
    wcsncpy(wszFilename,blk->cFileName,PM_MAX_PATH);
    WideCharToMultiByte(CP_ACP,WC_COMPOSITECHECK,wszFilename,-1,findData->name,PM_MAX_PATH,NULL,NULL);
#else
    strncpy(findData->name,blk->cFileName,PM_MAX_PATH);
#endif
    findData->name[PM_MAX_PATH-1] = 0;
}

/****************************************************************************
REMARKS:
Function to find the first file matching a search criteria in a directory.
****************************************************************************/
void *PMAPI PM_findFirstFile(
    const char *filename,
    PM_findData *findData)
{
    WIN32_FIND_DATA blk;
    HANDLE          hfile;
    TCHAR           szFilename[PM_MAX_PATH];
#ifdef UNICODE
    MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,filename,-1,szFilename,PM_MAX_PATH);
#else
    strcpy(szFilename,filename);
#endif
    hfile = FindFirstFile(szFilename,&blk);

    if (hfile != INVALID_HANDLE_VALUE) {
        convertFindData(findData,&blk);
        return (void*)hfile;
        }
    return PM_FILE_INVALID;
}

/****************************************************************************
REMARKS:
Function to find the next file matching a search criteria in a directory.
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
REMARKS:
Function to close the find process
****************************************************************************/
void PMAPI PM_findClose(
    void *handle)
{
    FindClose((HANDLE)handle);
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
    // TODO: Need to use shell services for navigation? (no GetCurrentDirectory)
    strcpy(dir,".");
}

/****************************************************************************
REMARKS:
This function is used to file attributes for a specific file to the values
passed in the attribute parameter (a combination of flags defined in
PMFileFlagsType). Under Unix system some of these flags are igonred,
such as the hidden and system attributes.
****************************************************************************/
void PMAPI PM_setFileAttr(
    const char *filename,
    uint attrib)
{
    DWORD attr = 0;
    TCHAR szFilename[PM_MAX_PATH];
#ifdef UNICODE
    MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,filename,-1,szFilename,PM_MAX_PATH);
#else
    strcpy(szFilename,filename);
#endif
    if (attrib & PM_FILE_READONLY)
        attr |= FILE_ATTRIBUTE_READONLY;
    if (attrib & PM_FILE_ARCHIVE)
        attr |= FILE_ATTRIBUTE_ARCHIVE;
    if (attrib & PM_FILE_HIDDEN)
        attr |= FILE_ATTRIBUTE_HIDDEN;
    if (attrib & PM_FILE_SYSTEM)
        attr |= FILE_ATTRIBUTE_SYSTEM;

    SetFileAttributes(szFilename, attr);
}

/****************************************************************************
REMARKS:
This function is used to retrieve the current file attributes for a specific
file.
****************************************************************************/
uint PMAPI PM_getFileAttr(
    const char *filename)
{
    DWORD   attr = 0;
    uint    attrib = 0;
    TCHAR   szFilename[PM_MAX_PATH];
#ifdef UNICODE
    MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,filename,-1,szFilename,PM_MAX_PATH);
#else
    strcpy(szFilename,filename);
#endif
    attr = GetFileAttributes(szFilename);
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
REMARKS:
This function is used to obtain the file date and time stamp for a specific
file. If the gmTime parameter is true, the time is returned in the GMT
time zone, otherwise it is in the local machine time zone.
****************************************************************************/
ibool PMAPI PM_getFileTime(
    const char *filename,
    ibool gmTime,
    PM_time *time)
{
    HANDLE      f;
    FILETIME    utcTime,localTime;
    SYSTEMTIME  sysTime;
    ibool       status = false;
    TCHAR       szFilename[PM_MAX_PATH];
#ifdef UNICODE
    MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,filename,-1,szFilename,PM_MAX_PATH);
#else
    strcpy(szFilename,filename);
#endif
    if ((f = CreateFile(szFilename,GENERIC_READ,0,NULL,OPEN_EXISTING,0,NULL)) == INVALID_HANDLE_VALUE)
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
REMARKS:
This function is used to set the file date and time stamp for a specific
file. If the gmTime parameter is true, the time passed in should be in
the GMT time zone, otherwise it is in the local machine time zone.
****************************************************************************/
ibool PMAPI PM_setFileTime(
    const char *filename,
    ibool gmTime,
    PM_time *time)
{
    HANDLE      f;
    FILETIME    utcTime,localTime;
    SYSTEMTIME  sysTime;
    ibool       status = false;
    TCHAR       szFilename[PM_MAX_PATH];
#ifdef UNICODE
    MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,filename,-1,szFilename,PM_MAX_PATH);
#else
    strcpy(szFilename,filename);
#endif
    if ((f = CreateFile(szFilename,GENERIC_WRITE,0,NULL,OPEN_EXISTING,0,NULL)) == INVALID_HANDLE_VALUE)
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
Function to create a directory.
****************************************************************************/
ibool PMAPI PM_mkdir(
    const char *filename)
{
    TCHAR szFilename[PM_MAX_PATH];
#ifdef UNICODE
    MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,filename,-1,szFilename,PM_MAX_PATH);
#else
    strcpy(szFilename,filename);
#endif
    return CreateDirectory(szFilename,NULL);
}

/****************************************************************************
REMARKS:
Function to remove a directory.
****************************************************************************/
ibool PMAPI PM_rmdir(
    const char *filename)
{
    TCHAR szFilename[PM_MAX_PATH];
#ifdef UNICODE
    MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,filename,-1,szFilename,PM_MAX_PATH);
#else
    strcpy(szFilename,filename);
#endif
    return RemoveDirectory(szFilename);
}

/****************************************************************************
REMARKS:
Increase the thread priority to maximum, if possible.
****************************************************************************/
ulong PMAPI PM_setMaxThreadPriority(void)
{
    DWORD oldPriority = GetThreadPriority(GetCurrentThread());
    SetThreadPriority(GetCurrentThread(),THREAD_PRIORITY_TIME_CRITICAL);
    return oldPriority;
}

/****************************************************************************
REMARKS:
Restore the original thread priority.
****************************************************************************/
void PMAPI PM_restoreThreadPriority(
    ulong oldPriority)
{
    SetThreadPriority(GetCurrentThread(),oldPriority);
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
Flush the Translation Lookaside buffer
****************************************************************************/
void PMAPI PM_flushTLB(void)
{
    // TODO: This is not sufficient to prevent exception in ASM code
#if 0
    // Switch driver to kernel mode to execute privileged instructions
    BOOL kmode = SetKMode(TRUE);
    _PM_flushTLB();
    SetKMode(kmode);
#endif
}

/****************************************************************************
REMARKS:
Save complete FPU state and re-initialize FPU with exceptions off.
****************************************************************************/
void PMAPI PM_saveFPUState(void)
{
    // Not necessary for WinCE (unlike Win9x driver)
}

/****************************************************************************
REMARKS:
Restore FPU state which was saved above.
****************************************************************************/
void PMAPI PM_restoreFPUState(void)
{
    // Not necessary for WinCE (unlike Win9x driver)
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
    /* Nothing to do on Win32 */
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
    /* Nothing to do on Win32 */
    (void)subsystem;
}

