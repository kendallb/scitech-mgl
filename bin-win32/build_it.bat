@echo off
rem Generic batch file to build a version of the library. This batch file
rem assumes that the correct batch files exist to setup the appropriate
rem compilation environments, and that the DMAKE.EXE program is available
rem somewhere on the path.
rem
rem Builds as release or debug depending on the value of the CHECKED
rem environment variable.

rem Unset all environment variables that change the compile process
set DBG=
set OPT=
set OPT_SIZE=
set BUILD_DLL=
set IMPORT_DLL=
set FPU=
set CHECKS=
set BETA=
set __A1=%1
shift 1
set __A2=%1
shift 1
set __A3=%1
shift 1
set __A4=%1
shift 1
set __A5=%1
shift 1
set __A6=%1
shift 1
set __A7=%1
shift 1
set __A8=%1
shift 1
set __A9=%1
shift 1
if %__A1%==bc45-d32 goto bc45-d32
if %__A1%==bc45-tnt goto bc45-tnt
if %__A1%==bc45-w32 goto bc45-w32
if %__A1%==bc45-c32 goto bc45-c32
if %__A1%==bc45-vxd goto bc45-vxd
if %__A1%==bc50-d32 goto bc50-d32
if %__A1%==bc50-tnt goto bc50-tnt
if %__A1%==bc50-w32 goto bc50-w32
if %__A1%==bc50-c32 goto bc50-c32
if %__A1%==bc50-vxd goto bc50-vxd
if %__A1%==gcc2-d32 goto gcc2-d32
if %__A1%==gcc2-w32 goto gcc2-w32
if %__A1%==gcc2-c32 goto gcc2-c32
if %__A1%==gcc2-linux goto gcc2-linux
if %__A1%==gcc-qnxnto goto gcc-qnxnto
if %__A1%==vc40-tnt goto vc40-tnt
if %__A1%==vc40-w32 goto vc40-w32
if %__A1%==vc40-c32 goto vc40-c32
if %__A1%==vc40-drv9x goto vc40-drv9x
if %__A1%==vc40-drvnt goto vc40-drvnt
if %__A1%==vc40-rtt goto vc40-rtt
if %__A1%==vc50-tnt goto vc50-tnt
if %__A1%==vc50-w32 goto vc50-w32
if %__A1%==vc50-c32 goto vc50-c32
if %__A1%==vc50-drv9x goto vc50-drv9x
if %__A1%==vc50-drvnt goto vc50-drvnt
if %__A1%==vc50-rtt goto vc50-rtt
if %__A1%==vc60-tnt goto vc60-tnt
if %__A1%==vc60-w16 goto vc60-w16
if %__A1%==vc60-w32 goto vc60-w32
if %__A1%==vc60-c32 goto vc60-c32
if %__A1%==vc60-drv9x goto vc60-drv9x
if %__A1%==vc60-drvnt goto vc60-drvnt
if %__A1%==vc60-drvw2k goto vc60-drvw2k
if %__A1%==vc60-rtt goto vc60-rtt
if %__A1%==vc60-mfc goto vc60-mfc
if %__A1%==evc3-drvce goto evc3-drvce
if %__A1%==evc4-drvce goto evc4-drvce
if %__A1%==evc5-drvce goto evc5-drvce
if %__A1%==wc10ad16 goto wc10ad16
if %__A1%==wc10ad32 goto wc10ad32
if %__A1%==wc10atnt goto wc10atnt
if %__A1%==wc10aw16 goto wc10aw16
if %__A1%==wc10aw32 goto wc10aw32
if %__A1%==wc10ac32 goto wc10ac32
if %__A1%==wc10ao32 goto wc10ao32
if %__A1%==wc10ap32 goto wc10ap32
if %__A1%==wc10asnp goto wc10asnp
if %__A1%==wc10-d32 goto wc10-d32
if %__A1%==wc10-tnt goto wc10-tnt
if %__A1%==wc10-w32 goto wc10-w32
if %__A1%==wc10-c32 goto wc10-c32
if %__A1%==wc10-o32 goto wc10-o32
if %__A1%==wc10-p32 goto wc10-p32
if %__A1%==wc10-qnx goto wc10-qnx
if %__A1%==wc11-d32 goto wc11-d32
if %__A1%==wc11-tnt goto wc11-tnt
if %__A1%==wc11-w32 goto wc11-w32
if %__A1%==wc11-c32 goto wc11-c32
if %__A1%==wc11-o32 goto wc11-o32
if %__A1%==wc11-p32 goto wc11-p32
if %__A1%==ow10-d32 goto ow10-d32
if %__A1%==ow10-tnt goto ow10-tnt
if %__A1%==ow10-w32 goto ow10-w32
if %__A1%==ow10-c32 goto ow10-c32
if %__A1%==ow10-o32 goto ow10-o32
if %__A1%==ow10-p32 goto ow10-p32
if %__A1%==ow10-snp goto ow10-snp
if %__A1%==ow10-drv goto ow10-drv
if %__A1%==ow10-rtt goto ow10-rtt

