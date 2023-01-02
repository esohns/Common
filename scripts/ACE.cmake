if (UNIX)
 set (ACE_LIB_FILE libACE${LIB_FILE_SUFFIX}.so)
 find_library (ACE_LIBRARY ${ACE_LIB_FILE}
               PATHS $ENV{ACE_ROOT} ${CMAKE_CURRENT_SOURCE_DIR}/../modules/ACE
               PATH_SUFFIXES lib
               DOC "searching for ${ACE_LIB_FILE}")
elseif (WIN32)
 set (ACE_LIB_FILE ACE${LIB_FILE_SUFFIX}.lib)

 if (VCPKG_SUPPORT AND NOT DEFINED ENV{ACE_ROOT})
#  find_package (ACE
#                COMPONENTS core
#                OPTIONAL_COMPONENTS ssl)
  
  message (STATUS "VCPKG_INCLUDE_DIR_BASE: ${VCPKG_INCLUDE_DIR_BASE}")
  set (ACE_INCLUDE_DIR ${VCPKG_INCLUDE_DIR_BASE})
  set (ACE_H "${VCPKG_INCLUDE_DIR_BASE}/ace/ACE.h")
  if (EXISTS ${ACE_H})
   message (STATUS "ACE_H: ${ACE_H}")
  endif ()
  find_path (ACE_INCLUDE_DIR ace/ACE.h
             PATHS ${VCPKG_INCLUDE_DIR_BASE}
             DOC "searching for ACE.h"
             NO_DEFAULT_PATH)
  message (STATUS "VCPKG_LIB_DIR: ${VCPKG_LIB_DIR}")
  set (ACE_LIB "${VCPKG_LIB_DIR}/lib/ACE.lib")
  if (EXISTS ${ACE_LIB})
   message (STATUS "ACE_LIB: ${ACE_LIB}")
  endif ()
  find_library (ACE_LIBRARY ${ACE_LIB_FILE}
                PATHS ${VCPKG_LIB_DIR}
                PATH_SUFFIXES lib
                DOC "searching for ${ACE_LIB_FILE}"
                NO_DEFAULT_PATH)
 else ()
  find_library (ACE_LIBRARY ${ACE_LIB_FILE}
                PATHS $ENV{ACE_ROOT} $ENV{LIB_ROOT}/ACE_TAO/ACE
                PATH_SUFFIXES lib
                DOC "searching for ${ACE_LIB_FILE}"
                REQUIRED
                NO_DEFAULT_PATH)
 endif (VCPKG_SUPPORT AND NOT DEFINED ENV{ACE_ROOT})
endif ()
#if (NOT EXISTS ACE_LIBRARY)
if (NOT ACE_LIBRARY)
 message (FATAL_ERROR "could not find ${ACE_LIB_FILE}, aborting")
else ()
 message (STATUS "found ACE library \"${ACE_LIBRARY}\"")
#add_definitions (-DACE_HAS_DLL)
endif ()

# set ACE include[/dll] directory
if (DEFINED ENV{ACE_ROOT})
 file (TO_CMAKE_PATH $ENV{ACE_ROOT} ACE_ROOT_CMAKE)
 set (ACE_INCLUDE_DIR ${ACE_ROOT_CMAKE})
 set (ACE_LIB_DIR ${ACE_ROOT_CMAKE}/lib)
endif (DEFINED ENV{ACE_ROOT})
if (UNIX)
 if (NOT DEFINED ENV{ACE_ROOT})
  set (ACE_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../modules/ACE)
 endif (NOT DEFINED ENV{ACE_ROOT})
elseif (WIN32)
 if (NOT DEFINED ENV{ACE_ROOT})
  if (DEFINED ENV{LIB_ROOT})
   set (ACE_INCLUDE_DIR $ENV{LIB_ROOT}/ACE_TAO/ACE)
   set (ACE_LIB_DIR $ENV{LIB_ROOT}/ACE_TAO/ACE/lib)
  elseif (VCPKG_SUPPORT)
   set (ACE_INCLUDE_DIR ${VCPKG_INCLUDE_DIR_BASE})
   set (ACE_LIB_DIR ${VCPKG_LIB_DIR}/bin)
  else ()
   message (FATAL_ERROR "ACE library not available, aborting")
  endif ()
 endif (NOT DEFINED ENV{ACE_ROOT})
endif ()
