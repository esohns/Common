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
set (wxwidgets_EXE_SRCS
     stdafx.cpp
     stdafx.h
#     ../test_u_common.h
#     ../test_u_defines.h
     test_u.cpp)

set (wxwidgets_EXE_2_SRCS
     stdafx.cpp
     stdafx.h
#     ../test_u_common.h
#     ../test_u_defines.h
     test_u_2.cpp
     test_u_2.h)

set (wxwidgets_EXE_3_SRCS
     stdafx.cpp
     stdafx.h
#     ../test_u_common.h
#     ../test_u_defines.h
     test_u_3.cpp
     test_u_3.h
     test_u_3_ui.cpp
     test_u_3_ui.h
     test_u_3_ui.inl
     test_u_3_ui_base.cpp
     test_u_3_ui_base.h)

#cmake_policy (SET CMP0065 OLD) # do not add flags to export symbols from executables without the ENABLE_EXPORTS target property
set (EXECUTABLE wxwidgets)
set (EXECUTABLE_2 wxwidgets_2)
set (EXECUTABLE_3 wxwidgets_3)
if (WIN32)
 add_executable (${EXECUTABLE} WIN32
                 EXCLUDE_FROM_ALL
                 ${wxwidgets_EXE_SRCS})
 add_executable (${EXECUTABLE_2} WIN32
                 EXCLUDE_FROM_ALL
                 ${wxwidgets_EXE_2_SRCS})
 add_executable (${EXECUTABLE_3}
                 EXCLUDE_FROM_ALL
                 ${wxwidgets_EXE_3_SRCS})
else ()
 add_executable (${EXECUTABLE}
                 EXCLUDE_FROM_ALL
                 ${wxwidgets_EXE_SRCS})
 add_executable (${EXECUTABLE_2}
                 EXCLUDE_FROM_ALL
                 ${wxwidgets_EXE_2_SRCS})
 add_executable (${EXECUTABLE_3}
                 EXCLUDE_FROM_ALL
                 ${wxwidgets_EXE_3_SRCS})
endif (WIN32)
set_property (TARGET ${EXECUTABLE} PROPERTY FOLDER "test_i")
set_property (TARGET ${EXECUTABLE_2} PROPERTY FOLDER "test_i")
set_property (TARGET ${EXECUTABLE_3} PROPERTY FOLDER "test_i")

target_link_libraries (${EXECUTABLE} ${wxWidgets_LIBRARIES})
target_link_libraries (${EXECUTABLE_2} ${wxWidgets_LIBRARIES})
target_link_libraries (${EXECUTABLE_3} ${wxWidgets_LIBRARIES})

target_link_libraries (${EXECUTABLE_2} ${ACE_LIBRARY})
target_link_libraries (${EXECUTABLE_3} ${ACE_LIBRARY})

target_link_libraries (${EXECUTABLE_3} CommonUI_wxWidgets)

# add PCH
set_target_properties (${EXECUTABLE} PROPERTIES COTIRE_CXX_PREFIX_HEADER_INIT "stdafx.h")
cotire (${EXECUTABLE})
#set_target_properties (${EXECUTABLE_2} PROPERTIES COTIRE_CXX_PREFIX_HEADER_INIT "stdafx.h")
#cotire (${EXECUTABLE_2})
#set_target_properties (${EXECUTABLE_3} PROPERTIES COTIRE_CXX_PREFIX_HEADER_INIT "stdafx.h")
#cotire (${EXECUTABLE_3})

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
# install (FILES etc/${EXECUTABLE}.glade
#          DESTINATION "${INSTALL_SHARE_DIR}/${PROJECT_NAME}/${EXECUTABLE}" COMPONENT config)

################ launchers ###############
file (TO_CMAKE_PATH "${wxWidgets_LIB_DIR}" wxWidgets_LIB_DIR)
if (UNIX)
 file (TO_CMAKE_PATH ${VCPKG_ROOT}/installed/bin ACE_LIB_PATH)
elseif (WIN32)
 if (VCPKG_SUPPORT)
  file (TO_CMAKE_PATH ${VCPKG_ROOT}/installed/bin ACE_LIB_PATH)
 else ()
  file (TO_CMAKE_PATH $ENV{LIB_ROOT}/ACE_TAO/ACE/lib ACE_LIB_PATH)
 endif (VCPKG_SUPPORT)
endif ()
set (RUNTIME_LIBRARY_DIRS ${RUNTIME_LIBRARY_DIRS}
                          ${ACE_LIB_PATH})
create_target_launcher (${EXECUTABLE}
                        ARGS ""
#                         FORWARD_ARGS
                        RUNTIME_LIBRARY_DIRS ${RUNTIME_LIBRARY_DIRS}
                                             ${PROJECT_BINARY_DIR}/src/${CMAKE_BUILD_TYPE}
                                             ${wxWidgets_LIB_DIR}
                        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/test_i/ui/${EXECUTABLE}
                        ENVIRONMENT foo=bar)

create_target_launcher (${EXECUTABLE_2}
                        ARGS ""
#                        FORWARD_ARGS
                        RUNTIME_LIBRARY_DIRS ${RUNTIME_LIBRARY_DIRS}
                                             ${PROJECT_BINARY_DIR}/src/${CMAKE_BUILD_TYPE}
                                             ${wxWidgets_LIB_DIR}
                        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/test_i/ui/${EXECUTABLE}
                        ENVIRONMENT foo=bar)

create_target_launcher (${EXECUTABLE_3}
                        ARGS ""
#                        FORWARD_ARGS
                        RUNTIME_LIBRARY_DIRS ${RUNTIME_LIBRARY_DIRS}
                                             ${PROJECT_BINARY_DIR}/src/${CMAKE_BUILD_TYPE}
                                             ${wxWidgets_LIB_DIR}
                        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/test_i/ui/${EXECUTABLE}
                        ENVIRONMENT foo=bar)

################ tests ###################
add_test (NAME ${EXECUTABLE}_test
          COMMAND ${PROJECT_BINARY_DIR}/${EXECUTABLE}
          WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/test_i/ui/${EXECUTABLE})
add_test (NAME ${EXECUTABLE_2}_test
          COMMAND ${PROJECT_BINARY_DIR}/${EXECUTABLE_2}
          WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/test_i/ui/${EXECUTABLE})
add_test (NAME ${EXECUTABLE_3}_test
          COMMAND ${PROJECT_BINARY_DIR}/${EXECUTABLE_3}
          WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/test_i/ui/${EXECUTABLE})

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
