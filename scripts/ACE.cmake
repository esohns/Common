if (UNIX)
# set (ACE_LIB_FILE libACE${LIB_FILE_SUFFIX}.so)
 set (ACE_LIB_FILE libACE.so)
 find_library (ACE_LIBRARY ${ACE_LIB_FILE}
               PATHS $ENV{ACE_ROOT} ${CMAKE_CURRENT_SOURCE_DIR}/../modules/ACE
               PATH_SUFFIXES lib
               DOC "searching for ${ACE_LIB_FILE}")
elseif (WIN32)
 set (ACE_LIB_FILE ACE${LIB_FILE_SUFFIX}.lib)

 if (VCPKG_USE)
#  find_package (ACE
#                COMPONENTS core
#                OPTIONAL_COMPONENTS ssl,xml)
  find_path (ACE_INCLUDE_DIR ace/ACE.h
             PATHS ${VCPKG_INCLUDE_DIR_BASE}
             DOC "searching for ACE.h"
             NO_DEFAULT_PATH)
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
 endif (VCPKG_USE)
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
  if (VCPKG_USE)
   set (ACE_INCLUDE_DIR ${VCPKG_INCLUDE_DIR_BASE})
   set (ACE_LIB_DIR ${VCPKG_LIB_DIR}/bin)
  elseif (DEFINED ENV{LIB_ROOT})
   set (ACE_INCLUDE_DIR $ENV{LIB_ROOT}/ACE_TAO/ACE)
   set (ACE_LIB_DIR $ENV{LIB_ROOT}/ACE_TAO/ACE/lib)
  else ()
   message (FATAL_ERROR "ACE library not available, aborting")
  endif ()
 endif (NOT DEFINED ENV{ACE_ROOT})
endif ()
