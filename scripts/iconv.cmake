set (LIBICONV_SUPPORT_DEFAULT ON)
if (UNIX)
 find_package (Iconv MODULE)
 if (Iconv_FOUND)
  message (STATUS "found iconv")
  set (LIBICONV_FOUND TRUE)
  set (LIBICONV_INCLUDE_DIRS "${Iconv_INCLUDE_DIRS}")
  set (LIBICONV_LIBRARIES "${Iconv_LIBRARIES}")
 endif (Iconv_FOUND)
# pkg_check_modules (PKG_LIBICONV libiconv)
elseif (WIN32)
 if (VCPKG_USE)
  find_package (Iconv)
  if (Iconv_FOUND)
   message (STATUS "found iconv")
   set (LIBICONV_FOUND TRUE)
   find_path (LIBICONV_INCLUDE_DIR NAMES iconv.h
              HINTS ${VCPKG_LIB_DIR_BASE}
              PATH_SUFFIXES include
              DOC "searching for iconv.h"
              NO_DEFAULT_PATH)
   set (LIBICONV_INCLUDE_DIRS ${LIBICONV_INCLUDE_DIR})
   set (LIBICONV_LIBRARIES "${VCPKG_LIB_DIR}/lib/iconv.lib")
   set (LIBICONV_LIB_DIR "${VCPKG_LIB_DIR}/bin")
  endif (Iconv_FOUND)
 endif (VCPKG_USE)
 if (NOT LIBICONV_FOUND)
  find_path (LIBICONV_INCLUDE_DIR NAMES iconv.h
             PATHS "$ENV{LIB_ROOT}/libiconv"
             PATH_SUFFIXES include
             DOC "searching for iconv.h"
             NO_DEFAULT_PATH)
  set (LIBICONV_LIB "libiconv")
  if ($<CONFIG> STREQUAL "Debug" OR
      $<CONFIG> STREQUAL "RelWithDebInfo" OR
      CMAKE_BUILD_TYPE STREQUAL "Debug" OR
      CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
   set (LIBICONV_LIB "${LIBICONV_LIB}D")
  endif ()
#  set (LIBICONV_LIB "${LIBICONV_LIB}.lib")
  set (LIB_PATH_SUFFIX "lib")
  if (${CMAKE_VS_PLATFORM_TOOLSET_HOST_ARCHITECTURE} STREQUAL "x64")
   set (LIB_PATH_SUFFIX "${LIB_PATH_SUFFIX}64")
  endif (${CMAKE_VS_PLATFORM_TOOLSET_HOST_ARCHITECTURE} STREQUAL "x64")
  find_library (LIBICONV_LIBRARY
                NAMES ${LIBICONV_LIB}
                PATHS "$ENV{LIB_ROOT}/libiconv"
                PATH_SUFFIXES ${LIB_PATH_SUFFIX}
                NO_DEFAULT_PATH)
  if (LIBICONV_INCLUDE_DIR AND LIBICONV_LIBRARY)
   message (STATUS "found libiconv")
   set (LIBICONV_FOUND TRUE)
   set (LIBICONV_INCLUDE_DIRS "${LIBICONV_INCLUDE_DIR}")
   set (LIBICONV_LIBRARIES "${LIBICONV_LIBRARY}")
   set (LIBICONV_LIB_DIR "$ENV{LIB_ROOT}/libiconv/${LIB_PATH_SUFFIX}")
  else ()
   message (WARNING "could not find libiconv, continuing")
  endif (LIBICONV_INCLUDE_DIR AND LIBICONV_LIBRARY)
 endif (NOT LIBICONV_FOUND)
endif ()
if (LIBICONV_FOUND)
 option (LIBICONV_SUPPORT "enable libiconv support" ${LIBICONV_SUPPORT_DEFAULT})
 if (LIBICONV_SUPPORT)
  add_definitions (-DLIBICONV_SUPPORT)
 endif (LIBICONV_SUPPORT)
endif (LIBICONV_FOUND)
