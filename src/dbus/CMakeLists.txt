set (ACE_LIB_FILE libACE.so)
if (UNIX)
 set (AIO_LIB_FILE libaio.so)
 find_library (AIO_LIBRARY ${AIO_LIB_FILE}
               PATHS /lib /usr/lib
               PATH_SUFFIXES x86_64-linux-gnu
               DOC "searching for ${AIO_LIB_FILE}")
# message ("CMAKE_FIND_ROOT_PATH is ${CMAKE_FIND_ROOT_PATH}")
 find_library (ACE_LIBRARY ${ACE_LIB_FILE}
               HINTS ${CMAKE_CURRENT_BINARY_DIR}/../../../ATCD/ACE /usr/local
               PATHS ENV ACE_ROOT
               PATH_SUFFIXES lib
               DOC "searching for ${ACE_LIB_FILE}"
               NO_DEFAULT_PATH)
# else ()
#  find_library (ACE_LIBRARY ${ACE_LIB_FILE}
#                DOC "searching for ${ACE_LIB_FILE} (system paths)")
# endif ()
elseif (WIN32)
 if (CMAKE_BUILD_TYPE STREQUAL Debug)
  set (LIB_FILE_SUFFIX d)
 endif ()
 unset (ACE_LIB_FILE)
 set (ACE_LIB_FILE ACE${LIB_FILE_SUFFIX}.lib)
 find_library (ACE_LIBRARY ${ACE_LIB_FILE}
               PATHS ENV ACE_ROOT
               PATH_SUFFIXES lib lib\\${CMAKE_BUILD_TYPE}\\Win32
               DOC "searching for ${ACE_LIB_FILE}")
endif ()
#if (NOT EXISTS ACE_LIBRARY)
if (NOT ACE_LIBRARY)
 message (FATAL_ERROR "Could not find ${ACE_LIB_FILE} (was: \"${ACE_LIBRARY}\"), aborting")
else ()
 message (STATUS "Found ACE library \"${ACE_LIBRARY}\"")
endif ()

include (FindPkgConfig)
if (UNIX)
 pkg_check_modules (PKG_DBUS REQUIRED dbus-1)
 pkg_check_modules (PKG_NETWORKMANAGER REQUIRED NetworkManager)
endif ()
#pkg_check_modules (PKG_ACE REQUIRED ACE)

if (UNIX)
 include_directories (${PKG_DBUS_INCLUDE_DIRS})
 #include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../dbus)
 include_directories (${PKG_NETWORKMANAGER_INCLUDE_DIRS})

 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../modules/ACE)
elseif (WIN32)
 include_directories ($ENV{ACE_ROOT})
endif ()
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/..)
include_directories (${CMAKE_CURRENT_BINARY_DIR}/..)

############## next target ################
set (${PROJECT_NAME}_DBus_LIB_SRCS
     common_dbus_tools.cpp
     common_dbus_tools.h
     stdafx.cpp
     stdafx.h)

add_library (${PROJECT_NAME}_DBus STATIC
             ${Common_DBus_LIB_SRCS})

#add_dependencies (${PROJECT_NAME} ACE)

#set_target_properties (${PROJECT_NAME}_DBus PROPERTIES COMPILE_DEFINITIONS ACE_HAS_DLL)
#set_target_properties (${PROJECT_NAME}_DBus PROPERTIES DEFINE_SYMBOL COMMON_DBUS_BUILD_DLL)
set_target_properties (${PROJECT_NAME}_DBus PROPERTIES
                       VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_MICRO}
                       SOVERSION 1)

if (UNIX)
# target_link_libraries (${PROJECT_NAME}_DBus ${ACE_LIBRARY})
# target_link_libraries (${PROJECT_NAME}_DBus ${PKG_DBUS_LIBRARIES})
 #link_directories (/usr/local/lib/dbus-1.0)
 #link_directories (${CMAKE_CURRENT_BINARY_DIR}/../../../dbus/dbus/.libs)
 #target_link_libraries (${PROJECT_NAME} libdbus-1.so.3)
elseif (WIN32)
# if (CMAKE_BUILD_TYPE STREQUAL Debug)
#  target_link_libraries (${PROJECT_NAME}_DBus Msvcrtd.lib)
# else ()
#  target_link_libraries (${PROJECT_NAME}_DBus Msvcrt.lib)
# endif ()
# target_link_libraries (${PROJECT_NAME}_DBus Quartz.lib)
# target_link_libraries (${PROJECT_NAME}_DBus Secur32.lib)
# target_link_libraries (${PROJECT_NAME_DBus} Userenv.lib)

 target_link_libraries (${PROJECT_NAME}_DBus ${ACE_LIBRARY})
endif ()

# add PCH
set_target_properties (${PROJECT_NAME}_DBus PROPERTIES
                       COTIRE_CXX_PREFIX_HEADER_INIT "stdafx.h")
cotire (${PROJECT_NAME}_DBus)

# add all targets to the build-tree export set
export (TARGETS ${PROJECT_NAME}_DBus
        APPEND FILE "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake")

############### install files ################
install (FILES
         common_dbus_tools.h
         stdafx.h
         DESTINATION ${INSTALL_INCLUDE_DIR})
install (TARGETS ${PROJECT_NAME}_DBus
         EXPORT ${PROJECT_NAME}Targets
         ARCHIVE DESTINATION "${INSTALL_LIB_DIR}" COMPONENT lib
         LIBRARY DESTINATION "${INSTALL_LIB_DIR}" COMPONENT shlib
#         RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin
         COMPONENT dev)
