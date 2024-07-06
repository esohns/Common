set (CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/../../src/gl/scripts")
include (glfw)
include (glm)
#include (assimp)
include (vulkan)

set (CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/../../src/image/scripts")
include (png)

############## includes ###############
if (GLFW_SUPPORT)
 include_directories (${GLFW_INCLUDE_DIRS})
endif (GLFW_SUPPORT)
if (GLM_SUPPORT)
 include_directories (${GLM_INCLUDE_DIRS})
endif (GLM_SUPPORT)
if (VULKAN_SUPPORT)
 include_directories (${VULKAN_INCLUDE_DIRS})
endif (VULKAN_SUPPORT)

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../src)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../src/gl)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../src/image)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../src/log)
#include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../src/timer)

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/..)

include_directories (${CMAKE_CURRENT_BINARY_DIR}/../..)

############## next target ###############
set (EXECUTABLE vulkan)
set (${EXECUTABLE}_EXE_SRCS
#     stdafx.cpp
     stdafx.h
     test_i.cpp
     test_i_app.cpp
     test_i_app.h
     test_i_vulkan_common.h
     test_i_vulkan_defines.h)

#cmake_policy (SET CMP0065 OLD) # do not add flags to export symbols from executables without the ENABLE_EXPORTS target property
add_executable (${EXECUTABLE}
                EXCLUDE_FROM_ALL
                ${${EXECUTABLE}_EXE_SRCS})
set_property (TARGET ${EXECUTABLE} PROPERTY FOLDER "test_i")

if (GLFW_SUPPORT)
 target_link_libraries (${EXECUTABLE} PUBLIC ${GLFW_LIBRARIES})
endif (GLFW_SUPPORT)
if (VULKAN_SUPPORT)
  target_link_libraries (${EXECUTABLE} PUBLIC ${VULKAN_LIBRARIES})
endif (VULKAN_SUPPORT)

target_link_libraries (${EXECUTABLE} PUBLIC ${ACE_LIBRARY})

target_link_libraries (${EXECUTABLE} PUBLIC Common)
target_link_libraries (${EXECUTABLE} PUBLIC CommonLog)
#if (OPENGL_SUPPORT)
# target_link_libraries (${EXECUTABLE} PUBLIC CommonOpenGL)
#endif (OPENGL_SUPPORT)
target_link_libraries (${EXECUTABLE} PUBLIC CommonTimer)

target_link_libraries (${EXECUTABLE} PUBLIC Common_Test_I)

# add PCH
target_precompile_headers (${EXECUTABLE}
                           PRIVATE stdafx.h)

############### installation #############
install (TARGETS ${EXECUTABLE}
         EXPORT ${PROJECT_NAME}Targets
         RUNTIME DESTINATION ${INSTALL_BIN_DIR} COMPONENT test_i)
install (FILES share/vulkan_logo.png
         DESTINATION "${INSTALL_SHARE_DIR}/${EXECUTABLE}" COMPONENT data)

################ launchers ###############

set (ARGS "-t")
if (UNIX)
 file (TO_NATIVE_PATH "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/${EXECUTABLE}" COMMAND)
 create_target_launcher (${EXECUTABLE}
                         COMMAND ${COMMAND}
                         ARGS ${ARGS}
#                         FORWARD_ARGS
                         RUNTIME_LIBRARY_DIRS ${ACE_LIB_PATH}
                         WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/../test_i/${EXECUTABLE}
                         ENVIRONMENT ${ENVIRONMENT})
elseif (WIN32)
 file (TO_NATIVE_PATH "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/${EXECUTABLE}.exe" COMMAND)
 string (REPLACE "\\" "\\\\" COMMAND ${COMMAND})
# file (TO_CMAKE_PATH "${GLEW_LIB_DIR}" GLEW_LIB_PATH)
 file (TO_CMAKE_PATH "${GLFW_LIB_DIR}" GLFW_LIB_PATH)
 file (TO_CMAKE_PATH "${PNG_LIB_DIR}" PNG_LIB_PATH)
 file (TO_CMAKE_PATH "${ASSIMP_LIB_DIR}" ASSIMP_LIB_PATH)
 # *NOTE*: ffmpeg was built with "vcpkg-libpng" support, so now there is a new dependency to zlib1.dll, liblzma.dll, iconv-2.dll :-(
# file (TO_CMAKE_PATH "$ENV{VCPKG_INSTALLATION_ROOT}/installed/x64-windows/bin" VCPKG_LIB_PATH)
# file (TO_CMAKE_PATH "${ffmpeg_LIB_DIR}" FFMPEG_LIB_PATH)
# file (TO_CMAKE_PATH "${ImageMagick_LIB_DIR}" IMAGEMAGICK_LIB_PATH)
 file (TO_CMAKE_PATH "${ACE_LIB_DIR}" ACE_LIB_PATH)
 create_target_launcher (${EXECUTABLE}
                         COMMAND ${COMMAND}
                         ARGS ${ARGS}
#                         FORWARD_ARGS
                         RUNTIME_LIBRARY_DIRS ${GLFW_LIB_PATH}
                                              ${PNG_LIB_PATH}
                                              ${ASSIMP_LIB_PATH}
                                              ${ACE_LIB_PATH}
                         WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/test_i/${EXECUTABLE}
                         ENVIRONMENT ${ENVIRONMENT})
endif ()

################ tests ###################
add_test (NAME ${EXECUTABLE}_test
          COMMAND ${PROJECT_BINARY_DIR}/${EXECUTABLE}
          WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/test_i/${EXECUTABLE})
set_tests_properties (${EXECUTABLE}_test
                      PROPERTIES
                      ENVIRONMENT "PATH=D:\\projects\\ATCD\\ACE\\lib;D:\\projects\\gtk\\bin;D:\\projects\\libglade\\bin;D:\\projects\\libxml2-2.9.1\\.libs;G:\\software\\Development\\dll;${PROJECT_SOURCE_DIR}\\..\\libCommon\\cmake\\src\\Debug;${PROJECT_SOURCE_DIR}\\..\\libCommon\\cmake\\src\\ui\\Debug;${PROJECT_SOURCE_DIR}\\cmake\\src\\Debug;${PROJECT_SOURCE_DIR}\\cmake\\src\\modules\\dev\\Debug;${PROJECT_SOURCE_DIR}\\..\\libACENetwork\\cmake\\src\\Debug;%PATH%")
#PATH=%PATH%;$(ProjectDir)..\..\..\..\ATCD\ACE\lib;$(ProjectDir)..\..\..\..\gtk3\bin;$(ProjectDir)..\..\..\..\libav-11.7\libavcodec;$(ProjectDir)..\..\..\..\libav-11.7\libavresample;$(ProjectDir)..\..\..\..\libav-11.7\libavutil;$(ProjectDir)..\..\..\..\libCommon\cmake\src\Debug;$(ProjectDir)..\..\..\..\libCommon\cmake\src\err\Debug;$(ProjectDir)..\..\..\..\libCommon\cmake\src\ui\Debug;$(ProjectDir)..\..\src\Debug;$(ProjectDir)..\..\src\modules\dev\Debug;$(ProjectDir)..\..\src\modules\vis\Debug
