#! /bin/sh
# Clear all makefile variables that might be used

unset  USE_LINUX
unset  USE_WIN32
unset  WIN32_GUI
unset  USE_OS232
unset  USE_OS2GUI
unset  USE_X86
unset  USE_X86_64
unset  USE_PPC_BE
unset  USE_PPC_LE
unset  USE_MIPS_LE
unset  USE_MIPS_BE
unset  USE_ALPHA
unset  USE_UUU
unset  USE_SNAP
unset  USE_SNAP_DRV
unset  UNIX_TARGET

# Restore the path to the original clean path with no 
# compiler dependant paths in front of it (GNU C/C++ tools
# will be on the path because they are considered system
# compilers)
export PATH=$DEFPATH

