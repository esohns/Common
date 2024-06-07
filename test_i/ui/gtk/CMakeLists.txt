include (ffmpeg)
include (ImageMagick)
if (GTKGL_SUPPORT)
 set (CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/../../../src/gl/scripts")
 include (assimp)
 set (CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/../../../src/image/scripts")
 include (png)
endif (GTKGL_SUPPORT)

string (REPLACE ${CMAKE_SOURCE_DIR} "" BUILD_PATH_SUFFIX ${CMAKE_BINARY_DIR})
string (SUBSTRING ${BUILD_PATH_SUFFIX} 1 -1 BUILD_PATH_SUFFIX)

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
set (EXECUTABLE gtk)
set (${EXECUTABLE}_EXE_SRCS
#     stdafx.cpp
     stdafx.h
     test_i.cpp
     test_i_gtk_defines.h)

#cmake_policy (SET CMP0065 OLD) # do not add flags to export symbols from executables without the ENABLE_EXPORTS target property
add_executable (${EXECUTABLE}
                EXCLUDE_FROM_ALL
                ${${EXECUTABLE}_EXE_SRCS})
set_property (TARGET ${EXECUTABLE} PROPERTY FOLDER "test_i")

if (UNIX)
 add_link_options ("-rdynamic")
endif (UNIX)

if (GTKGL_SUPPORT)
 target_link_libraries (${EXECUTABLE} PUBLIC ${OPENGL_LIBRARIES})
endif (GTKGL_SUPPORT)
#target_link_libraries (${EXECUTABLE} PUBLIC ${ffmpeg_2_LIBRARIES})
target_link_libraries (${EXECUTABLE} PUBLIC ${GTK_LIBRARIES})

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
target_precompile_headers (${EXECUTABLE}
                           PRIVATE stdafx.h)

############## next target ###############
set (EXECUTABLE_2 gtk_2)
set (${EXECUTABLE_2}_EXE_SRCS
#     stdafx.cpp
     stdafx.h
     test_i_2.cpp
     test_i_gtk_callbacks.cpp
     test_i_gtk_callbacks.h
     test_i_gtk_defines.h)

#cmake_policy (SET CMP0065 OLD) # do not add flags to export symbols from executables without the ENABLE_EXPORTS target property
add_executable (${EXECUTABLE_2}
                EXCLUDE_FROM_ALL
                ${${EXECUTABLE_2}_EXE_SRCS})
set_property (TARGET ${EXECUTABLE_2} PROPERTY FOLDER "test_i")

if (UNIX)
 target_link_libraries (${EXECUTABLE_2} PUBLIC X11)
 target_link_libraries (${EXECUTABLE_2} PUBLIC GL)
elseif (WIN32)
 target_link_libraries (${EXECUTABLE_2} PUBLIC OpenGL32)
endif (UNIX)
target_link_libraries (${EXECUTABLE_2} PUBLIC ${GTK_LIBRARIES})

target_link_libraries (${EXECUTABLE_2} PUBLIC ${ACE_LIBRARY})

target_link_libraries (${EXECUTABLE_2} PUBLIC Common)
target_link_libraries (${EXECUTABLE_2} PUBLIC CommonOpenGL)
target_link_libraries (${EXECUTABLE_2} PUBLIC CommonLog)
target_link_libraries (${EXECUTABLE_2} PUBLIC CommonTimer)
target_link_libraries (${EXECUTABLE_2} PUBLIC CommonUI)
target_link_libraries (${EXECUTABLE_2} PUBLIC CommonUI_GTK)

# add PCH
target_precompile_headers (${EXECUTABLE_2}
                           PRIVATE stdafx.h)

############## next target ###############
set (EXECUTABLE_3 gtk_3)
set (${EXECUTABLE_3}_EXE_SRCS
#     stdafx.cpp
     stdafx.h
     test_i_3.cpp
     test_i_gtk_callbacks.cpp
     test_i_gtk_callbacks.h
     test_i_gtk_defines.h)

#cmake_policy (SET CMP0065 OLD) # do not add flags to export symbols from executables without the ENABLE_EXPORTS target property
add_executable (${EXECUTABLE_3}
                EXCLUDE_FROM_ALL
                ${${EXECUTABLE_3}_EXE_SRCS})
set_property (TARGET ${EXECUTABLE_3} PROPERTY FOLDER "test_i")

if (UNIX)
 target_link_libraries (${EXECUTABLE_3} PUBLIC X11)
 target_link_libraries (${EXECUTABLE_3} PUBLIC GL)
elseif (WIN32)
 target_link_libraries (${EXECUTABLE_3} PUBLIC OpenGL32)
endif (UNIX)
#target_link_libraries (${EXECUTABLE_3} PUBLIC ${ffmpeg_2_LIBRARIES})
target_link_libraries (${EXECUTABLE_3} PUBLIC ${GTK_LIBRARIES})

target_link_libraries (${EXECUTABLE_3} PUBLIC ${ACE_LIBRARY})

target_link_libraries (${EXECUTABLE_3} PUBLIC Common)
target_link_libraries (${EXECUTABLE_3} PUBLIC CommonLog)
target_link_libraries (${EXECUTABLE_3} PUBLIC CommonOpenGL)
target_link_libraries (${EXECUTABLE_3} PUBLIC CommonTimer)
target_link_libraries (${EXECUTABLE_3} PUBLIC CommonUI)
target_link_libraries (${EXECUTABLE_3} PUBLIC CommonUI_GTK)

# add PCH
target_precompile_headers (${EXECUTABLE_3}
                           PRIVATE stdafx.h)

set (ui_definition_file "")
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

############### installation #############
install (TARGETS ${EXECUTABLE} ${EXECUTABLE_2} ${EXECUTABLE_3}
         EXPORT ${PROJECT_NAME}Targets
         RUNTIME DESTINATION ${INSTALL_BIN_DIR} COMPONENT test_i)
if (WIN32)
 get_filename_component (ACE_LIB_PATH ${ACE_LIBRARY} DIRECTORY)
 set (ACE_DLL_FILE ACE${LIB_FILE_SUFFIX}.dll)
 set (ACE_DLL_FQPATH ${ACE_LIB_PATH}/${ACE_DLL_FILE})
 if (DEFINED ENV{LIB_ROOT})
  file (TO_NATIVE_PATH $ENV{LIB_ROOT} LIB_ROOT)
  string (REPLACE "\\" "\\\\" LIB_ROOT ${LIB_ROOT})
 endif (DEFINED ENV{LIB_ROOT})
 install (FILES ${ACE_DLL_FQPATH}
                ${LIB_ROOT}/gtk3/bin/libatk-1.0-0.dll
                ${LIB_ROOT}/gtk3/bin/libfreetype-6.dll
                ${LIB_ROOT}/gtk3/bin/libintl-8.dll
                ${LIB_ROOT}/gtk3/bin/libcairo-2.dll
                ${LIB_ROOT}/gtk3/bin/libfontconfig-1.dll
                ${LIB_ROOT}/gtk3/bin/libgio-2.0-0.dll
                ${LIB_ROOT}/gtk3/bin/libglib-2.0-0.dll
                ${LIB_ROOT}/gtk3/bin/libgmodule-2.0-0.dll
                ${LIB_ROOT}/gtk3/bin/libgobject-2.0-0.dll
                ${LIB_ROOT}/gtk3/bin/libgthread-2.0-0.dll
                ${LIB_ROOT}/gtk3/bin/libgdk_pixbuf-2.0-0.dll
                ${LIB_ROOT}/gtk3/bin/libgdk-3-0.dll
                ${LIB_ROOT}/gtk3/bin/libgtk-3-0.dll
                ${LIB_ROOT}/gtk3/bin/libpango-1.0-0.dll
                ${LIB_ROOT}/gtk3/bin/libpangocairo-1.0-0.dll
                ${LIB_ROOT}/gtk3/bin/libpangoft2-1.0-0.dll
                ${LIB_ROOT}/gtk3/bin/libpangowin32-1.0-0.dll
                ${LIB_ROOT}/gtk3/bin/libiconv-2.dll
                ${LIB_ROOT}/gtk3/bin/libpng15-15.dll
                ${LIB_ROOT}/gtk3/bin/liblzma-5.dll
                ${LIB_ROOT}/gtk3/bin/zlib1.dll
                ${LIB_ROOT}/gtk3/bin/libxml2-2.dll
                ${LIB_ROOT}/gtk3/bin/libffi-6.dll
                ${LIB_ROOT}/gtk3/bin/libcairo-gobject-2.dll
                ${LIB_ROOT}/gtk3/bin/libpixman-1-0.dll
                ${LIB_ROOT}/gtk3/bin/libwinpthread-1.dll
                ${LIB_ROOT}/assimp/cmake/bin/Debug/assimp-vc142-mtd.dll
                ${LIB_ROOT}/libpng/build/Debug/libpng16d.dll
                ${LIB_ROOT}/ffmpeg/libswresample/swresample-4.dll
                ${LIB_ROOT}/ffmpeg/libswscale/swscale-7.dll
                ${LIB_ROOT}/ffmpeg/libavcodec/avcodec-60.dll
                ${LIB_ROOT}/ffmpeg/libavutil/avutil-58.dll
                ${LIB_ROOT}/libepoxy/bin/epoxy-0.dll
                ${LIB_ROOT}/wxWidgets/lib/vc_x64_dll/wxmsw316d_core_vc_x64_custom.dll
                ${LIB_ROOT}/wxWidgets/lib/vc_x64_dll/wxmsw316d_html_vc_x64_custom.dll
                ${LIB_ROOT}/wxWidgets/lib/vc_x64_dll/wxmsw316d_xrc_vc_x64_custom.dll
                ${LIB_ROOT}/wxWidgets/lib/vc_x64_dll/wxbase316d_xml_vc_x64_custom.dll
                ${LIB_ROOT}/zlib/build/Debug/zlibd.dll
          DESTINATION ${INSTALL_BIN_DIR} COMPONENT test_i)
endif (WIN32)
if (GTK_USE)
 install (FILES etc/${ui_definition_file}
          DESTINATION "etc/${EXECUTABLE}" COMPONENT config)
endif (GTK_USE)
install (FILES share/opengl_logo.png
         DESTINATION "${INSTALL_SHARE_DIR}/${EXECUTABLE}" COMPONENT data)

################ launchers ###############

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
 file (TO_NATIVE_PATH "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/${EXECUTABLE}" COMMAND)
 create_target_launcher (${EXECUTABLE}
                         COMMAND ${COMMAND}
                         ARGS ${ARGS}
#                         FORWARD_ARGS
                         RUNTIME_LIBRARY_DIRS ${GTK_LIB_PATH}
                                              ${ACE_LIB_PATH}
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
                         WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/../test_i/ui/gtk
                         ENVIRONMENT ${ENVIRONMENT})
elseif (WIN32)
 file (TO_NATIVE_PATH "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/${EXECUTABLE}.exe" COMMAND)
 string (REPLACE "\\" "\\\\" COMMAND ${COMMAND})
 file (TO_CMAKE_PATH "${ACE_LIB_DIR}" ACE_LIB_PATH)
 file (TO_CMAKE_PATH "${ffmpeg_LIB_DIR}" FFMPEG_LIB_PATH)
 file (TO_CMAKE_PATH "${ImageMagick_LIB_DIR}" IMAGEMAGICK_LIB_PATH)
 file (TO_CMAKE_PATH "${PNG_LIB_DIR}" PNG_LIB_PATH)
 file (TO_CMAKE_PATH "${ASSIMP_LIB_DIR}" ASSIMP_LIB_PATH)
 file (TO_CMAKE_PATH "${GTK_LIB_DIRS}" GTK_LIB_PATH)
 create_target_launcher (${EXECUTABLE}
                         COMMAND ${COMMAND}
                         ARGS ${ARGS}
#                         FORWARD_ARGS
                         RUNTIME_LIBRARY_DIRS ${GTK_LIB_PATH}
                                              ${ACE_LIB_PATH}
                                              ${FFMPEG_LIB_PATH}
                                              ${IMAGEMAGICK_LIB_PATH}
                                              ${PNG_LIB_PATH}
                                              ${ASSIMP_LIB_PATH}
                         WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/test_i/ui/gtk
                         ENVIRONMENT ${ENVIRONMENT})

 file (TO_NATIVE_PATH "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/${EXECUTABLE_2}.exe" COMMAND_2)
 string (REPLACE "\\" "\\\\" COMMAND_2 ${COMMAND_2})
 create_target_launcher (${EXECUTABLE_2}
                         COMMAND ${COMMAND_2}
                         ARGS ${ARGS}
 #                         FORWARD_ARGS
                         RUNTIME_LIBRARY_DIRS ${GTK_LIB_PATH}
                                              ${LIBXML2_LIB_PATH}
                                              ${ASSIMP_LIB_PATH}
                                              ${PNG_LIB_PATH}
                                              ${FFMPEG_LIB_PATH}
                                              ${IMAGEMAGICK_LIB_PATH}
                                              ${LIBGLADE_LIB_PATH}
                                              ${ACE_LIB_PATH}
                         WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/test_i/ui/gtk
                         ENVIRONMENT ${ENVIRONMENT})

 file (TO_NATIVE_PATH "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/${EXECUTABLE_3}.exe" COMMAND_3)
 string (REPLACE "\\" "\\\\" COMMAND_3 ${COMMAND_3})
 create_target_launcher (${EXECUTABLE_3}
                         COMMAND ${COMMAND_3}
                         ARGS ${ARGS}
 #                         FORWARD_ARGS
                         RUNTIME_LIBRARY_DIRS ${GTK_LIB_PATH}
                                              ${LIBXML2_LIB_PATH}
                                              ${ASSIMP_LIB_PATH}
                                              ${PNG_LIB_PATH}
                                              ${FFMPEG_LIB_PATH}
                                              ${IMAGEMAGICK_LIB_PATH}
                                              ${LIBGLADE_LIB_PATH}
                                              ${ACE_LIB_PATH}
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
