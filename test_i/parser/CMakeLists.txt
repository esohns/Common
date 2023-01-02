if (UNIX)
 pkg_check_modules (PKG_FLEX_BISON QUIET flex bison)
endif (UNIX)

include (iconv)
set (CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/../../src/xml/scripts")
include (libxml2)

string (REPLACE ${CMAKE_SOURCE_DIR} "" BUILD_PATH_SUFFIX ${CMAKE_BINARY_DIR})
string (SUBSTRING ${BUILD_PATH_SUFFIX} 1 -1 BUILD_PATH_SUFFIX)

############## definitions ###############

############## includes ###############
if (UNIX)
 include_directories (${PKG_FLEX_BISON_INCLUDE_DIRS})
endif (UNIX)
if (LIBXML2_SUPPORT)
 include_directories (${LIBXML2_INCLUDE_DIRS})
endif (LIBXML2_SUPPORT)
if (WIN32)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../3rd_party/msvc)
endif (WIN32)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../src)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../src/log)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../src/parser)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../src/parser/bencoding)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../src/parser/m3u)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../src/timer)

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/..)

include_directories (${CMAKE_CURRENT_BINARY_DIR}/../..)

############## next target ###############
set (parser_EXE_SRCS
#     stdafx.cpp
     stdafx.h
#     ../test_i_common.h
#     ../test_i_defines.h
     test_i.cpp)

set (parser_2_EXE_SRCS
#     stdafx.cpp
     stdafx.h
#     ../test_i_common.h
#     ../test_i_defines.h
     test_i_2.cpp)

set (parser_3_EXE_SRCS
#     stdafx.cpp
     stdafx.h
#     ../test_i_common.h
#     ../test_i_defines.h
     test_i_3.cpp)

set (parser_4_EXE_SRCS
#     stdafx.cpp
     stdafx.h
#     ../test_i_common.h
#     ../test_i_defines.h
     test_i_4.cpp)

#cmake_policy (SET CMP0065 OLD) # do not add flags to export symbols from executables without the ENABLE_EXPORTS target property
set (EXECUTABLE parser)
set (EXECUTABLE_2 parser_2)
set (EXECUTABLE_3 parser_3)
set (EXECUTABLE_4 parser_4)
add_executable (${EXECUTABLE}
                EXCLUDE_FROM_ALL
                ${parser_EXE_SRCS})
add_executable (${EXECUTABLE_2}
                EXCLUDE_FROM_ALL
                ${parser_2_EXE_SRCS})
add_executable (${EXECUTABLE_3}
                EXCLUDE_FROM_ALL
                ${parser_3_EXE_SRCS})
add_executable (${EXECUTABLE_4}
                EXCLUDE_FROM_ALL
                ${parser_4_EXE_SRCS})
set_property (TARGET ${EXECUTABLE} PROPERTY FOLDER "test_i")
set_property (TARGET ${EXECUTABLE_2} PROPERTY FOLDER "test_i")
set_property (TARGET ${EXECUTABLE_3} PROPERTY FOLDER "test_i")
set_property (TARGET ${EXECUTABLE_4} PROPERTY FOLDER "test_i")

target_link_libraries (${EXECUTABLE} ${ACE_LIBRARY})

target_link_libraries (${EXECUTABLE} Common)
target_link_libraries (${EXECUTABLE} CommonLog)
target_link_libraries (${EXECUTABLE} CommonParser)
target_link_libraries (${EXECUTABLE} CommonParser_Bencoding)
target_link_libraries (${EXECUTABLE} CommonTimer)

target_link_libraries (${EXECUTABLE_2} ${ACE_LIBRARY})

target_link_libraries (${EXECUTABLE_2} Common)
target_link_libraries (${EXECUTABLE_2} CommonLog)
target_link_libraries (${EXECUTABLE_2} CommonParser)
target_link_libraries (${EXECUTABLE_2} CommonParser_Bencoding)
target_link_libraries (${EXECUTABLE_2} CommonTimer)

