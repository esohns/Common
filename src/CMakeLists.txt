if (UNIX)
 option (DBUS_SUPPORT "compile DBus support" ON)
endif (UNIX)

# target system introspection (see configure.ac)
include (CheckFunctionExists)
check_function_exists (strerror HAVE_FUNC_STRERROR)
check_function_exists (getcwd HAVE_FUNC_GETCWD)
check_function_exists (memset HAVE_FUNC_MEMSET)
check_function_exists (mkdir HAVE_FUNC_MKDIR)
if (UNIX)
 check_function_exists (gettimeofday HAVE_FUNC_GETTIMEOFDAY)
 check_function_exists (realpath HAVE_FUNC_REALPATH)
 check_function_exists (uname HAVE_FUNC_UNAME)
endif (UNIX)

if (UNIX)
# include (CheckLibraryExists)
# check_library_exists (pthread pthread_create "" HAVE_LIB_PTHREAD)
 set (CMAKE_THREAD_PREFER_PTHREAD ON)
 find_package (Threads)
endif (UNIX)

if (UNIX)
 include (CheckLibraryExists)
 #*TODO*: why isn't the function found ?
 check_library_exists (rt aio_init "" HAVE_LIB_AIO)
endif (UNIX)

set (ACE_LIB_FILE libACE.so)
if (UNIX)
 set (AIO_LIB_FILE libaio.so)
 find_library (AIO_LIBRARY ${AIO_LIB_FILE}
               PATHS /lib /usr/lib
               PATH_SUFFIXES x86_64-linux-gnu
               DOC "searching for ${AIO_LIB_FILE}")
# message ("CMAKE_FIND_ROOT_PATH is ${CMAKE_FIND_ROOT_PATH}")
 message ("CMAKE_CURRENT_BINARY_DIR is ${CMAKE_CURRENT_BINARY_DIR}")
 find_library (ACE_LIBRARY ${ACE_LIB_FILE}
               PATHS ${CMAKE_CURRENT_BINARY_DIR}/../../../ATCD/ACE /usr/local
    #           HINTS ENV ACE_ROOT
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
 #message (STATUS "Found ACE library \"${ACE_LIBRARY}\"")
endif ()
add_definitions (-DACE_HAS_DLL)

include (FindPkgConfig)
if (UNIX)
 pkg_check_modules (PKG_LIBCAP REQUIRED libcap)
 if (DBUS_SUPPORT)
  pkg_check_modules (PKG_DBUS REQUIRED dbus-1)
  add_definitions (-DDBUS_SUPPORT)
 endif (DBUS_SUPPORT)
endif (UNIX)
#pkg_check_modules (PKG_ACE REQUIRED ACE)

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
endif ()

add_library (${PROJECT_NAME} STATIC "")

if (UNIX)
 target_include_directories (${PROJECT_NAME} PUBLIC
                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../modules/ACE>)

 if (DBUS_SUPPORT)
  target_include_directories (${PROJECT_NAME} PUBLIC
                              "$<BUILD_INTERFACE:${PKG_DBUS_INCLUDE_DIRS}>")
  target_include_directories (${PROJECT_NAME} PUBLIC
                              $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/dbus>)
 endif (DBUS_SUPPORT)
elseif (WIN32)
 include_directories ($ENV{ACE_ROOT})
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/err)
endif ()
target_include_directories (${PROJECT_NAME} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)
target_include_directories (${PROJECT_NAME} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/timer>)
target_include_directories (${PROJECT_NAME} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/..>)

