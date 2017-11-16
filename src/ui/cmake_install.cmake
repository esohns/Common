# Install script for directory: /mnt/win_d/projects/libCommon/src/ui

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
    "/mnt/win_d/projects/libCommon/src/ui/common_ui_common.h"
    "/mnt/win_d/projects/libCommon/src/ui/common_ui_defines.h"
    "/mnt/win_d/projects/libCommon/src/ui/common_ui_exports.h"
    "/mnt/win_d/projects/libCommon/src/ui/common_ui_gtk_builder_definition.h"
    "/mnt/win_d/projects/libCommon/src/ui/common_ui_gtk_builder_definition.inl"
    "/mnt/win_d/projects/libCommon/src/ui/common_ui_gtk_common.h"
    "/mnt/win_d/projects/libCommon/src/ui/common_ui_gtk_manager.h"
    "/mnt/win_d/projects/libCommon/src/ui/common_ui_gtk_manager.inl"
    "/mnt/win_d/projects/libCommon/src/ui/common_ui_gtk_manager_common.h"
    "/mnt/win_d/projects/libCommon/src/ui/common_ui_igtk.h"
    "/mnt/win_d/projects/libCommon/src/ui/common_ui_tools.h"
    "/mnt/win_d/projects/libCommon/src/ui/common_ui_types.h"
    )
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "lib" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/usr/local/lib/libCommonUI.a")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/usr/local/lib" TYPE STATIC_LIBRARY FILES "/mnt/win_d/projects/libCommon/src/ui/libCommonUI.a")
endif()

