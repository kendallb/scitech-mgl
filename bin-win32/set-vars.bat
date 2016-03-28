@echo off
REM:=========================================================================
REM: Master batch file to set up all necessary environment variables for
REM: the SciTech makefile utilities. This batch file should be executed
REM: *first* before any other batch files when you start a command shell.
REM: You should not need to modify any batch files except this one to
REM: configure the makefile utilities.
REM:=========================================================================

REM: Set the place where SciTech Software is installed, and where each
REM: of the supported compilers is installed. These environment variables
REM: are used by the batch files in the SCITECH\BIN directory.
REM:
REM: Modify the as appropriate for your compiler configuration (you should
REM: only need to change things in this batch file).
REM:
REM: This version is for a normal MSDOS installation.

REM: The SCITECH variable points to where batch files, makefile startups,
REM: include files and source files will be found when compiling.

if not .%SCITECH%==. goto scitech_set
SET SCITECH=C:\SCITECH
:scitech_set

REM: The SCITECH_LIB variable points to where the SciTech libraries live
REM: for installation and linking. This allows you to have the source and
REM: include files on local machines for compiling and have the libraries
REM: located on a common network machine (for network builds).

if not .%SCITECH_LIB%==. goto scitechlib_set
SET SCITECH_LIB=%SCITECH%
:scitechlib_set

REM: The PRIVATE variable points to where private source files reside that
REM: do not live in the public source tree

if not .%PRIVATE%==. goto private_set
SET PRIVATE=C:\PRIVATE
:private_set

REM: The following sets up the path to the SciTech command line utilities
REM: for the development operating system. We not longer support the
REM: DOS hosted tools, only the Win32 hosted tools which will work on
REM: Windows 9x and Windows NT systems.

SET SCITECH_BIN=%SCITECH%\bin-win32

REM: Set the TMP variable for dmake if this is not already set

SET TMP=%TEMP%

REM: Set the following environment variable to use the Netwide Assembler
REM: (NASM) provided with the MGL tools to build all assembler modules.
REM: If you have Turbo Assembler 4.0 or later and you wish to use it,
REM: you can use it by removing the following line.

SET USE_NASM=1

REM: Set the following to the HOST operating system architecture.
REM: For now this is always x86 for DOS, Windows and OS/2.

SET SNAP_HOST_ARCH=x86
SET SNAP_TARGET_ARCH=x86

REM: The following is used to set up DDK directories for device driver
REM: development. They can safely be ignored unless you are using the
REM: SciTech makefile utilities to build device drivers.

SET DDKDRIVE=c:
SET MSSDK=c:\c\win32sdk
SET W95_DDKROOT=c:\c\95ddk
SET W98_DDKROOT=c:\c\98ddk
SET NT_DDKROOT=c:\c\ntddk
SET W2K_DDKROOT=c:\c\2000ddk
SET WINCE_DDKROOT=c:\c\wince
SET MASM_ROOT=c:\c\masm611
SET VTOOLSD=c:\c\vtd95
SET SOFTICE_PATH=c:\c\sidsuite\softice
SET RTOS32_PATH=c:\c\rtos32

REM: The following define the locations of all the compilers that you may
REM: be using. Change them to reflect where you have installed your
REM: compilers.

SET BC3_PATH=c:\c\bc3
SET BC4_PATH=c:\c\bc45
SET BC5_PATH=c:\c\bc50
SET BCB5_PATH=c:\c\bcb50
SET VC_PATH=c:\c\msvc
SET VC4_PATH=c:\c\vc42
SET VC5_PATH=c:\c\vc50
SET VC6_PATH=c:\c\vc60
SET VC7_PATH=c:\c\vc70
SET EVC4_PATH=c:\c\evc40
SET SC70_PATH=c:\c\sc75
SET WC10A_PATH=c:\c\wc10a
SET WC10_PATH=c:\c\wc10
SET WC11_PATH=c:\c\wc11
SET OW10_PATH=c:\c\ow10
SET TNT_PATH=c:\c\tnt
SET DJ_PATH=c:\c\djgpp
SET GCC_PATH=c:\unix\usr
SET QNX6_PATH=c:\c\qnx630

REM: The following define the locations of the IDE and compiler path
REM: tools for Visual C++. If you do a standard installation, you wont
REM: need to change this. If however you did a custom install and changed
REM: the paths to these directory, you will need to modify this to suit.

SET VC5_MSDevDir=%VC5_PATH%\sharedide
SET VC5_MSVCDir=%VC5_PATH%\vc
SET VC6_MSDevDir=%VC6_PATH%\common\msdev98
SET VC6_MSVCDir=%VC6_PATH%\vc98
SET VC7_MSDevDir=%VC7_PATH%\Common7\IDE
SET VC7_MSVCDir=%VC7_PATH%\vc7

REM: Set the PATH variable so we can see all the batch files. This PATH
REM: will be overridden the first time you run one of the compiler batch files.

PATH %SCITECH%\bin-win32;%DEFPATH%

REM Windows and OS/2 are only x86 based
SET USE_X86=1
