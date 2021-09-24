if (GTKGL_SUPPORT)
 set (CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/../../../src/gl/scripts")
 include (assimp)
endif (GTKGL_SUPPORT)

get_filename_component (BUILD_PATH ${CMAKE_CURRENT_BINARY_DIR}/../../.. REALPATH)
get_filename_component (BUILD_PATH_SUFFIX ${BUILD_PATH} NAME)

############## includes ###############
include_directories (${GTK_INCLUDE_DIRS})

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../src)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../src/gl)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../src/image)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../src/log)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../src/timer)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../src/ui)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../src/ui/gtk)

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../..)

include_directories (${CMAKE_CURRENT_BINARY_DIR}/../../..)

link_directories (${GTK_LIB_DIRS})

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

if (GTKGL_SUPPORT)
 target_link_libraries (${EXECUTABLE} PUBLIC ${OPENGL_LIBRARIES})
endif (GTKGL_SUPPORT)
#target_link_libraries (${EXECUTABLE} ${GTK_LIBRARIES})
#if (UNIX)
#elseif (WIN32)
# if (GTKGLAREA_SUPPORT)
#  target_link_libraries (${EXECUTABLE} $ENV{LIB_ROOT}/gtkglarea/${BUILD_PATH_SUFFIX}/${CMAKE_BUILD_TYPE}/gtkglarea.lib)
# endif (GTKGLAREA_SUPPORT)
#endif ()

target_link_libraries (${EXECUTABLE} PUBLIC ${ACE_LIBRARY})

target_link_libraries (${EXECUTABLE} PUBLIC Common)
target_link_libraries (${EXECUTABLE} PUBLIC CommonLog)
if (GTKGL_SUPPORT)
 target_link_libraries (${EXECUTABLE} PUBLIC CommonOpenGL)
endif (GTKGL_SUPPORT)
target_link_libraries (${EXECUTABLE} PUBLIC CommonTimer)
target_link_libraries (${EXECUTABLE} PUBLIC CommonUI)
target_link_libraries (${EXECUTABLE} PUBLIC CommonUI_GTK)

target_link_libraries (${EXECUTABLE} PUBLIC Common_Test_I)

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
# set_target_properties (${EXECUTABLE_2} PROPERTIES LINK_FLAGS /SAFESEH:NO)

 target_link_libraries (${EXECUTABLE_2} OpenGL32)
endif (UNIX)
target_link_libraries (${EXECUTABLE_2} ${GTK_LIBRARIES})
#if (UNIX)
#elseif (WIN32)
# if (GTKGLAREA_SUPPORT)
#  target_link_libraries (${EXECUTABLE_2} $ENV{LIB_ROOT}/gtkglarea/${BUILD_PATH_SUFFIX}/${CMAKE_BUILD_TYPE}/gtkglarea.lib)
# endif (GTKGLAREA_SUPPORT)
#endif ()

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
# set_target_properties (${EXECUTABLE_3} PROPERTIES LINK_FLAGS /SAFESEH:NO)

 target_link_libraries (${EXECUTABLE_3} OpenGL32)
endif (UNIX)
target_link_libraries (${EXECUTABLE_3} ${GTK_LIBRARIES})
#if (UNIX)
#elseif (WIN32)
# if (GTKGLAREA_SUPPORT)
#  target_link_libraries (${EXECUTABLE_3} $ENV{LIB_ROOT}/gtkglarea/${BUILD_PATH_SUFFIX}/${CMAKE_BUILD_TYPE}/gtkglarea.lib)
# endif (GTKGLAREA_SUPPORT)
#endif ()

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
 if (GTK4_USE)
  set (ui_definition_file ${EXECUTABLE}.gtk4)
  file (TO_CMAKE_PATH $ENV{LIB_ROOT}/gtk4/bin GTK_LIB_PATH)
 elseif (GTK3_USE)
  set (ui_definition_file ${EXECUTABLE}.gtk3)
  file (TO_CMAKE_PATH $ENV{LIB_ROOT}/gtk3/bin GTK_LIB_PATH)
 elseif (GTK2_USE)
  set (ui_definition_file ${EXECUTABLE}.gtk2)
  file (TO_CMAKE_PATH $ENV{LIB_ROOT}/gtk2/bin GTK_LIB_PATH)
 endif ()
elseif (WXWIDGETS_USE)
endif ()

