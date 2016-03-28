@echo off
rem Batch file to build all MGL libraries in release mode for the
rem currently active compiler. Speed optimisations are enabled, and
rem no debug information or extra runtime checks are performed.
rem We also turn down the warning level.

set DBG=
set OPT=1
set OPT_SIZE=
set CHECKED=
set MAX_WARN=
dmake build
