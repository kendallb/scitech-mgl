#! /bin/sh

# Setup for compiling with GCC/G++ for Unununium

if [ "$CHECKED" = "1" ]; then
    echo Checked debug build enabled.
else
    echo Release build enabled.
fi

export INCLUDE="-Iinclude -I$SCITECH/include -I$PRIVATE/include"
export USE_UUU=1
export UNIX_TARGET=1
unset  USE_LINUX
unset  USE_SNAP
unset  USE_SNAP_DRV
export MAKESTARTUP=$SCITECH/makedefs/gcc_uuu.mk

if [ "x$USE_PPC_BE" != x ]; then
    export SCITECH_DEBUG_LIB_PATH=$SCITECH/lib/debug/uuu/gcc/ppc-be
    export SCITECH_RELEASE_LIB_PATH=$SCITECH/lib/release/uuu/gcc/ppc-be
    echo "GCC Unununium console compilation environment set up (PPC Big Endian)"
elif [ "x$USE_ALPHA" != x ]; then
    export SCITECH_DEBUG_LIB_PATH=$SCITECH/lib/debug/uuu/gcc/alpha
    export SCITECH_RELEASE_LIB_PATH=$SCITECH/lib/release/uuu/gcc/alpha
    echo "GCC Unununium console compilation environment set up (Alpha)"
else
    export SCITECH_DEBUG_LIB_PATH=$SCITECH/lib/debug/uuu/gcc/x86
    export SCITECH_RELEASE_LIB_PATH=$SCITECH/lib/release/uuu/gcc/x86
    echo "GCC Unununium console compilation environment set up (x86)"
fi

if [ "$CHECKED" = "1" ]; then
    export SCITECH_LIB_PATH=$SCITECH_DEBUG_LIB_PATH
else
    export SCITECH_LIB_PATH=$SCITECH_RELEASE_LIB_PATH
fi
export LD_LIBRARY_PATH=$SCITECH_LIB_PATH/so:$LD_LIBRARY_DEFPATH

