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
* Environment:  16/32 bit DOS
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
#include "ztimerc.h"
#include "mtrr.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dos.h>
#include <conio.h>
#ifdef  __GNUC__
#include <unistd.h>
#include <sys/nearptr.h>
#include <sys/stat.h>
#else
#include <direct.h>
#endif
#ifdef  __BORLANDC__
#pragma warn -par
#endif
#ifndef NO_PMHELP
#include "pm_help.h"
#endif

/*--------------------------- Global variables ----------------------------*/

typedef struct {
    long    edi;
    long    esi;
    long    ebp;
    long    reserved;
    long    ebx;
    long    edx;
    long    ecx;
    long    eax;
    short   flags;
    short   es,ds,fs,gs,ip,cs,sp,ss;
    } DPMI_regs;

typedef struct {
    int     oldMode;
    int     old50Lines;
    } DOS_stateBuf;

#define MAX_RM_BLOCKS   10

static struct {
    void    *p;
    uint    tag;
    } rmBlocks[MAX_RM_BLOCKS];

static uint     VESABuf_len = 1024;     /* Length of the VESABuf buffer     */
static void     *VESABuf_ptr = NULL;    /* Near pointer to VESABuf          */
static uint     VESABuf_rseg;           /* Real mode segment of VESABuf     */
static uint     VESABuf_roff;           /* Real mode offset of VESABuf      */
static void     (PMAPIP fatalErrorCleanup)(void) = NULL;
ushort _VARAPI  _PM_savedDS = 0;
#ifdef  DOS4GW
static ulong    PDB = 0,*pPDB = NULL;
#endif
#ifndef REALMODE
#ifndef NO_PMHELP
static char     VXD_name[] = PMHELP_NAME;
static char     VXD_module[] = PMHELP_MODULE;
static char     VXD_DDBName[] = PMHELP_DDBNAME;
static uint     VXD_version = -1;
static uint     VXD_loadOff = 0;
static uint     VXD_loadSel = 0;
#endif
uint _VARAPI    _PM_VXD_off = 0;
uint _VARAPI    _PM_VXD_sel = 0;
int _VARAPI     _PM_haveCauseWay = -1;

/* Memory mapping cache */

#define MAX_MEMORY_MAPPINGS 100
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
#endif

/* Start of all page tables in CauseWay */

#define CW_PAGE_TABLE_START (1024UL*4096UL*1023UL)

/*----------------------------- Implementation ----------------------------*/

/* External assembler functions */

ulong   _ASMAPI _PM_getPDB(void);
int     _ASMAPI _PM_pagingEnabled(void);
void    _ASMAPI _PM_enableSSE(void);

#if !defined(REALMODE) && !defined(NO_PMHELP)
/****************************************************************************
REMARKS:
Exit function to unload the dynamically loaded VxD
****************************************************************************/
static void UnloadVxD(void)
{
    PMSREGS     sregs;
    VXD_regs    r;

    r.eax = 2;
    r.ebx = 0;
    r.edx = (uint)VXD_module;
    PM_segread(&sregs);
#ifdef  __16BIT__
    r.ds = ((ulong)VXD_module) >> 16;
#else
    r.ds = sregs.ds;
#endif
    r.es = sregs.es;
    _PM_VxDCall(&r,VXD_loadOff,VXD_loadSel);
}

/****************************************************************************
RETURNS:
BCD coded version number of the VxD, or 0 if not loaded (ie: 0x202 - 2.2)

REMARKS:
This function gets the version number for the VxD that we have connected to.
****************************************************************************/
uint PMAPI PMHELP_getVersion(void)
{
    VXD_regs    r;

    /* Call the helper VxD to determine the version number */
    if (_PM_VXD_sel != 0 || _PM_VXD_off != 0) {
        memset(&r,0,sizeof(r));
        r.eax = API_NUM(PMHELP_GETVER);
        _PM_VxDCall(&r,_PM_VXD_off,_PM_VXD_sel);
        return VXD_version = (uint)r.eax;
        }
    return VXD_version = 0;
}

/****************************************************************************
DESCRIPTION:
Connects to the helper VxD and returns the version number

RETURNS:
True if the VxD was found and loaded, false otherwise.

REMARKS:
This function connects to the VxD (loading it if it is dynamically loadable)
and returns the version number of the VxD.
****************************************************************************/
static ibool PMHELP_connect(void)
{
    PMREGS      regs;
    PMSREGS     sregs;
    VXD_regs    r;

    /* Bail early if we have alread connected */
    if (VXD_version != -1)
        return VXD_version != 0;

    /* Get the static SDDHELP.VXD entry point if available */
    PM_segread(&sregs);
    regs.x.ax = 0x1684;
    regs.x.bx = SDDHELP_DeviceID;
    regs.x.di = 0;
    sregs.es = 0;
    PM_int386x(0x2F,&regs,&regs,&sregs);
    _PM_VXD_sel = sregs.es;
    _PM_VXD_off = regs.x.di;
    if (_PM_VXD_sel != 0 || _PM_VXD_off != 0) {
        if (PMHELP_getVersion() >= PMHELP_VERSION)
            return true;
        }

    /* If we get here, then either SDDHELP.VXD is not loaded, or it is an
     * earlier version. In this case try to dynamically load the PMHELP.VXD
     * helper VxD instead.
     */
    PM_segread(&sregs);
    regs.x.ax = 0x1684;
    regs.x.bx = VXDLDR_DeviceID;
    regs.x.di = 0;
    sregs.es = 0;
    PM_int386x(0x2F,&regs,&regs,&sregs);
    VXD_loadSel = sregs.es;
    VXD_loadOff = regs.x.di;
    if (VXD_loadSel == 0 && VXD_loadOff == 0)
        return VXD_version = 0;
    r.eax = 1;
    r.ebx = 0;
    r.edx = (uint)VXD_name;
    PM_segread(&sregs);
    r.ds = sregs.ds;
    r.es = sregs.es;
    _PM_VxDCall(&r,VXD_loadOff,VXD_loadSel);
    if (r.eax != 0)
        return VXD_version = 0;

    /* Get the dynamic VxD entry point so we can call it */
    atexit(UnloadVxD);
    PM_segread(&sregs);
    regs.x.ax = 0x1684;
    regs.x.bx = 0;
    regs.e.edi = (uint)VXD_DDBName;
    PM_int386x(0x2F,&regs,&regs,&sregs);
    _PM_VXD_sel = sregs.es;
    _PM_VXD_off = regs.x.di;
    if (_PM_VXD_sel == 0 && _PM_VXD_off == 0)
        return VXD_version = 0;
    if (PMHELP_getVersion() >= PMHELP_VERSION)
        return true;
    return VXD_version = 0;
}
#endif

