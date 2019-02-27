option (BUILD_TEST_U "build test_u" OFF)

set (ACE_LIB_FILE libACE.so)
if (UNIX)
 find_library (ACE_LIBRARY ${ACE_LIB_FILE}
               PATHS ${CMAKE_CURRENT_BINARY_DIR}/../../../ATCD/ACE /usr/local
    #           HINTS ENV ACE_ROOT
               PATH_SUFFIXES lib
               DOC "searching for ${ACE_LIB_FILE}"
               NO_DEFAULT_PATH)
# else ()
#  find_library (ACE_LIBRARY ${ACE_LIB_FILE}
#                DOC "searching for ${ACE_LIB_FILE} (system paths)")
# endif ()
elseif (WIN32)
 if (CMAKE_BUILD_TYPE STREQUAL Debug)
  set (LIB_FILE_SUFFIX d)
 endif ()
 unset (ACE_LIB_FILE)
 set (ACE_LIB_FILE ACE${LIB_FILE_SUFFIX}.lib)
 find_library (ACE_LIBRARY ${ACE_LIB_FILE}
               PATHS ENV ACE_ROOT
               PATHS ${CMAKE_CURRENT_BINARY_DIR}/../../../ATCD/ACE
               PATH_SUFFIXES lib lib\\${CMAKE_BUILD_TYPE}\\Win32
               DOC "searching for ${ACE_LIB_FILE}")
endif ()
#if (NOT EXISTS ACE_LIBRARY)
if (NOT ACE_LIBRARY)
 message (FATAL_ERROR "Could not find ${ACE_LIB_FILE} (was: \"${ACE_LIBRARY}\"), aborting")
else ()
 #message (STATUS "Found ACE library \"${ACE_LIBRARY}\"")
endif ()
add_definitions (-DACE_HAS_DLL)
#pkg_check_modules (PKG_ACE REQUIRED ACE)

# user interface
option (GUI_SUPPORT "enable graphical user interface support" ON)
if (GUI_SUPPORT)
 add_definitions (-DGUI_SUPPORT)
 include (${CMAKE_CURRENT_SOURCE_DIR}/../../${PROJECT_NAME}/src/ui/scripts/PreUIFramework.cmake)
 include (${CMAKE_CURRENT_SOURCE_DIR}/../../${PROJECT_NAME}/src/ui/scripts/SetUIFramework.cmake)

 if (GTK_USE)
  include (${CMAKE_CURRENT_SOURCE_DIR}/../../${PROJECT_NAME}/src/ui/scripts/SetGTKVersion.cmake)
  CMAKE_DEPENDENT_OPTION (GTKGL_SUPPORT "enable GTK OpenGL support" ON
                          "GUI_SUPPORT" OFF)
# *IMPORTANT NOTE*: to use gtkglarea on gtk2, check out the 'gtkglarea-2' branch
#                   of the project
  CMAKE_DEPENDENT_OPTION (GTKGLAREA_SUPPORT "enable GtkGLArea support" OFF
                          "GUI_SUPPORT; GTKGL_SUPPORT" OFF)
 elseif (WXWIDGETS_USE)
  message (STATUS "wxWidgets_DEFINITIONS: ${wxWidgets_DEFINITIONS}")
  message (STATUS "wxWidgets_DEFINITIONS_DEBUG: ${wxWidgets_DEFINITIONS_DEBUG}")
  message (STATUS "wxWidgets_CXX_FLAGS: ${wxWidgets_CXX_FLAGS}")
  message (STATUS "wxWidgets_USE_FILE: ${wxWidgets_USE_FILE}")
  # *NOTE*: re-enable ANSI mode
#  add_definitions (-DwxUSE_UNICODE=0)
  if (WIN32)
