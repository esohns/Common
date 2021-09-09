#include(CMakeForceCompiler) #Disables checks

# this one is important
if (UNIX)
 set (CMAKE_SYSTEM_NAME Linux)
elseif (WIN32)
 set (CMAKE_SYSTEM_NAME Windows)
endif ()
#this one not so much
set (CMAKE_SYSTEM_VERSION 1)

if (WIN32)
 set (VCPKG_SUPPORT OFF)
 if (NOT DEFINED ENV{VCPKG_ROOT})
# try to find vcpkg
  file (TO_CMAKE_PATH "H:\\\\lib\\\\vcpkg" VCPKG_ROOT)
 else ()
  file (TO_CMAKE_PATH "ENV{VCPKG_ROOT}" VCPKG_ROOT)
 endif (NOT DEFINED ENV{VCPKG_ROOT})
 set (VCPKG_SCRIPT "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
 if (EXISTS ${VCPKG_SCRIPT})
  set (CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "vcpkg.cmake")
  set (VCPKG_TARGET_TRIPLET "x86-windows")
  message (STATUS "using vcpkg in ${VCPKG_ROOT}")
  set (VCPKG_SUPPORT ON)
 endif (EXISTS ${VCPKG_SCRIPT})
endif (WIN32)
