@echo off
REM Setup for compiling with Borland C++ Builder 5.0 32 bit RTTarget32.

if .%CHECKED%==.1 goto checked_build
SET LIB=%SCITECH_LIB%\LIB\RELEASE\RTT32\BCB5;%RTOS32_PATH%\LIBBC;%BCB5_PATH%\LIB;.
echo Release build enabled.
goto setvars

:checked_build
SET LIB=%SCITECH_LIB%\LIB\DEBUG\RTT32\BCB5;%RTOS32_PATH%\LIBBC;%BCB5_PATH%\LIB;.
echo Checked debug build enabled.
goto setvars

:setvars
SET C_INCLUDE=%RTOS32_PATH%\INCLUDE;%BCB5_PATH%\INCLUDE
SET INCLUDE=INCLUDE;%SCITECH%\INCLUDE;%PRIVATE%\INCLUDE;%C_INCLUDE%
SET MAKESTARTUP=%SCITECH%\MAKEDEFS\BC32.MK
call clrvars.bat
SET USE_RTTARGET=1
SET USE_RTFILES32=
SET USE_RTKERNEL32=
SET USE_RTT32DLL=
SET USE_BC5=1
SET BC_LIBBASE=BCB5
PATH %SCITECH_BIN%;%BCB5_PATH%\BIN;%DEFPATH%%BC5_CD_PATH%

REM: Enable Win32 SDK if desired (sdk on command line)
if NOT .%1%==.sdk goto createfiles
call win32sdk.bat borland

:createfiles
REM: Create Borland compile/link configuration scripts
echo -I%INCLUDE% > %BCB5_PATH%\BIN\bcc32.cfg
echo -L%LIB% >> %BCB5_PATH%\BIN\bcc32.cfg
echo -L%LIB% > %BCB5_PATH%\BIN\tlink32.cfg
echo -L%LIB% > %BCB5_PATH%\BIN\ilink32.cfg

echo Borland C++ Builder 5.0 32 bit RTTarget32 compilation configuration set up.
