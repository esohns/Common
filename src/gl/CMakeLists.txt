include (FindOpenGL)
if (NOT OPENGL_FOUND)
 message (FATAL_ERROR "OpenGL support not found on this system, aborting")
endif ()

include (FindPkgConfig)
if (UNIX)
# pkg_check_modules (PKG_GLM REQUIRED glm)
 find_file (GLM_HPP_FILE
            NAMES "glm.hpp"
            HINTS /usr/include
            PATHS /usr/include
            PATH_SUFFIXES glm
            DOC "searching for glm.h"
            NO_DEFAULT_PATH)
 if (NOT GLM_HPP_FILE)
  message (FATAL_ERROR "Could not find GLM header (was: \"${GLM_HPP_FILE}\"), aborting")
 else ()
  #message (STATUS "Found GLM header \"${GLM_HPP_FILE}\"")
 endif ()
 pkg_check_modules (PKG_LIBPNG REQUIRED libpng)
 pkg_check_modules (PKG_ASSIMP REQUIRED assimp)
endif ()
#pkg_check_modules (PKG_ACE REQUIRED ACE)

set (ACE_LIB_FILE libACE.so)
if (UNIX)
 set (AIO_LIB_FILE libaio.so)
 find_library (AIO_LIBRARY ${AIO_LIB_FILE}
               PATHS /lib /usr/lib
               PATH_SUFFIXES x86_64-linux-gnu
               DOC "searching for ${AIO_LIB_FILE}")
# message ("CMAKE_FIND_ROOT_PATH is ${CMAKE_FIND_ROOT_PATH}")
 find_library (ACE_LIBRARY ${ACE_LIB_FILE}
               HINTS ${CMAKE_CURRENT_BINARY_DIR}/../../../ATCD/ACE /usr/local
               PATHS ENV ACE_ROOT
               PATH_SUFFIXES lib
               DOC "searching for ${ACE_LIB_FILE}"
               NO_DEFAULT_PATH)
# else ()
#  find_library (ACE_LIBRARY ${ACE_LIB_FILE}
#                DOC "searching for ${ACE_LIB_FILE} (system paths)")
# endif ()
elseif (WIN32)
 if (CMAKE_BUILD_TYPE STREQUAL Debug)
  set (LIB_FILE_SUFFIX d)
 endif ()
 unset (ACE_LIB_FILE)
 set (ACE_LIB_FILE ACE${LIB_FILE_SUFFIX}.lib)
 find_library (ACE_LIBRARY ${ACE_LIB_FILE}
               PATHS ENV ACE_ROOT
               PATH_SUFFIXES lib lib\\${CMAKE_BUILD_TYPE}\\Win32
               DOC "searching for ${ACE_LIB_FILE}")
endif ()
#if (NOT EXISTS ACE_LIBRARY)
if (NOT ACE_LIBRARY)
 message (FATAL_ERROR "Could not find ACE library (was: \"${ACE_LIB_FILE}\"), aborting")
else ()
 #message (STATUS "Found ACE library \"${ACE_LIBRARY}\"")
endif ()
add_definitions (-DACE_HAS_DLL)

if (UNIX)
 include_directories (${PKG_LIBPNG_INCLUDE_DIRS})
 include_directories (${PKG_ASSIMP_INCLUDE_DIRS})
 include_directories (${PKG_GLM_INCLUDE_DIRS})

 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../modules/ACE)
elseif (WIN32)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../assimp/include)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../glm)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../libpng-1.6.25)

 include_directories ($ENV{ACE_ROOT})

 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../3rd_party)
endif ()
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/..)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../image)
include_directories (${CMAKE_CURRENT_BINARY_DIR}/..)

############## next target ################
set (${PROJECT_NAME}OpenGL_LIB_SRCS
     stdafx.cpp
     stdafx.h
     common_gl_common.h
     common_gl_defines.h
     common_gl_exports.h
     common_gl_tools.cpp
     common_gl_tools.h)

#set (LLVM_COMPILER_ID_STRING "Clang")
#string (TOLOWER "${LLVM_COMPILER_ID_STRING}" LLVM_COMPILER_ID_STRING_LOWER)
#string (TOLOWER "${CMAKE_CXX_COMPILER_ID}" CXX_COMPILER_ID_STRING_LOWER)
#if (CXX_COMPILER_ID_STRING_LOWER STREQUAL LLVM_COMPILER_ID_STRING_LOWER)
# add_compile_options (-fPIC)
# add_definitions (-D_GLIBCXX_USE_CXX11_ABI=0)
#endif ()

add_definitions (-DHAVE_CONFIG_H)
#set_property (SOURCE PROPERTY COMPILE_DEFINITIONS HAVE_CONFIG_H)

add_library (${PROJECT_NAME}OpenGL STATIC
             ${CommonOpenGL_LIB_SRCS})

#add_dependencies (${PROJECT_NAME} ACE)

#set_target_properties (${PROJECT_NAME} PROPERTIES COMPILE_DEFINITIONS ACE_HAS_DLL)
#set_target_properties (${PROJECT_NAME} PROPERTIES
#                       DEFINE_SYMBOL COMMON_BUILD_DLL)
set_target_properties (${PROJECT_NAME}OpenGL PROPERTIES
                       VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_MICRO}
                       SOVERSION 1)

if (UNIX)
 target_link_libraries (${PROJECT_NAME}OpenGL ${PKG_ASSIMP_LIBRARIES})
elseif (WIN32)
 if (CMAKE_BUILD_TYPE STREQUAL Debug)
#  target_link_libraries (${PROJECT_NAME}OpenGL Msvcrtd.lib)
 else ()
#  target_link_libraries (${PROJECT_NAME}OpenGL Msvcrt.lib)
 endif ()
# target_link_libraries (${PROJECT_NAME}OpenGL Quartz.lib)
# target_link_libraries (${PROJECT_NAME}OpenGL Secur32.lib)
# target_link_libraries (${PROJECT_NAME}OpenGL Userenv.lib)

# target_link_libraries (${PROJECT_NAME}OpenGL ${ACE_LIBRARY})

# target_link_libraries (${PROJECT_NAME}OpenGL ${PROJECT_NAME}Error)
endif ()

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
         common_gl_exports.h
         common_gl_tools.h
         DESTINATION ${INSTALL_INCLUDE_DIR})
install (TARGETS ${PROJECT_NAME}OpenGL
         EXPORT ${PROJECT_NAME}Targets
         ARCHIVE DESTINATION "${INSTALL_LIB_DIR}"
         COMPONENT lib)
