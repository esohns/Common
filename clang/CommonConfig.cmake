# CommonConfig.cmake.in
#  Common_INCLUDE_DIRS - include directories for libCommon
#  Common_LIBRARIES    - libraries to link against

# Compute paths
get_filename_component (Common_CMAKE_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)
#message (STATUS "Common_CMAKE_DIR: ${Common_CMAKE_DIR}")
set (Common_INCLUDE_DIRS "/mnt/win_d/projects/Common;/mnt/win_d/projects/Common/clang")

# library dependencies (contains definitions for IMPORTED targets)
if (NOT TARGET Common AND NOT Common_BINARY_DIR)
include ("${Common_CMAKE_DIR}/CommonTargets.cmake")
endif ()

# These are IMPORTED targets created by CommonTargets.cmake
set (Common_LIBRARIES Common)
