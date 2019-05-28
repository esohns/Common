if (UNIX)
 set (ACE_LIB_FILE libACE.so)
 find_library (ACE_LIBRARY ${ACE_LIB_FILE}
#                HINTS ${CMAKE_CURRENT_SOURCE_DIR}/../modules/ACE
               PATHS $ENV{ACE_ROOT} /usr/local/src/ACE_wrappers
               PATHS ${CMAKE_CURRENT_LIST_DIR}/../../ACE_TAO/ACE
               PATH_SUFFIXES lib
               DOC "searching for ${ACE_LIB_FILE} (system paths)")
elseif (WIN32)
# *TODO*: this does not work
 set (LIB_FILE_SUFFIX "")
 if (CMAKE_BUILD_TYPE STREQUAL "Debug" OR
     CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
  set (LIB_FILE_SUFFIX "d")
 endif ()
 set (ACE_LIB_FILE ACE${LIB_FILE_SUFFIX}.lib)
 find_library (ACE_LIBRARY ${ACE_LIB_FILE}
               PATHS $ENV{ACE_ROOT} ${CMAKE_CURRENT_BINARY_DIR}/../../../ACE_TAO/ACE
               PATH_SUFFIXES lib lib\\${CMAKE_BUILD_TYPE}\\Win32
               DOC "searching for ${ACE_LIB_FILE}")
endif ()
#if (NOT EXISTS ACE_LIBRARY)
if (NOT ACE_LIBRARY)
 message (FATAL_ERROR "could not find ${ACE_LIB_FILE}, aborting")
else ()
 #message (STATUS "Found ACE library \"${ACE_LIBRARY}\"")
endif ()
add_definitions (-DACE_HAS_DLL)

