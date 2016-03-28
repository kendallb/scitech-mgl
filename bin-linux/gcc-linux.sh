#! /bin/sh

# If the SNAP_TARGET_ARCH variable is set, it means we have already called
# one of these scripts before so we should use the same architecture
# for subsqeuent invocation. To change architectures, call one of the
# architecture specific scripts directly.

if [ "$SNAP_TARGET_ARCH" == ppc-be ]; then
    . $SCITECH/bin-linux/gcc-linux-ppc-be.sh
elif [ "$SNAP_TARGET_ARCH" == axp-be ]; then
    . $SCITECH/bin-linux/gcc-linux-axp-be.sh
elif [ "$SNAP_TARGET_ARCH" == x86-64 ]; then
    . $SCITECH/bin-linux/gcc-linux-x86-64.sh
elif [ "$SNAP_TARGET_ARCH" == mips-le ]; then
    . $SCITECH/bin-linux/gcc-linux-mips-le.sh
elif [ "$SNAP_TARGET_ARCH" == x86 ]; then
    . $SCITECH/bin-linux/gcc-linux-x86.sh
else
    # Determine the host machine and call the proper script to set up 
    # compiling natively for the host processor by default for new shells.
    
    HOST_MACHINE=`(uname -m) 2>/dev/null` || HOST_MACHINE=unknown
    if [ "$HOST_MACHINE" == ppc ]; then
	. $SCITECH/bin-linux/gcc-linux-ppc-be.sh
    elif [ "$HOST_MACHINE" == alpha ]; then
	. $SCITECH/bin-linux/gcc-linux-axp-be.sh
    elif [ "$HOST_MACHINE" == x86_64 ]; then
	. $SCITECH/bin-linux/gcc-linux-x86-64.sh
    elif [ "$HOST_MACHINE" == mips ]; then
	. $SCITECH/bin-linux/gcc-linux-mips-le.sh
    else
	. $SCITECH/bin-linux/gcc-linux-x86.sh
    fi
fi

