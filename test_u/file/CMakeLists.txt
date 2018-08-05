#option (GTK3_SUPPORT "compile Gtk3 support" OFF)

if (UNIX)
 include (CheckLibraryExists)
 check_library_exists (pthread pthread_create "" HAVE_LIB_PTHREAD)
 set (CMAKE_THREAD_PREFER_PTHREAD ON)
 find_package (Threads)
endif (UNIX)

#set (ACE_LIB_FILE libACE.so)
#if (UNIX)
# if (DEFINED ENV{ACE_ROOT})
#  find_library (ACE_LIBRARY ${ACE_LIB_FILE}
#                PATHS ENV ACE_ROOT
#                PATH_SUFFIXES ace build/linux/ace
#                DOC "searching for ${ACE_LIB_FILE}")
# else ()
#  find_library (ACE_LIBRARY ${ACE_LIB_FILE}
#                DOC "searching for ${ACE_LIB_FILE} (system paths)")
# endif ()
#elseif (WIN32)
# if (CMAKE_BUILD_TYPE STREQUAL Debug)
#  set (LIB_FILE_SUFFIX d)
# endif ()
# unset (ACE_LIB_FILE)
# set (ACE_LIB_FILE ACE${LIB_FILE_SUFFIX}.lib)
# find_library (ACE_LIBRARY ${ACE_LIB_FILE}
#               PATHS ENV ACE_ROOT
#               PATH_SUFFIXES lib lib\\${CMAKE_BUILD_TYPE}\\Win32
#               DOC "searching for ${ACE_LIB_FILE}")
#endif ()
##if (NOT EXISTS ACE_LIBRARY)
#if (NOT ACE_LIBRARY)
# message (FATAL_ERROR "could not find ${ACE_LIB_FILE} (was: \"${ACE_LIBRARY}\"), aborting")
#else ()
# #message (STATUS "Found ACE library \"${ACE_LIBRARY}\"")
#endif ()
#add_definitions (-DACE_HAS_DLL)

include (FindPkgConfig)
#pkg_check_modules (PKG_ACE REQUIRED ACE)
if (UNIX)
# *NOTE*: libCommon requires libcap
 pkg_check_modules (PKG_CAP REQUIRED libcap)
# if (GTK3_SUPPORT)
#  pkg_check_modules (PKG_GTK3 REQUIRED gtk+-3.0)
# else ()
## pkg_check_modules (PKG_GTK_GLADE REQUIRED libpng gmodule-2.0 gthread-2.0 gtk+-2.0 libglade-2.0)
#  pkg_check_modules (PKG_GTK2 REQUIRED libpng gmodule-2.0 gthread-2.0 gtk+-2.0)
# endif ()
elseif (WIN32)
# if (GTK3_SUPPORT)
## *TODO*: repair pkg-config support
##  pkg_check_modules (PKG_GTK3 REQUIRED gtk+-3.0)
# else ()
## pkg_check_modules (PKG_GTK_GLADE REQUIRED libpng gmodule-2.0 gthread-2.0 gtk+-2.0 libglade-2.0)
##  pkg_check_modules (PKG_GTK2 REQUIRED libpng gmodule-2.0 gthread-2.0 gtk+-2.0)
# endif ()
endif ()
# *TODO*: repair WIN32 pkgconfig support

#get_filename_component (BUILD_PATH ${CMAKE_CURRENT_BINARY_DIR}/../.. REALPATH DIRECTORY)
#get_filename_component (BUILD_PATH_SUFFIX ${BUILD_PATH} NAME)
#find_package (Common ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_MICRO}
#              REQUIRED
#              CONFIG
#              PATHS ${CMAKE_CURRENT_SOURCE_DIR}/../modules ${CMAKE_CURRENT_SOURCE_DIR}/../..
#              PATH_SUFFIXES libCommon/${BUILD_PATH_SUFFIX}
#              NO_DEFAULT_PATH)
#if (NOT EXISTS ${Common_DIR})
# message (FATAL_ERROR "could not find package \"Common\", aborting")
#endif ()
#add_definitions (-DCOMMON_HAS_DLL)

#add_definitions (-DHAVE_CONFIG_H)
#set_property (SOURCE PROPERTY COMPILE_DEFINITIONS HAVE_CONFIG_H)

if (UNIX)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../modules/ACE)
# include_directories (${PKG_CAP_INCLUDE_DIRS})
# if (GTK3_SUPPORT)
#  include_directories (${PKG_GTK3_INCLUDE_DIRS})
# else ()
#  include_directories (${PKG_GTK2_INCLUDE_DIRS})
# endif ()
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../src)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../src/timer)
# include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../modules/libCommon/src/ui)
# include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../modules/libCommon/src/ui/gtk)
elseif (WIN32)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../ATCD/ACE)
# include_directories ($ENV{ACE_ROOT})
# *TODO*: repair pkgconfig support
# gtk
# if (GTK3_SUPPORT)
#  include_directories (${PKG_GTK3_INCLUDE_DIRS})
# else ()
#  include_directories (${PKG_GTK2_INCLUDE_DIRS})
# endif ()

 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../libCommon/src)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../libCommon/src/timer)