echo Usage: BUILD 'compiler_name' [DMAKE commands]
echo.
echo Where 'compiler_name' is of the form comp-os, where
echo 'comp' defines the compiler and 'os' defines the OS environment.
echo For instance 'ow10-w32' is for Open Watcom C++ 1.1 for Win32.
echo The value of 'comp' can be any of the following:
echo.
echo    bc45 - Borland C++ 4.5x
echo    bc50 - Borland C++ 5.x
echo    vc40 - Visual C++ 4.x
echo    vc50 - Visual C++ 5.x
echo    vc60 - Visual C++ 6.x
echo    wc10 - Watcom C++ 10.6
echo    wc11 - Watcom C++ 11.x
echo    ow10 - Open Watcom C++ 1.x
echo    gcc2 - GNU C/C++ 2.9x
echo    gcc  - GNU C/C++
echo.
echo The value of 'os' can be one of the following:
echo.
echo    d32    - 32-bit DOS
echo    c32    - 32-bit Windows console mode
echo    w32    - 32-bit Windows GUI mode
echo    o32    - 32-bit OS/2 console mode
echo    p32    - 32-bit OS/2 Presentation Manager
echo    rtt    - 32-bit RTTarget binary
echo    snp    - 32-bit SciTech SNAP application
echo    drv    - 32-bit SciTech SNAP driver
echo    linux  - 32-bit Linux application
echo    qnxnto - 32-bit QNX Neutrino
goto end

rem -------------------------------------------------------------------------
rem Setup for the specified compiler

:bc45-d32
call bc45-d32.bat
goto compileit

:bc45-tnt
call bc45-tnt.bat
goto compileit

:bc45-w32
call bc45-w32.bat
goto compileit

:bc45-c32
call bc45-c32.bat
goto compileit

:bc45-vxd
call bc45-vxd.bat
goto compileit

:bc50-d32
call bc50-d32.bat
goto compileit

:bc50-tnt
call bc50-tnt.bat
goto compileit

:bc50-w32
call bc50-w32.bat
goto compileit

:bc50-c32
call bc50-c32.bat
goto compileit

:bc50-vxd
call bc50-vxd.bat
goto compileit

:gcc2-d32
call gcc2-d32.bat
goto compileit

:gcc2-w32
call gcc2-w32.bat
goto compileit

:gcc2-c32
call gcc2-c32.bat
goto compileit

:gcc2-linux
call gcc2-linux.bat
goto compileit

:gcc-qnxnto
call gcc-qnxnto.bat
goto compileit

:sc70-tnt
call sc70-tnt.bat
goto compileit

:sc70-w32
call sc70-w32.bat
goto compileit

:sc70-c32
call sc70-c32.bat
goto compileit

:vc40-tnt
call vc40-tnt.bat
goto compileit

:vc40-w32
call vc40-w32.bat
goto compileit

