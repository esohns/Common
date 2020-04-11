#!/bin/sh
bindir=$(pwd)
cd /mnt/win_d/projects/Common/clang/../../../ardrone/src
export LD_LIBRARY_PATH=/mnt/win_d/projects/Common/modules/ATCD/ACE/lib:/mnt/win_d/projects/Common/cmake/src/Debug::$LD_LIBRARY_PATH
export foo=bar

if test "x$1" = "x--debugger"; then
	shift
	if test "xYES" = "xYES"; then
		echo "r -f ../../../ardrone/cmake/src/ardrone -l -t " > $bindir/gdbscript
		echo "bt" >> $bindir/gdbscript
		/usr/bin/gdb -batch -command=$bindir/gdbscript --return-child-result /mnt/win_d/projects/Common/clang/test_u/debug/debug_wrapper 
	else
		"/mnt/win_d/projects/Common/clang/test_u/debug/debug_wrapper" -f ../../../ardrone/cmake/src/ardrone -l -t 
	fi
else
	"/mnt/win_d/projects/Common/clang/test_u/debug/debug_wrapper" -f ../../../ardrone/cmake/src/ardrone -l -t 
fi
