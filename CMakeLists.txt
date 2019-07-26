include (${CMAKE_CURRENT_SOURCE_DIR}/scripts/PreToolChain.cmake)
include (${CMAKE_CURRENT_SOURCE_DIR}/scripts/SetToolChain.cmake)
include (${CMAKE_CURRENT_SOURCE_DIR}/scripts/ToolChain.cmake)
# *NOTE*: or use the cmake.exe '-T' option instead
include (${CMAKE_CURRENT_SOURCE_DIR}/scripts/SetTargetPlatform.cmake)
include (${CMAKE_CURRENT_SOURCE_DIR}/scripts/TargetPlatform.cmake)

include (${CMAKE_CURRENT_SOURCE_DIR}/scripts/Configuration.cmake)
include (${CMAKE_CURRENT_SOURCE_DIR}/scripts/Macros.cmake)
include (${CMAKE_CURRENT_SOURCE_DIR}/scripts/VersionConfig.cmake)

# detect version
#include (MacrosAutotools)
#autoinit (configure.ac)
#autoversion (Common)
#if (EXISTS config.h.in)
# autoheaderchecks (config.h.in ConfigureChecks.cmake config.h.cmake)
#else ()
# message (STATUS "Generate config.h.in with autogen.sh to enable cmake header difference check.")
#endif ()
# set PACKAGE_... variables
#autopackage (dbus
#             ${DBUS_VERSION_STRING}
#             "http://dbus.freedesktop.org"
#             "https://bugs.freedesktop.org/enter_bug.cgi?product=dbus")

cmake_minimum_required (VERSION 2.8)

cmake_policy (SET CMP0069 NEW) # IPO is enforced when enabled
include (CheckIPOSupported)
check_ipo_supported (RESULT IPO_SUPPORTED
                     OUTPUT IPO_REASON)

# global options
if (IPO_SUPPORTED)
 message(STATUS "-- IPO is supported: \"${IPO_REASON}\"")
 set (CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
endif (IPO_SUPPORTED)
#set (CMAKE_VERBOSE_MAKEFILE ON)
#set (MSVC_INCREMENTAL_DEFAULT OFF)
set_property (GLOBAL PROPERTY USE_FOLDERS ON)

#include (MacroLibrary)
if (UNIX)
# include (ConfigureChecks.cmake)
 find_program (CCACHE_PROGRAM ccache)
 if (CCACHE_PROGRAM)
#  set_property (GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
 endif (CCACHE_PROGRAM)
elseif (WIN32)
# include (Win32Macros)
# include (ProjectSourceGroup)
endif ()

# build type
#variable_watch (CMAKE_BUILD_TYPE)
# *NOTE*: iff the user specifies -DCMAKE_BUILD_TYPE on the command line, take
#         their definition and dump it in the cache along with proper
#         documentation, otherwise set CMAKE_BUILD_TYPE to Debug prior to
#         calling PROJECT()
set (BUILD_TYPE_CACHE_STRING "choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel.")
if (DEFINED CMAKE_BUILD_TYPE)
 set (CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING ${BUILD_TYPE_CACHE_STRING})
elseif (NOT CMAKE_CONFIGURATION_TYPES)
 set (CMAKE_BUILD_TYPE Debug CACHE STRING ${BUILD_TYPE_CACHE_STRING})
endif ()

#message("current CMAKE_SYSTEM_VERSION = ${CMAKE_SYSTEM_VERSION}")

cmake_policy (SET CMP0048 NEW) # allow project VERSION
#cmake_policy (SET CMP0056 OLD) # honor per-config flags in try_compile()
#cmake_policy (SET CMP0066 OLD) # honor per-config flags in try_compile()
#cmake_policy (SET CMP0056 NEW)
project (${PROJECT_NAME}
         VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_MICRO}
         LANGUAGES CXX C)

# directories
# override the installation directories ?
#set (CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION OFF)
if (UNIX)
 include (GNUInstallDirs)
