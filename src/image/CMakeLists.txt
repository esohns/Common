include (ffmpeg)
include (ImageMagick)

set (CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/scripts")
include (png)

set (LIBRARY ${PROJECT_NAME}Image)
add_library (${LIBRARY} STATIC "")

if (LIBPNG_SUPPORT)
 target_include_directories (${LIBRARY} PUBLIC
                             "$<BUILD_INTERFACE:${PNG_INCLUDE_DIRS}>")
endif (LIBPNG_SUPPORT)
if (FFMPEG_SUPPORT)
# message (STATUS "ffmpeg_INCLUDE_DIRS: ${ffmpeg_INCLUDE_DIRS}")
 target_include_directories (${LIBRARY} PUBLIC
                             $<BUILD_INTERFACE:${ffmpeg_INCLUDE_DIRS}>)
endif (FFMPEG_SUPPORT)
if (IMAGEMAGICK_SUPPORT)
 target_include_directories (${LIBRARY} PUBLIC
                             ${ImageMagick_INCLUDE_DIRS})
endif (IMAGEMAGICK_SUPPORT)
if (UNIX)
elseif (WIN32)
 if (DEFINED ENV{DXSDK_DIR})
  target_include_directories (${LIBRARY} PUBLIC
                              $<BUILD_INTERFACE:$ENV{DXSDK_DIR}/Include>)
 elseif (VCPKG_SUPPORT)
#  message (STATUS "DXSDK include dir: ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include")
  target_include_directories (${LIBRARY} PUBLIC
                              $<BUILD_INTERFACE:${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include>)
 else ()
  target_include_directories (${LIBRARY} PUBLIC
                              $<BUILD_INTERFACE:$ENV{LIB_ROOT}/DXSDK/Include>)
 endif ()
endif ()

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
if (UNIX)
elseif (WIN32)
 target_link_libraries (${LIBRARY} PUBLIC $ENV{LIB_ROOT}/DXSDK/Lib/${CMAKE_VS_PLATFORM_TOOLSET_HOST_ARCHITECTURE}/d3dx9.lib)
endif ()

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
