@echo off
REM Setup for compiling with Open Watcom 1.0 in 32-bit OS/2 mode

if .%CHECKED%==.1 goto checked_build
SET LIB=%SCITECH_LIB%\lib\release\os232\ow10;.;%OS2TK_PATH%\LIB
echo Release build enabled.
goto setvars

:checked_build
SET LIB=%SCITECH_LIB%\lib\debug\os232\ow10;.;%OS2TK_PATH%\LIB
echo Checked debug build enabled.
goto setvars

:setvars
SET EDPATH=%OW10_PATH%\eddat
SET INCLUDE=include;%SCITECH%\include;%PRIVATE%\include;%OW10_PATH%\h;%OW10_PATH%\h\os2;%OS2TK_PATH%\h
SET WATCOM=%OW10_PATH%
SET MAKESTARTUP=%SCITECH%\makedefs\wc32.mk
SET USE_WIN16=
SET USE_WIN32=
SET USE_WIN386=
SET USE_OS216=
SET USE_OS232=1
SET USE_OS2GUI=
SET USE_SNAP=
SET WC_LIBBASE=ow10
SET EDPATH=%OW10_PATH%\EDDAT
PATH %SCITECH%\bin-os2;%OW10_PATH%\binp;%OW10_PATH%\binw;%DEFPATH%%WC_CD_PATH%
SET BEGINLIBPATH=%OW10_PATH%\binp\dll
SET USE_LIB_PATH=%LIB%

echo Open Watcom C/C++ 1.0 32-bit OS/2 console compilation environment set up.
