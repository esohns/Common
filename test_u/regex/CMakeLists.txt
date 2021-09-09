get_filename_component (BUILD_PATH ${CMAKE_CURRENT_BINARY_DIR}/../.. REALPATH)
get_filename_component (BUILD_PATH_SUFFIX ${BUILD_PATH} NAME)

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

#include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../src)
include_directories (${CMAKE_CURRENT_BINARY_DIR}/../..)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../include)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/..)

############## next target ################
set (regex_EXE_SRCS
     stdafx.cpp
     stdafx.h
#     ../test_u_common.h
#     ../test_u_defines.h
#     targetver.h
     test_u.cpp)

#cmake_policy (SET CMP0065 OLD) # do not add flags to export symbols from executables without the ENABLE_EXPORTS target property
set (EXECUTABLE regex)
add_executable (${EXECUTABLE}
                EXCLUDE_FROM_ALL
                ${regex_EXE_SRCS})
set_property (TARGET ${EXECUTABLE} PROPERTY FOLDER "test_u")
#cmake_policy (SET CMP0012 NEW)
#if (WIN32 AND NOT BUILD_TEST_U)
# set_target_properties (${EXECUTABLE} PROPERTIES EXCLUDE_FROM_DEFAULT_BUILD TRUE)
#endif ()

#add_dependencies (${ACESTREAM_TARGET_NAME} ACE)
#add_dependencies (${PROJECT_NAME} Common)

#target_link_libraries (${EXECUTABLE} ${CMAKE_THREAD_LIBS_INIT})
if (UNIX)
# target_link_libraries (${EXECUTABLE} rt)
# target_link_libraries (${EXECUTABLE} ${PKG_CAP_LIBRARIES})
# target_link_libraries (${EXECUTABLE} X11)
elseif (WIN32)
# target_link_libraries (${EXECUTABLE} Advapi32.lib)
# target_link_libraries (${EXECUTABLE} Ole32.lib)
# target_link_libraries (${EXECUTABLE} Quartz.lib)
# target_link_libraries (${EXECUTABLE} Shell32.lib)
# target_link_libraries (${EXECUTABLE} ucrtd.lib)
# target_link_libraries (${EXECUTABLE} User32.lib)
# target_link_libraries (${EXECUTABLE} Userenv.lib)
# *TODO*: repair WIN32 pkgconfig support
 target_link_libraries (${EXECUTABLE} ${ACE_LIBRARY})
endif ()
#target_link_libraries (${EXECUTABLE} Common)
#if (WIN32)
 # *WARNING*: linking CommonUI requires "/FORCE:MULTIPLE""
# set_target_properties (${EXECUTABLE} PROPERTIES LINK_FLAGS /FORCE:MULTIPLE)
#endif ()
#target_link_libraries (${EXECUTABLE} CommonTimer)
target_link_libraries (${EXECUTABLE} Common_Test_U)

# add PCH
set_target_properties (${EXECUTABLE} PROPERTIES COTIRE_CXX_PREFIX_HEADER_INIT "stdafx.h")
cotire (${EXECUTABLE})

install (TARGETS ${EXECUTABLE}
         EXPORT ${PROJECT_NAME}Targets
         RUNTIME DESTINATION ${INSTALL_BIN_DIR} COMPONENT test_u)

if (UNIX)
 create_target_launcher (${EXECUTABLE}
                         ARGS "-l -t"
#                         FORWARD_ARGS
                         RUNTIME_LIBRARY_DIRS ${PROJECT_SOURCE_DIR}/modules/ACE/lib
                                              ${PROJECT_SOURCE_DIR}/${BUILD_PATH_SUFFIX}/src/${CMAKE_BUILD_TYPE}
                         WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/../test_u/regex
                         ENVIRONMENT foo=bar)
elseif (WIN32)
 if (VCPKG_SUPPORT)
  file (TO_CMAKE_PATH $ENV{VCPKG_ROOT}/installed/bin ACE_LIB_PATH)
 else ()
  file (TO_CMAKE_PATH $ENV{LIB_ROOT}/ACE_TAO/ACE/lib ACE_LIB_PATH)
 endif (VCPKG_SUPPORT)
 create_target_launcher (${EXECUTABLE}
                         ARGS "-l -t"
#                        FORWARD_ARGS
                         RUNTIME_LIBRARY_DIRS ${ACE_LIB_PATH}
                                              ${PROJECT_SOURCE_DIR}/${BUILD_PATH_SUFFIX}/src/${CMAKE_BUILD_TYPE}
                         WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/../test_u/regex
                         ENVIRONMENT foo=bar)
endif ()

add_test (NAME ${EXECUTABLE}_test
          COMMAND ${PROJECT_BINARY_DIR}/${EXECUTABLE} -l -t
          WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})

set_tests_properties (${EXECUTABLE}_test
                      PROPERTIES
                      ENVIRONMENT "PATH=D:\\projects\\ATCD\\ACE\\lib;D:\\projects\\gtk\\bin;D:\\projects\\libglade\\bin;D:\\projects\\libxml2-2.9.1\\.libs;G:\\software\\Development\\dll;${PROJECT_SOURCE_DIR}\\..\\libCommon\\cmake\\src\\Debug;${PROJECT_SOURCE_DIR}\\..\\libCommon\\cmake\\src\\ui\\Debug;${PROJECT_SOURCE_DIR}\\cmake\\src\\Debug;${PROJECT_SOURCE_DIR}\\cmake\\src\\modules\\dev\\Debug;${PROJECT_SOURCE_DIR}\\..\\libACENetwork\\cmake\\src\\Debug;%PATH%")
#PATH=%PATH%;$(ProjectDir)..\..\..\..\ATCD\ACE\lib;$(ProjectDir)..\..\..\..\gtk3\bin;$(ProjectDir)..\..\..\..\libav-11.7\libavcodec;$(ProjectDir)..\..\..\..\libav-11.7\libavresample;$(ProjectDir)..\..\..\..\libav-11.7\libavutil;$(ProjectDir)..\..\..\..\libCommon\cmake\src\Debug;$(ProjectDir)..\..\..\..\libCommon\cmake\src\err\Debug;$(ProjectDir)..\..\..\..\libCommon\cmake\src\ui\Debug;$(ProjectDir)..\..\src\Debug;$(ProjectDir)..\..\src\modules\dev\Debug;$(ProjectDir)..\..\src\modules\vis\Debug
