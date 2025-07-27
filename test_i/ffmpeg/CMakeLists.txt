include (ffmpeg)
#include (ImageMagick)

string (REPLACE ${CMAKE_SOURCE_DIR} "" BUILD_PATH_SUFFIX ${CMAKE_BINARY_DIR})
string (SUBSTRING ${BUILD_PATH_SUFFIX} 1 -1 BUILD_PATH_SUFFIX)

############## definitions ###############

############## includes ###############

include_directories (${ffmpeg_INCLUDE_DIRS})

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../src)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../src/image)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../src/log)

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/..)

include_directories (${CMAKE_CURRENT_BINARY_DIR}/..)

############## next target ###############
set (ffmpeg_EXE_SRCS
#     stdafx.cpp
     stdafx.h
     test_i.cpp)

#cmake_policy (SET CMP0065 OLD) # do not add flags to export symbols from executables without the ENABLE_EXPORTS target property
set (EXECUTABLE ffmpeg)
add_executable (${EXECUTABLE}
                EXCLUDE_FROM_ALL
                ${ffmpeg_EXE_SRCS})
set_property (TARGET ${EXECUTABLE} PROPERTY FOLDER "test_i")

if (UNIX)
 if (THREADS_HAVE_PTHREAD_ARG)
  set_property (TARGET ${EXECUTABLE} PROPERTY COMPILE_OPTIONS "-pthread")
  set_property (TARGET ${EXECUTABLE} PROPERTY INTERFACE_COMPILE_OPTIONS "-pthread")
 endif (THREADS_HAVE_PTHREAD_ARG)
 target_link_libraries (${EXECUTABLE} PUBLIC ${CMAKE_THREAD_LIBS_INIT})
endif (UNIX)
target_link_libraries (${EXECUTABLE} PUBLIC ${ffmpeg_LIBRARIES})
#target_link_libraries (${EXECUTABLE} PUBLIC ${ffmpeg_2_LIBRARIES})

target_link_libraries (${EXECUTABLE} PUBLIC ${ACE_LIBRARY})

target_link_libraries (${EXECUTABLE} PUBLIC Common)
target_link_libraries (${EXECUTABLE} PUBLIC CommonImage)
target_link_libraries (${EXECUTABLE} PUBLIC CommonLog)
target_link_libraries (${EXECUTABLE} PUBLIC CommonTimer)

# add PCH
target_precompile_headers (${EXECUTABLE}
                           PUBLIC stdafx.h)

############### installation #############
install (TARGETS ${EXECUTABLE}
         EXPORT ${PROJECT_NAME}Targets
         RUNTIME DESTINATION ${INSTALL_BIN_DIR} COMPONENT test_i)
install (FILES share/oak-tree.png
         DESTINATION "${INSTALL_SHARE_DIR}/${EXECUTABLE}" COMPONENT data)

################ launchers ###############
if (UNIX)
 file (TO_NATIVE_PATH "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/${EXECUTABLE}" COMMAND)
 create_target_launcher (${EXECUTABLE}
                         COMMAND ${COMMAND}
                         ARGS "-f share/oak-tree.png -t"
#                         FORWARD_ARGS
                         RUNTIME_LIBRARY_DIRS ${PROJECT_SOURCE_DIR}/modules/ACE/lib
                                              ${PROJECT_BINARY_DIR}/src/${CMAKE_BUILD_TYPE}
                         WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/test_i/${EXECUTABLE}
                         ENVIRONMENT foo=bar)
elseif (WIN32)
 file (TO_NATIVE_PATH "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/${EXECUTABLE}.exe" COMMAND)
 string (REPLACE "\\" "\\\\" COMMAND ${COMMAND})
 file (TO_CMAKE_PATH "${VCPKG_RELEASE_BIN_DIR}" VCPKG_BIN_PATH)
 file (TO_CMAKE_PATH "${ffmpeg_LIB_DIR}" FFMPEG_LIB_PATH)
 file (TO_CMAKE_PATH "${ImageMagick_LIB_DIR}" IMAGEMAGICK_LIB_PATH)
 file (TO_CMAKE_PATH "${ACE_LIB_DIR}" ACE_LIB_PATH)
 set (RUNTIME_LIBRARY_DIRS ${FFMPEG_LIB_PATH}
                           ${VCPKG_BIN_PATH}
                           ${IMAGEMAGICK_LIB_PATH}
                           ${ACE_LIB_PATH})
 create_target_launcher (${EXECUTABLE}
                         COMMAND ${COMMAND}
                         ARGS "-f share\\\\oak-tree.png -t"
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
