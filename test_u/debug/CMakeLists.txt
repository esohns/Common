if (UNIX)
# find_library (CAP_LIBRARY libcap.so.2
#               DOC "searching for libcap")
# if (NOT CAP_LIBRARY)
#  message (FATAL_ERROR "could not find libcap, aborting")
# endif ()
 pkg_check_modules (PKG_CAP REQUIRED libcap)
endif (UNIX)

string (REPLACE ${CMAKE_SOURCE_DIR} "" BUILD_PATH_SUFFIX ${CMAKE_BINARY_DIR})
string (SUBSTRING ${BUILD_PATH_SUFFIX} 1 -1 BUILD_PATH_SUFFIX)

if (UNIX)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../modules/ACE)

 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../src)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../src/log)
elseif (WIN32)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src/timer)
endif ()

############## next target ################
set (debug_wrapper_EXE_SRCS
#     stdafx.cpp
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
 create_target_launcher (${EXECUTABLE}
                         ARGS "-f ../../../ardrone/cmake/src/ardrone -l -t"
#                         FORWARD_ARGS
                         RUNTIME_LIBRARY_DIRS ${PROJECT_SOURCE_DIR}/modules/ACE/lib
                         WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/../../../ardrone/src
                         ENVIRONMENT foo=bar)
endif (UNIX)

add_test (NAME ${EXECUTABLE}_test
          COMMAND ${PROJECT_BINARY_DIR}/${EXECUTABLE} -f ../../../ardrone/cmake/src/ardrone -l -t
          WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/../../../ardrone/src)

set_tests_properties (${EXECUTABLE}_test
                      PROPERTIES
                      ENVIRONMENT "PATH=D:\\projects\\ATCD\\ACE\\lib;D:\\projects\\gtk\\bin;D:\\projects\\libglade\\bin;D:\\projects\\libxml2-2.9.1\\.libs;G:\\software\\Development\\dll;${PROJECT_SOURCE_DIR}\\..\\libCommon\\cmake\\src\\Debug;${PROJECT_SOURCE_DIR}\\..\\libCommon\\cmake\\src\\ui\\Debug;${PROJECT_SOURCE_DIR}\\cmake\\src\\Debug;${PROJECT_SOURCE_DIR}\\cmake\\src\\modules\\dev\\Debug;${PROJECT_SOURCE_DIR}\\..\\libACENetwork\\cmake\\src\\Debug;%PATH%")
#PATH=%PATH%;$(ProjectDir)..\..\..\..\ATCD\ACE\lib;$(ProjectDir)..\..\..\..\gtk3\bin;$(ProjectDir)..\..\..\..\libav-11.7\libavcodec;$(ProjectDir)..\..\..\..\libav-11.7\libavresample;$(ProjectDir)..\..\..\..\libav-11.7\libavutil;$(ProjectDir)..\..\..\..\libCommon\cmake\src\Debug;$(ProjectDir)..\..\..\..\libCommon\cmake\src\err\Debug;$(ProjectDir)..\..\..\..\libCommon\cmake\src\ui\Debug;$(ProjectDir)..\..\src\Debug;$(ProjectDir)..\..\src\modules\dev\Debug;$(ProjectDir)..\..\src\modules\vis\Debug
