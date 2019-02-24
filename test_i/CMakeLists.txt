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

if (UNIX)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../modules/ACE)

# include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../modules/Common/src)
 #include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../modules/Common/src/timer)
# if (GTK_SUPPORT)
#  include_directories (${PKG_GTK_INCLUDE_DIRS})

#  include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../modules/Common/src/ui)
#  include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../modules/Common/src/ui/gtk)
# endif ()
elseif (WIN32)
 include_directories ($ENV{ACE_ROOT})

# if (GTK_SUPPORT)
#  include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../Common/src/ui)
#  include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../Common/src/ui/gtk)
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
add_dependencies (test_i gtk_ui)

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
