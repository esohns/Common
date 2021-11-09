include (ImageMagick)

set (CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/scripts")
include (assimp)
include (glm)
include (png)

set (LIBRARY ${PROJECT_NAME}OpenGL)
add_library (${LIBRARY} STATIC "")

if (IMAGEMAGICK_SUPPORT)
 target_include_directories (${LIBRARY} PUBLIC
                             "$<BUILD_INTERFACE:${ImageMagick_INCLUDE_DIRS}>")
endif (IMAGEMAGICK_SUPPORT)

if (GLM_SUPPORT)
 target_include_directories (${LIBRARY} PUBLIC
                             "$<BUILD_INTERFACE:${GLM_INCLUDE_DIRS}>")
endif (GLM_SUPPORT)
if (LIBPNG_SUPPORT)
 target_include_directories (${LIBRARY} PUBLIC
                             "$<BUILD_INTERFACE:${PNG_INCLUDE_DIRS}>")
endif (LIBPNG_SUPPORT)
if (ASSIMP_SUPPORT)
 target_include_directories (${LIBRARY} PUBLIC
                             "$<BUILD_INTERFACE:${ASSIMP_INCLUDE_DIRS}>")
endif (ASSIMP_SUPPORT)

target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/../..>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../image>)

############## next target ################
set (source_files
     common_gl_common.h
     common_gl_defines.h
     common_gl_tools.cpp
     common_gl_tools.h
     common_gl_image_tools.cpp
     common_gl_image_tools.h
     common_gl_texture.cpp
     common_gl_texture.h)
if (ASSIMP_SUPPORT)
 set (source_files
      ${source_files}
      common_gl_assimp_tools.cpp
      common_gl_assimp_tools.h)
endif (ASSIMP_SUPPORT)

# Include dir for installed targets
#set (INCLUDE_DIR include/${PROJECT_NAME}-${upstream_version})

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

#add_dependencies (${PROJECT_NAME} ACE)

#set_target_properties (${PROJECT_NAME} PROPERTIES DEFINE_SYMBOL COMMON_BUILD_DLL)
#set_target_properties (${LIBRARY} PROPERTIES VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_MICRO} SOVERSION 1)

target_link_libraries (${LIBRARY} PUBLIC ${OPENGL_LIBRARIES})

if (IMAGEMAGICK_SUPPORT)
 target_link_libraries (${LIBRARY} PUBLIC ${ImageMagick_LIBRARIES})
endif (IMAGEMAGICK_SUPPORT)

if (LIBPNG_SUPPORT)
 target_link_libraries (${LIBRARY} PUBLIC ${PNG_LIBRARIES})
endif (LIBPNG_SUPPORT)
if (ASSIMP_SUPPORT)
 target_link_libraries (${LIBRARY} PUBLIC ${ASSIMP_LIBRARIES})
endif (ASSIMP_SUPPORT)

target_link_libraries (${LIBRARY} PUBLIC ${PROJECT_NAME}Image)

# add PCH
target_precompile_headers (${LIBRARY}
                           PRIVATE stdafx.h)

set_property (TARGET ${LIBRARY} PROPERTY FOLDER "modules")

# add all targets to the build-tree export set
export (TARGETS ${LIBRARY}
        APPEND FILE "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake")

############### install files ################
 set (source_files
      common_gl_common.h
      common_gl_defines.h
      common_gl_image_tools.h
      common_gl_texture.h
      common_gl_tools.h)
if (ASSIMP_SUPPORT)
 set (source_files
      ${source_files}
      common_gl_assimp_tools.h)
endif (ASSIMP_SUPPORT)
install (FILES
         ${source_files}
         DESTINATION ${INSTALL_INCLUDE_DIR})
#install (TARGETS ${LIBRARY}
#         EXPORT ${PROJECT_NAME}Targets
#         ARCHIVE DESTINATION "${INSTALL_LIB_DIR}"
#         COMPONENT lib)
