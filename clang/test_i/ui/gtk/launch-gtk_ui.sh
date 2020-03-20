#!/bin/sh
bindir=$(pwd)
cd /mnt/win_d/projects/Common/clang/../test_i/ui/gtk
export LD_LIBRARY_PATH=/media/erik/USB_BLACK/lib/gtk2/bin:/media/erik/USB_BLACK/lib/ACE_TAO/ACE/lib:/mnt/win_d/projects/Common/cmake/src/Debug::$LD_LIBRARY_PATH

if test "x$1" = "x--debugger"; then
	shift
	if test "xYES" = "xYES"; then
		echo "r -g./etc/gtk_ui.gtk2  " > $bindir/gdbscript
		echo "bt" >> $bindir/gdbscript
		/usr/bin/gdb -batch -command=$bindir/gdbscript --return-child-result /mnt/win_d/projects/Common/clang/test_i/ui/gtk/gtk_ui 
	else
		"/mnt/win_d/projects/Common/clang/test_i/ui/gtk/gtk_ui" -g./etc/gtk_ui.gtk2  
	fi
else
	"/mnt/win_d/projects/Common/clang/test_i/ui/gtk/gtk_ui" -g./etc/gtk_ui.gtk2  
fi
