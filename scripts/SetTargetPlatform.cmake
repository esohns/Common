#include (CMakeDetermineSystem)

# *NOTE*: the first entry is the default option
# *TODO*: support cross-compiling
if (UNIX)
elseif (WIN32)
 set (TARGET_PLATFORM "Host System" CACHE STRING "Host System (default)")

 # desktop (PC)
 set (TARGET_PLATFORM "XP" CACHE STRING "Windows XP") # obsolete
 set (TARGET_PLATFORM "Vista" CACHE STRING "Windows Vista") # obsolete
 set (TARGET_PLATFORM "7" CACHE STRING "Windows 7") # obsolete
 set (TARGET_PLATFORM "8" CACHE STRING "Windows 8/8.1")
 set (TARGET_PLATFORM "10" CACHE STRING "Windows 10")

 # mobile (phone)
 set (TARGET_PLATFORM "CE" CACHE STRING "Windows CE") # obsolete
 set (TARGET_PLATFORM "Phone" CACHE STRING "Windows Phone")
 set (TARGET_PLATFORM "Android" CACHE STRING "Android")

 # app (web)
 set (TARGET_PLATFORM "Store" CACHE STRING "Windows Store") # (*NOTE*: aka 'universal app')

 set_property (CACHE TARGET_PLATFORM PROPERTY STRINGS "Host System" "XP" "Vista" "7" "8" "10" "CE" "Phone" "Android" "Store")
endif ()
if (NOT (DEFINED TARGET_PLATFORM_LAST))
 set (TARGET_PLATFORM_LAST "NotAPlatform" CACHE STRING "last target platform used")
 mark_as_advanced (FORCE TARGET_PLATFORM_LAST)
endif ()
if (NOT (${TARGET_PLATFORM} MATCHES ${TARGET_PLATFORM_LAST}))
 if (UNIX)
 elseif (WIN32)
  unset (TARGET_PLATFORM_XP CACHE)
 endif ()
 set (TARGET_PLATFORM_LAST ${TARGET_PLATFORM} CACHE STRING "Updating Target Platform Option" FORCE)
endif ()

if (UNIX)
elseif (WIN32)
 if (${TARGET_PLATFORM} MATCHES "XP")
  if (NOT TARGET_PLATFORM_XP_SUPPORT)
   message (FATAL_ERROR "Windows XP not supported")
  endif ()
  set (TARGET_PLATFORM_XP ON CACHE STRING "target Windows XP")
  mark_as_advanced (FORCE TARGET_PLATFORM_XP)
#  add_definitions (-DTARGET_PLATFORM_XP)
 endif ()
endif ()

