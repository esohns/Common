#set debug auto-load on
set script-extension strict

# C
set print pretty on
set print static-members on
set print union on

# C++
set overload-resolution on
set print asm-demangle on
set print demangle on
set print object on
set print vtbl on
#set demangle-style gnu-v3

# breakpoints
#set breakpoint menus
set breakpoint auto-hw

# signals
handle SIGCHLD pass nostop print
handle SIGINT pass nostop print
handle SIGQUIT pass nostop print
handle SIGPIPE pass nostop print
handle SIGUSR1 pass nostop print
handle SIGUSR2 pass nostop print
handle SIG33 pass nostop noprint
handle SIG34 pass nostop noprint

# source paths / debug symbols
#directory /home/esohns/libxml2-2.9.2+zdfsg1
#set debug-file-directory

# python
#set auto-load python-scripts on
add-auto-load-safe-path /home/esohns/.gdb_debug
# load (python) visualizers
source /home/esohns/.gdb_debug/libcommon_gdb_visualizers.py
#python execfile ("libcommon_gdb_visualizers.py")
#python
#exec(open("libcommon_gdb_visualizers.py").read())
#end

# options
show directories
show auto-load safe-path
info auto-load python-scripts
info pretty-printer

