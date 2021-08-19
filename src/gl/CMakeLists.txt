include (OS)

option (LIBPNG_SUPPORT "compile libpng support" OFF)
if (LIBPNG_SUPPORT)
 add_definitions (-DLIBPNG_SUPPORT)
endif (LIBPNG_SUPPORT)

add_definitions (-DFFMPEG_SUPPORT)

option (IMAGEMAGICK_SUPPORT "compile ImageMagick support" ON)
if (IMAGEMAGICK_SUPPORT)
 add_definitions (-DIMAGEMAGICK_SUPPORT)
 if (UNIX)
  pkg_check_modules (PKG_IMAGEMAGICK REQUIRED ImageMagick)
 endif (UNIX)
endif (IMAGEMAGICK_SUPPORT)

if (${CMAKE_VERSION} VERSION_GREATER "3.10")
 cmake_policy (SET CMP0072 OLD) # set OpenGL_GL_PREFERENCE to LEGACY|GLVND
endif (${CMAKE_VERSION} VERSION_GREATER "3.10")
include (FindOpenGL)
if (NOT OPENGL_FOUND)
 if (UNIX)
  set (OPENGL_LIB_FILE libGL.so)
 elseif (WIN32)
  set (OPENGL_LIB_FILE OpenGL32.lib)
 endif ()
# find_package (OPENGL REQUIRED)
 find_library (OPENGL_LIBRARY
               NAMES ${OPENGL_LIB_FILE}
               PATHS /usr/lib64 /usr/lib/x86_64-linux-gnu
               DOC "searching for ${OPENGL_LIB_FILE}")
 if (NOT OPENGL_FOUND AND NOT OPENGL_LIBRARY)
  message (FATAL_ERROR "could not find ${OPENGL_LIB_FILE} (was: \"${OPENGL_LIBRARY}\"), aborting")
 else ()
  message (STATUS "Found OpenGL library \"${OPENGL_LIBRARY}\"")
 endif (NOT OPENGL_FOUND AND NOT OPENGL_LIBRARY)
endif (NOT OPENGL_FOUND)

if (UNIX)
# GLM
# pkg_check_modules (PKG_GLM REQUIRED glm)
 find_file (GLM_HPP_FILE
            NAMES "glm.hpp"
            PATHS /usr/include
            PATH_SUFFIXES glm
            DOC "searching for glm.h"
            NO_DEFAULT_PATH)
 if (NOT GLM_HPP_FILE)
  message (FATAL_ERROR "Could not find GLM header, aborting")
 else ()
  #message (STATUS "Found GLM header \"${GLM_HPP_FILE}\"")
 endif ()

# assimp
 if (LIBPNG_SUPPORT)
  find_library (PNG_LIBRARY libpng16.so.16
                DOC "searching for libpng")
  if (NOT PNG_LIBRARY)
   message (FATAL_ERROR "could not find libpng, aborting")
  endif (NOT PNG_LIBRARY)
 endif (LIBPNG_SUPPORT)
# pkg_check_modules (PKG_LIBPNG REQUIRED libpng)

# find_library (ASSIMP_LIBRARY libassimp.so.3
#               DOC "searching for libassimp")
# if (NOT ASSIMP_LIBRARY)
#  message (FATAL_ERROR "could not find libassimp, aborting")
# endif (NOT ASSIMP_LIBRARY)
 pkg_check_modules (PKG_ASSIMP REQUIRED assimp)
endif (UNIX)

set (LIBRARY ${PROJECT_NAME}OpenGL)
add_library (${LIBRARY} STATIC "")

if (UNIX)
 target_include_directories (${LIBRARY} PUBLIC
                             $<BUILD_INTERFACE:${PKG_GLM_INCLUDE_DIRS}>)
 if (LIBPNG_SUPPORT)
  target_include_directories (${LIBRARY} PUBLIC
                              $<BUILD_INTERFACE:${PKG_LIBPNG_INCLUDE_DIRS}>)
 endif (LIBPNG_SUPPORT)
# *WORKAROUND*: there seems to be a problem on Ubuntu
 if (CMAKE_SYSTEM_NAME MATCHES "Linux")
   message (STATUS "on linux")
   message (STATUS "LSB_RELEASE_ID_SHORT: ${LSB_RELEASE_ID_SHORT}")
  if (${LSB_RELEASE_ID_SHORT} MATCHES "Ubuntu")
   message (STATUS "on ubuntu")
   target_include_directories (${LIBRARY} PUBLIC
                               $<BUILD_INTERFACE:/usr/include/assimp>)
  else ()
   target_include_directories (${LIBRARY} PUBLIC
                               $<BUILD_INTERFACE:${PKG_ASSIMP_INCLUDE_DIRS}>)
  endif (${LSB_RELEASE_ID_SHORT} MATCHES "Ubuntu")
 else ()
  target_include_directories (${LIBRARY} PUBLIC
                              $<BUILD_INTERFACE:${PKG_ASSIMP_INCLUDE_DIRS}>)
 endif (CMAKE_SYSTEM_NAME MATCHES "Linux")
 if (IMAGEMAGICK_SUPPORT)
  if (CMAKE_SYSTEM_NAME MATCHES "Linux")
   message (STATUS "PKG_IMAGEMAGICK_INCLUDE_DIRS: ${PKG_IMAGEMAGICK_INCLUDE_DIRS}")
   if (${LSB_RELEASE_ID_SHORT} MATCHES "Ubuntu")
    target_include_directories (${LIBRARY} PUBLIC
                                $<BUILD_INTERFACE:/usr/include/x86_64-linux-gnu/ImageMagick-6;/usr/include/ImageMagick-6>)
   else ()
    target_include_directories (${LIBRARY} PUBLIC
                                $<BUILD_INTERFACE:${PKG_IMAGEMAGICK_INCLUDE_DIRS}>)
   endif (${LSB_RELEASE_ID_SHORT} MATCHES "Ubuntu")
  endif (CMAKE_SYSTEM_NAME MATCHES "Linux")
 endif (IMAGEMAGICK_SUPPORT)
