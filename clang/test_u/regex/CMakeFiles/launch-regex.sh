#!/bin/sh
bindir=$(pwd)
cd /mnt/win_d/projects/Common/clang/../test_u/regex
export LD_LIBRARY_PATH=/mnt/win_d/projects/Common/modules/ATCD/ACE/lib:/mnt/win_d/projects/Common/cmake/src/Debug::$LD_LIBRARY_PATH
export foo=bar

if test "x$1" = "x--debugger"; then
	shift
	if test "xYES" = "xYES"; then
		echo "r -l -t " > $bindir/gdbscript
		echo "bt" >> $bindir/gdbscript
		/usr/bin/gdb -batch -command=$bindir/gdbscript --return-child-result /mnt/win_d/projects/Common/clang/test_u/regex/regex 
	else
		"/mnt/win_d/projects/Common/clang/test_u/regex/regex" -l -t 
	fi
else
	"/mnt/win_d/projects/Common/clang/test_u/regex/regex" -l -t 
fi
