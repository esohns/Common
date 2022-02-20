string (REPLACE ${CMAKE_SOURCE_DIR} "" BUILD_PATH_SUFFIX ${CMAKE_BINARY_DIR})
string (SUBSTRING ${BUILD_PATH_SUFFIX} 1 -1 BUILD_PATH_SUFFIX)

# Find includes in corresponding build directories
set (CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed
set (CMAKE_AUTOMOC ON)
# Create code from a list of Qt designer ui files
set (CMAKE_AUTOUIC ON)

include_directories (${ACE_INCLUDE_DIR})

include_directories (${CMAKE_BINARY_DIR})

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../src)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../src/ui)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../src/ui/qt)

# Populate a CMake variable with the sources
set (qt_ui_SRCS
#     stdafx.cpp
     stdafx.h
     mainwindow.ui
     mainwindow.cpp
     mainwindow.h
     test_u.cpp)
set (qt_ui_2_SRCS
#     stdafx.cpp
     stdafx.h
     mainwindow.ui
     mainwindow.cpp
     mainwindow.h
     mainwindow_2.cpp
     mainwindow_2.h
     mainwindow_2.inl
     test_u_2.cpp)

# Tell CMake to create the helloworld executable
add_executable (qt_ui ${qt_ui_SRCS})
add_executable (qt_ui_2 ${qt_ui_2_SRCS})

set_property (TARGET qt_ui PROPERTY FOLDER "test_i")
set_property (TARGET qt_ui_2 PROPERTY FOLDER "test_i")

target_link_libraries (qt_ui PUBLIC ${QT_LIBRARIES})

if (UNIX)
 target_link_libraries (qt_ui_2 PUBLIC ${CMAKE_THREAD_LIBS_INIT})
# target_link_libraries (qt_ui_2 PUBLIC c)
endif (UNIX)

target_link_libraries (qt_ui_2 PUBLIC ${QT_LIBRARIES})

target_link_libraries (qt_ui_2 PUBLIC ${ACE_LIBRARY})

target_link_libraries (qt_ui_2 PUBLIC CommonUI_Qt)

# add PCH
target_precompile_headers (qt_ui
                           PUBLIC stdafx.h)
target_precompile_headers (qt_ui_2
                           REUSE_FROM qt_ui)

############### installation #############
install (TARGETS qt_ui
         EXPORT ${PROJECT_NAME}Targets
         RUNTIME DESTINATION ${INSTALL_BIN_DIR} COMPONENT test_i)
install (TARGETS qt_ui_2
         EXPORT ${PROJECT_NAME}Targets
         RUNTIME DESTINATION ${INSTALL_BIN_DIR} COMPONENT test_i)

################ launchers ###############
file (TO_CMAKE_PATH "${QT5_LIB_DIR}" QT_LIB_DIR)
if (UNIX)
# file (TO_CMAKE_PATH ${VCPKG_ROOT}/installed/${VCPKG_TARGET_TRIPLET}/bin ACE_LIB_PATH)
elseif (WIN32)
 if (VCPKG_SUPPORT)
  file (TO_CMAKE_PATH ${VCPKG_ROOT}/installed/${VCPKG_TARGET_TRIPLET}/bin ACE_LIB_PATH)
 else ()
  file (TO_CMAKE_PATH $ENV{LIB_ROOT}/ACE_TAO/ACE/lib ACE_LIB_PATH)
 endif (VCPKG_SUPPORT)
endif ()
set (RUNTIME_LIBRARY_DIRS ${RUNTIME_LIBRARY_DIRS}
                          ${ACE_LIB_PATH})
create_target_launcher (qt_ui
                        ARGS ""
#                         FORWARD_ARGS
                        RUNTIME_LIBRARY_DIRS ${RUNTIME_LIBRARY_DIRS}
                                             ${PROJECT_BINARY_DIR}/src/${CMAKE_BUILD_TYPE}
                                             ${QT_LIB_DIR}
                        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/test_i/ui/qt
                        ENVIRONMENT foo=bar)

