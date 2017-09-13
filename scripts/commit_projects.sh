#!/bin/sh
# author:      Erik Sohns <eriksohns@123mail.org>
# this script commits all pending changes and uploads them to the github servers
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

DEFAULT_GIT_BRANCH="master"

cd $PROJECTS_DIRECTORY
[ $? -ne 0 ] && echo "ERROR: failed to cd to \"${PROJECTS_DIRECTORY}\": $?, aborting" && exit 1

SUB_DIRECTORIES="libCommon
libACEStream
libACENetwork"
for DIRECTORY in $SUB_DIRECTORIES
do
 CURRENT_DIRECTORY="${PROJECTS_DIRECTORY}/${DIRECTORY}"
 cd $CURRENT_DIRECTORY
 [ $? -ne 0 ] && echo "ERROR: failed to cd to \"${CURRENT_DIRECTORY}\": $?, aborting" && exit 1

 git add --all >/dev/null 2>&1
 [ $? -ne 0 ] && echo "ERROR: failed to stage changes to \"${DIRECTORY}\": $?, aborting" && exit 1
 echo "staged changes to ${DIRECTORY}...DONE"
 git commit -am "minor changes" >/dev/null 2>&1
 [ $? -ne 0 ] && echo "ERROR: failed to commit changes to \"${DIRECTORY}\": $?, continuing" && continue
 echo "committed changes to ${DIRECTORY}...DONE"
 git push origin $DEFAULT_GIT_BRANCH >/dev/null 2>&1
 [ $? -ne 0 ] && echo "ERROR: failed to upstream changes to \"${DIRECTORY}\": $?, aborting" && exit 1
 echo "upstreamed changes to ${DIRECTORY}...DONE"

 echo "\"$DIRECTORY\"...DONE"
done

