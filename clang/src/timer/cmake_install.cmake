# Install script for directory: /mnt/win_d/projects/Common/src/timer

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
   "/usr/local/include/common_itimer.h;/usr/local/include/common_itimerhandler.h;/usr/local/include/common_time_common.h;/usr/local/include/common_timer.h;/usr/local/include/common_timer_common.h;/usr/local/include/common_timer_defines.h;/usr/local/include/common_timer_handler.h;/usr/local/include/common_timer_manager.h;/usr/local/include/common_timer_manager.inl;/usr/local/include/common_timer_manager_common.h;/usr/local/include/common_timer_resetcounterhandler.h;/usr/local/include/common_timer_second_publisher.h;/usr/local/include/common_timer_second_publisher.inl;/usr/local/include/common_timer_tools.h")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/usr/local/include" TYPE FILE FILES
    "/mnt/win_d/projects/Common/src/timer/common_itimer.h"
    "/mnt/win_d/projects/Common/src/timer/common_itimerhandler.h"
    "/mnt/win_d/projects/Common/src/timer/common_time_common.h"
    "/mnt/win_d/projects/Common/src/timer/common_timer.h"
    "/mnt/win_d/projects/Common/src/timer/common_timer_common.h"
    "/mnt/win_d/projects/Common/src/timer/common_timer_defines.h"
    "/mnt/win_d/projects/Common/src/timer/common_timer_handler.h"
    "/mnt/win_d/projects/Common/src/timer/common_timer_manager.h"
    "/mnt/win_d/projects/Common/src/timer/common_timer_manager.inl"
    "/mnt/win_d/projects/Common/src/timer/common_timer_manager_common.h"
    "/mnt/win_d/projects/Common/src/timer/common_timer_resetcounterhandler.h"
    "/mnt/win_d/projects/Common/src/timer/common_timer_second_publisher.h"
    "/mnt/win_d/projects/Common/src/timer/common_timer_second_publisher.inl"
    "/mnt/win_d/projects/Common/src/timer/common_timer_tools.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xlibx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/usr/local/lib/libCommonTimer.a")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/usr/local/lib" TYPE STATIC_LIBRARY FILES "/mnt/win_d/projects/Common/clang/src/timer/libCommonTimer.a")
endif()

