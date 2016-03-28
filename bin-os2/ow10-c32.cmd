@echo off
REM Setup for compiling with Open Watcom 1.x in 32 bit mode

if .%CHECKED%==.1 goto checked_build
SET LIB=%SCITECH_LIB%\LIB\RELEASE\WIN32\OW10;%OW10_PATH%\LIB386;%OW10_PATH%\LIB386\NT;.
echo Release build enabled.
goto setvars

:checked_build
SET LIB=%SCITECH_LIB%\LIB\DEBUG\WIN32\OW10;%OW10_PATH%\LIB386;%OW10_PATH%\LIB386\NT;.
echo Checked debug build enabled.
goto setvars

:setvars
SET EDPATH=%OW10_PATH%\EDDAT
SET C_INCLUDE=%OW10_PATH%\H;%OW10_PATH%\H\NT
SET INCLUDE=INCLUDE;%SCITECH%\INCLUDE;%PRIVATE%\INCLUDE;%C_INCLUDE%
SET WATCOM=%OW10_PATH%
SET MAKESTARTUP=%SCITECH%\MAKEDEFS\WC32.MK
SET USE_TNT=
SET USE_X32=
SET USE_X32VM=
SET USE_WIN16=
SET USE_WIN32=1
SET USE_WIN386=
SET WIN32_GUI=
SET USE_OS216=
SET USE_OS232=
SET USE_OS2GUI=
SET USE_SNAP=
SET WC_LIBBASE=ow10
SET EDPATH=%OW10_PATH%\EDDAT
PATH %SCITECH%\bin-os2;%OW10_PATH%\binp;%OW10_PATH%\binw;%DEFPATH%%WC_CD_PATH%
SET BEGINLIBPATH=%OW10_PATH%\binp\dll
SET USE_LIB_PATH=%LIB%

REM: Enable Win32 SDK if desired (sdk on command line)
if NOT .%1%==.sdk goto done
call win32sdk.cmd

:done
echo Open Watcom 1.x Win32 console compilation environment set up.
