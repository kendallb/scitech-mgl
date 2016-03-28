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
* Environment:  Win32
*
* Description:  Implementation for the OS Portability Manager Library, which
*               contains functions to implement OS specific services in a
*               generic, cross platform API. Porting the OS Portability
*               Manager library is the first step to porting any SciTech
*               products to a new platform.
*
****************************************************************************/

#define WIN32_LEAN_AND_MEAN
#define STRICT
#include <windows.h>
#include <mmsystem.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef  __GNUC__
#include <dirent.h>
#else
#include <direct.h>
#endif
#include "pmapi.h"
#include "clib/os/os.h"
#include "pm_help.h"

/*--------------------------- Global variables ----------------------------*/

/* {secret} */
ibool           _PM_haveWinNT;      /* True if we are running on NT     */
/* {secret} */
HANDLE          _PM_hDevice = INVALID_HANDLE_VALUE;
/* {secret} */
uint            PMHELP_version;     /* Version of PMHELP driver loaded  */
static ibool    inited = false;     /* Flags if we are initialised      */
static ibool    useSDDHELP = true;  /* We always use SDDHELP by default */
static ibool    isSDDHELP = false;  /* SDDHELP driver actually loaded?  */
static void     (PMAPIP fatalErrorCleanup)(void) = NULL;

static char *szMachineNameKey   = "System\\CurrentControlSet\\control\\ComputerName\\ComputerName";
static char *szMachineNameKeyNT = "System\\CurrentControlSet\\control\\ComputerName\\ActiveComputerName";
static char *szMachineName      = "ComputerName";

/*----------------------------- Implementation ----------------------------*/

/* Macro to check for a valid, loaded version of PMHELP. We check this
 * on demand when we need these services rather than when PM_init() is
 * called because if we are running on DirectDraw we don't need PMHELP.VXD.
 */

#define CHECK_FOR_PMHELP()                                                      \
{                                                                               \
    if (_PM_hDevice == INVALID_HANDLE_VALUE) {                                  \
        if (_PM_haveWinNT)                                                      \
            PM_fatalError("Unable to connect to PMHELP.SYS or SDDHELP.SYS!");   \
        else                                                                    \
            PM_fatalError("Unable to connect to PMHELP.VXD or SDDHELP.VXD!");   \
        }                                                                       \
}

