/* SciTech SNAP REXX script */

parse arg args

'@echo off'
/* Enable checked build and build debug code */
'set CHECKED=1'
'@call build_it 'args

if (rc \= 0) then exit rc
