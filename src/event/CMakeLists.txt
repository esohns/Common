set (LIBRARY ${PROJECT_NAME}Event)
add_library (${LIBRARY} STATIC "")

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
if (UNIX)
 target_include_directories (${LIBRARY} PUBLIC
                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../error>)
endif (UNIX)

############## next target ################
set (source_files
     common_event_common.h
     common_event_defines.h
     common_event_handler_base.cpp
     common_event_handler_base.h
     common_event_tools.cpp
     common_event_tools.h)
if (WIN32)
 set (source_files
      ${source_files}
      common_event.cpp
      common_event.h)
endif (WIN32)

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
                PRIVATE stdafx.h
                PRIVATE ${absolute_source_files})

#add_dependencies (${LIBRARY} ACE)
if (UNIX)
 add_dependencies (${LIBRARY} ${PROJECT_NAME})
 add_dependencies (${LIBRARY} ${PROJECT_NAME}Error)

 target_link_libraries (${LIBRARY} PUBLIC ${PROJECT_NAME})
 target_link_libraries (${LIBRARY} PUBLIC ${PROJECT_NAME}Error)
endif (UNIX)

#set_target_properties (${LIBRARY} PROPERTIES
#                       VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_MICRO}
#                       SOVERSION 1)

# add PCH
target_precompile_headers (${LIBRARY}
                           PRIVATE stdafx.h)

set_property (TARGET ${LIBRARY} PROPERTY FOLDER "modules")

# add all targets to the build-tree export set
export (TARGETS ${LIBRARY}
        APPEND FILE "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake")

############### install files ################
install (FILES
         common_event_common.h
         common_event_defines.h
         common_event_handler_base.h
         common_event_tools.h
         DESTINATION ${INSTALL_INCLUDE_DIR})
if (WIN32)
 install (FILES
          common_event.h
          DESTINATION ${INSTALL_INCLUDE_DIR})
endif (WIN32)
install (TARGETS ${LIBRARY}
         EXPORT ${PROJECT_NAME}Targets
         ARCHIVE DESTINATION "${INSTALL_LIB_DIR}"
         COMPONENT lib)
