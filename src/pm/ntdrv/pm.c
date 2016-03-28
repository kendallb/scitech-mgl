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
#include "oshdr.h"

/*--------------------------- Global variables ----------------------------*/

char                _PM_cntPath[PM_MAX_PATH] = "";
char                _PM_SNAPPath[PM_MAX_PATH] = "";
ibool               _PM_sddIsActive = false;
static uchar *      lowMem = NULL;
static void (PMAPIP fatalErrorCleanup)(void) = NULL;

// Pointer to BIOS interrupt function in miniport driver.
int (PMAPIP _pPM_int86)(int intno,RMREGS *in,RMREGS *out) = NULL;
HANDLE (*pEngLoadImage)(LPWSTR pwszDriver) = NULL;
PVOID (*pEngFindImageProcAddress)(HANDLE hModule,LPSTR lpProcName) = NULL;
VOID (*pEngUnloadImage)(HANDLE hModule) = NULL;

static char *szNTWindowsKey     = "\\REGISTRY\\Machine\\Software\\Microsoft\\Windows NT\\CurrentVersion";
static char *szNTSystemRoot     = "SystemRoot";
static char *szMachineNameKey   = "\\REGISTRY\\Machine\\System\\CurrentControlSet\\control\\ComputerName\\ComputerName";
static char *szMachineNameKeyNT = "\\REGISTRY\\Machine\\System\\CurrentControlSet\\control\\ComputerName\\ActiveComputerName";
static char *szMachineName      = "ComputerName";

// Support for PM heart beat callbacks via NT timer events

#define MAX_HEART_BEAT_CALLBACKS    4
#define HEART_BEAT_MS               32

typedef struct {
    KTIMER                  kTimer;
    KDPC                    kTimerDpc;
    KEVENT                  kTimerEvent;
    HANDLE                  hDriverThread;
    ibool                   bThreadRunning;
    int                     numHeartBeatCallbacks;
    PM_heartBeat_cb         heartBeat[MAX_HEART_BEAT_CALLBACKS];
    void *                  heartBeatData[MAX_HEART_BEAT_CALLBACKS];
    } _PM_heartBeat_t;

static  _PM_heartBeat_t     *_PM_hb = NULL;
static  KEVENT              *_PM_pkLockEvent = NULL;

/*----------------------------- Implementation ----------------------------*/

/****************************************************************************
REMARKS:
PM_malloc override function for SNAP drivers loaded in VxD's.
****************************************************************************/
void * NTDRV_malloc(
    size_t size)
{
    return PM_mallocShared(size);
}

/****************************************************************************
REMARKS:
PM_calloc override function for SNAP drivers loaded in VxD's.
****************************************************************************/
void * NTDRV_calloc(
    size_t nelem,
    size_t size)
{
    void *p = PM_mallocShared(nelem * size);
    if (p)
        memset(p,0,nelem * size);
    return p;
}

/****************************************************************************
REMARKS:
PM_realloc override function for SNAP drivers loaded in VxD's.
****************************************************************************/
void * NTDRV_realloc(
    void *ptr,
    size_t size)
{
    void *p = PM_mallocShared(size);
    if (p) {
        memcpy(p,ptr,size);
        PM_freeShared(ptr);
        }
    return p;
}

/****************************************************************************
REMARKS:
PM_free override function for SNAP drivers loaded in VxD's.
****************************************************************************/
void NTDRV_free(
    void *p)
{
    PM_freeShared(p);
}

/****************************************************************************
REMARKS:
Initialise the PM library.
****************************************************************************/
void PMAPI PM_init(void)
{
    static ibool    inited = false;

    /* Exit if we are already inited */
    if (inited)
        return;

    /* Override the default memory allocators for all SNAP drivers
     * loaded in SDDHELP/PMHELP. We do this so that we can ensure all memory
     * dynamically allocated by SNAP drivers and internal C runtime
     * library functions are shared memory blocks that all processes
     * connecting to SDDHELP can see.
     */
    PM_useLocalMalloc(NTDRV_malloc,NTDRV_calloc,NTDRV_realloc,NTDRV_free);

    /* Initialiase the MTRR module */
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
    return _OS_WINNTDRV;
}

