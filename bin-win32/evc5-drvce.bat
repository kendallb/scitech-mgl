@echo off
rem -- WinCE DDK environment variable setup
set _WINCEOSVER=500
set _WINCEOSVERMAJOR=5
set _WINCEOSVERMINOR=0
call winceddk.bat
set WINCE_PATH=%WINCE_PATH%;%WINCE_ROOT%\SDK\bin\i386\x86;

rem -- WinCE project sysgen path setup
set WINCE_PROJECT=%WINCE_ROOT%\pbworkspaces\%_TGTPROJ%
set WINCE_SYSGEN=%WINCE_PROJECT%\%WINCE_BUILD%\%_TGTPLAT%_%_TGTCPU%\cesysgen\oak\lib\%_TGTCPU%\retail
set WINCE_SYSGEN=%WINCE_SYSGEN%;%WINCE_PROJECT%\%WINCE_BUILD%\%_TGTPLAT%_%_TGTCPU%\cesysgen\sdk\lib\%_TGTCPU%\retail

rem -- SciTech DMAKE setup
if .%CHECKED%==.1 goto checked_build
set LIB=%SCITECH_LIB%\LIB\RELEASE\CEDRV\EVC5;%WINCE_SYSGEN%;%WINCE_LIB%;.
echo Release build enabled.
goto setvars

:checked_build
set LIB=%SCITECH_LIB%\LIB\DEBUG\CEDRV\EVC5;%WINCE_SYSGEN%;%WINCE_LIB%;.
echo Checked debug build enabled.
goto setvars

:setvars
set TOOLROOTDIR=%WINCE_ROOT%\SDK\bin\i386
set C_INCLUDE=%WINCE_INC%;
set INCLUDE=.;INCLUDE;%SCITECH%\INCLUDE;%PRIVATE%\INCLUDE;%C_INCLUDE%
SET MAKESTARTUP=%SCITECH%\MAKEDEFS\VC32.MK
call clrvars.bat
SET USE_WINCE=1
SET VC_LIBBASE=evc5
PATH %SCITECH_BIN%;%WINCE_PATH%;%DEFPATH%

:done
echo Windows CE Embedded Visual C++ 5.x compilation environment set up