/****************************************************************************
DESCRIPTION:
Initialise the PM library

HEADER:
pmapi.h

REMARKS:
Initialise the PM library and connect to our helper device driver. If we
cannot connect to our helper device driver, we bail out with an error
message. Our Windows 9x VxD is dynamically loadable, so it can be loaded
after the system has started. On Windows NT/2000/XP the device driver
sevice must be installed first during application installation by
a system administrator. On OS/2 the SDDHELP.SYS driver must be installed
in the CONFIG.SYS file prior to use. On other platforms this function
does not usually require an external device driver and just initialises
the PM library internals.
****************************************************************************/
void PMAPI PM_init(void)
{
    DWORD   inBuf[1];   /* Buffer to receive data from VxD  */
    DWORD   outBuf[1];  /* Buffer to receive data from VxD  */
    DWORD   count;      /* Count of bytes returned from VxD */
    char    cntPath[PM_MAX_PATH];
    char    *env;
    uint    SDDHELP_version = 0;
    HANDLE  _SDD_hDevice = INVALID_HANDLE_VALUE;

    /* Create a file handle for the static VxD if possible, otherwise
     * dynamically load the PMHELP helper VxD. Note that if an old version
     * of SDD is loaded, we use the PMHELP VxD instead.
     */
    if (!inited) {
        /* Determine if we are running under Windows NT or not and
         * set the global OS type variable.
         */
        _PM_haveWinNT = false;
        if (PM_getOSType() != _OS_WIN95)
            _PM_haveWinNT = true;

        /* Now try to connect to SDDHELP.VXD or SDDHELP.SYS and get the version number */
        if (useSDDHELP) {
            _SDD_hDevice = CreateFile(SDDHELP_MODULE_PATH,GENERIC_READ | GENERIC_WRITE,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
            if (_SDD_hDevice != INVALID_HANDLE_VALUE) {
                DeviceIoControl(_SDD_hDevice, PMHELP_GETVER32, NULL, 0, outBuf, sizeof(outBuf), &count, NULL);
                SDDHELP_version = outBuf[0];
                isSDDHELP = true;
                }
            }

        /* First try to see if there is a currently loaded PMHELP driver.
         * This is usually the case when we are running under Windows NT/2K.
         */
        _PM_hDevice = CreateFile(PMHELP_MODULE_PATH,GENERIC_READ | GENERIC_WRITE,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
        if (_PM_hDevice == INVALID_HANDLE_VALUE) {
            /* The driver was not staticly loaded, so try creating a file handle
             * to a dynamic version of the VxD if possible. Note that on WinNT/2K we
             * cannot support dynamically loading the drivers.
             */
            _PM_hDevice = CreateFile(PMHELP_VXD_PATH, 0,0,0, CREATE_NEW, FILE_FLAG_DELETE_ON_CLOSE, 0);
            if (_PM_hDevice == INVALID_HANDLE_VALUE) {
                /* If no PMHELP driver is found, use the SDDHELP driver if it was found */
                _PM_hDevice = _SDD_hDevice;
                /* This next step prevents closing SDDHELP driver below */
                _SDD_hDevice = INVALID_HANDLE_VALUE;
                SDDHELP_version = 0;
                }
            }

        /* If we found either or both device drivers, initialise them */
        if (_PM_hDevice != INVALID_HANDLE_VALUE) {
            /* Call the driver to determine the version number */
            DeviceIoControl(_PM_hDevice, PMHELP_GETVER32, inBuf, sizeof(inBuf), outBuf, sizeof(outBuf), &count, NULL);
            PMHELP_version = outBuf[0];
            if (SDDHELP_version >= PMHELP_version) {
                /* If both SDDHELP and PMHELP are found, use the one with the later version
                 * number (we prefer SDDHELP if the versions are the same).
                 */
                CloseHandle(_PM_hDevice);
                _PM_hDevice = _SDD_hDevice;
                PMHELP_version = SDDHELP_version;
                }
            else if (_SDD_hDevice != INVALID_HANDLE_VALUE) {
                /* Close separate SDDHELP handle as PMHELP is a later version */
                CloseHandle(_SDD_hDevice);
                isSDDHELP = false;
                }

            /* Now set the current path inside the VxD so it knows what the
             * current directory is for loading SNAP drivers.
             */
            inBuf[0] = (ulong)PM_getCurrentPath(cntPath,sizeof(cntPath));
            if (!DeviceIoControl(_PM_hDevice, PMHELP_SETCNTPATH32, inBuf, sizeof(inBuf), outBuf, sizeof(outBuf), &count, NULL))
                PM_fatalError("Unable to set VxD current path!");

            /* Now pass down the SNAP_PATH environment variable to the device
             * driver so it can use this value if it is found.
             */
            if ((env = getenv("SNAP_PATH")) != NULL) {
                inBuf[0] = (ulong)env;
                if (!DeviceIoControl(_PM_hDevice, PMHELP_SETSNAPPATH32, inBuf, sizeof(inBuf), outBuf, sizeof(outBuf), &count, NULL))
                    PM_fatalError("Unable to set VxD SNAP path!");
                }

            /* Enable IOPL for ring-3 code by default if driver is present */
            if (_PM_haveWinNT)
                PM_setIOPL(3);
            }

        /* Indicate that we have been initialised */
        inited = true;
        }
}

/****************************************************************************
DESCRIPTION:
Set the I/O priveledge level for the current process

HEADER:
pmapi.h

PARAMETERS:
iopl    - New IOPL to make active (0 - 3)

RETURNS:
Previous IOPL active before the change was made

REMARKS:
This function is used to change the I/O privledge level of the current
process, so that it can access I/O ports directly. This works on all supported
OS'es to date, even on OS/2 and Windows NT/2000/XP, provided you have the
necessary kernel level drivers or services installed.

SEE ALSO:
PM_getIOPL
****************************************************************************/
int PMAPI PM_setIOPL(
    int iopl)
{
    DWORD       inBuf[1];   /* Buffer to receive data from VxD  */
    DWORD       outBuf[1];  /* Buffer to receive data from VxD  */
    DWORD       count;      /* Count of bytes returned from VxD */
    static int  cntIOPL = 0;
    int         oldIOPL = cntIOPL;

    /* Enable I/O by adjusting the I/O permissions map on Windows NT */
    if (_PM_haveWinNT) {
        CHECK_FOR_PMHELP();
        if (iopl == 3)
            DeviceIoControl(_PM_hDevice, PMHELP_ENABLERING3IOPL32, inBuf, sizeof(inBuf),outBuf, sizeof(outBuf), &count, NULL);
        else
            DeviceIoControl(_PM_hDevice, PMHELP_DISABLERING3IOPL32, inBuf, sizeof(inBuf),outBuf, sizeof(outBuf), &count, NULL);
        cntIOPL = iopl;
        return oldIOPL;
        }

    /* We always have IOPL on Windows 9x */
    return 3;
}

/****************************************************************************
DESCRIPTION:
Get the I/O priveledge level for the process

HEADER:
pmapi.h

RETURNS:
Current IOPL active for the process

REMARKS:
This function is used to obtain the I/O privledge level of the current
process.

SEE ALSO:
PM_setIOPL
****************************************************************************/
int PMAPI PM_getIOPL(void);

/****************************************************************************
REMARKS:
Enable/Disable the file filter driver in Windows NT.
{secret}
****************************************************************************/
void PMAPI PM_enableFileFilter(
    ibool enable)
{
    DWORD       inBuf[1];   /* Buffer to receive data from VxD  */
    DWORD       outBuf[1];  /* Buffer to receive data from VxD  */
    DWORD       count;      /* Count of bytes returned from VxD */

    /* Enable/Disable file filter via PM device driver IOCTL */
    PM_init();
    if (_PM_haveWinNT) {
        CHECK_FOR_PMHELP();
        if (enable)
            DeviceIoControl(_PM_hDevice, PMHELP_ENABLEFILEFILTER32, inBuf, sizeof(inBuf),outBuf, sizeof(outBuf), &count, NULL);
        else
            DeviceIoControl(_PM_hDevice, PMHELP_DISABLEFILEFILTER32, inBuf, sizeof(inBuf),outBuf, sizeof(outBuf), &count, NULL);
        }
}

/****************************************************************************
DESCRIPTION:
Determines if access to the real mode BIOS is available

HEADER:
pmapi.h

RETURNS:
True if the system provides BIOS access, false if not.

REMARKS:
This function is used to determine if the operating system can provide
access to the real mode BIOS or not. Many operating systems can provide
full access (DOS, Windows 9x, Linux), others can provide limited access
(OS/2) while other still provide no access (Windows NT/2000/XP, QNX etc).

If you need to call the PM_int86 functions, you can first call this
function to determine if the BIOS access is available or not. If this
function returns false, do *not* call the PM_int86 functions!
****************************************************************************/
ibool PMAPI PM_haveBIOSAccess(void)
{
    if (PM_getOSType() != _OS_WIN95)
        return false;
    else
        return _PM_hDevice != INVALID_HANDLE_VALUE;
}

/****************************************************************************
DESCRIPTION:
Return the operating system type identifier.

HEADER:
pmapi.h

RETURNS:
Flag representing the OS type

REMARKS:
This function returns a flag that represents the operating system type,
so that binary portable code that does need to handle OS dependencies
internally can do so with runtime checks if necessary. Please see the
%SCITECH%\include\drvlib\os\os.h header file for the current definition
of operating system types supported. This list will grow as more
operating systems are supported by the PM library.

SEE ALSO:
PM_getOSName
****************************************************************************/
long PMAPI PM_getOSType(void)
{
    if ((GetVersion() & 0x80000000UL) == 0) {
        if ((GetVersion() & 0x000000FFUL) > 4)
            return ___drv_os_type = _OS_WIN2K;
        else
            return ___drv_os_type = _OS_WINNT4;
        }
    else
        return ___drv_os_type = _OS_WIN95;
}

/****************************************************************************
DESCRIPTION:
Return the name of the operating system environment.

HEADER:
pmapi.h

RETURNS:
String representing the operating system name

REMARKS:
This function returns a string representation of the name of the runtime
operating system environment. This is useful for binary portable code
that needs to display or log the operating system name for informational
purposes.

SEE ALSO:
PM_getOSType
****************************************************************************/
char * PMAPI PM_getOSName(void)
{
    if ((GetVersion() & 0x80000000UL) == 0) {
        // TODO: Add code to detect versions of the OS!
        return "Windows NT/2000/XP";
        }
    else {
        // TODO: Add code to detect the versions of the OS!
        return "Windows 95/98/Me";
        }
}

/****************************************************************************
REMARKS:
Obsolete. Return the runtime type identifier.
{secret}
****************************************************************************/
int PMAPI PM_getModeType(void)
{
    return PM_386;
}

/****************************************************************************
DESCRIPTION:
Add a file directory separator to the end of the filename.

HEADER:
pmapi.h

PARAMETERS:
s   - String to append the directory separator character to

REMARKS:
This function is a portable way to add a file directory separator to the end
of the filename. The separator added will always work on the target platform,
and is used extensively by binary portable modules that need to construct
path names dynamically that will work properly on the target operating
system.
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
DESCRIPTION:
Add a user defined PM_fatalError cleanup function.

HEADER:
pmapi.h

PARAMETERS:
cleanup - New fatal error cleanup function to use

REMARKS:
This function is provided to allow a user defined fatal error cleanup
function to be registered. If any code call PM_fatalError, this cleanup
function will be called first, allowing the cleanup function to put the
operating system back into a valid state before displaying the fatal
error message.

SEE ALSO:
PM_fatalError
****************************************************************************/
void PMAPI PM_setFatalErrorCleanup(
    void (PMAPIP cleanup)(void))
{
    fatalErrorCleanup = cleanup;
}

/****************************************************************************
DESCRIPTION:
Report a fatal error condition and halt the program.

HEADER:
pmapi.h

PARAMETERS:
msg - Message to display as the fatal error prior to exit

REMARKS:
This function is a portable method to report a fatal error condition and
then halt program execution. It will display the error message using
whatever mechanism is appropriate for the operating system. ie: A console
text message in DOS, OS/2, Linux etc, or a popup dialog box for a GUI based
environment like Windows, PMSHELL or X11.

SEE ALSO:
PM_setFatalErrorCleanup
****************************************************************************/
void PMAPI PM_fatalError(
    const char *msg)
{
    // TODO: For wxWindows apps we need to display the message box before
    //       we call the cleanup function. Ideally we should change the
    //       entire fatal error handling system to allow for the
    //       cleanup function itself to display the message box before
    //       the app is exited.
    if (fatalErrorCleanup)
        fatalErrorCleanup();
    MessageBox(NULL,msg,"Fatal Error!", MB_ICONEXCLAMATION);
    exit(1);
}

/****************************************************************************
DESCRIPTION:
Allocate the real mode VESA transfer buffer for communicating with the BIOS.

HEADER:
pmapi.h

PARAMETERS:
len     - Place to store the length of the VESA buffer
rseg    - Place to store the real mode segment of the VESA buffer
roff    - Place to store the real mode offset of the VESA buffer

RETURNS:
Pointer to the transfer buffer on success, NULL on failure.

REMARKS:
This function is used to allocate the real mode VESA transfer buffer for
communicating with the underlying real mode Video BIOS. If the operating
system cannot support accessing the VESA BIOS functions, this function
will return NULL. If this function does succeed, the length of the buffer
will be returned in the len parameter while the real mode segment and offset
of the buffer will be returned in the rseg and roff parameters. A regular
C pointer to the buffer is returned directly and can be used to read and
write data from the transfer buffer.
****************************************************************************/
void * PMAPI PM_getVESABuf(
    uint *len,
    uint *rseg,
    uint *roff)
{
    /* Not supported in Win32 */
    (void)len;
    (void)rseg;
    (void)roff;
    return NULL;
}

/****************************************************************************
DESCRIPTION:
Check if a key has been pressed.

HEADER:
pmapi.h

RETURNS:
True if a key was pressed, false if not.

REMARKS:
This function check if a key has been pressed. This function is valid only
for operating systems that support running in console modes (DOS, Linux,
OS/2 etc).
****************************************************************************/
int PMAPI PM_kbhit(void)
{
    /* Not used in Windows */
    return true;
}

/****************************************************************************
DESCRIPTION:
Wait for and return the next keypress.

HEADER:
pmapi.h

RETURNS:
ASCII code for the key that was pressed

REMARKS:
This function waits for and returns the next keypress, and returns the
ASCII code of the key that was pressed. This function is valid only
for operating systems that support running in console modes (DOS, Linux,
OS/2 etc).
****************************************************************************/
int PMAPI PM_getch(void)
{
    /* Not used in Windows */
    return 0xD;
}

/****************************************************************************
DESCRIPTION:
Set the location of the OS text mode console cursor.

HEADER:
pmapi.h

PARAMETERS:
x   - New console cursor X coordinate
y   - New console cursor Y coordinate

REMARKS:
This function is used to set the location of the OS text mode console
cursor. This function is valid only for operating systems that support
running in console modes (DOS, Linux, OS/2 etc).
****************************************************************************/
void PM_setOSCursorLocation(
    int x,
    int y)
{
    /* Nothing to do for Windows */
    (void)x;
    (void)y;
}

/****************************************************************************
DESCRIPTION:
Set the dimensions of the OS text mode console.

HEADER:
pmapi.h

PARAMETERS:
width   - New width of the OS text mode console
height  - New height of the OS text mode console

REMARKS:
This function set the width and height of the OS text mode console. This
should be done if some method other than OS provided functions is used to
change the console mode (ie: 80x50 or 80x60 instead of 80x25), so that
the operating system functions themselves will know how to output to the
new mode. This function is valid only for operating systems that support
running in console modes (DOS, Linux, OS/2 etc).
****************************************************************************/
void PM_setOSScreenWidth(
    int width,
    int height)
{
    /* Nothing to do for Windows */
    (void)width;
    (void)height;
}

/****************************************************************************
DESCRIPTION:
Set the real time clock handler (used for software stereo modes).

HEADER:
pmapi.h

PARAMETERS:
ih          - New C based interrupt handler to install
frequency   - New frequency to program the RTC to run at

RETURNS:
True on success, false on failure.

REMARKS:
This function is used to set the real time clock handler that is used for
software stereo modes by the SNAP Graphics drivers. This is presently
only supported under DOS, Windows 9x and Windows NT/2000/XP environments.
It is also not supported by general application programs, only by
device driver environments (ie: Win32 apps cannot use this function,
only Windows 9x VxD drivers or Windows NT style kernel drivers).

SEE ALSO:
PM_setRealTimeClockFrequency, PM_stopRealTimeClock, PM_restartRealTimeClock,
PM_restoreRealTimeClockHandler
****************************************************************************/
ibool PMAPI PM_setRealTimeClockHandler(
    PM_intHandler ih,
    int frequency)
{
    /* We do not support this from Win32 programs. Rather the VxD handles
     * this stuff it will take care of hooking the stereo flip functions at
     * the VxD level.
     */
    (void)ih;
    (void)frequency;
    return false;
}

/****************************************************************************
DESCRIPTION:
Set the real time clock frequency (for stereo modes).

HEADER:
pmapi.h

PARAMETERS:
frequency   - New frequency to program the RTC to run at

REMARKS:
This function is used to change the real time clock frequency that is used
for software stereo modes by the SNAP Graphics drivers. The interrupt
handler must first be installed with the PM_setRealTimeClockHandler
function.

SEE ALSO:
PM_setRealTimeClockHandler, PM_stopRealTimeClock, PM_restartRealTimeClock,
PM_restoreRealTimeClockHandler
****************************************************************************/
void PMAPI PM_setRealTimeClockFrequency(
    int frequency)
{
    /* Not supported under Win32 */
    (void)frequency;
}

/****************************************************************************
DESCRIPTION:
Stops the real time clock from ticking

HEADER:
pmapi.h

REMARKS:
This function is used to stops the real time clock from ticking. Note that
when we are actually using IRQ0 instead, this functions does nothing
(unlike calling PM_setRealTimeClockFrequency directly).

SEE ALSO:
PM_setRealTimeClockHandler, PM_setRealTimeClockFrequency,
PM_restartRealTimeClock, PM_restoreRealTimeClockHandler
****************************************************************************/
void PMAPI PM_stopRealTimeClock(void)
{
    PM_setRealTimeClockFrequency(0);
}

/****************************************************************************
DESCRIPTION:
Restarts the real time clock ticking again

HEADER:
pmapi.h

REMARKS:
This function is used to restart the real time clock ticking. Note that when we
are actually using IRQ0 instead, this functions does nothing.

SEE ALSO:
PM_setRealTimeClockHandler, PM_setRealTimeClockFrequency, PM_stopRealTimeClock,
PM_restoreRealTimeClockHandler
****************************************************************************/
void PMAPI PM_restartRealTimeClock(
    int frequency)
{
    PM_setRealTimeClockFrequency(frequency);
}

/****************************************************************************
DESCRIPTION:
Restore the original real time clock handler.

HEADER:
pmapi.h

REMARKS:
This function is used to restore the original real time clock handler.

SEE ALSO:
PM_setRealTimeClockHandler
****************************************************************************/
void PMAPI PM_restoreRealTimeClockHandler(void)
{
    /* Not supported under Win32 */
}

/****************************************************************************
DESCRIPTION:
Return the current operating system path or working directory.

HEADER:
pmapi.h

PARAMETERS:
path    - Place to store the path string
maxLen  - Maximum length of the path string

RETURNS:
Pointer to the current path string

REMARKS:
This function is used to obtain the current operating system path or working
directory. The string is copied into the path parameter, with a maximum
length of maxLen characters. A pointer to path is also returned from
the function.

SEE ALSO:
PM_getdcwd
****************************************************************************/
char * PMAPI PM_getCurrentPath(
    char *path,
    int maxLen)
{
    return getcwd(path,maxLen);
}

/****************************************************************************
REMARKS:
Query a string from the registry (extended version).
****************************************************************************/
static ibool REG_queryStringEx(
    HKEY hKey,
    const char *szValue,
    char *value,
    ulong size)
{
    DWORD   type;

    if (RegQueryValueEx(hKey,(PCHAR)szValue,(PDWORD)NULL,(PDWORD)&type,(LPBYTE)value,(PDWORD)&size) == ERROR_SUCCESS)
        return true;
    return false;
}

/****************************************************************************
REMARKS:
Query a string from the registry.
****************************************************************************/
static ibool REG_queryString(
    const char *szKey,
    const char *szValue,
    char *value,
    DWORD size)
{
    HKEY    hKey;
    ibool   status = false;

    memset(value,0,sizeof(value));
    if (RegOpenKey(HKEY_LOCAL_MACHINE,szKey,&hKey) == ERROR_SUCCESS) {
        status = REG_queryStringEx(hKey,szValue,value,size);
        RegCloseKey(hKey);
        }
    return status;
}

/****************************************************************************
DESCRIPTION:
Return the drive letter for the boot drive.

HEADER:
pmapi.h

RETURNS:
Character representing the operating system boot drive.

REMARKS:
This function is used to obtain the drive letter for the boot drive used
by the operating system. This is only valid for operating systems that
use driver letters, such as DOS, OS/2 and Windows.
****************************************************************************/
char PMAPI PM_getBootDrive(void)
{
    static char path[256];

    if (PM_getOSType() != _OS_WIN95) {
        GetSystemDirectory(path,sizeof(path));
        return path[0];
        }
    return 'c';
}

/****************************************************************************
DESCRIPTION:
Return the path to the SNAP driver files.

HEADER:
pmapi.h

RETURNS:
Constant string pointer to the SNAP path

REMARKS:
This function is used to obtain the standard path where the SNAP
drivers should be found. This is usually operating system specific, but it
can be overridden for debugging and development purposes using the
SNAP_PATH environment variable.

SEE ALSO:
PM_getSNAPConfigPath
****************************************************************************/
const char * PMAPI PM_getSNAPPath(void)
{
    static char path[256];
    char        *env;

    if ((env = getenv("SNAP_PATH")) != NULL)
        return env;
    GetSystemDirectory(path,sizeof(path));
    strcat(path,"\\snap");
    return path;
}

/****************************************************************************
DESCRIPTION:
Return the path to the SNAP configuration files.

HEADER:
pmapi.h

RETURNS:
Constant string pointer to the SNAP configuration files path

REMARKS:
This function is used to obtain the standard path where the SNAP configuration
files should be found. This is usually operating system specific, but it
can be overridden for debugging and development purposes using the
SNAP_PATH environment variable. In most cases this is a 'config' directory
below the directory reported by PM_getSNAPPath, however on some network
operating systems with shared directories, the config directories may be
specific to each user while the SNAP binaries live in a shared directory
common to all users. This is the case for instance under the QNX
operating systems.

SEE ALSO:
PM_getSNAPPath
****************************************************************************/
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

/****************************************************************************
DESCRIPTION:
Return a unique identifier for the machine if possible.

HEADER:
pmapi.h

RETURNS:
Constant string pointer to the unique identifier

REMARKS:
This function is used to obtain a unique identifier string for the computer on
the netork if possible. This is not always possible for all OS'es (especially
when the OS has no networking!), so in some cases this will simply be a
constant value if the network machine name or unique ID cannot be determined.
****************************************************************************/
const char * PMAPI PM_getUniqueID(void)
{
    return PM_getMachineName();
}

/****************************************************************************
DESCRIPTION:
Get the name of the machine on the network.

HEADER:
pmapi.h

RETURNS:
Constant string pointer to the network machine name

REMARKS:
This function is used to obtain the machine name for the computer on
the network if possible. This is not always possible for all OS'es (especially
when the OS has no networking!), so in some cases this will simply be a
constant value if the network machine name cannot be determined.
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
DESCRIPTION:
Return a pointer to the real mode BIOS data area.

HEADER:
pmapi.h

RETURNS:
Pointer to the real mode BIOS data area

REMARKS:
This function is used to obtain a pointer to the real mode BIOS data
area. This is only possible on machines that provide access to the real
mode BIOS, so if the PM_haveBIOSAccess function returns false, this
function will not return a useful pointer (so don't try to use it!).
****************************************************************************/
void * PMAPI PM_getBIOSPointer(void)
{
    if (_PM_haveWinNT) {
        /* On Windows NT we have to map it physically directly */
        return PM_mapPhysicalAddr(0x400, 0x1000, true);
        }
    else {
        /* For Windows 9x we can access this memory directly */
        return (void*)0x400;
        }
}

/****************************************************************************
DESCRIPTION:
Return a pointer to 0xA0000 physical VGA graphics framebuffer.

HEADER:
pmapi.h

RETURNS:
Pointer to 0xA0000 physical VGA graphics framebuffer.

REMARKS:
This function is used to obtain a pointer to the physical VGA graphics
framebuffer which is located at physical address 0xA0000. This is supported
on all operating systems.
****************************************************************************/
void * PMAPI PM_getA0000Pointer(void)
{
    if (_PM_haveWinNT) {
        /* On Windows NT we have to map it physically directly */
        return PM_mapPhysicalAddr(0xA0000, 0x0FFFF, false);
        }
    else {
        /* Always use the 0xA0000 linear address so that we will use
         * whatever page table mappings are set up for us (ie: for virtual
         * bank switching.
         */
        return (void*)0xA0000;
        }
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
    DWORD   inBuf[3];   /* Buffer to send data to VxD       */
    DWORD   outBuf[1];  /* Buffer to receive data from VxD  */
    DWORD   count;      /* Count of bytes returned from VxD */

    if (!inited)
        PM_init();
    inBuf[0] = base;
    inBuf[1] = limit;
    inBuf[2] = isCached;
    CHECK_FOR_PMHELP();
    if (DeviceIoControl(_PM_hDevice, PMHELP_MAPPHYS32, inBuf, sizeof(inBuf),
            outBuf, sizeof(outBuf), &count, NULL))
        return (void*)outBuf[0];
    return NULL;
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
    /* We never free the mappings under Win32 (the VxD tracks them and
     * reissues the same mappings until the system is rebooted).
     */
    (void)ptr;
    (void)limit;
}

/****************************************************************************
DESCRIPTION:
Find the physical address of a linear memory address for the current process.

HEADER:
pmapi.h

PARAMETERS:
p   - Linear address to convert

RETURNS:
Physical memory address, or PM_BAD_PHYS_ADDRESS on error.

REMARKS:
This function is used to convert a linear address pointer to a physical
memory address. If this fails, or is not supported for some reason, this
function will return a value of PM_BAD_PHYS_ADDRESS.

SEE ALSO:
PM_mapPhysicalAddr, PM_getPhysicalAddrRange
****************************************************************************/
ulong PMAPI PM_getPhysicalAddr(
    void *p)
{
    DWORD   inBuf[1];   /* Buffer to send data to VxD       */
    DWORD   outBuf[1];  /* Buffer to receive data from VxD  */
    DWORD   count;      /* Count of bytes returned from VxD */

    if (!inited)
        PM_init();
    inBuf[0] = (ulong)p;
    CHECK_FOR_PMHELP();
    if (DeviceIoControl(_PM_hDevice, PMHELP_GETPHYSICALADDR32, inBuf, sizeof(inBuf),
            outBuf, sizeof(outBuf), &count, NULL))
        return outBuf[0];
    return PM_BAD_PHYS_ADDRESS;
}

/****************************************************************************
DESCRIPTION:
Find physical addresss of a linear memory address for the current process.

HEADER:
pmapi.h

PARAMETERS:
p           - Linear address to convert
length      - Length of memory region to convert
physAddress - Array to store physical addresses into

RETURNS:
True on success, false on error.

REMARKS:
This function is used to convert a large linear address pointer to a
list of physical memory addresses. The list of addresses will be one per page
for the linear address, and the addresses will all be page aligned. This
is useful to convert a single linear address block into the list of
physical memory pages for the memory to be programmed into DMA operations
etc.

SEE ALSO:
PM_mapPhysicalAddr, PM_getPhysicalAddr
****************************************************************************/
ibool PMAPI PM_getPhysicalAddrRange(
    void *p,
    ulong length,
    ulong *physAddress)
{
    DWORD   inBuf[3];   /* Buffer to send data to VxD       */
    DWORD   outBuf[1];  /* Buffer to receive data from VxD  */
    DWORD   count;      /* Count of bytes returned from VxD */

    if (!inited)
        PM_init();
    inBuf[0] = (ulong)p;
    inBuf[1] = (ulong)length;
    inBuf[2] = (ulong)physAddress;
    CHECK_FOR_PMHELP();
    if (DeviceIoControl(_PM_hDevice, PMHELP_GETPHYSICALADDRRANGE32, inBuf, sizeof(inBuf),
            outBuf, sizeof(outBuf), &count, NULL))
        return outBuf[0];
    return false;
}

/****************************************************************************
DESCRIPTION:
Sleep for the specified number of milliseconds.

HEADER:
pmapi.h

PARAMETERS:
milliseconds    - Number of milliseconds to sleep for

REMARKS:
This function is used to pause the current process and put it to sleep for
the specified number of milliseconds.
****************************************************************************/
void PMAPI PM_sleep(
    ulong milliseconds)
{
    Sleep(milliseconds);
}

/****************************************************************************
DESCRIPTION:
Read a byte value from an I/O port

HEADER:
pmapi.h

PARAMETERS:
port    - I/O port to read the value from

RETURNS:
Byte value read from the I/O port

REMARKS:
This function is used to read a byte value from an I/O port.

SEE ALSO:
PM_inpw, PM_inpd, PM_outpb
****************************************************************************/
u8 PMAPI PM_inpb(
    int port);

/****************************************************************************
DESCRIPTION:
Read a word value from an I/O port

HEADER:
pmapi.h

PARAMETERS:
port    - I/O port to read the value from

RETURNS:
Word value read from the I/O port

REMARKS:
This function is used to read a word value from an I/O port.

SEE ALSO:
PM_inpb, PM_inpd, PM_outpb
****************************************************************************/
u16 PMAPI PM_inpw(
    int port);

/****************************************************************************
DESCRIPTION:
Read a double word value from an I/O port

HEADER:
pmapi.h

PARAMETERS:
port    - I/O port to read the value from

RETURNS:
Double word value read from the I/O port

REMARKS:
This function is used to read a double word value from an I/O port.

SEE ALSO:
PM_inpb, PM_inpw, PM_outpb
****************************************************************************/
u32 PMAPI PM_inpd(
    int port);

/****************************************************************************
DESCRIPTION:
Write a byte value to an I/O port

HEADER:
pmapi.h

PARAMETERS:
port    - I/O port to read the value from
val     - Value to write to the I/O port

REMARKS:
This function is used to write a byte value to an I/O port.

SEE ALSO:
PM_inpb, PM_outpw, PM_outpd
****************************************************************************/
void PMAPI PM_outpb(
    int port,
    u8 val);

/****************************************************************************
DESCRIPTION:
Write a word value to an I/O port

HEADER:
pmapi.h

PARAMETERS:
port    - I/O port to read the value from
val     - Value to write to the I/O port

REMARKS:
This function is used to write a word value to an I/O port.

SEE ALSO:
PM_inpb, PM_outpb, PM_outpd
****************************************************************************/
void PMAPI PM_outpw(
    int port,
    u16 val);

/****************************************************************************
DESCRIPTION:
Write a double word value to an I/O port

HEADER:
pmapi.h

PARAMETERS:
port    - I/O port to read the value from
val     - Value to write to the I/O port

REMARKS:
This function is used to write a double word value to an I/O port.

SEE ALSO:
PM_inpb, PM_outpb, PM_outpw
****************************************************************************/
void PMAPI PM_outpd(
    int port,
    u32 val);

/****************************************************************************
DESCRIPTION:
Return the base I/O port for the specified COM port.

HEADER:
pmapi.h

PARAMETERS:
port    - COM port number to get I/O port for

RETURNS:
Base I/O port for the specified COM port

REMARKS:
This function is used to determine from the operating system what the
base I/O port is for the specified COM port in the system. This is only
used presently for software stereo support on supported operating systems
(ie: DOS, Windows 9x and Windows NT/2000/XP).
****************************************************************************/
int PMAPI PM_getCOMPort(
    int port)
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
DESCRIPTION:
Return the base I/O port for the specified printer port.

HEADER:
pmapi.h

PARAMETERS:
port    - Printer port number to get I/O port for

RETURNS:
Base I/O port for the specified printer port

REMARKS:
This function is used to determine from the operating system what the
base I/O port is for the specified printer port in the system. This is only
used presently for software stereo support on supported operating systems
(ie: DOS, Windows 9x and Windows NT/2000/XP).
****************************************************************************/
int PMAPI PM_getLPTPort(
    int port)
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
Function to enable/disable global address mapping on an as needed basis.
If global address mapping is enabled, all PM shared memory, locked memory
and physical memory mapping addresses are opened up to user space programs.
All future memory allocations will also be enabled for user space access.
Once this is turned off, we restore everything back to kernal space access
only.
{secret}
****************************************************************************/
void PMAPI PM_enableGlobalAddressMapping(
    ibool enable)
{
    DWORD   inBuf[1];   /* Buffer to send data to VxD       */
    DWORD   outBuf[1];  /* Buffer to receive data from VxD  */
    DWORD   count;      /* Count of bytes returned from VxD */

    inBuf[0] = enable;
    CHECK_FOR_PMHELP();
    DeviceIoControl(_PM_hDevice, PMHELP_ENABLEGLOBALMAPPING32, inBuf, sizeof(inBuf),
        outBuf, sizeof(outBuf), &count, NULL);
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
linear address returned for this shared memory is /identical/ for all
processes in the system. If this cannot be provided, this function
will return NULL.

SEE ALSO:
PM_freeShared
****************************************************************************/
void * PMAPI PM_mallocShared(
    long size)
{
    DWORD   inBuf[1];   /* Buffer to send data to VxD       */
    DWORD   outBuf[1];  /* Buffer to receive data from VxD  */
    DWORD   count;      /* Count of bytes returned from VxD */

    inBuf[0] = size;
    CHECK_FOR_PMHELP();
    if (DeviceIoControl(_PM_hDevice, PMHELP_MALLOCSHARED32, inBuf, sizeof(inBuf),
            outBuf, sizeof(outBuf), &count, NULL))
        return (void*)outBuf[0];
    return NULL;
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

SEE ALSO:
PM_mallocShared
****************************************************************************/
void PMAPI PM_freeShared(
    void *ptr)
{
    DWORD   inBuf[1];   /* Buffer to send data to VxD       */
    DWORD   outBuf[1];  /* Buffer to receive data from VxD  */
    DWORD   count;      /* Count of bytes returned from VxD */

    inBuf[0] = (ulong)ptr;
    CHECK_FOR_PMHELP();
    DeviceIoControl(_PM_hDevice, PMHELP_FREESHARED32, inBuf, sizeof(inBuf),
        outBuf, sizeof(outBuf), &count, NULL);
}

/****************************************************************************
DESCRIPTION:
Map a real mode pointer to a protected mode pointer.

HEADER:
pmapi.h

PARAMETERS:
r_seg   - Real mode segment address to map
r_off   - Real mode segment offset to map

REMARKS:
This function is used to map a real mode pointer in segment:offset format
into a protected mode linear address. This is only supported for operating
systems that support BIOS access (ie: the PM_haveBIOSAccess function
returns true).

SEE ALSO:
PM_allocRealSeg
****************************************************************************/
void * PMAPI PM_mapRealPointer(
    uint r_seg,
    uint r_off)
{
    return (void*)(MK_PHYS(r_seg,r_off));
}

/****************************************************************************
DESCRIPTION:
Allocate a block of real mode memory

HEADER:
pmapi.h

PARAMETERS:
size    - Size of memory block to allocate
r_seg   - Place to store real mode segment address of memory block
r_off   - Place to store real mode segment offset of memory block

RETURNS:
Linear pointer to the real mode memory block, NULL on failure.

REMARKS:
This function is used to allocate a block of real mode memory for
communicating with the real mode BIOS. If this function succeeds,
the r_seg and r_off parameters will be filled in with the real mode
address of the memory block, and the function will return a regular C
style linear pointer to the memory block. This is only supported for operating
systems that support BIOS access (ie: the PM_haveBIOSAccess function
returns true).

SEE ALSO:
PM_freeRealSeg, PM_mapRealPointer
****************************************************************************/
void * PMAPI PM_allocRealSeg(
    uint size,
    uint *r_seg,
    uint *r_off)
{
    /* We do not support dynamically allocating real mode memory buffers
     * from Win32 programs (we need a 16-bit DLL for this, and Windows
     * 9x becomes very unstable if you free the memory blocks out of order).
     */
    (void)size;
    (void)r_seg;
    (void)r_off;
    return NULL;
}

/****************************************************************************
DESCRIPTION:
Free a block of real mode memory.

HEADER:
pmapi.h

PARAMETERS:
mem - Pointer to the memory block to free

REMARKS:
This function is used to free a block of real mode memory previously
allocated with the PM_allocRealSeg function. This is only supported for operating
systems that support BIOS access (ie: the PM_haveBIOSAccess function
returns true).

SEE ALSO:
PM_allocRealSeg, PM_mapRealPointer
****************************************************************************/
void PMAPI PM_freeRealSeg(
    void *mem)
{
    /* Not supported in Windows */
    (void)mem;
}

/****************************************************************************
DESCRIPTION:
Execute a real mode software interrupt.

HEADER:
pmapi.h

PARAMETERS:
intno   - Software interrupt number to execute
in      - Register block to load before calling interrupt
out     - Register block to load with registers after interrupt was called

RETURNS:
Value returned in the EAX register.

REMARKS:
This function is used execute a real mode software interrupt, which is used
to call the real mode BIOS functions and drivers directly. If you make
calls to the real mode BIOS functions or drivers with this function, there
is /no/ parameter translation at all. Hence you need to translate any real
mode memory pointers etc passed into and returned from this function
with the PM_allocRealSeg and PM_mapRealPointer functions.

When this function executes the real mode software interrupt, the machine
registers will be loaded with the values passed in the 'in' parameter.
When the interrupt completes, the values in the machine registers will then
be saved into the 'out' parameter.

This is only supported for operating systems that support BIOS access (ie:
the PM_haveBIOSAccess function returns true).

SEE ALSO:
PM_int86x, PM_callRealMode, PM_allocRealSeg
****************************************************************************/
int PMAPI PM_int86(
    int intno,
    RMREGS *in,
    RMREGS *out)
{
    DWORD   inBuf[3];   /* Buffer to send data to VxD       */
    DWORD   outBuf[1];  /* Buffer to receive data from VxD  */
    DWORD   count;      /* Count of bytes returned from VxD */

    if (!inited)
        PM_init();
    inBuf[0] = intno;
    inBuf[1] = (ulong)in;
    inBuf[2] = (ulong)out;
    CHECK_FOR_PMHELP();
    if (DeviceIoControl(_PM_hDevice, PMHELP_INT8632, inBuf, sizeof(inBuf),
            outBuf, sizeof(outBuf), &count, NULL))
        return outBuf[0];
    return 0;
}

/****************************************************************************
DESCRIPTION:
Execute a real mode software interrupt with segment registers

HEADER:
pmapi.h

PARAMETERS:
intno   - Software interrupt number to execute
in      - Register block to load before calling interrupt
out     - Register block to load with registers after interrupt was called
sregs   - Segment register block to load and return values in

RETURNS:
Value returned in the EAX register.

REMARKS:
This function is used execute a real mode software interrupt, which is used
to call the real mode BIOS functions and drivers directly. If you make
calls to the real mode BIOS functions or drivers with this function, there
is /no/ parameter translation at all. Hence you need to translate any real
mode memory pointers etc passed into and returned from this function
with the PM_allocRealSeg and PM_mapRealPointer functions.

When this function executes the real mode software interrupt, the machine
registers will be loaded with the values passed in the 'in' parameter.
When the interrupt completes, the values in the machine registers will then
be saved into the 'out' parameter.

This version also allows you to pass down segment register values to be
passed to the real mode code with the sregs parameter. On return from this
function, sregs will contain the segment registers that were returned from
the function when the real mode interrupt was completed.

This is only supported for operating systems that support BIOS access (ie:
the PM_haveBIOSAccess function returns true).

SEE ALSO:
PM_int86, PM_callRealMode, PM_allocRealSeg
****************************************************************************/
int PMAPI PM_int86x(
    int intno,
    RMREGS *in,
    RMREGS *out,
    RMSREGS *sregs)
{
    DWORD   inBuf[4];   /* Buffer to send data to VxD       */
    DWORD   outBuf[1];  /* Buffer to receive data from VxD  */
    DWORD   count;      /* Count of bytes returned from VxD */

    if (!inited)
        PM_init();
    inBuf[0] = intno;
    inBuf[1] = (ulong)in;
    inBuf[2] = (ulong)out;
    inBuf[3] = (ulong)sregs;
    CHECK_FOR_PMHELP();
    if (DeviceIoControl(_PM_hDevice, PMHELP_INT86X32, inBuf, sizeof(inBuf),
            outBuf, sizeof(outBuf), &count, NULL))
        return outBuf[0];
    return 0;
}

/****************************************************************************
DESCRIPTION:
Call a real mode far function.

HEADER:
pmapi.h

PARAMETERS:
seg     - Real mode segment address of function to call
off     - Real mode segment offset of function to call
in      - Register block to load before calling interrupt
out     - Register block to load with registers after interrupt was called
sregs   - Segment register block to load and return values in

REMARKS:
This function is used call a real mode far function, which is used
to call the real mode BIOS functions and drivers directly. If you make
calls to the real mode BIOS functions or drivers with this function, there
is /no/ parameter translation at all. Hence you need to translate any real
mode memory pointers etc passed into and returned from this function
with the PM_allocRealSeg and PM_mapRealPointer functions.

When this function executes the real mode far function, the machine
registers will be loaded with the values passed in the 'in' parameter and
the segment registers from the 'sregs' parameter. When the function returns,
the values in the machine registers will then be saved into the 'out' parameter
and the segment registers into the 'sregs' parameter.

This is only supported for operating systems that support BIOS access (ie:
the PM_haveBIOSAccess function returns true).

SEE ALSO:
PM_int86, PM_int86x, PM_allocRealSeg
****************************************************************************/
void PMAPI PM_callRealMode(
    uint seg,
    uint off,
    RMREGS *in,
    RMSREGS *sregs)
{
    DWORD   inBuf[4];   /* Buffer to send data to VxD       */
    DWORD   outBuf[1];  /* Buffer to receive data from VxD  */
    DWORD   count;      /* Count of bytes returned from VxD */

    if (!inited)
        PM_init();
    inBuf[0] = seg;
    inBuf[1] = off;
    inBuf[2] = (ulong)in;
    inBuf[3] = (ulong)sregs;
    CHECK_FOR_PMHELP();
    DeviceIoControl(_PM_hDevice, PMHELP_CALLREALMODE32, inBuf, sizeof(inBuf),
        outBuf, sizeof(outBuf), &count, NULL);
}

/****************************************************************************
DESCRIPTION:
Allocate a block of locked, physical memory for DMA operations.

HEADER:
pmapi.h

PARAMETERS:
size        - Size of memory block to allocate
physAddr    - Place to return the physical memory address
contiguous  - True if the block should be contiguous
below16M    - True if the block must be below 16M physical

RETURNS:
Linear pointer to memory block, or NULL on failure.

REMARKS:
This function is used to allocate a block of locked, physical memory for
use in hardware DMA operations. If the contiguous parameter is set to
true, then the memory block requested must be contiguous in physical memory
or this function will fail (requesting physically contiguous memory usually
does not succeed for large blocks except immediately after the operating
system is loaded or during the boot process). If the below16M flag is
set to true, then the physical memory block must be allocated below the
16Mb physical memory address (required for old ISA bus sound card DMA
buffers for instance). When this function succeeds, it will return a
regular C pointer to the allocated memory block.

Note also that the memory block allocated by this function must also be
globally shared, such that the linear address returned will be valid in
all processes in the system at the same location.

SEE ALSO:
PM_freeLockedMem
****************************************************************************/
void * PMAPI PM_allocLockedMem(
    uint size,
    ulong *physAddr,
    ibool contiguous,
    ibool below16M)
{
    DWORD   inBuf[4];   /* Buffer to send data to VxD       */
    DWORD   outBuf[1];  /* Buffer to receive data from VxD  */
    DWORD   count;      /* Count of bytes returned from VxD */

    if (!inited)
        PM_init();
    inBuf[0] = size;
    inBuf[1] = (ulong)physAddr;
    inBuf[2] = (ulong)contiguous;
    inBuf[3] = (ulong)below16M;
    CHECK_FOR_PMHELP();
    if (DeviceIoControl(_PM_hDevice, PMHELP_ALLOCLOCKED32, inBuf, sizeof(inBuf),
            outBuf, sizeof(outBuf), &count, NULL))
        return (void*)outBuf[0];
    return NULL;
}

/****************************************************************************
DESCRIPTION:
Free a block of locked physical memory.

HEADER:
pmapi.h

PARAMETERS:
p           - Pointer to the memory block to free
size        - Size of memory block that was allocated
contiguous  - True if the block was contiguously allocated

REMARKS:
This function is used to free a block of locked, physical memory previously
allocated by the PM_allocLockedMem function. This function must be passed
the exact same size and contiguous values that were passed to the
PM_allocLockedMem function, or it may produce strange results.

SEE ALSO:
PM_allocLockedMem
****************************************************************************/
void PMAPI PM_freeLockedMem(
    void *p,
    uint size,
    ibool contiguous)
{
    DWORD   inBuf[3];   /* Buffer to send data to VxD       */
    DWORD   outBuf[1];  /* Buffer to receive data from VxD  */
    DWORD   count;      /* Count of bytes returned from VxD */

    if (!inited)
        PM_init();
    inBuf[0] = (ulong)p;
    inBuf[1] = size;
    inBuf[2] = contiguous;
    CHECK_FOR_PMHELP();
    DeviceIoControl(_PM_hDevice, PMHELP_FREELOCKED32, inBuf, sizeof(inBuf),
        outBuf, sizeof(outBuf), &count, NULL);
}

/****************************************************************************
DESCRIPTION:
Allocates a page aligned and page sized block of memory

HEADER:
pmapi.h

PARAMETERS:
locked  - True if the memory should be locked down, false if not

RETURNS:
Pointer to the page aligned page of memory allocated, NULL on failure.

REMARKS:
This function is used to allocate a single page sized and page aligned block
of memory from the operating system. The memory block may be optionally
locked in physical memory if the locked parameter is set to true.

This function is mostly used to allocate pages of physical memory that are
used to back the AGP memory regions used by graphics drivers. As such
there is presently no requirement for the pages allocated by this
function to be globally mapped.

SEE ALSO:
PM_freePage
****************************************************************************/
void * PMAPI PM_allocPage(
    ibool locked)
{
    DWORD   inBuf[2];   /* Buffer to send data to VxD       */
    DWORD   outBuf[1];  /* Buffer to receive data from VxD  */
    DWORD   count;      /* Count of bytes returned from VxD */

    if (!inited)
        PM_init();
    inBuf[0] = locked;
    CHECK_FOR_PMHELP();
    if (DeviceIoControl(_PM_hDevice, PMHELP_ALLOCPAGE32, inBuf, sizeof(inBuf),
            outBuf, sizeof(outBuf), &count, NULL))
        return (void*)outBuf[0];
    return NULL;
}

/****************************************************************************
DESCRIPTION:
Free a page aligned and page sized block of memory

HEADER:
pmapi.h

PARAMETERS:
p   - Linear pointer to the page of memory to free

REMARKS:
This function is used to free a page of memory previously allocated with the
PM_allocPage function.

SEE ALSO:
PM_allocPage
****************************************************************************/
void PMAPI PM_freePage(
    void *p)
{
    DWORD   inBuf[1];   /* Buffer to send data to VxD       */
    DWORD   outBuf[1];  /* Buffer to receive data from VxD  */
    DWORD   count;      /* Count of bytes returned from VxD */

    if (!inited)
        PM_init();
    inBuf[0] = (ulong)p;
    CHECK_FOR_PMHELP();
    DeviceIoControl(_PM_hDevice, PMHELP_FREEPAGE32, inBuf, sizeof(inBuf),
        outBuf, sizeof(outBuf), &count, NULL);
}

/****************************************************************************
DESCRIPTION:
Lock data pages so they won't be paged to disk

HEADER:
pmapi.h

PARAMETERS:
p   - Linear pointer to the memory to lock down
len - Length of the memory block to lock down
lh  - Pointer to the lock handle returned

REMARKS:
This function is used to lock a block of memory such that it will not be
paged to disk by the operating systems virtual memory manager. This is
mostly used such that interrupt handlers in device drivers and the data
used by the interrupt handlers will never be paged out to disk.

This version is used to lock data pages in memory.

SEE ALSO:
PM_unlockDataPages, PM_lockCodePages
****************************************************************************/
int PMAPI PM_lockDataPages(
    void *p,
    uint len,
    PM_lockHandle *lh)
{
    DWORD   inBuf[2];   /* Buffer to send data to VxD       */
    DWORD   outBuf[1];  /* Buffer to receive data from VxD  */
    DWORD   count;      /* Count of bytes returned from VxD */

    inBuf[0] = (ulong)p;
    inBuf[1] = len;
    inBuf[2] = (ulong)lh;
    CHECK_FOR_PMHELP();
    if (DeviceIoControl(_PM_hDevice, PMHELP_LOCKDATAPAGES32, inBuf, sizeof(inBuf),
            outBuf, sizeof(outBuf), &count, NULL))
        return outBuf[0];
    return 0;
}

/****************************************************************************
DESCRIPTION:
Unlock data pages previously locked down.

HEADER:
pmapi.h

PARAMETERS:
p   - Linear pointer to the memory that was locked down
len - Length of the memory block that was locked down
lh  - Pointer to the lock handle returned from PM_lockDataPages

REMARKS:
This function is used to unlock a block of memory that was previously
locked down with the PM_lockDataPages function.

This version is used to unlock data pages in memory.

SEE ALSO:
PM_lockDataPages
****************************************************************************/
int PMAPI PM_unlockDataPages(
    void *p,
    uint len,
    PM_lockHandle *lh)
{
    DWORD   inBuf[2];   /* Buffer to send data to VxD       */
    DWORD   outBuf[1];  /* Buffer to receive data from VxD  */
    DWORD   count;      /* Count of bytes returned from VxD */

    inBuf[0] = (ulong)p;
    inBuf[1] = len;
    inBuf[2] = (ulong)lh;
    CHECK_FOR_PMHELP();
    if (DeviceIoControl(_PM_hDevice, PMHELP_UNLOCKDATAPAGES32, inBuf, sizeof(inBuf),
            outBuf, sizeof(outBuf), &count, NULL))
        return outBuf[0];
    return 0;
}

/****************************************************************************
DESCRIPTION:
Lock code pages so they won't be paged to disk

HEADER:
pmapi.h

PARAMETERS:
p   - Linear pointer to the memory to lock down
len - Length of the memory block to lock down
lh  - Pointer to the lock handle returned

REMARKS:
This function is used to lock a block of memory such that it will not be
paged to disk by the operating systems virtual memory manager. This is
mostly used such that interrupt handlers in device drivers and the data
used by the interrupt handlers will never be paged out to disk.

This version is used to lock code pages in memory.

SEE ALSO:
PM_unlockCodePages, PM_lockDataPages
****************************************************************************/
int PMAPI PM_lockCodePages(
    __codePtr p,
    uint len,
    PM_lockHandle *lh)
{
    DWORD   inBuf[2];   /* Buffer to send data to VxD       */
    DWORD   outBuf[1];  /* Buffer to receive data from VxD  */
    DWORD   count;      /* Count of bytes returned from VxD */

    inBuf[0] = (ulong)p;
    inBuf[1] = len;
    inBuf[2] = (ulong)lh;
    CHECK_FOR_PMHELP();
    if (DeviceIoControl(_PM_hDevice, PMHELP_LOCKCODEPAGES32, inBuf, sizeof(inBuf),
            outBuf, sizeof(outBuf), &count, NULL))
        return outBuf[0];
    return 0;
}

/****************************************************************************
DESCRIPTION:
Unlock code pages previously locked down.

HEADER:
pmapi.h

PARAMETERS:
p   - Linear pointer to the memory that was locked down
len - Length of the memory block that was locked down
lh  - Pointer to the lock handle returned from PM_lockCodePages

REMARKS:
This function is used to unlock a block of memory that was previously
locked down with the PM_lockCodePages function.

This version is used to unlock data pages in memory.

SEE ALSO:
PM_lockCodePages
****************************************************************************/
int PMAPI PM_unlockCodePages(
    __codePtr p,
    uint len,
    PM_lockHandle *lh)
{
    DWORD   inBuf[2];   /* Buffer to send data to VxD       */
    DWORD   outBuf[1];  /* Buffer to receive data from VxD  */
    DWORD   count;      /* Count of bytes returned from VxD */

    inBuf[0] = (ulong)p;
    inBuf[1] = len;
    inBuf[2] = (ulong)lh;
    CHECK_FOR_PMHELP();
    if (DeviceIoControl(_PM_hDevice, PMHELP_UNLOCKCODEPAGES32, inBuf, sizeof(inBuf),
            outBuf, sizeof(outBuf), &count, NULL))
        return outBuf[0];
    return 0;
}

/****************************************************************************
DESCRIPTION:
Enable write combining for a physical memory region

HEADER:
pmapi.h

PARAMETERS:
base    - Physical base address of region to write combine
length  - Length of the region to write combine
type    - Type of write caching to enable (PMEnableWriteCombineFlags)

REMARKS:
This function is used to change the write combine caching values for
a physical memory region. The type of caching that can be enabled for
the region can be one of the PMEnableWriteCombineFlags types.

Note that most CPU's only have a very limited number of write combine
regions available, so this function must be used as sparingly as possible
to ensure the hardware in the system can get the caching that it needs.
****************************************************************************/
ibool PMAPI PM_enableWriteCombine(
    ulong base,
    ulong length,
    uint type)
{
    DWORD   inBuf[3];   /* Buffer to send data to VxD       */
    DWORD   outBuf[1];  /* Buffer to receive data from VxD  */
    DWORD   count;      /* Count of bytes returned from VxD */

    if (!inited)
        PM_init();
    inBuf[0] = base;
    inBuf[1] = length;
    inBuf[2] = type;
    CHECK_FOR_PMHELP();
    if (DeviceIoControl(_PM_hDevice, PMHELP_ENABLELFBCOMB32, inBuf, sizeof(inBuf),
            outBuf, sizeof(outBuf), &count, NULL))
        return outBuf[0];
    return false;
}

/****************************************************************************
DESCRIPTION:
Enumerates all write combine regions currently enabled for the processor.

HEADER:
pmapi.h

PARAMETERS:
callback    - Function to callback with write combine information

RETURNS:
PM_MTRR_ERR_OK on success, otherwise error code.

REMARKS:
This function is used to enumerate all write combine regions currently
enabled for the processor.
****************************************************************************/
int PMAPI PM_enumWriteCombine(
    PM_enumWriteCombine_t callback)
{
    return PM_MTRR_ERR_NO_OS_SUPPORT;
}

/****************************************************************************
REMARKS:
Get the page directory base register value
{secret}
****************************************************************************/
ulong PMAPI _PM_getPDB(void)
{
    DWORD   outBuf[1];  /* Buffer to receive data from VxD  */
    DWORD   count;      /* Count of bytes returned from VxD */

    CHECK_FOR_PMHELP();
    if (DeviceIoControl(_PM_hDevice, PMHELP_GETPDB32, NULL, 0,
            outBuf, sizeof(outBuf), &count, NULL))
        return outBuf[0];
    return 0;
}

/****************************************************************************
DESCRIPTION:
Flush the translation lookaside buffer.

HEADER:
pmapi.h

REMARKS:
This function is used to flush the translation lookaside buffer.
****************************************************************************/
void PMAPI PM_flushTLB(void)
{
    DWORD   outBuf[1];  /* Buffer to receive data from VxD  */
    DWORD   count;      /* Count of bytes returned from VxD */

    CHECK_FOR_PMHELP();
    DeviceIoControl(_PM_hDevice, PMHELP_FLUSHTLB32, NULL, 0,
        outBuf, sizeof(outBuf), &count, NULL);
}

/****************************************************************************
DESCRIPTION:
Read a machine status register for the CPU.

HEADER:
pmapi.h

PARAMETERS:
reg     - 32-bit MSR register address to be loaded into ECX
*eax    - Pointer to Low-order 32-bit result retured in EAX
*edx    - Pointer to High-order 32-bit result returned in EDX

REMARKS:
Read a machine status register for the CPU. Executes privileged instruction
RDMSR via ring-0 service.
****************************************************************************/
void _ASMAPI PM_readMSR(
    ulong reg,
    ulong *eax,
    ulong *edx)
{
    DWORD   inBuf[1];   /* Buffer to send data to VxD       */
    DWORD   outBuf[2];  /* Buffer to receive data from VxD  */
    DWORD   count;      /* Count of bytes returned from VxD */

    CHECK_FOR_PMHELP();
    inBuf[0] = reg;
    DeviceIoControl(_PM_hDevice, PMHELP_READMSR32, inBuf, sizeof(inBuf),
        outBuf, sizeof(outBuf), &count, NULL);
    *eax = outBuf[0];
    *edx = outBuf[1];
}

/****************************************************************************
DESCRIPTION:
Read a machine status register for the CPU.

HEADER:
pmapi.h

PARAMETERS:
reg     - 32-bit MSR register address to be loaded into ECX
eax     - Low-order 32-bit data value to be loaded into EAX
edx     - High-order 32-bit data value to be loaded into EDX

REMARKS:
Write a machine status register for the CPU. Executes privileged instruction
WRMSR via ring-0 service.
****************************************************************************/
void _ASMAPI PM_writeMSR(
    ulong reg,
    ulong eax,
    ulong edx)
{
    DWORD   inBuf[3];   /* Buffer to send data to VxD       */
    DWORD   outBuf[1];  /* Buffer to receive data from VxD  */
    DWORD   count;      /* Count of bytes returned from VxD */

    CHECK_FOR_PMHELP();
    inBuf[0] = reg;
    inBuf[1] = eax;
    inBuf[2] = edx;
    DeviceIoControl(_PM_hDevice, PMHELP_WRITEMSR32, inBuf, sizeof(inBuf),
        outBuf, sizeof(outBuf), &count, NULL);
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
Function to get the current working directory for the specified drive.

HEADER:
pmapi.h

PARAMETERS:
drive   - Drive letter to get working directory for
dir     - Place to store working directory
len     - Length of working directory buffer

REMARKS:
This function is used to get the current working directory for the specified
drive from the operating system. Under Unix systems, this will always return
the current working directory regardless of what the value of 'drive' is
since there is no concept of drives under Unix.

SEE ALSO:
PM_getCurrentPath
****************************************************************************/
void PMAPI PM_getdcwd(
    int drive,
    char *dir,
    int len)
{
    // NT stores the current directory for drive N in the magic environment
    // variable =N: so we simply look for that environment variable.
    char envname[4];

    envname[0] = '=';
    envname[1] = drive - 1 + 'A';
    envname[2] = ':';
    envname[3] = '\0';
    if (GetEnvironmentVariable(envname,dir,len) == 0) {
        // The current directory or the drive has not been set yet, so
        // simply set it to the root.
        dir[0] = envname[1];
        dir[1] = ':';
        dir[2] = '\\';
        dir[3] = '\0';
        SetEnvironmentVariable(envname,dir);
        }
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
Function to create a directory.

HEADER:
pmapi.h

PARAMETERS:
filename    - Full path to filename for directory to create

RETURNS:
True on success, false on failure.

REMARKS:
This function is used to create a new directory in the file system.

SEE ALSO:
PM_rmdir
****************************************************************************/
ibool PMAPI PM_mkdir(
    const char *filename)
{
    return mkdir(filename) == 0;
}

/****************************************************************************
DESCRIPTION:
Function to remove a directory.

HEADER:
pmapi.h

PARAMETERS:
filename    - Full path to filename for directory to remove

RETURNS:
True on success, false on failure.

REMARKS:
This function is used to remove a directory from the file system. This
function will fail unless the directory is empty.

SEE ALSO:
PM_mkdir
****************************************************************************/
ibool PMAPI PM_rmdir(
    const char *filename)
{
    return RemoveDirectory(filename);
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
    HFILE       f;
    OFSTRUCT    of;
    FILETIME    utcTime,localTime;
    SYSTEMTIME  sysTime;
    ibool       status = false;

    of.cBytes = sizeof(of);
    if ((f = OpenFile(filename,&of,OF_READ)) == HFILE_ERROR)
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
the GMT time zone, otherwise it is in the local machine time zone.

SEE ALSO:
PM_getFileTime
****************************************************************************/
ibool PMAPI PM_setFileTime(
    const char *filename,
    ibool gmTime,
    PM_time *time)
{
    HFILE       f;
    OFSTRUCT    of;
    FILETIME    utcTime,localTime;
    SYSTEMTIME  sysTime;
    ibool       status = false;

    of.cBytes = sizeof(of);
    if ((f = OpenFile(filename,&of,OF_WRITE)) == HFILE_ERROR)
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
DESCRIPTION:
Increase the thread priority to maximum, if possible.

HEADER:
pmapi.h

RETURNS:
Old thread priority

REMARKS:
This function is used to set the current thread priority to the maximum
possible. This should not be used very often, but is useful for important
timing and calibration loops that need to be very accurate. The current
thread priority that was active before the change is returned.

SEE ALSO:
PM_restoreThreadPriority
****************************************************************************/
ulong PMAPI PM_setMaxThreadPriority(void)
{
    int     oldPriority;
    HANDLE  hThread = GetCurrentThread();

    oldPriority = GetThreadPriority(hThread);
    if (oldPriority != THREAD_PRIORITY_ERROR_RETURN)
        SetThreadPriority(hThread, THREAD_PRIORITY_TIME_CRITICAL);
    return oldPriority;
}

/****************************************************************************
DESCRIPTION:
Restore the original thread priority.

HEADER:
pmapi.h

PARAMETERS:
oldPriority - Old thread priority to restore

REMARKS:
This function is used to restore the current thread priority to the
previous value.

SEE ALSO:
PM_setThreadPriority
****************************************************************************/
void PMAPI PM_restoreThreadPriority(
    ulong oldPriority)
{
    HANDLE  hThread = GetCurrentThread();

    if (oldPriority != THREAD_PRIORITY_ERROR_RETURN)
        SetThreadPriority(hThread, oldPriority);
}

/****************************************************************************
DESCRIPTION:
Returns true if SNAP Graphics is the active display driver in the system.

HEADER:
pmapi.h

RETURNS:
True if SNAP Graphics is active, false if not.

REMARKS:
This function is used to determine if the SNAP Graphics display drivers
are the active display drivers in the system or not.
****************************************************************************/
ibool PMAPI PM_isSDDActive(void)
{
    DWORD   outBuf[1];  /* Buffer to receive data from VxD  */
    DWORD   count;      /* Count of bytes returned from VxD */

    PM_init();
    if (_PM_hDevice == INVALID_HANDLE_VALUE)
        return false;
    if (DeviceIoControl(_PM_hDevice, PMHELP_ISSDDACTIVE32, NULL, 0,
            outBuf, sizeof(outBuf), &count, NULL))
        return outBuf[0];
    return false;
}

/****************************************************************************
DESCRIPTION:
Determines if the application is running in a window.

HEADER:
pmapi.h

RETURNS:
True if running in a window, false if not.

REMARKS:
This function is primarily used for console programs that need to know if
they are running in a fullscreen console mode or in a window under a
GUI environment. Presently this function is implemented for DOS and OS/2
console mode programs. It could also be implemented for Linux console
apps also.
****************************************************************************/
ibool PMAPI PM_runningInAWindow(void)
{
    return false;
}

/****************************************************************************
REMARKS:
This function is used to enable or disable checking for and using the
SDDHELP.VXD or SDDHELP.SYS device drivers on Windows systems. By default we
always check for and support using SDDHELP, however sometimes programs should
only ever use PMHELP (ie: GLDirect, DirectDraw stereo etc) to avoid
conflicts with the SciTech SNAP Graphics drivers.
{secret}
****************************************************************************/
void PMAPI PM_useSDDHELP(
    ibool enable)
{
    useSDDHELP = enable;
}

/****************************************************************************
REMARKS:
Returns true if the SDDHELP device driver is installed and active.
{secret}
****************************************************************************/
ibool PMAPI PM_isSDDHELPActive(void)
{
    /* If SDDHELP device is opened we know it has to be active */
    if (isSDDHELP)
        return true;

    /* Otherwise query the service manager if SDDHELP service is active,
     * since we would prefer *not* to load the device driver in that case.
     */
   return (PM_queryService("sddhelp") == ERROR_SUCCESS);
}

/****************************************************************************
REMARKS:
Tell's the SDD display driver to rebuilt the mode list.
{secret}
****************************************************************************/
void PMAPI PM_rebuildSDDModeList(void)
{
    DWORD   outBuf[1];  /* Buffer to receive data from VxD  */
    DWORD   count;      /* Count of bytes returned from VxD */

    CHECK_FOR_PMHELP();
    DeviceIoControl(_PM_hDevice, PMHELP_REBUILDSDDMODELIST32, NULL, 0,
            outBuf, sizeof(outBuf), &count, NULL);
}

/****************************************************************************
DESCRIPTION:
Get the size of the VGA hardware state save buffer

HEADER:
pmapi.h

REMARKS:
Returns the size of the VGA state buffer.

SEE ALSO:
PM_saveVGAState, PM_restoreVGAState
****************************************************************************/
int PMAPI PM_getVGAStateSize(void)
{
    /* Nothing to do for Win32 programs */
    return 1;
}

/****************************************************************************
DESCRIPTION:
Save the VGA hardware state into a save buffer

HEADER:
pmapi.h

PARAMETERS:
stateBuf    - Place to save the state of the VGA hardware

REMARKS:
Save the state of all VGA compatible registers into save buffer passed
in the 'stateBuf' parameter. You must first call the PM_getVGAStateSize
function to allocate a buffer big enough to hold the VGA hardware state
before you call this function.

SEE ALSO:
PM_getVGAStateSize, PM_restoreVGAState
****************************************************************************/
void PMAPI PM_saveVGAState(
    void *stateBuf)
{
    /* Nothing to do for Win32 programs */
    (void)stateBuf;
}

/****************************************************************************
DESCRIPTION:
Restore the VGA hardware state from the save buffer

HEADER:
pmapi.h

PARAMETERS:
stateBuf    - Save buffer to restore the state of the VGA hardware from

REMARKS:
Restores the state of all VGA compatible registers from the save buffer
passed in the 'stateBuf' parameter.

SEE ALSO:
PM_getVGAStateSize, PM_saveVGAState
****************************************************************************/
void PMAPI PM_restoreVGAState(
    const void *stateBuf)
{
    /* Nothing to do for Win32 programs */
    (void)stateBuf;
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
    // Nothing to do in Win32 user-land as locked access to SNAP functions
    // is all handled in system-land by NT driver version PM library.
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

