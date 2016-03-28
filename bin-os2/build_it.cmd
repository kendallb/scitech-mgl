/* SciTech SNAP REXX script                                                *
 *                                                                         *
 * Generic batch file to build a version of the library. This batch file   *
 * assumes that the correct batch files exist to setup the appropriate     *
 * compilation environments, and that the DMAKE.EXE program is available   *
 * somewhere on the path.                                                  *
 *                                                                         *
 * Builds as release or debug depending on the value of the CHECKED        *
 * environment variable.                                                   */

parse arg compiler args

'@echo off'

/* Unset all environment variables that change the compile process */
'set DBG='
'set OPT='
'set OPT_SIZE='
'set BUILD_DLL='
'set IMPORT_DLL='
'set FPU='
'set CHECKS='
'set BETA='
            
if (compiler \= '')
then 'call '|| compiler || '.cmd'

if (rc \= 0 | compiler = '') then do
	say "Usage: BUILD 'compiler_name' [DMAKE commands]"
	say
	say "Where 'compiler_name' is of the form comp-os, where"
	say "'comp' defines the compiler and 'os' defines the OS environment."
	say "For instance 'wc10-o32' is for Watcom C++ 10.6 for OS/2 Console."
	say "The value of 'comp' can be any of the following:"
	say
	say "   bc20 - Borland C++ 2.0"
	say "   va30 - IBM VisualAge C++ 3.0 32-bit"
	say "   wc10 - Watcom C++ 10.6"
	say "   wc11 - Watcom C++ 11.0"
	say "   ow10 - Open Watcom C++ 1.x"
	say "   emx  - emx 0.9d"
	say
	say "The value of 'os' can be one of the following:"
	say
	say "   d16  - 16-bit DOS"
	say "   d32  - 32-bit DOS"
	say "   w16  - 16-bit Windows GUI mode"
	say "   c32  - 32-bit Windows console mode"
	say "   w32  - 32-bit Windows GUI mode"
	say "   o16  - 16-bit OS/2 console mode"
	say "   o32  - 32-bit OS/2 console mode"
	say "   p32  - 32-bit OS/2 Presentation Manager"
	say "   snp  - 32-bit SciTech Snap application"
	say "   drv  - 32-bit SciTech SNAP driver"
	exit 1
end

'k_rm -f *.lib *.a'
'dmake 'args

if (rc \= 0) then do
	say "*************************************************"
	say "* An error occurred while building the library. *"
	say "*************************************************"
	exit rc
end