############## next target ################
set (source_files
     common.h
     common_configuration.h
     common_defines.h
     common_event_handler.cpp
     common_event_handler.h
     common_exports.h
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
     common_iscanner.h
     common_isignal.h
     common_istatemachine.h
     common_istatistic.h
     common_isubscribe.h
     common_itask.h
     common_itaskcontrol.h
     common_logger.cpp
     common_logger.h
     common_logger.inl
     common_macros.h
     common_message_queue_iterator.cpp
     common_message_queue_iterator.h
     common_message_queue_iterator.inl
     common_pragmas.h
     common_referencecounter_base.cpp
     common_referencecounter_base.h
     common_signal_tools.cpp
     common_signal_tools.h
     common_signalhandler.cpp
     common_signalhandler.h
     common_signalhandler.inl
     common_statemachine.cpp
     common_statemachine.h
     common_statemachine.inl
     common_statemachine_base.cpp
     common_statemachine_base.h
     common_statemachine_base.inl
     common_statistic_handler.cpp
     common_statistic_handler.h
     common_statistic_handler.inl
     common_task.cpp
     common_task.h
     common_task.inl
     common_task_base.cpp
     common_task_base.h
     common_task_base.inl
     common_time_common.h
     common_tools.cpp
     common_tools.h)

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
# target_link_libraries (${PROJECT_NAME} ${AIO_LIBRARY})
 if (DBUS_SUPPORT)
#  target_link_libraries (${PROJECT_NAME} ${PKG_DBUS_LIBRARIES})
  target_link_libraries (${PROJECT_NAME} ${PROJECT_NAME}DBus)
 endif (DBUS_SUPPORT)
# target_link_libraries (${PROJECT_NAME} ${ACE_LIBRARY})
elseif (WIN32)
 if (CMAKE_BUILD_TYPE STREQUAL Debug)
  target_link_libraries (${PROJECT_NAME} Msvcrtd.lib)
 else ()
  target_link_libraries (${PROJECT_NAME} Msvcrt.lib)
 endif ()
 target_link_libraries (${PROJECT_NAME} Quartz.lib)
 target_link_libraries (${PROJECT_NAME} Secur32.lib)
 target_link_libraries (${PROJECT_NAME} Userenv.lib)

 target_link_libraries (${PROJECT_NAME} ${ACE_LIBRARY})

 target_link_libraries (${PROJECT_NAME} ${PROJECT_NAME}Error)
endif ()

# add PCH
set_target_properties (${PROJECT_NAME} PROPERTIES
                       COTIRE_CXX_PREFIX_HEADER_INIT "stdafx.h")
cotire (${PROJECT_NAME})

# add all targets to the build-tree export set
export (TARGETS ${PROJECT_NAME}
        FILE "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake")

############### install files ################
install (FILES
         common.h
         common_configuration.h
         common_defines.h
         common_event_handler.h
         common_exports.h
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
         common_iscanner.h
         common_isignal.h
         common_istatemachine.h
         common_istatistic.h
         common_isubscribe.h
         common_itask.h
         common_itaskcontrol.h
         common_logger.h
         common_logger.inl
         common_macros.h
         common_message_queue_iterator.h
         common_message_queue_iterator.inl
         common_pragmas.h
         common_referencecounter_base.h
         common_signal_tools.h
         common_signalhandler.h
         common_signalhandler.inl
         common_statemachine.h
         common_statemachine.inl
         common_statemachine_base.h
         common_statemachine_base.inl
         common_statistic_handler.h
         common_statistic_handler.inl
         common_task.h
         common_task.inl
         common_task_base.h
         common_task_base.inl
         common_time_common.h
         common_tools.h
         DESTINATION ${INSTALL_INCLUDE_DIR})
install (TARGETS ${PROJECT_NAME}
         EXPORT ${PROJECT_NAME}Targets
         ARCHIVE DESTINATION "${INSTALL_LIB_DIR}"
         COMPONENT lib)

if (DBUS_SUPPORT)
 add_subdirectory (dbus)
endif (DBUS_SUPPORT)
if (WIN32)
 add_subdirectory (err)
endif (WIN32)
add_subdirectory (gl)
add_subdirectory (image)
add_subdirectory (math)
add_subdirectory (timer)
add_subdirectory (ui)
add_subdirectory (xml)
