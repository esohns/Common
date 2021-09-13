#include (CMakeDetermineSystem)

# *NOTE*: the first entry is the default option
# *TODO*: support cross-compiling
if (UNIX)
elseif (WIN32)
 set (TARGET_OS_PLATFORM "Host System" CACHE STRING "Host System (default)")
 set (TARGET_OS_PLATFORMS "Host System")

 # desktop (PC)
 set (TARGET_OS_PLATFORMS "${TARGET_OS_PLATFORMS};XP") # obsolete
 set (TARGET_OS_PLATFORMS "${TARGET_OS_PLATFORMS};Vista") # obsolete
 set (TARGET_OS_PLATFORMS "${TARGET_OS_PLATFORMS};7") # obsolete
 set (TARGET_OS_PLATFORMS "${TARGET_OS_PLATFORMS};8")
 set (TARGET_OS_PLATFORMS "${TARGET_OS_PLATFORMS};10")

 # mobile (phone)
 set (TARGET_OS_PLATFORMS "${TARGET_OS_PLATFORMS};CE") # obsolete
 set (TARGET_OS_PLATFORMS "${TARGET_OS_PLATFORMS};Phone")
 set (TARGET_OS_PLATFORMS "${TARGET_OS_PLATFORMS};Android")

 # app (web)
 set (TARGET_OS_PLATFORMS "${TARGET_OS_PLATFORMS};Store") # (*NOTE*: aka 'universal app')

 set_property (CACHE TARGET_OS_PLATFORM PROPERTY STRINGS ${TARGET_OS_PLATFORMS})
endif ()
if (NOT (DEFINED TARGET_OS_PLATFORM_LAST))
 set (TARGET_OS_PLATFORM_LAST "NotAPlatform" CACHE STRING "last target platform used")
 mark_as_advanced (FORCE TARGET_OS_PLATFORM_LAST)
endif ()
if (NOT (${TARGET_OS_PLATFORM} MATCHES ${TARGET_OS_PLATFORM_LAST}))
 if (UNIX)
 elseif (WIN32)
  unset (TARGET_OS_PLATFORM_XP CACHE)
 endif ()
 set (TARGET_OS_PLATFORM_LAST ${TARGET_OS_PLATFORM} CACHE STRING "Updating Target Platform Option" FORCE)
endif ()

if (UNIX)
elseif (WIN32)
 if (${TARGET_OS_PLATFORM} MATCHES "XP")
  if (NOT TARGET_PLATFORM_XP_SUPPORT)
   message (FATAL_ERROR "Windows XP not supported")
  endif (NOT TARGET_OS_PLATFORM_XP_SUPPORT)
  set (TARGET_OS_PLATFORM_XP ON CACHE STRING "target Windows XP")
  mark_as_advanced (FORCE TARGET_OS_PLATFORM_XP)
#  add_definitions (-DTARGET_PLATFORM_XP)
 endif (${TARGET_OS_PLATFORM} MATCHES "XP")
endif ()