target_link_libraries (${EXECUTABLE_3} ${ACE_LIBRARY})

target_link_libraries (${EXECUTABLE_3} Common)
target_link_libraries (${EXECUTABLE_3} CommonLog)
target_link_libraries (${EXECUTABLE_3} CommonParser)
target_link_libraries (${EXECUTABLE_3} CommonParser_M3U)
target_link_libraries (${EXECUTABLE_3} CommonTimer)

if (LIBXML2_SUPPORT)
 target_link_libraries (${EXECUTABLE_4} "${LIBXML2_LIBRARIES}")
endif (LIBXML2_SUPPORT)

target_link_libraries (${EXECUTABLE_4} ${ACE_LIBRARY})

target_link_libraries (${EXECUTABLE_4} Common)
target_link_libraries (${EXECUTABLE_4} CommonLog)
target_link_libraries (${EXECUTABLE_4} CommonTimer)

# add PCH
target_precompile_headers (${EXECUTABLE}
                           PRIVATE stdafx.h)
target_precompile_headers (${EXECUTABLE_2}
                           REUSE_FROM ${EXECUTABLE})
target_precompile_headers (${EXECUTABLE_3}
                           REUSE_FROM ${EXECUTABLE})
target_precompile_headers (${EXECUTABLE_4}
                           REUSE_FROM ${EXECUTABLE})

############### installation #############
install (TARGETS ${EXECUTABLE}
         EXPORT ${PROJECT_NAME}Targets
         RUNTIME DESTINATION ${INSTALL_BIN_DIR} COMPONENT test_i)
install (TARGETS ${EXECUTABLE_2}
         EXPORT ${PROJECT_NAME}Targets
         RUNTIME DESTINATION ${INSTALL_BIN_DIR} COMPONENT test_i)
install (TARGETS ${EXECUTABLE_3}
         EXPORT ${PROJECT_NAME}Targets
         RUNTIME DESTINATION ${INSTALL_BIN_DIR} COMPONENT test_i)
install (TARGETS ${EXECUTABLE_4}
         EXPORT ${PROJECT_NAME}Targets
         RUNTIME DESTINATION ${INSTALL_BIN_DIR} COMPONENT test_i)

################ launchers ###############
if (UNIX)
 file (TO_NATIVE_PATH "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/${EXECUTABLE}" COMMAND)
 file (TO_NATIVE_PATH "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/${EXECUTABLE_2}" COMMAND_2)
 file (TO_NATIVE_PATH "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/${EXECUTABLE_3}" COMMAND_3)
 file (TO_NATIVE_PATH "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/${EXECUTABLE_4}" COMMAND_4)
elseif (WIN32)
# if (VCPKG_USE)
#  file (TO_CMAKE_PATH ${VCPKG_ROOT}/installed/${VCPKG_TARGET_TRIPLET}/bin ACE_LIB_PATH)
# else ()
 file (TO_CMAKE_PATH "${LIBICONV_LIB_DIR}" LIBICONV_LIB_PATH)
 file (TO_CMAKE_PATH "${LIBXML2_LIB_DIR}" LIBXML2_LIB_PATH)
 if (GTK_SUPPORT)
  file (TO_CMAKE_PATH "${GTK_LIB_DIRS}" GTK_LIB_PATH)
 endif (GTK_SUPPORT)
 file (TO_CMAKE_PATH "${ACE_LIB_DIR}" ACE_LIB_PATH)

