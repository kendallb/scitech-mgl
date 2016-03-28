@echo off
REM Setup environment variables for Visual C++ 5.0 32 bit edition

if .%CHECKED%==.1 goto checked_build
set LIB=%SCITECH_LIB%\LIB\RELEASE\RTT32\VC5;%RTOS32_PATH%\LIBMSVC;%VC5_PATH%\VC\LIB;.
echo Release build enabled.
goto setvars

:checked_build
set LIB=%SCITECH_LIB%\LIB\DEBUG\RTT32\VC5;%RTOS32_PATH%\LIBMSVC;%VC5_PATH%\VC\LIB;.
echo Checked debug build enabled.
goto setvars

:setvars
set TOOLROOTDIR=%VC5_PATH%\VC
set C_INCLUDE=%RTOS32_PATH%\INCLUDE;%VC5_PATH%\VC\INCLUDE;
set INCLUDE=.;INCLUDE;%SCITECH%\INCLUDE;%PRIVATE%\INCLUDE;%C_INCLUDE%;
set INIT=%VC5_PATH%\VC
SET MAKESTARTUP=%SCITECH%\MAKEDEFS\VC32.MK
call clrvars.bat
SET USE_RTTARGET=1
SET VC_LIBBASE=vc5
PATH %SCITECH_BIN%;%RTOS32_PATH%\BIN;%VC5_PATH%\VC\BIN;%VC5_PATH%\SHAREDIDE\BIN;%DEFPATH%%VC32_CD_PATH%

echo Visual C++ 5.0 RTTarget-32 compilation environment set up
