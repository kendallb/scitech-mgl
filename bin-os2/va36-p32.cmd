@echo off
REM Setup for compiling with IBM VisualAge C++ 3.65 in 32-bit mode

if not exist %VA365_PATH%\BIN\setenvtk.cmd echo "VisualAge C++ 3.65 not installed or incorrectly installed"
call %VA365_PATH%\BIN\setenvtk.cmd

if .%CHECKED%==.1 goto checked_build
SET LIB=%SCITECH_LIB%\lib\release\os232\va36;%OS2TK_PATH%\LIB;%VA365_PATH%\LIB
echo Release build enabled.
goto setvars

:checked_build
SET LIB=%SCITECH_LIB%\lib\debug\os232\va36;%OS2TK_PATH%\LIB;%VA365_PATH%\LIB
echo Checked debug build enabled.
goto setvars

:setvars
SET INCLUDE=.\include;%SCITECH%\include;%PRIVATE%\include;%OS2TK_PATH%\H;%VA365_PATH%\INCLUDE
SET MAKESTARTUP=%SCITECH%\makedefs\va365.mk
SET USE_OS232=1
SET USE_OS2GUI=1
SET VA_LIBBASE=va36
PATH %SCITECH%\bin-os2;%OS2TK_PATH%\BIN;%VA365_PATH%\BIN;%DEFPATH%

echo IBM VisualAge C++ 3.65 OS/2 32-bit GUI compilation environment set up.