elseif (WIN32)
 if (LIBPNG_SUPPORT)
  target_include_directories (${LIBRARY} PUBLIC
                              $<BUILD_INTERFACE:$ENV{LIB_ROOT}/libpng/cmake>)
  target_include_directories (${LIBRARY} PUBLIC
                              $<BUILD_INTERFACE:$ENV{LIB_ROOT}/libpng>)
 endif (LIBPNG_SUPPORT)

 if (IMAGEMAGICK_SUPPORT)
  target_include_directories (${LIBRARY} PUBLIC
                              $<BUILD_INTERFACE:$ENV{LIB_ROOT}/ImageMagick/include>)
 endif (IMAGEMAGICK_SUPPORT)

 target_include_directories (${LIBRARY} PUBLIC
                             $<BUILD_INTERFACE:$ENV{LIB_ROOT}/glm>)

 target_include_directories (${LIBRARY} PUBLIC
                             $<BUILD_INTERFACE:$ENV{LIB_ROOT}/assimp/cmake/include>)
 target_include_directories (${LIBRARY} PUBLIC
                             $<BUILD_INTERFACE:$ENV{LIB_ROOT}/assimp/include>)
endif ()

target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/../..>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../image>)

get_target_property (debug ${LIBRARY} INTERFACE_INCLUDE_DIRECTORIES)
message (STATUS "INTERFACE_INCLUDE_DIRECTORIES: \"${debug}\"")

############## next target ################
set (source_files
     common_gl_common.h
     common_gl_defines.h
     common_gl_tools.cpp
     common_gl_tools.h
     common_gl_assimp_tools.cpp
     common_gl_assimp_tools.h
     common_gl_image_tools.cpp
     common_gl_image_tools.h
     common_gl_texture.cpp
     common_gl_texture.h)

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

#add_definitions (-DHAVE_CONFIG_H)
#set_property (SOURCE PROPERTY COMPILE_DEFINITIONS HAVE_CONFIG_H)

#add_dependencies (${PROJECT_NAME} ACE)

#set_target_properties (${PROJECT_NAME} PROPERTIES COMPILE_DEFINITIONS ACE_HAS_DLL)
#set_target_properties (${PROJECT_NAME} PROPERTIES
#                       DEFINE_SYMBOL COMMON_BUILD_DLL)
#set_target_properties (${LIBRARY} PROPERTIES
#                       VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_MICRO}
#                       SOVERSION 1)

if (UNIX)
 if (LIBPNG_SUPPORT)
  target_link_libraries (${LIBRARY} ${PNG_LIBRARY})
 endif (LIBPNG_SUPPORT)
 target_link_libraries (${LIBRARY} ${PKG_ASSIMP_LIBRARIES})
elseif (WIN32)
 if (LIBPNG_SUPPORT)
  target_link_libraries (${LIBRARY} $ENV{LIB_ROOT}/libpng/cmake/${CMAKE_BUILD_TYPE}/libpng16${LIB_FILE_SUFFIX}.lib)
 endif (LIBPNG_SUPPORT)

 target_link_libraries (${LIBRARY} $ENV{LIB_ROOT}/assimp/cmake/code/${CMAKE_BUILD_TYPE}/assimp-vc140-mt.lib)

 if (IMAGEMAGICK_SUPPORT)
  target_link_libraries (${LIBRARY} $ENV{LIB_ROOT}/ImageMagick/lib/CORE_RL_MagickCore_.lib)
  target_link_libraries (${LIBRARY} $ENV{LIB_ROOT}/ImageMagick/lib/CORE_RL_MagickWand_.lib)
 endif (IMAGEMAGICK_SUPPORT)
endif ()
if (OPENGL_FOUND)
 target_link_libraries (${LIBRARY} ${OPENGL_LIBRARIES})
else ()
 target_link_libraries (${LIBRARY} ${OPENGL_LIBRARY})
endif (OPENGL_FOUND)
target_link_libraries (${LIBRARY} ${PROJECT_NAME}Image)

# add PCH
set_target_properties (${LIBRARY} PROPERTIES
                       COTIRE_CXX_PREFIX_HEADER_INIT "stdafx.h")
cotire (${LIBRARY})

# add all targets to the build-tree export set
export (TARGETS ${LIBRARY}
        APPEND FILE "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake")

############### install files ################
install (FILES
         common_gl_common.h
         common_gl_defines.h
         common_gl_tools.h
         common_gl_assimp_tools.h
         common_gl_texture.h
         DESTINATION ${INSTALL_INCLUDE_DIR})
#install (TARGETS ${LIBRARY}
#         EXPORT ${PROJECT_NAME}Targets
#         ARCHIVE DESTINATION "${INSTALL_LIB_DIR}"
#         COMPONENT lib)
