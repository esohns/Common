get_filename_component (BUILD_PATH ${CMAKE_CURRENT_BINARY_DIR}/../.. REALPATH)
get_filename_component (BUILD_PATH_SUFFIX ${BUILD_PATH} NAME)

include_directories (${wxWidgets_INCLUDE_DIRS})

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../src)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../src/image)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../src/log)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../src/ui)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../src/ui/wxwidgets)

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../..)

include_directories (${CMAKE_CURRENT_BINARY_DIR}/../../..)

#set (CMAKE_POSITION_INDEPENDENT_CODE ON)

if (WIN32)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../include)
# add_definitions(-DUNICODE -D_UNICODE)
# add_definitions (-D_CRT_SECURE_NO_WARNINGS)
# add_definitions (-D_SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING)
endif (WIN32)

############## next target ###############
set (EXECUTABLE wxwidgets)
set (${EXECUTABLE}_EXE_SRCS
#     stdafx.cpp
     stdafx.h
     test_i.cpp)

set (EXECUTABLE_2 wxwidgets_2)
set (${EXECUTABLE_2}_EXE_SRCS
#     stdafx.cpp
     stdafx.h
     test_i_2.cpp
     test_i_2.h)

set (EXECUTABLE_3 wxwidgets_3)
set (${EXECUTABLE_3}_EXE_SRCS
#     stdafx.cpp
     stdafx.h
     test_i_3.cpp
     test_i_3.h
     test_i_3_ui.cpp
     test_i_3_ui.h
     test_i_3_ui_base.cpp
     test_i_3_ui_base.h)

#cmake_policy (SET CMP0065 OLD) # do not add flags to export symbols from executables without the ENABLE_EXPORTS target property
if (WIN32)
 add_executable (${EXECUTABLE} WIN32
                 EXCLUDE_FROM_ALL
                 ${${EXECUTABLE}_EXE_SRCS})
 add_executable (${EXECUTABLE_2} WIN32
                 EXCLUDE_FROM_ALL
                 ${${EXECUTABLE_2}_EXE_SRCS})
 add_executable (${EXECUTABLE_3}
                 EXCLUDE_FROM_ALL
                 ${${EXECUTABLE_3}_EXE_SRCS})
else ()
 add_executable (${EXECUTABLE}
                 EXCLUDE_FROM_ALL
                 ${${EXECUTABLE}_EXE_SRCS})
 add_executable (${EXECUTABLE_2}
                 EXCLUDE_FROM_ALL
                 ${${EXECUTABLE_2}_EXE_SRCS})
 add_executable (${EXECUTABLE_3}
                 EXCLUDE_FROM_ALL
                 ${${EXECUTABLE_3}_EXE_SRCS})
endif (WIN32)
set_property (TARGET ${EXECUTABLE} PROPERTY FOLDER "test_i")
set_property (TARGET ${EXECUTABLE_2} PROPERTY FOLDER "test_i")
set_property (TARGET ${EXECUTABLE_3} PROPERTY FOLDER "test_i")

# add PCH
target_precompile_headers (${EXECUTABLE}
                           PRIVATE stdafx.h)
target_precompile_headers (${EXECUTABLE_2}
                           REUSE_FROM ${EXECUTABLE})
target_precompile_headers (${EXECUTABLE_3}
                           REUSE_FROM ${EXECUTABLE})

target_link_libraries (${EXECUTABLE} PUBLIC ${wxWidgets_LIBRARIES})
target_link_libraries (${EXECUTABLE_2} PUBLIC ${wxWidgets_LIBRARIES})
target_link_libraries (${EXECUTABLE_3} PUBLIC ${wxWidgets_LIBRARIES})

target_link_libraries (${EXECUTABLE} PUBLIC ${ACE_LIBRARY})
target_link_libraries (${EXECUTABLE_2} PUBLIC ${ACE_LIBRARY})
target_link_libraries (${EXECUTABLE_3} PUBLIC ${ACE_LIBRARY})

target_link_libraries (${EXECUTABLE_2} PUBLIC Common)

target_link_libraries (${EXECUTABLE_2} PUBLIC CommonLog)
target_link_libraries (${EXECUTABLE_3} PUBLIC CommonLog)

target_link_libraries (${EXECUTABLE_3} PUBLIC CommonUI_wxWidgets)

target_link_libraries (${EXECUTABLE} PUBLIC Common_Test_I)
target_link_libraries (${EXECUTABLE_2} PUBLIC Common_Test_I)
target_link_libraries (${EXECUTABLE_3} PUBLIC Common_Test_I)

############### installation #############
install (TARGETS ${EXECUTABLE}
         EXPORT ${PROJECT_NAME}Targets
         RUNTIME DESTINATION ${INSTALL_BIN_DIR} COMPONENT test_i)
