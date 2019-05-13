option (IMAGEMAGICK_SUPPORT "compile ImageMagick support" ON)
if (IMAGEMAGICK_SUPPORT)
 add_definitions (-DIMAGEMAGICK_SUPPORT)
endif (IMAGEMAGICK_SUPPORT)
option (LIBPNG_SUPPORT "compile libpng support" OFF)
if (LIBPNG_SUPPORT)
 add_definitions (-DLIBPNG_SUPPORT)
endif (LIBPNG_SUPPORT)

if (UNIX)
# find_library (SWSCALE_LIBRARY libswscale.so.5
#               DOC "searching for libswscale")
# if (NOT SWSCALE_LIBRARY)
#  message (FATAL_ERROR "could not find libswscale, aborting")
# endif (NOT SWSCALE_LIBRARY)
# find_library (AVFORMAT_LIBRARY libavformat.so.58
#               DOC "searching for libavformat")
# if (NOT AVFORMAT_LIBRARY)
#  message (FATAL_ERROR "could not find libavformat, aborting")
# endif (NOT AVFORMAT_LIBRARY)
# find_library (AVCODEC_LIBRARY libavcodec.so.58
#               DOC "searching for libavcodec")
# if (NOT AVCODEC_LIBRARY)
#  message (FATAL_ERROR "could not find libavcodec, aborting")
# endif (NOT AVCODEC_LIBRARY)
 pkg_check_modules (PKG_FFMPEG REQUIRED libavcodec libavformat libswscale)
 if (IMAGEMAGICK_SUPPORT)
  find_package (ImageMagick COMPONENTS MagickCore MagickWand)
 endif (IMAGEMAGICK_SUPPORT)
endif (UNIX)

add_library (${PROJECT_NAME}Image STATIC "")

if (UNIX)
 target_include_directories (${PROJECT_NAME}Image PUBLIC
                             $<BUILD_INTERFACE:${PKG_FFMPEG_INCLUDE_DIRS}>)
 target_include_directories (${PROJECT_NAME}Image PUBLIC
                             $<BUILD_INTERFACE:${ImageMagick_INCLUDE_DIRS}>)

 target_include_directories (${PROJECT_NAME}Image PUBLIC
#                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../modules/ACE>)
                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../ACE_TAO/ACE>)

elseif (WIN32)
 if (DEFINED ENV{DXSDK_DIR})
  target_include_directories (${PROJECT_NAME}Image PUBLIC
                              $<BUILD_INTERFACE:$ENV{DXSDK_DIR}/Include>)
 else ()
 # set (DXSDK_DIR
 #      "$ENV{PROGRAMFILES}/Microsoft DirectX SDK (June 2010)/Include")
# message (STATUS "DirectX SDK directory: \"${DXSDK_DIR}\"")
 # target_include_directories (${PROJECT_NAME}Image PUBLIC
 #                             $<BUILD_INTERFACE:${DXSDK_DIR}>)
  target_include_directories (${PROJECT_NAME}Image PUBLIC
                              $<BUILD_INTERFACE:$ENV{LIB_ROOT}/DXSDK/Include>)
 endif (DEFINED ENV{DXSDK_DIR})

 if (LIBPNG_SUPPORT)
  target_include_directories (${PROJECT_NAME}Image PUBLIC
                              $<BUILD_INTERFACE:$ENV{LIB_ROOT}/libpng>)
 endif (LIBPNG_SUPPORT)

 target_include_directories (${PROJECT_NAME}Image PUBLIC
                             $<BUILD_INTERFACE:$ENV{LIB_ROOT}/ffmpeg/include>)
#                             $<BUILD_INTERFACE:$ENV{LIB_ROOT}/ffmpeg>)
 if (IMAGEMAGICK_SUPPORT)
  target_include_directories (${PROJECT_NAME}Image PUBLIC
                              $<BUILD_INTERFACE:$ENV{LIB_ROOT}/ImageMagick/include>)
 endif (IMAGEMAGICK_SUPPORT)

 if (DEFINED ENV{ACE_ROOT})
  target_include_directories (${PROJECT_NAME}Image PUBLIC
                              $<BUILD_INTERFACE:$ENV{ACE_ROOT}>)
 else ()
  target_include_directories (${PROJECT_NAME}Image PUBLIC
                              $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../ATCD/ACE>)
 endif (DEFINED ENV{ACE_ROOT})
