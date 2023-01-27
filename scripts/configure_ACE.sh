#!/bin/sh
# author:      Erik Sohns <eriksohns@123mail.org>
# this script compiles the ACE framework on UNIX platforms
# *NOTE*: it is neither portable nor particularly stable !
# parameters: - platform [linux|solaris|win32] {linux}
#             - mwc.pl '-type' parameter {gnuace}
#             - -b generate platform-specific build tree {false}
# return value: - 0 success, 1 failure

# sanity checks
command -v dirname >/dev/null 2>&1 || { echo "dirname is not installed, aborting" >&2; exit 1; }
command -v make >/dev/null 2>&1 || { echo "make is not installed, aborting" >&2; exit 1; }
command -v mkdir >/dev/null 2>&1 || { echo "mkdir is not installed, aborting" >&2; exit 1; }
command -v patch >/dev/null 2>&1 || { echo "patch is not installed, aborting" >&2; exit 1; }
command -v perl >/dev/null 2>&1 || { echo "perl is not installed, aborting" >&2; exit 1; }
command -v readlink >/dev/null 2>&1 || { echo "readlink is not installed, aborting" >&2; exit 1; }

DEFAULT_PLATFORM="linux"
PLATFORM=${DEFAULT_PLATFORM}
if [ $# -lt 1 ]
then
 echo "INFO: using default platform: \"${PLATFORM}\""
else
 # parse any arguments
 if [ $# -ge 1 ]
 then
  if [ "$1" != "linux" -a "$1" != "macos" -a "$1" != "solaris" -a "$1" != "windows" ]
  then
   echo "invalid argument (was: "$1"), aborting"; exit 1;
  fi
  PLATFORM="$1"
 fi
fi
#echo "DEBUG: platform: \"${PLATFORM}\""

DEFAULT_PROJECT_TYPE="gnuace"
PROJECT_TYPE=${DEFAULT_PROJECT_TYPE}
if [ $# -lt 2 ]
then
 case "${PLATFORM}" in
  linux|solaris)
#   PROJECT_TYPE="gnuace"
   ;;
  win32)
   PROJECT_TYPE="vc17"
   ;;
  *)
   echo "invalid platform (was: "${PLATFORM}"), aborting"; exit 1;
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

