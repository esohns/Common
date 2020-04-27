include (${CMAKE_CURRENT_SOURCE_DIR}/../scripts/Threads.cmake)

include (${CMAKE_CURRENT_SOURCE_DIR}/../scripts/ACE.cmake)

# *TODO*: compile in support as available automatically
if (UNIX)
 option (DBUS_SUPPORT "compile DBus support" ON)
endif (UNIX)

# configure C/C++ exceptions
include (${CMAKE_CURRENT_SOURCE_DIR}/../scripts/SetCppExceptions.cmake)

if (UNIX)
 include (CheckLibraryExists)
 #*TODO*: why isn't the function found ?
 check_library_exists (rt aio_init "" HAVE_LIB_AIO)

 set (AIO_LIB_FILE libaio.so.1)
 find_library (AIO_LIBRARY ${AIO_LIB_FILE}
               PATHS /lib /usr/lib /usr/lib64
               PATH_SUFFIXES x86_64-linux-gnu
               DOC "searching for ${AIO_LIB_FILE}")
 if (NOT AIO_LIBRARY)
  message (FATAL_ERROR "could not find libaio, aborting")
 endif (NOT AIO_LIBRARY)

# find_library (CAP_LIBRARY libcap.so.2
#               DOC "searching for libcap")
# if (NOT CAP_LIBRARY)
#  message (FATAL_ERROR "could not find libcap, aborting")
# endif ()
 pkg_check_modules (PKG_LIBCAP REQUIRED libcap)
endif (UNIX)

if (UNIX)
 add_definitions (-D_GLIBCXX_INCLUDE_NEXT_C_HEADERS)
endif (UNIX)

set (LLVM_COMPILER_ID_STRING "Clang")
string (TOLOWER "${LLVM_COMPILER_ID_STRING}" LLVM_COMPILER_ID_STRING_LOWER)
string (TOLOWER "${CMAKE_CXX_COMPILER_ID}" CXX_COMPILER_ID_STRING_LOWER)
if (CXX_COMPILER_ID_STRING_LOWER STREQUAL LLVM_COMPILER_ID_STRING_LOWER)
# message (STATUS "using ${CMAKE_CXX_COMPILER_ID} compiler options")
 add_compile_options (-std=c++11)
# add_compile_options (-fPIC)
 add_definitions (-D_GLIBCXX_USE_CXX11_ABI=0)
endif ()

add_definitions (-DHAVE_CONFIG_H)
#set_property (SOURCE PROPERTY COMPILE_DEFINITIONS HAVE_CONFIG_H)

if (UNIX)
elseif (WIN32)
 add_definitions (-DWIN32)
 set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc") # C++ exception handling
 if (CMAKE_BUILD_TYPE STREQUAL Debug)
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MDd") # multithreaded debug dll
 else ()
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MD")  # multithreaded dll
 endif ()
 add_definitions (-DSTRSAFE_NO_DEPRECATE)
endif ()

add_library (${PROJECT_NAME} STATIC "")

if (UNIX)
 target_include_directories (${PROJECT_NAME} PUBLIC
                             $<BUILD_INTERFACE:${PKG_LIBCAP_INCLUDE_DIRS}>)

 target_include_directories (${PROJECT_NAME} PUBLIC
                             $<BUILD_INTERFACE:$ENV{LIB_ROOT}/ACE_TAO/ACE>)
#                             $<BUILD_INTERFACE:/usr/local/src/ACE_wrappers>)
elseif (WIN32)
 if (DEFINED ENV{ACE_ROOT})
  target_include_directories (${PROJECT_NAME} PUBLIC
                              $<BUILD_INTERFACE:$ENV{ACE_ROOT}>)
 else ()
  target_include_directories (${PROJECT_NAME} PUBLIC
                              $<BUILD_INTERFACE:$ENV{LIB_ROOT}/ACE_TAO/ACE>)
 endif (DEFINED ENV{ACE_ROOT})
endif ()
target_include_directories (${PROJECT_NAME} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>)
target_include_directories (${PROJECT_NAME} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/error>)
target_include_directories (${PROJECT_NAME} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/signal>)
target_include_directories (${PROJECT_NAME} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/timer>)
target_include_directories (${PROJECT_NAME} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/..>)

############## next target ################
set (source_files
     common.h
     common_configuration.h
     common_counter.cpp
     common_counter.h
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
     common_itask.h
     common_itaskcontrol.h
     common_macros.h
     common_message_queue_iterator.cpp
     common_message_queue_iterator.h
     common_message_queue_iterator.inl
     common_pragmas.h
     common_referencecounter_base.cpp
     common_referencecounter_base.h
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
     common_task.cpp
     common_task.h
     common_task.inl
     common_task_base.cpp
     common_task_base.h
     common_task_base.inl
     common_tools.cpp
     common_tools.h)