endif ()
target_include_directories (${PROJECT_NAME}Image PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/../..>)
target_include_directories (${PROJECT_NAME}Image PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>)
target_include_directories (${PROJECT_NAME}Image PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../error>)

############## next target ################
set (source_files
     common_image_common.h
     common_image_defines.h
#     common_image_exports.h
     common_image_tools.cpp
     common_image_tools.h)

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
target_sources (${PROJECT_NAME}Image
                PRIVATE stdafx.cpp
                PRIVATE stdafx.h
                ${absolute_source_files})

if (WIN32)
## set (CMAKE_GENERATOR_TOOLSET "LLVM-vs2014" CACHE STRING "Platform Toolset" FORCE)
# set (CMAKE_VS_PLATFORM_TOOLSET "LLVM-vs2014" CACHE STRING "Platform Toolset" FORCE)
 if (CMAKE_VS_PLATFORM_TOOLSET STREQUAL LLVM-vs2014)
  add_compile_options (-Wno-c++11-narrowing)
 endif ()
endif ()

if (UNIX)
 target_link_libraries (${PROJECT_NAME}Image ${PKG_FFMPEG_LIBRARIES})

 target_link_libraries (${PROJECT_NAME}Image ${ImageMagick_LIBRARIES})
elseif (WIN32)
 target_link_libraries (${PROJECT_NAME}Image $ENV{LIB_ROOT}/DXSDK/Lib/x86/d3dx9.lib)

# libpng
 if (LIBPNG_SUPPORT)
  target_link_libraries (${PROJECT_NAME}Image $ENV{LIB_ROOT}/libpng/cmake/${CMAKE_BUILD_TYPE}/libpng16${LIB_FILE_SUFFIX}.lib)
# target_link_libraries (${PROJECT_NAME}Image ${CMAKE_CURRENT_SOURCE_DIR}/../../../libpng-1.6.25/projects/vstudio/Debug/libpng16.lib)
 endif (LIBPNG_SUPPORT)

# ffmpeg
 target_link_libraries (${PROJECT_NAME}Image $ENV{LIB_ROOT}/ffmpeg/lib/avcodec.lib)
 target_link_libraries (${PROJECT_NAME}Image $ENV{LIB_ROOT}/ffmpeg/lib/avutil.lib)
 target_link_libraries (${PROJECT_NAME}Image $ENV{LIB_ROOT}/ffmpeg/lib/swscale.lib)
# target_link_libraries (${PROJECT_NAME}Image $ENV{LIB_ROOT}/ffmpeg/libavcodec/avcodec.lib)
# target_link_libraries (${PROJECT_NAME}Image $ENV{LIB_ROOT}/ffmpeg/libavutil/avutil.lib)

 if (IMAGEMAGICK_SUPPORT)
  target_link_libraries (${PROJECT_NAME}Image $ENV{LIB_ROOT}/ImageMagick/lib/CORE_RL_MagickCore_.lib)
  target_link_libraries (${PROJECT_NAME}Image $ENV{LIB_ROOT}/ImageMagick/lib/CORE_RL_MagickWand_.lib)
 endif (IMAGEMAGICK_SUPPORT)

 target_link_libraries (${PROJECT_NAME}Image ${ACE_LIBRARY})
# target_link_libraries (${PROJECT_NAME}Image ${COMMON_TARGET_NAME})
endif ()
#set_target_properties (${PROJECT_NAME}Image PROPERTIES DEFINE_SYMBOL COMMON_IMAGE_BUILD_DLL)
set_target_properties (${PROJECT_NAME}Image PROPERTIES
                       VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_MICRO}
                       SOVERSION 1)

# add PCH
set_target_properties (${PROJECT_NAME}Image PROPERTIES
                       COTIRE_CXX_PREFIX_HEADER_INIT "stdafx.h")
cotire (${PROJECT_NAME}Image)

# add all targets to the build-tree export set
export (TARGETS ${PROJECT_NAME}Image
        APPEND FILE "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake")

############### install files ################
install (FILES
         common_image_common.h
         common_image_defines.h
#         common_image_exports.h
         common_image_tools.h
         stdafx.h
         DESTINATION ${INSTALL_INCLUDE_DIR})
install (TARGETS ${PROJECT_NAME}Image
         EXPORT ${PROJECT_NAME}Targets
         ARCHIVE DESTINATION "${INSTALL_LIB_DIR}"
         COMPONENT lib)