GENERATE_BUILD_TREE=0
if [ $# -lt 3 ]
then
 echo "INFO: building in \"${ACE_ROOT}\"..."
else
# parse any arguments
if [ $# -ge 3 ]
then
 if [ "$3" != "-b" ]
 then
  echo "invalid argument (was: "$3"), aborting"; exit 1;
 fi
 GENERATE_BUILD_TREE=1
 echo "INFO: generating platform-specific build tree..."
fi
fi

DEFAULT_PROJECT_DIRECTORY="$(dirname $(readlink -f $0))/../.."
PROJECT_DIRECTORY=${DEFAULT_PROJECT_DIRECTORY}
# sanity check(s)
[ ! -d ${PROJECT_DIRECTORY} ] && echo "ERROR: invalid directory (was: \"${PROJECT_DIRECTORY}\"), aborting" && exit 1
#echo "DEBUG: project directory: \"${PROJECT_DIRECTORY}\""

DEFAULT_MPC_DIRECTORY=${LIB_ROOT}/MPC # <-- UNIX
if [ ! -d ${DEFAULT_MPC_DIRECTORY} ]
then
 if [ ! -z ${MPC_ROOT} ]
 then
  DEFAULT_MPC_DIRECTORY=${MPC_ROOT} # <-- cygwin/mingw/msys
 else
  DEFAULT_MPC_DIRECTORY=/d/projects/MPC # <-- mingw/msys
 fi
 [ ! -d ${DEFAULT_MPC_DIRECTORY} ] && echo "ERROR: invalid directory (was: \"${DEFAULT_MPC_DIRECTORY}\"), aborting" && exit 1
fi
MPC_DIRECTORY=${DEFAULT_MPC_DIRECTORY}
if [ -z ${MPC_ROOT} ]
then
 export MPC_ROOT=${MPC_DIRECTORY}
 [ $? -ne 0 ] && echo "ERROR: failed to export MPC_ROOT environment variable (was: \"${MPC_DIRECTORY}\"), aborting" && exit 1
 echo "INFO: exported MPC_ROOT (as: \"${MPC_ROOT}\")"
fi
#echo "INFO: \$MPC_ROOT is: \"${MPC_ROOT}\")"

DEFAULT_ACE_DIRECTORY=${LIB_ROOT}/ACE_TAO/ACE # <-- UNIX
if [ ! -d ${DEFAULT_ACE_DIRECTORY} ]
then
 if [ ! -z ${ACE_ROOT} ]
 then
  DEFAULT_ACE_DIRECTORY=${ACE_ROOT} # <-- cygwin/mingw/msys
 else
  DEFAULT_ACE_DIRECTORY=/d/projects/ACE_TAO/ACE # <-- mingw/msys
 fi
 [ ! -d ${DEFAULT_ACE_DIRECTORY} ] && echo "ERROR: invalid directory (was: \"${DEFAULT_ACE_DIRECTORY}\"), aborting" && exit 1
fi
ACE_DIRECTORY=${DEFAULT_ACE_DIRECTORY}
if [ -z ${ACE_ROOT} ]
then
 export ACE_ROOT=${ACE_DIRECTORY}
 [ $? -ne 0 ] && echo "ERROR: failed to export ACE_ROOT environment variable (was: \"${ACE_DIRECTORY}\"), aborting" && exit 1
 echo "INFO: exported ACE_ROOT (as: \"${ACE_ROOT}\")"
fi
#echo "INFO: \$ACE_ROOT is: \"${ACE_ROOT}\")"

DEFAULT_OPENSSL_DIRECTORY=${LIB_ROOT}/openssl
if [ ! -d ${DEFAULT_OPENSSL_DIRECTORY} ]
then
 if [ ! -z ${SSL_ROOT} ]
 then
  DEFAULT_OPENSSL_DIRECTORY=${SSL_ROOT} # <-- cygwin/mingw/msys
 else
  DEFAULT_OPENSSL_DIRECTORY=/d/projects/openssl # <-- mingw/msys
 fi
 [ ! -d ${DEFAULT_OPENSSL_DIRECTORY} ] && echo "ERROR: invalid directory (was: \"${DEFAULT_OPENSSL_DIRECTORY}\"), aborting" && exit 1
fi
OPENSSL_DIRECTORY=${DEFAULT_OPENSSL_DIRECTORY}
if [ -z ${SSL_ROOT} ]
then
 export SSL_ROOT=${OPENSSL_DIRECTORY}
 [ $? -ne 0 ] && echo "ERROR: failed to export SSL_ROOT environment variable (was: \"${OPENSSL_DIRECTORY}\"), aborting" && exit 1
 echo "INFO: exported SSL_ROOT (as: \"${SSL_ROOT}\")"
fi
#echo "INFO: \$SSL_ROOT is: \"${SSL_ROOT}\")"

# step1: apply patches
PROJECTS="Common
ACEStream
ACENetwork"
for PROJECT in $PROJECTS
do
 echo "INFO: processing project \"${PROJECT}\"..."
 PATCH_DIRECTORY=${PROJECT_DIRECTORY}/${PROJECT}/3rd_party/ACE_wrappers/patches
 [ ! -d ${PATCH_DIRECTORY} ] && echo "ERROR: invalid directory (was: \"${PATCH_DIRECTORY}\"), aborting" && exit 1
 cd ${ACE_DIRECTORY}
 for filename in ${PATCH_DIRECTORY}/*.patch; do
  echo "INFO: applying patch \"$(basename ${filename})\"..."
  patch -f -i$filename -p4 -s -u
  [ $? -gt 1 ] && echo "ERROR: failed to apply patch \"${filename}\", aborting" && exit 1
 done
 echo "INFO: processing project \"${PROJECT}\"...DONE"
done

# step2: verify build directories
ACE_BUILD_ROOT_DIRECTORY=${ACE_DIRECTORY}/build
if [ ! -z ${GENERATE_BUILD_TREE} -a ! -d ${ACE_BUILD_ROOT_DIRECTORY} ]
then
 echo "DEBUG: ACE build root directory (was: \"${ACE_BUILD_ROOT_DIRECTORY}\") does not exist, creating"
 mkdir ${ACE_BUILD_ROOT_DIRECTORY} >/dev/null 2>&1
 [ $? -ne 0 ] && echo "ERROR: failed to create directory (was: \"${ACE_BUILD_ROOT_DIRECTORY}\"): $?, aborting" && exit 1
 case "${PLATFORM}" in
  linux|solaris)
   ACE_BUILD_DIRECTORY=${ACE_BUILD_ROOT_DIRECTORY}/${PLATFORM}
   if [ ! -d ${ACE_BUILD_DIRECTORY} ]
   then
    echo "DEBUG: ACE build directory (was: \"${ACE_BUILD_DIRECTORY}\") does not exist, creating and cloning source tree"
    mkdir ${ACE_BUILD_DIRECTORY} >/dev/null 2>&1
    [ $? -ne 0 ] && echo "ERROR: failed to create directory (was: \"${ACE_BUILD_DIRECTORY}\"): $?, aborting" && exit 1
    CREATE_ACE_BUILD=${ACE_DIRECTORY}/bin/create_ace_build.pl
    [ ! -x ${CREATE_ACE_BUILD} ] && echo "ERROR: invalid file (was: \"${CREATE_ACE_BUILD}\"): not executable, aborting" && exit 1
    cd ${ACE_DIRECTORY}
    # *TODO*: platform-specific build trees do not work ATM
#    perl ${CREATE_ACE_BUILD} -a -v ${PLATFORM}
#    [ $? -ne 0 ] && echo "ERROR: failed to \"${CREATE_ACE_BUILD}\": $?, aborting" && exit 1
    ACE_BUILD_DIRECTORY=${ACE_DIRECTORY}
   fi
   ;;
  win32)
   ACE_BUILD_DIRECTORY=${ACE_DIRECTORY}
   ;;
  *)
   echo "unknown/invalid platform (was: "${PLATFORM}"), falling back"
   ACE_BUILD_DIRECTORY=${ACE_DIRECTORY}
   ;;
 esac
else
 ACE_BUILD_DIRECTORY=${ACE_DIRECTORY}
fi
#echo "ACE_BUILD_DIRECTORY: ${ACE_BUILD_DIRECTORY}"
[ ! -d ${ACE_BUILD_DIRECTORY} ] && echo "ERROR: invalid directory (was: \"${ACE_BUILD_DIRECTORY}\"), aborting" && exit 1
#echo "DEBUG: ACE build directory: \"${ACE_BUILD_DIRECTORY}\""

# step3: generate Makefiles
if [ ! -f ${ACE_BUILD_DIRECTORY}/GNUmakefile ]
then
 FEATURES_FILE_DIRECTORY=${PROJECT_DIRECTORY}/Common/3rd_party/ACE_wrappers
 [ ! -d ${FEATURES_FILE_DIRECTORY} ] && echo "ERROR: invalid directory (was: \"${FEATURES_FILE_DIRECTORY}\"), aborting" && exit 1
 LOCAL_FEATURES_FILE=${FEATURES_FILE_DIRECTORY}/local.features
 [ ! -r ${LOCAL_FEATURES_FILE} ] && echo "ERROR: invalid file (was: \"${LOCAL_FEATURES_FILE}\"): not readable, aborting" && exit 1
 echo "INFO: feature file is: \"${LOCAL_FEATURES_FILE}\""

 ACE_MWC_FILE=${ACE_BUILD_DIRECTORY}/ACE.mwc
 [ ! -r ${ACE_MWC_FILE} ] && echo "ERROR: invalid file (was: \"${ACE_MWC_FILE}\"): not readable, aborting" && exit 1

 MWC_PL=${ACE_DIRECTORY}/bin/mwc.pl
 [ ! -x ${MWC_PL} ] && echo "ERROR: invalid file (was: \"${MWC_PL}\"): not executable, aborting" && exit 1
 MWC_PL_OPTIONS=
 #if [ "${PLATFORM}" = "linux" ]
 #then
 # MWC_PL_OPTIONS="-name_modifier *_gnu"
 #fi
 cd ${ACE_BUILD_DIRECTORY}
 perl ${MWC_PL} -feature_file ${LOCAL_FEATURES_FILE} ${MWC_PL_OPTIONS} -type ${PROJECT_TYPE} ${ACE_MWC_FILE} -workers 4 -exclude tests -exclude performance-tests -exclude examples
 [ $? -ne 0 ] && echo "ERROR: failed to mwc.pl \"${ACE_MWC_FILE}\": $?, aborting" && exit 1
 echo "processing ${ACE_MWC_FILE}...DONE"
fi

# step4: make
MAKE_OPTIONS=-j4
make ${MAKE_OPTIONS}
[ $? -ne 0 ] && echo "ERROR: failed to make: $?, aborting" && exit 1
# *NOTE*: if this fails, try 'cd /mnt/win_d/projects/ATCD/ACE/apps/gperf/tests'; export LD_LIBRARY_PATH=$ACE_ROOT/lib; make realclean; make

