option (IMAGEMAGICK_SUPPORT "compile ImageMagick support" ON)
if (IMAGEMAGICK_SUPPORT)
 add_definitions (-DIMAGEMAGICK_SUPPORT)
endif (IMAGEMAGICK_SUPPORT)
option (LIBPNG_SUPPORT "compile libpng support" OFF)
if (LIBPNG_SUPPORT)
 add_definitions (-DLIBPNG_SUPPORT)
endif (LIBPNG_SUPPORT)

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
               PATHS /usr/lib/x86_64-linux-gnu
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
            HINTS /usr/include
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

add_library (${PROJECT_NAME}OpenGL STATIC "")

if (UNIX)
 target_include_directories (${PROJECT_NAME}OpenGL PUBLIC
                             $<BUILD_INTERFACE:${PKG_GLM_INCLUDE_DIRS}>)
 target_include_directories (${PROJECT_NAME}OpenGL PUBLIC
                             $<BUILD_INTERFACE:${PKG_LIBPNG_INCLUDE_DIRS}>)
 target_include_directories (${PROJECT_NAME}OpenGL PUBLIC
                             $<BUILD_INTERFACE:${PKG_ASSIMP_INCLUDE_DIRS}>)

 target_include_directories (${PROJECT_NAME}OpenGL PUBLIC
                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../modules/ACE>)
elseif (WIN32)
 if (LIBPNG_SUPPORT)
  target_include_directories (${PROJECT_NAME}OpenGL PUBLIC
                              $<BUILD_INTERFACE:$ENV{LIB_ROOT}/libpng/cmake>)
  target_include_directories (${PROJECT_NAME}OpenGL PUBLIC
                              $<BUILD_INTERFACE:$ENV{LIB_ROOT}/libpng>)
 endif (LIBPNG_SUPPORT)

 if (IMAGEMAGICK_SUPPORT)
  target_include_directories (${PROJECT_NAME}OpenGL PUBLIC
                              $<BUILD_INTERFACE:$ENV{LIB_ROOT}/ImageMagick/include>)
 endif (IMAGEMAGICK_SUPPORT)

 target_include_directories (${PROJECT_NAME}OpenGL PUBLIC
                             $<BUILD_INTERFACE:$ENV{LIB_ROOT}/glm>)

 target_include_directories (${PROJECT_NAME}OpenGL PUBLIC
                             $<BUILD_INTERFACE:$ENV{LIB_ROOT}/assimp/cmake/include>)
 target_include_directories (${PROJECT_NAME}OpenGL PUBLIC
                             $<BUILD_INTERFACE:$ENV{LIB_ROOT}/assimp/include>)

 if (DEFINED ENV{ACE_ROOT})
  target_include_directories (${PROJECT_NAME}OpenGL PUBLIC
                              $<BUILD_INTERFACE:$ENV{ACE_ROOT}>)
 else ()
  target_include_directories (${PROJECT_NAME}OpenGL PUBLIC
                              $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../ATCD/ACE>)
 endif (DEFINED ENV{ACE_ROOT})
endif ()
target_include_directories (${PROJECT_NAME}OpenGL PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>)
target_include_directories (${PROJECT_NAME}OpenGL PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../image>)
target_include_directories (${PROJECT_NAME}OpenGL PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/../..>)

############## next target ################
set (source_files
     common_gl_common.h
     common_gl_defines.h
#     common_gl_exports.h
     common_gl_tools.cpp
     common_gl_tools.h
     common_gl_assimp_tools.cpp
     common_gl_assimp_tools.h
     common_gl_image_tools.cpp
     common_gl_image_tools.h)

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
target_sources (${PROJECT_NAME}OpenGL
                PRIVATE stdafx.cpp
                PRIVATE stdafx.h
                ${absolute_source_files})

#set (LLVM_COMPILER_ID_STRING "Clang")
#string (TOLOWER "${LLVM_COMPILER_ID_STRING}" LLVM_COMPILER_ID_STRING_LOWER)
#string (TOLOWER "${CMAKE_CXX_COMPILER_ID}" CXX_COMPILER_ID_STRING_LOWER)
#if (CXX_COMPILER_ID_STRING_LOWER STREQUAL LLVM_COMPILER_ID_STRING_LOWER)
# add_compile_options (-fPIC)
# add_definitions (-D_GLIBCXX_USE_CXX11_ABI=0)
#endif ()

#add_definitions (-DHAVE_CONFIG_H)
#set_property (SOURCE PROPERTY COMPILE_DEFINITIONS HAVE_CONFIG_H)

#add_dependencies (${PROJECT_NAME} ACE)

#set_target_properties (${PROJECT_NAME} PROPERTIES COMPILE_DEFINITIONS ACE_HAS_DLL)
#set_target_properties (${PROJECT_NAME} PROPERTIES
#                       DEFINE_SYMBOL COMMON_BUILD_DLL)
set_target_properties (${PROJECT_NAME}OpenGL PROPERTIES
                       VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_MICRO}
                       SOVERSION 1)

if (UNIX)
 if (LIBPNG_SUPPORT)
  target_link_libraries (${PROJECT_NAME}OpenGL ${PNG_LIBRARY})
 endif (LIBPNG_SUPPORT)
 target_link_libraries (${PROJECT_NAME}OpenGL ${PKG_ASSIMP_LIBRARIES})
elseif (WIN32)
 if (LIBPNG_SUPPORT)
  target_link_libraries (${PROJECT_NAME}OpenGL $ENV{LIB_ROOT}/libpng/cmake/${CMAKE_BUILD_TYPE}/libpng16${LIB_FILE_SUFFIX}.lib)
 endif (LIBPNG_SUPPORT)

 if (IMAGEMAGICK_SUPPORT)
  target_link_libraries (${PROJECT_NAME}OpenGL $ENV{LIB_ROOT}/ImageMagick/lib/CORE_RL_MagickCore_.lib)
  target_link_libraries (${PROJECT_NAME}OpenGL $ENV{LIB_ROOT}/ImageMagick/lib/CORE_RL_MagickWand_.lib)
 endif (IMAGEMAGICK_SUPPORT)
endif ()
if (OPENGL_FOUND)
 target_link_libraries (${PROJECT_NAME}OpenGL ${OPENGL_LIBRARIES})
else ()
 target_link_libraries (${PROJECT_NAME}OpenGL ${OPENGL_LIBRARY})
endif (OPENGL_FOUND)
target_link_libraries (${PROJECT_NAME}OpenGL ${PROJECT_NAME}Image)

# add PCH
set_target_properties (${PROJECT_NAME}OpenGL PROPERTIES
                       COTIRE_CXX_PREFIX_HEADER_INIT "stdafx.h")
cotire (${PROJECT_NAME}OpenGL)

# add all targets to the build-tree export set
export (TARGETS ${PROJECT_NAME}OpenGL
        APPEND FILE "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake")

############### install files ################
install (FILES
         common_gl_common.h
         common_gl_defines.h
#         common_gl_exports.h
         common_gl_tools.h
         common_gl_assimp_tools.h
         DESTINATION ${INSTALL_INCLUDE_DIR})
install (TARGETS ${PROJECT_NAME}OpenGL
         EXPORT ${PROJECT_NAME}Targets
         ARCHIVE DESTINATION "${INSTALL_LIB_DIR}"
         COMPONENT lib)
