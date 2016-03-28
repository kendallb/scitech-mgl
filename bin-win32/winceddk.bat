@echo off
rem -- WinCE development path setup
set WINCE_BUILD=build
set WINCE_ROOT=%WINCE_DDKROOT%%_WINCEOSVER%\%WINCE_BUILD%
set WINCE_DDK=%WINCE_ROOT%\public\common\ddk
set WINCE_OAK=%WINCE_ROOT%\public\common\oak
set WINCE_SDK=%WINCE_ROOT%\public\common\sdk
set WINCE_INC=%WINCE_OAK%\inc;%WINCE_DDK%\inc;%WINCE_SDK%\inc;
set WINCE_LIB=%WINCE_OAK%\lib\x86\retail;%WINCE_DDK%\lib\x86\retail;%WINCE_SDK%\lib\x86\retail;
set WINCE_PATH=%WINCE_ROOT%\SDK\bin\i386;%WINCE_OAK%\bin\i386;%WINCE_OAK%\misc;

rem -- Microsoft NMAKE setup
set _WINCEROOT=%WINCE_ROOT%
set _TGTCPU=x86
set _TGTPLAT=cepc
set _TGTPROJ=settopbox
rem call %WINCE_OAK%\misc\wince.bat
rem call %WINCE_OAK%\misc\setenv.bat
set _COMMONOAKROOT=%_WINCEROOT%\public\common\oak
set _SYSGENOAKROOT=%_WINCEROOT%\public\%_TGTPROJ%\wince%_WINCEOSVER%\%_TGTPLAT%\cesysgen\oak

rem -- WinCE project sysgen path setup
set WINCE_PROJECT=%WINCE_ROOT%\public\%_TGTPROJ%
set WINCE_SYSGEN=%WINCE_PROJECT%\%WINCE_BUILD%\%_TGTPLAT%\cesysgen\oak\lib\x86\retail