if (WIN32)
 if (DEFINED ENV{LIB_ROOT})
  file (TO_NATIVE_PATH $ENV{LIB_ROOT} LIB_ROOT)
  string (REPLACE "\\" "\\\\" LIB_ROOT ${LIB_ROOT})
 endif (DEFINED ENV{LIB_ROOT})
 install (FILES ${LIB_ROOT}/wxWidgets/lib/vc_x64_dll/wxbase316d_vc_x64_custom.dll
          DESTINATION ${INSTALL_BIN_DIR} COMPONENT test_i)
endif (WIN32)
install (TARGETS ${EXECUTABLE_2}
         EXPORT ${PROJECT_NAME}Targets
         RUNTIME DESTINATION ${INSTALL_BIN_DIR} COMPONENT test_i)
install (FILES etc/test_i_2.xrc
         DESTINATION "etc/${EXECUTABLE}" COMPONENT config)
install (TARGETS ${EXECUTABLE_3}
         EXPORT ${PROJECT_NAME}Targets
         RUNTIME DESTINATION ${INSTALL_BIN_DIR} COMPONENT test_i)
install (FILES etc/test_i_3.xrc
         DESTINATION "etc/${EXECUTABLE}" COMPONENT config)

################ launchers ###############
file (TO_CMAKE_PATH "${wxWidgets_LIB_DIR}" wxWidgets_LIB_DIR)
if (UNIX)
 file (TO_NATIVE_PATH "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/${EXECUTABLE}" COMMAND)
 file (TO_NATIVE_PATH "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/${EXECUTABLE_2}" COMMAND_2)
 file (TO_NATIVE_PATH "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/${EXECUTABLE_3}" COMMAND_3)
# file (TO_CMAKE_PATH ${VCPKG_ROOT}/installed/${VCPKG_TARGET_TRIPLET}/bin ACE_LIB_PATH)
elseif (WIN32)
 file (TO_CMAKE_PATH ${ACE_LIB_DIR} ACE_LIB_PATH)
 file (TO_NATIVE_PATH "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/${EXECUTABLE}.exe" COMMAND)
 string (REPLACE "\\" "\\\\" COMMAND ${COMMAND})
 file (TO_NATIVE_PATH "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/${EXECUTABLE_2}.exe" COMMAND_2)
 string (REPLACE "\\" "\\\\" COMMAND_2 ${COMMAND_2})
 file (TO_NATIVE_PATH "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/${EXECUTABLE_3}.exe" COMMAND_3)
 string (REPLACE "\\" "\\\\" COMMAND_3 ${COMMAND_3})
endif ()
set (RUNTIME_LIBRARY_DIRS ${RUNTIME_LIBRARY_DIRS}
                          ${ACE_LIB_PATH})
create_target_launcher (${EXECUTABLE}
                        COMMAND ${COMMAND}
                        ARGS ""
#                         FORWARD_ARGS
                        RUNTIME_LIBRARY_DIRS ${RUNTIME_LIBRARY_DIRS}
                                             ${PROJECT_BINARY_DIR}/src/${CMAKE_BUILD_TYPE}
                                             ${wxWidgets_LIB_DIR}
                        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/test_i/ui/wxwidgets
                        ENVIRONMENT foo=bar)

create_target_launcher (${EXECUTABLE_2}
                        COMMAND ${COMMAND_2}
                        ARGS ""
#                        FORWARD_ARGS
                        RUNTIME_LIBRARY_DIRS ${RUNTIME_LIBRARY_DIRS}
                                             ${PROJECT_BINARY_DIR}/src/${CMAKE_BUILD_TYPE}
                                             ${wxWidgets_LIB_DIR}
                        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/test_i/ui/wxwidgets
                        ENVIRONMENT foo=bar)

create_target_launcher (${EXECUTABLE_3}
                        COMMAND ${COMMAND_3}
                        ARGS ""
#                        FORWARD_ARGS
                        RUNTIME_LIBRARY_DIRS ${RUNTIME_LIBRARY_DIRS}
                                             ${PROJECT_BINARY_DIR}/src/${CMAKE_BUILD_TYPE}
                                             ${wxWidgets_LIB_DIR}
                        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/test_i/ui/wxwidgets
                        ENVIRONMENT foo=bar)

################ tests ###################
add_test (NAME ${EXECUTABLE}_test
          COMMAND ${PROJECT_BINARY_DIR}/${EXECUTABLE}
          WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/test_i/ui/wxwidgets)
add_test (NAME ${EXECUTABLE_2}_test
          COMMAND ${PROJECT_BINARY_DIR}/${EXECUTABLE_2}
          WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/test_i/ui/wxwidgets)
add_test (NAME ${EXECUTABLE_3}_test
          COMMAND ${PROJECT_BINARY_DIR}/${EXECUTABLE_3}
          WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/test_i/ui/wxwidgets)