create_target_launcher (qt_ui_2
                        ARGS ""
#                        FORWARD_ARGS
                        RUNTIME_LIBRARY_DIRS ${RUNTIME_LIBRARY_DIRS}
                                             ${PROJECT_BINARY_DIR}/src/${CMAKE_BUILD_TYPE}
                                             ${QT_LIB_DIR}
                        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/test_i/ui/qt
                        ENVIRONMENT foo=bar)

################ tests ###################
add_test (NAME qt_ui_test
          COMMAND ${PROJECT_BINARY_DIR}/qt_ui
          WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/test_i/ui/qt)
add_test (NAME qt_ui_2_test
          COMMAND ${PROJECT_BINARY_DIR}/qt_ui_2
          WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/test_i/ui/qt)

set_tests_properties (qt_ui_test
                      PROPERTIES
                      ENVIRONMENT "PATH=D:\\projects\\ATCD\\ACE\\lib;D:\\projects\\gtk\\bin;D:\\projects\\libglade\\bin;D:\\projects\\libxml2-2.9.1\\.libs;G:\\software\\Development\\dll;${PROJECT_SOURCE_DIR}\\..\\libCommon\\cmake\\src\\Debug;${PROJECT_SOURCE_DIR}\\..\\libCommon\\cmake\\src\\ui\\Debug;${PROJECT_SOURCE_DIR}\\cmake\\src\\Debug;${PROJECT_SOURCE_DIR}\\cmake\\src\\modules\\dev\\Debug;${PROJECT_SOURCE_DIR}\\..\\libACENetwork\\cmake\\src\\Debug;%PATH%")
#PATH=%PATH%;$(ProjectDir)..\..\..\..\ATCD\ACE\lib;$(ProjectDir)..\..\..\..\gtk3\bin;$(ProjectDir)..\..\..\..\libav-11.7\libavcodec;$(ProjectDir)..\..\..\..\libav-11.7\libavresample;$(ProjectDir)..\..\..\..\libav-11.7\libavutil;$(ProjectDir)..\..\..\..\libCommon\cmake\src\Debug;$(ProjectDir)..\..\..\..\libCommon\cmake\src\err\Debug;$(ProjectDir)..\..\..\..\libCommon\cmake\src\ui\Debug;$(ProjectDir)..\..\src\Debug;$(ProjectDir)..\..\src\modules\dev\Debug;$(ProjectDir)..\..\src\modules\vis\Debug
set_tests_properties (qt_ui_2_test
                      PROPERTIES
                      ENVIRONMENT "PATH=D:\\projects\\ATCD\\ACE\\lib;D:\\projects\\gtk\\bin;D:\\projects\\libglade\\bin;D:\\projects\\libxml2-2.9.1\\.libs;G:\\software\\Development\\dll;${PROJECT_SOURCE_DIR}\\..\\libCommon\\cmake\\src\\Debug;${PROJECT_SOURCE_DIR}\\..\\libCommon\\cmake\\src\\ui\\Debug;${PROJECT_SOURCE_DIR}\\cmake\\src\\Debug;${PROJECT_SOURCE_DIR}\\cmake\\src\\modules\\dev\\Debug;${PROJECT_SOURCE_DIR}\\..\\libACENetwork\\cmake\\src\\Debug;%PATH%")
#PATH=%PATH%;$(ProjectDir)..\..\..\..\ATCD\ACE\lib;$(ProjectDir)..\..\..\..\gtk3\bin;$(ProjectDir)..\..\..\..\libav-11.7\libavcodec;$(ProjectDir)..\..\..\..\libav-11.7\libavresample;$(ProjectDir)..\..\..\..\libav-11.7\libavutil;$(ProjectDir)..\..\..\..\libCommon\cmake\src\Debug;$(ProjectDir)..\..\..\..\libCommon\cmake\src\err\Debug;$(ProjectDir)..\..\..\..\libCommon\cmake\src\ui\Debug;$(ProjectDir)..\..\src\Debug;$(ProjectDir)..\..\src\modules\dev\Debug;$(ProjectDir)..\..\src\modules\vis\Debug
