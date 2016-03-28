@echo off
rem -- WinCE DDK environment variable setup
set _WINCEOSVER=420
set _WINCEOSVERMAJOR=4
set _WINCEOSVERMINOR=2
call winceddk.bat

rem -- SciTech DMAKE setup
if .%CHECKED%==.1 goto checked_build
set LIB=%SCITECH_LIB%\LIB\RELEASE\CEDRV\EVC4;%WINCE_SYSGEN%;%WINCE_LIB%;.
echo Release build enabled.
goto setvars

:checked_build
set LIB=%SCITECH_LIB%\LIB\DEBUG\CEDRV\EVC4;%WINCE_SYSGEN%;%WINCE_LIB%;.
echo Checked debug build enabled.
goto setvars

:setvars
set TOOLROOTDIR=%WINCE_ROOT%\SDK\bin\i386
set C_INCLUDE=%WINCE_INC%;
set INCLUDE=.;INCLUDE;%SCITECH%\INCLUDE;%PRIVATE%\INCLUDE;%C_INCLUDE%
SET MAKESTARTUP=%SCITECH%\MAKEDEFS\VC32.MK
call clrvars.bat
SET USE_WINCE=1
SET VC_LIBBASE=evc4
PATH %SCITECH_BIN%;%WINCE_PATH%;%DEFPATH%

:done
echo Windows CE Embedded Visual C++ 4.x compilation environment set up