set_tests_properties (${EXECUTABLE}_test
                      PROPERTIES
                      ENVIRONMENT "PATH=D:\\projects\\ATCD\\ACE\\lib;D:\\projects\\gtk\\bin;D:\\projects\\libglade\\bin;D:\\projects\\libxml2-2.9.1\\.libs;G:\\software\\Development\\dll;${PROJECT_SOURCE_DIR}\\..\\libCommon\\cmake\\src\\Debug;${PROJECT_SOURCE_DIR}\\..\\libCommon\\cmake\\src\\ui\\Debug;${PROJECT_SOURCE_DIR}\\cmake\\src\\Debug;${PROJECT_SOURCE_DIR}\\cmake\\src\\modules\\dev\\Debug;${PROJECT_SOURCE_DIR}\\..\\libACENetwork\\cmake\\src\\Debug;%PATH%")
#PATH=%PATH%;$(ProjectDir)..\..\..\..\ATCD\ACE\lib;$(ProjectDir)..\..\..\..\gtk3\bin;$(ProjectDir)..\..\..\..\libav-11.7\libavcodec;$(ProjectDir)..\..\..\..\libav-11.7\libavresample;$(ProjectDir)..\..\..\..\libav-11.7\libavutil;$(ProjectDir)..\..\..\..\libCommon\cmake\src\Debug;$(ProjectDir)..\..\..\..\libCommon\cmake\src\err\Debug;$(ProjectDir)..\..\..\..\libCommon\cmake\src\ui\Debug;$(ProjectDir)..\..\src\Debug;$(ProjectDir)..\..\src\modules\dev\Debug;$(ProjectDir)..\..\src\modules\vis\Debug
set_tests_properties (${EXECUTABLE_2}_test
                      PROPERTIES
                      ENVIRONMENT "PATH=D:\\projects\\ATCD\\ACE\\lib;D:\\projects\\gtk\\bin;D:\\projects\\libglade\\bin;D:\\projects\\libxml2-2.9.1\\.libs;G:\\software\\Development\\dll;${PROJECT_SOURCE_DIR}\\..\\libCommon\\cmake\\src\\Debug;${PROJECT_SOURCE_DIR}\\..\\libCommon\\cmake\\src\\ui\\Debug;${PROJECT_SOURCE_DIR}\\cmake\\src\\Debug;${PROJECT_SOURCE_DIR}\\cmake\\src\\modules\\dev\\Debug;${PROJECT_SOURCE_DIR}\\..\\libACENetwork\\cmake\\src\\Debug;%PATH%")
#PATH=%PATH%;$(ProjectDir)..\..\..\..\ATCD\ACE\lib;$(ProjectDir)..\..\..\..\gtk3\bin;$(ProjectDir)..\..\..\..\libav-11.7\libavcodec;$(ProjectDir)..\..\..\..\libav-11.7\libavresample;$(ProjectDir)..\..\..\..\libav-11.7\libavutil;$(ProjectDir)..\..\..\..\libCommon\cmake\src\Debug;$(ProjectDir)..\..\..\..\libCommon\cmake\src\err\Debug;$(ProjectDir)..\..\..\..\libCommon\cmake\src\ui\Debug;$(ProjectDir)..\..\src\Debug;$(ProjectDir)..\..\src\modules\dev\Debug;$(ProjectDir)..\..\src\modules\vis\Debug
set_tests_properties (${EXECUTABLE_3}_test
                      PROPERTIES
                      ENVIRONMENT "PATH=D:\\projects\\ATCD\\ACE\\lib;D:\\projects\\gtk\\bin;D:\\projects\\libglade\\bin;D:\\projects\\libxml2-2.9.1\\.libs;G:\\software\\Development\\dll;${PROJECT_SOURCE_DIR}\\..\\libCommon\\cmake\\src\\Debug;${PROJECT_SOURCE_DIR}\\..\\libCommon\\cmake\\src\\ui\\Debug;${PROJECT_SOURCE_DIR}\\cmake\\src\\Debug;${PROJECT_SOURCE_DIR}\\cmake\\src\\modules\\dev\\Debug;${PROJECT_SOURCE_DIR}\\..\\libACENetwork\\cmake\\src\\Debug;%PATH%")
#PATH=%PATH%;$(ProjectDir)..\..\..\..\ATCD\ACE\lib;$(ProjectDir)..\..\..\..\gtk3\bin;$(ProjectDir)..\..\..\..\libav-11.7\libavcodec;$(ProjectDir)..\..\..\..\libav-11.7\libavresample;$(ProjectDir)..\..\..\..\libav-11.7\libavutil;$(ProjectDir)..\..\..\..\libCommon\cmake\src\Debug;$(ProjectDir)..\..\..\..\libCommon\cmake\src\err\Debug;$(ProjectDir)..\..\..\..\libCommon\cmake\src\ui\Debug;$(ProjectDir)..\..\src\Debug;$(ProjectDir)..\..\src\modules\dev\Debug;$(ProjectDir)..\..\src\modules\vis\Debug
