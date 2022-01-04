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
  file (TO_CMAKE_PATH "$ENV{LIB_ROOT}/vcpkg" VCPKG_ROOT)
 else ()
  file (TO_CMAKE_PATH "$ENV{VCPKG_ROOT}" VCPKG_ROOT)
#  message (STATUS "VCPKG_ROOT set to \"${VCPKG_ROOT}\"")
 endif (NOT DEFINED ENV{VCPKG_ROOT})
 set (VCPKG_SCRIPT "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
 if (EXISTS ${VCPKG_SCRIPT})
  set (CMAKE_TOOLCHAIN_FILE ${VCPKG_SCRIPT} CACHE STRING "vcpkg.cmake")
  set (VCPKG_TARGET_TRIPLET "${CMAKE_VS_PLATFORM_TOOLSET_HOST_ARCHITECTURE}-windows")
  set (VCPKG_APPLOCAL_DEPS OFF CACHE BOOL "" FORCE)
  message (STATUS "using vcpkg in ${VCPKG_ROOT}")
  set (VCPKG_SUPPORT ON)
 else ()
  message (WARNING "vcpkg toolchain script not found (was: \"${VCPKG_SCRIPT}\")")
 endif (EXISTS ${VCPKG_SCRIPT})
endif (WIN32)
