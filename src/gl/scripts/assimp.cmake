if (UNIX)
 include (FindPkgConfig)
 pkg_check_modules (PKG_ASSIMP assimp)
 if (PKG_ASSIMP_FOUND)
  set (ASSIMP_FOUND TRUE)
  if (CMAKE_SYSTEM_NAME MATCHES "Linux")
   include (OS)
   if (${LSB_RELEASE_ID_SHORT} MATCHES "Fedora")
# *WORKAROUND*: 'prefix' variable must be prepended to 'includedir'
    pkg_get_variable (PKG_ASSIMP_PREFIX assimp prefix)
    set (ASSIMP_INCLUDE_DIRS "${PKG_ASSIMP_PREFIX}/${PKG_ASSIMP_INCLUDE_DIRS}")
   elseif (${LSB_RELEASE_ID_SHORT} MATCHES "Ubuntu")
# *WORKAROUND*: there seems to be a problem on Ubuntu
    set (ASSIMP_INCLUDE_DIRS "/usr/include/assimp")
   endif ()
  endif (CMAKE_SYSTEM_NAME MATCHES "Linux")
  set (ASSIMP_LIBRARIES ${PKG_ASSIMP_LIBRARIES})
 endif (PKG_ASSIMP_FOUND)
# find_library (ASSIMP_LIBRARY libassimp.so.3
#               DOC "searching for libassimp")
# if (NOT ASSIMP_LIBRARY)
#  message (FATAL_ERROR "could not find libassimp, aborting")
# endif (NOT ASSIMP_LIBRARY)
elseif (WIN32)
 if (VCPKG_USE)
#  cmake_policy (SET CMP0074 OLD)
  find_package (assimp CONFIG)
  if (assimp_FOUND)
   message (STATUS "found assimp")
   set (ASSIMP_FOUND TRUE)
   set (ASSIMP_INCLUDE_DIRS ${VCPKG_INCLUDE_DIR_BASE})
   set (ASSIMP_LIBRARIES "assimp::assimp")
   set (ASSIMP_LIB_DIR ${VCPKG_LIB_DIR}/bin)
  endif (assimp_FOUND)
 endif (VCPKG_USE)
 if (NOT ASSIMP_FOUND)
  message (STATUS "found assimp")
  set (ASSIMP_FOUND TRUE)
  set (ASSIMP_INCLUDE_DIRS "$ENV{LIB_ROOT}/assimp/include;$ENV{LIB_ROOT}/assimp/cmake/include")
  set (ASSIMP_LIB_WITH_SUFFIX "assimp-vc${MSVC_TOOLSET_VERSION}-mt${LIB_FILE_SUFFIX}.lib")
  set (ASSIMP_LIBRARIES "$ENV{LIB_ROOT}/assimp/cmake/lib/${CMAKE_BUILD_TYPE}/${ASSIMP_LIB_WITH_SUFFIX}")
  set (ASSIMP_LIB_DIR "$ENV{LIB_ROOT}/assimp/cmake/bin/${CMAKE_BUILD_TYPE}")
 endif (NOT ASSIMP_FOUND)
endif ()
if (ASSIMP_FOUND)
 option (ASSIMP_SUPPORT "enable assimp support" ON)
 if (ASSIMP_SUPPORT)
  add_definitions (-DASSIMP_SUPPORT)
 endif (ASSIMP_SUPPORT)
endif (ASSIMP_FOUND)
