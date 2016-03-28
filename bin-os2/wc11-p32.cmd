@echo off
REM Setup for compiling with Watcom C/C++ 11.0 in 32-bit OS/2 mode

if .%CHECKED%==.1 goto checked_build
SET LIB=%SCITECH_LIB%\lib\release\os232\wc11;.;%OS2TK_PATH%\LIB
echo Release build enabled.
goto setvars

:checked_build
SET LIB=%SCITECH_LIB%\lib\debug\os232\wc11;.;%OS2TK_PATH%\LIB
echo Checked debug build enabled.
goto setvars

:setvars
SET EDPATH=%WC11_PATH%\eddat
SET INCLUDE=include;%SCITECH%\include;%PRIVATE%\include;%WC11_PATH%\h\os2;%WC11_PATH%\h;%OS2TK_PATH%\h
SET WATCOM=%WC11_PATH%
SET MAKESTARTUP=%SCITECH%\makedefs\wc32.mk
SET USE_WIN16=
SET USE_WIN32=
SET USE_WIN386=
SET USE_OS216=
SET USE_OS232=1
SET USE_OS2GUI=1
SET USE_SNAP=
SET WC_LIBBASE=wc11
SET EDPATH=%WC11_PATH%\EDDAT
PATH %SCITECH%\bin-os2;%WC11_PATH%\binp;%WC11_PATH%\binw;%DEFPATH%%WC_CD_PATH%
SET BEGINLIBPATH=%WC11_PATH%\binp\dll
SET USE_LIB_PATH=%LIB%

echo Watcom C/C++ 11.0 32-bit OS/2 GUI compilation environment set up.
