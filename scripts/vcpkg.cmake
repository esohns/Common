if (WIN32)
 if (DEFINED ENV{VCPKG_ROOT})
  file (TO_CMAKE_PATH "$ENV{VCPKG_ROOT}" VCPKG_ROOT)
  message (STATUS "VCPKG_ROOT set to \"${VCPKG_ROOT}\"")
 else ()
# try to find vcpkg
  file (TO_CMAKE_PATH "$ENV{LIB_ROOT}/vcpkg" VCPKG_ROOT)
 endif (DEFINED ENV{VCPKG_ROOT})
 if (EXISTS ${VCPKG_ROOT})
  set (VCPKG_SCRIPT "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
  if (EXISTS ${VCPKG_SCRIPT})
   set (VCPKG_SUPPORT ON)
  else ()
   message (WARNING "vcpkg toolchain script not found (was: \"${VCPKG_SCRIPT}\"), continuing")
  endif (EXISTS ${VCPKG_SCRIPT})
 endif (EXISTS ${VCPKG_ROOT})
endif (WIN32)
if (VCPKG_SUPPORT)
 option (VCPKG_USE "use vcpgk" OFF)
 if (VCPKG_USE)
  message (STATUS "using vcpkg in ${VCPKG_ROOT}")

  set (CMAKE_TOOLCHAIN_FILE ${VCPKG_SCRIPT} CACHE STRING "vcpkg.cmake")
  message (STATUS "including vcpkg.cmake from \"${VCPKG_SCRIPT}\"")

  set (VCPKG_TARGET_TRIPLET "${CMAKE_VS_PLATFORM_TOOLSET_HOST_ARCHITECTURE}-windows")
  message (STATUS "vcpkg triplet \"${VCPKG_TARGET_TRIPLET}\"")

  if (DEFINED ENV{VCPKG_INSTALLED_DIR})
   set (VCPKG_INSTALLED_DIR "$ENV{VCPKG_INSTALLED_DIR}")
  else ()
   set (VCPKG_INSTALLED_DIR "${VCPKG_ROOT}/installed")
  endif (DEFINED ENV{VCPKG_INSTALLED_DIR})
  message (STATUS "vcpkg installed directory \"${VCPKG_INSTALLED_DIR}\"")

  set (VCPKG_APPLOCAL_DEPS OFF CACHE BOOL "" FORCE)

  include (${VCPKG_SCRIPT})

  set (VCPKG_LIB_DIR_BASE ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET} CACHE STRING "vcpkg library path (base)" FORCE)
  set (VCPKG_INCLUDE_DIR_BASE ${VCPKG_LIB_DIR_BASE}/include CACHE STRING "vcpkg include path (base)" FORCE)
  if ($<CONFIG> STREQUAL "Debug" OR
      $<CONFIG> STREQUAL "RelWithDebInfo" OR
      ${CMAKE_BUILD_TYPE} STREQUAL "Debug" OR
      ${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo")
   set (VCPKG_LIB_DIR ${VCPKG_LIB_DIR_BASE}/debug CACHE STRING "vcpkg library path" FORCE)
  else ()
   set (VCPKG_LIB_DIR ${VCPKG_LIB_DIR_BASE} CACHE STRING "vcpkg library path" FORCE)
  endif ()
 endif (VCPKG_USE)
endif (VCPKG_SUPPORT)
