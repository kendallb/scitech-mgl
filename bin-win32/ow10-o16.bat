@echo off
REM Setup for compiling with Open Watcom C/C++ 1.0 in 16-bit OS/2 mode

if .%CHECKED%==.1 goto checked_build
SET LIB=%SCITECH_LIB%\lib\release\os216\ow10;.
echo Release build enabled.
goto setvars

:checked_build
SET LIB=%SCITECH_LIB%\lib\debug\os216\ow10;.
echo Checked debug build enabled.
goto setvars

:setvars
SET EDPATH=%OW10_PATH%\eddat
SET INCLUDE=include;%SCITECH%\include;%PRIVATE%\include;%OW10_PATH%\h\os2;%OW10_PATH%\h
SET WATCOM=%OW10_PATH%
SET MAKESTARTUP=%SCITECH%\makedefs\wc16.mk
SET USE_WIN16=
SET USE_WIN32=
SET USE_WIN386=
SET USE_OS216=1
SET USE_OS232=
SET USE_OS2GUI=
SET USE_SNAP=
SET USE_QNX4=
SET USE_LINUX=
SET WC_LIBBASE=ow10
SET EDPATH=%OW10_PATH%\EDDAT
IF .%OS%==.Windows_NT goto Win32_path
IF NOT .%WINDIR%==. goto Win32_path
PATH %SCITECH_BIN%;%OW10_PATH%\BINW;%DEFPATH%%WC_CD_PATH%
goto path_set
:Win32_path
PATH %SCITECH_BIN%;%OW10_PATH%\BINNT;%OW10_PATH%\BINW;%DEFPATH%%WC_CD_PATH%
:path_set

echo Open Watcom C/C++ 1.0 16-bit OS/2 compilation environment set up.
