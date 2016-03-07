#!/bin/bash
# author:      Erik Sohns <eriksohns@123mail.org>
# this script initializes gdb and pretty printers for the project
# *NOTE*: it is neither portable nor particularly stable !
# parameters:   - (UNIX) platform [linux|solaris]
# return value: - 0 success, 1 failure

# sanity checks
command -v basename >/dev/null 2>&1 || { echo "basename is not installed, aborting" >&2; exit 1; }
command -v cp >/dev/null 2>&1 || { echo "cp is not installed, aborting" >&2; exit 1; }
command -v dirname >/dev/null 2>&1 || { echo "dirname is not installed, aborting" >&2; exit 1; }
command -v mkdir >/dev/null 2>&1 || { echo "mkdir is not installed, aborting" >&2; exit 1; }
command -v readlink >/dev/null 2>&1 || { echo "readlink is not installed, aborting" >&2; exit 1; }
command -v shopt >/dev/null 2>&1 || { echo "shopt is not installed, aborting" >&2; exit 1; }

#DEFAULT_PLATFORM="linux"
#PLATFORM=${DEFAULT_PLATFORM}
#if [ $# -lt 1 ]; then
# echo "INFO: using default platform: \"${PLATFORM}\""
#else
# # parse any arguments
# if [ $# -ge 1 ]; then
#  PLATFORM="$1"
# fi
#fi
#echo "DEBUG: platform: \"${PLATFORM}\""

DEFAULT_PROJECTS_DIRECTORY="$(dirname $(readlink -f $0))/../.."
PROJECTS_DIRECTORY=${DEFAULT_PROJECTS_DIRECTORY}
# sanity check(s)
[ ! -d ${PROJECTS_DIRECTORY} ] && echo "ERROR: invalid projects directory (was: \"${PROJECTS_DIRECTORY}\"), aborting" && exit 1
#echo "DEBUG: project directory: \"${PROJECT_DIRECTORY}\""
echo "set projects directory: \"${PROJECTS_DIRECTORY}\""

PROJECT_PATH="${PROJECTS_DIRECTORY}/libCommon"
SCRIPTS_PATH="${PROJECT_PATH}/scripts"
[ ! -d ${SCRIPTS_PATH} ] && echo "ERROR: invalid scripts directory (was: \"${SCRIPTS_PATH}\"), aborting" && exit 1
GDBINIT="${SCRIPTS_PATH}/.gdbinit"
[ ! -r ${GDBINIT} ] && echo "ERROR: invalid .gdbinit (was: \"${GDBINIT}\"), aborting" && exit 1

cp -f ${GDBINIT} ${HOME} >/dev/null 2>&1
[ $? -ne 0 ] && echo "ERROR: failed to cp \"${GDBINIT}\" to \"${HOME}\": $?, aborting" && exit 1 
echo "copied \"$(basename ${GDBINIT})\"..."

GDBSCRIPTS_DIRECTORY="${HOME}/.gdb_debug"
if [ ! -d ${GDBSCRIPTS_DIRECTORY} ]; then
 mkdir ${GDBSCRIPTS_DIRECTORY} >/dev/null 2>&1
 [ $? -ne 0 ] && echo "ERROR: failed to mkdir \"${GDBSCRIPTS_DIRECTORY}\": $?, aborting" && exit 1 
 [ ! -d ${GDBSCRIPTS_DIRECTORY} ] && echo "ERROR: failed to mkdir \"${GDBSCRIPTS_DIRECTORY}\", aborting" && exit 1 
 echo "created ${GDBSCRIPTS_DIRECTORY}..."
fi

shopt -s nullglob
PROJECTS="libCommon
libACEStream
libACENetwork"
for project in ${PROJECTS}
do
 PROJECT_PATH="${PROJECTS_DIRECTORY}/${project}"
 [ ! -d ${PROJECT_PATH} ] && echo "ERROR: invalid project directory (was: \"${PROJECT_PATH}\"), aborting" && exit 1
 SCRIPTS_PATH="${PROJECT_PATH}/scripts"
 [ ! -d ${SCRIPTS_PATH} ] && echo "ERROR: invalid scripts directory (was: \"${SCRIPTS_PATH}\"), aborting" && exit 1

 for file in ${SCRIPTS_PATH}/*.py
 do
  cp -f ${file} ${GDBSCRIPTS_DIRECTORY} >/dev/null 2>&1
  [ $? -ne 0 ] && echo "ERROR: failed to cp \"${file}\": $?, aborting" && exit 1 
  echo "copied \"$(basename ${file})\"..."
 done

 echo "processing ${project}...DONE"
done

