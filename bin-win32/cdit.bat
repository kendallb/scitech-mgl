@echo off
if not exist %1\*.* goto Error
cd %1
k_rm -f *.lib *.a
shift 1
set __PROG=%1
shift 1
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
%__PROG% %__A1% %__A2% %__A3% %__A4% %__A5% %__A6% %__A7% %__A8% %__A9% %1 %2 %3 %4 %5 %6 %7 %8 %9
if ERRORLEVEL 1 goto error2
if ERRORLEVEL 0 goto continue
:error2
set __PROG=
set __A1=
set __A2=
set __A3=
set __A4=
set __A5=
set __A6=
set __A7=
set __A8=
set __A9=
exit 1
:continue
set __PROG=
set __A1=
set __A2=
set __A3=
set __A4=
set __A5=
set __A6=
set __A7=
set __A8=
set __A9=
goto End
:Error
echo Directory '%1' not found!
exit 1
:End
exit 0
