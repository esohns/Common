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
#set non-stop on
#set target-async on
set unwindonsignal on

handle SIGHUP pass nostop print

#handle SIGINT pass nostop print
handle SIGQUIT pass nostop print
handle SIGILL pass nostop print
handle SIGABRT pass nostop print
handle SIGFPE pass nostop print
handle SIGKILL pass nostop print
handle SIGSEGV pass nostop print
handle SIGPIPE pass nostop print

handle SIGALRM pass nostop print
handle SIGTERM pass nostop print
handle SIGUSR1 pass nostop print
handle SIGUSR2 pass nostop print
handle SIGCHLD pass nostop print
handle SIGCONT pass nostop print
handle SIGSTOP pass nostop print
handle SIGTSTP pass nostop print
handle SIGTTIN pass nostop print
handle SIGTTOU pass nostop print

#-----------------------------------------

handle SIGBUS pass nostop noprint
handle SIGPOLL pass nostop noprint

handle SIGPROF pass nostop noprint
handle SIGSYS pass nostop noprint

#handle SIGTRAP pass nostop noprint
handle SIGURG pass nostop noprint
handle SIGVTALRM pass nostop noprint
handle SIGXCPU pass nostop noprint

handle SIGXFSZ pass nostop noprint

#-----------------------------------------

#handle SIGIOT pass nostop noprint
handle SIGEMT pass nostop noprint
#handle SIGSTKFLT pass nostop noprint
handle SIGIO pass nostop noprint
#handle SIGCLD pass nostop noprint
handle SIGPWR pass nostop noprint
handle SIGINFO pass nostop noprint
handle SIGLOST pass nostop noprint
handle SIGWINCH pass nostop noprint
#handle SIGUNUSED pass nostop noprint

handle SIG33 pass nostop noprint
handle SIG34 pass nostop noprint

# source paths / debug symbols
#directory /home/erik/libxml2-2.9.2+zdfsg1
directory /mnt/win_d/projects/ffmpeg
directory /mnt/win_d/projects/ffmpeg/libavcodec
#set debug-file-directory

# python
#set auto-load python-scripts on
add-auto-load-safe-path /home/erik/.gdb_debug
# load (python) visualizers
source /home/erik/.gdb_debug/libcommon_gdb_visualizers.py
#python execfile ("libcommon_gdb_visualizers.py")
#python
#exec(open("libcommon_gdb_visualizers.py").read())
#end

# options
show directories
show auto-load safe-path
info auto-load python-scripts
info pretty-printer

