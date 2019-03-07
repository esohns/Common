include (${CMAKE_CURRENT_SOURCE_DIR}/../../../src/ui/scripts/SetUIFramework.cmake)
if (GTK_SUPPORT)
 include (${CMAKE_CURRENT_SOURCE_DIR}/../../../src/ui/scripts/SetGTKVersion.cmake)
endif (GTK_SUPPORT)

set (ACE_LIB_FILE libACE.so)
if (DEFINED ENV{ACE_ROOT})
 find_library (ACE_LIBRARY ${ACE_LIB_FILE}
               PATHS ENV ACE_ROOT
               PATH_SUFFIXES ace build/linux/ace
               DOC "searching for ${ACE_LIB_FILE}")
else ()
 find_library (ACE_LIBRARY ${ACE_LIB_FILE}
               DOC "searching for ${ACE_LIB_FILE} (system paths)")
endif ()
#if (NOT EXISTS ACE_LIBRARY)
if (NOT ACE_LIBRARY)
 message (FATAL_ERROR "could not find ${ACE_LIB_FILE} (was: \"${ACE_LIBRARY}\"), aborting")
endif ()

get_filename_component (BUILD_PATH ${CMAKE_CURRENT_BINARY_DIR}/../.. REALPATH DIRECTORY)
get_filename_component (BUILD_PATH_SUFFIX ${BUILD_PATH} NAME)
find_package (Common ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_MICRO}
              REQUIRED COMPONENTS lib
              CONFIG
              PATHS ${CMAKE_CURRENT_SOURCE_DIR}/../modules ${CMAKE_CURRENT_SOURCE_DIR}/../..
              PATH_SUFFIXES libCommon/${BUILD_PATH_SUFFIX}
              NO_DEFAULT_PATH)
if (NOT EXISTS ${Common_DIR})
 message (FATAL_ERROR "could not find package \"Common\", aborting")
endif ()

############## definitions ###############
add_definitions (-DACE_HAS_DLL)
add_definitions (-DHAVE_CONFIG_H)
set_property (SOURCE PROPERTY COMPILE_DEFINITIONS HAVE_CONFIG_H)
add_definitions (-DCOMMON_HAS_DLL)
add_definitions (-DCOMMON_TIMER_HAS_DLL)
############## includes ###############
include_directories (${GTK_INCLUDE_DIRS})
if (GTKGL_SUPPORT)
 if (GTKGLAREA_SUPPORT)
# *NOTE*: check out the 'master' branch for gtk3-based applications
# *NOTE*: check out the 'gtkglarea-2' branch for gtk2-based applications
# *TODO*: verify automatically
  include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../../gtkglarea)
 endif (GTKGLAREA_SUPPORT)
endif (GTKGL_SUPPORT)

#include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../modules/ACE)

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../src)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../src/image)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../src/log)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../src/timer)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../src/ui)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../src/ui/gtk)

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../..)

include_directories (${CMAKE_CURRENT_BINARY_DIR}/../../..)

############## next target ###############
set (gtk_ui_EXE_SRCS
     stdafx.cpp
     stdafx.h
     test_i.cpp
     test_i_gtk_callbacks.cpp
     test_i_gtk_callbacks.h
     test_i_gtk_defines.h)

#cmake_policy (SET CMP0065 OLD) # do not add flags to export symbols from executables without the ENABLE_EXPORTS target property
set (EXECUTABLE gtk_ui)
add_executable (${EXECUTABLE}
                EXCLUDE_FROM_ALL
                ${gtk_ui_EXE_SRCS})
set_property (TARGET ${EXECUTABLE} PROPERTY FOLDER "test_i")

if (UNIX)
 target_link_libraries (${EXECUTABLE} X11)
 target_link_libraries (${EXECUTABLE} GL)
elseif (WIN32)
 target_link_libraries (${EXECUTABLE} OpenGL32)
endif (UNIX)
target_link_libraries (${EXECUTABLE} ${GTK_LIBRARIES})
target_link_libraries (${EXECUTABLE} ${ACE_LIBRARY})

