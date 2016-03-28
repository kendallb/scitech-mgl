/* SciTech SNAP REXX script */

parse arg args

'@echo off'
/* Disable checked build and build release code */
'set CHECKED='
'@call build_it 'args

if (rc \= 0) then exit rc