# include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../libCommon/src/ui)
# include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../libCommon/src/ui/gtk)
endif ()
#include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../src)
#include_directories (${CMAKE_CURRENT_BINARY_DIR}/../..)
#include_directories (${CMAKE_CURRENT_SOURCE_DIR}/..)

############## next target ################
set (file_EXE_SRCS
     stdafx.cpp
     stdafx.h
#     ../test_u_common.h
#     ../test_u_defines.h
#     targetver.h
     test_u.cpp)

set (EXECUTABLE file)
if (WIN32)
# message (STATUS "MSVC_VERSION: ${MSVC_VERSION}")
 if (MSVC AND (NOT MSVC_VERSION LESS 1700))
  generate_product_version (VERSION_FILES
                            NAME ${EXECUTABLE}
                            BUNDLE ${PROJECT_NAME}
#                          ICON "${CMAKE_SOURCE_DIR}/product.ico"
                            ICON "${CMAKE_CURRENT_SOURCE_DIR}/../../include/libcommon_icon.ico"
                            VERSION_MAJOR ${VERSION_MAJOR}
                            VERSION_MINOR ${VERSION_MINOR}
                            VERSION_PATCH ${VERSION_MICRO}
#                          VERSION_REVISION ${VERSION_DEVEL}
                            VERSION_REVISION 0
                            COMPANY_NAME ""
#                          COMPANY_COPYRIGHT "${COMPANY_NAME} (C) Copyright ${CURRENT_YEAR}"
                            COMPANY_COPYRIGHT ""
                            COMMENTS ${PACKAGE_DESCRIPTION_SUMMARY}
                            ORIGINAL_FILENAME ""
                            INTERNAL_NAME ""
                            FILE_DESCRIPTION ${PACKAGE_DESCRIPTION})
 endif ()
endif (WIN32)

#cmake_policy (SET CMP0065 OLD) # do not add flags to export symbols from executables without the ENABLE_EXPORTS target property
if (UNIX)
 add_executable (${EXECUTABLE}
                 EXCLUDE_FROM_ALL
                 ${file_EXE_SRCS})
elseif (WIN32)
 add_executable (${EXECUTABLE}
                 EXCLUDE_FROM_ALL
                 ${file_EXE_SRCS}
                 ${VERSION_FILES})
endif ()
set_property (TARGET ${EXECUTABLE} PROPERTY FOLDER "test_u")
#cmake_policy (SET CMP0012 NEW)
#if (WIN32 AND NOT BUILD_TEST_U)
# set_target_properties (${EXECUTABLE} PROPERTIES EXCLUDE_FROM_DEFAULT_BUILD TRUE)
#endif ()

#add_dependencies (${ACESTREAM_TARGET_NAME} ACE)
#add_dependencies (${PROJECT_NAME} Common)

#message (STATUS "CMAKE_THREAD_LIBS_INIT: \"${CMAKE_THREAD_LIBS_INIT}\"")
#target_link_libraries (${EXECUTABLE} ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries (${EXECUTABLE} ${ACE_LIBRARY})
if (UNIX)
 target_link_libraries (${EXECUTABLE} rt)
 target_link_libraries (${EXECUTABLE} pthread)
 target_link_libraries (${EXECUTABLE} ${PKG_CAP_LIBRARIES})
# target_link_libraries (${EXECUTABLE} X11)
# if (GTK3_SUPPORT)
#  target_link_libraries (${EXECUTABLE} ${PKG_GTK3_LIBRARIES})
# else ()
#  target_link_libraries (${EXECUTABLE} ${PKG_GTK2_LIBRARIES})
# endif ()
elseif (WIN32)
# target_link_libraries (${EXECUTABLE} Advapi32.lib)
# target_link_libraries (${EXECUTABLE} Ole32.lib)
# target_link_libraries (${EXECUTABLE} Quartz.lib)
# target_link_libraries (${EXECUTABLE} Shell32.lib)
# target_link_libraries (${EXECUTABLE} ucrtd.lib)
# target_link_libraries (${EXECUTABLE} User32.lib)
# target_link_libraries (${EXECUTABLE} Userenv.lib)
endif ()
target_link_libraries (${EXECUTABLE} Common)
#if (WIN32)
 # *WARNING*: linking CommonUI requires "/FORCE:MULTIPLE""
# set_target_properties (${EXECUTABLE} PROPERTIES LINK_FLAGS /FORCE:MULTIPLE)
#endif ()
target_link_libraries (${EXECUTABLE} CommonTimer)
#target_link_libraries (${EXECUTABLE} CommonUI)
#target_link_libraries (${EXECUTABLE} CommonUI_GTK)