/****************************************************************************
REMARKS:
Initialise the PM library. First we try to connect to a static SDDHELP.VXD
helper VxD, and check that it is a version we can use. If not we try to
dynamically load the PMHELP.VXD helper VxD
****************************************************************************/
void PMAPI PM_init(void)
{
#ifndef REALMODE
    PMREGS  regs;

    /* Check if we are running under CauseWay under real DOS */
    if (_PM_haveCauseWay == -1) {
        /* Check if we are running under DPMI in which case we will not be
         * able to use our special ring 0 CauseWay functions.
         */
        _PM_haveCauseWay = false;
        regs.x.ax = 0xFF00;
        PM_int386(0x31,&regs,&regs);
        if (regs.x.cflag || !(regs.e.edi & 8)) {
            /* We are not under DPMI, so now check if CauseWay is active */
            regs.x.ax = 0xFFF9;
            PM_int386(0x31,&regs,&regs);
            if (!regs.x.cflag && regs.e.ecx == 0x43415553 && regs.e.edx == 0x45574159)
                _PM_haveCauseWay = true;
            }

#ifndef NO_PMHELP
        /* Now connect to PMHELP.VXD and initialise MTRR module */
        if (!PMHELP_connect())
#endif
            MTRR_init();

        /* Now set up SSE/SSE2 support if available */
        if (_MTRR_isRing0() && (CPU_haveSSE() || CPU_haveSSE2()))
            _PM_enableSSE();
        }
#endif
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
#ifndef REALMODE
#ifndef NO_PMHELP
    VXD_regs    regs;

    if (PMHELP_connect()) {
        memset(&regs,0,sizeof(regs));
        regs.eax = API_NUM(PMHELP_ENABLELFBCOMB);
        regs.ebx = base;
        regs.ecx = size;
        regs.edx = type;
        _PM_VxDCall(&regs,_PM_VXD_off,_PM_VXD_sel);
        return regs.eax;
        }
#endif
    return MTRR_enableWriteCombine(base,size,type);
#else
    return PM_MTRR_NOT_SUPPORTED;
#endif
}

/****************************************************************************
REMARKS:
Function to enumerate all write combine regions currently enabled for the
processor.
****************************************************************************/
int PMAPI PM_enumWriteCombine(
    PM_enumWriteCombine_t callback)
{
#ifndef REALMODE
    return MTRR_enumWriteCombine(callback);
#else
    return PM_MTRR_NOT_SUPPORTED;
#endif
}

ibool PMAPI PM_haveBIOSAccess(void)
{
    /* Allow the BIOS emulator to be forced on */
    if (getenv("SNAP_FORCE_BIOSEMU") != NULL)
        return false;
    return true;
}

long PMAPI PM_getOSType(void)
{ return _OS_DOS; }

char * PMAPI PM_getOSName(void)
{ return "MSDOS"; }

int PMAPI PM_getModeType(void)
{
#if defined(REALMODE)
    return PM_realMode;
#elif defined(PM286)
    return PM_286;
#elif defined(PM386)
    return PM_386;
#endif
}

void PMAPI PM_backslash(char *s)
{
    uint pos = strlen(s);
    if (s[pos-1] != '\\') {
        s[pos] = '\\';
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
        atexit(ExitVBEBuf);
        }
    *len = VESABuf_len;
    *rseg = VESABuf_rseg;
    *roff = VESABuf_roff;
    return VESABuf_ptr;
}

int PMAPI PM_int386(int intno, PMREGS *in, PMREGS *out)
{
    PMSREGS sregs;
    PM_segread(&sregs);
    return PM_int386x(intno,in,out,&sregs);
}

/* Routines to set and get the real mode interrupt vectors, by making
 * direct real mode calls to DOS and bypassing the DOS extenders API.
 * This is the safest way to handle this, as some servers try to be
 * smart about changing real mode vectors.
 */

void PMAPI _PM_getRMvect(int intno, long *realisr)
{
    RMREGS  regs;
    RMSREGS sregs;

    PM_saveDS();
    regs.h.ah = 0x35;
    regs.h.al = intno;
    PM_int86x(0x21, &regs, &regs, &sregs);
    *realisr = ((long)sregs.es << 16) | regs.x.bx;
}

void PMAPI _PM_setRMvect(int intno, long realisr)
{
    RMREGS  regs;
    RMSREGS sregs;

    PM_saveDS();
    regs.h.ah = 0x25;
    regs.h.al = intno;
    sregs.ds = (int)(realisr >> 16);
    regs.x.dx = (int)(realisr & 0xFFFF);
    PM_int86x(0x21, &regs, &regs, &sregs);
}

void PMAPI _PM_addRealModeBlock(void *mem,uint tag)
{
    int i;

    for (i = 0; i < MAX_RM_BLOCKS; i++) {
        if (rmBlocks[i].p == NULL) {
            rmBlocks[i].p = mem;
            rmBlocks[i].tag = tag;
            return;
            }
        }
    PM_fatalError("To many real mode memory block allocations!");
}

uint PMAPI _PM_findRealModeBlock(void *mem)
{
    int i;

    for (i = 0; i < MAX_RM_BLOCKS; i++) {
        if (rmBlocks[i].p == mem)
            return rmBlocks[i].tag;
        }
    PM_fatalError("Could not find prior real mode memory block allocation!");
    return 0;
}

char * PMAPI PM_getCurrentPath(
    char *path,
    int maxLen)
{
    return getcwd(path,maxLen);
}

char PMAPI PM_getBootDrive(void)
{ return 'C'; }

const char * PMAPI PM_getSNAPPath(void)
{
    static char path[256];
    char        *env;

    if ((env = getenv("SNAP_PATH")) != NULL)
        return env;
    if ((env = getenv("WINBOOTDIR")) != NULL) {
        /* Running in a Windows 9x DOS box or DOS mode */
        strcpy(path,env);
        strcat(path,"\\system\\snap");
        return path;
        }
    if ((env = getenv("SystemRoot")) != NULL) {
        /* Running in an NT/2K DOS box */
        strcpy(path,env);
        strcat(path,"\\system32\\snap");
        return path;
        }
    return "c:\\snap\\drivers";
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
    static char name[256];
    RMREGS      regs;
    RMSREGS     sregs;
    uint        len,rseg,roff;
    char        *p,*q;
    void        *buf;

    if ((buf = PM_getVESABuf(&len,&rseg,&roff)) == NULL)
        PM_fatalError("Unable to get real mode transfer buffer!\n");
    regs.h.ah = 0x5E;
    regs.h.al = 0;
    regs.x.dx = roff;
    sregs.ds = rseg;
    PM_int86x(0x21,&regs,&regs,&sregs);
    if (regs.h.ch != 0) {
        p = buf;
        q = name;
        while (*p != ' ' && *p != '\0')
            *q++ = *p++;
        *q = '\0';
        return name;
        }
    return "UNKNOWN";
}

int PMAPI PM_kbhit(void)
{
    return kbhit();
}

int PMAPI PM_getch(void)
{
    return getch();
}

PM_HWND PMAPI PM_openConsole(PM_HWND hwndUser,int device,int xRes,int yRes,int bpp,ibool fullScreen)
{
    /* Not used for DOS */
    (void)hwndUser;
    (void)device;
    (void)xRes;
    (void)yRes;
    (void)bpp;
    (void)fullScreen;
    return 0;
}

int PMAPI PM_getConsoleStateSize(void)
{
    return sizeof(DOS_stateBuf);
}

void PMAPI PM_saveConsoleState(void *stateBuf,PM_HWND hwndConsole)
{
    RMREGS          regs;
    DOS_stateBuf    *sb = stateBuf;

    /* Save the old video mode state */
    regs.h.ah = 0x0F;
    PM_int86(0x10,&regs,&regs);
    sb->oldMode = regs.h.al & 0x7F;
    sb->old50Lines = false;
    if (sb->oldMode == 0x3) {
        regs.x.ax = 0x1130;
        regs.x.bx = 0;
        regs.x.dx = 0;
        PM_int86(0x10,&regs,&regs);
        sb->old50Lines = (regs.h.dl == 42 || regs.h.dl == 49);
        }
    (void)hwndConsole;
}

void PMAPI PM_setSuspendAppCallback(int (_ASMAPIP saveState)(int flags))
{
    /* Not used for DOS */
    (void)saveState;
}

