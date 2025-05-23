set (LIBRARY ${PROJECT_NAME}UI_GTK)
add_library (${LIBRARY} STATIC "")

get_filename_component (BUILD_PATH ${CMAKE_CURRENT_BINARY_DIR}/../../.. REALPATH)
get_filename_component (BUILD_PATH_SUFFIX ${BUILD_PATH} NAME)

# *NOTE*: a bug in cmake (3.12.1) requires the additional comment
target_include_directories (${LIBRARY} PUBLIC
                            "$<BUILD_INTERFACE:${GTK_INCLUDE_DIRS}>")

target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${ACE_INCLUDE_DIR}>)

target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}>)

target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../..>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../image>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../log>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../task>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../timer>)

############## next target ################
set (source_files
     common_ui_gtk_builder_definition.cpp
     common_ui_gtk_builder_definition.h
     common_ui_gtk_builder_definition.inl
     common_ui_gtk_common.h
     common_ui_gtk_defines.h
     common_ui_gtk_eventhandler.cpp
     common_ui_gtk_eventhandler.h
     common_ui_gtk_eventhandler.inl
     common_ui_gtk_manager.cpp
     common_ui_gtk_manager.h
     common_ui_gtk_manager.inl
     common_ui_gtk_manager_common.h
     common_ui_gtk_tools.cpp
     common_ui_gtk_tools.h)
#     common_ui_gtk_types.h
if (LIBGLADE_SUPPORT AND GTK2_USE)
 set (source_files
      ${source_files}
      common_ui_gtk_glade_definition.cpp
      common_ui_gtk_glade_definition.h
      common_ui_gtk_glade_definition.inl)
endif (LIBGLADE_SUPPORT AND GTK2_USE) # LIBGLADE_SUPPORT
if (GTKGL_SUPPORT)
 set (source_files
      ${source_files}
      common_ui_gtk_gl_common.h)
endif (GTKGL_SUPPORT) # GTKGL_SUPPORT

# Include dir for installed targets
set (INCLUDE_DIR include/${PROJECT_NAME}-${upstream_version})

#set_target_properties (${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)

unset (absolute_source_files)
foreach (source_file ${source_files})
 get_filename_component (abs_source_file ${source_file} ABSOLUTE)
 list (APPEND absolute_source_files
       PRIVATE ${abs_source_file}
       )
 #       $<INSTALL_INTERFACE:${INCLUDE_DIR}/${source_file}>)
endforeach ()
target_sources (${LIBRARY}
#                PRIVATE stdafx.cpp
                PRIVATE stdafx.h
                PRIVATE ${absolute_source_files})

if (CMAKE_BUILD_TYPE STREQUAL Debug)
 add_definitions (-DG_ERRORCHECK_MUTEXES)
endif (CMAKE_BUILD_TYPE STREQUAL Debug)

add_dependencies (${LIBRARY} Common)

target_link_libraries (${LIBRARY} ${GTK_LIBRARIES})

if (UNIX)
 if (GTKGL_SUPPORT)
  target_link_libraries (${LIBRARY} X11)
  target_link_libraries (${LIBRARY} GL)
 endif (GTKGL_SUPPORT)
elseif (WIN32)
# if (CMAKE_BUILD_TYPE STREQUAL Debug)
#  target_link_libraries (${LIBRARY} Msvcrtd.lib)
# else ()
#  target_link_libraries (${LIBRARY} Msvcrt.lib)
# endif (CMAKE_BUILD_TYPE STREQUAL Debug)

 if (GTKGLAREA_SUPPORT)
  target_link_libraries (${LIBRARY} $ENV{LIB_ROOT}/libepoxy/lib/epoxy.lib)
#  target_link_libraries (${LIBRARY} $ENV{LIB_ROOT}/gtkglarea/${BUILD_PATH_SUFFIX}/${CMAKE_BUILD_TYPE}/gtkglarea.lib)
# else (GTKGLAREA_SUPPORT)
 # target_link_libraries (${EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../../gtkglext/gdk/.libs/libgdkglext-win32-1.0.dll.a)
 # target_link_libraries (${EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../../gtkglext/gtk/.libs/libgtkglext-win32-1.0.dll.a)
 endif (GTKGLAREA_SUPPORT)

# if (LIBGLADE_SUPPORT)
# libglade
#  target_link_libraries (${LIBRARY} ${LIBGLADE_LIBRARY})
# endif (LIBGLADE_SUPPORT)

 target_link_libraries (${LIBRARY} ${ACE_LIBRARY})

 target_link_libraries (${LIBRARY} Common)
 target_link_libraries (${LIBRARY} CommonError)
endif ()

# *WARNING*: linking libglade-2.dll(.a) requires "/SAFESEH:NO""
if (WIN32)
 if (LIBGLADE_SUPPORT)
  set_target_properties (${LIBRARY} PROPERTIES LINK_FLAGS /SAFESEH:NO)
 endif (LIBGLADE_SUPPORT)
endif ()
#set_target_properties (${LIBRARY} PROPERTIES DEFINE_SYMBOL COMMON_UI_BUILD_DLL)
#set_target_properties (${LIBRARY} PROPERTIES VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_MICRO} SOVERSION 1)

# add PCH
target_precompile_headers (${LIBRARY}
                           PRIVATE stdafx.h)

set_property (TARGET ${LIBRARY} PROPERTY FOLDER "modules/ui")

# add all targets to the build-tree export set
export (TARGETS ${LIBRARY}
        APPEND FILE "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake")

############### install files ################
set (${LIBRARY}_LIB_HDRS
     common_ui_gtk_builder_definition.h
     common_ui_gtk_builder_definition.inl
     common_ui_gtk_common.h
     common_ui_gtk_defines.h
     common_ui_gtk_eventhandler.h
     common_ui_gtk_eventhandler.inl
     common_ui_gtk_manager.h
     common_ui_gtk_manager.inl
     common_ui_gtk_manager_common.h
     common_ui_gtk_tools.h)
#     common_ui_gtk_types.h
if (LIBGLADE_SUPPORT)
 set (${LIBRARY}_LIB_GLADE_HDRS ${CommonUI_GTK_LIB_GLADE_HDRS}
      common_ui_glade_definition.h
      common_ui_gtk_glade_definition.inl)
endif (LIBGLADE_SUPPORT)
if (GTKGL_SUPPORT)
 set (${LIBRARY}_LIB_GLADE_HDRS ${CommonUI_GTK_LIB_GLADE_HDRS}
      common_ui_gtk_gl_common.h)
endif (GTKGL_SUPPORT)
install (FILES
         ${CommonUI_GTK_LIB_HDRS}
         DESTINATION include)

install (TARGETS ${LIBRARY}
         EXPORT ${PROJECT_NAME}Targets
         ARCHIVE DESTINATION "${INSTALL_LIB_DIR}"
         COMPONENT lib)
