#! /bin/sh

# Determine the host machine and call the proper script to set up 
# compiling natively for the host processor by default for new shells.
HOST_MACHINE=`(uname -m) 2>/dev/null` || HOST_MACHINE=unknown
if [ "$HOST_MACHINE" == ppc ]; then
    . $SCITECH/bin-linux/ow-linux-ppc-be.sh
elif [ "$HOST_MACHINE" == alpha ]; then
    . $SCITECH/bin-linux/ow-linux-axp-be.sh
elif [ "$HOST_MACHINE" == x86_64 ]; then
    . $SCITECH/bin-linux/ow-linux-x86-64.sh
else
    . $SCITECH/bin-linux/ow-linux-x86.sh
fi