void PMAPI PM_restoreConsoleState(const void *stateBuf,PM_HWND hwndConsole)
{
    RMREGS              regs;
    const DOS_stateBuf  *sb = stateBuf;

    /* Retore 50 line mode if set */
    if (sb->old50Lines) {
        regs.x.ax = 0x1112;
        regs.x.bx = 0;
        PM_int86(0x10,&regs,&regs);
        }
    (void)hwndConsole;
}

void PMAPI PM_closeConsole(PM_HWND hwndConsole)
{
    /* Not used for DOS */
    (void)hwndConsole;
}

void PMAPI PM_setOSCursorLocation(int x,int y)
{
    uchar *_biosPtr = PM_getBIOSPointer();
    PM_setByte(_biosPtr+0x50,x);
    PM_setByte(_biosPtr+0x51,y);
}

void PMAPI PM_setOSScreenWidth(int width,int height)
{
    uchar *_biosPtr = PM_getBIOSPointer();
    PM_setWord(_biosPtr+0x4A,width);
    PM_setWord(_biosPtr+0x4C,width*2);
    PM_setByte(_biosPtr+0x84,height-1);
    if (height > 25) {
        PM_setWord(_biosPtr+0x60,0x0607);
        PM_setByte(_biosPtr+0x85,0x08);
        }
    else {
        PM_setWord(_biosPtr+0x60,0x0D0E);
        PM_setByte(_biosPtr+0x85,0x016);
        }
}

void * PMAPI PM_mallocShared(long size)
{
    return PM_malloc(size);
}

void PMAPI PM_freeShared(void *ptr)
{
    PM_free(ptr);
}

void PMAPI PM_sleep(ulong milliseconds)
{
    ulong           microseconds = milliseconds * 1000L;
    LZTimerObject   tm;

    LZTimerOnExt(&tm);
    while (LZTimerLapExt(&tm) < microseconds)
        ;
    LZTimerOffExt(&tm);
}

int PMAPI PM_getCOMPort(int port)
{
    switch (port) {
        case 0: return 0x3F8;
        case 1: return 0x2F8;
        }
    return 0;
}

int PMAPI PM_getLPTPort(int port)
{
    switch (port) {
        case 0: return 0x3BC;
        case 1: return 0x378;
        case 2: return 0x278;
        }
    return 0;
}

PM_MODULE PMAPI PM_loadLibrary(
    const char *szDLLName)
{
    (void)szDLLName;
    return NULL;
}

void * PMAPI PM_getProcAddress(
    PM_MODULE hModule,
    const char *szProcName)
{
    (void)hModule;
    (void)szProcName;
    return NULL;
}

void PMAPI PM_freeLibrary(
    PM_MODULE hModule)
{
    (void)hModule;
}

int PMAPI PM_setIOPL(
    int level)
{
    return level;
}

/****************************************************************************
REMARKS:
Internal function to convert the find data to the generic interface.
****************************************************************************/
static void convertFindData(
    PM_findData *findData,
    struct find_t *blk)
{
    ulong   dwSize = findData->dwSize;

    memset(findData,0,findData->dwSize);
    findData->dwSize = dwSize;
    if (blk->attrib & _A_RDONLY)
        findData->attrib |= PM_FILE_READONLY;
    if (blk->attrib & _A_SUBDIR)
        findData->attrib |= PM_FILE_DIRECTORY;
    if (blk->attrib & _A_ARCH)
        findData->attrib |= PM_FILE_ARCHIVE;
    if (blk->attrib & _A_HIDDEN)
        findData->attrib |= PM_FILE_HIDDEN;
    if (blk->attrib & _A_SYSTEM)
        findData->attrib |= PM_FILE_SYSTEM;
    findData->sizeLo = blk->size;
    strncpy(findData->name,blk->name,PM_MAX_PATH);
    findData->name[PM_MAX_PATH-1] = 0;
}

#define FIND_MASK   (_A_RDONLY | _A_ARCH | _A_SUBDIR | _A_HIDDEN | _A_SYSTEM)

