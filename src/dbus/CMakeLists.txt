set (CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/scripts")
include (dbus)
include (systemd)

add_library (${PROJECT_NAME}DBus STATIC "")

if (UNIX)
# *NOTE*: a bug in cmake (3.10.2) requires the additional comment
# target_include_directories (${PROJECT_NAME}DBus PUBLIC
#                             $<BUILD_INTERFACE:${DBus1_INCLUDE_DIR}>)
# target_include_directories (${PROJECT_NAME}DBus PUBLIC
#                             $<BUILD_INTERFACE:${DBus1_ARCH_INCLUDE_DIR}>)

# message (STATUS "PKG_DBUS_INCLUDE_DIRS: ${PKG_DBUS_INCLUDE_DIRS}")
 target_include_directories (${PROJECT_NAME}DBus PUBLIC
                             "$<BUILD_INTERFACE:${PKG_DBUS_INCLUDE_DIRS}>")
# target_include_directories (${PROJECT_NAME}DBus PUBLIC
#                             "$<BUILD_INTERFACE:${PKG_NETWORKMANAGER_INCLUDE_DIRS}>")

 if (SD_BUS_SUPPORT)
  target_include_directories (${PROJECT_NAME}DBus PUBLIC
                              $<BUILD_INTERFACE:${PKG_SYSTEMD_INCLUDE_DIRS}>)
 endif (SD_BUS_SUPPORT)
endif (UNIX)
target_include_directories (${PROJECT_NAME}DBus PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/../..>)
target_include_directories (${PROJECT_NAME}DBus PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>)

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

#add_dependencies (${PROJECT_NAME} ACE)

#set_target_properties (${PROJECT_NAME}_DBus PROPERTIES DEFINE_SYMBOL COMMON_DBUS_BUILD_DLL)
#set_target_properties (${PROJECT_NAME}DBus PROPERTIES
#                       VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_MICRO}
#                       SOVERSION 1)

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
# target_link_libraries (${PROJECT_NAME}DBus ${ACE_LIBRARY})
endif ()

# add PCH
target_precompile_headers (${PROJECT_NAME}DBus
                           PUBLIC stdafx.h)

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
