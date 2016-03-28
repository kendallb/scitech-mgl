#! /usr/bin/perl -s
#
# PERL version of the makedep util for Unix systems
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
# This is a wrapper script for the gcc dependencies generation facilities
#
# Usage : makedep.pl -include="-Ifoo -Ibar" -sources=dir1;dir2;dir3 -makefile=dependencies_file.dep
#
# $Author: StephaneP$
#

use Shell;

# Substitute backslashes with slashes for Unix semantics
$sources =~ s/\\/\//mg;

$makefile = 'makefile.dep' unless $makefile;

@dirs = split(/;/, $sources);
@includes = split(/ /, $include);
@srcext = ("*.c", "*.cc", "*.cpp", "*.cxx", "*.asm");

$olddir = `pwd`;
chop($olddir);
foreach $i (@dirs) {
  &process_dir ($i);
  chdir ($olddir);
}

foreach $i (@dirs) {
  $include_sources .= " -I" . $i;
}

# Makes the command actually called

my($tmp) = "gcc -MM " .  join(" ", @includes) . $include_sources . " " . join(" ",@all_files) . "> $makefile";
system($tmp);

### End of program

# Parses a directory for C files

sub process_dir {
  my ($dir) = @_;

  chdir ($dir);
  foreach $ext (@srcext) {
    my @files = glob($ext);
    foreach $j (@files) {
      push @all_files, $dir . "/" . $j;
    }
  }
}