#   add_definitions(-DUNICODE -D_UNICODE)
   add_definitions (-D__WXMSW__)
   add_definitions (-DWXUSINGDLL)
   if (MSVC)
    add_definitions (-DwxMSVC_VERSION_AUTO)
   endif (MSVC)
  endif (WIN32)
  add_definitions (-DwxUSE_GUI=1)
  if (CMAKE_BUILD_TYPE STREQUAL Debug)
   foreach (DEFINITION ${wxWidgets_DEFINITIONS_DEBUG})
    add_definitions (-D${DEFINITION})
   endforeach ()
   add_definitions (-D__WXDEBUG__)
  else ()
   foreach (DEFINITION ${wxWidgets_DEFINITIONS})
    add_definitions (-D${DEFINITION})
  endif (CMAKE_BUILD_TYPE STREQUAL Debug)
 endif ()
endif (GUI_SUPPORT)

add_library (${PROJECT_NAME}_Test_U STATIC "")

if (UNIX)
 if (GUI_SUPPORT)
  if (GTK_USE)
   target_include_directories (${PROJECT_NAME}_Test_U PUBLIC
                               $<BUILD_INTERFACE:${PKG_GTK_INCLUDE_DIRS}>)
  elseif (WXWIDGETS_USE)
   target_include_directories (${PROJECT_NAME}_Test_U PUBLIC
                               $<BUILD_INTERFACE:${PKG_WXWIDGETS_INCLUDE_DIRS}>)
  endif ()
 endif (GUI_SUPPORT)

 target_include_directories (${PROJECT_NAME}_Test_U PUBLIC
                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../modules/ACE>)

 target_include_directories (${PROJECT_NAME}_Test_U PUBLIC
                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src>)
 if (GUI_SUPPORT)
  target_include_directories (${PROJECT_NAME}_Test_U PUBLIC
                              $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src/ui>)
  if (GTK_USE)
   target_include_directories (${PROJECT_NAME}_Test_U PUBLIC
                               $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src/ui/gtk>)
  elseif (WXWIDGETS_USE)
   target_include_directories (${PROJECT_NAME}_Test_U PUBLIC
                               $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src/ui/wxwidgets>)
  endif ()
 endif (GUI_SUPPORT)
elseif (WIN32)
 if (GUI_SUPPORT)
  if (GTK_USE)
   message (STATUS "GTK_INCLUDE_DIRS: \"${GTK_INCLUDE_DIRS}\"")
   target_include_directories (${PROJECT_NAME}_Test_U PUBLIC
                               "$<BUILD_INTERFACE:${GTK_INCLUDE_DIRS}>")

# libglade
# include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../libglade-2.6.4/include/libglade-2.0)
  elseif (WXWIDGETS_USE)
#   target_include_directories (${PROJECT_NAME}_Test_U PUBLIC
#                               $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../wxWidgets_win32/wxWidgets-3.0.0/include>)
   target_include_directories (${PROJECT_NAME}_Test_U PUBLIC
                               "$<BUILD_INTERFACE:${wxWidgets_INCLUDE_DIRS}>")
  endif ()
 endif (GUI_SUPPORT)

 if (DEFINED ENV{ACE_ROOT})
  file (TO_CMAKE_PATH $ENV{ACE_ROOT} ACE_ROOT_CMAKE)
  target_include_directories (${PROJECT_NAME}_Test_U PUBLIC
                              $<BUILD_INTERFACE:${ACE_ROOT_CMAKE}>)
 else ()
  target_include_directories (${PROJECT_NAME}_Test_U PUBLIC
                              $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../ATCD/ACE>)
 endif (DEFINED ENV{ACE_ROOT})

 target_include_directories (${PROJECT_NAME}_Test_U PUBLIC
                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src>)
 target_include_directories (${PROJECT_NAME}_Test_U PUBLIC
                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src/log>)
 if (GUI_SUPPORT)
  target_include_directories (${PROJECT_NAME}_Test_U PUBLIC
                              $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src/ui>)
  if (GTK_USE)
   target_include_directories (${PROJECT_NAME}_Test_U PUBLIC
                               $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src/ui/gtk>)
  elseif (WXWIDGETS_USE)
   target_include_directories (${PROJECT_NAME}_Test_U PUBLIC
                               $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src/ui/wxwidgets>)
  endif ()
 endif (GUI_SUPPORT)
endif ()
target_include_directories (${PROJECT_NAME}_Test_U PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/..>)

