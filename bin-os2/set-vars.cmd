@echo off
REM: Set the place where SciTech Software is installed, and where each
REM: of the supported compilers is installed. These environment variables
REM: are used by the batch files in the SCITECH\BIN-OS2 directory.
REM:
REM: Modify the as appropriate for your compiler configuration (you should
REM: only need to change things in this batch file).
REM:
REM: This version is for a normal OS/2 installation.

REM: The SCITECH variable points to where batch files, makefile startups,
REM: include files and source files will be found when compiling.

if not .%SCITECH%==. goto scitech_set
SET SCITECH=c:\scitech
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
SET PRIVATE=c:\private
:private_set

REM: Set the following environment variable to use the Netwide Assembler
REM: (NASM) provided with the MGL tools to build all assembler modules.
REM: If you have Turbo Assembler 4.0 and you wish to build for debugging
REM: you can use it by removing the following line.

SET USE_NASM=1

REM: Set the following to the HOST operating system architecture.
REM: For now this is always x86 for DOS, Windows and OS/2.

SET SNAP_HOST_ARCH=x86
SET SNAP_TARGET_ARCH=x86

REM: The following define the locations of all the compilers that you may
REM: be using. Change them to reflect where you have installed your
REM: compilers.

SET VA3_PATH=C:\C\IBMC30
SET WC10_PATH=C:\C\WC10
SET WC11_PATH=C:\C\WC11
SET OW10_PATH=C:\C\OW10
SET BC2_PATH=C:\C\BCOS2
SET EMX_PATH=C:\C\EMX
SET VA365_PATH=C:\ibmcxxo

REM: Let the makefile utilities know that we are runing in an OS/2 shell
SET OS2_SHELL=1

REM: Path to the OS/2 Toolkit
SET OS2TK_PATH=C:\TOOLKIT

REM: Set the PATH variable so we can see all the batch files. This PATH
REM: will be overridden the first time you run one of the compiler batch files.

PATH %SCITECH%\bin-os2;%DEFPATH%

REM Windows and OS/2 are only x86 based
SET USE_X86=1
