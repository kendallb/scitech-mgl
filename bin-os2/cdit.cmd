/*  SciTech SNAP REXX script */

parse arg dir prog args

if (prog = '') then do
    say 'Usage:  cdit dir command args'
    exit 1
end

newdir = directory(dir);

if (newdir == '') then do
    say 'Directory 'dir' not found!'
    exit 1
end

'@k_rm -f *.lib *.a'
'@call ' prog args

if (rc \= 0) then exit rc
