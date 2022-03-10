# configure C/C++ exceptions
include (SetCppExceptions)

include (Threads)

include (ACE)

set (CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/dbus/scripts")
include (dbus)
set (CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/gl/scripts")
include (OpenGL)

if (UNIX)
 include (aio)

 pkg_check_modules (PKG_LIBCAP libcap)
 if (PKG_LIBCAP_FOUND)
  option (LIBCAP_SUPPORT "enable libcap support" ON)
  if (LIBCAP_SUPPORT)
   add_definitions (-DLIBCAP_SUPPORT)
  endif (LIBCAP_SUPPORT)
 endif (PKG_LIBCAP_FOUND)

 pkg_check_modules (PKG_LIBSYSFS libsysfs)
 if (PKG_LIBSYSFS_FOUND)
  option (LIBSYSFS_SUPPORT "enable libsysfs support" ON)
  if (LIBSYSFS_SUPPORT)
   add_definitions (-DLIBSYSFS_SUPPORT)
  endif (LIBSYSFS_SUPPORT)
 endif (PKG_LIBSYSFS_FOUND)

 add_link_options ("LINKER:--as-needed")
endif (UNIX)

add_library (${PROJECT_NAME} STATIC "")

if (UNIX)
 if (LIBCAP_SUPPORT)
  target_include_directories (${PROJECT_NAME} PUBLIC
                              $<BUILD_INTERFACE:${PKG_LIBCAP_INCLUDE_DIRS}>)
 endif (LIBCAP_SUPPORT)

 if (LIBSYSFS_SUPPORT)
  target_include_directories (${PROJECT_NAME} PUBLIC
                              $<BUILD_INTERFACE:${PKG_LIBSYSFS_INCLUDE_DIRS}>)
 endif (LIBSYSFS_SUPPORT)
endif (UNIX)

target_include_directories (${PROJECT_NAME} PUBLIC
                            $<BUILD_INTERFACE:${ACE_INCLUDE_DIR}>)

target_include_directories (${PROJECT_NAME} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}>)
if (WIN32)
 target_include_directories (${PROJECT_NAME} PUBLIC
                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>)
endif (WIN32)
target_include_directories (${PROJECT_NAME} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/error>)
target_include_directories (${PROJECT_NAME} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/signal>)
target_include_directories (${PROJECT_NAME} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/task>)
target_include_directories (${PROJECT_NAME} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/timer>)

############## next target ################
set (source_files
     common.h
     common_configuration.h
     common_counter.cpp
     common_counter.h
     common_counter.inl
     common_defines.h
     common_event_handler.cpp
     common_event_handler.h
     common_file_common.h
     common_file_tools.cpp
     common_file_tools.h
     common_iclone.h
     common_icounter.h
     common_idumpstate.h
     common_iget.h
     common_iinitialize.h
     common_ilock.h
     common_inotify.h
     common_ireferencecount.h
     common_istatemachine.h
     common_istatistic.h
     common_isubscribe.h
     common_macros.h
     common_message_queue_iterator.cpp
     common_message_queue_iterator.h
     common_message_queue_iterator.inl
     common_pragmas.h
     common_process_tools.cpp
     common_process_tools.h
     common_referencecounter.cpp
     common_referencecounter.h
     common_referencecounter.inl
     common_statemachine.cpp
     common_statemachine.h
     common_statemachine.inl
     common_statemachine_base.cpp
     common_statemachine_base.h
     common_statemachine_base.inl
     common_statistic_handler.cpp
     common_statistic_handler.h
     common_statistic_handler.inl
     common_string_tools.cpp
     common_string_tools.h
     common_tools.cpp
     common_tools.h
     common_tools.inl)
set (source_files
     ${CMAKE_CURRENT_BINARY_DIR}/../${PROJECT_NAME}_config.h
     ${source_files})
if (WIN32)
 set (source_files
      ../include/targetver.h
      ${source_files}
      common_event.cpp
      common_event.h)
endif (WIN32)

# Include dir for installed targets
#set (INCLUDE_DIR include/${PROJECT_NAME}-${upstream_version})

foreach (source_file ${source_files})
 get_filename_component (abs_source_file ${source_file} ABSOLUTE)
 list (APPEND absolute_source_files
       PRIVATE ${abs_source_file}
       )
#       $<INSTALL_INTERFACE:${INCLUDE_DIR}/${source_file}>)
endforeach ()
target_sources (${PROJECT_NAME}
#                PRIVATE stdafx.cpp
                PRIVATE stdafx.h
                ${absolute_source_files})
target_precompile_headers (${PROJECT_NAME}
                           PRIVATE stdafx.h)

#add_dependencies (${PROJECT_NAME} ACE)
if (WIN32)
 add_dependencies (${PROJECT_NAME} ${PROJECT_NAME}Error)
endif (WIN32)
#add_dependencies (${PROJECT_NAME} ${PROJECT_NAME}Timer)

#set_target_properties (${PROJECT_NAME} PROPERTIES
#                       DEFINE_SYMBOL COMMON_BUILD_DLL)
#set_target_properties (${PROJECT_NAME} PROPERTIES
#                       VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_MICRO} SOVERSION 1)

if (UNIX)
 if (HAVE_POSIX_AIO)
  target_link_libraries (${PROJECT_NAME} PUBLIC rt)
 endif (HAVE_POSIX_AIO)

 if (LIBCAP_SUPPORT)
  target_link_libraries (${PROJECT_NAME} PUBLIC ${PKG_LIBCAP_LIBRARIES})
 endif (LIBCAP_SUPPORT)

 if (LIBSYSFS_SUPPORT)
  target_link_libraries (${PROJECT_NAME} PUBLIC ${PKG_LIBSYSFS_LIBRARIES})
 endif (LIBSYSFS_SUPPORT)
elseif (WIN32)
 if (CMAKE_BUILD_TYPE STREQUAL Debug)
  target_link_libraries (${PROJECT_NAME} PUBLIC Msvcrtd.lib)
 else ()
  target_link_libraries (${PROJECT_NAME} PUBLIC Msvcrt.lib)
 endif (CMAKE_BUILD_TYPE STREQUAL Debug)
 target_link_libraries (${PROJECT_NAME} PUBLIC Dbghelp.lib)
 target_link_libraries (${PROJECT_NAME} PUBLIC Quartz.lib)
 target_link_libraries (${PROJECT_NAME} PUBLIC Secur32.lib)
 target_link_libraries (${PROJECT_NAME} PUBLIC Userenv.lib)

 target_link_libraries (${PROJECT_NAME} PUBLIC ${ACE_LIBRARY})
endif ()
target_link_libraries (${PROJECT_NAME} PUBLIC ${PROJECT_NAME}Error)

##########################################

# add all targets to the build-tree export set
export (TARGETS ${PROJECT_NAME}
        FILE "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake")

############### install files ############
install (FILES
         common.h
         common_configuration.h
         common_counter.h
         common_counter.inl
         common_defines.h
         common_event_handler.h
         common_file_common.h
         common_file_tools.h
         common_iclone.h
         common_icontrol.h
         common_icounter.h
         common_idumpstate.h
         common_iget.h
         common_iinitialize.h
         common_ilock.h
         common_inotify.h
         common_ireferencecount.h
         common_istatemachine.h
         common_istatistic.h
         common_isubscribe.h
         common_macros.h
         common_message_queue_iterator.h
         common_message_queue_iterator.inl
         common_pragmas.h
         common_process_tools.h
         common_referencecounter.h
         common_referencecounter.inl
         common_statemachine.h
         common_statemachine.inl
         common_statemachine_base.h
         common_statemachine_base.inl
         common_statistic_handler.h
         common_statistic_handler.inl
         common_string_tools.h
         common_tools.h
         common_tools.inl
         DESTINATION ${INSTALL_INCLUDE_DIR})
if (WIN32)
 install (FILES
          ../include/lib${PROJECT_NAME}_config.h
         DESTINATION ${INSTALL_INCLUDE_DIR})
endif (WIN32)
install (TARGETS ${PROJECT_NAME}
         EXPORT ${PROJECT_NAME}Targets
         ARCHIVE DESTINATION "${INSTALL_LIB_DIR}"
         COMPONENT lib)

##########################################

if (DBUS_SUPPORT)
 add_subdirectory (dbus)
endif (DBUS_SUPPORT)
add_subdirectory (error)
if (OPENGL_SUPPORT)
 add_subdirectory (gl)
endif (OPENGL_SUPPORT)
add_subdirectory (image)
add_subdirectory (input)
add_subdirectory (log)
add_subdirectory (math)
add_subdirectory (parser)
add_subdirectory (signal)
add_subdirectory (task)
add_subdirectory (timer)
add_subdirectory (ui)
add_subdirectory (xml)
