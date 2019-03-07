#!/bin/sh
# author:      Erik Sohns <eriksohns@123mail.org>
# this script sets the required file capabilities
# return value: - 0 success, 1 failure

# sanity checks
command -v sudo >/dev/null 2>&1 || { echo "sudo is not installed, aborting" >&2; exit 1; }
#command -v gksudo >/dev/null 2>&1 || { echo "gksudo is not installed, aborting" >&2; exit 1; }
command -v exec >/dev/null 2>&1 || { echo "exec is not installed, aborting" >&2; exit 1; }

# *NOTE*: regular users may not have the CAP_SETFCAP capability needed to modify
#         (executable) file capabilities --> run as root
# *TODO*: verify this programmatically
HAS_GKSUDO=0
if [ -x gksudo ]; then
 HAS_GKSUDO=1
fi

if [ "${USER}" != "root" ]; then
 SUDO=sudo
 CMDLINE_ARGS="$@"
 if [ ${HAS_GKSUDO} -eq 1 ]; then
  SUDO=gksudo
  CMDLINE_ARGS="--disable-grab $0 $@"
 fi
# echo "invoking sudo $0 \"${CMDLINE_ARGS}\"..."
 exec ${SUDO} $0 ${CMDLINE_ARGS}
fi
#echo "starting..."

# sanity checks
#command -v chgrp >/dev/null 2>&1 || { echo "chgrp is not installed, aborting" >&2; exit 1; }
#command -v chmod >/dev/null 2>&1 || { echo "chmod is not installed, aborting" >&2; exit 1; }
#command -v chown >/dev/null 2>&1 || { echo "chown is not installed, aborting" >&2; exit 1; }
command -v dirname >/dev/null 2>&1 || { echo "dirname is not installed, aborting" >&2; exit 1; }
command -v echo >/dev/null 2>&1 || { echo "echo is not supported, aborting" >&2; exit 1; }
command -v readlink >/dev/null 2>&1 || { echo "readlink is not installed, aborting" >&2; exit 1; }
command -v tr >/dev/null 2>&1 || { echo "tr is not installed, aborting" >&2; exit 1; }
command -v setcap >/dev/null 2>&1 || { echo "setcap is not installed, aborting" >&2; exit 1; }
command -v getcap >/dev/null 2>&1 || { echo "getcap is not installed, aborting" >&2; exit 1; }

DEFAULT_PROJECT_DIR="$(dirname $(readlink -f $0))/.."
PROJECT_DIR=${DEFAULT_PROJECT_DIR}
# sanity check(s)
[ ! -d ${PROJECT_DIR} ] && echo "ERROR: invalid project dir (was: \"${PROJECT_DIR}\"), aborting" && exit 1

#DEFAULT_BUILD="Debug"
#BUILD=${DEFAULT_BUILD}
#if [ $# -lt 1 ]; then
# echo "INFO: using default build: \"${BUILD}\""
#else
 # parse any arguments
# if [ $# -ge 1 ]; then
#  BUILD="$1"
# fi
#fi
#DEFAULT_TMP_DIR="${TEMP}"
#TMP_DIR="$DEFAULT_TMP_DIR"
#if [ -z "${TMP_DIR}" ]; then
# TMP_DIR="/tmp"
#fi
#[ ! -d ${TMP_DIR} ] && echo "ERROR: invalid tmp dir (was: \"${TMP_DIR}\"), aborting" && exit 1
#echo "using tmp directory \"$TMP_DIR\"..."

# sanity check(s)
#[ ${BUILD} != "Debug" -a ${BUILD} != "debug_tracing" -a ${BUILD} != "release" ] && echo "WARNING: invalid/unknown build (was: \"${BUILD}\"), continuing"
BUILD_DIR="${PROJECT_DIR}/cmake"
[ ! -d "${BUILD_DIR}" ] && echo "ERROR: invalid build dir (was: \"${BUILD_DIR}\"), aborting" && exit 1

TEST_I_DIR="test_i"
CAPABILITIES_DIR="capabilities"
#SUB_DIRS="ardrone"
#declare -a LIBS=("libACE.so")
BINS="capability_wrapper"
i=0
#for DIR in $SUB_DIRS
#do
# LIB="${MODULES_DIR}/${DIR}/${LIB_DIR}/${LIBS[$i]}"
 BIN="${BUILD_DIR}/${TEST_I_DIR}/${CAPABILITIES_DIR}/${BINS}"
 [ ! -r "${BIN}" ] && echo "ERROR: invalid binary file (was: \"${BIN}\"), aborting" && exit 1

# cp -f ${BIN} ${TMP_DIR}
# [ $? -ne 0 ] && echo "ERROR: failed to cp ${BIN}: \"$?\", aborting" && exit 1
# echo "copied \"$BIN\"..."

# BIN_TMP="${TMP_DIR}/${BINS}"
# [ ! -r "${BIN_TMP}" ] && echo "ERROR: invalid binary file (was: \"${BIN_TMP}\"), aborting" && exit 1

# chown --quiet root ${BIN_TMP}
 chown --quiet root:root ${BIN}
 [ $? -ne 0 ] && echo "ERROR: failed to chown ${BIN}: \"$?\", aborting" && exit 1
# chgrp --quiet root ${BIN}
# [ $? -ne 0 ] && echo "ERROR: failed to chgrp ${BIN}: \"$?\", aborting" && exit 1
# chmod --quiet +s ${BIN_TMP}
 chmod --quiet ug+s ${BIN}
 [ $? -ne 0 ] && echo "ERROR: failed to chmod u+s ${BIN}: \"$?\", aborting" && exit 1
 echo "modified \"$BINS\": suid sgid root"

# /sbin/setcap 'cap_net_bind_service=eip' ${BIN_TMP}
# setcap 'cap_net_admin+eip' ${BIN}
# setcap 'cap_net_admin+eip' ${BIN}
# [ $? -ne 0 ] && echo "ERROR: failed to setcap ${BIN}: \"$?\", aborting" && exit 1

# CMD_OUTPUT=$(getcap ${BIN})
# j=0
# for k in $(echo $CMD_OUTPUT | tr " " "\n")
# do
##  echo "$j: \"$k\""
#  if [ $j -eq 2 ]
#  then
#   CAPABILITIES=$k
#  fi
#  j=$(($j+1))
# done
# echo "modified \"$BINS\": ${CAPABILITIES}"
## i=$(($i+1))
##done

