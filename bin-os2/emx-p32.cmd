@echo off
REM Setup for compiling with EMX 0.9c in 32-bit OS/2 mode

if .%CHECKED%==.1 goto checked_build
SET LIB=%SCITECH_LIB%\lib\release\os232\emx;%EMX_PATH%\lib;.
echo Release build enabled.
goto setvars

:checked_build
SET LIB=%SCITECH_LIB%\lib\debug\os232\emx;%EMX_PATH%\lib;.
echo Checked debug build enabled.
goto setvars

:setvars
SET INCLUDE=include;%SCITECH%\include;%PRIVATE%\include;%EMX_PATH%\include\cpp;%EMX_PATH%\include
SET C_INCLUDE_PATH=%INCLUDE%
SET CPLUS_INCLUDE_PATH=%INCLUDE%
SET LIBRARY_PATH=%LIB%
SET PROTODIR=%EMX_PATH%\include\cpp\gen
SET GCCLOAD=5
SET GCCOPT=-pipe
SET TERM=mono
SET TERMCAP=%EMX_PATH%\etc\termcap.dat
SET INFOPATH=%EMX_PATH%\info
SET MAKESTARTUP=%SCITECH%\makedefs\emx.mk
SET USE_OS232=1
SET USE_OS2GUI=1
SET EMX_LIBBASE=emx
PATH %SCITECH%\bin-os2;%EMX_PATH%\bin;%DEFPATH%

echo EMX C/C++ 0.9c 32-bit OS/2 GUI compilation environment set up.
