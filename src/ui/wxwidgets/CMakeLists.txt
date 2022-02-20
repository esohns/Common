set (LIBRARY ${PROJECT_NAME}UI_wxWidgets)
add_library (${LIBRARY} STATIC "")

target_include_directories (${LIBRARY} PUBLIC
                            "$<BUILD_INTERFACE:${wxWidgets_INCLUDE_DIRS}>")

target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../..>)
if (WIN32)
 target_include_directories (${LIBRARY} PUBLIC
                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../error>)
endif (WIN32)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../log>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../task>)

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
                ${absolute_source_files})
target_precompile_headers (${LIBRARY}
                           PRIVATE stdafx.h)

#get_target_property (debug ${PROJECT_NAME}UI_GTK INTERFACE_INCLUDE_DIRECTORIES)
#message (STATUS "INTERFACE_INCLUDE_DIRECTORIES: \"${debug}\"")

if (WIN32)
#*TODO*: use pkg-config instead
 add_dependencies (${LIBRARY} Common)

 if (CMAKE_BUILD_TYPE STREQUAL Debug)
  target_link_libraries (${LIBRARY} Msvcrtd.lib)
 else ()
  target_link_libraries (${LIBRARY} Msvcrt.lib)
 endif ()

 target_link_libraries (${LIBRARY} ${wxWidgets_LIBRARIES})

# target_link_libraries (${LIBRARY} ${ACE_LIBRARY})

 target_link_libraries (${LIBRARY} Common)
endif (WIN32)

#set_target_properties (${LIBRARY} PROPERTIES DEFINE_SYMBOL COMMON_UI_BUILD_DLL)
#set_target_properties (${LIBRARY} PROPERTIES VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_MICRO} SOVERSION 1)

set_property (TARGET ${LIBRARY} PROPERTY FOLDER "modules/ui")

# add all targets to the build-tree export set
export (TARGETS ${LIBRARY}
        APPEND FILE "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake")

############### install files ################
set (${LIBRARY}_LIB_HDRS
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

install (TARGETS ${LIBRARY}
         EXPORT ${PROJECT_NAME}Targets
         ARCHIVE DESTINATION "${INSTALL_LIB_DIR}"
         COMPONENT lib)
