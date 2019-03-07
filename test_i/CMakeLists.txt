option (BUILD_TEST_I "build test_i" OFF)

if (UNIX)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../modules/ACE)
elseif (WIN32)
 include_directories ($ENV{ACE_ROOT})
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
if (GTK_SUPPORT)
 add_dependencies (test_i gtk_ui)
endif (GTK_SUPPORT)
if (WXWIDGETS_SUPPORT)
 add_dependencies (test_i wxwidgets)
 add_dependencies (test_i wxwidgets_2)
 add_dependencies (test_i wxwidgets_3)
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

