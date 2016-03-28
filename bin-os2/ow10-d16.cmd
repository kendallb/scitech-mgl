@echo off
REM Setup for compiling with Open Watcom C/C++ 1.x in 16 bit mode

if .%CHECKED%==.1 goto checked_build
SET LIB=%SCITECH_LIB%\LIB\RELEASE\DOS16\OW10;.
echo Release build enabled.
goto setvars

:checked_build
SET LIB=%SCITECH_LIB%\LIB\DEBUG\DOS16\OW10;.
echo Checked debug build enabled.
goto setvars

:setvars
SET EDPATH=%OW10_PATH%\EDDAT
SET INCLUDE=INCLUDE;%SCITECH%\INCLUDE;%PRIVATE%\INCLUDE;%OW10_PATH%\H;%OW10_PATH%\H\WIN;
SET WATCOM=%OW10_PATH%
SET MAKESTARTUP=%SCITECH%\MAKEDEFS\WC16.MK
SET USE_WIN16=
SET USE_WIN32=
SET USE_OS216=
SET USE_OS232=
SET USE_OS2GUI=
SET USE_SNAP=
SET WC_LIBBASE=ow10
SET EDPATH=%WC11_PATH%\EDDAT
PATH %SCITECH%\bin-os2;%OW10_PATH%\binp;%OW10_PATH%\binw;%DEFPATH%%WC_CD_PATH%
SET BEGINLIBPATH=%OW10_PATH%\binp\dll
SET USE_LIB_PATH=%LIB%

echo Open Watcom C/C++ 1.x 16-bit DOS compilation environment set up.
