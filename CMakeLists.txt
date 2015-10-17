cmake_minimum_required(VERSION 2.8)
project (libCommon)

set (libCommon_VERSION_MAJOR 0)
set (libCommon_VERSION_MINOR 0)
set (libCommon_VERSION_MICRO 1)
set (libCommon_VERSION_DEVEL devel)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in.cmake
               ${CMAKE_CURRENT_BINARY_DIR}/libCommon_config.h
               @ONLY)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_subdirectory(src)

set (GCC_CXXFLAGS_DEBUG "")
option(DEBUG
       "enable debugging" ON)
if (DEBUG)
  set (GCC_CXXFLAGS_DEBUG "-g -O0 -fno-inline")
  set_property(GLOBAL PROPERTY COMPILE_DEFINITIONS _DEBUG)
else
  set (GCC_CXXFLAGS_DEBUG "-O3")
  set_property(GLOBAL PROPERTY COMPILE_DEFINITIONS ACE_NDEBUG)
endif

option(TRACING
       "enable tracing" OFF)
if (TRACING)
  set_property(GLOBAL PROPERTY COMPILE_DEFINITIONS COMMON_NTRACE=0)
else
  set_property(GLOBAL PROPERTY COMPILE_DEFINITIONS COMMON_NTRACE=1)
endif

option(VALGRIND_SUPPORT
       "enable valgrind support" OFF)
if (VALGRIND_SUPPORT)
  include (CheckIncludeFiles)
  check_include_files(valgrind/valgrind.h HAVE_VALGRIND_H)
#  include (CheckLibraryExists)
#  check_library_exists(libvalgrind "" "" HAVE_LIB_VALGRIND)
  include (FindPkgConfig)
  pkg_check_modules(PKGVALGRIND REQUIRED libvalgrind)
  set_property(GLOBAL PROPERTY COMPILE_DEFINITIONS LIBCOMMON_ENABLE_VALGRIND_SUPPORT)
endif

set (GCC_CXXFLAGS_BASE "-ansi -std=c++11 -Wall")
if (CMAKE_COMPILER_IS_GNUCC)
  set (CMAKE_CXX_FLAGS_DEBUG "${GCC_CXXFLAGS_BASE} ${GCC_CXXFLAGS_DEBUG}")
  set (CMAKE_CXX_FLAGS_RELEASE "${GCC_CXXFLAGS_BASE} ${GCC_CXXFLAGS_DEBUG}")
  set (CMAKE_CXX_FLAGS_RELWITHDEBINFO "${GCC_CXXFLAGS_BASE} -g ${GCC_CXXFLAGS_DEBUG}")
  set_property(GLOBAL PROPERTY COMPILE_DEFINITIONS _GNU_SOURCE)
endif

option(BUILD_MODULES
       "build (git) sub-modules" OFF)

# build a CPack driven installer package
include (InstallRequiredSystemLibraries)
set (CPACK_PACKAGE_VERSION_MAJOR ${libCommon_VERSION_MAJOR})
set (CPACK_PACKAGE_VERSION_MINOR ${libCommon_VERSION_MINOR})
set (CPACK_PACKAGE_VERSION_PATCH ${libCommon_VERSION_MICRO})
set (CPACK_RESOURCE_FILE_LICENSE ${CMAKE_CURRENT_SOURCE_DIR}/LICENSE)
set (CPACK_RESOURCE_FILE_README ${CMAKE_CURRENT_SOURCE_DIR}/README)
include (CPack)

