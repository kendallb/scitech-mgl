@echo off
REM Setup for compiling with IBM VisualAge C++ 3.0 in 32-bit mode

SET DDKROOT=C:\DDK

if .%CHECKED%==.1 goto checked_build
SET LIB=%SCITECH_LIB%\lib\release\os232\va3;%OS2TK_PATH%\lib;%VA3_PATH%\LIB;%VA3_PATH%\DLL
echo Release build enabled.
goto setvars

:checked_build
SET LIB=%SCITECH_LIB%\lib\debug\os232\va3;%OS2TK_PATH%\lib;%VA3_PATH%\LIB;%VA3_PATH%\DLL
echo Checked debug build enabled.
goto setvars

:setvars
SET INCLUDE=include;%SCITECH%\include;%PRIVATE%\include;%OS2TK_PATH%\H;%VA3_PATH%\INCLUDE;%VA3_PATH%\INCLUDE\OS2;%VA3_PATH%\INC;%VA3_PATH%\INCLUDE\SOM
SET MAKESTARTUP=%SCITECH%\makedefs\va32.mk
SET USE_OS232=1
SET USE_OS2GUI=
SET VA_LIBBASE=va3
set USER_MSC=c:\ddk\msc60
set USER_VACPP=%VA3_PATH%
set USER_CSET=%VA3_PATH%
set HOST_PROC=386
set HOST_OS=os2
PATH %DDKROOT%\tools;%DDKROOT%\masm60\binp;%SCITECH%\bin-os2;%VA3_PATH%\BIN;%VA3_PATH%\HELP;%VA3_PATH%\SMARTS\SCRIPTS;%DEFPATH%%VA_CD_PATH%
set beginlibpath=%USER_VACPP%\dll;%USER_CSET%\dll

echo IBM VisualAge C++ 3.0 OS/2 32-bit device driver compilation environment set up.
