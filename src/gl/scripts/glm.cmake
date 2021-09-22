if (UNIX)
# include (FindPkgConfig)
# pkg_check_modules (PKG_GLM glm)
 find_file (GLM_HPP_FILE
            NAMES "glm.hpp"
            PATHS /usr/include
            PATH_SUFFIXES glm
            DOC "searching for glm.h"
            NO_DEFAULT_PATH)
elseif (WIN32)
 find_file (GLM_HPP_FILE
            NAMES "glm.hpp"
            PATHS $ENV{LIB_ROOT}/glm
            PATH_SUFFIXES glm
            DOC "searching for glm.h"
            NO_DEFAULT_PATH)
endif ()
if (NOT GLM_HPP_FILE)
 message (WARNING "could not find GLM header, continuing")
else ()
 message (STATUS "Found GLM header \"${GLM_HPP_FILE}\"")
 option (GLM_SUPPORT "enable glm support" ON)
 if (GLM_SUPPORT)
  get_filename_component (GLM_HPP_PATH ${GLM_HPP_FILE} REALPATH)
  get_filename_component (GLM_HPP_PATH ${GLM_HPP_PATH} DIRECTORY)
  get_filename_component (GLM_HPP_PATH ${GLM_HPP_PATH} DIRECTORY)
  add_definitions (-DGLM_SUPPORT)
 endif (GLM_SUPPORT)
endif (NOT GLM_HPP_FILE)
