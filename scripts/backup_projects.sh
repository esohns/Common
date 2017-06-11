#!/bin/sh
# author:      Erik Sohns <eriksohns@123mail.org>
# this script backs up all projects to a local drive
# *NOTE*: it is neither portable nor particularly stable !
# parameters:   N/A
# return value: - 0 success, 1 failure

# sanity checks
command -v dirname >/dev/null 2>&1 || { echo "dirname is not installed, aborting" >&2; exit 1; }
command -v git >/dev/null 2>&1 || { echo "git is not installed, aborting" >&2; exit 1; }
command -v realpath >/dev/null 2>&1 || { echo "realpath is not installed, aborting" >&2; exit 1; }

DEFAULT_PROJECTS_ROOT_DIRECTORY="$(dirname $(realpath -e $0))/../.."
PROJECTS_DIRECTORY=${DEFAULT_PROJECTS_ROOT_DIRECTORY}
# sanity check(s)
[ ! -d ${PROJECTS_DIRECTORY} ] && echo "ERROR: invalid project directory (was: \"${PROJECTS_DIRECTORY}\"), aborting" && exit 1
#echo "DEBUG: projects directory: \"${PROJECTS_DIRECTORY}\""
DEFAULT_TARGET_DIRECTORY="/mnt/mmc/backup/projects"
TARGET_DIRECTORY=${DEFAULT_TARGET_DIRECTORY}
[ ! -d ${TARGET_DIRECTORY} ] && echo "ERROR: invalid target directory (was: \"${TARGET_DIRECTORY}\"), aborting" && exit 1

DEFAULT_GIT_BRANCH="master"

SUB_DIRECTORIES="libCommon
libACEStream
libACENetwork"

BUNDLE_COMMAND="create"
TAG="lastBackupBundle"

cd $PROJECTS_DIRECTORY
[ $? -ne 0 ] && echo "ERROR: failed to cd to \"${PROJECTS_DIRECTORY}\": $?, aborting" && exit 1

for DIRECTORY in $SUB_DIRECTORIES
do
 CURRENT_DIRECTORY="${PROJECTS_DIRECTORY}/${DIRECTORY}"
 cd $CURRENT_DIRECTORY
 [ $? -ne 0 ] && echo "ERROR: failed to cd to \"${CURRENT_DIRECTORY}\": $?, aborting" && exit 1
 CURRENT_FILE="${TARGET_DIRECTORY}/${DIRECTORY}.bundle"
# if (-f ${CURRENT_FILE}); then
#  BUNDLE_COMMAND="update"
# fi

 git bundle $BUNDLE_COMMAND $CURRENT_FILE $DEFAULT_GIT_BRANCH >/dev/null 2>&1
 [ $? -ne 0 ] && echo "ERROR: failed to bundle repository \"${DIRECTORY}\", aborting" && exit 1
 echo "bundled repository ${DIRECTORY}...DONE"
 git tag -f $TAG $DEFAULT_GIT_BRANCH >/dev/null 2>&1
 [ $? -ne 0 ] && echo "ERROR: failed to tag repository \"${DIRECTORY}\", aborting" && exit 1
 echo "tagged repository ${DIRECTORY}...DONE"

 echo "\"$DIRECTORY\"...DONE"
done

