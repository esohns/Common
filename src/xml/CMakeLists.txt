set (CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/scripts")
include (PreXMLImplementation)
include (SetXMLImplementation)

set (ACE_XML_LIB_FILE libACEXML.so)
if (UNIX)
 find_library (ACE_XML_LIBRARY ${ACE_XML_LIB_FILE}
               PATHS ${CMAKE_CURRENT_SOURCE_DIR}/../../modules/ACE/ACEXML $ENV{ACE_ROOT}/ACEXML
               PATH_SUFFIXES common
               DOC "searching for ${ACE_XML_LIB_FILE} (system paths)")
elseif (WIN32)
 if (CMAKE_BUILD_TYPE STREQUAL Debug)
  set (LIB_FILE_SUFFIX d)
 endif ()
 unset (ACE_XML_LIB_FILE)
 set (ACE_XML_LIB_FILE ACEXML${LIB_FILE_SUFFIX}.lib)
 find_library (ACE_XML_LIBRARY ${ACE_XML_LIB_FILE}
               PATHS $ENV{LIB_ROOT}/ACE_TAO/ACE
               PATH_SUFFIXES lib lib\\${CMAKE_BUILD_TYPE}\\Win32
               DOC "searching for ${ACE_XML_LIB_FILE}")
endif ()
#if (NOT EXISTS ACE_XML_LIBRARY)
if (NOT ACE_XML_LIBRARY)
 message (FATAL_ERROR "Could not find ${ACE_XML_LIB_FILE} (was: \"${ACE_XML_LIBRARY}\"), aborting")
else ()
 #message (STATUS "Found ACEXML library \"${ACE_XML_LIBRARY}\"")
endif ()
set (ACE_XML_PARSER_LIB_FILE libACEXML_Parser.so)
if (UNIX)
# message ("CMAKE_FIND_ROOT_PATH is ${CMAKE_FIND_ROOT_PATH}")
 find_library (ACE_XML_PARSER_LIBRARY ${ACE_XML_PARSER_LIB_FILE}
               PATHS $ENV{ACE_ROOT}/ACEXML/parser /usr/local/src/ACE_wrappers/ACEXML/parser
               PATH_SUFFIXES lib parser
               DOC "searching for ${ACE_XML_PARSER_LIB_FILE}")
# else ()
#  find_library (ACE_XML_PARSER_LIBRARY ${ACE_XML_PARSER_LIB_FILE}
#                DOC "searching for ${ACE_XML_PARSER_LIB_FILE} (system paths)")
# endif ()
elseif (WIN32)
 if (CMAKE_BUILD_TYPE STREQUAL Debug)
  set (LIB_FILE_SUFFIX d)
 endif ()
 unset (ACE_XML_PARSER_LIB_FILE)
 set (ACE_XML_PARSER_LIB_FILE ACEXML_Parser${LIB_FILE_SUFFIX}.lib)
 find_library (ACE_XML_PARSER_LIBRARY ${ACE_XML_PARSER_LIB_FILE}
               PATHS $ENV{LIB_ROOT}/ACE_TAO/ACE
               PATH_SUFFIXES lib lib\\${CMAKE_BUILD_TYPE}\\Win32
               DOC "searching for ${ACE_XML_PARSER_LIB_FILE}")
endif ()
#if (NOT EXISTS ACE_XML_PARSER_LIBRARY)
if (NOT ACE_XML_PARSER_LIBRARY)
 message (FATAL_ERROR "Could not find ${ACE_XML_PARSER_LIB_FILE} (was: \"${ACE_XML_PARSER_LIBRARY}\"), aborting")
else ()
 #message (STATUS "Found ACEXML parser library \"${ACE_XML_PARSER_LIBRARY}\"")
endif ()

set (LIBRARY ${PROJECT_NAME}XML)
add_library (${LIBRARY} STATIC "")

if (UNIX)
elseif (WIN32)
 if (LIBXML2_SUPPORT)
  target_include_directories (${LIBRARY} PUBLIC
                              $<BUILD_INTERFACE:$ENV{LIB_ROOT}/libiconv-1.9.2-1/include>)
 endif (LIBXML2_SUPPORT)
endif ()
if (LIBXML2_SUPPORT)
 target_include_directories (${LIBRARY} PUBLIC
                             $<BUILD_INTERFACE:${LIBXML2_INCLUDE_DIR}>)
endif (LIBXML2_SUPPORT)

target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/../..>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../include>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>)

############## next target ################
set (source_files
     common_xml_common.h
     common_xml_defines.h
     common_xml_handler.cpp
     common_xml_handler.h
     common_xml_handler_base.cpp
     common_xml_handler_base.h
     common_xml_handler_base.inl
     common_xml_parser.cpp
     common_xml_parser.h
     common_xml_parser.inl
     common_xml_tools.cpp
     common_xml_tools.h)

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
target_sources (${PROJECT_NAME}XML
                PRIVATE stdafx.cpp
                PRIVATE stdafx.h
                ${absolute_source_files})

#add_definitions (-DHAVE_CONFIG_H)
#set_property (SOURCE PROPERTY COMPILE_DEFINITIONS HAVE_CONFIG_H)

#add_dependencies (${PROJECT_NAME}XML ACE)

#set_target_properties (${PROJECT_NAME}XML PROPERTIES COMPILE_DEFINITIONS ACE_HAS_DLL)
#set_target_properties (${PROJECT_NAME}XML PROPERTIES
#                       DEFINE_SYMBOL COMMON_XML_BUILD_DLL)
set_target_properties (${PROJECT_NAME}XML PROPERTIES
                       VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_MICRO}
                       SOVERSION 1)

target_link_libraries (${PROJECT_NAME}XML ${ACE_XML_LIBRARY})
target_link_libraries (${PROJECT_NAME}XML ${ACE_XML_PARSER_LIBRARY})

# add PCH
set_target_properties (${PROJECT_NAME}XML PROPERTIES
                       COTIRE_CXX_PREFIX_HEADER_INIT "stdafx.h")
cotire (${PROJECT_NAME}XML)

# add all targets to the build-tree export set
export (TARGETS ${PROJECT_NAME}XML
        APPEND FILE "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake")

############### install files ################
install (FILES
         common_xml_common.h
         common_xml_defines.h
         common_xml_handler.h
         common_xml_handler_base.h
         common_xml_handler_base.inl
         common_xml_parser.h
         common_xml_parser.inl
         common_xml_tools.h
         DESTINATION ${INSTALL_INCLUDE_DIR})
install (TARGETS ${PROJECT_NAME}XML
         EXPORT ${PROJECT_NAME}Targets
         ARCHIVE DESTINATION "${INSTALL_LIB_DIR}"
         COMPONENT lib)
