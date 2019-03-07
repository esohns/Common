#!/bin/sh
# author:      Erik Sohns <eriksohns@123mail.org>
# this script starts a shell invoking the given command. The process is
# SIGSTOPped immediately so gdb can attach and users can debug the command from
# the very beginning
# return value: - 0 success, 1 failure

# sanity checks
command -v basename >/dev/null 2>&1 || { echo "basename is not installed, aborting" >&2; exit 1; }
command -v capsh >/dev/null 2>&1 || { echo "capsh is not installed, aborting" >&2; exit 1; }
command -v cat >/dev/null 2>&1 || { echo "cat is not installed, aborting" >&2; exit 1; }
command -v cut >/dev/null 2>&1 || { echo "cut is not installed, aborting" >&2; exit 1; }
command -v echo >/dev/null 2>&1 || { echo "echo is not supported, aborting" >&2; exit 1; }
command -v exec >/dev/null 2>&1 || { echo "exec is not supported, aborting" >&2; exit 1; }
command -v kill >/dev/null 2>&1 || { echo "kill is not supported, aborting" >&2; exit 1; }

#for i in "$*"
#do
# echo "$i"
#done

CAPABILITIES_CODED=$(cat /proc/$$/status | grep CapInh | cut -f 2)
#echo "CAPABILITIES_CODED \"$CAPABILITIES_CODED\""
CAPABILITIES_DECODED=$(capsh --decode=$CAPABILITIES_CODED)
echo "$$ CapInh: \"$CAPABILITIES_DECODED\""
CAPABILITIES_CODED=$(cat /proc/$$/status | grep CapPrm | cut -f 2)
#echo "CAPABILITIES_CODED \"$CAPABILITIES_CODED\""
CAPABILITIES_DECODED=$(capsh --decode=$CAPABILITIES_CODED)
echo "$$ CapPrm: \"$CAPABILITIES_DECODED\""
CAPABILITIES_CODED=$(cat /proc/$$/status | grep CapEff | cut -f 2)
#echo "CAPABILITIES_CODED \"$CAPABILITIES_CODED\""
CAPABILITIES_DECODED=$(capsh --decode=$CAPABILITIES_CODED)
echo "$$ CapEff: \"$CAPABILITIES_DECODED\""
CAPABILITIES_CODED=$(cat /proc/$$/status | grep CapBnd | cut -f 2)
#echo "CAPABILITIES_CODED \"$CAPABILITIES_CODED\""
CAPABILITIES_DECODED=$(capsh --decode=$CAPABILITIES_CODED)
echo "$$ CapBnd: \"$CAPABILITIES_DECODED\""
CAPABILITIES_CODED=$(cat /proc/$$/status | grep CapAmb | cut -f 2)
#echo "CAPABILITIES_CODED \"$CAPABILITIES_CODED\""
CAPABILITIES_DECODED=$(capsh --decode=$CAPABILITIES_CODED)
echo "$$ CapAmb: \"$CAPABILITIES_DECODED\""

# parse arguments
COMMAND="$1"
if [ $# -lt 1 ]; then
 echo "USAGE: $(basename $0) command arguments"; exit 1
fi
shift
#echo "running \"$(basename ${COMMAND})\" with arguments: \"$@\" as ${USER}/${GROUP}"
echo "running \"$(basename ${COMMAND})\" with arguments: \"$@\" as ${USER}"

#echo ${BASHPID}
#kill -STOP ${BASHPID}
echo $$
kill -STOP $$
#kill -s 19 ${BASHPID}
#exec sudo -u ${USER} -g ${GROUP} ${COMMAND} $@
#exec sudo -u ${USER} ${COMMAND} $@
exec ${COMMAND} $@
