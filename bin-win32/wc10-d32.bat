@echo off
REM Setup for compiling with Watcom C/C++ 10.6 in 32 bit mode (DOS4GW)

if .%CHECKED%==.1 goto checked_build
SET LIB=%SCITECH_LIB%\LIB\RELEASE\DOS32\WC10;%WC10_PATH%\LIB386\DOS;%WC10_PATH%\LIB386;.
echo Release build enabled.
goto setvars

:checked_build
SET LIB=%SCITECH_LIB%\LIB\DEBUG\DOS32\WC10;%WC10_PATH%\LIB386\DOS;%WC10_PATH%\LIB386;.
echo Checked debug build enabled.
goto setvars

:setvars
SET EDPATH=%WC10_PATH%\EDDAT
SET INCLUDE=INCLUDE;%SCITECH%\INCLUDE;%PRIVATE%\INCLUDE;%WC10_PATH%\H;
SET WATCOM=%WC10_PATH%
SET MAKESTARTUP=%SCITECH%\MAKEDEFS\WC32.MK
call clrvars.bat
SET WC_LIBBASE=wc10
IF .%OS%==.Windows_NT goto Win32_path
IF NOT .%WINDIR%==. goto Win32_path
PATH %SCITECH_BIN%;%WC10_PATH%\BINW;%DEFPATH%%WC_CD_PATH%
goto path_set
:Win32_path
PATH %SCITECH_BIN%;%WC10_PATH%\BINNT;%WC10_PATH%\BINW;%DEFPATH%%WC_CD_PATH%
:path_set

echo Watcom C/C++ 10.6 32-bit DOS compilation environment set up (DOS4GW)

