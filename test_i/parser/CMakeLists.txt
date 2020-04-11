﻿if (UNIX)
# pkg_check_modules (PKG_FLEX_BISON REQUIRED flex bison)
endif (UNIX)

set (ACE_LIB_FILE libACE.so)
if (DEFINED ENV{ACE_ROOT})
 find_library (ACE_LIBRARY ${ACE_LIB_FILE}
               PATHS ENV ACE_ROOT
               PATH_SUFFIXES ace build/linux/ace
               DOC "searching for ${ACE_LIB_FILE}")
else ()
 find_library (ACE_LIBRARY ${ACE_LIB_FILE}
               DOC "searching for ${ACE_LIB_FILE} (system paths)")
endif ()
#if (NOT EXISTS ACE_LIBRARY)
if (NOT ACE_LIBRARY)
 message (FATAL_ERROR "could not find ${ACE_LIB_FILE} (was: \"${ACE_LIBRARY}\"), aborting")
endif ()

get_filename_component (BUILD_PATH ${CMAKE_CURRENT_BINARY_DIR}/../.. REALPATH DIRECTORY)
get_filename_component (BUILD_PATH_SUFFIX ${BUILD_PATH} NAME)
find_package (Common ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_MICRO}
              REQUIRED COMPONENTS lib
              CONFIG
              PATHS ${CMAKE_CURRENT_SOURCE_DIR}/../modules ${CMAKE_CURRENT_SOURCE_DIR}/../..
              PATH_SUFFIXES libCommon/${BUILD_PATH_SUFFIX}
              NO_DEFAULT_PATH)
if (NOT EXISTS ${Common_DIR})
 message (FATAL_ERROR "could not find package \"Common\", aborting")
endif ()

############## definitions ###############
add_definitions (-DACE_HAS_DLL)
add_definitions (-DHAVE_CONFIG_H)
set_property (SOURCE PROPERTY COMPILE_DEFINITIONS HAVE_CONFIG_H)
add_definitions (-DCOMMON_HAS_DLL)

############## includes ###############
#include_directories (${PKG_FLEX_BISON_INCLUDE_DIRS})

include_directories ($ENV{LIB_ROOT}/ACE_TAO/ACE)
#include_directories (/usr/local/src/ACE_wrappers)

if (WIN32)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../3rd_party)
endif (WIN32)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../src)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../src/log)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../src/parser)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../src/timer)

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/..)

include_directories (${CMAKE_CURRENT_BINARY_DIR}/../..)

############## next target ###############
set (parser_EXE_SRCS
     bencoding_iparser.h
     bencoding_parser.cpp
     bencoding_parser.h
     bencoding_parser_driver.cpp
     bencoding_parser_driver.h
     bencoding_scanner.cpp
     bencoding_scanner.h
     stdafx.cpp
     stdafx.h
#     ../test_i_common.h
#     ../test_i_defines.h
     test_i.cpp)

set (parser_2_EXE_SRCS
     bencoding_iparser.h
     bencoding_parser.cpp
     bencoding_parser.h
     bencoding_parser_driver.cpp
     bencoding_parser_driver.h
     bencoding_scanner.cpp
     bencoding_scanner.h
     stdafx.cpp
     stdafx.h
#     ../test_i_common.h
#     ../test_i_defines.h
     test_i_2.cpp)

#cmake_policy (SET CMP0065 OLD) # do not add flags to export symbols from executables without the ENABLE_EXPORTS target property
set (EXECUTABLE parser)
set (EXECUTABLE_2 parser_2)
add_executable (${EXECUTABLE}
                EXCLUDE_FROM_ALL
                ${parser_EXE_SRCS})
add_executable (${EXECUTABLE_2}
                EXCLUDE_FROM_ALL
                ${parser_2_EXE_SRCS})
set_property (TARGET ${EXECUTABLE} PROPERTY FOLDER "test_i")
set_property (TARGET ${EXECUTABLE_2} PROPERTY FOLDER "test_i")

#target_link_libraries (${EXECUTABLE} ${PKG_FLEX_BISON_LIBRARIES})

target_link_libraries (${EXECUTABLE} ${ACE_LIBRARY})

target_link_libraries (${EXECUTABLE} Common)
target_link_libraries (${EXECUTABLE} CommonLog)
target_link_libraries (${EXECUTABLE} CommonParser)
target_link_libraries (${EXECUTABLE} CommonTimer)

target_link_libraries (${EXECUTABLE_2} ${ACE_LIBRARY})

target_link_libraries (${EXECUTABLE_2} Common)
target_link_libraries (${EXECUTABLE_2} CommonLog)
target_link_libraries (${EXECUTABLE_2} CommonParser)
target_link_libraries (${EXECUTABLE_2} CommonTimer)

