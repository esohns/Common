set (LIBICONV_SUPPORT_DEFAULT ON)
if (UNIX)
 find_package (libiconv MODULE)
 if (libiconv_FOUND)
  message (STATUS "found libiconv")
  set (libiconv_FOUND TRUE)
 endif (libiconv_FOUND)
# pkg_check_modules (PKG_LIBICONV libiconv)
elseif (WIN32)
 if (VCPKG_SUPPORT)
  find_package (libiconv MODULE)
  find_path (LIBICONV_INCLUDE_DIR NAMES iconv.h
             HINTS "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}"
             PATH_SUFFIXES include
             NO_DEFAULT_PATH)
  if (libiconv_FOUND)
   message (STATUS "found libiconv")
   set (LIBICONV_FOUND TRUE)
   set (LIBICONV_INCLUDE_DIRS "${LIBICONV_INCLUDE_DIR}")
   if (CMAKE_BUILD_TYPE STREQUAL "Debug" OR
       CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    set (LIBICONV_LIB_DIR "${VCPKG_ROOT}/installed/${VCPKG_TARGET_TRIPLET}/debug/bin")
   else ()
    set (LIBICONV_LIB_DIR "${VCPKG_ROOT}/installed/${VCPKG_TARGET_TRIPLET}/bin")
   endif (CMAKE_BUILD_TYPE STREQUAL "Debug" OR
          CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
  endif (libiconv_FOUND)
 endif (VCPKG_SUPPORT)
 if (NOT LIBICONV_FOUND)
  find_path (LIBICONV_INCLUDE_DIR NAMES iconv.h
             PATHS "$ENV{LIB_ROOT}/libiconv"
             PATH_SUFFIXES include)
  set (LIBICONV_LIB "libiconv")
  if (CMAKE_BUILD_TYPE STREQUAL "Debug" OR
      CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
   set (LIBICONV_LIB "${LIBICONV_LIB}D")
  endif (CMAKE_BUILD_TYPE STREQUAL "Debug" OR
         CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
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
   set (LIBICONV_LIB_DIR "$ENV{LIB_ROOT}/${LIB_PATH_SUFFIX}")
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
