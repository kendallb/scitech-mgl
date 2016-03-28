@echo off
rem Batch file to build all MGL libraries in debug mode for the
rem currently active compiler. Optimisations are *NOT* enabled, so
rem that debugging is easier. We also turn down the warning level. You
rem may want to set MAX_WARN=1 if you want maximum warnings.

set DBG=1
set OPT=
set OPT_SIZE=
set CHECKED=1
set MAX_WARN=
dmake build
