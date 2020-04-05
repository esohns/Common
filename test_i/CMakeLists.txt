include (${CMAKE_CURRENT_SOURCE_DIR}/../scripts/Threads.cmake)

include (${CMAKE_CURRENT_SOURCE_DIR}/../scripts/ACE.cmake)

option (BUILD_TEST_I "build test_i" OFF)

if (UNIX)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../modules/ACE)
elseif (WIN32)
 if (DEFINED ENV{ACE_ROOT})
  file (TO_CMAKE_PATH $ENV{ACE_ROOT} ACE_ROOT_CMAKE)
  include_directories (${ACE_ROOT_CMAKE})
 else ()
  include_directories ($ENV{LIB_ROOT}/ACE_TAO/ACE)
 endif (DEFINED ENV{ACE_ROOT})
endif ()
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../src)

# ----------------------------------------

if (UNIX)
 add_subdirectory (capabilities)
endif ()
add_subdirectory (ffmpeg)
if (UNIX)
 add_subdirectory (imagemagick)
endif ()
add_subdirectory (parser)
if (UNIX)
 add_subdirectory (print)
endif ()
add_subdirectory (ui)

# phonies
add_custom_target (test_i ALL
                   COMMENT "building test_i...")
if (UNIX)
 add_dependencies (test_i capability_wrapper)
endif ()
add_dependencies (test_i ffmpeg)
if (UNIX)
 add_dependencies (test_i imagemagick)
endif ()
add_dependencies (test_i parser)
if (UNIX)
 add_dependencies (test_i print)
endif ()
if (GTK_SUPPORT)
 add_dependencies (test_i gtk_ui gtk_ui_2 gtk_ui_3)
endif (GTK_SUPPORT)
if (GTK_SUPPORT)
 add_dependencies (test_i qt_ui qt_ui_2)
endif (GTK_SUPPORT)
if (WXWIDGETS_SUPPORT)
 add_dependencies (test_i wxwidgets wxwidgets_2 wxwidgets_3)
endif (WXWIDGETS_SUPPORT)

if (NOT BUILD_TEST_I)
 set_target_properties (test_i PROPERTIES EXCLUDE_FROM_ALL 1)
 cmake_policy (SET CMP0012 NEW)
 if (WIN32)
  set_target_properties (test_i PROPERTIES EXCLUDE_FROM_DEFAULT_BUILD TRUE)
 endif ()
endif ()

# ----------------------------------------

# enable (unit/integration) tests
#include (CTest)
enable_testing ()

