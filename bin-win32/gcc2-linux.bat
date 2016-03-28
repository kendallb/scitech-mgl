@echo off
REM Setup for compiling with GNU C cross-compiler

if .%CHECKED%==.1 goto checked_build
set LIB=%SCITECH_LIB%\LIB\release\win32\gcc2
echo Release build enabled.
goto setvars

:checked_build
set LIB=%SCITECH_LIB%\LIB\debug\win32\gcc2
echo Checked debug build enabled.
goto setvars

:setvars
set INCLUDE=include;%SCITECH%\include;%PRIVATE%\include
set MAKESTARTUP=%SCITECH%\MAKEDEFS\gcc_linux.mk
call clrvars.bat
set MAKE_MODE=UNIX
set GCC_LIBBASE=gcc2
PATH %SCITECH_BIN%;%GCC_PATH%\cross-linux\i386-redhat-linux\BIN;%DEFPATH%

echo GCC 2.9.x 32-bit Linux console cross compilation environment set up

