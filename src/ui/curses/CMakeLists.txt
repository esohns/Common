set (LIBRARY ${PROJECT_NAME}UI_Curses)
add_library (${LIBRARY} STATIC "")

get_filename_component (BUILD_PATH ${CMAKE_CURRENT_BINARY_DIR}/../../.. REALPATH)
get_filename_component (BUILD_PATH_SUFFIX ${BUILD_PATH} NAME)

target_include_directories (${LIBRARY} PUBLIC
                            "$<BUILD_INTERFACE:${CURSES_INCLUDE_DIRS}>")

target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/../../..>)
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
     common_ui_curses_common.h
     common_ui_curses_defines.h
     common_ui_curses_eventhandler.cpp
     common_ui_curses_eventhandler.h
     common_ui_curses_eventhandler.inl
#     common_ui_curses_exports.h
     common_ui_curses_manager.cpp
     common_ui_curses_manager.h
     common_ui_curses_manager.inl
     common_ui_curses_manager_common.h
     common_ui_curses_tools.cpp
     common_ui_curses_tools.h)

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

add_dependencies (${LIBRARY} Common)

target_link_libraries (${LIBRARY} ${CURSES_LIBRARIES})

#target_link_libraries (${LIBRARY} ${ACE_LIBRARY})

target_link_libraries (${LIBRARY} Common)

set_property (TARGET ${LIBRARY} PROPERTY FOLDER "modules/ui")

# add all targets to the build-tree export set
export (TARGETS ${LIBRARY}
        APPEND FILE "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake")

############### install files ################
set (${LIBRARY}_LIB_HDRS
     common_ui_curses_common.h
     common_ui_curses_defines.h
     common_ui_curses_eventhandler.h
     common_ui_curses_eventhandler.inl
#     common_ui_curses_exports.h
     common_ui_curses_manager.h
     common_ui_curses_manager.inl
     common_ui_curses_manager_common.h
     common_ui_curses_tools.h)
install (FILES
         ${${LIBRARY}_LIB_HDRS}
         DESTINATION include)

install (TARGETS ${LIBRARY}
         EXPORT ${PROJECT_NAME}Targets
         ARCHIVE DESTINATION "${INSTALL_LIB_DIR}"
         COMPONENT lib)