:vc40-c32
call vc40-c32.bat
goto compileit

:vc40-drv9x
call vc40-drv9x.bat
goto compileit

:vc40-drvnt
call vc40-drvnt.bat
goto compileit

:vc40-rtt
call vc40-rtt.bat
goto compileit

:vc50-tnt
call vc50-tnt.bat
goto compileit

:vc50-w32
call vc50-w32.bat
goto compileit

:vc50-c32
call vc50-c32.bat
goto compileit

:vc50-drv9x
call vc50-drv9x.bat
goto compileit

:vc50-drvnt
call vc50-drvnt.bat
goto compileit

:vc50-rtt
call vc50-rtt.bat
goto compileit

:vc60-tnt
call vc60-tnt.bat
goto compileit

:vc60-w16
call vc60-w16.bat
goto compileit

:vc60-w32
call vc60-w32.bat
goto compileit

:vc60-c32
call vc60-c32.bat
goto compileit

:vc60-drv9x
call vc60-drv9x.bat
goto compileit

:vc60-drvnt
call vc60-drvnt.bat
goto compileit

:vc60-drvw2k
call vc60-drvw2k.bat
goto compileit

:vc60-rtt
call vc60-rtt.bat
goto compileit

:vc60-mfc
call vc60-mfc.bat
goto compileit

:evc3-drvce
call evc3-drvce.bat
goto compileit

:evc4-drvce
call evc4-drvce.bat
goto compileit

:evc5-drvce
call evc5-drvce.bat
goto compileit

:wc10ad16
call wc10ad16.bat
goto compileit

:wc10ad32
call wc10ad32.bat
goto compileit

:wc10atnt
call wc10atnt.bat
goto compileit

:wc10aw16
call wc10aw16.bat
goto compileit

:wc10aw32
call wc10aw32.bat
goto compileit

:wc10ac32
call wc10ac32.bat
goto compileit

:wc10ao32
call wc10ao32.bat
goto compileit

:wc10ap32
call wc10ap32.bat
goto compileit

:wc10-d32
call wc10-d32.bat
goto compileit

:wc10-tnt
call wc10-tnt.bat
goto compileit

:wc10-w32
call wc10-w32.bat
goto compileit

:wc10-c32
call wc10-c32.bat
goto compileit

:wc10-o32
call wc10-o32.bat
goto compileit

:wc10-p32
call wc10-p32.bat
goto compileit

:wc10-qnx
call wc10-qnx.bat
goto compileit

:wc11-d32
call wc11-d32.bat
goto compileit

:wc11-tnt
call wc11-tnt.bat
goto compileit

:wc11-w32
call wc11-w32.bat
goto compileit

:wc11-c32
call wc11-c32.bat
goto compileit

:wc11-o32
call wc11-o32.bat
goto compileit

:wc11-p32
call wc11-p32.bat
goto compileit

:ow10-d32
call ow10-d32.bat
goto compileit

:ow10-tnt
call ow10-tnt.bat
goto compileit

:ow10-w32
call ow10-w32.bat
goto compileit

:ow10-c32
call ow10-c32.bat
goto compileit

:ow10-o32
call ow10-o32.bat
goto compileit

:ow10-p32
call ow10-p32.bat
goto compileit

:ow10-snp
call ow10-snp.bat
goto compileit

:ow10-drv
call ow10-drv.bat
goto compileit

:ow10-rtt
call ow10-rtt.bat
goto compileit

:compileit
k_rm -f *.lib *.a
dmake %__A2% %__A3% %__A4% %__A5% %__A6% %__A7% %__A8% %__A9% %1 %2 %3 %4 %5 %6 %7 %8 %9
if errorlevel 1 goto errorend
goto end

:errorend
echo *************************************************
echo * An error occurred while building the library. *
echo *************************************************
exit 1
:end
set __A1=
set __A2=
set __A3=
set __A4=
set __A5=
set __A6=
set __A7=
set __A8=
set __A9=
exit 0

