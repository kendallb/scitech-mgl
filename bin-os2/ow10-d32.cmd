@echo off
REM Setup for compiling with Open Watcom 1.0 in 32-bit DOS mode (DOS/4GW)

if .%CHECKED%==.1 goto checked_build
SET LIB=%SCITECH_LIB%\lib\release\dos32\ow10;
echo Release build enabled.
goto setvars

:checked_build
SET LIB=%SCITECH_LIB%\lib\debug\dos32\ow10;
echo Checked debug build enabled.
goto setvars

:setvars
SET EDPATH=%OW10_PATH%\eddat
SET INCLUDE=include;%SCITECH%\include;%PRIVATE%\include;%OW10_PATH%\h;
SET WATCOM=%OW10_PATH%
SET MAKESTARTUP=%SCITECH%\makedefs\wc32.mk
SET USE_TNT=
SET USE_X32=
SET USE_X32VM=
SET USE_WIN16=
SET USE_WIN32=
SET USE_WIN386=
SET USE_OS216=
SET USE_OS232=
SET USE_OS2GUI=
SET USE_SNAP=
SET WC_LIBBASE=ow10
SET EDPATH=%OW10_PATH%\EDDAT
PATH %SCITECH%\bin-os2;%OW10_PATH%\binp;%OW10_PATH%\binw;%DEFPATH%%WC_CD_PATH%
SET BEGINLIBPATH=%OW10_PATH%\binp\dll
SET USE_LIB_PATH=%LIB%

echo Open Watcom C/C++ 1.0 32-bit DOS compilation environment set up (DOS/4GW).
