include (${CMAKE_CURRENT_SOURCE_DIR}/../scripts/Threads.cmake)

include (${CMAKE_CURRENT_SOURCE_DIR}/../scripts/ACE.cmake)

option (BUILD_TEST_U "build test_u" OFF)

# user interface
option (GUI_SUPPORT "enable graphical user interface support" ON)
if (GUI_SUPPORT)
 add_definitions (-DGUI_SUPPORT)
 set (CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/../src/ui/scripts")
 include (PreUIFramework)
 include (SetUIFramework)
 if (GTK_USE)
  include (SetGTKVersion)
 endif (GTK_USE)
endif (GUI_SUPPORT)

if (UNIX)
 add_link_options ("LINKER:--as-needed")
endif (UNIX)

add_library (${PROJECT_NAME}_Test_U STATIC "")

if (GUI_SUPPORT)
 if (CURSES_SUPPORT)
  target_include_directories (${PROJECT_NAME}_Test_U PUBLIC
                              "$<BUILD_INTERFACE:${CURSES_INCLUDE_DIRS}>")
 endif (CURSES_SUPPORT)
 if (GTK_SUPPORT)
  target_include_directories (${PROJECT_NAME}_Test_U PUBLIC
                              "$<BUILD_INTERFACE:${GTK_INCLUDE_DIRS}>")

  if (LIBGLADE_SUPPORT)
   target_include_directories (${PROJECT_NAME}_Test_U PUBLIC
                               $<BUILD_INTERFACE:${LIBGLADE_INCLUDES}>)
  endif (LIBGLADE_SUPPORT)

  if (GTKGLAREA_SUPPORT)
# *NOTE*: check out the 'master' branch for gtk3-based applications
# *NOTE*: check out the 'gtkglarea-2' branch for gtk2-based applications
# *TODO*: verify automatically
   target_include_directories (${PROJECT_NAME}_Test_U PUBLIC
                               $<BUILD_INTERFACE:${GTKGLAREA_INCLUDES}>)
  endif (GTKGLAREA_SUPPORT)
 endif (GTK_SUPPORT)
 if (QT_SUPPORT)
  target_include_directories (${PROJECT_NAME}_Test_U PUBLIC
                              $<BUILD_INTERFACE:${QT_INCLUDE_DIRS}>)
 endif (QT_SUPPORT)
 if (WXWIDGETS_SUPPORT)
  target_include_directories (${PROJECT_NAME}_Test_U PUBLIC
                              $<BUILD_INTERFACE:${wxWidgets_INCLUDE_DIRS}>)
 endif (WXWIDGETS_SUPPORT)
endif (GUI_SUPPORT)

target_include_directories (${PROJECT_NAME}_Test_U PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src>)
target_include_directories (${PROJECT_NAME}_Test_U PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src/log>)
target_include_directories (${PROJECT_NAME}_Test_U PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src/signal>)
if (GUI_SUPPORT)
 target_include_directories (${PROJECT_NAME}_Test_U PUBLIC
                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src/ui>)
 if (CURSES_SUPPORT)
  target_include_directories (${PROJECT_NAME}_Test_U PUBLIC
                              $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src/ui/curses>)
 endif (CURSES_SUPPORT)
 if (GTK_SUPPORT)
  target_include_directories (${PROJECT_NAME}_Test_U PUBLIC
                              $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src/ui/gtk>)
 endif (GTK_SUPPORT)
 if (QT_SUPPORT)
  target_include_directories (${PROJECT_NAME}_Test_U PUBLIC
                              $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src/ui/qt>)
 endif (QT_SUPPORT)
 if (WXWIDGETS_SUPPORT)
  target_include_directories (${PROJECT_NAME}_Test_U PUBLIC
                              $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src/ui/wxwidgets>)
 endif (WXWIDGETS_SUPPORT)
endif (GUI_SUPPORT)
if (WIN32)
 target_include_directories (${PROJECT_NAME}_Test_U PUBLIC
                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>)
endif ()
target_include_directories (${PROJECT_NAME}_Test_U PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/..>)

if (GUI_SUPPORT)
 if (GTK_SUPPORT)
  target_link_libraries (${PROJECT_NAME}_Test_U PUBLIC ${GTK_LIBRARIES})
  if (GTKGL_SUPPORT)
   if (GTKGLAREA_SUPPORT)
    target_link_libraries (${PROJECT_NAME}_Test_U PUBLIC ${GTKGLAREA_LIBRARY})
   endif (GTKGLAREA_SUPPORT)
  endif (GTKGL_SUPPORT)
 endif (GTK_SUPPORT)
 if (QT_SUPPORT)
  target_link_libraries (${PROJECT_NAME}_Test_U PUBLIC ${QT_LIBRARIES})
 endif (QT_SUPPORT)
 if (WXWIDGETS_SUPPORT)
  target_link_libraries (${PROJECT_NAME}_Test_U PUBLIC ${wxWidgets_LIBRARIES})
 endif (WXWIDGETS_SUPPORT)
endif (GUI_SUPPORT)

set (source_files
     common_test_u_common.h
#     test_u_defines.h
     common_test_u_tools.cpp
     common_test_u_tools.h
     common_test_u_tools.inl)
#if (GTK_SUPPORT)
# set (source_files
#      ${source_files}
#      test_u_gtk_common.h
#      test_u_gtk_eventhandler.cpp
#      test_u_gtk_eventhandler.h
#      test_u_gtk_eventhandler.inl)
#endif (GTK_SUPPORT)
#if (WXWIDGETS_SUPPORT)
# set (source_files
#      ${source_files}
#      test_u_wxwidgets_common.h)
#endif (WXWIDGETS_SUPPORT)

foreach (source_file ${source_files})
 get_filename_component (abs_source_file ${source_file} ABSOLUTE)
 list (APPEND absolute_source_files
       PRIVATE ${abs_source_file}
       )
#       $<INSTALL_INTERFACE:${INCLUDE_DIR}/${source_file}>)
endforeach ()

##########################################

target_sources (${PROJECT_NAME}_Test_U
                PRIVATE stdafx.cpp
                PRIVATE stdafx.h
                ${absolute_source_files})
target_precompile_headers (${PROJECT_NAME}_Test_U
                           PUBLIC stdafx.h)

#set_target_properties (${PROJECT_NAME}_Test_U PROPERTIES VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_MICRO} SOVERSION 1)

##########################################

install (FILES
         common_test_u_common.h
         common_test_u_tools.h
         common_test_u_tools.inl
         DESTINATION include)
install (TARGETS ${PROJECT_NAME}_Test_U
         EXPORT ${PROJECT_NAME}Targets
         ARCHIVE DESTINATION ${INSTALL_LIB_DIR} COMPONENT lib
         LIBRARY DESTINATION ${INSTALL_LIB_DIR} COMPONENT lib
         RUNTIME DESTINATION ${INSTALL_BIN_DIR} COMPONENT lib)

##########################################

# add all targets to the build-tree export set
export (TARGETS ${PROJECT_NAME}_Test_U
        APPEND FILE "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake")

##########################################

if (UNIX)
 add_subdirectory (capabilities)
 add_subdirectory (debug)
 add_subdirectory (features)
endif (UNIX)
add_subdirectory (base64)
add_subdirectory (count)
add_subdirectory (file)
add_subdirectory (locale)
add_subdirectory (regex)

# phonies
add_custom_target (test_u ALL
                   COMMENT "building test_u...")
if (UNIX)
 add_dependencies (test_u capabilities capabilities_2)
 add_dependencies (test_u debug_wrapper)
 add_dependencies (test_u features)
endif (UNIX)
add_dependencies (test_u base64)
add_dependencies (test_u count)
add_dependencies (test_u file)
add_dependencies (test_u regex)

if (NOT BUILD_TEST_U)
 set_target_properties (test_u PROPERTIES EXCLUDE_FROM_ALL 1)
 cmake_policy (SET CMP0012 NEW)
 if (WIN32)
  set_target_properties (test_u PROPERTIES EXCLUDE_FROM_DEFAULT_BUILD TRUE)
 endif (WIN32)
endif (NOT BUILD_TEST_U)

# ----------------------------------------

# enable (unit/integration) tests
#include (CTest)
enable_testing ()
