include (${CMAKE_CURRENT_SOURCE_DIR}/../scripts/Threads.cmake)

include (${CMAKE_CURRENT_SOURCE_DIR}/../scripts/ffmpeg.cmake)

include (${CMAKE_CURRENT_SOURCE_DIR}/../src/ui/scripts/PreUIFramework.cmake)
include (${CMAKE_CURRENT_SOURCE_DIR}/../src/ui/scripts/SetUIFramework.cmake)

include (${CMAKE_CURRENT_SOURCE_DIR}/../scripts/ACE.cmake)

option (BUILD_TEST_I "build test_i" OFF)

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../src)

# ----------------------------------------

if (UNIX)
 add_subdirectory (capabilities)
endif (UNIX)
if (FFMPEG_SUPPORT)
 add_subdirectory (ffmpeg)
endif (FFMPEG_SUPPORT)
if (IMAGEMAGICK_SUPPORT)
 add_subdirectory (imagemagick)
endif (IMAGEMAGICK_SUPPORT)
add_subdirectory (parser)
if (UNIX)
 add_subdirectory (print)
endif (UNIX)
add_subdirectory (ui)

# phonies
add_custom_target (test_i ALL
                   COMMENT "building test_i...")
if (UNIX)
 add_dependencies (test_i capability_wrapper)
endif ()
if (FFMPEG_SUPPORT)
 add_dependencies (test_i ffmpeg)
endif (FFMPEG_SUPPORT)
if (IMAGEMAGICK_SUPPORT)
 add_dependencies (test_i imagemagick)
endif (IMAGEMAGICK_SUPPORT)
add_dependencies (test_i parser parser_2)
if (UNIX)
 add_dependencies (test_i print)
endif ()
if (GTK_SUPPORT)
 add_dependencies (test_i gtk_ui gtk_ui_2 gtk_ui_3)
endif (GTK_SUPPORT)
if (QT_SUPPORT)
 add_dependencies (test_i qt_ui qt_ui_2)
endif (QT_SUPPORT)
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
