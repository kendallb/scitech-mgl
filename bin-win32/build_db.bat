@echo off
rem Enable checked build and build debug code
set CHECKED=1
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
call build_it %__A1% %__A2% %__A3% %__A4% %__A5% %__A6% %__A7% %__A8% %__A9% %1 %2 %3 %4 %5 %6 %7 %8 %9
if ERRORLEVEL 1 exit 1
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

