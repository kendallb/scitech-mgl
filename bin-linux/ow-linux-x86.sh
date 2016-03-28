#! /bin/sh
#
# Setup for compiling with Open Watcom C/C++ for Linux (x86)

if [ "$CHECKED" = "1" ]; then
    echo Checked debug build enabled.
    export SCITECH_LIB_PATH=$SCITECH/lib/debug/linux/ow10/x86
else
    echo Release build enabled.
    export SCITECH_LIB_PATH=$SCITECH/lib/release/linux/ow10/x86
fi
. $SCITECH/bin-linux/clrvars.sh
export WATCOM=$OW10_PATH
export EDPATH=$WATCOM/eddat
export INCLUDE=include:$SCITECH/include/clib:$SCITECH/include:$PRIVATE/include
export USE_LINUX=1
export USE_X86=1
export SNAP_TARGET_ARCH=x86
export UNIX_TARGET=1
export WC_LIBBASE=ow10
export MAKESTARTUP=$SCITECH/makedefs/wc32.mk
export PATH=$OW10_PATH/binl:$DEFPATH
export LD_LIBRARY_PATH=$SCITECH_LIB_PATH/so:$LD_LIBRARY_DEFPATH
echo "Open Watcom Linux compilation environment set up (x86)"

