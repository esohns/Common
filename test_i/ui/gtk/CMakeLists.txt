include (PreUIFramework)
include (SetUIFramework)
if (GTK_SUPPORT)
 include (SetGTKVersion)
endif (GTK_SUPPORT)

############## includes ###############
include_directories (${GTK_INCLUDE_DIRS})
if (GTKGL_SUPPORT)
 if (GTKGLAREA_SUPPORT)
# *NOTE*: check out the 'master' branch for gtk3-based applications
# *NOTE*: check out the 'gtkglarea-2' branch for gtk2-based applications
# *TODO*: verify automatically
  include_directories ($ENV{LIB_ROOT}/gtkglarea)
 endif (GTKGLAREA_SUPPORT)
endif (GTKGL_SUPPORT)

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../src)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../src/gl)
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
     test_i_gtk_defines.h)

#cmake_policy (SET CMP0065 OLD) # do not add flags to export symbols from executables without the ENABLE_EXPORTS target property
set (EXECUTABLE gtk_ui)
add_executable (${EXECUTABLE}
                EXCLUDE_FROM_ALL
                ${gtk_ui_EXE_SRCS})
set_property (TARGET ${EXECUTABLE} PROPERTY FOLDER "test_i")

get_filename_component (BUILD_PATH ${CMAKE_CURRENT_BINARY_DIR}/../../.. REALPATH DIRECTORY)
get_filename_component (BUILD_PATH_SUFFIX ${BUILD_PATH} NAME)
if (UNIX)
 target_link_libraries (${EXECUTABLE} X11)
 target_link_libraries (${EXECUTABLE} GL)
elseif (WIN32)
 target_link_libraries (${EXECUTABLE} OpenGL32)
endif ()
target_link_libraries (${EXECUTABLE} ${GTK_LIBRARIES})
if (UNIX)
elseif (WIN32)
 if (GTKGLAREA_SUPPORT)
  target_link_libraries (${EXECUTABLE} $ENV{LIB_ROOT}/gtkglarea/${BUILD_PATH_SUFFIX}/${CMAKE_BUILD_TYPE}/gtkglarea.lib)
 endif (GTKGLAREA_SUPPORT)
endif ()

target_link_libraries (${EXECUTABLE} ${ACE_LIBRARY})

target_link_libraries (${EXECUTABLE} Common)
target_link_libraries (${EXECUTABLE} CommonLog)
target_link_libraries (${EXECUTABLE} CommonTimer)
target_link_libraries (${EXECUTABLE} CommonUI)
target_link_libraries (${EXECUTABLE} CommonUI_GTK)

# add PCH
#set_target_properties (${EXECUTABLE} PROPERTIES COTIRE_CXX_PREFIX_HEADER_INIT "stdafx.h")
#cotire (${EXECUTABLE})

############## next target ###############
set (gtk_ui_2_EXE_SRCS
     stdafx.cpp
     stdafx.h
     test_i_2.cpp
     test_i_gtk_callbacks.cpp
     test_i_gtk_callbacks.h
     test_i_gtk_defines.h)

#cmake_policy (SET CMP0065 OLD) # do not add flags to export symbols from executables without the ENABLE_EXPORTS target property
set (EXECUTABLE_2 gtk_ui_2)
add_executable (${EXECUTABLE_2}
                EXCLUDE_FROM_ALL
                ${gtk_ui_2_EXE_SRCS})
set_property (TARGET ${EXECUTABLE_2} PROPERTY FOLDER "test_i")

if (UNIX)
 target_link_libraries (${EXECUTABLE_2} X11)
 target_link_libraries (${EXECUTABLE_2} GL)

 target_link_libraries (${EXECUTABLE_2} ${PKG_FFMPEG_LIBRARIES})
elseif (WIN32)
 set_target_properties (${EXECUTABLE_2} PROPERTIES LINK_FLAGS /SAFESEH:NO)

 target_link_libraries (${EXECUTABLE_2} OpenGL32)