endif (UNIX)
set (INSTALL_LIB_DIR lib CACHE PATH "Installation directory for libraries")
set (INSTALL_BIN_DIR bin CACHE PATH "Installation directory for executables")
set (INSTALL_INCLUDE_DIR include CACHE PATH "Installation directory for header files")
if (WIN32 AND NOT CYGWIN)
 set (DEF_INSTALL_CMAKE_DIR CMake)
else ()
 set (DEF_INSTALL_CMAKE_DIR lib/CMake/lib${PROJECT_NAME})
endif ()
set (INSTALL_CMAKE_DIR ${DEF_INSTALL_CMAKE_DIR} CACHE PATH "Installation directory for CMake files")

# make relative paths absolute (needed later on)
foreach (path LIB BIN INCLUDE CMAKE)
 set (var INSTALL_${path}_DIR)
 if (NOT IS_ABSOLUTE "${${var}}")
  set (${var} "${CMAKE_INSTALL_PREFIX}/${${var}}")
 endif ()
endforeach ()

# project options
set (DEBUG_DEBUGGER TRUE
     CACHE BOOL "in debugger")

if (CMAKE_BUILD_TYPE STREQUAL Debug)
 if (CMAKE_COMPILER_IS_GNUCXX)
  add_compile_options (-g)
  add_compile_options (-O0)
  add_compile_options (-fno-inline)
 endif ()
 add_definitions (-D_DEBUG)
 if (WIN32)
  add_definitions (-D_ITERATOR_DEBUG_LEVEL=2)
 endif ()
#  set_property (SOURCE PROPERTY COMPILE_DEFINITIONS _DEBUG)
 if (DEBUG_DEBUGGER AND
     NOT DEFINED IN_CPACK)
  add_definitions (-DDEBUG_DEBUGGER)
 endif ()
elseif (CMAKE_BUILD_TYPE STREQUAL Release)
 if (CMAKE_COMPILER_IS_GNUCXX)
  add_compile_options (-O3)
 endif ()
 add_definitions (-DACE_NDEBUG)
#  set_property (SOURCE PROPERTY COMPILE_DEFINITIONS ACE_NDEBUG)
 if (WIN32)
  add_definitions (-D_ITERATOR_DEBUG_LEVEL=0)
 endif ()
endif ()

if (UNIX)
 include (FindPkgConfig)
 if (NOT PKG_CONFIG_FOUND)
  message (FATAL_ERROR "pkg-config not found, aborting")
 endif (NOT PKG_CONFIG_FOUND)
endif (UNIX)

option (TRACING "enable tracing" OFF)
if (TRACING)
 add_definitions (-DCOMMON_NTRACE=0)
#  set_property (SOURCE PROPERTY COMPILE_DEFINITIONS COMMON_NTRACE=0)
else ()
 add_definitions (-DCOMMON_NTRACE=1)
#  set_property (SOURCE PROPERTY COMPILE_DEFINITIONS COMMON_NTRACE=1)
endif ()

option (VALGRIND_SUPPORT "enable valgrind support" OFF)
if (VALGRIND_SUPPORT)
 include (CheckIncludeFiles)
 check_include_files (valgrind/valgrind.h HAVE_VALGRIND_H)
#  include (CheckLibraryExists)
#  check_library_exists (libvalgrind "" "" HAVE_LIB_VALGRIND)
 pkg_check_modules (PKGVALGRIND REQUIRED valgrind)
 add_definitions (-DLIBCOMMON_ENABLE_VALGRIND_SUPPORT)
#  set_property (SOURCE PROPERTY COMPILE_DEFINITIONS LIBCOMMON_ENABLE_VALGRIND_SUPPORT)
endif ()

option (BUILD_MODULES "build (git) sub-modules" OFF)

