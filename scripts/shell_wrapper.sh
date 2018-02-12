#!/bin/sh
# author:      Erik Sohns <eriksohns@123mail.org>
# this script starts a shell invoking the given command. The process is
# SIGSTOPped immediately so gdb can attach and users can debug the command from
# the very beginning
# return value: - 0 success, 1 failure

# sanity checks
command -v echo >/dev/null 2>&1 || { echo "echo is not supported, aborting" >&2; exit 1; }
command -v exec >/dev/null 2>&1 || { echo "exec is not supported, aborting" >&2; exit 1; }
#command -v kill >/dev/null 2>&1 || { echo "kill is not supported, aborting" >&2; exit 1; }

command -v basename >/dev/null 2>&1 || { echo "basename is not installed, aborting" >&2; exit 1; }
#command -v dirname >/dev/null 2>&1 || { echo "dirname is not installed, aborting" >&2; exit 1; }
#command -v readlink >/dev/null 2>&1 || { echo "readlink is not installed, aborting" >&2; exit 1; }
#command -v sudo >/dev/null 2>&1 || { echo "sudo is not installed, aborting" >&2; exit 1; }

# parse arguments
COMMAND="$1"
if [ $# -lt 1 ]; then
 echo "USAGE: $(basename $0) command arguments"; exit 1
fi
shift
#echo "running \"$(basename ${COMMAND})\" with arguments: \"$@\" as ${USER}/${GROUP}"
echo "running \"$(basename ${COMMAND})\" with arguments: \"$@\" as ${USER}"

echo ${BASHPID}
kill -STOP ${BASHPID}
#exec sudo -u ${USER} -g ${GROUP} ${COMMAND} $@
#exec sudo -u ${USER} ${COMMAND} $@
exec ${COMMAND} $@

