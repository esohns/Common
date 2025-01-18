set (CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/scripts")
include (PreUIFramework)
include (SetUIFramework)
if (GTK_SUPPORT)
 include (SetGTKVersion)
endif (GTK_SUPPORT)

set (LIBRARY ${PROJECT_NAME}UI)
add_library (${LIBRARY} STATIC "")

if (GTK_SUPPORT)
# message (STATUS "GTK_INCLUDE_DIRS: ${GTK_INCLUDE_DIRS}")
 target_include_directories (${LIBRARY} PUBLIC
                             "$<BUILD_INTERFACE:${GTK_INCLUDE_DIRS}>")
endif (GTK_SUPPORT)

target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${ACE_INCLUDE_DIR}>)

target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}>)
if (WIN32)
 target_include_directories (${LIBRARY} PUBLIC
                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../include>)
 target_include_directories (${LIBRARY} PUBLIC
                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../error>)
endif (WIN32)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../log>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../xml>)

############## next target ################
set (source_files
     common_ui_common.h
     common_ui_defines.h
     common_ui_idefinition.h
     common_ui_ifullscreen.h
     common_ui_tools.cpp
     common_ui_tools.h
     common_ui_types.h
     common_ui_windowtype_converter.cpp
     common_ui_windowtype_converter.h
     common_ui_windowtype_converter.inl)
if (UNIX)
 set (source_files
      ${source_files}
      common_ui_monitor_setup_xml_handler.cpp
      common_ui_monitor_setup_xml_handler.h
      common_ui_x11_tools.cpp
      common_ui_x11_tools.h)
endif (UNIX)

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

add_dependencies (${LIBRARY} Common)
if (UNIX)
 target_link_libraries (${LIBRARY} PUBLIC X11)
 target_link_libraries (${LIBRARY} PUBLIC Xrandr)
 target_link_libraries (${LIBRARY} PUBLIC ${ACEXML_LIBRARY})
elseif (WIN32)
 target_link_libraries (${LIBRARY} PUBLIC Dxva2.lib)
endif ()

#set_target_properties (${LIBRARY} PROPERTIES DEFINE_SYMBOL COMMON_UI_BUILD_DLL)
#set_target_properties (${LIBRARY} PROPERTIES VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_MICRO} SOVERSION 1)

# add PCH
target_precompile_headers (${LIBRARY}
                           PRIVATE stdafx.h)

set_property (TARGET ${LIBRARY} PROPERTY FOLDER "modules")

# add all targets to the build-tree export set
export (TARGETS ${LIBRARY}
        APPEND FILE "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake")

############### install files ################
set (${LIBRARY}_LIB_HDRS
     common_ui_common.h
     common_ui_defines.h
     common_ui_idefinition.h
     common_ui_ifullscreen.h
     common_ui_tools.h
     common_ui_types.h
     common_ui_windowtype_converter.h
     common_ui_windowtype_converter.inl)
if (UNIX)
 set (${LIBRARY}_LIB_HDRS
      ${${LIBRARY}_LIB_HDRS}
      common_ui_monitor_setup_xml_handler.h
      common_ui_x11_tools.h)
endif (UNIX)
install (FILES
         ${CommonUI_LIB_HDRS}
         DESTINATION include)
install (TARGETS ${LIBRARY}
         EXPORT ${PROJECT_NAME}Targets
         ARCHIVE DESTINATION "${INSTALL_LIB_DIR}"
         COMPONENT lib)

if (CURSES_SUPPORT)
 add_subdirectory (curses)
endif (CURSES_SUPPORT)
if (GTK_SUPPORT)
 add_subdirectory (gtk)
endif (GTK_SUPPORT)
if (QT_SUPPORT)
 add_subdirectory (qt)
endif (QT_SUPPORT)
if (WXWIDGETS_SUPPORT)
 add_subdirectory (wxwidgets)
endif (WXWIDGETS_SUPPORT)
