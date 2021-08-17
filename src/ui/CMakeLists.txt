include (${CMAKE_CURRENT_SOURCE_DIR}/scripts/PreUIFramework.cmake)
include (${CMAKE_CURRENT_SOURCE_DIR}/scripts/SetUIFramework.cmake)
if (GTK_SUPPORT)
 include (${CMAKE_CURRENT_SOURCE_DIR}/scripts/SetGTKVersion.cmake)
endif (GTK_SUPPORT)

set (ACEXML_LIB_FILE libACEXML.so)
if (UNIX)
# message ("CMAKE_FIND_ROOT_PATH is ${CMAKE_FIND_ROOT_PATH}")
 find_library (ACE_XML_LIBRARY ${ACEXML_LIB_FILE}
               PATHS $ENV{ACE_ROOT}/ACEXML /usr/local/src/ACE_wrappers/ACEXML
               PATH_SUFFIXES common
               DOC "searching for ${ACEXML_LIB_FILE} (system paths)")
# else ()
#  find_library (ACE_XML_LIBRARY ${ACEXML_LIB_FILE}
#                DOC "searching for ${ACEXML_LIB_FILE} (system paths)")
# endif ()
elseif (WIN32)
 if (CMAKE_BUILD_TYPE STREQUAL Debug)
  set (LIB_FILE_SUFFIX d)
 endif ()
 unset (ACEXML_LIB_FILE)
 set (ACEXML_LIB_FILE ACEXML${LIB_FILE_SUFFIX}.lib)
 find_library (ACE_XML_LIBRARY ${ACEXML_LIB_FILE}
               PATHS $ENV{LIB_ROOT}/ACE_TAO/ACE
               PATH_SUFFIXES lib lib\\${CMAKE_BUILD_TYPE}\\Win32
               DOC "searching for ${ACEXML_LIB_FILE}")
endif ()
#if (NOT EXISTS ACE_XML_LIBRARY)
if (NOT ACE_XML_LIBRARY)
 message (FATAL_ERROR "Could not find ${ACEXML_LIB_FILE} (was: \"${ACE_XML_LIBRARY}\"), aborting")
else ()
 #message (STATUS "Found ACEXML library \"${ACE_XML_LIBRARY}\"")
endif ()
#add_definitions (-DACEXML_HAS_DLL)

set (LIBRARY ${PROJECT_NAME}UI)
add_library (${LIBRARY} STATIC "")

if (GTK_SUPPORT)
 message (STATUS "GTK_INCLUDE_DIRS: ${GTK_INCLUDE_DIRS}")
 target_include_directories (${LIBRARY} PUBLIC
                             "$<BUILD_INTERFACE:${GTK_INCLUDE_DIRS}>")
endif (GTK_SUPPORT)

#if (UNIX)
# target_include_directories (${LIBRARY} PUBLIC
#                             $<BUILD_INTERFACE:$ENV{LIB_ROOT}/ACE_TAO/ACE>)
#                             $<BUILD_INTERFACE:/usr/local/src/ACE_wrappers>)
#elseif (WIN32)
# if (DEFINED ENV{ACE_ROOT})
#  target_include_directories (${LIBRARY} PUBLIC
#                              $<BUILD_INTERFACE:$ENV{ACE_ROOT}>)
# else ()
#  target_include_directories (${LIBRARY} PUBLIC
#                              $<BUILD_INTERFACE:$ENV{LIB_ROOT}/ACE_TAO/ACE>)
# endif (DEFINED ENV{ACE_ROOT})
#endif ()

target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/../..>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../include>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../error>)
#target_include_directories (${LIBRARY} PUBLIC
#                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../image>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../log>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../xml>)

############## next target ################
set (source_files
     common_ui_common.h
     common_ui_defines.h
#     common_ui_exports.h
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
      common_ui_monitor_setup_xml_handler.h)
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
                PRIVATE stdafx.cpp
                PRIVATE stdafx.h
                ${absolute_source_files})

add_dependencies (${LIBRARY} Common)
if (UNIX)
 target_link_libraries (${LIBRARY} X11 Xrandr)
elseif (WIN32)
# target_link_libraries (${LIBRARY} Common)
endif ()

if (WIN32)
# set_target_properties (${LIBRARY} PROPERTIES LINK_FLAGS /SAFESEH:NO)
endif ()
set_target_properties (${LIBRARY} PROPERTIES DEFINE_SYMBOL COMMON_UI_BUILD_DLL)
set_target_properties (${LIBRARY} PROPERTIES VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_MICRO} SOVERSION 1)

# add PCH
set_target_properties (${LIBRARY} PROPERTIES COTIRE_CXX_PREFIX_HEADER_INIT "stdafx.h")
cotire (${LIBRARY})

# add all targets to the build-tree export set
export (TARGETS ${LIBRARY}
        APPEND FILE "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake")

############### install files ################
set (${LIBRARY}_LIB_HDRS
     common_ui_common.h
     common_ui_defines.h
#     common_ui_exports.h
     common_ui_idefinition.h
     common_ui_ifullscreen.h
     common_ui_tools.h
     common_ui_types.h
     common_ui_windowtype_converter.h
     common_ui_windowtype_converter.inl)
if (UNIX)
 set (${LIBRARY}_LIB_HDRS
      ${${LIBRARY}_LIB_HDRS}
      common_ui_monitor_setup_xml_handler.h)
endif (UNIX)
install (FILES
         ${CommonUI_LIB_HDRS}
         DESTINATION include)
install (TARGETS ${LIBRARY}
         EXPORT ${PROJECT_NAME}Targets
         ARCHIVE DESTINATION "${INSTALL_LIB_DIR}"
         COMPONENT lib)

if (GTK_SUPPORT)
 add_subdirectory (gtk)
endif (GTK_SUPPORT)
if (QT_SUPPORT)
 add_subdirectory (qt)
endif (QT_SUPPORT)
if (WXWIDGETS_SUPPORT)
 add_subdirectory (wxwidgets)
endif (WXWIDGETS_SUPPORT)