if (UNIX)
 if (CMAKE_COMPILER_IS_GNUCXX)
  add_compile_options (-ansi)
  add_compile_options (-std=c++11)
  add_compile_options (-Wall)
  set (CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS} -g")
  add_definitions (-D_GNU_SOURCE)
#  set_property (SOURCE PROPERTY COMPILE_DEFINITIONS _GNU_SOURCE)
 endif (CMAKE_COMPILER_IS_GNUCXX)
elseif (WIN32)
 if (MSVC)
  add_compile_options (/MP)
 # add_definitions (-D_CRT_SECURE_NO_WARNINGS)

  set (MSVC_INCREMENTAL_YES_FLAG "/INCREMENTAL:NO")
  foreach (FLAG_TYPE EXE MODULE SHARED STATIC)
   string (REGEX REPLACE "/INCREMENTAL* " "/INCREMENTAL:NO " CMAKE_${FLAG_TYPE}_LINKER_FLAGS_DEBUG "${CMAKE_${FLAG_TYPE}_LINKER_FLAGS_DEBUG}")
#  message (STATUS "flag: ${FLAG_TYPE}: ${CMAKE_${FLAG_TYPE}_LINKER_FLAGS_DEBUG}")
   string (REPLACE "/EDITANDCONTINUE"     ""                 CMAKE_${FLAG_TYPE}_LINKER_FLAGS_DEBUG "${CMAKE_${FLAG_TYPE}_LINKER_FLAGS_DEBUG}")
   set (CMAKE_${FLAG_TYPE}_LINKER_FLAGS_DEBUG "${CMAKE_${FLAG_TYPE}_LINKER_FLAGS_DEBUG}"
        CACHE STRING "Overriding default debug ${FLAG_TYPE} linker flags." FORCE)
   mark_as_advanced (CMAKE_${FLAG_TYPE}_LINKER_FLAGS_DEBUG)
  endforeach ()
 endif (MSVC)
endif ()

################################################################################
file (TO_CMAKE_PATH "$ENV{LIB_ROOT}/cotire/CMake" COTIRE_PATH)
list (APPEND CMAKE_MODULE_PATH "${COTIRE_PATH}")
message (STATUS "CMAKE_MODULE_PATH: ${CMAKE_MODULE_PATH}")
include (cotire)
set_directory_properties (PROPERTIES COTIRE_ADD_UNITY_BUILD FALSE)

################################################################################

add_subdirectory (src)

################################################################################

# configuration files
# Export the package for use from the build-tree
# (this registers the build-tree with a global CMake-registry)
export (PACKAGE ${PROJECT_NAME})

# Create the CommonConfig.cmake and CommonConfigVersion.cmake files
# *NOTE*: see: https://cmake.org/Wiki/CMake/Tutorials/How_to_create_a_ProjectConfig.cmake_file
file (RELATIVE_PATH REL_INCLUDE_DIR "${INSTALL_CMAKE_DIR}" "${INSTALL_INCLUDE_DIR}")
# ... for the build tree
set (CONF_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}" "${PROJECT_BINARY_DIR}")
if (UNIX)
 set (NEWLINE_STYLE UNIX)
elseif (WIN32)
 set (NEWLINE_STYLE WIN32)
else ()
 set (NEWLINE_STYLE UNIX)
endif ()
configure_file (${PROJECT_NAME}Config.cmake.in
                "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
                @ONLY
                NEWLINE_STYLE ${NEWLINE_STYLE})
# ... for the install tree
set (CONF_INCLUDE_DIRS "\${Common_CMAKE_DIR}/${REL_INCLUDE_DIR}")
configure_file (${PROJECT_NAME}Config.cmake.in
                "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/${PROJECT_NAME}Config.cmake"
                @ONLY
                NEWLINE_STYLE ${NEWLINE_STYLE})
# ... for both
configure_file (${PROJECT_NAME}ConfigVersion.cmake.in
                "${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
                @ONLY
                NEWLINE_STYLE ${NEWLINE_STYLE})

