#! /bin/sh
#
#  ========================================================================
#
#   Copyright (C) 1991-2002 SciTech Software, Inc. All rights reserved.
#
#   This file may be distributed and/or modified under the terms of the
#   GNU General Public License version 2 as published by the Free
#   Software Foundation and appearing in the file LICENSE.GPL included
#   in the packaging of this file.
#
#   Licensees holding a valid Commercial License for this product from
#   SciTech Software, Inc. may use this file in accordance with the
#   Commercial License Agreement provided with the Software.
#
#   This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING
#   THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR
#   PURPOSE.
#
#   See http://www.scitechsoft.com/license/ for information about
#   the licensing options available and how to purchase a Commercial
#   License Agreement.
#
#   Contact license@scitechsoft.com if any conditions of this licensing
#   are not clear to you, or you have questions about licensing options.
#
#  ========================================================================
#
# makeself 1.2
#
# Utilty to create self-extracting tar.gz archives.
# The resulting archive is a file holding the tar.gz archive with
# a small Shell script stub that uncompresses the archive to a temporary
# directory and then executes a given script from withing that directory.
#
# Makeself home page: http://www.lokigames.com/~megastep/makeself/
#
# Version history :
# - 1.0 : Initial public release
# - 1.1 : The archive can be passed parameters that will be passed on to
#         the embedded script, thanks to John C. Quillan
# - 1.2 : Package distribution, bzip2 compression, more command line options,
#         support for non-temporary archives. Ideas thanks to Francois Petitjean
#
# (C) 1998-1999 by Stéphane Peter <megastep@lokigames.com>
#
# This software is released under the terms of the GNU GPL
# Please read the license at http://www.gnu.org/copyleft/gpl.html
#
VERSION=1.2
GZIP="gzip -c"
KEEP=n
if [ "$1" = --version ]; then
    echo Makeself version $VERSION
    exit 0
fi
if [ "$1" = --bzip2 ]; then
    if which bzip2 2>&1 > /dev/null; then
        GZIP=bzip2
        shift 1
    else
        echo Unable to locate the bzip2 program in your \$PATH.>&2
        exit 1
    fi
fi
if [ "$1" = --notemp ]; then
    KEEP=y
    shift 1
fi
if [ $# -lt 4 ]; then
    echo $0: Not enough parameters.
    echo "Usage: $0 [params] archive_dir file_name label startup_script [args]"
    echo "params can be one of those :"
    echo "    --version  : Print out Makeself version number and exit"
    echo "    --bzip2    : Compress using bzip2 instead of gzip"
    echo "    --notemp   : The archive will uncompress to the current directory"
    echo Do not forget to give a fully qualified startup script name
    echo "(i.e. with a ./ prefix if inside the archive)."
    exit 1
fi

archdir=$1
archname=$2

# The following is the shell script stub code
echo '#! /bin/sh' > $archname
echo \# This script was generated using Makeself $VERSION >> $archname
echo label=\"$3\" >> $archname
echo script=$4 >> $archname
shift 4
echo scriptargs=\"$*\" >> $archname
echo "keep=$KEEP" >> $archname

cat << EOF >> $archname
skip=27
if [ "\$1" = "-keep" ]; then keep=y; shift 1; fi
if [ "\$keep" = y ]; then echo "Creating directory $archdir"; tmpdir=$archdir;
else tmpdir="/tmp/selfgz\$\$"; fi
location=\`pwd\`
echo=echo; [ -x /usr/ucb/echo ] && echo=/usr/ucb/echo
mkdir \$tmpdir || {
        echo 'Cannot create target directory' >&2
        exit 1
}
\$echo -n Uncompressing \$label
cd \$tmpdir
[ "\$keep" = y ] || trap 'cd /tmp; /bin/rm -rf \$tmpdir; exit \$res'
if ( (cd \$location; tail +\$skip \$0; ) | $GZIP -d | tar xvof - | \
 (while read a; do \$echo -n .; done; echo; )) 2> /dev/null; then
    \$script \$scriptargs \$*; res=\$?
    [ "\$keep" = y ] || ( cd /tmp; /bin/rm -rf \$tmpdir; )
else
  echo Cannot decompress \$0; exit 1
fi;
exit \$res
EOF

# Append the tar.gz data after the stub
echo Adding files to archive named \"$archname\"...
(cd $archdir; tar cvf - *| $GZIP -9 ) >> $archname && chmod +x $archname && \
echo Self-extractible archive \"$archname\" successfully created.
