include (tensorflow)

string (REPLACE ${CMAKE_SOURCE_DIR} "" BUILD_PATH_SUFFIX ${CMAKE_BINARY_DIR})
string (SUBSTRING ${BUILD_PATH_SUFFIX} 1 -1 BUILD_PATH_SUFFIX)

############## definitions ###############

############## includes ###############

include_directories (${tensorflow_INCLUDE_DIRS})
if (TENSORFLOW_CC_SUPPORT)
 include_directories (${tensorflow_cc_INCLUDE_DIRS})
endif (TENSORFLOW_CC_SUPPORT)

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../src)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../src/log)

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/..)

include_directories (${CMAKE_CURRENT_BINARY_DIR}/..)

############## next target ###############
set (EXECUTABLE tensorflow)
set (${EXECUTABLE}_EXE_SRCS
     test_i.cpp)

#cmake_policy (SET CMP0065 OLD) # do not add flags to export symbols from executables without the ENABLE_EXPORTS target property
add_executable (${EXECUTABLE}
                EXCLUDE_FROM_ALL
                ${${EXECUTABLE}_EXE_SRCS})
set_property (TARGET ${EXECUTABLE} PROPERTY FOLDER "test_i")

if (TENSORFLOW_SUPPORT)
 target_link_libraries (${EXECUTABLE} PUBLIC ${tensorflow_LIBRARIES})
endif (TENSORFLOW_SUPPORT)
if (TENSORFLOW_CC_SUPPORT)
 target_link_libraries (${EXECUTABLE} PUBLIC ${tensorflow_cc_LIBRARIES})
endif (TENSORFLOW_CC_SUPPORT)

target_link_libraries (${EXECUTABLE} PUBLIC ${ACE_LIBRARY})

target_link_libraries (${EXECUTABLE} PUBLIC Common)
target_link_libraries (${EXECUTABLE} PUBLIC CommonLog)
target_link_libraries (${EXECUTABLE} PUBLIC CommonTimer)

# add PCH
#target_precompile_headers (${EXECUTABLE}
#                           PUBLIC stdafx.h)

############### installation #############
install (TARGETS ${EXECUTABLE}
         EXPORT ${PROJECT_NAME}Targets
         RUNTIME DESTINATION ${INSTALL_BIN_DIR} COMPONENT test_i)
if (WIN32)
 if (DEFINED ENV{LIB_ROOT})
  file (TO_NATIVE_PATH $ENV{LIB_ROOT} LIB_ROOT)
  string (REPLACE "\\" "\\\\" LIB_ROOT ${LIB_ROOT})
 endif (DEFINED ENV{LIB_ROOT})
 install (FILES ${LIB_ROOT}/tensorflow/lib/tensorflow.dll
          DESTINATION ${INSTALL_BIN_DIR} COMPONENT test_i)
endif (WIN32)

################ launchers ###############
if (UNIX)
 file (TO_NATIVE_PATH "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/${EXECUTABLE}" COMMAND)
 create_target_launcher (${EXECUTABLE}
                         COMMAND ${COMMAND}
                         ARGS "-m 0 -t"
#                         FORWARD_ARGS
                         RUNTIME_LIBRARY_DIRS ${PROJECT_SOURCE_DIR}/modules/ACE/lib
                         WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/test_i/${EXECUTABLE}
                         ENVIRONMENT foo=bar)
elseif (WIN32)
 file (TO_NATIVE_PATH "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/${EXECUTABLE}.exe" COMMAND)
 string (REPLACE "\\" "\\\\" COMMAND ${COMMAND})
 file (TO_CMAKE_PATH "${tensorflow_LIB_DIR}" TENSORFLOW_LIB_PATH)
 file (TO_CMAKE_PATH "${ACE_LIB_DIR}" ACE_LIB_PATH)
 set (RUNTIME_LIBRARY_DIRS ${RUNTIME_LIBRARY_DIRS}
                           ${TENSORFLOW_LIB_PATH}
                           ${ACE_LIB_PATH})
 create_target_launcher (${EXECUTABLE}
                         COMMAND ${COMMAND}
                         ARGS "-m 0 -t"
#                         FORWARD_ARGS
                         RUNTIME_LIBRARY_DIRS ${RUNTIME_LIBRARY_DIRS}
                         WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/test_i/${EXECUTABLE}
                         ENVIRONMENT foo=bar)
endif ()

################ tests ###################
add_test (NAME ${EXECUTABLE}_test
          COMMAND ${PROJECT_BINARY_DIR}/${EXECUTABLE}
          WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/test_i/${EXECUTABLE})

set_tests_properties (${EXECUTABLE}_test
                      PROPERTIES
                      ENVIRONMENT "PATH=D:\\projects\\ATCD\\ACE\\lib;D:\\projects\\gtk\\bin;D:\\projects\\libglade\\bin;D:\\projects\\libxml2-2.9.1\\.libs;G:\\software\\Development\\dll;${PROJECT_SOURCE_DIR}\\..\\libCommon\\cmake\\src\\Debug;${PROJECT_SOURCE_DIR}\\..\\libCommon\\cmake\\src\\ui\\Debug;${PROJECT_SOURCE_DIR}\\cmake\\src\\Debug;${PROJECT_SOURCE_DIR}\\cmake\\src\\modules\\dev\\Debug;${PROJECT_SOURCE_DIR}\\..\\libACENetwork\\cmake\\src\\Debug;%PATH%")
#PATH=%PATH%;$(ProjectDir)..\..\..\..\ATCD\ACE\lib;$(ProjectDir)..\..\..\..\gtk3\bin;$(ProjectDir)..\..\..\..\libav-11.7\libavcodec;$(ProjectDir)..\..\..\..\libav-11.7\libavresample;$(ProjectDir)..\..\..\..\libav-11.7\libavutil;$(ProjectDir)..\..\..\..\libCommon\cmake\src\Debug;$(ProjectDir)..\..\..\..\libCommon\cmake\src\err\Debug;$(ProjectDir)..\..\..\..\libCommon\cmake\src\ui\Debug;$(ProjectDir)..\..\src\Debug;$(ProjectDir)..\..\src\modules\dev\Debug;$(ProjectDir)..\..\src\modules\vis\Debug
