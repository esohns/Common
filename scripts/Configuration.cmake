# enable pkg-config support
include (FindPkgConfig)
if (UNIX)
 if (NOT PKG_CONFIG_FOUND)
  message (FATAL_ERROR "pkg-config not found, aborting")
 endif (NOT PKG_CONFIG_FOUND)
endif (UNIX)

if (NOT DEFINED PKG_CONFIG_EXECUTABLE)
 message (STATUS "setting PKG_CONFIG_EXECUTABLE...")
 if (DEFINED ENV{PKG_CONFIG_EXECUTABLE})
  set (PKG_CONFIG_EXECUTABLE $ENV{PKG_CONFIG_EXECUTABLE} CACHE STRING "pkg-config executable" FORCE)
 else ()
  message (WARNING "PKG_CONFIG_EXECUTABLE not defined, falling back")
  if (UNIX)
   set (PKG_CONFIG_EXECUTABLE "/usr/bin/pkg-config" CACHE STRING "pkg-config executable" FORCE)
  elseif (WIN32)
   set (PKG_CONFIG_EXECUTABLE "G:/bin/pkg-config-0.28/bin/pkg-config.exe" CACHE STRING "pkg-config executable" FORCE)
  endif ()
 endif (DEFINED ENV{PKG_CONFIG_EXECUTABLE})
endif (NOT DEFINED PKG_CONFIG_EXECUTABLE)
if (NOT EXISTS ${PKG_CONFIG_EXECUTABLE})
 message (WARNING "PKG_CONFIG_EXECUTABLE does not exist (was: \"${PKG_CONFIG_EXECUTABLE}\"), continuing")
endif ()
