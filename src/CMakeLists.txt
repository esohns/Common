# target system introspection (see configure.ac)
include (CheckFunctionExists)
check_function_exists (strerror HAVE_FUNC_STRERROR)
check_function_exists (getcwd HAVE_FUNC_GETCWD)
if (UNIX)
 check_function_exists (gettimeofday HAVE_FUNC_GETTIMEOFDAY)
endif ()
check_function_exists (memset HAVE_FUNC_MEMSET)
check_function_exists (mkdir HAVE_FUNC_MKDIR)
if (UNIX)
 check_function_exists (uname HAVE_FUNC_UNAME)
endif ()
if (UNIX)
 check_function_exists (realpath HAVE_FUNC_REALPATH)
endif ()

if (UNIX)
 include (CheckLibraryExists)
 check_library_exists (pthread pthread_create "" HAVE_LIB_PTHREAD)
endif ()

set (ACE_LIB_FILE libACE.so)
if (UNIX)
 if (DEFINED ENV{ACE_ROOT})
  find_library (ACE_LIBRARY ${ACE_LIB_FILE}
                PATHS ENV ACE_ROOT
                PATH_SUFFIXES ace build/linux/ace
                DOC "searching for ${ACE_LIB_FILE}"
                NO_DEFAULT_PATH)
 else ()
  find_library (ACE_LIBRARY ${ACE_LIB_FILE}
                DOC "searching for ${ACE_LIB_FILE} (system paths)")
 endif ()
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
 pkg_check_modules (PKG_LIBCAP REQUIRED libcap)
endif ()
#pkg_check_modules (PKG_ACE REQUIRED ACE)

add_definitions (-DHAVE_CONFIG_H)
#set_property (SOURCE PROPERTY COMPILE_DEFINITIONS HAVE_CONFIG_H)

if (UNIX)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../modules/ACE)
elseif (WIN32)
 include_directories ($ENV{ACE_ROOT})
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/err)
endif ()
include_directories (${CMAKE_CURRENT_BINARY_DIR}/..)

############## next target ################
set (${PROJECT_NAME}_LIB_SRCS
     common.h
     common_defines.h
     common_exports.h
     common_file_tools.cpp
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
     common_isignal.h
     common_istatemachine.h
     common_istatistic.h
     common_isubscribe.h
     common_itimer.h
     common_itimerhandler.h
     common_logger.cpp
     common_logger.h
     common_macros.h
     common_referencecounter_base.cpp
     common_referencecounter_base.h
     common_signalhandler.cpp
     common_signalhandler.h
     common_statemachine_base.cpp
     common_statemachine_base.h
     common_statemachine_base.inl
     common_task_base.cpp
     common_task_base.h
     common_task_base.inl
     common_time_common.h
     common_timer_common.h
     common_timerhandler.cpp
     common_timerhandler.h
     common_timer_manager.cpp
     common_timer_manager.h
     common_timer_manager.inl
     common_timer_manager_common.h
     common_tools.cpp
     common_tools.h
     stdafx.cpp
     stdafx.h)

if (WIN32)
 add_definitions (-DWIN32)
 set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc") # C++ exception handling
 if (CMAKE_BUILD_TYPE STREQUAL Debug)
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MDd") # multithreaded debug dll
 else ()
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MD")  # multithreaded dll
 endif ()
endif ()

add_library (${PROJECT_NAME} SHARED ${Common_LIB_SRCS})

#add_dependencies (${PROJECT_NAME} ACE)

set_target_properties (${PROJECT_NAME} PROPERTIES COMPILE_DEFINITIONS ACE_HAS_DLL)
set_target_properties (${PROJECT_NAME} PROPERTIES DEFINE_SYMBOL COMMON_BUILD_DLL)
set_target_properties (${PROJECT_NAME} PROPERTIES VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_MICRO} SOVERSION 1)

if (WIN32)
 if (CMAKE_BUILD_TYPE STREQUAL Debug)
  target_link_libraries (${PROJECT_NAME} Msvcrtd.lib)
 else ()
  target_link_libraries (${PROJECT_NAME} Msvcrt.lib)
 endif ()
 target_link_libraries (${PROJECT_NAME} Secur32.lib)
 target_link_libraries (${PROJECT_NAME} Userenv.lib)

 target_link_libraries (${PROJECT_NAME} ${ACE_LIBRARY})

 target_link_libraries (${PROJECT_NAME} ${PROJECT_NAME}ERR)
endif ()

# add all targets to the build-tree export set
export (TARGETS ${PROJECT_NAME}
        FILE "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake")

############### install files ################
install (FILES
         common.h
         common_defines.h
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
         common_isignal.h
         common_istatemachine.h
         common_istatistic.h
         common_isubscribe.h
         common_itimer.h
         common_logger.h
         common_macros.h
         common_referencecounter_base.h
         common_signalhandler.cpp
         common_signalhandler.h
         common_statemachine_base.h
         common_statemachine_base.inl
         common_task_base.h
         common_task_base.inl
         common_time_common.h
         common_timer_common.h
         common_timerhandler.h
         common_timer_manager.h
         common_timer_manager.inl
         common_timer_manager_common.h
         common_tools.h
         DESTINATION include)
install (TARGETS ${PROJECT_NAME}
         EXPORT ${PROJECT_NAME}Targets
         ARCHIVE DESTINATION "${INSTALL_LIB_DIR}" COMPONENT lib
         LIBRARY DESTINATION "${INSTALL_LIB_DIR}" COMPONENT shlib
         RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin
         COMPONENT dev)

if (WIN32)
 add_subdirectory (err)
endif ()
add_subdirectory (ui)
