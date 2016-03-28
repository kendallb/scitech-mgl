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
* Environment:  Unununium
*
* Description:  Implementation for the OS Portability Manager Library, which
*               contains functions to implement OS specific services in a
*               generic, cross platform API. Porting the OS Portability
*               Manager library is the first step to porting any SciTech
*               products to a new platform.
*
* Credits:      Fernando Fernandes Neto
*               Dave Poirier
*               Davison Avery
*       Phil Frost
****************************************************************************/

#include "pmapi.h"
#include "clib/os/os.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#define PM_TRACE

/*--------------------------- Global variables ----------------------------*/

static void (PMAPIP fatalErrorCleanup)(void) = NULL;

/*----------------------------- Implementation ----------------------------*/

/****************************************************************************
REMARKS:
Initialise the PM library.
****************************************************************************/
void PMAPI PM_init(void)
{
#ifdef PM_TRACE
    printf( "PM_init()\n" );
#endif
    // TODO: Do any initialisation in here. This includes getting IOPL
    //       access for the process calling PM_init. This will get called
    //       more than once.

    // TODO: If you support the supplied MTRR register stuff (you need to
    //       be at ring 0 for this!), you should initialise it in here.

    /* MTRR_init(); */
}

/****************************************************************************
REMARKS:
Return the operating system type identifier.
****************************************************************************/
long PMAPI PM_getOSType(void)
{
#ifdef PM_TRACE
    printf( "PM_getOSType()\n" );
#endif
    return _OS_UNUNUNIUM;
}

/****************************************************************************
REMARKS:
Return the name of the runtime environment.
****************************************************************************/
char * PMAPI PM_getOSName(void)
{
#ifdef PM_TRACE
    printf( "PM_getOSName()\n" );
#endif
    return "Unununium Operating Engine";
}

/****************************************************************************
REMARKS:
Return the runtime type identifier (always PM_386 for protected mode)
****************************************************************************/
int PMAPI PM_getModeType(void)
{
#ifdef PM_TRACE
    printf( "PM_getModeType()\n" );
#endif
    return PM_386;
}

