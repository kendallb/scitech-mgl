@echo off
REM:=========================================================================
REM: Master batch file to set up all necessary environment variables for
REM: building Qt for Windows with Visual C++ 6.0, Platform SDK and
REM: MySQL client libraries.
REM:=========================================================================

echo Trolltech Qt and MySQL client library support enabled.
call vc60-w32 sdk
set INCLUDE=c:\mysql\include;%INCLUDE%
set LIB=c:\mysql\lib\opt;%LIB%
set PATH=c:\mysql\bin;%PATH%

