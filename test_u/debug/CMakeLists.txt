if (UNIX)
 include (CheckLibraryExists)
 check_library_exists (pthread pthread_create "" HAVE_LIB_PTHREAD)
 set (CMAKE_THREAD_PREFER_PTHREAD ON)
 find_package (Threads)
endif (UNIX)

if (UNIX)
 find_library (CAP_LIBRARY libcap.so.2
               DOC "searching for libcap")
 if (NOT CAP_LIBRARY)
  message (FATAL_ERROR "could not find libcap, aborting")
 endif ()
# pkg_check_modules (PKG_CAP REQUIRED libcap)
endif ()

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

 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../src)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../src/log)
# include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../src/timer)
# include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../modules/Common/src/ui)
# include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../modules/Common/src/ui/gtk)
elseif (WIN32)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../ATCD/ACE)
# include_directories ($ENV{ACE_ROOT})

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src/timer)
# include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src/ui)
# include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src/ui/gtk)
endif ()
#include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../src)
#include_directories (${CMAKE_CURRENT_BINARY_DIR}/../..)
#include_directories (${CMAKE_CURRENT_SOURCE_DIR}/..)

############## next target ################
set (debug_wrapper_EXE_SRCS
     stdafx.cpp
     stdafx.h
#     ../test_u_common.h
#     ../test_u_defines.h
#     targetver.h
     test_u.cpp)

set (EXECUTABLE debug_wrapper)
if (WIN32)
 generate_product_version (VERSION_FILES
                           NAME ${EXECUTABLE}
                           BUNDLE ${PROJECT_NAME}
#                           ICON "${CMAKE_SOURCE_DIR}/product.ico"
                           ICON "${CMAKE_CURRENT_SOURCE_DIR}/../../include/libcommon_icon.ico"
                           VERSION_MAJOR ${VERSION_MAJOR}
                           VERSION_MINOR ${VERSION_MINOR}
                           VERSION_PATCH ${VERSION_MICRO}
#                           VERSION_REVISION ${VERSION_DEVEL}
                           VERSION_REVISION 0
                           COMPANY_NAME ""
#                           COMPANY_COPYRIGHT "${COMPANY_NAME} (C) Copyright ${CURRENT_YEAR}"
                           COMPANY_COPYRIGHT ""
                           COMMENTS ${PACKAGE_DESCRIPTION_SUMMARY}
                           ORIGINAL_FILENAME ""
                           INTERNAL_NAME ""
                           FILE_DESCRIPTION ${PACKAGE_DESCRIPTION})
endif (WIN32)

#cmake_policy (SET CMP0065 OLD) # do not add flags to export symbols from executables without the ENABLE_EXPORTS target property
if (UNIX)
 add_executable (${EXECUTABLE}
                 EXCLUDE_FROM_ALL
                 ${debug_wrapper_EXE_SRCS})
elseif (WIN32)
 add_executable (${EXECUTABLE}
                 EXCLUDE_FROM_ALL
                 ${debug_wrapper_EXE_SRCS}
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
target_link_libraries (${EXECUTABLE} CommonLog)
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
                         ARGS "-f ../../../ardrone/cmake/src/ardrone -l -t"
#                         FORWARD_ARGS
                         RUNTIME_LIBRARY_DIRS ${PROJECT_SOURCE_DIR}/modules/ATCD/ACE/lib
#                                              ${PROJECT_SOURCE_DIR}/../gtk/gtk/Win32/debug/bin
#                                              ${PROJECT_SOURCE_DIR}/../libCommon/cmake/src/Debug
#                                              ${PROJECT_SOURCE_DIR}/../libCommon/cmake/src/ui/Debug
                                              ${PROJECT_SOURCE_DIR}/cmake/src/Debug
#                                              ${PROJECT_SOURCE_DIR}/cmake/src/modules/dev/Debug
                         WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/../../../ardrone/src
                         ENVIRONMENT foo=bar)