# endif (VCPKG_USE)
 set (RUNTIME_LIBRARY_DIRS ${RUNTIME_LIBRARY_DIRS}
                           ${LIBICONV_LIB_PATH}
                           ${LIBXML2_LIB_PATH}
                           ${GTK_LIB_PATH}
                           ${ACE_LIB_PATH})
 file (TO_NATIVE_PATH "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/${EXECUTABLE}.exe" COMMAND)
 string (REPLACE "\\" "\\\\" COMMAND ${COMMAND})
 file (TO_NATIVE_PATH "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/${EXECUTABLE_2}.exe" COMMAND_2)
 string (REPLACE "\\" "\\\\" COMMAND_2 ${COMMAND_2})
 file (TO_NATIVE_PATH "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/${EXECUTABLE_3}.exe" COMMAND_3)
 string (REPLACE "\\" "\\\\" COMMAND_3 ${COMMAND_3})
 file (TO_NATIVE_PATH "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/${EXECUTABLE_4}.exe" COMMAND_4)
 string (REPLACE "\\" "\\\\" COMMAND_4 ${COMMAND_4})
endif ()
create_target_launcher (${EXECUTABLE}
                        COMMAND ${COMMAND}
                        ARGS "-d -e -f test.txt -l -t"
#                         FORWARD_ARGS
                        RUNTIME_LIBRARY_DIRS ${RUNTIME_LIBRARY_DIRS}
                                             ${PROJECT_BINARY_DIR}/src/${CMAKE_BUILD_TYPE}
                        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/test_i/${EXECUTABLE}
                        ENVIRONMENT foo=bar)
create_target_launcher (${EXECUTABLE_2}
                        COMMAND ${COMMAND_2}
                        ARGS "-d -e -f test_2.txt -l -t"
#                         FORWARD_ARGS
                        RUNTIME_LIBRARY_DIRS ${RUNTIME_LIBRARY_DIRS}
                                             ${PROJECT_BINARY_DIR}/src/${CMAKE_BUILD_TYPE}
                        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/test_i/${EXECUTABLE}
                        ENVIRONMENT foo=bar)
create_target_launcher (${EXECUTABLE_3}
                        COMMAND ${COMMAND_3}
                        ARGS "-d -e -f test_3.txt -l -t"
#                         FORWARD_ARGS
                        RUNTIME_LIBRARY_DIRS ${RUNTIME_LIBRARY_DIRS}
                                             ${PROJECT_BINARY_DIR}/src/${CMAKE_BUILD_TYPE}
                        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/test_i/${EXECUTABLE}
                        ENVIRONMENT foo=bar)
create_target_launcher (${EXECUTABLE_4}
                        COMMAND ${COMMAND_4}
                        ARGS "-f test_4.xml -l -t"
#                         FORWARD_ARGS
                        RUNTIME_LIBRARY_DIRS ${RUNTIME_LIBRARY_DIRS}
                                             ${PROJECT_BINARY_DIR}/src/${CMAKE_BUILD_TYPE}
                        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/test_i/${EXECUTABLE}
                        ENVIRONMENT foo=bar)

################ tests ###################
add_test (NAME ${EXECUTABLE}_test
          COMMAND ${PROJECT_BINARY_DIR}/${EXECUTABLE}
          WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/test_i/${EXECUTABLE})
set_tests_properties (${EXECUTABLE}_test
                      PROPERTIES
                      ENVIRONMENT "PATH=D:\\projects\\ATCD\\ACE\\lib;D:\\projects\\gtk\\bin;D:\\projects\\libglade\\bin;D:\\projects\\libxml2-2.9.1\\.libs;G:\\software\\Development\\dll;${PROJECT_SOURCE_DIR}\\..\\libCommon\\cmake\\src\\Debug;${PROJECT_SOURCE_DIR}\\..\\libCommon\\cmake\\src\\ui\\Debug;${PROJECT_SOURCE_DIR}\\cmake\\src\\Debug;${PROJECT_SOURCE_DIR}\\cmake\\src\\modules\\dev\\Debug;${PROJECT_SOURCE_DIR}\\..\\libACENetwork\\cmake\\src\\Debug;%PATH%")

add_test (NAME ${EXECUTABLE_2}_test
          COMMAND ${PROJECT_BINARY_DIR}/${EXECUTABLE_2}
          WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/test_i/${EXECUTABLE})
