#! /bin/sh
#
# Setup for compiling with Open Watcom C/C++ for SNAP drivers (x86)

if [ "$CHECKED" = "1" ]; then
    echo Checked debug build enabled.
    export SCITECH_LIB_PATH=$SCITECH/lib/debug/drv/ow10
else
    echo Release build enabled.
    export SCITECH_LIB_PATH=$SCITECH/lib/release/drv/ow10
fi
. $SCITECH/bin-linux/clrvars.sh
export WATCOM=$OW10_PATH
export EDPATH=$WATCOM/eddat
export INCLUDE=include:$SCITECH/include/clib:$SCITECH/include:$PRIVATE/include
export USE_SNAP=1
export USE_SNAP_DRV=1
export USE_X86=1
export SNAP_TARGET_ARCH=x86
export WC_LIBBASE=ow10
export MAKESTARTUP=$SCITECH/makedefs/wc32.mk
export PATH=$OW10_PATH/binl:$DEFPATH
export USE_LIB_PATH=$SCITECH_LIB_PATH
export LD_LIBRARY_PATH=$SCITECH_LIB_PATH/so:$LD_LIBRARY_DEFPATH
echo "Open Watcom SNAP driver compilation environment set up (x86)"

