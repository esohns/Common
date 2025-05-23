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
target_include_directories (${PROJECT_NAME} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/ui>)

############## next target ################
set (source_files
     common.h
     common_configuration.h
     common_counter.cpp
     common_counter.h
     common_counter.inl
     common_defines.h
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
     common_os_tools.h
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
     ${CMAKE_BINARY_DIR}/${PROJECT_NAME}_config.h
     ${source_files})
if (UNIX)
 set (source_files
      common_os_tools_unix.cpp
      ${source_files})
elseif (WIN32)
 set (source_files
      common_os_tools_win32.cpp
      ../include/targetver.h
      ${source_files})
endif ()

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
                PRIVATE ${absolute_source_files})
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

# target_link_libraries (${PROJECT_NAME} PUBLIC X11)
elseif (WIN32)
# set (LIB_WITH_SUFFIX "Msvcrt")
# if (CMAKE_BUILD_TYPE STREQUAL "Debug" OR
#     CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
#  set (LIB_WITH_SUFFIX "${LIB_WITH_SUFFIX}d")
# endif (CMAKE_BUILD_TYPE STREQUAL "Debug" OR
#        CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
# set (LIB_WITH_SUFFIX "${LIB_WITH_SUFFIX}.lib")
# target_link_libraries (${PROJECT_NAME} PUBLIC ${LIB_WITH_SUFFIX})
# target_link_libraries (${PROJECT_NAME} PUBLIC Dbghelp.lib)
 target_link_libraries (${PROJECT_NAME} PUBLIC Quartz.lib)
 target_link_libraries (${PROJECT_NAME} PUBLIC Secur32.lib)
 target_link_libraries (${PROJECT_NAME} PUBLIC Userenv.lib)

# target_link_libraries (${PROJECT_NAME} PUBLIC WinMM.lib)
endif ()
target_link_libraries (${PROJECT_NAME} PUBLIC ${ACE_LIBRARY})

target_link_libraries (${PROJECT_NAME} PUBLIC ${PROJECT_NAME}Error)
target_link_libraries (${PROJECT_NAME} PUBLIC ${PROJECT_NAME}Signal)
target_link_libraries (${PROJECT_NAME} PUBLIC ${PROJECT_NAME}UI)

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
         common_os_tools.h
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
         DESTINATION "${INSTALL_INCLUDE_DIR}/${PROJECT_NAME}"
         COMPONENT include)
install (FILES
         ${CMAKE_BINARY_DIR}/${PROJECT_NAME}_config.h
         DESTINATION "${INSTALL_INCLUDE_DIR}/${PROJECT_NAME}"
         COMPONENT include)
install (TARGETS ${PROJECT_NAME}
         EXPORT ${PROJECT_NAME}Targets
         ARCHIVE DESTINATION ${INSTALL_LIB_DIR} COMPONENT lib
         LIBRARY DESTINATION ${INSTALL_LIB_DIR} COMPONENT lib
         RUNTIME DESTINATION ${INSTALL_LIB_DIR} COMPONENT lib)

##########################################

if (DBUS_SUPPORT)
 add_subdirectory (dbus)
endif (DBUS_SUPPORT)
add_subdirectory (error)
add_subdirectory (event)
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
