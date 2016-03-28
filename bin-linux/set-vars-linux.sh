#! /bin/sh

# LINUX VERSION
# Set the place where SciTech Software is installed, and where each
# of the supported compilers is installed. These environment variables
# are used by the batch files in the SCITECH\BIN directory.
#
# Modify the as appropriate for your compiler configuration (you should
# only need to change things in this batch file).
#
# This version is for a normal Linux installation.
#
# Note that it is safe to call this again if you change $SCITECH to
# point to somewhere else.

# The SCITECH variable points to where batch files, makefile startups,
# include files and source files will be found when compiling. If
# the MGL_ROOT variable is set, we set the SCITECH variable to point
# to the same location, unless SCITECH has already been set (in which
# case we presume it is set for a reason and don't override it).

if [ -z $SCITECH ]; then
    if [ "x$MGL_ROOT" != x ]; then
        export SCITECH=$MGL_ROOT
    fi
fi

# The SCITECH_LIB variable points to where the SciTech libraries live
# for installation and linking. This allows you to have the source and
# include files on local machines for compiling and have the libraries
# located on a common network machine (for network builds).

export SCITECH_LIB=$SCITECH

# The PRIVATE variable points to where private source files reside that
# do not live in the public source tree
if [ -z $PRIVATE ]; then export PRIVATE=$HOME/private ; fi

# Set the locations for non system compilers 
export OW10_PATH=/opt/watcom

# The following defines the TEMP directory
export TEMP=/tmp TMP=/tmp

# Save the default path so running this script again doesn't expand it
if [ -z $_ORG_PATH ]; then export _ORG_PATH=$PATH ; fi

# Change the path to include the scitech binaries depending on host platform
HOST_MACHINE=`(uname -m) 2>/dev/null` || HOST_MACHINE=unknown
if [ "$HOST_MACHINE" == ppc ]; then
   export SNAP_HOST_ARCH=ppc-be
   export PATH=$HOME/bin/ppc-be:$SCITECH/bin-linux:$SCITECH/bin-linux/ppc-be:$_ORG_PATH
elif [ "$HOST_MACHINE" == alpha ]; then
   export SNAP_HOST_ARCH=axp-be
   export PATH=$HOME/bin/alpha:$SCITECH/bin-linux:$SCITECH/bin-linux/alpha:$_ORG_PATH
elif [ "$HOST_MACHINE" == x86_64 ]; then
   export SNAP_HOST_ARCH=x86-64
   export PATH=$HOME/bin/x86-64:$HOME/bin/x86:$SCITECH/bin-linux:$SCITECH/bin-linux/x86:$_ORG_PATH
elif [ "$HOST_MACHINE" == mips ]; then
   export SNAP_HOST_ARCH=mips-le
   export PATH=$HOME/bin/mips-le:$HOME/bin/mips-le:$SCITECH/bin-linux:$SCITECH/bin-linux/mips-le:$_ORG_PATH
else
   export SNAP_HOST_ARCH=x86
   export PATH=$HOME/bin/x86:$SCITECH/bin-linux:$SCITECH/bin-linux/x86:$_ORG_PATH
fi

# Save the default path so we can switch between compilers on the fly
export DEFPATH=$PATH

# Save the LD_LIBRARY_PATH so we can override this later if we switch between debug and release builds.
if [ -z $LD_LIBRARY_DEFPATH ]; then export LD_LIBRARY_DEFPATH=$LD_LIBRARY_PATH ; fi

# give LD_LIBRARY_DEFPATH a length of at least one character
if [ "q$LD_LIBRARY_DEFPATH" == "q" ]; then export LD_LIBRARY_DEFPATH=":" ; fi

