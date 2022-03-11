set (CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/scripts")
include (systemd)

set (LIBRARY ${PROJECT_NAME}DBus)
add_library (${LIBRARY} STATIC "")

if (UNIX)
# target_include_directories (${LIBRARY} PUBLIC
#                             $<BUILD_INTERFACE:${DBus1_ARCH_INCLUDE_DIR}>)
 if (DBUS_SUPPORT)
  target_include_directories (${LIBRARY} PUBLIC
                              "$<BUILD_INTERFACE:${DBus1_INCLUDE_DIRS}>")
# target_include_directories (${LIBRARY} PUBLIC
#                             "$<BUILD_INTERFACE:${PKG_NETWORKMANAGER_INCLUDE_DIRS}>")
 endif (DBUS_SUPPORT)

 if (SD_BUS_SUPPORT)
  target_include_directories (${LIBRARY} PUBLIC
                              $<BUILD_INTERFACE:${PKG_SYSTEMD_INCLUDE_DIRS}>)
 endif (SD_BUS_SUPPORT)
endif (UNIX)

target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${ACE_INCLUDE_DIR}>)

target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>)

############## next target ################
set (source_files
     common_dbus_common.h
     common_dbus_defines.h
     common_dbus_tools.cpp
     common_dbus_tools.h)

# Include dir for installed targets
set (INCLUDE_DIR include/${PROJECT_NAME}-${upstream_version})

unset (absolute_source_files)
foreach (source_file ${source_files})
 get_filename_component (abs_source_file ${source_file} ABSOLUTE)
 list (APPEND absolute_source_files
       PRIVATE ${abs_source_file}
       )
#       $<INSTALL_INTERFACE:${INCLUDE_DIR}/${source_file}>)
endforeach ()
target_sources (${LIBRARY}
#                PRIVATE stdafx.cpp
                PRIVATE stdafx.h
                PRIVATE ${absolute_source_files})

#add_dependencies (${PROJECT_NAME} ACE)

#set_target_properties (${LIBRARY} PROPERTIES DEFINE_SYMBOL COMMON_DBUS_BUILD_DLL)
#set_target_properties (${LIBRARY} PROPERTIES
#                       VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_MICRO}
#                       SOVERSION 1)

if (UNIX)
 if (DBUS_SUPPORT)
  target_link_libraries (${LIBRARY} ${DBus1_LIBRARIES})
#link_directories (/usr/local/lib/dbus-1.0)
#link_directories (${CMAKE_CURRENT_BINARY_DIR}/../../../dbus/dbus/.libs)
#target_link_libraries (${LIBRARY} libdbus-1.so.3)
 endif (DBUS_SUPPORT)
 if (SD_BUS_SUPPORT)
  target_link_libraries (${LIBRARY} ${PKG_SYSTEMD_LIBRARIES})
 endif (SD_BUS_SUPPORT)
endif ()

# add PCH
target_precompile_headers (${LIBRARY}
                           PRIVATE stdafx.h)

# add all targets to the build-tree export set
export (TARGETS ${LIBRARY}
        APPEND FILE "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake")

############### install files ################
install (FILES
         common_dbus_common.h
         common_dbus_defines.h
         common_dbus_tools.h
         DESTINATION ${INSTALL_INCLUDE_DIR})
install (TARGETS ${LIBRARY}
         EXPORT ${PROJECT_NAME}Targets
         ARCHIVE DESTINATION "${INSTALL_LIB_DIR}"
         COMPONENT lib)