if (WIN32)
 if (GUI_SUPPORT)
  if (GTK_USE)
   target_link_libraries (${PROJECT_NAME}_Test_U ${GTK_LIBRARIES})

   if (GTKGL_SUPPORT)
    target_link_libraries (${PROJECT_NAME}_Test_U OpenGL32.lib)
    target_link_libraries (${PROJECT_NAME}_Test_U ${CMAKE_CURRENT_SOURCE_DIR}/../../libepoxy/cmake/${CMAKE_BUILD_TYPE}/epoxy1_static.lib)
    target_link_libraries (${PROJECT_NAME}_Test_U "C:/Documents and Settings/Erik/My Documents/Visual Studio 2010/Projects/gtkglarea/${CMAKE_BUILD_TYPE}/gtkglarea.lib")
   endif (GTKGL_SUPPORT)
  elseif (WXWIDGETS_USE)
   target_link_libraries (${PROJECT_NAME}_Test_U ${wxWidgets_LIBRARIES})
  endif ()
 endif (GUI_SUPPORT)
endif (WIN32)

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

# add PCH
set_target_properties (${PROJECT_NAME}_Test_U PROPERTIES
                       COTIRE_CXX_PREFIX_HEADER_INIT "stdafx.h")
cotire (${PROJECT_NAME}_Test_U)
#add_msvc_precompiled_header (stdafx.h, stdafx.cpp, ${absolute_source_files})

##########################################

target_sources (${PROJECT_NAME}_Test_U
#                PRIVATE stdafx.cpp
                PRIVATE stdafx.h
                ${absolute_source_files})

#set_target_properties (${PROJECT_NAME}_Test_U PROPERTIES
#                       VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_MICRO}
#                       SOVERSION 1)

#get_target_property (debug ${PROJECT_NAME}_Test_U INTERFACE_INCLUDE_DIRECTORIES)
#message (STATUS "INTERFACE_INCLUDE_DIRECTORIES: \"${debug}\"")
#message (STATUS "INTERFACE_INCLUDE_DIRECTORIES=${INTERFACE_INCLUDE_DIRECTORIES}")

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

unset (absolute_source_files)
foreach (source_file ${source_files})
 get_filename_component (abs_source_file ${source_file} ABSOLUTE)
 list (APPEND absolute_source_files
       PRIVATE ${abs_source_file}
       )
 #       $<INSTALL_INTERFACE:${INCLUDE_DIR}/${source_file}>)
endforeach ()
target_sources (${PROJECT_NAME}_Test_U
                PRIVATE stdafx.cpp
                PRIVATE stdafx.h
                ${absolute_source_files})

# add all targets to the build-tree export set
export (TARGETS ${PROJECT_NAME}_Test_U
        APPEND FILE "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake")

##########################################

if (UNIX)
 add_subdirectory (capabilities)
 add_subdirectory (debug)
 add_subdirectory (features)
endif (UNIX)
add_subdirectory (file)
add_subdirectory (regex)
add_subdirectory (ui)

# phonies
add_custom_target (test_u ALL
                   COMMENT "building test_u...")
if (UNIX)
 add_dependencies (test_u capabilities)
 add_dependencies (test_u capabilities_2)
 add_dependencies (test_u debug_wrapper)
 add_dependencies (test_u features)
endif (UNIX)
add_dependencies (test_u file)
add_dependencies (test_u regex)
if (WXWIDGETS_USE)
 add_dependencies (test_u wxwidgets)
 add_dependencies (test_u wxwidgets_2)
 add_dependencies (test_u wxwidgets_3)
endif (WXWIDGETS_USE)

if (NOT BUILD_TEST_U)
 set_target_properties (test_u PROPERTIES EXCLUDE_FROM_ALL 1)
 cmake_policy (SET CMP0012 NEW)
 if (WIN32)
  set_target_properties (test_u PROPERTIES EXCLUDE_FROM_DEFAULT_BUILD TRUE)
 endif (WIN32)
endif ()

# ----------------------------------------

#include (CreateLaunchers)

# enable (unit/integration) tests
#include (CTest)
enable_testing ()
