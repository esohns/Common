# Install script for directory: /mnt/win_d/projects/Common/src

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

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "TRUE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/usr/local/include/common.h;/usr/local/include/common_configuration.h;/usr/local/include/common_defines.h;/usr/local/include/common_event_handler.h;/usr/local/include/common_file_common.h;/usr/local/include/common_file_tools.h;/usr/local/include/common_iclone.h;/usr/local/include/common_icontrol.h;/usr/local/include/common_icounter.h;/usr/local/include/common_idumpstate.h;/usr/local/include/common_iget.h;/usr/local/include/common_iinitialize.h;/usr/local/include/common_ilock.h;/usr/local/include/common_inotify.h;/usr/local/include/common_ireferencecount.h;/usr/local/include/common_istatemachine.h;/usr/local/include/common_istatistic.h;/usr/local/include/common_isubscribe.h;/usr/local/include/common_itask.h;/usr/local/include/common_itaskcontrol.h;/usr/local/include/common_macros.h;/usr/local/include/common_message_queue_iterator.h;/usr/local/include/common_message_queue_iterator.inl;/usr/local/include/common_pragmas.h;/usr/local/include/common_referencecounter_base.h;/usr/local/include/common_statemachine.h;/usr/local/include/common_statemachine.inl;/usr/local/include/common_statemachine_base.h;/usr/local/include/common_statemachine_base.inl;/usr/local/include/common_statistic_handler.h;/usr/local/include/common_statistic_handler.inl;/usr/local/include/common_string_tools.h;/usr/local/include/common_task.h;/usr/local/include/common_task.inl;/usr/local/include/common_task_base.h;/usr/local/include/common_task_base.inl;/usr/local/include/common_tools.h")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/usr/local/include" TYPE FILE FILES
    "/mnt/win_d/projects/Common/src/common.h"
    "/mnt/win_d/projects/Common/src/common_configuration.h"
    "/mnt/win_d/projects/Common/src/common_defines.h"
    "/mnt/win_d/projects/Common/src/common_event_handler.h"
    "/mnt/win_d/projects/Common/src/common_file_common.h"
    "/mnt/win_d/projects/Common/src/common_file_tools.h"
    "/mnt/win_d/projects/Common/src/common_iclone.h"
    "/mnt/win_d/projects/Common/src/common_icontrol.h"
    "/mnt/win_d/projects/Common/src/common_icounter.h"
    "/mnt/win_d/projects/Common/src/common_idumpstate.h"
    "/mnt/win_d/projects/Common/src/common_iget.h"
    "/mnt/win_d/projects/Common/src/common_iinitialize.h"
    "/mnt/win_d/projects/Common/src/common_ilock.h"
    "/mnt/win_d/projects/Common/src/common_inotify.h"
    "/mnt/win_d/projects/Common/src/common_ireferencecount.h"
    "/mnt/win_d/projects/Common/src/common_istatemachine.h"
    "/mnt/win_d/projects/Common/src/common_istatistic.h"
    "/mnt/win_d/projects/Common/src/common_isubscribe.h"
    "/mnt/win_d/projects/Common/src/common_itask.h"
    "/mnt/win_d/projects/Common/src/common_itaskcontrol.h"
    "/mnt/win_d/projects/Common/src/common_macros.h"
    "/mnt/win_d/projects/Common/src/common_message_queue_iterator.h"
    "/mnt/win_d/projects/Common/src/common_message_queue_iterator.inl"
    "/mnt/win_d/projects/Common/src/common_pragmas.h"
    "/mnt/win_d/projects/Common/src/common_referencecounter_base.h"
    "/mnt/win_d/projects/Common/src/common_statemachine.h"
    "/mnt/win_d/projects/Common/src/common_statemachine.inl"
    "/mnt/win_d/projects/Common/src/common_statemachine_base.h"
    "/mnt/win_d/projects/Common/src/common_statemachine_base.inl"
    "/mnt/win_d/projects/Common/src/common_statistic_handler.h"
    "/mnt/win_d/projects/Common/src/common_statistic_handler.inl"
    "/mnt/win_d/projects/Common/src/common_string_tools.h"
    "/mnt/win_d/projects/Common/src/common_task.h"
    "/mnt/win_d/projects/Common/src/common_task.inl"
    "/mnt/win_d/projects/Common/src/common_task_base.h"
    "/mnt/win_d/projects/Common/src/common_task_base.inl"
    "/mnt/win_d/projects/Common/src/common_tools.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xlibx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/usr/local/lib/libCommon.a")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/usr/local/lib" TYPE STATIC_LIBRARY FILES "/mnt/win_d/projects/Common/clang/src/libCommon.a")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/mnt/win_d/projects/Common/clang/src/dbus/cmake_install.cmake")
  include("/mnt/win_d/projects/Common/clang/src/error/cmake_install.cmake")
  include("/mnt/win_d/projects/Common/clang/src/gl/cmake_install.cmake")
  include("/mnt/win_d/projects/Common/clang/src/image/cmake_install.cmake")
  include("/mnt/win_d/projects/Common/clang/src/log/cmake_install.cmake")
  include("/mnt/win_d/projects/Common/clang/src/math/cmake_install.cmake")
  include("/mnt/win_d/projects/Common/clang/src/parser/cmake_install.cmake")
  include("/mnt/win_d/projects/Common/clang/src/signal/cmake_install.cmake")
  include("/mnt/win_d/projects/Common/clang/src/timer/cmake_install.cmake")
  include("/mnt/win_d/projects/Common/clang/src/ui/cmake_install.cmake")
  include("/mnt/win_d/projects/Common/clang/src/xml/cmake_install.cmake")

endif()

