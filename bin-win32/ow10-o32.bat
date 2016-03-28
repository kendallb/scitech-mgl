@echo off
REM Setup for compiling with Open Watcom C/C++ 1.0 in 32-bit OS/2 mode

if .%CHECKED%==.1 goto checked_build
SET LIB=%SCITECH_LIB%\lib\release\os232\ow10;.
echo Release build enabled.
goto setvars

:checked_build
SET LIB=%SCITECH_LIB%\lib\debug\os232\ow10;.
echo Checked debug build enabled.
goto setvars

:setvars
SET EDPATH=%OW10_PATH%\eddat
SET INCLUDE=include;%SCITECH%\include;%PRIVATE%\include;%OW10_PATH%\h;%OW10_PATH%\h\os2;
SET WATCOM=%OW10_PATH%
SET MAKESTARTUP=%SCITECH%\makedefs\wc32.mk
call clrvars.bat
SET USE_OS232=1
SET WC_LIBBASE=ow10
SET EDPATH=%OW10_PATH%\EDDAT
IF .%OS%==.Windows_NT goto Win32_path
IF NOT .%WINDIR%==. goto Win32_path
PATH %SCITECH_BIN%;%OW10_PATH%\BINW;%DEFPATH%%WC_CD_PATH%
goto path_set
:Win32_path
PATH %SCITECH_BIN%;%OW10_PATH%\BINNT;%OW10_PATH%\BINW;%DEFPATH%%WC_CD_PATH%
:path_set

echo Open Watcom C/C++ 1.0 32-bit OS/2 console compilation environment set up.
