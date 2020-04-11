#!/bin/sh
bindir=$(pwd)
cd /mnt/win_d/projects/Common/clang/../test_i/parser
export LD_LIBRARY_PATH=/media/erik/USB_BLACK/lib/ACE_TAO/ACE/lib:/mnt/win_d/projects/Common/cmake/src/Debug::$LD_LIBRARY_PATH
export foo=bar

if test "x$1" = "x--debugger"; then
	shift
	if test "xYES" = "xYES"; then
		echo "r -d -e -f test.txt -l -t " > $bindir/gdbscript
		echo "bt" >> $bindir/gdbscript
		/usr/bin/gdb -batch -command=$bindir/gdbscript --return-child-result /mnt/win_d/projects/Common/clang/test_i/parser/parser 
	else
		"/mnt/win_d/projects/Common/clang/test_i/parser/parser" -d -e -f test.txt -l -t 
	fi
else
	"/mnt/win_d/projects/Common/clang/test_i/parser/parser" -d -e -f test.txt -l -t 
fi
