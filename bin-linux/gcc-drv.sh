#! /bin/sh

# Setup for compiling ELF SNAP modules with GCC/G++ for Linux

if [ "$CHECKED" = "1" ]; then
    echo Checked debug build enabled.
else
    echo Release build enabled.
fi

export INCLUDE="-Iinclude -I$SCITECH/include -I$PRIVATE/include"
export USE_SNAP=1
export USE_SNAP_DRV=1
export UNIX_TARGET=1
unset  USE_LINUX
unset  USE_UUU
export MAKESTARTUP=$SCITECH/makedefs/gcc_linux.mk

if [ "x$USE_PPC_BE" != x ]; then
    export LD_LIBRARY_PATH=$SCITECH/lib/debug/linux/gcc/ppc-be/so:$LD_LIBRARY_DEFPATH
    echo "GCC ELF SNAP driver compilation environment set up (PPC Big Endian)"
elif [ "x$USE_ALPHA" != x ]; then
    export LD_LIBRARY_PATH=$SCITECH/lib/debug/linux/gcc/alpha/so:$LD_LIBRARY_DEFPATH
    echo "GCC ELF SNAP driver environment set up (Alpha)"
elif [ "x$USE_X86_64" != x ]; then
    export LD_LIBRARY_PATH=$SCITECH/lib/debug/linux/gcc/x86-64/so:$LD_LIBRARY_DEFPATH
    echo "GCC ELF SNAP driver environment set up (x86-64)"
else
    export LD_LIBRARY_PATH=$SCITECH/lib/debug/linux/gcc/x86/so:$LD_LIBRARY_DEFPATH
    echo "GCC ELF SNAP driver compilation environment set up (x86)"
fi

