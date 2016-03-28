@echo off
REM Setup for compiling with Open Watcom C/C++ 1.0 in 32 bit mode with Phar Lap TNT

if .%CHECKED%==.1 goto checked_build
SET LIB=%SCITECH_LIB%\LIB\RELEASE\DOS32\OW10;%OW10_PATH%\LIB386\DOS;%OW10_PATH%\LIB386;%TNT_PATH%\LIB;.
echo Release build enabled.
goto setvars

:checked_build
SET LIB=%SCITECH_LIB%\LIB\DEBUG\DOS32\OW10;%OW10_PATH%\LIB386\DOS;%OW10_PATH%\LIB386;%TNT_PATH%\LIB;.
echo Checked debug build enabled.
goto setvars

:setvars
SET EDPATH=%OW10_PATH%\EDDAT
SET INCLUDE=INCLUDE;%SCITECH%\INCLUDE;%PRIVATE%\INCLUDE;%OW10_PATH%\H;%OW10_PATH%\H\NT;%TNT_PATH%\INCLUDE
SET WATCOM=%OW10_PATH%
SET MAKESTARTUP=%SCITECH%\MAKEDEFS\WC32.MK
call clrvars.bat
SET USE_TNT=1
SET WC_LIBBASE=ow10
IF .%OS%==.Windows_NT goto Win32_path
IF NOT .%WINDIR%==. goto Win32_path
PATH %SCITECH_BIN%;%OW10_PATH%\BINW;%DEFPATH%%WC_CD_PATH%
goto path_set
:Win32_path
PATH %SCITECH_BIN%;%OW10_PATH%\BINNT;%OW10_PATH%\BINW;%DEFPATH%%WC_CD_PATH%
:path_set

REM If you set the following to a 1, a TNT DosStyle app will be created.
REM Otherwise a TNT NtStyle app will be created. NtStyle apps will *only*
REM run under real DOS when using our libraries, since we require access
REM to functions that the Win32 API does not support (such as direct access
REM to video memory, calling Int 10h BIOS functions etc). DosStyle apps
REM will however run fine in both DOS and a Win95 DOS box (NT DOS boxes don't
REM work too well).
REM
REM If you are using the RealTime DOS extender, your apps *must* be NtStyle,
REM and hence will never be able to run under Win95 or WinNT, only DOS.

SET DOSSTYLE=1

echo Open Watcom C/C++ 1.0 32-bit DOS compilation environment set up (TNT).
