option (BUILD_TEST_I "build test_i" OFF)
#option (GTK_SUPPORT "enable GTK support" ON)
# option (GTK3_SUPPORT "enable Gtk3 support" ON)
 # *NOTE*: implies DHCLIENT_SUPPORT off
#include (CMakeDependentOption)
#CMAKE_DEPENDENT_OPTION (GTK3_SUPPORT "enable GTK 3 support" ON
#                        "GTK_SUPPORT" OFF)
#CMAKE_DEPENDENT_OPTION (GTK2_SUPPORT "enable GTK 2 support" ON
#                        "GTK_SUPPORT; NOT GTK3_SUPPORT" OFF)

#set (${PROJECT_NAME}_Test_U_SRCS
#     test_u_common.h
#     test_u_common_modules.h
#     test_u_defines.h)
#install (FILES
#         ${PROJECT_NAME}_Test_U_SRCS
#         DESTINATION include)

#if (GTK_SUPPORT)
# set (${PROJECT_NAME}_Test_U_GTK_SRCS
#      test_u_gtk_common.h
#      test_u_gtk_eventhandler.cpp
#      test_u_gtk_eventhandler.h
#      test_u_gtk_eventhandler.inl)
#endif ()
#install (FILES
#         ${PROJECT_NAME}_Test_U_GTK_SRCS
#         DESTINATION include)

include (FindPkgConfig)
if (UNIX)
# if (GTK_SUPPORT)
#  if (GTK2_SUPPORT)
#   pkg_check_modules (PKG_GTK REQUIRED gmodule-2.0 gthread-2.0 gtk+-2.0)
#  elseif (GTK3_SUPPORT)
#   pkg_check_modules (PKG_GTK REQUIRED gtk+-3.0)
#  endif ()
# endif ()
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../modules/ACE)

 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../modules/libCommon/src)
 #include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../modules/libCommon/src/timer)
# if (GTK_SUPPORT)
#  include_directories (${PKG_GTK_INCLUDE_DIRS})

#  include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../modules/libCommon/src/ui)
#  include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../modules/libCommon/src/ui/gtk)
# endif ()
elseif (WIN32)
# gtk
# if (GTK_SUPPORT)
#  if (GTK2_SUPPORT)
#   include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../gtk2/include/atk-1.0)
#   include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../gtk2/include/gdk-pixbuf-2.0)
#   include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../gtk2/include/cairo)
#   include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../gtk2/include/pango-1.0)
#   include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../gtk2/lib/glib-2.0/include)
#   include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../gtk2/include/glib-2.0)
#   include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../gtk2/lib/gtk-2.0/include)
#   include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../gtk2/include/gtk-2.0)
#  elseif (GTK3_SUPPORT)
#   include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../gtk/gtk/Win32/debug/include/atk-1.0)
#   include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../gtk/gtk/Win32/debug/include/gdk-pixbuf-2.0)
#   include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../gtk/gtk/Win32/debug/include/cairo)
#   include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../gtk/gtk/Win32/debug/include/pango-1.0)
#   include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../gtk/gtk/Win32/debug/lib/glib-2.0/include)
#   include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../gtk/gtk/Win32/debug/include/glib-2.0)
#   include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../gtk/gtk/Win32/debug/include/gtk-3.0)
#  endif ()
# endif ()

 include_directories ($ENV{ACE_ROOT})

# if (GTK_SUPPORT)
#  include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../libCommon/src/ui)
#  include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../libCommon/src/ui/gtk)
# endif ()
endif ()
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../src)
# include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../src/timer)

#if (GTK_SUPPORT)
# add_library (${PROJECT_NAME}_Test_U STATIC
#              ${ACEStream_Test_U_SRCS}
#              ${ACEStream_Test_U_GTK_SRCS})
#else ()
# add_library (${PROJECT_NAME}_Test_U STATIC
#              ${ACEStream_Test_U_SRCS})
#endif ()

# ----------------------------------------

if (UNIX)
 add_subdirectory (capabilities)
endif ()

# phonies
add_custom_target (test_i ALL
                   COMMENT "building test_i...")
if (UNIX)
 add_dependencies (test_i capability_wrapper)
endif ()

if (NOT BUILD_TEST_I)
 set_target_properties (test_i PROPERTIES EXCLUDE_FROM_ALL 1)
 cmake_policy (SET CMP0012 NEW)
 if (WIN32)
  set_target_properties (test_i PROPERTIES EXCLUDE_FROM_DEFAULT_BUILD TRUE)
 endif ()
endif ()

# ----------------------------------------

#include (CreateLaunchers)

# enable (unit/integration) tests
#include (CTest)
enable_testing ()

