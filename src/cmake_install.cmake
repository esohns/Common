# Install script for directory: /mnt/win_d/projects/libCommon/src

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE FILE FILES
    "/mnt/win_d/projects/libCommon/src/common.h"
    "/mnt/win_d/projects/libCommon/src/common_defines.h"
    "/mnt/win_d/projects/libCommon/src/common_exports.h"
    "/mnt/win_d/projects/libCommon/src/common_file_tools.h"
    "/mnt/win_d/projects/libCommon/src/common_iclone.h"
    "/mnt/win_d/projects/libCommon/src/common_icontrol.h"
    "/mnt/win_d/projects/libCommon/src/common_icounter.h"
    "/mnt/win_d/projects/libCommon/src/common_idumpstate.h"
    "/mnt/win_d/projects/libCommon/src/common_iget.h"
    "/mnt/win_d/projects/libCommon/src/common_iinitialize.h"
    "/mnt/win_d/projects/libCommon/src/common_ilock.h"
    "/mnt/win_d/projects/libCommon/src/common_inotify.h"
    "/mnt/win_d/projects/libCommon/src/common_ireferencecount.h"
    "/mnt/win_d/projects/libCommon/src/common_iscanner.h"
    "/mnt/win_d/projects/libCommon/src/common_isignal.h"
    "/mnt/win_d/projects/libCommon/src/common_istatemachine.h"
    "/mnt/win_d/projects/libCommon/src/common_istatistic.h"
    "/mnt/win_d/projects/libCommon/src/common_isubscribe.h"
    "/mnt/win_d/projects/libCommon/src/common_itask.h"
    "/mnt/win_d/projects/libCommon/src/common_itaskcontrol.h"
    "/mnt/win_d/projects/libCommon/src/common_itimer.h"
    "/mnt/win_d/projects/libCommon/src/common_logger.h"
    "/mnt/win_d/projects/libCommon/src/common_logger.inl"
    "/mnt/win_d/projects/libCommon/src/common_macros.h"
    "/mnt/win_d/projects/libCommon/src/common_referencecounter_base.h"
    "/mnt/win_d/projects/libCommon/src/common_signalhandler.h"
    "/mnt/win_d/projects/libCommon/src/common_signalhandler.inl"
    "/mnt/win_d/projects/libCommon/src/common_statemachine_base.h"
    "/mnt/win_d/projects/libCommon/src/common_statemachine_base.inl"
    "/mnt/win_d/projects/libCommon/src/common_task_base.h"
    "/mnt/win_d/projects/libCommon/src/common_task_base.inl"
    "/mnt/win_d/projects/libCommon/src/common_time_common.h"
    "/mnt/win_d/projects/libCommon/src/common_timer_common.h"
    "/mnt/win_d/projects/libCommon/src/common_timerhandler.h"
    "/mnt/win_d/projects/libCommon/src/common_timer_manager.h"
    "/mnt/win_d/projects/libCommon/src/common_timer_manager.inl"
    "/mnt/win_d/projects/libCommon/src/common_timer_manager_common.h"
    "/mnt/win_d/projects/libCommon/src/common_tools.h"
    )
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "lib" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/usr/local/lib/libCommon.a")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/usr/local/lib" TYPE STATIC_LIBRARY FILES "/mnt/win_d/projects/libCommon/src/libCommon.a")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/mnt/win_d/projects/libCommon/src/image/cmake_install.cmake")
  include("/mnt/win_d/projects/libCommon/src/math/cmake_install.cmake")
  include("/mnt/win_d/projects/libCommon/src/ui/cmake_install.cmake")

endif()