# add PCH
#set_target_properties (${EXECUTABLE} PROPERTIES COTIRE_CXX_PREFIX_HEADER_INIT "stdafx.h")
#cotire (${EXECUTABLE})
#set_target_properties (${EXECUTABLE_2} PROPERTIES COTIRE_CXX_PREFIX_HEADER_INIT "stdafx.h")
#cotire (${EXECUTABLE_2})

############### installation #############
install (TARGETS ${EXECUTABLE}
         EXPORT ${PROJECT_NAME}Targets
         RUNTIME DESTINATION ${INSTALL_BIN_DIR} COMPONENT test_i)
install (TARGETS ${EXECUTABLE_2}
         EXPORT ${PROJECT_NAME}Targets
         RUNTIME DESTINATION ${INSTALL_BIN_DIR} COMPONENT test_i)

# install (FILES etc/${EXECUTABLE}.glade
#          DESTINATION "${INSTALL_SHARE_DIR}/${PROJECT_NAME}/${EXECUTABLE}" COMPONENT config)

################ launchers ###############
file (TO_CMAKE_PATH $ENV{LIB_ROOT}/ACE_TAO/ACE/lib ACE_LIB_PATH)
set (RUNTIME_LIBRARY_DIRS ${RUNTIME_LIBRARY_DIRS}
                          ${ACE_LIB_PATH})
create_target_launcher (${EXECUTABLE}
                        ARGS "-d -e -f test.txt -l -t"
#                         FORWARD_ARGS
                        RUNTIME_LIBRARY_DIRS ${RUNTIME_LIBRARY_DIRS}
                                             ${PROJECT_SOURCE_DIR}/cmake/src/Debug
                        WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/../test_i/parser
                        ENVIRONMENT foo=bar)
create_target_launcher (${EXECUTABLE_2}
                        ARGS "-d -e -f test.txt -l -t"
#                         FORWARD_ARGS
                        RUNTIME_LIBRARY_DIRS ${RUNTIME_LIBRARY_DIRS}
                                             ${PROJECT_SOURCE_DIR}/cmake/src/Debug
                        WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/../test_i/parser
                        ENVIRONMENT foo=bar)

################ tests ###################
add_test (NAME ${EXECUTABLE}_test
          COMMAND ${PROJECT_BINARY_DIR}/${EXECUTABLE}
          WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
set_tests_properties (${EXECUTABLE}_test
                      PROPERTIES
                      ENVIRONMENT "PATH=D:\\projects\\ATCD\\ACE\\lib;D:\\projects\\gtk\\bin;D:\\projects\\libglade\\bin;D:\\projects\\libxml2-2.9.1\\.libs;G:\\software\\Development\\dll;${PROJECT_SOURCE_DIR}\\..\\libCommon\\cmake\\src\\Debug;${PROJECT_SOURCE_DIR}\\..\\libCommon\\cmake\\src\\ui\\Debug;${PROJECT_SOURCE_DIR}\\cmake\\src\\Debug;${PROJECT_SOURCE_DIR}\\cmake\\src\\modules\\dev\\Debug;${PROJECT_SOURCE_DIR}\\..\\libACENetwork\\cmake\\src\\Debug;%PATH%")
add_test (NAME ${EXECUTABLE_2}_test
          COMMAND ${PROJECT_BINARY_DIR}/${EXECUTABLE_2}
          WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
set_tests_properties (${EXECUTABLE_2}_test
                      PROPERTIES
                      ENVIRONMENT "PATH=D:\\projects\\ATCD\\ACE\\lib;D:\\projects\\gtk\\bin;D:\\projects\\libglade\\bin;D:\\projects\\libxml2-2.9.1\\.libs;G:\\software\\Development\\dll;${PROJECT_SOURCE_DIR}\\..\\libCommon\\cmake\\src\\Debug;${PROJECT_SOURCE_DIR}\\..\\libCommon\\cmake\\src\\ui\\Debug;${PROJECT_SOURCE_DIR}\\cmake\\src\\Debug;${PROJECT_SOURCE_DIR}\\cmake\\src\\modules\\dev\\Debug;${PROJECT_SOURCE_DIR}\\..\\libACENetwork\\cmake\\src\\Debug;%PATH%")
#PATH=%PATH%;$(ProjectDir)..\..\..\..\ATCD\ACE\lib;$(ProjectDir)..\..\..\..\gtk3\bin;$(ProjectDir)..\..\..\..\libav-11.7\libavcodec;$(ProjectDir)..\..\..\..\libav-11.7\libavresample;$(ProjectDir)..\..\..\..\libav-11.7\libavutil;$(ProjectDir)..\..\..\..\libCommon\cmake\src\Debug;$(ProjectDir)..\..\..\..\libCommon\cmake\src\err\Debug;$(ProjectDir)..\..\..\..\libCommon\cmake\src\ui\Debug;$(ProjectDir)..\..\src\Debug;$(ProjectDir)..\..\src\modules\dev\Debug;$(ProjectDir)..\..\src\modules\vis\Debug