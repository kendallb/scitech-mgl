@echo off
REM Setup environment variables for Visual C++.NET 7.0 32 bit edition

SET MSDevDir=%VC7_MSDevDir%
SET MSVCDir=%VC7_MSVCDir%

if .%CHECKED%==.1 goto checked_build
set LIB=%SCITECH_LIB%\LIB\RELEASE\WIN32\VC7;%MSVCDir%\LIB;%MSVCDir%\ATLMFC\LIB;%MSVCDir%\LIB;%MSVCDir%\PlatformSDK\lib;.
echo Release build enabled.
goto setvars

:checked_build
set LIB=%SCITECH_LIB%\LIB\DEBUG\WIN32\VC7;%MSVCDir%\LIB;%MSVCDir%\ATLMFC\LIB;%MSVCDir%\LIB;%MSVCDir%\PlatformSDK\lib;.
echo Checked debug build enabled.
goto setvars

:setvars
set C_INCLUDE=%MSVCDir%\ATLMFC\INCLUDE;%MSVCDir%\INCLUDE;%MSVCDir%\PlatformSDK\include;
set INCLUDE=.;INCLUDE;%SCITECH%\INCLUDE;%PRIVATE%\INCLUDE;%C_INCLUDE%
SET MAKESTARTUP=%SCITECH%\MAKEDEFS\VC32.MK
call clrvars.bat
SET USE_WIN32=1
SET WIN32_GUI=1
SET VC_LIBBASE=vc7
PATH %SCITECH_BIN%;%MSDevDir%;%MSVCDir%\BIN;%MSVCDir%\Common7\Tools;%MSVCDir%\Common7\Tools\bin;%DEFPATH%

echo Visual C++.NET 7.0 32-bit Windows compilation environment set up
