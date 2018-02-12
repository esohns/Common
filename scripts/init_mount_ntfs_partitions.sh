#!/bin/sh
# author:      Erik Sohns <eriksohns@123mail.org>
# this script installs the mount_ntfs_paritions service
# parameters:   - start|stop
# return value: - 0 success, 1 failure

# sanity checks
command -v basename >/dev/null 2>&1 || { echo "basename is not installed, aborting" >&2; exit 1; }
#command -v readlink >/dev/null 2>&1 || { echo "readlink is not installed, aborting" >&2; exit 1; }
command -v /sbin/insserv >/dev/null 2>&1 || { echo "insserv is not installed, aborting" >&2; exit 1; }
#command -v /sbin/service >/dev/null 2>&1 || { echo "service is not installed, aborting" >&2; exit 1; }
command -v cp >/dev/null 2>&1 || { echo "cp is not supported, aborting" >&2; exit 1; }
command -v rm >/dev/null 2>&1 || { echo "rm is not supported, aborting" >&2; exit 1; }

# sanity checks
case "$1" in
 start)
 ;;
 stop)
 ;;
 *)
  echo "Usage $(basename $0) [start|stop]"
  exit 1; 
 ;;
esac 

# sanity check(s)
INIT_DIRECTORY="/etc/init.d"
[ ! -d ${INIT_DIRECTORY} ] && echo "ERROR: invalid init directory (was: \"${INIT_DIRECTORY}\"), aborting" && exit 1
SBIN_DIRECTORY="/sbin"
[ ! -d ${SBIN_DIRECTORY} ] && echo "ERROR: invalid sbin directory (was: \"${SBIN_DIRECTORY}\"), aborting" && exit 1
SCRIPTS_DIRECTORY="$(dirname $(readlink -f $0))"
[ ! -d ${SCRIPTS_DIRECTORY} ] && echo "ERROR: invalid scripts directory (was: \"${SCRIPTS_DIRECTORY}\"), aborting" && exit 1

case "$1" in
 start)
  INIT_SERVICE_FILE="${SCRIPTS_DIRECTORY}/mount_ntfs_partitions.init"
  [ ! -r ${INIT_SERVICE_FILE} ] && echo "ERROR: invalid init service file (was: \"${INIT_SERVICE_FILE}\"), aborting" && exit 1
  SERVICE_SCRIPT_FILE="${SCRIPTS_DIRECTORY}/mount_ntfs_partitions.sh"
  [ ! -r ${SERVICE_SCRIPT_FILE} ] && echo "ERROR: invalid script file (was: \"${SERVICE_SCRIPT_FILE}\"), aborting" && exit 1

  cp -f $INIT_SERVICE_FILE $INIT_DIRECTORY >/dev/null 2>&1
  [ $? -ne 0 ] && echo "ERROR: failed to cp \"$INIT_SERVICE_FILE\" to \"$INIT_DIRECTORY\": $?, aborting" && exit 1 
  echo "copied ${INIT_SERVICE_FILE}"
  cp -f $SERVICE_SCRIPT_FILE $SBIN_DIRECTORY >/dev/null 2>&1
  [ $? -ne 0 ] && echo "ERROR: failed to cp \"$SERVICE_SCRIPT_FILE\" to \"$SBIN_DIRECTORY\": $?, aborting" && exit 1 
  echo "copied ${SERVICE_SCRIPT_FILE}"

  insserv "$INIT_DIRECTORY/mount_ntfs_partitions.init"
  [ $? -ne 0 ] && echo "ERROR: failed to insserv \"$INIT_DIRECTORY/mount_ntfs_partitions.init\": $?, aborting" && exit 1 
  echo "installed mount_ntfs_partitions.init"  
 # service mount_ntfs_partitions start
 # [ $? -ne 0 ] && echo "ERROR: failed to start mount_ntfs_partitions service: $?, aborting" && exit 1 
 # echo "started mount_ntfs_partitions service"
  ;;
 stop)
#  service mount_ntfs_partitions stop
#  if [ $? -ne 0 ]; then
#   echo "ERROR: failed to stop mount_ntfs_partitions service: $?, continuing"
#  else
#   echo "stopped mount_ntfs_partitions service"
#  fi
  insserv -r "$INIT_DIRECTORY/mount_ntfs_partitions.init"
  if [ $? -ne 0 ]; then
   echo "ERROR: failed to insserv -r \"$INIT_DIRECTORY/mount_ntfs_partitions.init\": $?, continuing"
  else
   echo "uninstalled mount_ntfs_partitions.init"
  fi

  INIT_SERVICE_FILE="${INIT_DIRECTORY}/mount_ntfs_partitions.init"
  [ ! -r ${INIT_SERVICE_FILE} ] && echo "ERROR: invalid init service file (was: \"${INIT_SERVICE_FILE}\"), aborting" && exit 1
  SERVICE_SCRIPT_FILE="${SBIN_DIRECTORY}/mount_ntfs_partitions.sh"
  [ ! -r ${SERVICE_SCRIPT_FILE} ] && echo "ERROR: invalid service script file (was: \"${SERVICE_SCRIPT_FILE}\"), aborting" && exit 1

  rm -f $INIT_SERVICE_FILE >/dev/null 2>&1
  [ $? -ne 0 ] && echo "ERROR: failed to rm \"$INIT_SERVICE_FILE\": $?, aborting" && exit 1 
  echo "deleted ${INIT_SERVICE_FILE}"
  rm -f $SERVICE_SCRIPT_FILE >/dev/null 2>&1
  [ $? -ne 0 ] && echo "ERROR: failed to rm \"$SERVICE_SCRIPT_FILE\": $?, aborting" && exit 1 
  echo "deleted ${SERVICE_SCRIPT_FILE}"
  ;;
 *)
  echo "usage: $(basename $0) [start|stop]"
  exit 1; 
  ;;
esac 

