@echo off
REM Setup environment variables for Microsoft C 6.0 16 bit edition

set DDKROOT=c:\ddk
set C16_ROOT=c:\ddktools\toolkits\msc60
set MASM_ROOT=c:\ddktools\toolkits\masm60

if .%CHECKED%==.1 goto checked_build
set LIB=%C16_ROOT%\LIB;.
echo Release build enabled.
goto setvars

:checked_build
set LIB=%C16_ROOT%\LIB;.
echo Checked debug build enabled.
goto setvars

:setvars
set INCLUDE=.;INCLUDE;%SCITECH%\INCLUDE;%PRIVATE%\INCLUDE;%C16_ROOT%\INCLUDE
SET MAKESTARTUP=%SCITECH%\MAKEDEFS\CL16.MK
SET USE_TNT=
SET USE_OS216=1
SET USE_RTTARGET=
SET HOST_OS=os2
SET HOST_PROC=386
PATH %SCITECH%\bin-os2;%C16_ROOT%\BINP;%DDKROOT%\tools;%MASM_ROOT%\binp;%SCITECH%\bin-os2;%DEFPATH%

set MASTER_MAKE=1
set USE_MASM=1

echo Microsoft C 6.0 16-bit driver compilation environment set up
