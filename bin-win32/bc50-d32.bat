@echo off
REM Setup for compiling with Borland C++ 5.0 in 32 bit mode.

if .%CHECKED%==.1 goto checked_build
SET LIB=%SCITECH_LIB%\LIB\RELEASE\DOS32\BC5;%BC5_PATH%\LIB;.
echo Release build enabled.
goto setvars

:checked_build
SET LIB=%SCITECH_LIB%\LIB\DEBUG\DOS32\BC5;%BC5_PATH%\LIB;.
echo Checked debug build enabled.
goto setvars

:setvars
SET INCLUDE=INCLUDE;%SCITECH%\INCLUDE;%PRIVATE%\INCLUDE;%BC5_PATH%\INCLUDE;
SET MAKESTARTUP=%SCITECH%\MAKEDEFS\BC32.MK
call clrvars.bat
SET USE_BC5=1
SET BC_LIBBASE=BC5
PATH %SCITECH_BIN%;%BC5_PATH%\BIN;%DEFPATH%%BC5_CD_PATH%

REM: Create Borland compile/link configuration scripts
echo -I%INCLUDE% > %BC5_PATH%\BIN\bcc32.cfg
echo -L%LIB% >> %BC5_PATH%\BIN\bcc32.cfg
echo -L%LIB% > %BC5_PATH%\BIN\tlink32.cfg

echo Borland C++ 5.0 32 bit DOS compilation configuration set up (DPMI32).
