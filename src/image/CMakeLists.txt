option (LIBPNG_SUPPORT "compile libpng support" OFF)
if (LIBPNG_SUPPORT)
 add_definitions (-DLIBPNG_SUPPORT)
endif (LIBPNG_SUPPORT)

include (${CMAKE_CURRENT_SOURCE_DIR}/../../scripts/ffmpeg.cmake)

option (IMAGEMAGICK_SUPPORT "compile ImageMagick support" ON)
if (IMAGEMAGICK_SUPPORT)
 add_definitions (-DIMAGEMAGICK_SUPPORT)
endif (IMAGEMAGICK_SUPPORT)

if (UNIX)
 if (IMAGEMAGICK_SUPPORT)
  find_package (ImageMagick COMPONENTS MagickCore MagickWand)
 endif (IMAGEMAGICK_SUPPORT)
endif (UNIX)

set (LIBRARY ${PROJECT_NAME}Image)
add_library (${LIBRARY} STATIC "")

if (UNIX)
 if (FFMPEG_SUPPORT)
  target_include_directories (${LIBRARY} PUBLIC
                              $<BUILD_INTERFACE:${PKG_FFMPEG_INCLUDE_DIRS}>)
 endif (FFMPEG_SUPPORT)

 if (IMAGEMAGICK_SUPPORT)
  target_include_directories (${LIBRARY} PUBLIC
                              ${ImageMagick_INCLUDE_DIRS})
 endif (IMAGEMAGICK_SUPPORT)
elseif (WIN32)
 if (DEFINED ENV{DXSDK_DIR})
  target_include_directories (${LIBRARY} PUBLIC
                              $<BUILD_INTERFACE:$ENV{DXSDK_DIR}/Include>)
 else ()
 # set (DXSDK_DIR
 #      "$ENV{PROGRAMFILES}/Microsoft DirectX SDK (June 2010)/Include")
# message (STATUS "DirectX SDK directory: \"${DXSDK_DIR}\"")
 # target_include_directories (${LIBRARY} PUBLIC
 #                             $<BUILD_INTERFACE:${DXSDK_DIR}>)
  target_include_directories (${LIBRARY} PUBLIC
                              $<BUILD_INTERFACE:$ENV{LIB_ROOT}/DXSDK/Include>)
 endif (DEFINED ENV{DXSDK_DIR})

 if (LIBPNG_SUPPORT)
  target_include_directories (${LIBRARY} PUBLIC
                              $<BUILD_INTERFACE:$ENV{LIB_ROOT}/libpng>)
 endif (LIBPNG_SUPPORT)

 if (FFMPEG_SUPPORT)
  target_include_directories (${LIBRARY} PUBLIC
                              $<BUILD_INTERFACE:$ENV{LIB_ROOT}/ffmpeg/include>)
#                             $<BUILD_INTERFACE:$ENV{LIB_ROOT}/ffmpeg>)
 endif (FFMPEG_SUPPORT)

 if (IMAGEMAGICK_SUPPORT)
  target_include_directories (${LIBRARY} PUBLIC
                              $<BUILD_INTERFACE:$ENV{LIB_ROOT}/ImageMagick/include>)
 endif (IMAGEMAGICK_SUPPORT)
endif ()
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/../..>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../error>)

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
                PRIVATE stdafx.cpp
                PRIVATE stdafx.h
                ${absolute_source_files})

if (WIN32)
## set (CMAKE_GENERATOR_TOOLSET "LLVM-vs2014" CACHE STRING "Platform Toolset" FORCE)
# set (CMAKE_VS_PLATFORM_TOOLSET "LLVM-vs2014" CACHE STRING "Platform Toolset" FORCE)
 if (CMAKE_VS_PLATFORM_TOOLSET STREQUAL LLVM-vs2014)
  add_compile_options (-Wno-c++11-narrowing)
 endif ()
endif (WIN32)

if (UNIX)
 if (FFMPEG_SUPPORT)
  target_link_libraries (${LIBRARY} ${PKG_FFMPEG_LIBRARIES})
 endif (FFMPEG_SUPPORT)
   
 if (IMAGEMAGICK_SUPPORT)
  target_link_libraries (${LIBRARY} ${ImageMagick_LIBRARIES})
 endif (IMAGEMAGICK_SUPPORT)
elseif (WIN32)
 target_link_libraries (${LIBRARY} $ENV{LIB_ROOT}/DXSDK/Lib/x86/d3dx9.lib)

# libpng
 if (LIBPNG_SUPPORT)
  target_link_libraries (${LIBRARY} $ENV{LIB_ROOT}/libpng/cmake/${CMAKE_BUILD_TYPE}/libpng16${LIB_FILE_SUFFIX}.lib)
# target_link_libraries (${LIBRARY} ${CMAKE_CURRENT_SOURCE_DIR}/../../../libpng-1.6.25/projects/vstudio/Debug/libpng16.lib)
 endif (LIBPNG_SUPPORT)

# ffmpeg
 if (FFMPEG_SUPPORT)
  target_link_options (${LIBRARY} PUBLIC "$<$<CXX_COMPILER_ID:MSVC>:-SAFESEH:NO>")
  target_link_libraries (${LIBRARY} $ENV{LIB_ROOT}/ffmpeg/lib/avcodec.lib)
  target_link_libraries (${LIBRARY} $ENV{LIB_ROOT}/ffmpeg/lib/avutil.lib)
  target_link_libraries (${LIBRARY} $ENV{LIB_ROOT}/ffmpeg/lib/swscale.lib)
 # target_link_libraries (${LIBRARY} $ENV{LIB_ROOT}/ffmpeg/libavcodec/avcodec.lib)
 # target_link_libraries (${LIBRARY} $ENV{LIB_ROOT}/ffmpeg/libavutil/avutil.lib)
 endif (FFMPEG_SUPPORT)

 if (IMAGEMAGICK_SUPPORT)
  target_link_libraries (${LIBRARY} $ENV{LIB_ROOT}/ImageMagick/lib/CORE_RL_MagickCore_.lib)
  target_link_libraries (${LIBRARY} $ENV{LIB_ROOT}/ImageMagick/lib/CORE_RL_MagickWand_.lib)
 endif (IMAGEMAGICK_SUPPORT)
endif ()
#set_target_properties (${LIBRARY} PROPERTIES DEFINE_SYMBOL COMMON_IMAGE_BUILD_DLL)
set_target_properties (${LIBRARY} PROPERTIES
                       VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_MICRO}
                       SOVERSION 1)

# add PCH
set_target_properties (${LIBRARY} PROPERTIES
                       COTIRE_CXX_PREFIX_HEADER_INIT "stdafx.h")
cotire (${LIBRARY})

# add all targets to the build-tree export set
export (TARGETS ${LIBRARY}
        APPEND FILE "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake")

############### install files ################
install (FILES
         common_image_common.h
         common_image_defines.h
         common_image_tools.h
         stdafx.h
         DESTINATION ${INSTALL_INCLUDE_DIR})
#install (TARGETS ${LIBRARY}
#         EXPORT ${PROJECT_NAME}Targets
#         ARCHIVE DESTINATION "${INSTALL_LIB_DIR}"
#         COMPONENT lib)