/****************************************************************************
REMARKS:
Add a file directory separator to the end of the filename.
****************************************************************************/
void PMAPI PM_backslash(
    char *s)
{
#ifdef PM_TRACE
    printf( "PM_backslash( %s )\n", s );
#endif
    uint pos = strlen(s);
    if (s[pos-1] != '/') {
        s[pos] = '/';
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
#ifdef PM_TRACE
    printf( "PM_setFatalErrorCleanup( %p )\n", cleanup );
#endif
    fatalErrorCleanup = cleanup;
}

/****************************************************************************
REMARKS:
Report a fatal error condition and halt the program.
****************************************************************************/
void PMAPI PM_fatalError(
    const char *msg)
{
#ifdef PM_TRACE
    printf( "PM_fatalError( %s )\n", msg );
#endif
    // TODO: If you are running in a GUI environment without a console,
    //       this needs to be changed to bring up a fatal error message
    //       box and terminate the program.
    if (fatalErrorCleanup)
        fatalErrorCleanup();
    fprintf(stderr,"FATAL ERROR: %s\n", msg);
    for(;;);
}

/****************************************************************************
REMARKS:
Exit handler to kill the VESA transfer buffer.
****************************************************************************/
// removed: no bios access




/****************************************************************************
REMARKS:
Allocate the real mode VESA transfer buffer for communicating with the BIOS.
****************************************************************************/
void * PMAPI PM_getVESABuf(
    uint *len,
    uint *rseg,
    uint *roff)
{
#ifdef PM_TRACE
    printf( "PM_getVESABuf( %p, %p, %p )\n", len, rseg, roff );
#endif
    return NULL;
}

/****************************************************************************
REMARKS:
Check if a key has been pressed.
****************************************************************************/
int PMAPI PM_kbhit(void)
{
#ifdef PM_TRACE
    printf( "PM_kbhit()\n" );
#endif
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
#ifdef PM_TRACE
    printf( "PM_getch()\n" );
#endif
    int key;
    asm( "call get_key\n" : "=a"(key) : : "ebx","ecx","edx","esi","edi","cc" );
    return key;
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
#ifdef PM_TRACE
    printf( "PM_openConsole( %p, %i, %i, %i, %i, %i )\n", hwndUser, device, xRes, yRes, bpp, fullScreen );
#endif
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
#ifdef PM_TRACE
    printf( "PM_getConsoleStateSize()\n" );
#endif
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
#ifdef PM_TRACE
    printf( "PM_saveConsoleState( %p, %p )\n", stateBuf, hwndConsole );
#endif
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
  // We don't care with the AppCallBack, because it will be GUI based, and we
  // won't have a fullscreen console mode
#ifdef PM_TRACE
    printf( "PM_setSuspendAppCallback( %p )\n", saveState );
#endif
   return;
}

/****************************************************************************
REMARKS:
Restore the state of the console from the state buffer.
****************************************************************************/
void PMAPI PM_restoreConsoleState(
    const void *stateBuf,
    PM_HWND hwndConsole)
{
#ifdef PM_TRACE
    printf( "PM_restoreConsoleState( %p, %p )\n", stateBuf, hwndConsole );
#endif
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
#ifdef PM_TRACE
    printf( "PM_closeConsole( %p )\n", hwndConsole );
#endif
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
#ifdef PM_TRACE
    printf( "PM_setOSCursorLocation( %i, %i )\n", x, y );
#endif
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
#ifdef PM_TRACE
    printf( "PM_setOSScreenWidth( %i, %i )\n", width, height );
#endif
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
#ifdef PM_TRACE
    printf( "PM_setRealTimeClockHandler( %p, %i )\n", ih, frequency );
#endif
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
#ifdef PM_TRACE
    printf( "PM_setRealTimeClockFrequency( %i )\n", frequency );
#endif
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
#ifdef PM_TRACE
    printf( "PM_stopRealTimeClock()\n" );
#endif
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
#ifdef PM_TRACE
    printf( "PM_restartRealTimeClock( %i )\n", frequency );
#endif
    PM_setRealTimeClockFrequency(frequency);
}

/****************************************************************************
REMARKS:
Restore the original real time clock handler.
****************************************************************************/
void PMAPI PM_restoreRealTimeClockHandler(void)
{
#ifdef PM_TRACE
    printf( "PM_restoreRealTimeClockHandler()\n" );
#endif
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
#ifdef PM_TRACE
    printf( "PM_( %p, %i )\n", path, maxLen );
#endif
    path[0] = '/';
    path[1] = '\0';
    return path;
}

/****************************************************************************
REMARKS:
Return the drive letter for the boot drive.
****************************************************************************/
char PMAPI PM_getBootDrive(void)
{
#ifdef PM_TRACE
    printf( "PM_getBootDrive()\n" );
#endif
    return '/';
}

/****************************************************************************
REMARKS:
Return the path to the SNAP driver files.
****************************************************************************/
const char * PMAPI PM_getSNAPPath(void)
{
#ifdef PM_TRACE
    printf( "PM_getSNAPPath()\n" );
#endif
    char *env = "/";
    return env;
}

/****************************************************************************
REMARKS:
Return the path to the SNAP configuration files.
****************************************************************************/
const char * PMAPI PM_getSNAPConfigPath(void)
{
#ifdef PM_TRACE
    printf( "PM_getSNAPConfigPath()\n" );
#endif
    char *config = "/";
    return config;
}

/****************************************************************************
REMARKS:
Return a unique identifier for the machine if possible.
****************************************************************************/
const char * PMAPI PM_getUniqueID(void)
{
#ifdef PM_TRACE
    printf( "PM_getUniqueID()\n" );
#endif
    char *machine = "UUU-box";
    return machine;
}

/****************************************************************************
REMARKS:
Get the name of the machine on the network.
****************************************************************************/
const char * PMAPI PM_getMachineName(void)
{
#ifdef PM_TRACE
    printf( "PM_getMachineName()\n" );
#endif
    char *networkname = "unununium.org";
    return networkname;
}

/****************************************************************************
REMARKS:
Return a pointer to the real mode BIOS data area.
****************************************************************************/
void * PMAPI PM_getBIOSPointer(void)
{
#ifdef PM_TRACE
    printf( "PM_getBIOSPointer()\n" );
#endif
    // No BIOS access
    return NULL;
}

/****************************************************************************
REMARKS:
Return a pointer to 0xA0000 physical VGA graphics framebuffer.
****************************************************************************/
void * PMAPI PM_getA0000Pointer(void)
{
#ifdef PM_TRACE
    printf( "PM_getA0000Pointer()\n" );
#endif
    return (void *)0xA0000;
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
#ifdef PM_TRACE
    printf( "PM_mamPhysicalAddr( %lu, %lu, %i )\n", base, limit, isCached );
#endif
    return ((void *)base);
}

/****************************************************************************
REMARKS:
Free a physical address mapping allocated by PM_mapPhysicalAddr.
****************************************************************************/
void PMAPI PM_freePhysicalAddr(
    void *ptr,
    ulong limit)
{
#ifdef PM_TRACE
    printf( "PM_freePhysicalAddr( %p, %lu )\n", ptr, limit );
#endif
    // TODO: This function will free a physical memory mapping previously
    //       allocated with PM_mapPhysicalAddr() if at all possible. If
    //       you can't free physical memory mappings, simply do nothing.
}

/****************************************************************************
REMARKS:
Find the physical address of a linear memory address in current process.
****************************************************************************/
ulong PMAPI PM_getPhysicalAddr(void *p)
{
#ifdef PM_TRACE
    printf( "PM_getPhysicalAddr( %p )\n", p );
#endif
    return (ulong)p;
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
#ifdef PM_TRACE
    printf( "PM_getPhysicalAddrRange( %p, %lu, %p )\n", p, length, physAddress );
#endif
  // As in Unununium, all the Physical Address == Linear Address
  // it's just convert directly and store it in the array.
  // So, there's no reason to return false

  while (length > 0)
  {
   (ulong *)physAddress[length] = p + length;
   -- length;
  }
  return true;
}

/****************************************************************************
REMARKS:
Sleep for the specified number of milliseconds.
****************************************************************************/
void PMAPI PM_sleep(ulong milliseconds)
{
#ifdef PM_TRACE
    printf( "PM_sleep( %lu )\n", milliseconds );
#endif
    // TODO: Put the process to sleep for milliseconds
}

/****************************************************************************
REMARKS:
Return the base I/O port for the specified COM port.
****************************************************************************/
int PMAPI PM_getCOMPort(int port)
{
#ifdef PM_TRACE
    printf( "PM_getCOMPort( %i )\n", port );
#endif
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
#ifdef PM_TRACE
    printf( "PM_getLTPPort( %i )\n", port );
#endif
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
#ifdef PM_TRACE
    printf( "PM_mallocShared( %li )\n", size );
#endif
    // TODO: This is used to allocate memory that is shared between process
    //       that all access the common SNAP drivers via a common display
    //       driver DLL. If your OS does not support shared memory (or if
    //       the display driver does not need to allocate shared memory
    //       for each process address space), this should just call PM_malloc.
    return PM_malloc(size);
}

/****************************************************************************
REMARKS:
Free a block of shared memory.
****************************************************************************/
void PMAPI PM_freeShared(
    void *ptr)
{
#ifdef PM_TRACE
    printf( "PM_freeShared( %p )\n", ptr );
#endif
    // TODO: Free the shared memory block. This will be called in the context
    //       of the original calling process that allocated the shared
    //       memory with PM_mallocShared. Simply call PM_free if you do not
    //       need this.
    PM_free(ptr);
}

/****************************************************************************
REMARKS:
Map a real mode pointer to a protected mode pointer.
****************************************************************************/
void * PMAPI PM_mapRealPointer(
    uint r_seg,
    uint r_off)
{
#ifdef PM_TRACE
    printf( "PM_mapRealPointer( %u, %u )\n", r_seg, r_off );
#endif
    // No BIOS access
    return NULL;
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
#ifdef PM_TRACE
    printf( "PM_allocRealSeg( %u, %p, %p )\n", size, r_seg, r_off );
#endif
    // No BIOS access
    return NULL;
}

/****************************************************************************
REMARKS:
Free a block of real mode memory.
****************************************************************************/
void PMAPI PM_freeRealSeg(
    void *mem)
{
#ifdef PM_TRACE
    printf( "PM_freeRealSeg( %p )\n", mem );
#endif
    // No BIOS access
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
#ifdef PM_TRACE
    printf( "PM_int86( %i, %p, %p )\n", intno, in, out );
#endif
    // No BIOS access
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
#ifdef PM_TRACE
    printf( "PM_int86x( %i, %p, %p )\n", intno, in, out );
#endif
    // No BIOS access
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
#ifdef PM_TRACE
    printf( "PM_callRealMode( %u, %u, %p )\n", seg, off, in );
#endif
    // No BIOS access
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
#ifdef PM_TRACE
    printf( "PM_allocLockedMem( %u, %p, %i, %i )\n", size, physAddr, contiguous, below16Meg );
#endif
    // TODO: Allocate a block of locked, physical memory of the specified
    //       size. This is used for bus master operations. If this is not
    //       supported by the OS, return NULL and bus mastering will not
    //       be used.
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
#ifdef PM_TRACE
    printf( "PM_freeLockedMem( %p, %i, %i )\n", p, size, contiguous );
#endif
    // TODO: Free a memory block allocated with PM_allocLockedMem.
}

/****************************************************************************
REMARKS:
Allocates a page aligned and page sized block of memory
****************************************************************************/
void * PMAPI PM_allocPage(
    ibool locked)
{
#ifdef PM_TRACE
    printf( "PM_allocPage( %i )\n", locked );
#endif
  return NULL;
}

/****************************************************************************
REMARKS:
Free a page aligned and page sized block of memory
****************************************************************************/
void PMAPI PM_freePage(
    void *p)
{
#ifdef PM_TRACE
    printf( "PM_freePage( %p )\n", p );
#endif
  return;
}

/****************************************************************************
REMARKS:
Return true if we have real BIOS access, otherwise false.
****************************************************************************/
ibool PMAPI PM_haveBIOSAccess(void)
{
  // No BIOS access for the Unununium
#ifdef PM_TRACE
    printf( "PM_haveBIOSAccess()\n" );
#endif
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
#ifdef PM_TRACE
    printf( "PM_enableWriteCombine( %lu, %lu, %u )\n", base, length, type );
#endif
    // TODO: This function should enable Pentium Pro and Pentium II MTRR
    //       write combining for the passed in physical memory base address
    //       and length. Normally this is done via calls to an OS specific
    //       device driver as this can only be done at ring 0.
    //
    // NOTE: This is a *very* important function to implement! If you do
    //       not implement, graphics performance on the latest Intel chips
    //       will be severly impaired. For sample code that can be used
    //       directly in a ring 0 device driver, see the MSDOS implementation
    //       which includes assembler code to do this directly (if the
    //       program is running at ring 0).
    return false;
}

/****************************************************************************
REMARKS:
Function to enumerate all write combine regions currently enabled for the
processor.
****************************************************************************/
int PMAPI PM_enumWriteCombine(
    PM_enumWriteCombine_t callback)
{
#ifdef PM_TRACE
    printf( "PM_enumWriteCombine( %p )\n", callback );
#endif
    return 0;
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
#ifdef PM_TRACE
    printf( "PM_lockDataPages( %p, %u, %p )\n", p, len, lh );
#endif
  return 0;
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
#ifdef PM_TRACE
    printf( "PM_unlockDataPages( %p, %u, %p )\n", p, len, lh );
#endif
  return 0;
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
#ifdef PM_TRACE
    printf( "PM_lockCodePages( %p, %u, %p )\n", p, len, lh );
#endif
  return 0;
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

  // Unununium does not use paging
#ifdef PM_TRACE
    printf( "PM_unlockCodePages( %p, %u, %p )\n", p, len, lh );
#endif
  return 0;
}

/****************************************************************************
REMARKS:
Load an OS specific shared library or DLL. If the OS does not support
shared libraries, simply return NULL.
****************************************************************************/
PM_MODULE PMAPI PM_loadLibrary(
    const char *szDLLName)
{
  // We don't have OS specific code
#ifdef PM_TRACE
    printf( "PM_loadLibrary( %s )\n", szDLLName );
#endif
     return NULL;
}

/****************************************************************************
REMARKS:
Get the address of a named procedure from a shared library.
****************************************************************************/
void * PMAPI PM_getProcAddress(
    PM_MODULE hModule,
    const char *szProcName)
{
#ifdef PM_TRACE
    printf( "PM_getProcAddress( %p, %s )\n", hModule, szProcName );
#endif
  return NULL;
}

/****************************************************************************
REMARKS:
Unload a shared library.
****************************************************************************/
void PMAPI PM_freeLibrary(
    PM_MODULE hModule)
{
#ifdef PM_TRACE
    printf( "PM_freeLibrary( %p )\n", hModule );
#endif
    return;
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
#ifdef PM_TRACE
    printf( "PM_setIOPL( %i )\n", level );
#endif
    // TODO: This function should enable IOPL for the task (if IOPL is
    //       not always enabled for the app through some other means).
    return level;
}

/****************************************************************************
REMARKS:
Function to find the first file matching a search criteria in a directory.
****************************************************************************/
void *PMAPI PM_findFirstFile(
    const char *filename,
    PM_findData *findData)
{
    // TODO: This function should start a directory enumeration search
    //       given the filename (with wildcards). The data should be
    //       converted and returned in the findData standard form.
#ifdef PM_TRACE
    printf( "PM_findFirstFile( %s, %p )\n", filename, findData );
#endif
    (void)filename;
    (void)findData;
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
    // TODO: This function should find the next file in directory enumeration
    //       search given the search criteria defined in the call to
    //       PM_findFirstFile. The data should be converted and returned
    //       in the findData standard form.
#ifdef PM_TRACE
    printf( "PM_findNextFile( %p, %p )\n", handle, findData );
#endif
    (void)handle;
    (void)findData;
    return false;
}

/****************************************************************************
REMARKS:
Function to close the find process
****************************************************************************/
void PMAPI PM_findClose(
    void *handle)
{
    // TODO: This function should close the find process. This may do
    //       nothing for some OS'es.
#ifdef PM_TRACE
    printf( "PM_findClose( %p )\n", handle );
#endif
    (void)handle;
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
#ifdef PM_TRACE
    printf( "PM_getdcwd( %i, %p, %i )\n", drive, dir, len );
#endif
    (void)drive;
    (void)len;

    dir[0] = '/';
    dir[1] = '\0';
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
    // TODO: Set the file attributes for a file per OS spec
#ifdef PM_TRACE
    printf( "PM_setFileAttr( %s, %u )\n", filename, attrib );
#endif
    // TODO: Set the file attributes for a file
    (void)filename;
    (void)attrib;
}

/****************************************************************************
REMARKS:
This function is used to retrieve the current file attributes for a specific
file.
****************************************************************************/
uint PMAPI PM_getFileAttr(
    const char *filename)
{
    // TODO: Get the file attributes for a file per OS spec
#ifdef PM_TRACE
    printf( "PM_getFileAttr( %s )\n", filename );
#endif
  return 0;
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
    // TODO: Get the time stamp for a file per OS spec
#ifdef PM_TRACE
    printf( "PM_getFileTime( %s, %i, %p )\n", filename, gmTime, time );
#endif
  return false;
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
    // TODO: Set the time stamp for a file per OS spec
#ifdef PM_TRACE
    printf( "PM_setFileTime( %s, %i, %p )\n", filename, gmTime, time );
#endif
    return false;
}

/****************************************************************************
REMARKS:
Function to create a directory.
****************************************************************************/
ibool PMAPI PM_mkdir(
    const char *filename)
{
#ifdef PM_TRACE
    printf( "PM_mkdir( %s )\n", filename );
#endif
  return false;
}

/****************************************************************************
REMARKS:
Function to remove a directory.
****************************************************************************/
ibool PMAPI PM_rmdir(
    const char *filename)
{
#ifdef PM_TRACE
    printf( "PM_rmdir( %s )\n", filename );
#endif
  return false;
}

/****************************************************************************
REMARKS:
Increase the thread priority to maximum, if possible.
****************************************************************************/
ulong PMAPI PM_setMaxThreadPriority(void)
{
#ifdef PM_TRACE
    printf( "PM_setMaxThreadPriority()\n" );
#endif
    return 0;
}

/****************************************************************************
REMARKS:
Restore the original thread priority.
****************************************************************************/
void PMAPI PM_restoreThreadPriority(
    ulong oldPriority)
{
#ifdef PM_TRACE
    printf( "PM_restoreThreadPriority( %lu )\n", oldPriority );
#endif
  (void)oldPriority;
  return;
}

/****************************************************************************
REMARKS:
Returns true if SDD is the active display driver in the system.
****************************************************************************/
ibool PMAPI PM_isSDDActive(void)
{
#ifdef PM_TRACE
    printf( "PM_isSDDActive()\n" );
#endif
    return false;
}

/****************************************************************************
REMARKS:
This is not relevant to this OS.
****************************************************************************/
ibool PMAPI PM_runningInAWindow(void)
{
#ifdef PM_TRACE
    printf( "PM_runningInAWindow()\n" );
#endif
    return false;
}

void PMAPI PM_flushTLB(void)
{
    /* Do nothing */
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

