#!/bin/sh
# author:      Erik Sohns <eriksohns@123mail.org>
# this script sets up the OpenGL support for GTK version 2.x and 3.x
# respectively by activating the proper development branch of the gtkglarea git
# project. Note that the project itself still needs to be updated / recompiled
# manually
# *NOTE*: it is neither portable nor particularly stable !
# parameters:   - GTK version [2|3]
# return value: - 0 success, 1 failure

# sanity checks
command -v dirname >/dev/null 2>&1 || { echo "dirname is not installed, aborting" >&2; exit 1; }
command -v git >/dev/null 2>&1 || { echo "git is not installed, aborting" >&2; exit 1; }
command -v readlink >/dev/null 2>&1 || { echo "readlink is not installed, aborting" >&2; exit 1; }

DEFAULT_GTK_VERSION=3
GTK_VERSION=${DEFAULT_GTK_VERSION}
if [ $# -lt 1 ]; then
 echo "INFO: using default GTK version: \"${GTK_VERSION}\""
else
 # parse any arguments
 if [ $# -ge 1 ]; then
  GTK_VERSION=$1
 fi
fi
#echo "DEBUG: GTK version: \"${GTK_VERSION}\""

DEFAULT_PROJECT_DIRECTORY="$(dirname $(readlink -f $0))/../../gtkglarea"
PROJECT_DIRECTORY=${DEFAULT_PROJECT_DIRECTORY}
# sanity check(s)
[ ! -d ${PROJECT_DIRECTORY} ] && echo "ERROR: invalid project directory (was: \"${PROJECT_DIRECTORY}\"), aborting" && exit 1
#echo "DEBUG: project directory: \"${PROJECT_DIRECTORY}\""

DEFAULT_GIT_BRANCH="master"
if [ ${GTK_VERSION} -eq 3 ]; then
 GIT_BRANCH=${DEFAULT_GIT_BRANCH}
else
 GIT_BRANCH="gtkglarea-2"
fi

cd $PROJECT_DIRECTORY
[ $? -ne 0 ] && echo "ERROR: failed to cd to \"${PROJECT_DIRECTORY}\": $?, aborting" && exit 1
git checkout $GIT_BRANCH >/dev/null 2>&1
[ $? -ne 0 ] && echo "ERROR: failed to checkout branch \"${GIT_BRANCH}\", aborting" && exit 1
echo "checked out branch ${GIT_BRANCH}...DONE"

