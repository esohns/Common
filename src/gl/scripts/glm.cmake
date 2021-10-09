if (UNIX)
# include (FindPkgConfig)
# pkg_check_modules (PKG_GLM glm)
 find_file (GLM_HPP_FILE
            NAMES "glm.hpp"
            PATHS /usr/include
            PATH_SUFFIXES glm
            DOC "searching for glm.h"
            NO_DEFAULT_PATH)
 get_filename_component (GLM_HPP_PATH ${GLM_HPP_FILE} REALPATH)
 get_filename_component (GLM_HPP_PATH ${GLM_HPP_PATH} DIRECTORY)
 get_filename_component (GLM_HPP_PATH ${GLM_HPP_PATH} DIRECTORY)
elseif (WIN32)
 if (VCPKG_SUPPORT)
#  cmake_policy (SET CMP0074 OLD)
  find_package (glm CONFIG)
  if (glm_FOUND)
   message (STATUS "found glm")
   set (GLM_FOUND TRUE)
   set (GLM_INCLUDE_DIRS "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include")
  endif (glm_FOUND)
 endif (VCPKG_SUPPORT)
 if (NOT GLM_FOUND)
  find_file (GLM_HPP_FILE
             NAMES "glm.hpp"
             PATHS $ENV{LIB_ROOT}/glm
             PATH_SUFFIXES glm
             DOC "searching for glm.h"
             NO_DEFAULT_PATH)
  get_filename_component (GLM_HPP_PATH ${GLM_HPP_FILE} REALPATH)
  get_filename_component (GLM_HPP_PATH ${GLM_HPP_PATH} DIRECTORY)
  get_filename_component (GLM_HPP_PATH ${GLM_HPP_PATH} DIRECTORY)
 endif (NOT GLM_FOUND)
endif ()
if (NOT GLM_HPP_FILE)
 message (WARNING "could not find GLM header, continuing")
else ()
 message (STATUS "Found GLM header \"${GLM_HPP_FILE}\"")
 option (GLM_SUPPORT "enable glm support" ON)
 if (GLM_SUPPORT)
  add_definitions (-DGLM_SUPPORT)
 endif (GLM_SUPPORT)
endif (NOT GLM_HPP_FILE)
