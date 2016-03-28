@echo off
REM Setup environment variables for Microsoft C 386 32-bit edition

set DDKROOT=c:\ddk

if .%CHECKED%==.1 goto checked_build
SET LIB=%SCITECH_LIB%\lib\release\os232\cl386;%LIB%
echo Release build enabled.
goto setvars

:checked_build
SET LIB=%SCITECH_LIB%\lib\debug\os232\cl386;%LIB%
echo Checked debug build enabled.
goto setvars

:setvars
SET INCLUDE=.;INCLUDE;%SCITECH%\INCLUDE;%PRIVATE%\INCLUDE;%DDKROOT%\video\tools\OS2.386\lx.386\bin\mcl386\include;%DDKROOT%\video\rel\os2c\include\base\os2\16bit;
SET MAKESTARTUP=%SCITECH%\MAKEDEFS\cl386.mk
SET USE_TNT=
SET USE_OS232=
SET USE_VDD=1
SET USE_RTTARGET=
SET HOST_OS=os2
SET HOST_PROC=386
SET CL_LIBBASE=cl386
SET USE_MASM=1
PATH %SCITECH%\bin-os2;%DDKROOT%\video\tools\os2.386\lx.386\bin\mcl386\bin;%DDKROOT%\base\tools;%DDKROOT%\tools;%SCITECH%\bin-os2;%DEFPATH%

set MASTER_MAKE=1
set MASM_ROOT=%DDKROOT%\base\tools
set CL386_ROOT=%DDKROOT%\video\tools\os2.386\lx.386\bin\mcl386\bin

echo Microsoft 386 C 32-bit driver compilation environment set up
