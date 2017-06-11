#!/bin/sh
# author:      Erik Sohns <eriksohns@123mail.org>
# this script commits, upstreams and backs up all project changes
# *NOTE*: it is neither portable nor particularly stable !
# parameters:   N/A
# return value: - 0 success, 1 failure

# sanity checks
command -v dirname >/dev/null 2>&1 || { echo "dirname is not installed, aborting" >&2; exit 1; }
command -v realpath >/dev/null 2>&1 || { echo "realpath is not installed, aborting" >&2; exit 1; }

COMMIT_SCRIPT="commit_projects.sh"
BACKUP_SCRIPT="backup_projects.sh"

DEFAULT_SCRIPTS_DIRECTORY="$(dirname $(realpath -e $0))"
SCRIPTS_DIRECTORY="${DEFAULT_SCRIPTS_DIRECTORY}"
# sanity check(s)
[ ! -d ${SCRIPTS_DIRECTORY} ] && echo "ERROR: invalid scripts directory (was: \"${SCRIPTS_DIRECTORY}\"), aborting" && exit 1
#echo "DEBUG: scripts directory: \"${SCRIPTS_DIRECTORY}\""
COMMIT_SCRIPT_PATH="${SCRIPTS_DIRECTORY}/${COMMIT_SCRIPT}"
[ ! -x ${COMMIT_SCRIPT_PATH} ] && echo "ERROR: invalid commit script (was: \"${COMMIT_SCRIPT_PATH}\"), aborting" && exit 1
BACKUP_SCRIPT_PATH="${SCRIPTS_DIRECTORY}/${BACKUP_SCRIPT}"
[ ! -x ${BACKUP_SCRIPT_PATH} ] && echo "ERROR: invalid backup script (was: \"${BACKUP_SCRIPT_PATH}\"), aborting" && exit 1

$(${COMMIT_SCRIPT_PATH}) >/dev/null 2>&1
[ $? -ne 0 ] && echo "ERROR: failed to commit changes, aborting" && exit 1
echo "comitted changes...DONE"
$(${BACKUP_SCRIPT_PATH}) >/dev/null 2>&1
[ $? -ne 0 ] && echo "ERROR: failed to backup changes, aborting" && exit 1
echo "backed up repositories...DONE"

