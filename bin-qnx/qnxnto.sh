#! /bin/sh

# Setup for compiling with Watcom C/C++ for QNX Neutrino

if [ "$CHECKED" = "1" ]; then
    echo Checked debug build enabled.
else
    echo Release build enabled.
fi

export MAKESTARTUP=$SCITECH/makedefs/qnxnto.mk
export INCLUDE="-I$SCITECH/include -I$PRIVATE/include -I/usr/nto/include"
export USE_BIOS=1	# VBIOS lib is tiny under Neutrino, always include it
export USE_QNX=1
export USE_QNXNTO=1
export USE_X86=1
export UNIX_TARGET=1
export SNAP_TARGET_ARCH=x86

echo Qnx Neutrino console compilation environment set up
