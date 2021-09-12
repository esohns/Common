set (CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/scripts")
include (PreXMLImplementation)
include (SetXMLImplementation)

set (LIBRARY ${PROJECT_NAME}XML)
add_library (${LIBRARY} STATIC "")

if (LIBXML2_SUPPORT)
 target_include_directories (${LIBRARY} PUBLIC
                             "$<BUILD_INTERFACE:${LIBXML2_INCLUDE_DIRS}>")
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
#set_target_properties (${PROJECT_NAME}XML PROPERTIES
#                       VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_MICRO} SOVERSION 1)

if (LIBXML2_SUPPORT)
 target_link_libraries (${PROJECT_NAME}XML ${LIBXML2_LIBRARIES})
endif (LIBXML2_SUPPORT)

if (ACEXML_SUPPORT)
 target_link_libraries (${PROJECT_NAME}XML ${ACEXML_LIBRARY})
 target_link_libraries (${PROJECT_NAME}XML ${ACEXML_PARSER_LIBRARY})
endif (ACEXML_SUPPORT)

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
