set (LIBRARY ${PROJECT_NAME}SDL)
add_library (${LIBRARY} STATIC "")

target_include_directories (${LIBRARY} PUBLIC
                            "$<BUILD_INTERFACE:${SDL_INCLUDE_DIRS}>")

target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${ACE_INCLUDE_DIR}>)

target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>)
if (WIN32)
 target_include_directories (${LIBRARY} PUBLIC
                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../include>)
endif (WIN32)

############## next target ################
set (source_files
     common_sdl_common.h
     common_sdl_defines.h
     common_sdl_tools.cpp
     common_sdl_tools.h)

unset (absolute_source_files)
foreach (source_file ${source_files})
 get_filename_component (abs_source_file ${source_file} ABSOLUTE)
 list (APPEND absolute_source_files
       PRIVATE ${abs_source_file}
       )
endforeach ()
target_sources (${LIBRARY}
#                PRIVATE stdafx.cpp
                PRIVATE stdafx.h
                ${absolute_source_files})

#add_dependencies (${PROJECT_NAME} ACE)

target_link_libraries (${LIBRARY} PUBLIC ${SDL_LIBRARIES})

# add PCH
target_precompile_headers (${LIBRARY}
                           PRIVATE stdafx.h)

set_property (TARGET ${LIBRARY} PROPERTY FOLDER "modules")

# add all targets to the build-tree export set
export (TARGETS ${LIBRARY}
        APPEND FILE "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake")

############### install files ################
 set (source_files
      common_sdl_common.h
      common_sdl_defines.h
      common_sdl_tools.h)
install (FILES
         ${source_files}
         DESTINATION ${INSTALL_INCLUDE_DIR}
         COMPONENT include)
install (TARGETS ${LIBRARY}
         EXPORT ${PROJECT_NAME}Targets
         ARCHIVE DESTINATION ${INSTALL_LIB_DIR}
         COMPONENT lib)