configure_file (${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake.in
                ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}_config.h
                @ONLY
                NEWLINE_STYLE ${NEWLINE_STYLE})
#add_custom_target (${PROJECT_NAME}_config)
#set_target_properties (${PROJECT_NAME}_config PROPERTIES
#                       PUBLIC_HEADER "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}_config.h")
add_definitions (-DHAVE_CONFIG_H)
#set_property (SOURCE PROPERTY COMPILE_DEFINITIONS HAVE_CONFIG_H)

################################################################################

# Install the CommonConfig.cmake and CommonConfigVersion.cmake
install (FILES
         "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/${PROJECT_NAME}Config.cmake"
         "${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
         DESTINATION "${INSTALL_CMAKE_DIR}" COMPONENT dev)
# Install the export set for use with the install-tree
install (EXPORT ${PROJECT_NAME}Targets
         DESTINATION "${INSTALL_CMAKE_DIR}" COMPONENT dev)

install (FILES "${CMAKE_CURRENT_BINARY_DIR}/lib${PROJECT_NAME}_config.h"
         DESTINATION "${INSTALL_INCLUDE_DIR}/${PROJECT_NAME}"
         COMPONENT dev)

################################################################################
file (TO_CMAKE_PATH "$ENV{LIB_ROOT}/cmake-modules" CMAKE_MODULES_PATH)
file (TO_CMAKE_PATH "$ENV{LIB_ROOT}/CMakeHelpers" CMAKE_HELPERS_PATH)
list (APPEND CMAKE_MODULE_PATH
      "${CMAKE_MODULES_PATH}"
      "${CMAKE_HELPERS_PATH}")
#include (UseBackportedModules)
cmake_policy (SET CMP0026 OLD)
cmake_policy (SET CMP0053 OLD)
include (CreateLaunchers)
if (WIN32)
 include (generate_product_version)
endif (WIN32)

# enable (unit/integration) tests
#include (CTest)
#enable_testing ()

################################################################################

add_subdirectory (test_u)
add_subdirectory (test_i)

################################################################################

if (UNIX)
# include (CheckLibraryExists)
# check_library_exists (pthread pthread_create "" HAVE_LIB_PTHREAD)
 include (GNUInstallDirs)

 set (CMAKE_THREAD_PREFER_PTHREAD ON)
elseif (WIN32)
 if (MSVC)
#  set (CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION "10.0.16299.0") # set Windows SDK version
#  message (STATUS "CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION: ${CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION}")
#  add_definitions (-CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION=${CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION})
 endif (MSVC)
endif ()

#set (NSIS_PROJECT_FILE "lib${PROJECT_NAME}.nsi")
#add_custom_command (TARGET ${PROJECT_NAME}
#                    POST_BUILD
#                    COMMAND make_installer.bat ${NSIS_PROJECT_FILE}
#                    WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/scripts"
#                    COMMENT "making installer..."
#                    VERBATIM)

# build a CPack driven installer package
# *NOTE*: "... If CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_NO_WARNINGS is NOT set, then
#         this file warns about required files that do not exist. You can set
#         this variable to ON before including this file to avoid the warning.
#         For example, the Visual Studio Express editions do not include the
#         redistributable files, so if you include this file on a machine with
#         only VS Express installed, you’ll get the warning. ..."
include (InstallRequiredSystemLibraries)
set (CPACK_PACKAGE_VERSION_MAJOR ${VERSION_MAJOR})
set (CPACK_PACKAGE_VERSION_MINOR ${VERSION_MINOR})
set (CPACK_PACKAGE_VERSION_PATCH ${VERSION_MICRO})
set (CPACK_RESOURCE_FILE_LICENSE ${CMAKE_CURRENT_SOURCE_DIR}/LICENSE)
set (CPACK_RESOURCE_FILE_README ${CMAKE_CURRENT_SOURCE_DIR}/README)

# build a CPack driven installer package
include (CPack)
