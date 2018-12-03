include (${CMAKE_CURRENT_SOURCE_DIR}/scripts/PreUIFramework.cmake)
#include (${CMAKE_CURRENT_SOURCE_DIR}/scripts/SetUIFramework.cmake)
if (GTK_SUPPORT)
# include (${CMAKE_CURRENT_SOURCE_DIR}/scripts/SetGTKVersion.cmake)
endif (GTK_SUPPORT)
if (WXWIDGETS_SUPPORT)
 message (STATUS "wxWidgets_DEFINITIONS: ${wxWidgets_DEFINITIONS}")
 message (STATUS "wxWidgets_DEFINITIONS_DEBUG: ${wxWidgets_DEFINITIONS_DEBUG}")
 message (STATUS "wxWidgets_CXX_FLAGS: ${wxWidgets_CXX_FLAGS}")
 message (STATUS "wxWidgets_USE_FILE: ${wxWidgets_USE_FILE}")
# # *NOTE*: re-enable ANSI mode, iff available
# add_definitions (-DwxUSE_UNICODE=0)
 foreach (DEFINITION ${wxWidgets_DEFINITIONS})
  add_definitions (-D${DEFINITION})
 endforeach ()
 if (WIN32)
#  add_definitions (-D__WXMSW__)
#  add_definitions (-DWXUSINGDLL)
  if (MSVC)
   add_definitions (-DwxMSVC_VERSION_AUTO)
  endif (MSVC)
 endif (WIN32)
 add_definitions (-DwxUSE_GUI=1)
 if (CMAKE_BUILD_TYPE STREQUAL Debug)
  add_definitions (-D__WXDEBUG__)
 endif (CMAKE_BUILD_TYPE STREQUAL Debug)
endif (WXWIDGETS_SUPPORT)

add_library (${PROJECT_NAME}UI STATIC "")

if (UNIX)
 target_include_directories (${PROJECT_NAME}UI PUBLIC
                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../modules/ACE>)
elseif (WIN32)
 if (DEFINED ENV{ACE_ROOT})
  target_include_directories (${PROJECT_NAME}UI PUBLIC
                              $<BUILD_INTERFACE:$ENV{ACE_ROOT}>)
 else ()
  target_include_directories (${PROJECT_NAME}UI PUBLIC
                              $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../ATCD/ACE>)
 endif (DEFINED ENV{ACE_ROOT})
endif ()
target_include_directories (${PROJECT_NAME}UI PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>)
target_include_directories (${PROJECT_NAME}UI PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../error>)
target_include_directories (${PROJECT_NAME}UI PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../log>)
target_include_directories (${PROJECT_NAME}UI PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/../..>)

############## next target ################
set (source_files
     common_ui_common.h
     common_ui_defines.h
     common_ui_exports.h
     common_ui_idefinition.h
     common_ui_ifullscreen.h
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
     common_ui_idefinition.h
     common_ui_ifullscreen.h
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
endif (GTK_SUPPORT)
if (WXWIDGETS_SUPPORT)
 add_subdirectory (wxwidgets)
endif (WXWIDGETS_SUPPORT)