if (WIN32)
 set (source_files
      ../include/${PROJECT_NAME}_config.h
      ../include/targetver.h
      ${source_files}
      common_event.cpp
      common_event.h)
endif (WIN32)

# Include dir for installed targets
set (INCLUDE_DIR include/${PROJECT_NAME}-${upstream_version})

#set_target_properties (${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)

foreach (source_file ${source_files})
 get_filename_component (abs_source_file ${source_file} ABSOLUTE)
 list (APPEND absolute_source_files
       PRIVATE ${abs_source_file}
       )
#       $<INSTALL_INTERFACE:${INCLUDE_DIR}/${source_file}>)
endforeach ()
target_sources (${PROJECT_NAME}
                PRIVATE stdafx.cpp
                PRIVATE stdafx.h
                ${absolute_source_files})

#add_dependencies (${PROJECT_NAME} ACE)
if (UNIX)
 if (DBUS_SUPPORT)
  add_dependencies (${PROJECT_NAME} ${PROJECT_NAME}DBus)
 endif (DBUS_SUPPORT)
elseif (WIN32)
 add_dependencies (${PROJECT_NAME} ${PROJECT_NAME}Error)
endif ()
add_dependencies (${PROJECT_NAME} ${PROJECT_NAME}Timer)

#set_target_properties (${PROJECT_NAME} PROPERTIES COMPILE_DEFINITIONS ACE_HAS_DLL)
#set_target_properties (${PROJECT_NAME} PROPERTIES
#                       DEFINE_SYMBOL COMMON_BUILD_DLL)
set_target_properties (${PROJECT_NAME} PROPERTIES
                       VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_MICRO}
                       SOVERSION 1)

if (UNIX)
 if(CMAKE_COMPILER_IS_GNUCXX)
  set (CMAKE_SHARED_LINKER_FLAGS "-Wl,--as-needed")
#  set (CMAKE_STATIC_LINKER_FLAGS "-Wl,--as-needed")
 endif (CMAKE_COMPILER_IS_GNUCXX)
endif (UNIX)

if (UNIX)
 target_link_libraries (${PROJECT_NAME} pthread)
 target_link_libraries (${PROJECT_NAME} rt)
 target_link_libraries (${PROJECT_NAME} ${PKG_LIBCAP_LIBRARIES})
# target_link_libraries (${PROJECT_NAME} ${ACE_LIBRARY})
 if (DBUS_SUPPORT)
  target_link_libraries (${PROJECT_NAME} ${PROJECT_NAME}DBus)
 endif (DBUS_SUPPORT)
 target_link_libraries (${PROJECT_NAME} ${PROJECT_NAME}Signal)
elseif (WIN32)
 if (CMAKE_BUILD_TYPE STREQUAL Debug)
  target_link_libraries (${PROJECT_NAME} Msvcrtd.lib)
 else ()
  target_link_libraries (${PROJECT_NAME} Msvcrt.lib)
 endif (CMAKE_BUILD_TYPE STREQUAL Debug)
 target_link_libraries (${PROJECT_NAME} Dbghelp.lib)
 target_link_libraries (${PROJECT_NAME} Quartz.lib)
 target_link_libraries (${PROJECT_NAME} Secur32.lib)
 target_link_libraries (${PROJECT_NAME} Userenv.lib)

 target_link_libraries (${PROJECT_NAME} ${ACE_LIBRARY})
endif ()
target_link_libraries (${PROJECT_NAME} ${PROJECT_NAME}Error)

##########################################

# add PCH
set_target_properties (${PROJECT_NAME} PROPERTIES
                       COTIRE_CXX_PREFIX_HEADER_INIT "stdafx.h")
cotire (${PROJECT_NAME})

# add all targets to the build-tree export set
export (TARGETS ${PROJECT_NAME}
        FILE "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake")

############### install files ############
install (FILES
         common.h
         common_configuration.h
         common_counter.h
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
         common_itask.h
         common_itaskcontrol.h
         common_macros.h
         common_message_queue_iterator.h
         common_message_queue_iterator.inl
         common_pragmas.h
         common_referencecounter_base.h
         common_statemachine.h
         common_statemachine.inl
         common_statemachine_base.h
         common_statemachine_base.inl
         common_statistic_handler.h
         common_statistic_handler.inl
         common_string_tools.h
         common_task.h
         common_task.inl
         common_task_base.h
         common_task_base.inl
         common_tools.h
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
add_subdirectory (gl)
add_subdirectory (image)
add_subdirectory (log)
add_subdirectory (math)
add_subdirectory (parser)
add_subdirectory (signal)
add_subdirectory (timer)
add_subdirectory (ui)
add_subdirectory (xml)