endif (UNIX)
target_link_libraries (${EXECUTABLE_2} ${GTK_LIBRARIES})
if (UNIX)
elseif (WIN32)
 if (GTKGLAREA_SUPPORT)
  target_link_libraries (${EXECUTABLE_2} $ENV{LIB_ROOT}/gtkglarea/${BUILD_PATH_SUFFIX}/${CMAKE_BUILD_TYPE}/gtkglarea.lib)
 endif (GTKGLAREA_SUPPORT)
endif ()

target_link_libraries (${EXECUTABLE_2} ${ACE_LIBRARY})

target_link_libraries (${EXECUTABLE_2} Common)
target_link_libraries (${EXECUTABLE_2} CommonOpenGL)
target_link_libraries (${EXECUTABLE_2} CommonLog)
target_link_libraries (${EXECUTABLE_2} CommonTimer)
target_link_libraries (${EXECUTABLE_2} CommonUI)
target_link_libraries (${EXECUTABLE_2} CommonUI_GTK)

# add PCH
#set_target_properties (${EXECUTABLE_2} PROPERTIES COTIRE_CXX_PREFIX_HEADER_INIT "stdafx.h")
#cotire (${EXECUTABLE_2})

############## next target ###############
set (gtk_ui_3_EXE_SRCS
     stdafx.cpp
     stdafx.h
     test_i_3.cpp
     test_i_gtk_callbacks.cpp
     test_i_gtk_callbacks.h
     test_i_gtk_defines.h)

#cmake_policy (SET CMP0065 OLD) # do not add flags to export symbols from executables without the ENABLE_EXPORTS target property
set (EXECUTABLE_3 gtk_ui_3)
add_executable (${EXECUTABLE_3}
                EXCLUDE_FROM_ALL
                ${gtk_ui_3_EXE_SRCS})
set_property (TARGET ${EXECUTABLE_3} PROPERTY FOLDER "test_i")

if (UNIX)
 target_link_libraries (${EXECUTABLE_3} X11)
 target_link_libraries (${EXECUTABLE_3} GL)

 target_link_libraries (${EXECUTABLE_3} ${PKG_FFMPEG_LIBRARIES})
elseif (WIN32)
 set_target_properties (${EXECUTABLE_3} PROPERTIES LINK_FLAGS /SAFESEH:NO)

 target_link_libraries (${EXECUTABLE_3} OpenGL32)
endif (UNIX)
target_link_libraries (${EXECUTABLE_3} ${GTK_LIBRARIES})
if (UNIX)
elseif (WIN32)
 if (GTKGLAREA_SUPPORT)
  target_link_libraries (${EXECUTABLE_3} $ENV{LIB_ROOT}/gtkglarea/${BUILD_PATH_SUFFIX}/${CMAKE_BUILD_TYPE}/gtkglarea.lib)
 endif (GTKGLAREA_SUPPORT)
endif ()

target_link_libraries (${EXECUTABLE_3} ${ACE_LIBRARY})

target_link_libraries (${EXECUTABLE_3} Common)
target_link_libraries (${EXECUTABLE_3} CommonLog)
target_link_libraries (${EXECUTABLE_3} CommonOpenGL)
target_link_libraries (${EXECUTABLE_3} CommonTimer)
target_link_libraries (${EXECUTABLE_3} CommonUI)
target_link_libraries (${EXECUTABLE_3} CommonUI_GTK)

# add PCH
#set_target_properties (${EXECUTABLE_3} PROPERTIES COTIRE_CXX_PREFIX_HEADER_INIT "stdafx.h")
#cotire (${EXECUTABLE_3})

############### installation #############
install (TARGETS ${EXECUTABLE} ${EXECUTABLE_2} ${EXECUTABLE_3}
         EXPORT ${PROJECT_NAME}Targets
         RUNTIME DESTINATION ${INSTALL_BIN_DIR} COMPONENT test_i)
