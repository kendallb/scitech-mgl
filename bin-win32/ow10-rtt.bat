@echo off
REM Setup for compiling with Open Watcom C/C++ 1.0 in 32 bit mode

if .%CHECKED%==.1 goto checked_build
SET LIB=%SCITECH_LIB%\LIB\RELEASE\RTT32\OW10;%RTOS32_PATH%\LIBMSVC;%OW10_PATH%\LIB386\NT;%OW10_PATH%\LIB386;.
echo Release build enabled.
goto setvars

:checked_build
SET LIB=%SCITECH_LIB%\LIB\DEBUG\RTT32\OW10;%RTOS32_PATH%\LIBMSVC;%OW10_PATH%\LIB386\NT;%OW10_PATH%\LIB386;.
echo Checked debug build enabled.
goto setvars

:setvars
SET EDPATH=%OW10_PATH%\EDDAT
SET C_INCLUDE=%RTOS32_PATH%\INCLUDE;%OW10_PATH%\H;%OW10_PATH%\H\NT
SET INCLUDE=INCLUDE;%SCITECH%\INCLUDE;%PRIVATE%\INCLUDE;%C_INCLUDE%
SET WATCOM=%OW10_PATH%
SET MAKESTARTUP=%SCITECH%\MAKEDEFS\WC32.MK
call clrvars.bat
SET USE_RTTARGET=1
SET USE_RTT32DLL=1
SET USE_RTFILES32=
SET USE_RTKERNEL32=
SET WC_LIBBASE=ow10
IF .%OS%==.Windows_NT goto Win32_path
IF NOT .%WINDIR%==. goto Win32_path
PATH %SCITECH_BIN%;%RTOS32_PATH%\BIN;%OW10_PATH%\BINW;%DEFPATH%%WC_CD_PATH%
goto path_set
:Win32_path
PATH %SCITECH_BIN%;%RTOS32_PATH%\BIN;%OW10_PATH%\BINNT;%OW10_PATH%\BINW;%DEFPATH%%WC_CD_PATH%
:path_set

REM: Enable Win32 SDK if desired (sdk on command line)
if NOT .%1%==.sdk goto done
call win32sdk.bat

:done
echo Open Watcom C/C++ 1.0 RTTarget-32 compilation environment set up.
