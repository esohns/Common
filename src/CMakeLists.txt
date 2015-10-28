cmake_minimum_required (VERSION 2.8)

# target system introspection (see configure.ac)
include (CheckFunctionExists)
check_function_exists (strerror HAVE_FUNC_STRERROR)
check_function_exists (getcwd HAVE_FUNC_GETCWD)
check_function_exists (gettimeofday HAVE_FUNC_GETTIMEOFDAY)
check_function_exists (memset HAVE_FUNC_MEMSET)
check_function_exists (mkdir HAVE_FUNC_MKDIR)
check_function_exists (uname HAVE_FUNC_UNAME)
check_function_exists (realpath HAVE_FUNC_REALPATH)

include (CheckLibraryExists)
check_library_exists (pthread pthread_create "" HAVE_LIB_PTHREAD)
find_library (HAVE_LIB_ACE libACE)
#add_dependencies (${PROJECT_NAME} ACE)

include (FindPkgConfig)
#pkg_check_modules (sdl)
pkg_check_modules (PKGGTK REQUIRED libpng gmodule-export-2.0 gthread-2.0 gtk+-2.0 libglade-2.0)

add_definitions (-DHAVE_CONFIG_H)
#set_property (SOURCE PROPERTY COMPILE_DEFINITIONS HAVE_CONFIG_H)

#include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../modules/ATCD/ACE)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../ATCD/ACE)
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

#if (CMAKE_COMPILER_IS_GNUCC)
#    set_source_files_properties (${libCommon_LIB_SRCS} PROPERTIES COMPILE_FLAGS "-g -Wall")
#endif ()
set_source_files_properties (${libCommon_LIB_SRCS} PROPERTIES COMPILE_DEFINITIONS COMMON_BUILD_DLL)

add_library (Common SHARED ${libCommon_LIB_SRCS})

set_target_properties (Common PROPERTIES VERSION 1.0.0 SOVERSION 1)

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
install (TARGETS
         Common
         LIBRARY DESTINATION lib)
