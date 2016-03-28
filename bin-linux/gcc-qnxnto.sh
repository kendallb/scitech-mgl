#! /bin/sh

# Setup for compiling with GCC/G++ for QNX Neutrino (x86) target on Linux host

. $SCITECH/bin-linux/clrvars.sh
export USE_QNX=1
export USE_QNXNTO=1
export USE_BIOS=1
export USE_X86=1
export UNIX_TARGET=1
export SNAP_TARGET_ARCH=x86
export MAKESTARTUP=$SCITECH/makedefs/qnxnto.mk
if [ "$CHECKED" = "1" ]; then
    echo Checked debug build enabled.
    export SCITECH_LIB_PATH=$SCITECH/lib/debug/qnxnto/gcc/x86
else
    echo Release build enabled.
    export SCITECH_LIB_PATH=$SCITECH/lib/release/qnxnto/gcc/x86
fi
export LD_LIBRARY_PATH=$SCITECH_LIB_PATH/so:$LD_LIBRARY_DEFPATH
echo "GCC QNX Neutrino cross compilation environment set up (x86)"

