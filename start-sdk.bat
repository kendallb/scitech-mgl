@echo off
rem Batch file to create the initial setup for using the SciTech MGL
rem Graphics Library, using the selected default compiler.

rem Setup the default path environment variable

set DEFPATH=C:\WINDOWS\COMMAND;C:\BIN\MSDOS;C:\BIN\COMMON
PATH %DEFPATH%

rem Initialise the default compile time environment

call C:\SCITECH\bin-win32\set-vars.bat
set DEFPATH=%SCITECH%\redist\release;%DEFPATH%
call C:\SCITECH\bin-win32\OW10-W32.BAT

rem Change to the directory containing all the MGL sample code

cd %SCITECH%\EXAMPLES\SNAP\GRAPHICS

