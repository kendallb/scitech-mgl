@echo off
REM Setup for compiling with GCC for QNX Neutrino (x86) target on Windows host

if .%CHECKED%==.1 goto checked_build
set LIB=%SCITECH_LIB%\LIB\release\qnxnto\gcc\x86
echo Release build enabled.
goto setvars

:checked_build
set LIB=%SCITECH_LIB%\LIB\debug\qnxnto\gcc\x86
echo Checked debug build enabled.
goto setvars

:setvars
call clrvars.bat
set MAKE_MODE=UNIX
set GCC_LIBBASE=gcc3
set USE_QNX=1
set USE_QNXNTO=1
set USE_BIOS=1
set USE_X86=1
set UNIX_TARGET=1
set SNAP_TARGET_ARCH=x86
set MAKESTARTUP=%SCITECH%\makedefs\qnxnto.mk
set QNX_HOST=%QNX6_PATH%\host\win32\x86
set QNX_TARGET=%QNX6_PATH%\target\qnx6
if "%QNX_CONFIGURATION"=="" set QNX_CONFIGURATION=%QNX6_PATH%\qss
set QNXPATH=%QNX_HOST%\usr\bin;%QNX_HOST%\usr\ntox86\bin

set PATH=%SCITECH_BIN%;%QNXPATH%;%DEFPATH%

echo GCC QNX Neutrino cross compilation environment set up (x86)
