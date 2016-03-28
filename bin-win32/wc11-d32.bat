@echo off
REM Setup for compiling with Watcom C/C++ 11.0 in 32 bit mode (DOS4GW)

if .%CHECKED%==.1 goto checked_build
SET LIB=%SCITECH_LIB%\LIB\RELEASE\DOS32\WC11;%WC11_PATH%\LIB386\DOS;%WC11_PATH%\LIB386;.
echo Release build enabled.
goto setvars

:checked_build
SET LIB=%SCITECH_LIB%\LIB\DEBUG\DOS32\WC11;%WC11_PATH%\LIB386\DOS;%WC11_PATH%\LIB386;.
echo Checked debug build enabled.
goto setvars

:setvars
SET EDPATH=%WC11_PATH%\EDDAT
SET INCLUDE=INCLUDE;%SCITECH%\INCLUDE;%PRIVATE%\INCLUDE;%WC11_PATH%\H;
SET WATCOM=%WC11_PATH%
SET MAKESTARTUP=%SCITECH%\MAKEDEFS\WC32.MK
call clrvars.bat
SET WC_LIBBASE=wc11
IF .%OS%==.Windows_NT goto Win32_path
IF NOT .%WINDIR%==. goto Win32_path
PATH %SCITECH_BIN%;%WC11_PATH%\BINW;%DEFPATH%%WC_CD_PATH%
goto path_set
:Win32_path
PATH %SCITECH_BIN%;%WC11_PATH%\BINNT;%WC11_PATH%\BINW;%DEFPATH%%WC_CD_PATH%
:path_set
echo Watcom C/C++ 11.0 32-bit DOS compilation environment set up (DOS4GW).
