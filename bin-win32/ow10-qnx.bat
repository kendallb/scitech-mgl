@echo off
REM Setup for compiling with Open Watcom C/C++ 1.0 in 32 bit mode (QNX 4)

if .%CHECKED%==.1 goto checked_build
SET LIB=%SCITECH_LIB%\LIB\RELEASE\QNX4\OW10;%OW10_PATH%\LIB386\QNX;%OW10_PATH%\LIB386;.
echo Release build enabled.
goto setvars

:checked_build
SET LIB=%SCITECH_LIB%\LIB\DEBUG\QNX4\OW10;%OW10_PATH%\LIB386\QNX;%OW10_PATH%\LIB386;.
echo Checked debug build enabled.
goto setvars

:setvars
SET EDPATH=%OW10_PATH%\EDDAT
SET INCLUDE=INCLUDE;%SCITECH%\INCLUDE;%PRIVATE%\INCLUDE;%OW10_PATH%\QH;
SET WATCOM=%OW10_PATH%
SET MAKESTARTUP=%SCITECH%\MAKEDEFS\WC32.MK
call clrvars.bat
SET USE_QNX4=1
SET WC_LIBBASE=ow10
IF .%OS%==.Windows_NT goto Win32_path
IF NOT .%WINDIR%==. goto Win32_path
PATH %SCITECH_BIN%;%OW10_PATH%\BINW;%DEFPATH%%WC_CD_PATH%
goto path_set
:Win32_path
PATH %SCITECH_BIN%;%OW10_PATH%\BINNT;%OW10_PATH%\BINW;%DEFPATH%%WC_CD_PATH%
:path_set

echo Open Watcom C/C++ 1.0 32-bit QNX4 compilation environment set up

