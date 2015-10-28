cmake_minimum_required (VERSION 2.8)

# options
set (CMAKE_VERBOSE_MAKEFILE ON)

set (VERSION_MAJOR 0)
set (VERSION_MINOR 0)
set (VERSION_MICRO 1)
set (VERSION_DEVEL devel)

project (libCommon
         #VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_MICRO}-${VERSION_DEVEL}
         LANGUAGES CXX)

set (PACKAGE ${PROJECT_NAME})

configure_file (${CMAKE_CURRENT_SOURCE_DIR}/config.h.in.cmake
                ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}_config.h
                @ONLY)

add_subdirectory (src)

set (GCC_CXXFLAGS_DEBUG "")
option (DEBUG
        "enable debugging" ON)
if (DEBUG)
  set (GCC_CXXFLAGS_DEBUG "-g -O0 -fno-inline")
  add_definitions (-D_DEBUG)
#  set_property (SOURCE PROPERTY COMPILE_DEFINITIONS _DEBUG)
else ()
  set (GCC_CXXFLAGS_DEBUG "-O3")
  add_definitions (-DACE_NDEBUG)
#  set_property (SOURCE PROPERTY COMPILE_DEFINITIONS ACE_NDEBUG)
endif ()

option (TRACING
        "enable tracing" OFF)
if (TRACING)
  add_definitions (-DCOMMON_NTRACE=0)
#  set_property (SOURCE PROPERTY COMPILE_DEFINITIONS COMMON_NTRACE=0)
else ()
  add_definitions (-DCOMMON_NTRACE=1)
#  set_property (SOURCE PROPERTY COMPILE_DEFINITIONS COMMON_NTRACE=1)
endif ()

option (VALGRIND_SUPPORT
        "enable valgrind support" OFF)
if (VALGRIND_SUPPORT)
  include (CheckIncludeFiles)
  check_include_files (valgrind/valgrind.h HAVE_VALGRIND_H)
#  include (CheckLibraryExists)
#  check_library_exists (libvalgrind "" "" HAVE_LIB_VALGRIND)
  include (FindPkgConfig)
  pkg_check_modules (PKGVALGRIND REQUIRED libvalgrind)
  add_definitions (-DLIBCOMMON_ENABLE_VALGRIND_SUPPORT)
#  set_property (SOURCE PROPERTY COMPILE_DEFINITIONS LIBCOMMON_ENABLE_VALGRIND_SUPPORT)
endif ()

set (GCC_CXXFLAGS_BASE "-ansi -std=c++11 -Wall")
if (CMAKE_COMPILER_IS_GNUCC)
  set (CMAKE_CXX_FLAGS_DEBUG "${GCC_CXXFLAGS_BASE} ${GCC_CXXFLAGS_DEBUG}")
  set (CMAKE_CXX_FLAGS_RELEASE "${GCC_CXXFLAGS_BASE} ${GCC_CXXFLAGS_DEBUG}")
  set (CMAKE_CXX_FLAGS_RELWITHDEBINFO "${GCC_CXXFLAGS_BASE} -g ${GCC_CXXFLAGS_DEBUG}")
  add_definitions (-D_GNU_SOURCE)
#  set_property (SOURCE PROPERTY COMPILE_DEFINITIONS _GNU_SOURCE)
endif ()

option (BUILD_MODULES
        "build (git) sub-modules" OFF)

# build a CPack driven installer package
include (InstallRequiredSystemLibraries)
set (CPACK_PACKAGE_VERSION_MAJOR ${VERSION_MAJOR})
set (CPACK_PACKAGE_VERSION_MINOR ${VERSION_MINOR})
set (CPACK_PACKAGE_VERSION_PATCH ${VERSION_MICRO})
set (CPACK_RESOURCE_FILE_LICENSE ${CMAKE_CURRENT_SOURCE_DIR}/LICENSE)
set (CPACK_RESOURCE_FILE_README ${CMAKE_CURRENT_SOURCE_DIR}/README)

include (CPack)