set_tests_properties (${EXECUTABLE_2}_test
                      PROPERTIES
                      ENVIRONMENT "PATH=D:\\projects\\ATCD\\ACE\\lib;D:\\projects\\gtk\\bin;D:\\projects\\libglade\\bin;D:\\projects\\libxml2-2.9.1\\.libs;G:\\software\\Development\\dll;${PROJECT_SOURCE_DIR}\\..\\libCommon\\cmake\\src\\Debug;${PROJECT_SOURCE_DIR}\\..\\libCommon\\cmake\\src\\ui\\Debug;${PROJECT_SOURCE_DIR}\\cmake\\src\\Debug;${PROJECT_SOURCE_DIR}\\cmake\\src\\modules\\dev\\Debug;${PROJECT_SOURCE_DIR}\\..\\libACENetwork\\cmake\\src\\Debug;%PATH%")

add_test (NAME ${EXECUTABLE_3}_test
          COMMAND ${PROJECT_BINARY_DIR}/${EXECUTABLE_3}
          WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/test_i/${EXECUTABLE})
set_tests_properties (${EXECUTABLE_3}_test
                      PROPERTIES
                      ENVIRONMENT "PATH=D:\\projects\\ATCD\\ACE\\lib;D:\\projects\\gtk\\bin;D:\\projects\\libglade\\bin;D:\\projects\\libxml2-2.9.1\\.libs;G:\\software\\Development\\dll;${PROJECT_SOURCE_DIR}\\..\\libCommon\\cmake\\src\\Debug;${PROJECT_SOURCE_DIR}\\..\\libCommon\\cmake\\src\\ui\\Debug;${PROJECT_SOURCE_DIR}\\cmake\\src\\Debug;${PROJECT_SOURCE_DIR}\\cmake\\src\\modules\\dev\\Debug;${PROJECT_SOURCE_DIR}\\..\\libACENetwork\\cmake\\src\\Debug;%PATH%")

add_test (NAME ${EXECUTABLE_4}_test
          COMMAND ${PROJECT_BINARY_DIR}/${EXECUTABLE_4}
          WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/test_i/${EXECUTABLE})
set_tests_properties (${EXECUTABLE_4}_test
                      PROPERTIES
                      ENVIRONMENT "PATH=D:\\projects\\ATCD\\ACE\\lib;D:\\projects\\gtk\\bin;D:\\projects\\libglade\\bin;D:\\projects\\libxml2-2.9.1\\.libs;G:\\software\\Development\\dll;${PROJECT_SOURCE_DIR}\\..\\libCommon\\cmake\\src\\Debug;${PROJECT_SOURCE_DIR}\\..\\libCommon\\cmake\\src\\ui\\Debug;${PROJECT_SOURCE_DIR}\\cmake\\src\\Debug;${PROJECT_SOURCE_DIR}\\cmake\\src\\modules\\dev\\Debug;${PROJECT_SOURCE_DIR}\\..\\libACENetwork\\cmake\\src\\Debug;%PATH%")

#PATH=%PATH%;$(ProjectDir)..\..\..\..\ATCD\ACE\lib;$(ProjectDir)..\..\..\..\gtk3\bin;$(ProjectDir)..\..\..\..\libav-11.7\libavcodec;$(ProjectDir)..\..\..\..\libav-11.7\libavresample;$(ProjectDir)..\..\..\..\libav-11.7\libavutil;$(ProjectDir)..\..\..\..\libCommon\cmake\src\Debug;$(ProjectDir)..\..\..\..\libCommon\cmake\src\err\Debug;$(ProjectDir)..\..\..\..\libCommon\cmake\src\ui\Debug;$(ProjectDir)..\..\src\Debug;$(ProjectDir)..\..\src\modules\dev\Debug;$(ProjectDir)..\..\src\modules\vis\Debug
