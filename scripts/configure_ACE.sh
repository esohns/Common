#!/bin/sh
# author:      Erik Sohns <eriksohns@123mail.org>
# this script generates the ACE framework project files for Win32/Linux
# *NOTE*: it is neither portable nor particularly stable !
# parameters: - platform [win32|linux] {win32}
#             - mwc.pl '-type' parameter {vc14}
# return value: - 0 success, 1 failure

# sanity checks
command -v dirname >/dev/null 2>&1 || { echo "dirname is not installed, aborting" >&2; exit 1; }
command -v patch >/dev/null 2>&1 || { echo "patch is not installed, aborting" >&2; exit 1; }
command -v readlink >/dev/null 2>&1 || { echo "readlink is not installed, aborting" >&2; exit 1; }

DEFAULT_PLATFORM="win32"
PLATFORM=${DEFAULT_PLATFORM}
if [ $# -lt 1 ]
then
 echo "INFO: using default platform: \"${PLATFORM}\""
else
 # parse any arguments
 if [ $# -ge 1 ]
 then
  if [ "$1" != "win32" -a "$1" != "linux" ]
  then
   echo "invalid argument (was: "$1"), aborting"; exit 1;
  fi
  PLATFORM="$1"
 fi
fi
#echo "DEBUG: platform: \"${PLATFORM}\""

DEFAULT_PROJECT_TYPE="vc14"
PROJECT_TYPE=${DEFAULT_PROJECT_TYPE}
if [ $# -lt 2 ]
then
 case "${PLATFORM}" in
  win32)
   ;;
  linux|*)
   PROJECT_TYPE="gnuace"
   ;;
 esac
 echo "INFO: using default mpc project type: \"${PROJECT_TYPE}\""
else
 # parse any arguments
 if [ $# -ge 2 ]
 then
  PROJECT_TYPE="$2"
 fi
fi
#echo "DEBUG: project type: \"${PROJECT_TYPE}\""

DEFAULT_PROJECT_DIRECTORY="$(dirname $(readlink -f $0))/.."
PROJECT_DIRECTORY=${DEFAULT_PROJECT_DIRECTORY}
# sanity check(s)
[ ! -d ${PROJECT_DIRECTORY} ] && echo "ERROR: invalid project directory (was: \"${PROJECT_DIRECTORY}\"), aborting" && exit 1
#echo "DEBUG: project directory: \"${PROJECT_DIRECTORY}\""

DEFAULT_MPC_DIRECTORY=/mnt/win_d/projects/MPC # <-- UNIX
if [ ! -d ${DEFAULT_MPC_DIRECTORY} ]
then
  DEFAULT_MPC_DIRECTORY=/d/projects/MPC # <-- cygwin/mingw/msys
  [ ! -d ${DEFAULT_MPC_DIRECTORY} ] && echo "ERROR: invalid MPC directory (was: \"${DEFAULT_MPC_DIRECTORY}\"), aborting" && exit 1
fi
MPC_DIRECTORY=${DEFAULT_MPC_DIRECTORY}
if [ -z ${MPC_ROOT} ]
then
 MPC_ROOT=${MPC_DIRECTORY}
 export MPC_ROOT
 echo "INFO: exported MPC_ROOT (as: \"${MPC_ROOT}\")"
fi
#echo "INFO: \$MPC_ROOT is: \"${MPC_ROOT}\")"

DEFAULT_ACE_DIRECTORY=/mnt/win_d/projects/ATCD/ACE # <-- UNIX
if [ ! -d ${DEFAULT_ACE_DIRECTORY} ]
then
  DEFAULT_ACE_DIRECTORY=/d/projects/ATCD/ACE # <-- cygwin/mingw/msys
  [ ! -d ${DEFAULT_ACE_DIRECTORY} ] && echo "ERROR: invalid MPC directory (was: \"${DEFAULT_ACE_DIRECTORY}\"), aborting" && exit 1
fi
ACE_DIRECTORY=${DEFAULT_ACE_DIRECTORY}
if [ -z ${ACE_ROOT} ]
then
 ACE_ROOT=${ACE_DIRECTORY}
 export ACE_ROOT
 echo "INFO: exported ACE_ROOT (as: \"${ACE_ROOT}\")"
fi
#echo "INFO: \$ACE_ROOT is: \"${ACE_ROOT}\")"

# step1: apply patches
PATCH_DIRECTORY=${PROJECT_DIRECTORY}/../libACEStream/3rd_party/ACE_wrappers/patches
[ ! -d ${PATCH_DIRECTORY} ] && echo "ERROR: invalid ACE patch directory (was: \"${PATCH_DIRECTORY}\"), aborting" && exit 1
#echo "DEBUG: ACE patch directory: \"${PATCH_DIRECTORY}\""
cd ${ACE_DIRECTORY}
for filename in ${PATCH_DIRECTORY}/*.patch; do
 echo "INFO: applying patch \"$(basename ${filename})\"..."
 patch -f -i$filename -p4 -s -u
 [ $? -gt 1 ] && echo "ERROR: failed to apply patch \"${filename}\", aborting" && exit 1
done

# step2: generate Makefiles
DEFAULT_ACE_BUILD_DIRECTORY=${ACE_DIRECTORY} # <-- win32
case "${PLATFORM}" in
 win32)
  ;;
 linux|*)
  ACE_BUILD_DIRECTORY=${DEFAULT_ACE_DIRECTORY}/build/${PLATFORM}
  if [ ! -d ${ACE_BUILD_DIRECTORY} ]
  then
   echo "WARNING: invalid ACE build directory (was: \"${ACE_BUILD_DIRECTORY}\"), falling back"
   ACE_BUILD_DIRECTORY=${DEFAULT_ACE_DIRECTORY}
  fi
  ;;
esac
#echo "ACE_BUILD_DIRECTORY: ${ACE_BUILD_DIRECTORY}"
[ ! -d ${ACE_BUILD_DIRECTORY} ] && echo "ERROR: invalid ACE build directory (was: \"${ACE_BUILD_DIRECTORY}\"), aborting" && exit 1
#echo "DEBUG: ACE build directory: \"${ACE_BUILD_DIRECTORY}\""

FEATURES_FILE_DIRECTORY=${PROJECT_DIRECTORY}/3rd_party/ACE_wrappers
[ ! -d ${FEATURES_FILE_DIRECTORY} ] && echo "ERROR: invalid feature file directory (was: \"${FEATURES_FILE_DIRECTORY}\"), aborting" && exit 1
LOCAL_FEATURES_FILE=${FEATURES_FILE_DIRECTORY}/local.features
[ ! -r ${LOCAL_FEATURES_FILE} ] && echo "ERROR: invalid file (was: \"${LOCAL_FEATURES_FILE}\"), aborting" && exit 1
echo "INFO: feature file is: \"${LOCAL_FEATURES_FILE}\")"

ACE_MWC_FILE=${ACE_BUILD_DIRECTORY}/ACE.mwc
[ ! -r ${ACE_MWC_FILE} ] && echo "ERROR: invalid mpc configuration file (was: \"${ACE_MWC_FILE}\"), aborting" && exit 1

MWC_PL=${ACE_BUILD_DIRECTORY}/bin/mwc.pl
[ ! -x ${MWC_PL} ] && echo "ERROR: invalid mpc configuration script (was: \"${MWC_PL}\"), aborting" && exit 1
MWC_PL_OPTIONS=
#if [ "${PLATFORM}" = "linux" ]
#then
# MWC_PL_OPTIONS="-name_modifier *_gnu"
#fi
cd ${ACE_BUILD_DIRECTORY}
${MWC_PL} -feature_file ${LOCAL_FEATURES_FILE} ${MWC_PL_OPTIONS} -type ${PROJECT_TYPE} ${ACE_MWC_FILE}
[ $? -ne 0 ] && echo "ERROR: failed to mwc.pl \"${ACE_MWC_FILE}\", aborting" && exit 1
echo "processing ${ACE_MWC_FILE}...DONE"
