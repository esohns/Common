option (GTK_SUPPORT "enable GTK support" ON)

add_library (${PROJECT_NAME}UI STATIC "")

if (UNIX)
 target_include_directories (${PROJECT_NAME}UI PUBLIC
                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../modules/ACE>)
elseif (WIN32)
 include_directories ($ENV{ACE_ROOT})
# include_directories (${CMAKE_CURRENT_SOURCE_DIR}/err)
endif ()
target_include_directories (${PROJECT_NAME}UI PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>)
target_include_directories (${PROJECT_NAME}UI PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/../..>)

############## next target ################
set (source_files
     common_ui_common.h
     common_ui_defines.h
     common_ui_exports.h
     common_ui_tools.cpp
     common_ui_tools.h
     common_ui_types.h)

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
target_sources (${PROJECT_NAME}UI
                PRIVATE stdafx.cpp
                PRIVATE stdafx.h
                ${absolute_source_files})

add_dependencies (${PROJECT_NAME}UI Common)
if (UNIX)
elseif (WIN32)
# target_link_libraries (${PROJECT_NAME}UI Common)
endif ()

if (WIN32)
# set_target_properties (${PROJECT_NAME}UI PROPERTIES LINK_FLAGS /SAFESEH:NO)
endif ()
set_target_properties (${PROJECT_NAME}UI PROPERTIES DEFINE_SYMBOL COMMON_UI_BUILD_DLL)
set_target_properties (${PROJECT_NAME}UI PROPERTIES VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_MICRO} SOVERSION 1)

# add PCH
set_target_properties (${PROJECT_NAME}UI PROPERTIES COTIRE_CXX_PREFIX_HEADER_INIT "stdafx.h")
cotire (${PROJECT_NAME}UI)

# add all targets to the build-tree export set
export (TARGETS ${PROJECT_NAME}UI
        APPEND FILE "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake")

############### install files ################
set (${PROJECT_NAME}UI_LIB_HDRS
     common_ui_common.h
     common_ui_defines.h
     common_ui_exports.h
     common_ui_tools.h
     common_ui_types.h)

install (FILES
         ${CommonUI_LIB_HDRS}
         DESTINATION include)
install (TARGETS ${PROJECT_NAME}UI
         EXPORT ${PROJECT_NAME}Targets
         ARCHIVE DESTINATION "${INSTALL_LIB_DIR}"
         COMPONENT lib)

if (GTK_SUPPORT)
 add_subdirectory (gtk)
endif ()
