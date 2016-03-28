@echo off
REM Setup environment variables for Visual C++ 6.0 32 bit edition

SET MSDevDir=%VC6_MSDevDir%
SET MSVCDir=%VC6_MSVCDir%

if .%CHECKED%==.1 goto checked_build
set LIB=%SCITECH_LIB%\LIB\RELEASE\RTT32\VC6;%RTOS32_PATH%\LIBMSVC;%MSVCDir%\LIB;.
echo Release build enabled.
goto setvars

:checked_build
set LIB=%SCITECH_LIB%\LIB\DEBUG\RTT32\VC6;%RTOS32_PATH%\LIBMSVC;%MSVCDir%\LIB;.
echo Checked debug build enabled.
goto setvars

:setvars
set TOOLROOTDIR=%MSVCDir%
set C_INCLUDE=%RTOS32_PATH%\INCLUDE;%MSVCDir%\INCLUDE;
set INCLUDE=.;INCLUDE;%SCITECH%\INCLUDE;%PRIVATE%\INCLUDE;%C_INCLUDE%
set INIT=%MSVCDir%
SET MAKESTARTUP=%SCITECH%\MAKEDEFS\VC32.MK
call clrvars.bat
SET USE_RTTARGET=1
SET USE_RTFILES32=
SET USE_RTKERNEL32=
SET USE_RTT32DLL=
SET VC_LIBBASE=vc6
PATH %SCITECH_BIN%;%RTOS32_PATH%\BIN;%MSVCDir%\BIN;%MSDevDir%\BIN;%DEFPATH%%VC32_CD_PATH%

:done
echo Visual C++ 6.0 32-bit RTTarget-32 compilation environment set up
