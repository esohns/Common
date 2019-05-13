option (SD_BUS_SUPPORT "compile sd-bus (libsystemd) support" ON)

if (UNIX)
# find_package (DBus1 MODULE)
 find_package (DBus1)
 if (NOT DBus1_DIR)
  find_library (DBUS_LIBRARY libdbus-1.so.3
                DOC "searching for libdbus-1")
  if (NOT DBUS_LIBRARY)
   message (FATAL_ERROR "could not find libdbus-1, aborting")
  endif ()
#  pkg_check_modules (PKG_DBUS REQUIRED dbus-1)
 endif (NOT DBus1_DIR)
 add_definitions (-DDBUS_SUPPORT)

 if (SD_BUS_SUPPORT)
#  find_library (SYSTEMD_LIBRARY libsystemd.so.0
#                DOC "searching for libsystemd")
#  if (NOT SYSTEMD_LIBRARY)
#   message (FATAL_ERROR "could not find libsystemd, aborting")
#  endif (NOT SYSTEMD_LIBRARY)
  pkg_check_modules (PKG_SYSTEMD REQUIRED libsystemd)
  if (PKG_SYSTEMD_FOUND)
   # *TODO*: on Fedora 29 systems /usr/lib64/pkgconfig/libsystemd.pc is broken
#   message (STATUS "PKG_SYSTEMD_INCLUDE_DIRS: ${PKG_SYSTEMD_INCLUDE_DIRS}")
   set (PKG_SYSTEMD_INCLUDE_DIRS "/usr/include/systemd")
  endif (PKG_SYSTEMD_FOUND)
  add_definitions (-DSD_BUS_SUPPORT)
 endif (SD_BUS_SUPPORT)

 find_library (NETWORKMANAGER_LIBRARY libnm.so.0
               DOC "searching for libnm")
 if (NOT NETWORKMANAGER_LIBRARY)
  message (FATAL_ERROR "could not find libnm, aborting")
 endif (NOT NETWORKMANAGER_LIBRARY)
#  pkg_check_modules (PKG_NETWORKMANAGER REQUIRED NetworkManager)
# pkg_check_modules (PKG_NETWORKMANAGER REQUIRED libnm)
endif (UNIX)

add_library (${PROJECT_NAME}DBus STATIC "")

if (UNIX)
# *NOTE*: a bug in cmake (3.10.2) requires the additional comment
 target_include_directories (${PROJECT_NAME}DBus PUBLIC
                             $<BUILD_INTERFACE:${DBus1_INCLUDE_DIR}>)
 target_include_directories (${PROJECT_NAME}DBus PUBLIC
                             $<BUILD_INTERFACE:${DBus1_ARCH_INCLUDE_DIR}>)
# target_include_directories (${PROJECT_NAME}DBus PUBLIC
#                             "$<BUILD_INTERFACE:${PKG_DBUS_INCLUDE_DIRS}>")
 target_include_directories (${PROJECT_NAME}DBus PUBLIC
                             "$<BUILD_INTERFACE:${PKG_NETWORKMANAGER_INCLUDE_DIRS}>")

 if (SD_BUS_SUPPORT)
  target_include_directories (${PROJECT_NAME}DBus PUBLIC
                              $<BUILD_INTERFACE:${PKG_SYSTEMD_INCLUDE_DIRS}>)
 endif (SD_BUS_SUPPORT)

 target_include_directories (${PROJECT_NAME}DBus PUBLIC
                     #        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../modules/ACE>)
                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../ACE_TAO/ACE>)
endif (UNIX)
target_include_directories (${PROJECT_NAME}DBus PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>)
target_include_directories (${PROJECT_NAME}DBus PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/../..>)

############## next target ################
set (source_files
     common_dbus_common.h
     common_dbus_defines.h
     common_dbus_tools.cpp
     common_dbus_tools.h)

# Include dir for installed targets
set (INCLUDE_DIR include/${PROJECT_NAME}-${upstream_version})

#set_target_properties (${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)

unset (absolute_source_files)
foreach (source_file ${source_files})
 get_filename_component (abs_source_file ${source_file} ABSOLUTE)
 list (APPEND absolute_source_files
       PRIVATE ${abs_source_file}
       )
#       $<INSTALL_INTERFACE:${INCLUDE_DIR}/${source_file}>)
endforeach ()
target_sources (${PROJECT_NAME}DBus
                PRIVATE stdafx.cpp
                PRIVATE stdafx.h
                ${absolute_source_files})

#get_target_property (debug ${PROJECT_NAME}DBus INTERFACE_INCLUDE_DIRECTORIES)
#message (STATUS "INTERFACE_INCLUDE_DIRECTORIES: \"${debug}\"")

#add_dependencies (${PROJECT_NAME} ACE)

#set_target_properties (${PROJECT_NAME}_DBus PROPERTIES COMPILE_DEFINITIONS ACE_HAS_DLL)
#set_target_properties (${PROJECT_NAME}_DBus PROPERTIES DEFINE_SYMBOL COMMON_DBUS_BUILD_DLL)
set_target_properties (${PROJECT_NAME}DBus PROPERTIES
                       VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_MICRO}
                       SOVERSION 1)

if (UNIX)
# target_link_libraries (${PROJECT_NAME}DBus ${ACE_LIBRARY})
 target_link_libraries (${PROJECT_NAME}DBus ${PKG_DBUS_LIBRARIES})
 #link_directories (/usr/local/lib/dbus-1.0)
 #link_directories (${CMAKE_CURRENT_BINARY_DIR}/../../../dbus/dbus/.libs)
 #target_link_libraries (${PROJECT_NAME} libdbus-1.so.3)
 if (SD_BUS_SUPPORT)
  target_link_libraries (${PROJECT_NAME}DBus ${PKG_SYSTEMD_LIBRARIES})
 endif (SD_BUS_SUPPORT)
elseif (WIN32)
# if (CMAKE_BUILD_TYPE STREQUAL Debug)
#  target_link_libraries (${PROJECT_NAME}_DBus Msvcrtd.lib)
# else ()
#  target_link_libraries (${PROJECT_NAME}_DBus Msvcrt.lib)
# endif ()
# target_link_libraries (${PROJECT_NAME}_DBus Quartz.lib)
# target_link_libraries (${PROJECT_NAME}_DBus Secur32.lib)
# target_link_libraries (${PROJECT_NAME_DBus} Userenv.lib)

# target_link_libraries (${PROJECT_NAME}DBus ${ACE_LIBRARY})
endif ()

# add PCH
set_target_properties (${PROJECT_NAME}DBus PROPERTIES
                       COTIRE_CXX_PREFIX_HEADER_INIT "stdafx.h")
cotire (${PROJECT_NAME}DBus)

# add all targets to the build-tree export set
export (TARGETS ${PROJECT_NAME}DBus
        APPEND FILE "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake")

############### install files ################
install (FILES
         common_dbus_common.h
         common_dbus_defines.h
         common_dbus_tools.h
         DESTINATION ${INSTALL_INCLUDE_DIR})
install (TARGETS ${PROJECT_NAME}DBus
         EXPORT ${PROJECT_NAME}Targets
         ARCHIVE DESTINATION "${INSTALL_LIB_DIR}"
         COMPONENT lib)