/****************************************************************************
REMARKS:
Function to find the first file matching a search criteria in a directory.
****************************************************************************/
void * PMAPI PM_findFirstFile(
    const char *filename,
    PM_findData *findData)
{
    struct find_t *blk;

    if ((blk = PM_malloc(sizeof(*blk))) == NULL)
        return PM_FILE_INVALID;
    if (_dos_findfirst((char*)filename,FIND_MASK,blk) == 0) {
        convertFindData(findData,blk);
        return blk;
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
    struct find_t *blk = handle;

    if (_dos_findnext(blk) == 0) {
        convertFindData(findData,blk);
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
    PM_free(handle);
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
    uint oldDrive,maxDrives;
    _dos_getdrive(&oldDrive);
    _dos_setdrive(drive,&maxDrives);
    getcwd(dir,len);
    _dos_setdrive(oldDrive,&maxDrives);
}

/****************************************************************************
REMARKS:
Function to change the file attributes for a specific file.
****************************************************************************/
void PMAPI PM_setFileAttr(
    const char *filename,
    uint attrib)
{
#if defined(TNT) && defined(_MSC_VER)
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
#else
    uint attr = 0;

    if (attrib & PM_FILE_READONLY)
        attr |= _A_RDONLY;
    if (attrib & PM_FILE_ARCHIVE)
        attr |= _A_ARCH;
    if (attrib & PM_FILE_HIDDEN)
        attr |= _A_HIDDEN;
    if (attrib & PM_FILE_SYSTEM)
        attr |= _A_SYSTEM;
    _dos_setfileattr(filename,attr);
#endif
}

/****************************************************************************
REMARKS:
Function to create a directory.
****************************************************************************/
ibool PMAPI PM_mkdir(
    const char *filename)
{
#ifdef  __GNUC__
    return mkdir(filename,S_IRUSR) == 0;
#else
    return mkdir(filename) == 0;
#endif
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

/*-------------------------------------------------------------------------*/
/* Generic DPMI routines common to 16/32 bit code                          */
/*-------------------------------------------------------------------------*/

#ifndef REALMODE
ulong PMAPI DPMI_mapPhysicalToLinear(ulong physAddr,ulong limit)
{
    PMREGS  r;
    int     i;
    ulong   baseAddr,baseOfs,roundedLimit;

    /* We can't map memory below 1Mb, but the linear address are already
     * mapped 1:1 for this memory anyway so we just return the base address.
     */
    if (physAddr < 0x100000L)
        return physAddr;

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
    r.x.ax = 0x800;
    r.x.bx = baseAddr >> 16;
    r.x.cx = baseAddr & 0xFFFF;
    r.x.si = roundedLimit >> 16;
    r.x.di = roundedLimit & 0xFFFF;
    PM_int386(0x31, &r, &r);
    if (r.x.cflag)
        return 0xFFFFFFFFUL;
    maps[i].physical = physAddr;
    maps[i].limit = limit;
    maps[i].linear = ((ulong)r.x.bx << 16) + r.x.cx + baseOfs;
    return maps[i].linear;
}

int PMAPI DPMI_setSelectorBase(ushort sel,ulong linAddr)
{
    PMREGS  r;

    r.x.ax = 7;                     /* DPMI set selector base address   */
    r.x.bx = sel;
    r.x.cx = linAddr >> 16;
    r.x.dx = linAddr & 0xFFFF;
    PM_int386(0x31, &r, &r);
    if (r.x.cflag)
        return 0;
    return 1;
}

ulong PMAPI DPMI_getSelectorBase(ushort sel)
{
    PMREGS  r;

    r.x.ax = 6;                     /* DPMI get selector base address   */
    r.x.bx = sel;
    PM_int386(0x31, &r, &r);
    return ((ulong)r.x.cx << 16) + r.x.dx;
}

int PMAPI DPMI_setSelectorLimit(ushort sel,ulong limit)
{
    PMREGS  r;

    r.x.ax = 8;                     /* DPMI set selector limit          */
    r.x.bx = sel;
    r.x.cx = limit >> 16;
    r.x.dx = limit & 0xFFFF;
    PM_int386(0x31, &r, &r);
    if (r.x.cflag)
        return 0;
    return 1;
}

uint PMAPI DPMI_createSelector(ulong base,ulong limit)
{
    uint    sel;
    PMREGS  r;

    /* Allocate 1 descriptor */
    r.x.ax = 0;
    r.x.cx = 1;
    PM_int386(0x31, &r, &r);
    if (r.x.cflag) return 0;
    sel = r.x.ax;

    /* Set the descriptor access rights (for a 32 bit page granular
     * segment).
     */
    if (limit >= 0x10000L) {
        r.x.ax = 9;
        r.x.bx = sel;
        r.x.cx = 0x40F3;
        PM_int386(0x31, &r, &r);
        }

    /* Map physical memory and create selector */
    if ((base = DPMI_mapPhysicalToLinear(base,limit)) == 0xFFFFFFFFUL)
        return 0;
    if (!DPMI_setSelectorBase(sel,base))
        return 0;
    if (!DPMI_setSelectorLimit(sel,limit))
        return 0;
    return sel;
}

void PMAPI DPMI_freeSelector(uint sel)
{
    PMREGS  r;

    r.x.ax = 1;
    r.x.bx = sel;
    PM_int386(0x31, &r, &r);
}

int PMAPI DPMI_lockLinearPages(ulong linear,ulong len)
{
    PMREGS  r;

    r.x.ax = 0x600;                     /* DPMI Lock Linear Region      */
    r.x.bx = (linear >> 16);            /* Linear address in BX:CX      */
    r.x.cx = (linear & 0xFFFF);
    r.x.si = (len >> 16);               /* Length in SI:DI              */
    r.x.di = (len & 0xFFFF);
    PM_int386(0x31, &r, &r);
    return (!r.x.cflag);
}

int PMAPI DPMI_unlockLinearPages(ulong linear,ulong len)
{
    PMREGS  r;

    r.x.ax = 0x601;                     /* DPMI Unlock Linear Region    */
    r.x.bx = (linear >> 16);            /* Linear address in BX:CX      */
    r.x.cx = (linear & 0xFFFF);
    r.x.si = (len >> 16);               /* Length in SI:DI              */
    r.x.di = (len & 0xFFFF);
    PM_int386(0x31, &r, &r);
    return (!r.x.cflag);
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
static void PM_adjustPageTables(
    ulong linear,
    ulong limit,
    ibool isCached)
{
#ifdef  DOS4GW
    int     startPDB,endPDB,iPDB,startPage,endPage,start,end,iPage;
    ulong   andMask,orMask,pageTable,*pPageTable;

    andMask = ~0x18;
    orMask = (isCached) ? 0x00 : 0x18;
    if (_PM_pagingEnabled() == 1 && (PDB = _PM_getPDB()) != 0) {
        if (_PM_haveCauseWay) {
            /* CauseWay is a little different in the page table handling.
             * The code that we use for DOS4G/W does not appear to work
             * with CauseWay correctly as it does not appear to allow us
             * to map the page tables directly. Instead we can directly
             * access the page table entries in extended memory where
             * CauseWay always locates them (starting at 1024*4096*1023)
             */
            startPage = (linear >> 12);
            endPage = ((linear+limit) >> 12);
            pPageTable = (ulong*)CW_PAGE_TABLE_START;
            for (iPage = startPage; iPage <= endPage; iPage++)
                pPageTable[iPage] = (pPageTable[iPage] & andMask) | orMask;
            }
        else {
            pPDB = (ulong*)DPMI_mapPhysicalToLinear(PDB,0xFFF);
            if (pPDB) {
                startPDB = (linear >> 22) & 0x3FF;
                startPage = (linear >> 12) & 0x3FF;
                endPDB = ((linear+limit) >> 22) & 0x3FF;
                endPage = ((linear+limit) >> 12) & 0x3FF;
                for (iPDB = startPDB; iPDB <= endPDB; iPDB++) {
                    pageTable = pPDB[iPDB] & ~0xFFF;
                    pPageTable = (ulong*)DPMI_mapPhysicalToLinear(pageTable,0xFFF);
                    start = (iPDB == startPDB) ? startPage : 0;
                    end = (iPDB == endPDB) ? endPage : 0x3FF;
                    for (iPage = start; iPage <= end; iPage++)
                        pPageTable[iPage] = (pPageTable[iPage] & andMask) | orMask;
                    }
                }
            }
        PM_flushTLB();
        }
#endif
}

void * PMAPI DPMI_mapPhysicalAddr(
    ulong base,
    ulong limit,
    ibool isCached)
{
    PMSREGS     sregs;
    ulong       linAddr;
    ulong       DSBaseAddr;

    /* Get base address of DS selector */
    PM_segread(&sregs);
    DSBaseAddr = DPMI_getSelectorBase(sregs.ds);

    /* Map the memory to a linear address using DPMI function 0x800 */
    if ((linAddr = DPMI_mapPhysicalToLinear(base,limit)) == 0xFFFFFFFF) {
        if (base >= 0x100000)
            return NULL;
        /* If the linear address mapping fails but we are trying to
         * map an area in the first 1Mb of system memory, then we must
         * be running under a Windows or OS/2 DOS box. Under these
         * environments we can use the segment wrap around as a fallback
         * measure, as this does work properly.
         */
        linAddr = base;
        }

    /* Now expand the default DS selector to 4Gb so we can access it */
    if (!DPMI_setSelectorLimit(sregs.ds,0xFFFFFFFFUL))
        return NULL;

    /* Finally enable caching for the page tables that we just mapped in,
     * since DOS4GW and PMODE/W create the page table entries without
     * caching enabled which hurts the performance of the linear framebuffer
     * as it disables write combining on Pentium Pro and above processors.
     *
     * For those processors cache disabling is better handled through the
     * MTRR registers anyway (we can write combine a region but disable
     * caching) so that MMIO register regions do not screw up.
     */
    if (DSBaseAddr == 0)
        PM_adjustPageTables(linAddr,limit,isCached);

    /* Now return the base address of the memory into the default DS */
    return (void*)(linAddr - DSBaseAddr);
}

#if defined(PM386)

/* Some DOS extender implementations do not directly support calling a
 * real mode procedure from protected mode. However we can simulate what
 * we need temporarily hooking the INT 6Ah vector with a small real mode
 * stub that will call our real mode code for us.
 */

static uchar int6AHandler[] = {
    0x00,0x00,0x00,0x00,        /*  __PMODE_callReal variable           */
    0xFB,                       /*  sti                                 */
    0x2E,0xFF,0x1E,0x00,0x00,   /*  call    [cs:__PMODE_callReal]       */
    0xCF,                       /*  iretf                               */
    };
static uchar *crPtr = NULL; /* Pointer to of int 6A handler         */
static uint crRSeg,crROff;  /* Real mode seg:offset of handler      */

void PMAPI PM_callRealMode(uint seg,uint off, RMREGS *in,
    RMSREGS *sregs)
{
    uchar   *p;
    uint    oldSeg,oldOff;

    if (!crPtr) {
        /* Allocate and copy the memory block only once */
        crPtr = PM_allocRealSeg(sizeof(int6AHandler), &crRSeg, &crROff);
        memcpy(crPtr,int6AHandler,sizeof(int6AHandler));
        }
    PM_setWord(crPtr,off);              /* Plug in address to call  */
    PM_setWord(crPtr+2,seg);
    p = PM_mapRealPointer(0,0x6A * 4);
    oldOff = PM_getWord(p);             /* Save old handler address */
    oldSeg = PM_getWord(p+2);
    PM_setWord(p,crROff+4);             /* Hook 6A handler          */
    PM_setWord(p+2,crRSeg);
    PM_int86x(0x6A, in, in, sregs);     /* Call real mode code      */
    PM_setWord(p,oldOff);               /* Restore old handler      */
    PM_setWord(p+2,oldSeg);
}

#endif  /* PM386 */

#endif  /* !REALMODE */

/****************************************************************************
REMARKS:
Allocates a block of locked, physically contiguous memory. The memory
may be required to be below the 16Meg boundary.
****************************************************************************/
void * PMAPI PM_allocLockedMem(
    uint size,
    ulong *physAddr,
    ibool contiguous,
    ibool below16Meg)
{
    uchar           *p,*roundedP;
    uint            r_seg,r_off;
    uint            roundedSize = (size + 4 + 4096 + 0xFFF) & ~0xFFF;
    PM_lockHandle   lh; /* Unused in DOS */
#ifndef REALMODE
#ifndef NO_PMHELP
    VXD_regs        regs;

    /* If we have connected to our helper VxD in a Windows DOS box, use the
     * helper VxD services to allocate the memory that we need.
     */
    if (VXD_version) {
        memset(&regs,0,sizeof(regs));
        regs.eax = API_NUM(PMHELP_ALLOCLOCKED);
        regs.ebx = size;
        regs.ecx = (ulong)physAddr;
        regs.edx = contiguous | (below16Meg << 8);
        _PM_VxDCall(&regs,_PM_VXD_off,_PM_VXD_sel);
        return (void*)regs.eax;
        }
#endif

    /* If the memory is not contiguous, we simply need to allocate it
     * using regular memory allocation services, and lock it down
     * in memory.
     *
     * For contiguous memory blocks, the only way to guarantee contiguous physical
     * memory addresses under DOS is to allocate the memory below the
     * 1Meg boundary as real mode memory.
     *
     * Note that we must page align the memory block, and we also must
     * keep track of the non-aligned pointer so we can properly free
     * it later. Hence we actually allocate 4 bytes more than the
     * size rounded up to the next 4K boundary.
     */
    if (!contiguous)
        p = PM_malloc(roundedSize);
    else
#endif
        p = PM_allocRealSeg(roundedSize,&r_seg,&r_off);
    if (p == NULL)
        return NULL;
    roundedP = (void*)(((ulong)p + 0xFFF) & ~0xFFF);
    *((ulong*)(roundedP + size)) = (ulong)p;
    PM_lockDataPages(roundedP,size,&lh);
    if ((*physAddr = PM_getPhysicalAddr(roundedP)) == 0xFFFFFFFF) {
        PM_freeLockedMem(roundedP,size,contiguous);
        return NULL;
        }

    /* Disable caching for the memory since it is probably a DMA buffer */
#ifndef REALMODE
    PM_adjustPageTables((ulong)roundedP,size-1,false);
#endif
    return roundedP;
}

/****************************************************************************
REMARKS:
Free a block of locked memory.
****************************************************************************/
void PMAPI PM_freeLockedMem(void *p,uint size,ibool contiguous)
{
#ifndef REALMODE
#ifndef NO_PMHELP
    VXD_regs        regs;
#endif
    PM_lockHandle   lh; /* Unused in DOS */

    if (!p)
        return;
#ifndef NO_PMHELP
    if (VXD_version) {
        memset(&regs,0,sizeof(regs));
        regs.eax = API_NUM(PMHELP_FREELOCKED);
        regs.ebx = (ulong)p;
        regs.ecx = size;
        regs.edx = contiguous;
        _PM_VxDCall(&regs,_PM_VXD_off,_PM_VXD_sel);
        return;
        }
#endif
    PM_unlockDataPages(p,size,&lh);
    if (!contiguous)
        free(*((void**)((uchar*)p + size)));
    else
#endif
        PM_freeRealSeg(*((void**)((char*)p + size)));
}

#ifndef REALMODE
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
#endif

/****************************************************************************
REMARKS:
Allocates a page aligned and page sized block of memory
****************************************************************************/
void * PMAPI PM_allocPage(
    ibool locked)
{
#ifndef REALMODE
    pageblock       *block;
    void            *p;
    PM_lockHandle   lh; /* Unused in DOS */
#ifndef NO_PMHELP
    VXD_regs        regs;

    /* Call the helper VxD for this service if we are running in a DOS box */
    if (VXD_version) {
        memset(&regs,0,sizeof(regs));
        regs.eax = API_NUM(PMHELP_ALLOCPAGE);
        regs.ebx = locked;
        _PM_VxDCall(&regs,_PM_VXD_off,_PM_VXD_sel);
        return (void*)regs.eax;
        }
#endif

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
    if (locked)
        PM_lockDataPages(p,PM_PAGE_SIZE,&lh);
    return p;
#else
    return NULL;
#endif
}

/****************************************************************************
REMARKS:
Free a page aligned and page sized block of memory
****************************************************************************/
void PMAPI PM_freePage(
    void *p)
{
#ifndef REALMODE
    pageblock   *block;
#ifndef NO_PMHELP
    VXD_regs    regs;

    /* Call the helper VxD for this service if we are running in a DOS box */
    if (VXD_version) {
        memset(&regs,0,sizeof(regs));
        regs.eax = API_NUM(PMHELP_FREEPAGE);
        regs.ebx = (ulong)p;
        _PM_VxDCall(&regs,_PM_VXD_off,_PM_VXD_sel);
        return;
        }
#endif

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
#else
    (void)p;
#endif
}

/*-------------------------------------------------------------------------*/
/* DOS Real Mode support.                                                  */
/*-------------------------------------------------------------------------*/

#ifdef REALMODE

#ifndef MK_FP
#define MK_FP(s,o)  ( (void far *)( ((ulong)(s) << 16) + \
                    (ulong)(o) ))
#endif

void * PMAPI PM_mapRealPointer(uint r_seg,uint r_off)
{ return MK_FP(r_seg,r_off); }

void * PMAPI PM_getBIOSPointer(void)
{
    return MK_FP(0x40,0);
}

void * PMAPI PM_getA0000Pointer(void)
{
    return MK_FP(0xA000,0);
}

void * PMAPI PM_mapPhysicalAddr(ulong base,ulong limit,ibool isCached)
{
    uint sel = base >> 4;
    uint off = base & 0xF;
    limit = limit;
    return MK_FP(sel,off);
}

void PMAPI PM_freePhysicalAddr(void *ptr,ulong limit)
{ ptr = ptr; }

ulong PMAPI PM_getPhysicalAddr(void *p)
{
    return ((((ulong)p >> 16) << 4) + (ushort)p);
}

ibool PMAPI PM_getPhysicalAddrRange(void *p,ulong length,ulong *physAddress)
{ return false; }

void * PMAPI PM_allocRealSeg(uint size,uint *r_seg,uint *r_off)
{
    /* Call malloc() to allocate the memory for us */
    void *p = PM_malloc(size);
    *r_seg = FP_SEG(p);
    *r_off = FP_OFF(p);
    return p;
}

void PMAPI PM_freeRealSeg(void *mem)
{
    if (mem) PM_free(mem);
}

int PMAPI PM_int86(int intno, RMREGS *in, RMREGS *out)
{
    return PM_int386(intno,in,out);
}

int PMAPI PM_int86x(int intno, RMREGS *in, RMREGS *out,
    RMSREGS *sregs)
{
    return PM_int386x(intno,in,out,sregs);
}

#endif

/*-------------------------------------------------------------------------*/
/* Phar Lap TNT DOS Extender support.                                      */
/*-------------------------------------------------------------------------*/

#ifdef TNT

#include <pldos32.h>
#include <pharlap.h>
#include <hw386.h>

static uchar *zeroPtr = NULL;

void * PMAPI PM_getBIOSPointer(void)
{
    if (!zeroPtr)
        zeroPtr = PM_mapPhysicalAddr(0,0xFFFFF,true);
    return (void*)(zeroPtr + 0x400);
}

void * PMAPI PM_getA0000Pointer(void)
{
    static void *bankPtr;
    if (!bankPtr)
        bankPtr = PM_mapPhysicalAddr(0xA0000,0xFFFF,true);
    return bankPtr;
}

void * PMAPI PM_mapPhysicalAddr(ulong base,ulong limit,ibool isCached)
{
    CONFIG_INF  config;
    ULONG       offset;
    int         err;
    ulong       baseAddr,baseOfs,newLimit;
    VXD_regs    regs;

    /* If we have connected to our helper VxD in a Windows DOS box, use
     * the helper VxD services to map memory instead of the DPMI services.
     * We do this because the helper VxD can properly disable caching
     * where necessary, which we can only do directly here if we are
     * running at ring 0 (ie: under real DOS).
     */
    if (VXD_version == -1)
        PM_init();
    if (VXD_version) {
        memset(&regs,0,sizeof(regs));
        regs.eax = API_NUM(PMHELP_MAPPHYS);
        regs.ebx = base;
        regs.ecx = limit;
        regs.edx = isCached;
        _PM_VxDCall(&regs,_PM_VXD_off,_PM_VXD_sel);
        return (void*)regs.eax;
        }

    /* Round the physical address to a 4Kb boundary and the limit to a
     * 4Kb-1 boundary before passing the values to TNT. If we round the
     * physical address, then we also add an extra offset into the address
     * that we return.
     */
    baseOfs = base & 4095;
    baseAddr = base & ~4095;
    newLimit = ((limit+baseOfs+1+4095) & ~4095)-1;
    _dx_config_inf(&config, (UCHAR*)&config);
    err = _dx_map_phys(config.c_ds_sel,baseAddr,(newLimit + 4095) / 4096,&offset);
    if (err == 130) {
        /* If the TNT function failed, we are running in a DPMI environment
         * and this function does not work. However we know how to handle
         * DPMI properly, so we use our generic DPMI functions to do
         * what the TNT runtime libraries can't.
         */
        return DPMI_mapPhysicalAddr(base,limit,isCached);
        }
    if (err == 0)
        return (void*)(offset + baseOfs);
    return NULL;
}

void PMAPI PM_freePhysicalAddr(void *ptr,ulong limit)
{
}

ulong PMAPI PM_getPhysicalAddr(void *p)
{ return 0xFFFFFFFFUL; }

ibool PMAPI PM_getPhysicalAddrRange(void *p,ulong length,ulong *physAddress)
{ return false; }

void * PMAPI PM_mapRealPointer(uint r_seg,uint r_off)
{
    if (!zeroPtr)
        zeroPtr = PM_mapPhysicalAddr(0,0xFFFFF);
    return (void*)(zeroPtr + MK_PHYS(r_seg,r_off));
}

void * PMAPI PM_allocRealSeg(uint size,uint *r_seg,uint *r_off)
{
    USHORT  addr,t;
    void    *p;

    if (_dx_real_alloc((size + 0xF) >> 4,&addr,&t) != 0)
        return 0;
    *r_seg = addr;                  /* Real mode segment address    */
    *r_off = 0;                     /* Real mode segment offset     */
    p = PM_mapRealPointer(*r_seg,*r_off);
    _PM_addRealModeBlock(p,addr);
    return p;
}

void PMAPI PM_freeRealSeg(void *mem)
{
    if (mem) _dx_real_free(_PM_findRealModeBlock(mem));
}

#define INDPMI(reg)     rmregs.reg = regs->reg
#define OUTDPMI(reg)    regs->reg = rmregs.reg

void PMAPI DPMI_int86(int intno, DPMI_regs *regs)
{
    SWI_REGS    rmregs;

    memset(&rmregs, 0, sizeof(rmregs));
    INDPMI(eax); INDPMI(ebx); INDPMI(ecx); INDPMI(edx); INDPMI(esi); INDPMI(edi);

    _dx_real_int(intno,&rmregs);

    OUTDPMI(eax); OUTDPMI(ebx); OUTDPMI(ecx); OUTDPMI(edx); OUTDPMI(esi); OUTDPMI(edi);
    regs->flags = rmregs.flags;
}

#define IN(reg)     rmregs.reg = in->e.reg
#define OUT(reg)    out->e.reg = rmregs.reg

int PMAPI PM_int86(int intno, RMREGS *in, RMREGS *out)
{
    SWI_REGS    rmregs;

    memset(&rmregs, 0, sizeof(rmregs));
    IN(eax); IN(ebx); IN(ecx); IN(edx); IN(esi); IN(edi);

    _dx_real_int(intno,&rmregs);

    OUT(eax); OUT(ebx); OUT(ecx); OUT(edx); OUT(esi); OUT(edi);
    out->x.cflag = rmregs.flags & 0x1;
    return out->x.ax;
}

int PMAPI PM_int86x(int intno, RMREGS *in, RMREGS *out,
    RMSREGS *sregs)
{
    SWI_REGS    rmregs;

    memset(&rmregs, 0, sizeof(rmregs));
    IN(eax); IN(ebx); IN(ecx); IN(edx); IN(esi); IN(edi);
    rmregs.es = sregs->es;
    rmregs.ds = sregs->ds;

    _dx_real_int(intno,&rmregs);

    OUT(eax); OUT(ebx); OUT(ecx); OUT(edx); OUT(esi); OUT(edi);
    sregs->es = rmregs.es;
    sregs->cs = rmregs.cs;
    sregs->ss = rmregs.ss;
    sregs->ds = rmregs.ds;
    out->x.cflag = rmregs.flags & 0x1;
    return out->x.ax;
}

#endif

/*-------------------------------------------------------------------------*/
/* Symantec C++ DOSX and FlashTek X-32/X-32VM support                      */
/*-------------------------------------------------------------------------*/

#if defined(DOSX) || defined(X32VM)

#ifdef  X32VM
#include <x32.h>

#define _x386_mk_protected_ptr(p)   _x32_mk_protected_ptr((void*)p)
#define _x386_free_protected_ptr(p) _x32_free_protected_ptr(p)
#define _x386_zero_base_ptr         _x32_zero_base_ptr
#else
extern void *_x386_zero_base_ptr;
#endif

void * PMAPI PM_mapRealPointer(uint r_seg,uint r_off)
{
    return (void*)((ulong)_x386_zero_base_ptr + MK_PHYS(r_seg,r_off));
}

void * PMAPI PM_allocRealSeg(uint size,uint *r_seg,uint *r_off)
{
    PMREGS  r;

    r.h.ah = 0x48;                  /* DOS function 48h - allocate mem  */
    r.x.bx = (size + 0xF) >> 4;     /* Number of paragraphs to allocate */
    PM_int386(0x21, &r, &r);        /* Call DOS extender                */
    if (r.x.cflag)
        return 0;                   /* Could not allocate the memory    */
    *r_seg = r.e.eax;
    *r_off = 0;
    return PM_mapRealPointer(*r_seg,*r_off);
}

void PMAPI PM_freeRealSeg(void *mem)
{
    /* Cannot de-allocate this memory */
    mem = mem;
}

#pragma pack(1)

typedef struct {
    ushort  intno;
    ushort  ds;
    ushort  es;
    ushort  fs;
    ushort  gs;
    ulong   eax;
    ulong   edx;
    } _RMREGS;

#pragma pack()

#define IN(reg)     regs.e.reg = in->e.reg
#define OUT(reg)    out->e.reg = regs.e.reg

int PMAPI PM_int86(int intno, RMREGS *in, RMREGS *out)
{
    _RMREGS rmregs;
    PMREGS  regs;
    PMSREGS pmsregs;

    rmregs.intno = intno;
    rmregs.eax = in->e.eax;
    rmregs.edx = in->e.edx;
    IN(ebx); IN(ecx); IN(esi); IN(edi);
    regs.x.ax = 0x2511;
    regs.e.edx = (uint)(&rmregs);
    PM_segread(&pmsregs);
    PM_int386x(0x21,&regs,&regs,&pmsregs);

    OUT(eax); OUT(ebx); OUT(ecx); OUT(esi); OUT(edi);
    out->x.dx = rmregs.edx;
    out->x.cflag = regs.x.cflag;
    return out->x.ax;
}

int PMAPI PM_int86x(int intno, RMREGS *in, RMREGS *out, RMSREGS *sregs)
{
    _RMREGS rmregs;
    PMREGS  regs;
    PMSREGS pmsregs;

    rmregs.intno = intno;
    rmregs.eax = in->e.eax;
    rmregs.edx = in->e.edx;
    rmregs.es = sregs->es;
    rmregs.ds = sregs->ds;
    IN(ebx); IN(ecx); IN(esi); IN(edi);
    regs.x.ax = 0x2511;
    regs.e.edx = (uint)(&rmregs);
    PM_segread(&pmsregs);
    PM_int386x(0x21,&regs,&regs,&pmsregs);

    OUT(eax); OUT(ebx); OUT(ecx); OUT(esi); OUT(edi);
    sregs->es = rmregs.es;
    sregs->ds = rmregs.ds;
    out->x.dx = rmregs.edx;
    out->x.cflag = regs.x.cflag;
    return out->x.ax;
}

void * PMAPI PM_getBIOSPointer(void)
{
    return (void*)((ulong)_x386_zero_base_ptr + 0x400);
}

void * PMAPI PM_getA0000Pointer(void)
{
    return (void*)((ulong)_x386_zero_base_ptr + 0xA0000);
}

void * PMAPI PM_mapPhysicalAddr(ulong base,ulong limit,ibool isCached)
{
    VXD_regs    regs;

    /* If we have connected to our helper VxD in a Windows DOS box, use
     * the helper VxD services to map memory instead of the DPMI services.
     * We do this because the helper VxD can properly disable caching
     * where necessary, which we can only do directly here if we are
     * running at ring 0 (ie: under real DOS).
     */
    if (VXD_version == -1)
        PM_init();
    if (VXD_version) {
        memset(&regs,0,sizeof(regs));
        regs.eax = API_NUM(PMHELP_MAPPHYS);
        regs.ebx = base;
        regs.ecx = limit;
        regs.edx = isCached;
        _PM_VxDCall(&regs,_PM_VXD_off,_PM_VXD_sel);
        return (void*)regs.eax;
        }

    if (base > 0x100000)
        return _x386_map_physical_address((void*)base,limit);
    return (void*)((ulong)_x386_zero_base_ptr + base);
}

void PMAPI PM_freePhysicalAddr(void *ptr,ulong limit)
{
    /* Mapping cannot be freed */
}

ulong PMAPI PM_getPhysicalAddr(void *p)
{ return 0xFFFFFFFFUL; }

ibool PMAPI PM_getPhysicalAddrRange(void *p,ulong length,ulong *physAddress)
{ return false; }

ulong _cdecl _X32_getPhysMem(void);

#endif

/*-------------------------------------------------------------------------*/
/* Borland's DPMI32, Watcom DOS4GW and DJGPP DPMI support routines         */
/*-------------------------------------------------------------------------*/

#if defined(DPMI32) || defined(DOS4GW) || defined(DJGPP)

static uchar *BIOSPtr = NULL;
static uchar *A000Ptr = NULL;

void * PMAPI PM_getBIOSPointer(void)
{
    if (!BIOSPtr)
        BIOSPtr = PM_mapPhysicalAddr(0x400,0xFFFF,true);
    return BIOSPtr;
}

void * PMAPI PM_getA0000Pointer(void)
{
    if (!A000Ptr)
        A000Ptr = PM_mapPhysicalAddr(0xA0000,0xFFFF,true);
    return A000Ptr;
}

void * PMAPI PM_mapPhysicalAddr(ulong base,ulong limit,ibool isCached)
{
#ifndef NO_PMHELP
    VXD_regs    regs;
#endif
    PMSREGS     sregs;
    ulong       DSBaseAddr;

#ifdef  DJGPP
    /* Enable near pointers for DJGPP V2 */
    __djgpp_nearptr_enable();
#endif

    /* Get the base address for the default DS selector */
    PM_segread(&sregs);
    DSBaseAddr = DPMI_getSelectorBase(sregs.ds);
    if ((base < 0x100000) && (DSBaseAddr == 0)) {
        /* DS is zero based, so we can directly access the first 1Mb of
         * system memory (like under DOS4GW).
         */
        return (void*)base;
        }

    /* If we have connected to our helper VxD in a Windows DOS box, use
     * the helper VxD services to map memory instead of the DPMI services.
     * We do this because the helper VxD can properly disable caching
     * where necessary, which we can only do directly here if we are
     * running at ring 0 (ie: under real DOS).
     */
#ifndef NO_PMHELP
    if (VXD_version == -1)
        PM_init();
    if (VXD_version) {
        memset(&regs,0,sizeof(regs));
        regs.eax = API_NUM(PMHELP_MAPPHYS);
        regs.ebx = base;
        regs.ecx = limit;
        regs.edx = isCached;
        _PM_VxDCall(&regs,_PM_VXD_off,_PM_VXD_sel);
        return (void*)(regs.eax - DSBaseAddr);
        }
#endif
    return DPMI_mapPhysicalAddr(base,limit,isCached);
}

void PMAPI PM_freePhysicalAddr(void *ptr,ulong limit)
{
    /* Mapping cannot be freed */
    (void)ptr;
    (void)limit;
}

ulong PMAPI PM_getPhysicalAddr(void *p)
{
    ulong   physAddr;
    if (!PM_getPhysicalAddrRange(p,1,&physAddr))
        return 0xFFFFFFFF;
    return physAddr | ((ulong)p & 0xFFF);
}

ibool PMAPI PM_getPhysicalAddrRange(
    void *p,
    ulong length,
    ulong *physAddress)
{
    ulong       pte;
    PMSREGS     sregs;
    ulong       DSBaseAddr;
#ifndef NO_PMHELP
    VXD_regs    regs;

    /* If we have connected to our helper VxD in a Windows DOS box, use the
     * helper VxD services to find the physical address of an address.
     */
    if (VXD_version) {
        memset(&regs,0,sizeof(regs));
        regs.eax = API_NUM(PMHELP_GETPHYSICALADDRRANGE);
        regs.ebx = (ulong)p;
        regs.ecx = (ulong)length;
        regs.edx = (ulong)physAddress;
        _PM_VxDCall(&regs,_PM_VXD_off,_PM_VXD_sel);
        return regs.eax;
        }
#endif

    /* Find base address for default DS selector */
    PM_segread(&sregs);
    DSBaseAddr = DPMI_getSelectorBase(sregs.ds);

    /* Otherwise directly access the page tables to determine the
     * physical memory address. Note that we touch the memory before
     * calling, otherwise the memory may not be paged in correctly.
     */
    pte = *((ulong*)p);
#ifdef  DOS4GW
    if (_PM_pagingEnabled() == 0) {
        int     count;
        ulong   linAddr = (ulong)p;

        /* When paging is disabled physical=linear */
        for (count = (length+0xFFF) >> 12; count > 0; count--) {
            *physAddress++ = linAddr;
            linAddr += 4096;
            }
        return true;
        }
    else if ((PDB = _PM_getPDB()) != 0 && DSBaseAddr == 0) {
        int     startPDB,endPDB,iPDB,startPage,endPage,start,end,iPage;
        ulong   pageTable,*pPageTable,linAddr = (ulong)p;
        ulong   limit = length-1;

        pPDB = (ulong*)DPMI_mapPhysicalToLinear(PDB,0xFFF);
        if (pPDB) {
            startPDB = (linAddr >> 22) & 0x3FFL;
            startPage = (linAddr >> 12) & 0x3FFL;
            endPDB = ((linAddr+limit) >> 22) & 0x3FFL;
            endPage = ((linAddr+limit) >> 12) & 0x3FFL;
            for (iPDB = startPDB; iPDB <= endPDB; iPDB++) {
                pageTable = pPDB[iPDB] & ~0xFFFL;
                pPageTable = (ulong*)DPMI_mapPhysicalToLinear(pageTable,0xFFF);
                start = (iPDB == startPDB) ? startPage : 0;
                end = (iPDB == endPDB) ? endPage : 0x3FFL;
                for (iPage = start; iPage <= end; iPage++)
                    *physAddress++ = (pPageTable[iPage] & ~0xFFF);
                }
            return true;
            }
        }
#endif
    return false;
}

void * PMAPI PM_mapRealPointer(uint r_seg,uint r_off)
{
    static uchar *zeroPtr = NULL;

    if (!zeroPtr)
        zeroPtr = PM_mapPhysicalAddr(0,0xFFFFF,true);
    return (void*)(zeroPtr + MK_PHYS(r_seg,r_off));
}

void * PMAPI PM_allocRealSeg(uint size,uint *r_seg,uint *r_off)
{
    PMREGS      r;
    void        *p;

    r.x.ax = 0x100;                 /* DPMI allocate DOS memory         */
    r.x.bx = (size + 0xF) >> 4;     /* number of paragraphs             */
    PM_int386(0x31, &r, &r);
    if (r.x.cflag)
        return NULL;                /* DPMI call failed                 */
    *r_seg = r.x.ax;                /* Real mode segment                */
    *r_off = 0;
    p = PM_mapRealPointer(*r_seg,*r_off);
    _PM_addRealModeBlock(p,r.x.dx);
    return p;
}

void PMAPI PM_freeRealSeg(void *mem)
{
    PMREGS  r;

    if (mem) {
        r.x.ax = 0x101;                     /* DPMI free DOS memory         */
        r.x.dx = _PM_findRealModeBlock(mem);/* DX := selector from 0x100    */
        PM_int386(0x31, &r, &r);
        }
}

void PMAPI DPMI_int86(int intno, DPMI_regs *regs)
{
    PMREGS      r;
    PMSREGS     sr;

    PM_segread(&sr);
    r.x.ax = 0x300;                 /* DPMI issue real interrupt    */
    r.h.bl = intno;
    r.h.bh = 0;
    r.x.cx = 0;
    sr.es = sr.ds;
    r.e.edi = (uint)regs;
    PM_int386x(0x31, &r, &r, &sr);  /* Issue the interrupt          */
}

#define IN(reg)     rmregs.reg = in->e.reg
#define OUT(reg)    out->e.reg = rmregs.reg

int PMAPI PM_int86(int intno, RMREGS *in, RMREGS *out)
{
    DPMI_regs   rmregs;

    memset(&rmregs, 0, sizeof(rmregs));
    IN(eax); IN(ebx); IN(ecx); IN(edx); IN(esi); IN(edi);

    DPMI_int86(intno,&rmregs);      /* DPMI issue real interrupt    */

    OUT(eax); OUT(ebx); OUT(ecx); OUT(edx); OUT(esi); OUT(edi);
    out->x.cflag = rmregs.flags & 0x1;
    return out->x.ax;
}

int PMAPI PM_int86x(int intno, RMREGS *in, RMREGS *out,
    RMSREGS *sregs)
{
    DPMI_regs   rmregs;

    memset(&rmregs, 0, sizeof(rmregs));
    IN(eax); IN(ebx); IN(ecx); IN(edx); IN(esi); IN(edi);
    rmregs.es = sregs->es;
    rmregs.ds = sregs->ds;

    DPMI_int86(intno,&rmregs);      /* DPMI issue real interrupt    */

    OUT(eax); OUT(ebx); OUT(ecx); OUT(edx); OUT(esi); OUT(edi);
    sregs->es = rmregs.es;
    sregs->cs = rmregs.cs;
    sregs->ss = rmregs.ss;
    sregs->ds = rmregs.ds;
    out->x.cflag = rmregs.flags & 0x1;
    return out->x.ax;
}

#endif

/****************************************************************************
REMARKS:
Function to get the file attributes for a specific file.
****************************************************************************/
uint PMAPI PM_getFileAttr(
    const char *filename)
{
    // TODO: Implement this!
    return 0;
}

/****************************************************************************
REMARKS:
Function to get the file time and date for a specific file.
****************************************************************************/
ibool PMAPI PM_getFileTime(
    const char *filename,
    ibool gmTime,
    PM_time *time)
{
    // TODO: Implement this!
    return false;
}

/****************************************************************************
REMARKS:
Function to set the file time and date for a specific file.
****************************************************************************/
ibool PMAPI PM_setFileTime(
    const char *filename,
    ibool gmTime,
    PM_time *time)
{
    // TODO: Implement this!
    return false;
}

/****************************************************************************
REMARKS:
Do nothing for this OS.
****************************************************************************/
ulong PMAPI PM_setMaxThreadPriority(void)
{
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
Call our VxD under Windows to determine if we are running in a window.
****************************************************************************/
ibool PMAPI PM_runningInAWindow(void)
{
#ifndef NO_PMHELP
    VXD_regs    regs;

    if (PMHELP_connect()) {
        memset(&regs,0,sizeof(regs));
        regs.eax = API_NUM(PMHELP_RUNNINGINAWINDOW);
        _PM_VxDCall(&regs,_PM_VXD_off,_PM_VXD_sel);
        return regs.eax;
        }
#endif
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

