#!/bin/sh
# author:      Erik Sohns <eriksohns@123mail.org>
# currently there is a *bug* in ntfs-3g that (auto-)mounts partitions in
# /etc/fstab with 'nosuid,nodev' options (try it, check with 'cat /proc/mounts').
# The problem does not occur when mounting paritions 'manually' (i.e. calling
# 'mount.ntfs' directly)
# --> use this script as a *workaround* to mount ntfs-3g partitions, copy/link
#     to '/etc/init.d' (YMMV) to load on startup
# return value: - 0 success, 1 failure

# sanity checks
command -v mount.ntfs >/dev/null 2>&1 || { echo "ntfs-3g is not installed, aborting" >&2; exit 1; }
command -v umount >/dev/null 2>&1 || { echo "umount is not installed, aborting" >&2; exit 1; }
# sanity checks
command -v echo >/dev/null 2>&1 || { echo "echo is not supported, aborting" >&2; exit 1; }
#command -v readlink >/dev/null 2>&1 || { echo "readlink is not installed, aborting" >&2; exit 1; }
#command -v tr >/dev/null 2>&1 || { echo "tr is not installed, aborting" >&2; exit 1; }

# sanity checks
case "$1" in
 start)
 ;;
 stop)
 ;;
 *)
  echo "Usage $0 (start|stop)"
  exit 1; 
 ;;
esac 

# sanity checks
DEV_ROOT="/dev"
[ ! -d "${DEV_ROOT}" ] && echo "ERROR: invalid root device file directory (was: \"${DEV_ROOT}\"), aborting" && exit 1
MOUNT_ROOT="/mnt"
[ ! -d "${MOUNT_ROOT}" ] && echo "ERROR: invalid root mount point (was: \"${MOUNT_ROOT}\"), aborting" && exit 1
MOUNT_OPTIONS="permissions,big_writes,dev,suid"

declare -a DEVS=(sda1 sda3)
declare -a MOUNT_POINTS=(win_c win_d)
i=0
for DEV in "${DEVS[@]}"
do
# echo "DEBUG: dev: \"${DEV}\""
 case "$1" in
  start)
   # sanity checks
   [ ! -r "${DEV_ROOT}/${DEV}" ] && echo "ERROR: invalid device file/partition (was: \"${DEV_ROOT}/${DEV}\"), aborting" && exit 1
   MOUNT_POINT="${MOUNT_ROOT}/${MOUNT_POINTS[$i]}"
   [ ! -d "${MOUNT_POINT}" ] && echo "ERROR: invalid mount point (was: \"${MOUNT_POINT}\"), continuing" && continue

   mount.ntfs -o ${MOUNT_OPTIONS} ${DEV_ROOT}/${DEV} ${MOUNT_POINT} >/dev/null 2>&1
   if [ $? -ne 0 ]; then
     echo "ERROR: failed to mount.ntfs \"${DEV_ROOT}/${DEV}\" at ${MOUNT_POINT}: $?, continuing"
   else
    echo "mounted ${DEV_ROOT}/${DEV} at ${MOUNT_POINT}"
   fi
  ;;
  stop)
   MOUNT_POINT="${MOUNT_ROOT}/${MOUNT_POINTS[$i]}"
   [ ! -d "${MOUNT_POINT}" ] && echo "ERROR: invalid mount point (was: \"${MOUNT_POINT}\"), continuing" && continue

   umount ${MOUNT_POINT} >/dev/null 2>&1
   if [ $? -ne 0 ]; then
    echo "ERROR: failed to umount \"${MOUNT_POINT}\": $?, continuing"
   else
    echo "unmounted \"${MOUNT_POINT}\""
   fi
  ;;
  *)
  ;;
 esac
 i=$(($i+1))
done

