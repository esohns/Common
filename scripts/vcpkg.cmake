if (WIN32)
 if (DEFINED ENV{VCPKG_INSTALLATION_ROOT})
  file (TO_CMAKE_PATH "$ENV{VCPKG_INSTALLATION_ROOT}" VCPKG_INSTALLATION_ROOT)
  message (STATUS "VCPKG_INSTALLATION_ROOT set to \"${VCPKG_INSTALLATION_ROOT}\"")
 else ()
# try to find vcpkg
  file (TO_CMAKE_PATH "$ENV{LIB_ROOT}/vcpkg" VCPKG_INSTALLATION_ROOT)
 endif (DEFINED ENV{VCPKG_INSTALLATION_ROOT})
 if (EXISTS ${VCPKG_INSTALLATION_ROOT})
  set (VCPKG_SCRIPT "${VCPKG_INSTALLATION_ROOT}/scripts/buildsystems/vcpkg.cmake")
  if (EXISTS ${VCPKG_SCRIPT})
   set (VCPKG_SUPPORT ON)
  else ()
   message (WARNING "vcpkg toolchain script not found (was: \"${VCPKG_SCRIPT}\"), continuing")
  endif (EXISTS ${VCPKG_SCRIPT})
 endif (EXISTS ${VCPKG_INSTALLATION_ROOT})
endif (WIN32)
if (VCPKG_SUPPORT)
 option (VCPKG_USE "use vcpgk" OFF)
 if (VCPKG_USE)
  message (STATUS "using vcpkg in ${VCPKG_INSTALLATION_ROOT}")

  set (VCPKG_TARGET_TRIPLET "${CMAKE_VS_PLATFORM_TOOLSET_HOST_ARCHITECTURE}-windows")
  message (STATUS "vcpkg triplet \"${VCPKG_TARGET_TRIPLET}\"")

  if (DEFINED ENV{VCPKG_INSTALLED_DIR})
   set (VCPKG_INSTALLED_DIR "$ENV{VCPKG_INSTALLED_DIR}")
  else ()
   set (VCPKG_INSTALLED_DIR "${VCPKG_INSTALLATION_ROOT}/installed")
  endif (DEFINED ENV{VCPKG_INSTALLED_DIR})
  message (STATUS "vcpkg installed directory \"${VCPKG_INSTALLED_DIR}\"")

  set (VCPKG_APPLOCAL_DEPS OFF CACHE BOOL "" FORCE)
  set (VCPKG_MANIFEST_MODE OFF CACHE BOOL "" FORCE)
  set (VCPKG_MANIFEST_INSTALL OFF CACHE BOOL "" FORCE)

  set (CMAKE_TOOLCHAIN_FILE ${VCPKG_SCRIPT} CACHE STRING "vcpkg.cmake")
#  message (STATUS "including vcpkg.cmake from \"${VCPKG_SCRIPT}\"")
#  include (${VCPKG_SCRIPT})

  set (VCPKG_LIB_DIR_BASE ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET} CACHE STRING "vcpkg library path (base)" FORCE)
  set (VCPKG_INCLUDE_DIR ${VCPKG_LIB_DIR_BASE}/include CACHE STRING "vcpkg include path (base)" FORCE)
  if ($<CONFIG> STREQUAL "Debug" OR
      $<CONFIG> STREQUAL "RelWithDebInfo" OR
      ${CMAKE_BUILD_TYPE} STREQUAL "Debug" OR
      ${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo")
   set (VCPKG_LIB_DIR ${VCPKG_LIB_DIR_BASE}/debug/lib CACHE STRING "vcpkg library path" FORCE)
   set (VCPKG_BIN_DIR ${VCPKG_LIB_DIR_BASE}/debug/bin CACHE STRING "vcpkg binary path" FORCE)
  else ()
   set (VCPKG_LIB_DIR ${VCPKG_LIB_DIR_BASE}/lib CACHE STRING "vcpkg library path" FORCE)
   set (VCPKG_BIN_DIR ${VCPKG_LIB_DIR_BASE}/bin CACHE STRING "vcpkg binary path" FORCE)
 endif ()
 endif (VCPKG_USE)
endif (VCPKG_SUPPORT)
