#message (STATUS "wxWidgets_DEFINITIONS: ${wxWidgets_DEFINITIONS}")
#message (STATUS "wxWidgets_DEFINITIONS_DEBUG: ${wxWidgets_DEFINITIONS_DEBUG}")
#message (STATUS "wxWidgets_INCLUDE_DIRS: ${wxWidgets_INCLUDE_DIRS}")
#message (STATUS "wxWidgets_CXX_FLAGS: ${wxWidgets_CXX_FLAGS}")
#message (STATUS "wxWidgets_USE_FILE: ${wxWidgets_USE_FILE}")
# *NOTE*: re-enable ANSI mode
# add_definitions (-DwxUSE_UNICODE=0)

add_library (${PROJECT_NAME}UI_wxWidgets STATIC "")

target_include_directories (${PROJECT_NAME}UI_wxWidgets PUBLIC
                            "$<BUILD_INTERFACE:${wxWidgets_INCLUDE_DIRS}>")

target_include_directories (${PROJECT_NAME}UI_wxWidgets PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/../../..>)
target_include_directories (${PROJECT_NAME}UI_wxWidgets PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>)
target_include_directories (${PROJECT_NAME}UI_wxWidgets PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../..>)
target_include_directories (${PROJECT_NAME}UI_wxWidgets PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../image>)
target_include_directories (${PROJECT_NAME}UI_wxWidgets PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../log>)

############## next target ################
set (source_files
     common_ui_wxwidgets_application.cpp
     common_ui_wxwidgets_application.h
     common_ui_wxwidgets_application.inl
     common_ui_wxwidgets_common.h
     common_ui_wxwidgets_defines.h
     common_ui_wxwidgets_iapplication.h
     common_ui_wxwidgets_itoplevel.h
     common_ui_wxwidgets_logger.cpp
     common_ui_wxwidgets_logger.h
     common_ui_wxwidgets_manager.cpp
     common_ui_wxwidgets_manager.h
     common_ui_wxwidgets_manager.inl
     common_ui_wxwidgets_tools.cpp
     common_ui_wxwidgets_tools.h
     common_ui_wxwidgets_xrc_definition.cpp
     common_ui_wxwidgets_xrc_definition.h
     common_ui_wxwidgets_xrc_definition.inl)

# Include dir for installed targets
set (INCLUDE_DIR include/${PROJECT_NAME}-${upstream_version})

#set_target_properties (${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)
if (WIN32)
# set_target_properties (${PROJECT_NAME}UI_wxWidgets PROPERTIES DEFINE_SYMBOL _CRT_SECURE_NO_WARNINGS)
endif (WIN32)

unset (absolute_source_files)
foreach (source_file ${source_files})
 get_filename_component (abs_source_file ${source_file} ABSOLUTE)
 list (APPEND absolute_source_files
       PRIVATE ${abs_source_file}
       )
 #       $<INSTALL_INTERFACE:${INCLUDE_DIR}/${source_file}>)
endforeach ()
target_sources (${PROJECT_NAME}UI_wxWidgets
                PRIVATE stdafx.cpp
                PRIVATE stdafx.h
                ${absolute_source_files})
target_precompile_headers (${PROJECT_NAME}UI_wxWidgets
                           PUBLIC stdafx.h)

#get_target_property (debug ${PROJECT_NAME}UI_GTK INTERFACE_INCLUDE_DIRECTORIES)
#message (STATUS "INTERFACE_INCLUDE_DIRECTORIES: \"${debug}\"")

if (UNIX)
elseif (WIN32)
#*TODO*: use pkg-config instead
 add_dependencies (${PROJECT_NAME}UI_wxWidgets Common)

 if (CMAKE_BUILD_TYPE STREQUAL Debug)
  target_link_libraries (${PROJECT_NAME}UI_wxWidgets Msvcrtd.lib)
 else ()
  target_link_libraries (${PROJECT_NAME}UI_wxWidgets Msvcrt.lib)
 endif ()

 target_link_libraries (${PROJECT_NAME}UI_wxWidgets ${wxWidgets_LIBRARIES})

# target_link_libraries (${PROJECT_NAME}UI_wxWidgets ${ACE_LIBRARY})

 target_link_libraries (${PROJECT_NAME}UI_wxWidgets Common)
endif ()

# *WARNING*: linking libglade-2.dll(.a) requires "/SAFESEH:NO""
if (WIN32)
# set_target_properties (${PROJECT_NAME}UI_wxWidgets PROPERTIES LINK_FLAGS /SAFESEH:NO)
endif ()
#set_target_properties (${PROJECT_NAME}UI_wxWidgets PROPERTIES DEFINE_SYMBOL COMMON_UI_BUILD_DLL)
#set_target_properties (${PROJECT_NAME}UI_wxWidgets PROPERTIES VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_MICRO} SOVERSION 1)

# add PCH
#set_target_properties (${PROJECT_NAME}UI_wxWidgets PROPERTIES COTIRE_CXX_PREFIX_HEADER_INIT "stdafx.h")
#cotire (${PROJECT_NAME}UI_wxWidgets)

#message (STATUS "INTERFACE_INCLUDE_DIRECTORIES=${INTERFACE_INCLUDE_DIRECTORIES}")

# add all targets to the build-tree export set
export (TARGETS ${PROJECT_NAME}UI_wxWidgets
        APPEND FILE "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake")

############### install files ################
set (${PROJECT_NAME}UI_wxWidgets_LIB_HDRS
     common_ui_wxwidgets_application.h
     common_ui_wxwidgets_application.inl
     common_ui_wxwidgets_common.h
     common_ui_wxwidgets_defines.h
     common_ui_wxwidgets_iapplication.h
     common_ui_wxwidgets_itoplevel.h
     common_ui_wxwidgets_logger.h
     common_ui_wxwidgets_manager.h
     common_ui_wxwidgets_manager.inl
     common_ui_wxwidgets_tools.h
     common_ui_wxwidgets_xrc_definition.h
     common_ui_wxwidgets_xrc_definition.inl)
install (FILES
         ${CommonUI_wxWidgets_LIB_HDRS}
         DESTINATION include)

install (TARGETS ${PROJECT_NAME}UI_wxWidgets
         EXPORT ${PROJECT_NAME}Targets
         ARCHIVE DESTINATION "${INSTALL_LIB_DIR}"
         COMPONENT lib)
