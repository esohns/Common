
if (UNIX)
 include_directories ($ENV{LIB_ROOT}/ACE_TAO/ACE)
# include_directories (/usr/local/src/ACE_wrappers)

 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../src)
# include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../modules/Common/src/timer)
# include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../modules/Common/src/ui)
# include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../modules/Common/src/ui/gtk)
elseif (WIN32)
# include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../ATCD/ACE)
# include_directories ($ENV{ACE_ROOT})

# include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src)
# include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src/timer)
# include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src/ui)
# include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src/ui/gtk)
endif ()
#include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../src)
#include_directories (${CMAKE_CURRENT_BINARY_DIR}/../..)
#include_directories (${CMAKE_CURRENT_SOURCE_DIR}/..)

############## next target ################
set (features_EXE_SRCS
     stdafx.cpp
     stdafx.h
#     ../test_u_common.h
#     ../test_u_defines.h
#     targetver.h
     test_u.cpp)

#cmake_policy (SET CMP0065 OLD) # do not add flags to export symbols from executables without the ENABLE_EXPORTS target property
set (EXECUTABLE features)
add_executable (${EXECUTABLE}
                EXCLUDE_FROM_ALL
                ${features_EXE_SRCS})
set_property (TARGET ${EXECUTABLE} PROPERTY FOLDER "test_u")
#cmake_policy (SET CMP0012 NEW)
#if (WIN32 AND NOT BUILD_TEST_U)
# set_target_properties (${EXECUTABLE} PROPERTIES EXCLUDE_FROM_DEFAULT_BUILD TRUE)
#endif ()

#add_dependencies (${ACESTREAM_TARGET_NAME} ACE)
#add_dependencies (${PROJECT_NAME} Common)

# add PCH
target_precompile_headers (${EXECUTABLE}
                           PUBLIC stdafx.h)

install (TARGETS ${EXECUTABLE}
         EXPORT ${PROJECT_NAME}Targets
         RUNTIME DESTINATION ${INSTALL_BIN_DIR} COMPONENT test_u)

if (UNIX)
 create_target_launcher (${EXECUTABLE}
                         ARGS "-l -t"
#                         FORWARD_ARGS
                         RUNTIME_LIBRARY_DIRS ${PROJECT_SOURCE_DIR}/modules/ATCD/ACE/lib
#                                              ${PROJECT_SOURCE_DIR}/../gtk/gtk/Win32/debug/bin
#                                              ${PROJECT_SOURCE_DIR}/../libCommon/cmake/src/Debug
#                                              ${PROJECT_SOURCE_DIR}/../libCommon/cmake/src/ui/Debug
                                              ${PROJECT_SOURCE_DIR}/cmake/src/Debug
#                                              ${PROJECT_SOURCE_DIR}/cmake/src/modules/dev/Debug
                         WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/../test_u/features
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
          COMMAND ${PROJECT_BINARY_DIR}/${EXECUTABLE} -l -t
          WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})

set_tests_properties (${EXECUTABLE}_test
                      PROPERTIES
                      ENVIRONMENT "PATH=D:\\projects\\ATCD\\ACE\\lib;D:\\projects\\gtk\\bin;D:\\projects\\libglade\\bin;D:\\projects\\libxml2-2.9.1\\.libs;G:\\software\\Development\\dll;${PROJECT_SOURCE_DIR}\\..\\libCommon\\cmake\\src\\Debug;${PROJECT_SOURCE_DIR}\\..\\libCommon\\cmake\\src\\ui\\Debug;${PROJECT_SOURCE_DIR}\\cmake\\src\\Debug;${PROJECT_SOURCE_DIR}\\cmake\\src\\modules\\dev\\Debug;${PROJECT_SOURCE_DIR}\\..\\libACENetwork\\cmake\\src\\Debug;%PATH%")
#PATH=%PATH%;$(ProjectDir)..\..\..\..\ATCD\ACE\lib;$(ProjectDir)..\..\..\..\gtk3\bin;$(ProjectDir)..\..\..\..\libav-11.7\libavcodec;$(ProjectDir)..\..\..\..\libav-11.7\libavresample;$(ProjectDir)..\..\..\..\libav-11.7\libavutil;$(ProjectDir)..\..\..\..\libCommon\cmake\src\Debug;$(ProjectDir)..\..\..\..\libCommon\cmake\src\err\Debug;$(ProjectDir)..\..\..\..\libCommon\cmake\src\ui\Debug;$(ProjectDir)..\..\src\Debug;$(ProjectDir)..\..\src\modules\dev\Debug;$(ProjectDir)..\..\src\modules\vis\Debug