/****************************************************************************
REMARKS:
Return the name of the runtime environment.
****************************************************************************/
char * PMAPI PM_getOSName(void)
{
    return "NT Driver";
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
void PMAPI PM_backslash(char *s)
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
Handle fatal errors internally in the driver.
****************************************************************************/
void PMAPI PM_fatalError(
    const char *msg)
{
    ULONG   BugCheckCode = 0;
    ULONG   MoreBugCheckData[4] = {0};
    char    *p;
    ULONG   len;

    // Clean up the system first!
    if (fatalErrorCleanup)
        fatalErrorCleanup();

    // KeBugCheckEx brings down the system in a controlled
    // manner when the caller discovers an unrecoverable
    // inconsistency that would corrupt the system if
    // the caller continued to run.
    //
    // hack - dump the first 20 chars in hex using the variables
    //      provided - Each ULONG is equal to four characters...
    for(len = 0; len < 20; len++)
        if (msg[len] == (char)0)
            break;

    // This looks bad but it's quick and reliable...
    p = (char *)&BugCheckCode;
    if(len > 0) p[3] = msg[0];
    if(len > 1) p[2] = msg[1];
    if(len > 2) p[1] = msg[2];
    if(len > 3) p[0] = msg[3];

    p = (char *)&MoreBugCheckData[0];
    if(len > 4) p[3] = msg[4];
    if(len > 5) p[2] = msg[5];
    if(len > 6) p[1] = msg[6];
    if(len > 7) p[0] = msg[7];

    p = (char *)&MoreBugCheckData[1];
    if(len > 8) p[3] = msg[8];
    if(len > 9) p[2] = msg[9];
    if(len > 10) p[1] = msg[10];
    if(len > 11) p[0] = msg[11];

    p = (char *)&MoreBugCheckData[2];
    if(len > 12) p[3] = msg[12];
    if(len > 13) p[2] = msg[13];
    if(len > 14) p[1] = msg[14];
    if(len > 15) p[0] = msg[15];

    p = (char *)&MoreBugCheckData[3];
    if(len > 16) p[3] = msg[16];
    if(len > 17) p[2] = msg[17];
    if(len > 18) p[1] = msg[18];
    if(len > 19) p[0] = msg[19];

    // Halt the system!
    KeBugCheckEx(BugCheckCode, MoreBugCheckData[0], MoreBugCheckData[1], MoreBugCheckData[2], MoreBugCheckData[3]);
}

/****************************************************************************
REMARKS:
Return the current operating system path or working directory.
****************************************************************************/
char * PMAPI PM_getCurrentPath(
    char *path,
    int maxLen)
{
    strncpy(path,_PM_cntPath,maxLen);
    path[maxLen-1] = 0;
    return path;
}

/****************************************************************************
PARAMETERS:
szKey       - Key to query (can contain version number formatting)
szValue     - Value to get information for
value       - Place to store the registry key data read
size        - Size of the string buffer to read into

RETURNS:
true if the key was found, false if not.
****************************************************************************/
ibool REG_queryString(
    char *szKey,
    const char *szValue,
    char *value,
    DWORD size)
{
    ibool                           status;
    NTSTATUS                        rval;
    ULONG                           length;
    HANDLE                          Handle;
    OBJECT_ATTRIBUTES               keyAttributes;
    UNICODE_STRING                  *uniKey = NULL;
    UNICODE_STRING                  *uniValue = NULL;
    PKEY_VALUE_FULL_INFORMATION     fullInfo = NULL;
    STRING                          stringdata;
    UNICODE_STRING                  unidata;

    // Convert strings to UniCode
    status = false;
    if ((uniKey = _PM_CStringToUnicodeString(szKey)) == NULL)
        goto Exit;
    if ((uniValue = _PM_CStringToUnicodeString(szValue)) == NULL)
        goto Exit;

    // Open the key
    InitializeObjectAttributes( &keyAttributes,
                                uniKey,
                                OBJ_CASE_INSENSITIVE,
                                NULL,
                                NULL );
    rval = ZwOpenKey( &Handle,
                      KEY_ALL_ACCESS,
                      &keyAttributes );
    if (!NT_SUCCESS(rval))
        goto Exit;

    // Query the value
    length = sizeof (KEY_VALUE_FULL_INFORMATION)
           + size * sizeof(WCHAR);
    if ((fullInfo = ExAllocatePool (PagedPool, length)) == NULL)
        goto Exit;
    RtlZeroMemory(fullInfo, length);
    rval = ZwQueryValueKey (Handle,
                            uniValue,
                            KeyValueFullInformation,
                            fullInfo,
                            length,
                            &length);
    if (NT_SUCCESS (rval)) {
        // Create the UniCode string so we can convert it
        unidata.Buffer = (PWCHAR)(((PCHAR)fullInfo) + fullInfo->DataOffset);
        unidata.Length = (USHORT)fullInfo->DataLength;
        unidata.MaximumLength = (USHORT)fullInfo->DataLength + sizeof(WCHAR);

        // Convert unicode univalue to ansi string.
        rval = RtlUnicodeStringToAnsiString(&stringdata, &unidata, TRUE);
        if (NT_SUCCESS(rval)) {
            strcpy(value,stringdata.Buffer);
            status = true;
            }
        }

Exit:
    if (fullInfo) ExFreePool(fullInfo);
    if (uniKey) _PM_FreeUnicodeString(uniKey);
    if (uniValue) _PM_FreeUnicodeString(uniValue);
    return status;
}

/****************************************************************************
REMARKS:
Function to retrieve the system path for the OS. If we try to access the
registry during the early boot phase of the system
****************************************************************************/
static void PMAPI PM_getSystemRoot(
    char *path,
    int len)
{
    if (!REG_queryString(szNTWindowsKey,szNTSystemRoot,path,len)) {
        // Really early in the boot sequence the registry may not be
        // available, so we have no way to find the root of the
        // filesystem. In fact when this is the case we can't even
        // access the file system anyway, so I am not dealing with
        // solving this problem at the moment.

        // Use NT system root directory alias for early loading drivers.
        strcpy(path, "\\SystemRoot");
        }
}

/****************************************************************************
REMARKS:
Internal function to determine if the system has been booted or is presently
in the boot phase. We use a back door hack to determine this by checking
to see if we can access the registry for the NT system root key. If this
fails, then we know the system is booting as a partial registry is only
available for some parts of the boot sequence.
****************************************************************************/
ibool PMAPI PM_isSystemBooted(void)
{
    char path[256];
    return REG_queryString(szNTWindowsKey,szNTSystemRoot,path,sizeof(path));
}

/****************************************************************************
REMARKS:
Return the drive letter for the boot drive.
****************************************************************************/
char PMAPI PM_getBootDrive(void)
{
    char path[256];
    PM_getSystemRoot(path,sizeof(path));
    return path[0];
}

/****************************************************************************
REMARKS:
Return the path to the SNAP driver files.
****************************************************************************/
const char * PMAPI PM_getSNAPPath(void)
{
    static char path[256];

    if (strlen(_PM_SNAPPath) > 0) {
        strcpy(path,_PM_SNAPPath);
        PM_backslash(path);
        return path;
        }
    PM_getSystemRoot(path,sizeof(path));
    PM_backslash(path);
    strcat(path,"system32\\snap");
    return path;
}

/****************************************************************************
REMARKS:
Return the path to the SNAP configuration files.
****************************************************************************/
const char * PMAPI PM_getSNAPConfigPath(void)
{
    static char path[256];
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
    static char name[256];

    if (REG_queryString(szMachineNameKey,szMachineName,name,sizeof(name)))
        return name;
    if (REG_queryString(szMachineNameKeyNT,szMachineName,name,sizeof(name)))
        return name;
    return "UNKNOWN";
}

/****************************************************************************
REMARKS:
Check if a key has been pressed.
****************************************************************************/
int PMAPI PM_kbhit(void)
{
    // Not used in NT drivers
    return true;
}

/****************************************************************************
REMARKS:
Wait for and return the next keypress.
****************************************************************************/
int PMAPI PM_getch(void)
{
    // Not used in NT drivers
    return 0xD;
}

/****************************************************************************
REMARKS:
Open a console for output to the screen, creating the main event handling
window if necessary.
****************************************************************************/
PM_HWND PMAPI PM_openConsole(
    PM_HWND hwndUser,
    int device,
    int xRes,
    int yRes,
    int bpp,
    ibool fullScreen)
{
    // Not used in NT drivers
    (void)hwndUser;
    (void)device;
    (void)xRes;
    (void)yRes;
    (void)bpp;
    (void)fullScreen;
    return NULL;
}

/****************************************************************************
REMARKS:
Find the size of the console state buffer.
****************************************************************************/
int PMAPI PM_getConsoleStateSize(void)
{
    // Not used in NT drivers
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
    // Not used in NT drivers
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
    // Not used in NT drivers
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
    // Not used in NT drivers
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
    // Not used in NT drivers
    (void)hwndConsole;
}

/****************************************************************************
REMARKS:
Set the location of the OS console cursor.
****************************************************************************/
void PMAPI PM_setOSCursorLocation(
    int x,
    int y)
{
    /* Nothing to do for Windows */
    (void)x;
    (void)y;
}

/****************************************************************************
REMARKS:
Set the width of the OS console.
****************************************************************************/
void PMAPI PM_setOSScreenWidth(
    int width,
    int height)
{
    /* Nothing to do for Windows */
    (void)width;
    (void)height;
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
Sleep for the specified number of milliseconds.
****************************************************************************/
void PMAPI PM_sleep(
    ulong milliseconds)
{
    // We never use this in NT drivers
    (void)milliseconds;
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
        case 2: return 0x3E8;
        case 3: return 0x2E8;
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
OS specific shared libraries not supported inside an NT driver
****************************************************************************/
PM_MODULE PMAPI PM_loadLibrary(
    const char *szDLLName)
{
    UNICODE_STRING  *uniStr;
    PM_MODULE       *hMod;

    if (pEngLoadImage) {
        if ((uniStr = _PM_CStringToUnicodeString(szDLLName)) == NULL)
            return NULL;
        hMod = (PM_MODULE)pEngLoadImage(uniStr->Buffer);
        _PM_FreeUnicodeString(uniStr);
        return hMod;
        }
    return NULL;
}

/****************************************************************************
REMARKS:
OS specific shared libraries not supported inside an NT driver
****************************************************************************/
void * PMAPI PM_getProcAddress(
    PM_MODULE hModule,
    const char *szProcName)
{
    if (pEngFindImageProcAddress)
        return pEngFindImageProcAddress((HANDLE)hModule,(char*)szProcName);
    return NULL;
}

/****************************************************************************
REMARKS:
OS specific shared libraries not supported inside an NT driver
****************************************************************************/
void PMAPI PM_freeLibrary(
    PM_MODULE hModule)
{
    if (pEngUnloadImage)
        pEngUnloadImage((HANDLE)hModule);
}

/****************************************************************************
REMARKS:
IOPL is always enabled for NT drivers.
****************************************************************************/
int PMAPI PM_setIOPL(
    int level)
{
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
    // Not supported in NT drivers
    (void)drive;
    (void)dir;
    (void)len;
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
Function to change the file attributes for a specific file.
****************************************************************************/
void PMAPI PM_setFileAttr(
    const char *filename,
    uint attrib)
{
    NTSTATUS                status;
    ACCESS_MASK             DesiredAccess = GENERIC_READ | GENERIC_WRITE;
    OBJECT_ATTRIBUTES       ObjectAttributes;
    ULONG                   ShareAccess = FILE_SHARE_READ;
    ULONG                   CreateDisposition = FILE_OPEN;
    HANDLE                  FileHandle = NULL;
    UNICODE_STRING          *uniFile = NULL;
    IO_STATUS_BLOCK         IoStatusBlock;
    FILE_BASIC_INFORMATION  FileBasic;
    char                    kernelFilename[PM_MAX_PATH+5];
    ULONG                   FileAttributes = 0;

    // Convert file attribute flags
    if (attrib & PM_FILE_READONLY)
        FileAttributes |= FILE_ATTRIBUTE_READONLY;
    if (attrib & PM_FILE_ARCHIVE)
        FileAttributes |= FILE_ATTRIBUTE_ARCHIVE;
    if (attrib & PM_FILE_HIDDEN)
        FileAttributes |= FILE_ATTRIBUTE_HIDDEN;
    if (attrib & PM_FILE_SYSTEM)
        FileAttributes |= FILE_ATTRIBUTE_SYSTEM;

    // Add prefix for addressing the file system. "\??\" is short for "\DosDevices\"
    strcpy(kernelFilename, "\\??\\");
    strcat(kernelFilename, filename);

    // Convert filename string to ansi string
    if ((uniFile = _PM_CStringToUnicodeString(kernelFilename)) == NULL)
        goto Exit;

    // Must open a file to query it's attributes
    InitializeObjectAttributes (&ObjectAttributes,
                                uniFile,
                                OBJ_CASE_INSENSITIVE,
                                NULL,
                                NULL );
    status = ZwCreateFile( &FileHandle,
                            DesiredAccess | SYNCHRONIZE,
                            &ObjectAttributes,
                            &IoStatusBlock,
                            NULL,                  //AllocationSize  OPTIONAL,
                            FILE_ATTRIBUTE_NORMAL,
                            ShareAccess,
                            CreateDisposition,
                            FILE_RANDOM_ACCESS,        //CreateOptions,
                            NULL,                  //EaBuffer  OPTIONAL,
                            0                      //EaLength (required if EaBuffer)
                            );
    if (!NT_SUCCESS (status))
        goto Exit;

    // Query timestamps
    status = ZwQueryInformationFile(FileHandle,
                                    &IoStatusBlock,
                                    &FileBasic,
                                    sizeof(FILE_BASIC_INFORMATION),
                                    FileBasicInformation
                                    );
    if (!NT_SUCCESS (status))
        goto Exit;

    // Change the four bits we change
    FileBasic.FileAttributes &= ~(FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_ARCHIVE
                                  | FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM);
    FileBasic.FileAttributes |= FileAttributes;

    // Set timestamps
    ZwSetInformationFile(   FileHandle,
                            &IoStatusBlock,
                            &FileBasic,
                            sizeof(FILE_BASIC_INFORMATION),
                            FileBasicInformation
                            );

Exit:
    if (FileHandle) ZwClose(FileHandle);
    if (uniFile) _PM_FreeUnicodeString(uniFile);
    return;
}

/****************************************************************************
REMARKS:
Function to get the file attributes for a specific file.
****************************************************************************/
uint PMAPI PM_getFileAttr(
    const char *filename)
{
    NTSTATUS                status;
    ACCESS_MASK             DesiredAccess = GENERIC_READ | GENERIC_WRITE;
    OBJECT_ATTRIBUTES       ObjectAttributes;
    ULONG                   ShareAccess = FILE_SHARE_READ;
    ULONG                   CreateDisposition = FILE_OPEN;
    HANDLE                  FileHandle = NULL;
    UNICODE_STRING          *uniFile = NULL;
    IO_STATUS_BLOCK         IoStatusBlock;
    FILE_BASIC_INFORMATION  FileBasic;
    char                    kernelFilename[PM_MAX_PATH+5];
    ULONG                   FileAttributes = 0;
    uint                    retval = 0;

    // Add prefix for addressing the file system. "\??\" is short for "\DosDevices\"
    strcpy(kernelFilename, "\\??\\");
    strcat(kernelFilename, filename);

    // Convert filename string to ansi string
    if ((uniFile = _PM_CStringToUnicodeString(kernelFilename)) == NULL)
        goto Exit;

    // Must open a file to query it's attributes
    InitializeObjectAttributes (&ObjectAttributes,
                                uniFile,
                                OBJ_CASE_INSENSITIVE,
                                NULL,
                                NULL );
    status = ZwCreateFile( &FileHandle,
                           DesiredAccess | SYNCHRONIZE,
                           &ObjectAttributes,
                           &IoStatusBlock,
                           NULL,                  //AllocationSize  OPTIONAL,
                           FILE_ATTRIBUTE_NORMAL,
                           ShareAccess,
                           CreateDisposition,
                           FILE_RANDOM_ACCESS,        //CreateOptions,
                           NULL,                  //EaBuffer  OPTIONAL,
                           0                      //EaLength (required if EaBuffer)
                           );
    if (!NT_SUCCESS (status))
        goto Exit;

    // Query timestamps
    status = ZwQueryInformationFile(FileHandle,
                                    &IoStatusBlock,
                                    &FileBasic,
                                    sizeof(FILE_BASIC_INFORMATION),
                                    FileBasicInformation
                                    );
    if (!NT_SUCCESS (status))
        goto Exit;

    // Translate the file attributes
    if (FileBasic.FileAttributes & FILE_ATTRIBUTE_READONLY)
        retval |= PM_FILE_READONLY;
    if (FileBasic.FileAttributes & FILE_ATTRIBUTE_ARCHIVE)
        retval |= PM_FILE_ARCHIVE;
    if (FileBasic.FileAttributes & FILE_ATTRIBUTE_HIDDEN)
        retval |= PM_FILE_HIDDEN;
    if (FileBasic.FileAttributes & FILE_ATTRIBUTE_SYSTEM)
        retval |= PM_FILE_SYSTEM;

Exit:
    if (FileHandle) ZwClose(FileHandle);
    if (uniFile) _PM_FreeUnicodeString(uniFile);
    return retval;
}

/****************************************************************************
REMARKS:
Function to create a directory.
****************************************************************************/
ibool PMAPI PM_mkdir(
    const char *filename)
{
    // Not supported in NT drivers
    (void)filename;
    return false;
}

/****************************************************************************
REMARKS:
Function to remove a directory.
****************************************************************************/
ibool PMAPI PM_rmdir(
    const char *filename)
{
    // Not supported in NT drivers
    (void)filename;
    return false;
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
    // Not supported in NT drivers
    (void)filename;
    (void)gmTime;
    (void)time;
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
    // Not supported in NT drivers
    (void)filename;
    (void)gmTime;
    (void)time;
    return false;
}

/****************************************************************************
REMARKS:
Increase the thread priority to maximum, if possible.
****************************************************************************/
ulong PMAPI PM_setMaxThreadPriority(void)
{
    KIRQL oldIrql;
    KeRaiseIrql(DISPATCH_LEVEL+1,&oldIrql);
    return oldIrql;
}

/****************************************************************************
REMARKS:
Restore the original thread priority.
****************************************************************************/
void PMAPI PM_restoreThreadPriority(
    ulong oldPriority)
{
    KeLowerIrql(oldPriority);
}

/****************************************************************************
REMARKS:
Returns true if SDD is the active display driver in the system.
****************************************************************************/
ibool PMAPI PM_isSDDActive(void)
{
    return _PM_sddIsActive;
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
subsystem   - Subsystem to request access to (unused, 0 for now)
write       - True to enable a write lock, false for a read lock

REMARKS:
This function locks access to the internal SNAP device driver code.
****************************************************************************/
void PMAPI PM_lockSNAPAccess(
    int subsystem,
    ibool write)
{
    (void)subsystem;
    (void)write;

    // Create NT synchronization event object on first time called.
    // This is done separately from heartbeat callback initialization
    // since other callers may need serialized SNAP access as well.
    if (!_PM_pkLockEvent) {
        _PM_pkLockEvent = ExAllocatePool(NonPagedPool, sizeof(KEVENT));
        if (_PM_pkLockEvent == NULL)
            return;
        RtlZeroMemory(_PM_pkLockEvent, sizeof(KEVENT));
        KeInitializeEvent(_PM_pkLockEvent,SynchronizationEvent,TRUE);
        }

    // Synchronization Event will ordinarily be in signalled state
    // unless someone else got here before us. It will automatically
    // reset the event to its unsignalled state when the wait is over.
    KeWaitForSingleObject(_PM_pkLockEvent,Executive,KernelMode,FALSE,NULL);
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

    // Re-signal the synchronization event to allow subsequent access.
    KeSetEvent(_PM_pkLockEvent, 0, FALSE);
}

/****************************************************************************
REMARKS:
Heartbeat timeout DPC function for signalling callback thread.
****************************************************************************/
static void _PM_heartBeatTimeout(
    PKDPC Dpc,
    PVOID Context,
    PVOID Param1,
    PVOID Param2)
{
    _PM_heartBeat_t *hb = (_PM_heartBeat_t*)Context;

    // Signal DPC timeout event to wake up heartbeat thread
    KeSetEvent(&_PM_hb->kTimerEvent,0,FALSE);
}

/****************************************************************************
REMARKS:
Main driver thread for executing all registered heartbeat callbacks.
Win2K/XP does not allow waiting for semaphores inside DPCs, so instead the
DPC must signal a timeout event to wake up a driver helper thread.
****************************************************************************/
VOID _PM_heartBeatThread(
    PVOID pContext)
{
    int i;
    _PM_heartBeat_t *hb = (_PM_heartBeat_t*)pContext;

    while (hb->bThreadRunning) {
        // Yield thread until DPC timer signalled state
        KeWaitForSingleObject(&_PM_hb->kTimerEvent,Executive,KernelMode,FALSE,NULL);

        // Lock the SNAP subsystem and process all the registered callbacks
        PM_lockSNAPAccess(-1,true);
        for (i = 0; i < hb->numHeartBeatCallbacks; i++)
            (*hb->heartBeat[i])(hb->heartBeatData[i]);
        PM_unlockSNAPAccess(-1);

        // Reset signal for next DPC timeout
        KeResetEvent(&_PM_hb->kTimerEvent);
        }

    // Exit driver thread
    PsTerminateSystemThread(STATUS_SUCCESS);
}

/****************************************************************************
REMARKS:
Function to register a driver heart beat callback function. The first
function that is called sets the interval for all the callback functions
and they will be called in the order they were registered. This function
will implement this mechanism in whatever way is appropriate for the
device driver environment.

Note that currently there is no mechanism to specify the timer intervals at
run-time, so we use a pre-determined value of 32 milliseconds that will be
useful for NT display driver polling and DPVL update functions.
****************************************************************************/
void PMAPI PM_registerHeartBeatCallback(
    PM_heartBeat_cb cb,
    void *data)
{
    // Kernel objects must always be resident in memory
    if (_PM_hb == NULL) {
        _PM_hb = ExAllocatePool(NonPagedPool, sizeof(_PM_heartBeat_t));
        if (_PM_hb == NULL)
            return;
        RtlZeroMemory(_PM_hb, sizeof(_PM_heartBeat_t));
        }

    // If first time called, start periodic timer (pre-determined intervals)
    if (_PM_hb->numHeartBeatCallbacks == 0) {
        KeInitializeTimer(&_PM_hb->kTimer);
        KeInitializeDpc(&_PM_hb->kTimerDpc,_PM_heartBeatTimeout,(void*)_PM_hb);
        KeSetTimerEx(&_PM_hb->kTimer,RtlConvertLongToLargeInteger(-10000*HEART_BEAT_MS),
            HEART_BEAT_MS,&_PM_hb->kTimerDpc);
        KeInitializeEvent(&_PM_hb->kTimerEvent,NotificationEvent,FALSE);
        // Callbacks will be executed within driver helper thread, not DPC
        _PM_hb->bThreadRunning = true;
        PsCreateSystemThread(&_PM_hb->hDriverThread,THREAD_ALL_ACCESS,NULL,
            NULL,NULL,_PM_heartBeatThread,(void*)_PM_hb);
        }

    // Add heart beat callback to list
    PM_lockSNAPAccess(-1,true);
    if (_PM_hb->numHeartBeatCallbacks < MAX_HEART_BEAT_CALLBACKS) {
        _PM_hb->heartBeat[_PM_hb->numHeartBeatCallbacks] = cb;
        _PM_hb->heartBeatData[_PM_hb->numHeartBeatCallbacks] = data;
        _PM_hb->numHeartBeatCallbacks++;
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

    // Remove heart beat callback from list
    for (i = 0; i < _PM_hb->numHeartBeatCallbacks; i++) {
        if (_PM_hb->heartBeat[i] == cb) {
            PM_lockSNAPAccess(-1,true);
            if (i < _PM_hb->numHeartBeatCallbacks-1) {
                RtlMoveMemory(&_PM_hb->heartBeat[i],&_PM_hb->heartBeat[i+1], sizeof(_PM_hb->heartBeat[i]) * (_PM_hb->numHeartBeatCallbacks-1 - i));
                RtlMoveMemory(&_PM_hb->heartBeatData[i],&_PM_hb->heartBeatData[i+1],sizeof(_PM_hb->heartBeatData[i]) * (_PM_hb->numHeartBeatCallbacks-1 - i));
                }
            _PM_hb->numHeartBeatCallbacks--;
            PM_unlockSNAPAccess(-1);
            break;
            }
        }

    // If last time called, kill periodic timer and driver thread
    if (_PM_hb->numHeartBeatCallbacks == 0) {
        PM_lockSNAPAccess(-1,true);
        KeCancelTimer(&_PM_hb->kTimer);
        // PsTerminateSystemThread can only be called within driver thread context
        _PM_hb->bThreadRunning = false;
        PM_unlockSNAPAccess(-1);
        }
}

