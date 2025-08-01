
string (REPLACE ${CMAKE_SOURCE_DIR} "" BUILD_PATH_SUFFIX ${CMAKE_BINARY_DIR})
string (SUBSTRING ${BUILD_PATH_SUFFIX} 1 -1 BUILD_PATH_SUFFIX)

#include_directories (${CMAKE_CURRENT_BINARY_DIR}/../..)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../src)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../src/log)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../src/timer)
#include_directories (${CMAKE_CURRENT_SOURCE_DIR}/..)

############## next target ################
set (file_EXE_SRCS
#     stdafx.cpp
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
                            ICON "${PROJECT_SOURCE_DIR}/share/Common_icon.ico"
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
add_dependencies (${PROJECT_NAME} Common)

target_link_libraries (${EXECUTABLE} PUBLIC ${ACE_LIBRARY})

target_link_libraries (${EXECUTABLE} PUBLIC Common)
target_link_libraries (${EXECUTABLE} PUBLIC CommonLog)
target_link_libraries (${EXECUTABLE} PUBLIC CommonTimer)

# add PCH
target_precompile_headers (${EXECUTABLE}
                           PRIVATE stdafx.h)

install (TARGETS ${EXECUTABLE}
         EXPORT ${PROJECT_NAME}Targets
         RUNTIME DESTINATION ${INSTALL_BIN_DIR} COMPONENT test_u)

if (UNIX)
 file (TO_NATIVE_PATH "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/${EXECUTABLE}" COMMAND)
 file (TO_NATIVE_PATH "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/${EXECUTABLE}" FILE_PATH)
 create_target_launcher (${EXECUTABLE}
                         COMMAND ${COMMAND}
                         ARGS "-s ${FILE_PATH} -t"
#                         FORWARD_ARGS
                         RUNTIME_LIBRARY_DIRS ${PROJECT_SOURCE_DIR}/modules/ACE/lib
                         WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/test_u/${EXECUTABLE}
                         ENVIRONMENT foo=bar)
elseif (WIN32)
 file (TO_NATIVE_PATH "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/${EXECUTABLE}.exe" COMMAND)
 string (REPLACE "\\" "\\\\" COMMAND ${COMMAND})
# if (VCPKG_USE)
#  file (TO_CMAKE_PATH ${VCPKG_ROOT}/installed/${VCPKG_TARGET_TRIPLET}/bin ACE_LIB_PATH)
# else ()
  file (TO_CMAKE_PATH $ENV{LIB_ROOT}/ACE_TAO/ACE/lib ACE_LIB_PATH)
# endif (VCPKG_USE)
 file (TO_NATIVE_PATH "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/${EXECUTABLE}.exe" FILE_PATH)
 string (REPLACE "\\" "\\\\" FILE_PATH ${FILE_PATH})
 create_target_launcher (${EXECUTABLE}
                         COMMAND ${COMMAND}
                         ARGS "-s ${FILE_PATH} -t"
#                        FORWARD_ARGS
                         RUNTIME_LIBRARY_DIRS ${ACE_LIB_PATH}
                                              G:/software/Development/dll
                                              ${PROJECT_BINARY_DIR}/src/${CMAKE_BUILD_TYPE}
                                              ${PROJECT_BINARY_DIR}/src/err/${CMAKE_BUILD_TYPE}
                                              ${PROJECT_BINARY_DIR}/src/ui/${CMAKE_BUILD_TYPE}
                          WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/test_u/${EXECUTABLE}
                          ENVIRONMENT foo=bar)
endif ()

add_test (NAME ${EXECUTABLE}_test
          COMMAND ${PROJECT_BINARY_DIR}/${EXECUTABLE} -l -t
          WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/test_u/${EXECUTABLE})

set_tests_properties (${EXECUTABLE}_test
                      PROPERTIES
                      ENVIRONMENT "PATH=D:\\projects\\ATCD\\ACE\\lib;D:\\projects\\gtk\\bin;D:\\projects\\libglade\\bin;D:\\projects\\libxml2-2.9.1\\.libs;G:\\software\\Development\\dll;${PROJECT_SOURCE_DIR}\\..\\libCommon\\cmake\\src\\Debug;${PROJECT_SOURCE_DIR}\\..\\libCommon\\cmake\\src\\ui\\Debug;${PROJECT_SOURCE_DIR}\\cmake\\src\\Debug;${PROJECT_SOURCE_DIR}\\cmake\\src\\modules\\dev\\Debug;${PROJECT_SOURCE_DIR}\\..\\libACENetwork\\cmake\\src\\Debug;%PATH%")
#PATH=%PATH%;$(ProjectDir)..\..\..\..\ATCD\ACE\lib;$(ProjectDir)..\..\..\..\gtk3\bin;$(ProjectDir)..\..\..\..\libav-11.7\libavcodec;$(ProjectDir)..\..\..\..\libav-11.7\libavresample;$(ProjectDir)..\..\..\..\libav-11.7\libavutil;$(ProjectDir)..\..\..\..\libCommon\cmake\src\Debug;$(ProjectDir)..\..\..\..\libCommon\cmake\src\err\Debug;$(ProjectDir)..\..\..\..\libCommon\cmake\src\ui\Debug;$(ProjectDir)..\..\src\Debug;$(ProjectDir)..\..\src\modules\dev\Debug;$(ProjectDir)..\..\src\modules\vis\Debug
