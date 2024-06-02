if (UNIX)
# include (FindPkgConfig)
# pkg_check_modules (PKG_LIBPNG libpng)
 find_library (PNG_LIBRARY libpng16.so.16
               DOC "searching for libpng")
 if (NOT PNG_LIBRARY)
  message (WARNING "could not find libpng, continuing")
 else ()
  message (STATUS "Found libpng \"${PNG_LIBRARY}\"")
  set (PNG_FOUND TRUE)
  set (PNG_LIBRARIES "${PNG_LIBRARY}")
 endif (NOT PNG_LIBRARY)
elseif (WIN32)
 set (LIB_FILE_SUFFIX "")
 if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
  set (LIB_FILE_SUFFIX "d")
 endif (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
 if (VCPKG_USE)
#  cmake_policy (SET CMP0074 OLD)
  find_package (libpng)
  if (libpng_FOUND)
   message (STATUS "found libpng")
   set (PNG_FOUND TRUE)
   set (PNG_INCLUDE_DIRS ${VCPKG_INCLUDE_DIR})
   set (PNG_LIBRARIES ${VCPKG_LIB_DIR}/libpng16${LIB_FILE_SUFFIX}.lib)
   set (PNG_LIB_DIR "${VCPKG_BIN_DIR}")
  endif (libpng_FOUND)
 endif (VCPKG_USE)
 if (NOT PNG_FOUND)
  set (PNG_LIB_FILE libpng16${LIB_FILE_SUFFIX}.lib)
  find_library (PNG_LIBRARY ${PNG_LIB_FILE}
                PATHS $ENV{LIB_ROOT}/libpng/build
                PATH_SUFFIXES ${CMAKE_BUILD_TYPE}
                DOC "searching for \"${PNG_LIB_FILE}\"")
  if (PNG_LIBRARY)
   message (STATUS "found libpng")
   set (PNG_FOUND TRUE)
   set (PNG_INCLUDE_DIRS $ENV{LIB_ROOT}/libpng/build;$ENV{LIB_ROOT}/libpng)
   set (PNG_LIBRARIES ${PNG_LIBRARY})
   set (PNG_LIB_DIR $ENV{LIB_ROOT}/zlib/build/$<CONFIG>;$ENV{LIB_ROOT}/libpng/build/$<CONFIG>)
  endif (PNG_LIBRARY)
 endif (NOT PNG_FOUND)
endif ()
if (PNG_FOUND)
 option (LIBPNG_SUPPORT "enable libpng support" ON)
 if (LIBPNG_SUPPORT)
  add_definitions (-DLIBPNG_SUPPORT)
 endif (LIBPNG_SUPPORT)
endif (PNG_FOUND)