elseif (WIN32)
# if (GTK3_SUPPORT)
#  create_target_launcher (${EXECUTABLE}
#                          ARGS "-g.\\\\etc\\\\filecopy.glade -l -t"
#                        FORWARD_ARGS
#                          RUNTIME_LIBRARY_DIRS ${PROJECT_SOURCE_DIR}/../ATCD/ACE/lib
#                                               ${PROJECT_SOURCE_DIR}/../gtk/gtk/Win32/debug/bin
#                                               G:/software/Development/dll
#                                               ${PROJECT_SOURCE_DIR}/../libCommon/cmake/src/Debug
#                                               ${PROJECT_SOURCE_DIR}/../libCommon/cmake/src/err/Debug
#                                               ${PROJECT_SOURCE_DIR}/../libCommon/cmake/src/ui/Debug
#                                               ${PROJECT_SOURCE_DIR}/cmake/src/Debug
#                                               ${PROJECT_SOURCE_DIR}/cmake/src/modules/dec/Debug
#                                               ${PROJECT_SOURCE_DIR}/cmake/src/modules/dev/Debug
#                                               ${PROJECT_SOURCE_DIR}/cmake/src/modules/vis/Debug
#                          WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/../test_u/filecopy
#                          ENVIRONMENT foo=bar)
# else ()
#  create_target_launcher (${EXECUTABLE}
#                          ARGS "-g.\\\\etc\\\\filecopy.glade -l -t"
#                        FORWARD_ARGS
#                          RUNTIME_LIBRARY_DIRS ${PROJECT_SOURCE_DIR}/../ATCD/ACE/lib
#                                               ${PROJECT_SOURCE_DIR}/../gtk2/bin
#                                               G:/software/Development/dll
#                                               ${PROJECT_SOURCE_DIR}/../libCommon/cmake/src/Debug
#                                               ${PROJECT_SOURCE_DIR}/../libCommon/cmake/src/err/Debug
#                                               ${PROJECT_SOURCE_DIR}/../libCommon/cmake/src/ui/Debug
#                                               ${PROJECT_SOURCE_DIR}/cmake/src/Debug
#                                               ${PROJECT_SOURCE_DIR}/cmake/src/modules/dec/Debug
#                                               ${PROJECT_SOURCE_DIR}/cmake/src/modules/dev/Debug
#                                               ${PROJECT_SOURCE_DIR}/cmake/src/modules/vis/Debug
#                          WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/../test_u/filecopy
#                          ENVIRONMENT foo=bar)
# endif ()
endif ()

add_test (NAME ${EXECUTABLE}_test
          COMMAND ${PROJECT_BINARY_DIR}/${EXECUTABLE} -f ../../../ardrone/cmake/src/ardrone -l -t
          WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/../../../ardrone/src)

set_tests_properties (${EXECUTABLE}_test
                      PROPERTIES
                      ENVIRONMENT "PATH=D:\\projects\\ATCD\\ACE\\lib;D:\\projects\\gtk\\bin;D:\\projects\\libglade\\bin;D:\\projects\\libxml2-2.9.1\\.libs;G:\\software\\Development\\dll;${PROJECT_SOURCE_DIR}\\..\\libCommon\\cmake\\src\\Debug;${PROJECT_SOURCE_DIR}\\..\\libCommon\\cmake\\src\\ui\\Debug;${PROJECT_SOURCE_DIR}\\cmake\\src\\Debug;${PROJECT_SOURCE_DIR}\\cmake\\src\\modules\\dev\\Debug;${PROJECT_SOURCE_DIR}\\..\\libACENetwork\\cmake\\src\\Debug;%PATH%")
#PATH=%PATH%;$(ProjectDir)..\..\..\..\ATCD\ACE\lib;$(ProjectDir)..\..\..\..\gtk3\bin;$(ProjectDir)..\..\..\..\libav-11.7\libavcodec;$(ProjectDir)..\..\..\..\libav-11.7\libavresample;$(ProjectDir)..\..\..\..\libav-11.7\libavutil;$(ProjectDir)..\..\..\..\libCommon\cmake\src\Debug;$(ProjectDir)..\..\..\..\libCommon\cmake\src\err\Debug;$(ProjectDir)..\..\..\..\libCommon\cmake\src\ui\Debug;$(ProjectDir)..\..\src\Debug;$(ProjectDir)..\..\src\modules\dev\Debug;$(ProjectDir)..\..\src\modules\vis\Debug
