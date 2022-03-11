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
                            $<BUILD_INTERFACE:${ACE_INCLUDE_DIR}>)

target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}>)
if (WIN32)
 target_include_directories (${LIBRARY} PUBLIC
                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../include>)
endif (WIN32)
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
target_precompile_headers (${LIBRARY}
                           PRIVATE stdafx.h)

#add_dependencies (${LIBRARY} ACE)

#set_target_properties (${LIBRARY} PROPERTIES
#                       VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_MICRO} SOVERSION 1)

if (LIBXML2_SUPPORT)
 target_link_libraries (${LIBRARY} PUBLIC ${LIBXML2_LIBRARIES})
endif (LIBXML2_SUPPORT)

if (ACEXML_SUPPORT)
 target_link_libraries (${LIBRARY} PUBLIC ${ACEXML_LIBRARIES})
endif (ACEXML_SUPPORT)

set_property (TARGET ${LIBRARY} PROPERTY FOLDER "modules")

# add all targets to the build-tree export set
export (TARGETS ${LIBRARY}
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
install (TARGETS ${LIBRARY}
         EXPORT ${PROJECT_NAME}Targets
         ARCHIVE DESTINATION "${INSTALL_LIB_DIR}"
         COMPONENT lib)
