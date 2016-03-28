#! /bin/sh

# Setup for compiling with GCC/G++ for Linux (PPC)

. $SCITECH/bin-linux/clrvars.sh
export INCLUDE="-Iinclude -I$SCITECH/include -I$PRIVATE/include"
export USE_LINUX=1
export USE_PPC_BE=1
export UNIX_TARGET=1
export SNAP_TARGET_ARCH=ppc-be
export MAKESTARTUP=$SCITECH/makedefs/gcc_linux.mk
if [ "$CHECKED" = "1" ]; then
    echo Checked debug build enabled.
    export SCITECH_LIB_PATH=$SCITECH/lib/debug/linux/gcc/ppc-be
else
    echo Release build enabled.
    export SCITECH_LIB_PATH=$SCITECH/lib/release/linux/gcc/ppc-be
fi
export LD_LIBRARY_PATH=$SCITECH_LIB_PATH/so:$LD_LIBRARY_DEFPATH
echo "GCC Linux compilation environment set up (PowerPC)"

