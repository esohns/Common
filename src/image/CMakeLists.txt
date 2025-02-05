if (WIN32)
 include (directxsdk)
endif (WIN32)

set (CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/scripts")
include (png)
include (stb)

include (ffmpeg)
include (ImageMagick)

set (LIBRARY ${PROJECT_NAME}Image)
add_library (${LIBRARY} STATIC "")

if (WIN32 AND DIRECTXSDK_SUPPORT)
 target_include_directories (${LIBRARY} PUBLIC
                             "$<BUILD_INTERFACE:${DIRECTXSDK_INCLUDE_DIR}>")
endif (WIN32 AND DIRECTXSDK_SUPPORT)

if (LIBPNG_SUPPORT)
 target_include_directories (${LIBRARY} PUBLIC
                             "$<BUILD_INTERFACE:${PNG_INCLUDE_DIRS}>")
endif (LIBPNG_SUPPORT)
if (STB_IMAGE_SUPPORT)
 target_include_directories (${LIBRARY} PUBLIC
                             "$<BUILD_INTERFACE:${STB_IMAGE_INCLUDE_DIRS}>")
endif (STB_IMAGE_SUPPORT)

if (FFMPEG_SUPPORT)
 target_include_directories (${LIBRARY} PUBLIC
                             $<BUILD_INTERFACE:${ffmpeg_INCLUDE_DIRS}>)
endif (FFMPEG_SUPPORT)
if (IMAGEMAGICK_SUPPORT)
 target_compile_options (${LIBRARY} PUBLIC
                         ${MAGICK_CXX_FLAGS})
 target_include_directories (${LIBRARY} PUBLIC
                             ${ImageMagick_INCLUDE_DIRS})
endif (IMAGEMAGICK_SUPPORT)

target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${ACE_INCLUDE_DIR}>)

target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}>)
if (WIN32)
 target_include_directories (${LIBRARY} PUBLIC
                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../include>)
 target_include_directories (${LIBRARY} PUBLIC
                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../error>)
endif (WIN32)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>)

############## next target ################
set (source_files
     common_image_common.h
     common_image_defines.h
     common_image_tools.cpp
     common_image_tools.h)

# Include dir for installed targets
#set (INCLUDE_DIR include/${PROJECT_NAME}-${upstream_version})

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

if (WIN32 AND DIRECTXSDK_SUPPORT)
 target_link_libraries (${LIBRARY} PUBLIC ${DIRECTXSDK_LIBRARIES})
endif (WIN32 AND DIRECTXSDK_SUPPORT)
if (LIBPNG_SUPPORT)
 target_link_libraries (${LIBRARY} PUBLIC ${PNG_LIBRARIES})
endif (LIBPNG_SUPPORT)
if (FFMPEG_SUPPORT)
 target_link_libraries (${LIBRARY} PUBLIC ${ffmpeg_LIBRARIES})
 target_link_libraries (${LIBRARY} PUBLIC ${ffmpeg_2_LIBRARIES})
endif (FFMPEG_SUPPORT)
if (IMAGEMAGICK_SUPPORT)
 target_link_libraries (${LIBRARY} PUBLIC ${ImageMagick_LIBRARIES})
endif (IMAGEMAGICK_SUPPORT)

#set_target_properties (${LIBRARY} PROPERTIES DEFINE_SYMBOL COMMON_IMAGE_BUILD_DLL)
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
         common_image_common.h
         common_image_defines.h
         common_image_tools.h
         DESTINATION ${INSTALL_INCLUDE_DIR})
#install (TARGETS ${LIBRARY}
#         EXPORT ${PROJECT_NAME}Targets
#         ARCHIVE DESTINATION "${INSTALL_LIB_DIR}"
#         COMPONENT lib)
