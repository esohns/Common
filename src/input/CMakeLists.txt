set (LIBRARY ${PROJECT_NAME}Input)
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
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../event>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../task>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../timer>)

############## next target ################
set (source_files
     common_input_common.h
     common_input_defines.h
     common_input_handler_base.cpp
     common_input_handler_base.h
     common_input_handler_base.inl
     common_input_manager.cpp
     common_input_manager.h
     common_input_manager.inl
     common_input_tools.cpp
     common_input_tools.h)

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
#                PRIVATE stdafx.cpp
                PRIVATE stdafx.h
                ${absolute_source_files})

if (WIN32)
 if (CMAKE_VS_PLATFORM_TOOLSET STREQUAL LLVM-vs2014)
  add_compile_options (-Wno-c++11-narrowing)
 endif (CMAKE_VS_PLATFORM_TOOLSET STREQUAL LLVM-vs2014)
endif (WIN32)

#*TODO*: use pkg-config instead
if (WIN32)
 target_link_libraries (${LIBRARY} PUBLIC ${ACE_LIBRARY})
endif (WIN32)
target_link_libraries (${LIBRARY} PUBLIC ${PROJECT_NAME}Event)
target_link_libraries (${LIBRARY} PUBLIC ${PROJECT_NAME}Task)

#set_target_properties (${LIBRARY} PROPERTIES DEFINE_SYMBOL COMMON_PARSER_BUILD_DLL)
#set_target_properties (${LIBRARY} PROPERTIES VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_MICRO} SOVERSION 1)

# add PCH
target_precompile_headers (${LIBRARY}
                           PRIVATE stdafx.h)

set_property (TARGET ${LIBRARY} PROPERTY FOLDER "modules")

# add all targets to the build-tree export set
export (TARGETS ${LIBRARY}
        APPEND FILE "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake")

############### install files ################
install (FILES
         common_input_common.h
         common_input_defines.h
         common_input_handler_base.h
         common_input_handler_base.inl
         common_input_manager.h
         common_input_manager.inl
         common_input_tools.h
         DESTINATION include)
install (TARGETS ${LIBRARY}
         EXPORT ${PROJECT_NAME}Targets
         ARCHIVE DESTINATION "${INSTALL_LIB_DIR}"
         COMPONENT lib)
