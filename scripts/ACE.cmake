if (UNIX)
 set (ACE_LIB_FILE libACE.so)
 find_library (ACE_LIBRARY ${ACE_LIB_FILE}
               PATHS $ENV{ACE_ROOT} ${CMAKE_CURRENT_SOURCE_DIR}/../modules/ACE
               PATH_SUFFIXES lib
               DOC "searching for ${ACE_LIB_FILE} (system paths)")
elseif (WIN32)
# *TODO*: this does not work
 set (LIB_FILE_SUFFIX "")
# message (STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
 if (CMAKE_BUILD_TYPE STREQUAL "Debug" OR
     CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
  set (LIB_FILE_SUFFIX "d")
 endif ()

 set (ACE_LIB_FILE ACE${LIB_FILE_SUFFIX}.lib)
 if (VCPKG_SUPPORT AND NOT DEFINED ENV{ACE_ROOT})
#  find_package (ACE
#                COMPONENTS core
#                OPTIONAL_COMPONENTS ssl)
  find_path (ACE_INCLUDE_DIR ace/ACE.h)
  find_library (ACE_LIBRARY ${ACE_LIB_FILE}
                PATHS $ENV{ACE_ROOT}
                PATH_SUFFIXES lib lib\\${CMAKE_BUILD_TYPE}\\Win32
                DOC "searching for ${ACE_LIB_FILE}"
                REQUIRED)
 else ()
  find_library (ACE_LIBRARY ${ACE_LIB_FILE}
                PATHS $ENV{ACE_ROOT} $ENV{LIB_ROOT}/ACE_TAO/ACE
                PATH_SUFFIXES lib lib\\${CMAKE_BUILD_TYPE}\\Win32
                DOC "searching for ${ACE_LIB_FILE}"
                REQUIRED
                NO_DEFAULT_PATH)
 endif (VCPKG_SUPPORT AND NOT DEFINED ENV{ACE_ROOT})
endif ()
#if (NOT EXISTS ACE_LIBRARY)
if (NOT ACE_LIBRARY)
 message (FATAL_ERROR "could not find ${ACE_LIB_FILE}, aborting")
else ()
 message (STATUS "Found ACE library \"${ACE_LIBRARY}\"")
endif ()
add_definitions (-DACE_HAS_DLL)

if (DEFINED ENV{ACE_ROOT})
 file (TO_CMAKE_PATH $ENV{ACE_ROOT} ACE_ROOT_CMAKE)
 include_directories (${ACE_ROOT_CMAKE})   
endif (DEFINED ENV{ACE_ROOT})
if (UNIX)
 if (NOT DEFINED ENV{ACE_ROOT})
  include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../modules/ACE)
 endif (NOT DEFINED ENV{ACE_ROOT})
elseif (WIN32)
 if (NOT DEFINED ENV{ACE_ROOT})
  if (VCPKG_SUPPORT)
   include_directories (${ACE_INCLUDE_DIR})
  else ()
   include_directories ($ENV{LIB_ROOT}/ACE_TAO/ACE)
  endif (VCPKG_SUPPORT)
 endif (NOT DEFINED ENV{ACE_ROOT})
endif ()
