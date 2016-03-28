@echo off
REM Setup for compiling with Open Watcom C/C++ 1.x in 32 bit mode

if .%CHECKED%==.1 goto checked_build
SET LIB=%SCITECH_LIB%\LIB\RELEASE\DRV\OW10;.
echo Release build enabled.
goto setvars

:checked_build
SET LIB=%SCITECH_LIB%\LIB\DEBUG\DRV\OW10;.
echo Checked debug build enabled.
goto setvars

:setvars
SET EDPATH=%OW10_PATH%\EDDAT
SET INCLUDE=INCLUDE;%SCITECH%\INCLUDE\CLIB;%SCITECH%\INCLUDE;%PRIVATE%\INCLUDE
SET WATCOM=%OW10_PATH%
SET MAKESTARTUP=%SCITECH%\MAKEDEFS\WC32.MK
SET USE_WIN16=
SET USE_WIN32=
SET USE_WIN386=
SET USE_OS216=
SET USE_OS232=
SET USE_OS2GUI=
SET USE_SNAP=1
SET USE_SNAP_DRV=1
SET WC_LIBBASE=ow10
PATH %SCITECH%\bin-os2;%OW10_PATH%\binp;%OW10_PATH%\binw;%DEFPATH%%WC_CD_PATH%
SET BEGINLIBPATH=%OW10_PATH%\binp\dll
SET USE_LIB_PATH=%LIB%

echo Open Watcom C/C++ 1.x SNAP Driver compilation environment set up.