set (ARGS "-t")
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

if (UNIX)
 create_target_launcher (${EXECUTABLE}
                         ARGS ${ARGS}
#                         FORWARD_ARGS
                         RUNTIME_LIBRARY_DIRS ${GTK_LIB_PATH}
                                              ${ACE_LIB_PATH}
                                              ${PROJECT_SOURCE_DIR}/${BUILD_PATH_SUFFIX}/src/${CMAKE_BUILD_TYPE}
                         WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/../test_i/ui/gtk
                         ENVIRONMENT ${ENVIRONMENT})

 create_target_launcher (${EXECUTABLE_2}
                         ARGS ${ARGS}
 #                         FORWARD_ARGS
                         RUNTIME_LIBRARY_DIRS ${GTK_LIB_PATH}
                                              ${LIBXML2_LIB_PATH}
                                              ${ASSIMP_LIB_PATH}
                                              ${FFMPEG_LIB_PATH}
                                              ${IMAGEMAGICK_LIB_PATH}
                                              ${LIBGLADE_LIB_PATH}
                                              ${ACE_LIB_PATH}
                                              ${PROJECT_SOURCE_DIR}/${BUILD_PATH_SUFFIX}/src/${CMAKE_BUILD_TYPE}
                         WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/../test_i/ui/gtk
                         ENVIRONMENT ${ENVIRONMENT})

 create_target_launcher (${EXECUTABLE_3}
                         ARGS ${ARGS}
 #                         FORWARD_ARGS
                         RUNTIME_LIBRARY_DIRS ${GTK_LIB_PATH}
                                              ${LIBXML2_LIB_PATH}
                                              ${ASSIMP_LIB_PATH}
                                              ${FFMPEG_LIB_PATH}
                                              ${IMAGEMAGICK_LIB_PATH}
                                              ${LIBGLADE_LIB_PATH}
                                              ${ACE_LIB_PATH}
                                              ${PROJECT_SOURCE_DIR}/${BUILD_PATH_SUFFIX}/src/${CMAKE_BUILD_TYPE}
                         WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/../test_i/ui/gtk
                         ENVIRONMENT ${ENVIRONMENT})
elseif (WIN32)
 file (TO_CMAKE_PATH "${GTK_LIB_DIRS}" GTK_LIB_PATH)
 file (TO_CMAKE_PATH "${ACE_LIB_DIR}" ACE_LIB_PATH)
 file (TO_CMAKE_PATH "${ImageMagick_LIB_DIR}" IMAGEMAGICK_LIB_PATH)
 file (TO_CMAKE_PATH "${ASSIMP_LIB_DIR}" ASSIMP_LIB_PATH)
 create_target_launcher (${EXECUTABLE}
                         ARGS ${ARGS}
#                         FORWARD_ARGS
                         RUNTIME_LIBRARY_DIRS ${GTK_LIB_PATH}
                                              ${ACE_LIB_PATH}
                                              ${IMAGEMAGICK_LIB_PATH}
                                              ${ASSIMP_LIB_PATH}
                                              ${PROJECT_BINARY_DIR}/src/${CMAKE_BUILD_TYPE}
                         WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/test_i/ui/gtk
                         ENVIRONMENT ${ENVIRONMENT})

 create_target_launcher (${EXECUTABLE_2}
                         ARGS ${ARGS}
 #                         FORWARD_ARGS
                         RUNTIME_LIBRARY_DIRS ${GTK_LIB_PATH}
                                              ${LIBXML2_LIB_PATH}
                                              ${ASSIMP_LIB_PATH}
                                              ${FFMPEG_LIB_PATH}
                                              ${IMAGEMAGICK_LIB_PATH}
                                              ${LIBGLADE_LIB_PATH}
                                              ${ACE_LIB_PATH}
                                              ${PROJECT_BINARY_DIR}/src/${CMAKE_BUILD_TYPE}
                         WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/test_i/ui/gtk
                         ENVIRONMENT ${ENVIRONMENT})

 create_target_launcher (${EXECUTABLE_3}
                         ARGS ${ARGS}
 #                         FORWARD_ARGS
                         RUNTIME_LIBRARY_DIRS ${GTK_LIB_PATH}
                                              ${LIBXML2_LIB_PATH}
                                              ${ASSIMP_LIB_PATH}
                                              ${FFMPEG_LIB_PATH}
                                              ${IMAGEMAGICK_LIB_PATH}
                                              ${LIBGLADE_LIB_PATH}
                                              ${ACE_LIB_PATH}
                                              ${PROJECT_BINARY_DIR}/src/${CMAKE_BUILD_TYPE}
                         WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/test_i/ui/gtk
                         ENVIRONMENT ${ENVIRONMENT})
