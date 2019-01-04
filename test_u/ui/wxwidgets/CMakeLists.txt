
############## definitions ###############
#add_definitions (-DACE_HAS_DLL)
#add_definitions (-DHAVE_CONFIG_H)
#set_property (SOURCE PROPERTY COMPILE_DEFINITIONS HAVE_CONFIG_H)
#add_definitions (-DCOMMON_HAS_DLL)
#add_definitions (-DCOMMON_TIMER_HAS_DLL)

############## includes ###############
#include_directories (${PKG_CAP_INCLUDE_DIRS})

if (UNIX)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../modules/ACE)
elseif (WIN32)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../../ATCD/ACE)
endif ()

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../src)
#include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../src/timer)

include_directories (${CMAKE_CURRENT_BINARY_DIR}/../../..)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../..)

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
     test_u_2.cpp)

#cmake_policy (SET CMP0065 OLD) # do not add flags to export symbols from executables without the ENABLE_EXPORTS target property
set (EXECUTABLE wxwidgets)
set (EXECUTABLE_2 wxwidgets_2)
if (WIN32)
 add_executable (${EXECUTABLE} WIN32
                 EXCLUDE_FROM_ALL
                 ${wxwidgets_EXE_SRCS})
else ()
 add_executable (${EXECUTABLE}
                 EXCLUDE_FROM_ALL
                 ${wxwidgets_EXE_SRCS})
endif (WIN32)
add_executable (${EXECUTABLE_2}
                EXCLUDE_FROM_ALL
                ${wxwidgets_EXE_2_SRCS})
set_property (TARGET ${EXECUTABLE} PROPERTY FOLDER "test_u")
set_property (TARGET ${EXECUTABLE_2} PROPERTY FOLDER "test_u")

target_link_libraries (${EXECUTABLE_2} ${ACE_LIBRARY})

target_link_libraries (${EXECUTABLE} ${wxWidgets_LIBRARIES})
target_link_libraries (${EXECUTABLE_2} ${wxWidgets_LIBRARIES})

# add PCH
set_target_properties (${EXECUTABLE} PROPERTIES COTIRE_CXX_PREFIX_HEADER_INIT "stdafx.h")
cotire (${EXECUTABLE})
set_target_properties (${EXECUTABLE_2} PROPERTIES COTIRE_CXX_PREFIX_HEADER_INIT "stdafx.h")
cotire (${EXECUTABLE_2})

############### installation #############
install (TARGETS ${EXECUTABLE}
         EXPORT ${PROJECT_NAME}Targets
         RUNTIME DESTINATION ${INSTALL_BIN_DIR} COMPONENT test_u)
# install (FILES etc/${EXECUTABLE}.glade
#          DESTINATION "${INSTALL_SHARE_DIR}/${PROJECT_NAME}/${EXECUTABLE}" COMPONENT config)

################ launchers ###############
create_target_launcher (${EXECUTABLE}
                        ARGS ""
#                         FORWARD_ARGS
                        RUNTIME_LIBRARY_DIRS ${PROJECT_SOURCE_DIR}/modules/ATCD/ACE/lib
                                             ${PROJECT_SOURCE_DIR}/cmake/src/Debug
                                             ${wxWidgets_LIB_DIR}
                        WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/../test_u/ui/wxwidgets
                        ENVIRONMENT foo=bar)

################ tests ###################
add_test (NAME ${EXECUTABLE}_test
          COMMAND ${PROJECT_BINARY_DIR}/${EXECUTABLE}
          WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
add_test (NAME ${EXECUTABLE_2}_test
          COMMAND ${PROJECT_BINARY_DIR}/${EXECUTABLE_2}
          WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
set_tests_properties (${EXECUTABLE}_test
                      PROPERTIES
                      ENVIRONMENT "PATH=D:\\projects\\ATCD\\ACE\\lib;D:\\projects\\gtk\\bin;D:\\projects\\libglade\\bin;D:\\projects\\libxml2-2.9.1\\.libs;G:\\software\\Development\\dll;${PROJECT_SOURCE_DIR}\\..\\libCommon\\cmake\\src\\Debug;${PROJECT_SOURCE_DIR}\\..\\libCommon\\cmake\\src\\ui\\Debug;${PROJECT_SOURCE_DIR}\\cmake\\src\\Debug;${PROJECT_SOURCE_DIR}\\cmake\\src\\modules\\dev\\Debug;${PROJECT_SOURCE_DIR}\\..\\libACENetwork\\cmake\\src\\Debug;%PATH%")
#PATH=%PATH%;$(ProjectDir)..\..\..\..\ATCD\ACE\lib;$(ProjectDir)..\..\..\..\gtk3\bin;$(ProjectDir)..\..\..\..\libav-11.7\libavcodec;$(ProjectDir)..\..\..\..\libav-11.7\libavresample;$(ProjectDir)..\..\..\..\libav-11.7\libavutil;$(ProjectDir)..\..\..\..\libCommon\cmake\src\Debug;$(ProjectDir)..\..\..\..\libCommon\cmake\src\err\Debug;$(ProjectDir)..\..\..\..\libCommon\cmake\src\ui\Debug;$(ProjectDir)..\..\src\Debug;$(ProjectDir)..\..\src\modules\dev\Debug;$(ProjectDir)..\..\src\modules\vis\Debug
set_tests_properties (${EXECUTABLE_2}_test
                      PROPERTIES
                      ENVIRONMENT "PATH=D:\\projects\\ATCD\\ACE\\lib;D:\\projects\\gtk\\bin;D:\\projects\\libglade\\bin;D:\\projects\\libxml2-2.9.1\\.libs;G:\\software\\Development\\dll;${PROJECT_SOURCE_DIR}\\..\\libCommon\\cmake\\src\\Debug;${PROJECT_SOURCE_DIR}\\..\\libCommon\\cmake\\src\\ui\\Debug;${PROJECT_SOURCE_DIR}\\cmake\\src\\Debug;${PROJECT_SOURCE_DIR}\\cmake\\src\\modules\\dev\\Debug;${PROJECT_SOURCE_DIR}\\..\\libACENetwork\\cmake\\src\\Debug;%PATH%")
#PATH=%PATH%;$(ProjectDir)..\..\..\..\ATCD\ACE\lib;$(ProjectDir)..\..\..\..\gtk3\bin;$(ProjectDir)..\..\..\..\libav-11.7\libavcodec;$(ProjectDir)..\..\..\..\libav-11.7\libavresample;$(ProjectDir)..\..\..\..\libav-11.7\libavutil;$(ProjectDir)..\..\..\..\libCommon\cmake\src\Debug;$(ProjectDir)..\..\..\..\libCommon\cmake\src\err\Debug;$(ProjectDir)..\..\..\..\libCommon\cmake\src\ui\Debug;$(ProjectDir)..\..\src\Debug;$(ProjectDir)..\..\src\modules\dev\Debug;$(ProjectDir)..\..\src\modules\vis\Debug