install (FILES etc/${EXECUTABLE}.glade etc/${EXECUTABLE}.gtk2 etc/${EXECUTABLE}.gtk3
         DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}" COMPONENT config)

################ launchers ###############

set (ui_definition_file "")
if (GTK_USE)
 if (GTK3_USE)
  set (ui_definition_file ${EXECUTABLE}.gtk3)
  file (TO_CMAKE_PATH $ENV{LIB_ROOT}/gtk3/bin GTK_LIB_PATH)
 elseif (GTK2_USE)
  set (ui_definition_file ${EXECUTABLE}.gtk2)
  file (TO_CMAKE_PATH $ENV{LIB_ROOT}/gtk2/bin GTK_LIB_PATH)
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

file (TO_CMAKE_PATH $ENV{LIB_ROOT}/ACE_TAO/ACE/lib ACE_LIB_PATH)
file (TO_CMAKE_PATH $ENV{LIB_ROOT}/assimp/cmake/code/${CMAKE_BUILD_TYPE} ASSIMP_LIB_PATH)
file (TO_CMAKE_PATH $ENV{LIB_ROOT}/ffmpeg/bin FFMPEG_LIB_PATH)
file (TO_CMAKE_PATH $ENV{LIB_ROOT}/ImageMagick IMAGEMAGICK_LIB_PATH)
create_target_launcher (${EXECUTABLE}
                        ARGS ${ARGS}
#                         FORWARD_ARGS
                        RUNTIME_LIBRARY_DIRS ${GTK_LIB_PATH}
                                             ${ACE_LIB_PATH}
                                             ${PROJECT_SOURCE_DIR}/cmake/src/Debug
                        WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/../test_i/ui/gtk
                        ENVIRONMENT ${ENVIRONMENT})

create_target_launcher (${EXECUTABLE_2}
                        ARGS ${ARGS}
#                         FORWARD_ARGS
                        RUNTIME_LIBRARY_DIRS ${GTK_LIB_PATH}
                                             ${ASSIMP_LIB_PATH}
                                             ${FFMPEG_LIB_PATH}
                                             ${IMAGEMAGICK_LIB_PATH}
                                             ${ACE_LIB_PATH}
                                             ${PROJECT_SOURCE_DIR}/cmake/src/Debug
                        WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/../test_i/ui/gtk
                        ENVIRONMENT ${ENVIRONMENT})

create_target_launcher (${EXECUTABLE_3}
                        ARGS ${ARGS}
#                         FORWARD_ARGS
                        RUNTIME_LIBRARY_DIRS ${GTK_LIB_PATH}
                                             ${ASSIMP_LIB_PATH}
                                             ${FFMPEG_LIB_PATH}
                                             ${IMAGEMAGICK_LIB_PATH}
                                             ${ACE_LIB_PATH}
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

add_test (NAME ${EXECUTABLE_2}_test
          COMMAND ${PROJECT_BINARY_DIR}/${EXECUTABLE_2}
          WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
set_tests_properties (${EXECUTABLE_2}_test
                      PROPERTIES
                      ENVIRONMENT "PATH=D:\\projects\\ATCD\\ACE\\lib;D:\\projects\\gtk\\bin;D:\\projects\\libglade\\bin;D:\\projects\\libxml2-2.9.1\\.libs;G:\\software\\Development\\dll;${PROJECT_SOURCE_DIR}\\..\\libCommon\\cmake\\src\\Debug;${PROJECT_SOURCE_DIR}\\..\\libCommon\\cmake\\src\\ui\\Debug;${PROJECT_SOURCE_DIR}\\cmake\\src\\Debug;${PROJECT_SOURCE_DIR}\\cmake\\src\\modules\\dev\\Debug;${PROJECT_SOURCE_DIR}\\..\\libACENetwork\\cmake\\src\\Debug;%PATH%")