endif ()

################ tests ###################
add_test (NAME ${EXECUTABLE}_test
          COMMAND ${PROJECT_BINARY_DIR}/${EXECUTABLE}
          WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/test_i/ui/gtk)
set_tests_properties (${EXECUTABLE}_test
                      PROPERTIES
                      ENVIRONMENT "PATH=D:\\projects\\ATCD\\ACE\\lib;D:\\projects\\gtk\\bin;D:\\projects\\libglade\\bin;D:\\projects\\libxml2-2.9.1\\.libs;G:\\software\\Development\\dll;${PROJECT_SOURCE_DIR}\\..\\libCommon\\cmake\\src\\Debug;${PROJECT_SOURCE_DIR}\\..\\libCommon\\cmake\\src\\ui\\Debug;${PROJECT_SOURCE_DIR}\\cmake\\src\\Debug;${PROJECT_SOURCE_DIR}\\cmake\\src\\modules\\dev\\Debug;${PROJECT_SOURCE_DIR}\\..\\libACENetwork\\cmake\\src\\Debug;%PATH%")
#PATH=%PATH%;$(ProjectDir)..\..\..\..\ATCD\ACE\lib;$(ProjectDir)..\..\..\..\gtk3\bin;$(ProjectDir)..\..\..\..\libav-11.7\libavcodec;$(ProjectDir)..\..\..\..\libav-11.7\libavresample;$(ProjectDir)..\..\..\..\libav-11.7\libavutil;$(ProjectDir)..\..\..\..\libCommon\cmake\src\Debug;$(ProjectDir)..\..\..\..\libCommon\cmake\src\err\Debug;$(ProjectDir)..\..\..\..\libCommon\cmake\src\ui\Debug;$(ProjectDir)..\..\src\Debug;$(ProjectDir)..\..\src\modules\dev\Debug;$(ProjectDir)..\..\src\modules\vis\Debug

add_test (NAME ${EXECUTABLE_2}_test
          COMMAND ${PROJECT_BINARY_DIR}/${EXECUTABLE_2}
          WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/test_i/ui/gtk)
set_tests_properties (${EXECUTABLE_2}_test
                      PROPERTIES
                      ENVIRONMENT "PATH=D:\\projects\\ATCD\\ACE\\lib;D:\\projects\\gtk\\bin;D:\\projects\\libglade\\bin;D:\\projects\\libxml2-2.9.1\\.libs;G:\\software\\Development\\dll;${PROJECT_SOURCE_DIR}\\..\\libCommon\\cmake\\src\\Debug;${PROJECT_SOURCE_DIR}\\..\\libCommon\\cmake\\src\\ui\\Debug;${PROJECT_SOURCE_DIR}\\cmake\\src\\Debug;${PROJECT_SOURCE_DIR}\\cmake\\src\\modules\\dev\\Debug;${PROJECT_SOURCE_DIR}\\..\\libACENetwork\\cmake\\src\\Debug;%PATH%")

add_test (NAME ${EXECUTABLE_3}_test
          COMMAND ${PROJECT_BINARY_DIR}/${EXECUTABLE_3}
          WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/test_i/ui/gtk)
set_tests_properties (${EXECUTABLE_3}_test
                      PROPERTIES
                      ENVIRONMENT "PATH=D:\\projects\\ATCD\\ACE\\lib;D:\\projects\\gtk\\bin;D:\\projects\\libglade\\bin;D:\\projects\\libxml2-2.9.1\\.libs;G:\\software\\Development\\dll;${PROJECT_SOURCE_DIR}\\..\\libCommon\\cmake\\src\\Debug;${PROJECT_SOURCE_DIR}\\..\\libCommon\\cmake\\src\\ui\\Debug;${PROJECT_SOURCE_DIR}\\cmake\\src\\Debug;${PROJECT_SOURCE_DIR}\\cmake\\src\\modules\\dev\\Debug;${PROJECT_SOURCE_DIR}\\..\\libACENetwork\\cmake\\src\\Debug;%PATH%")