if (UNIX)
 target_link_libraries (${EXECUTABLE} pthread)
endif (UNIX)

# add PCH
set_target_properties (${EXECUTABLE} PROPERTIES COTIRE_CXX_PREFIX_HEADER_INIT "stdafx.h")
cotire (${EXECUTABLE})

install (TARGETS ${EXECUTABLE}
         EXPORT ${PROJECT_NAME}Targets
         RUNTIME DESTINATION ${INSTALL_BIN_DIR} COMPONENT test_u)
if (UNIX)
# install (FILES etc/${EXECUTABLE}.glade
#          DESTINATION "${INSTALL_SHARE_DIR}/${PROJECT_NAME}/${EXECUTABLE}" COMPONENT config)
elseif (WIN32)
# install (FILES etc/${EXECUTABLE}.glade
#          DESTINATION "data/${PROJECT_NAME}/${EXECUTABLE}" COMPONENT config)
#          DESTINATION "$ENV{APPDATA}/${PROJECT_NAME}/${EXECUTABLE}" COMPONENT config)
endif ()

if (UNIX)
 create_target_launcher (${EXECUTABLE}
                         ARGS ""
#                         FORWARD_ARGS
                         RUNTIME_LIBRARY_DIRS ${PROJECT_SOURCE_DIR}/modules/ATCD/ACE/lib
#                                              ${PROJECT_SOURCE_DIR}/../gtk/gtk/Win32/debug/bin
#                                              ${PROJECT_SOURCE_DIR}/../libCommon/cmake/src/Debug
#                                              ${PROJECT_SOURCE_DIR}/../libCommon/cmake/src/ui/Debug
                                              ${PROJECT_SOURCE_DIR}/cmake/src/Debug
#                                              ${PROJECT_SOURCE_DIR}/cmake/src/modules/dev/Debug
                         WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/../test_u/file
                         ENVIRONMENT foo=bar)
elseif (WIN32)
 create_target_launcher (${EXECUTABLE}
                         ARGS "-t"
#                        FORWARD_ARGS
                         RUNTIME_LIBRARY_DIRS ${PROJECT_SOURCE_DIR}/../ATCD/ACE/lib
                                               ${PROJECT_SOURCE_DIR}/../gtk/gtk/Win32/debug/bin
                                               G:/software/Development/dll
                                               ${PROJECT_SOURCE_DIR}/../libCommon/cmake/src/Debug
                                               ${PROJECT_SOURCE_DIR}/../libCommon/cmake/src/err/Debug
                                               ${PROJECT_SOURCE_DIR}/../libCommon/cmake/src/ui/Debug
                                               ${PROJECT_SOURCE_DIR}/cmake/src/Debug
                                               ${PROJECT_SOURCE_DIR}/cmake/src/modules/dec/Debug
                                               ${PROJECT_SOURCE_DIR}/cmake/src/modules/dev/Debug
                                               ${PROJECT_SOURCE_DIR}/cmake/src/modules/vis/Debug
                          WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/../test_u/file
                          ENVIRONMENT foo=bar)
endif ()

add_test (NAME ${EXECUTABLE}_test
          COMMAND ${PROJECT_BINARY_DIR}/${EXECUTABLE} -l -t
          WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})

set_tests_properties (${EXECUTABLE}_test
                      PROPERTIES
                      ENVIRONMENT "PATH=D:\\projects\\ATCD\\ACE\\lib;D:\\projects\\gtk\\bin;D:\\projects\\libglade\\bin;D:\\projects\\libxml2-2.9.1\\.libs;G:\\software\\Development\\dll;${PROJECT_SOURCE_DIR}\\..\\libCommon\\cmake\\src\\Debug;${PROJECT_SOURCE_DIR}\\..\\libCommon\\cmake\\src\\ui\\Debug;${PROJECT_SOURCE_DIR}\\cmake\\src\\Debug;${PROJECT_SOURCE_DIR}\\cmake\\src\\modules\\dev\\Debug;${PROJECT_SOURCE_DIR}\\..\\libACENetwork\\cmake\\src\\Debug;%PATH%")
#PATH=%PATH%;$(ProjectDir)..\..\..\..\ATCD\ACE\lib;$(ProjectDir)..\..\..\..\gtk3\bin;$(ProjectDir)..\..\..\..\libav-11.7\libavcodec;$(ProjectDir)..\..\..\..\libav-11.7\libavresample;$(ProjectDir)..\..\..\..\libav-11.7\libavutil;$(ProjectDir)..\..\..\..\libCommon\cmake\src\Debug;$(ProjectDir)..\..\..\..\libCommon\cmake\src\err\Debug;$(ProjectDir)..\..\..\..\libCommon\cmake\src\ui\Debug;$(ProjectDir)..\..\src\Debug;$(ProjectDir)..\..\src\modules\dev\Debug;$(ProjectDir)..\..\src\modules\vis\Debug