target_link_libraries (${EXECUTABLE} Common)
target_link_libraries (${EXECUTABLE} CommonLog)
target_link_libraries (${EXECUTABLE} CommonTimer)
target_link_libraries (${EXECUTABLE} CommonUI)
target_link_libraries (${EXECUTABLE} CommonUI_GTK)

# add PCH
set_target_properties (${EXECUTABLE} PROPERTIES COTIRE_CXX_PREFIX_HEADER_INIT "stdafx.h")
cotire (${EXECUTABLE})

############### installation #############
install (TARGETS ${EXECUTABLE}
         EXPORT ${PROJECT_NAME}Targets
         RUNTIME DESTINATION ${INSTALL_BIN_DIR} COMPONENT test_i)
# install (FILES etc/${EXECUTABLE}.glade
#          DESTINATION "${INSTALL_SHARE_DIR}/${PROJECT_NAME}/${EXECUTABLE}" COMPONENT config)

################ launchers ###############

set (ui_definition_file "")
if (GTK_USE)
 if (GTK3_USE)
  set (ui_definition_file ${EXECUTABLE}.gtk3)
 elseif (GTK2_USE)
  set (ui_definition_file ${EXECUTABLE}.gtk2)
 endif ()
elseif (WXWIDGETS_USE)
endif ()

if (UNIX)
 if (GTK_USE)
  set (ARGS "-g./etc/${ui_definition_file} ${ARGS}")
 elseif (WXWIDGETS_USE)
  set (ENVIRONMENT "DISPLAY=:0")
 endif ()
elseif (WIN32)
 if (GTK_USE)
  set (ARGS "-g.\\\\etc\\\\${ui_definition_file} ${ARGS}")
 elseif (WXWIDGETS_USE)
 endif ()
endif ()

file (TO_CMAKE_PATH $ENV{LIB_ROOT}/gtk3/bin GTK_LIB_PATH)
create_target_launcher (${EXECUTABLE}
                        ARGS ${ARGS}
#                         FORWARD_ARGS
                        RUNTIME_LIBRARY_DIRS ${GTK_LIB_PATH}
                                             ${PROJECT_SOURCE_DIR}/../ATCD/ACE/lib
                                             ${PROJECT_SOURCE_DIR}/cmake/src/Debug
                        WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/../test_i/ui/gtk
                        ENVIRONMENT ${ENVIRONMENT})

################ tests ###################
add_test (NAME ${EXECUTABLE}_test
          COMMAND ${PROJECT_BINARY_DIR}/${EXECUTABLE}
          WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
set_tests_properties (${EXECUTABLE}_test
                      PROPERTIES
                      ENVIRONMENT "PATH=D:\\projects\\ATCD\\ACE\\lib;D:\\projects\\gtk\\bin;D:\\projects\\libglade\\bin;D:\\projects\\libxml2-2.9.1\\.libs;G:\\software\\Development\\dll;${PROJECT_SOURCE_DIR}\\..\\libCommon\\cmake\\src\\Debug;${PROJECT_SOURCE_DIR}\\..\\libCommon\\cmake\\src\\ui\\Debug;${PROJECT_SOURCE_DIR}\\cmake\\src\\Debug;${PROJECT_SOURCE_DIR}\\cmake\\src\\modules\\dev\\Debug;${PROJECT_SOURCE_DIR}\\..\\libACENetwork\\cmake\\src\\Debug;%PATH%")
#PATH=%PATH%;$(ProjectDir)..\..\..\..\ATCD\ACE\lib;$(ProjectDir)..\..\..\..\gtk3\bin;$(ProjectDir)..\..\..\..\libav-11.7\libavcodec;$(ProjectDir)..\..\..\..\libav-11.7\libavresample;$(ProjectDir)..\..\..\..\libav-11.7\libavutil;$(ProjectDir)..\..\..\..\libCommon\cmake\src\Debug;$(ProjectDir)..\..\..\..\libCommon\cmake\src\err\Debug;$(ProjectDir)..\..\..\..\libCommon\cmake\src\ui\Debug;$(ProjectDir)..\..\src\Debug;$(ProjectDir)..\..\src\modules\dev\Debug;$(ProjectDir)..\..\src\modules\vis\Debug

