if (GTK_SUPPORT AND GTK_USE)
 # *NOTE*: the first entry is the default option
 if (UNIX)
  if (GTK3_SUPPORT)
   set (GTK_VERSION "gtk3" CACHE STRING "use GTK3 (default)")
   set (SUPPORTED_GTK_VERSIONS "gtk3")
  elseif (GTK2_SUPPORT)
   set (GTK_VERSION "gtk2" CACHE STRING "use GTK2 (default)")
   set (SUPPORTED_GTK_VERSIONS "gtk2")
  elseif (GTK4_SUPPORT)
   set (GTK_VERSION "gtk4" CACHE STRING "use GTK4 (default)")
   set (SUPPORTED_GTK_VERSIONS "gtk4")
  endif ()
 elseif (WIN32)
  if (GTK2_SUPPORT)
   set (GTK_VERSION "gtk2" CACHE STRING "use GTK2 (default)")
   set (SUPPORTED_GTK_VERSIONS "gtk2")
  elseif (GTK3_SUPPORT)
   set (GTK_VERSION "gtk3" CACHE STRING "use GTK3 (default)")
   set (SUPPORTED_GTK_VERSIONS "gtk3")
  elseif (GTK4_SUPPORT)
   set (GTK_VERSION "gtk4" CACHE STRING "use GTK4 (default)")
   set (SUPPORTED_GTK_VERSIONS "gtk4")
  endif ()
 endif ()
 if (GTK2_SUPPORT)
  set (SUPPORTED_GTK_VERSIONS "${SUPPORTED_GTK_VERSIONS};gtk2")
 endif (GTK2_SUPPORT)
 if (GTK3_SUPPORT)
  set (SUPPORTED_GTK_VERSIONS "${SUPPORTED_GTK_VERSIONS};gtk3")
 endif (GTK3_SUPPORT)
 if (GTK4_SUPPORT)
  set (SUPPORTED_GTK_VERSIONS "${SUPPORTED_GTK_VERSIONS};gtk4")
 endif (GTK4_SUPPORT)
 set_property (CACHE GTK_VERSION PROPERTY STRINGS ${SUPPORTED_GTK_VERSIONS})

 if (NOT DEFINED GTK_VERSION_LAST)
  set (GTK_VERSION_LAST "NotAVersion" CACHE STRING "last GTK version used")
  mark_as_advanced (FORCE GTK_VERSION_LAST)
 endif (NOT DEFINED GTK_VERSION_LAST)
 if (NOT (${GTK_VERSION} MATCHES ${GTK_VERSION_LAST}))
  unset (GTK2_USE CACHE)
  unset (GTK3_USE CACHE)
  unset (GTK4_USE CACHE)
  set (GTK_VERSION_LAST ${GTK_VERSION} CACHE STRING "Updating GTK Version Option" FORCE)
 endif (NOT (${GTK_VERSION} MATCHES ${GTK_VERSION_LAST}))

 if (${GTK_VERSION} MATCHES "gtk2")
  message (STATUS "using gtk2")
  set (GTK2_USE ON CACHE STRING "use GTK2")
  mark_as_advanced (FORCE GTK2_USE)
  add_definitions (-DGTK2_USE)
  set (GTK_INCLUDE_DIRS ${GTK2_INCLUDE_DIRS})
  set (GTK_LIBRARIES ${GTK2_LIBRARIES})
  set (GTK_LIB_DIRS ${GTK2_LIB_DIRS})
 elseif (${GTK_VERSION} MATCHES "gtk3")
  message (STATUS "using gtk3")
  set (GTK3_USE ON CACHE STRING "use GTK3")
  mark_as_advanced (FORCE GTK3_USE)
  add_definitions (-DGTK3_USE)
  set (GTK_INCLUDE_DIRS ${GTK3_INCLUDE_DIRS})
  set (GTK_LIBRARIES ${GTK3_LIBRARIES})
  set (GTK_LIB_DIRS ${GTK3_LIB_DIRS})
 elseif (${GTK_VERSION} MATCHES "gtk4")
  message (STATUS "using gtk4")
  set (GTK4_USE ON CACHE STRING "use GTK4")
  mark_as_advanced (FORCE GTK4_USE)
  add_definitions (-DGTK4_USE)
  set (GTK_INCLUDE_DIRS ${GTK4_INCLUDE_DIRS})
  set (GTK_LIBRARIES ${GTK4_LIBRARIES})
  set (GTK_LIB_DIRS ${GTK4_LIB_DIRS})
 else ()
  message (FATAL_ERROR "invalid/unknown GTK version, aborting")
 endif ()

 if (CMAKE_BUILD_TYPE STREQUAL Debug OR
     CMAKE_BUILD_TYPE STREQUAL ReleaseWithDebugInfo)
  add_definitions (-DG_ERRORCHECK_MUTEXES)
 endif ()
 if (LIBGLADE_SUPPORT)
  set (GTK_INCLUDE_DIRS "${GTK_INCLUDE_DIRS};${LIBGLADE_INCLUDES}")
  set (GTK_LIBRARIES "${GTK_LIBRARIES};${LIBGLADE_LIBRARY}")
  set (GTK_LIB_DIRS "${GTK_LIB_DIRS};${LIBGLADE_LIB_DIR}")
 endif (LIBGLADE_SUPPORT)
 if (GTKGLAREA_SUPPORT)
  set (GTK_INCLUDE_DIRS "${GTK_INCLUDE_DIRS};${GTKGLAREA_INCLUDES}")
  set (GTK_LIBRARIES "${GTK_LIBRARIES};${GTKGLAREA_LIBRARY}")
 endif (GTKGLAREA_SUPPORT)
endif (GTK_SUPPORT AND GTK_USE)

# *NOTE*: unify include dirs and libraries; includers use GTK_INCLUDE_DIRS and GTK_LIBRARIES
# *NOTE*: if no ui has been selected, but gtk is supported, prefer gtk2 over gtk3
if (GTK_SUPPORT AND NOT GTK4_USE AND NOT GTK3_USE AND NOT GTK2_USE)
 message (WARNING "GTK version not selected, preferring GTK2 over GTK3 over GTK4")
 if (GTK2_SUPPORT)
  set (GTK_INCLUDE_DIRS ${GTK2_INCLUDE_DIRS})
  set (GTK_LIBRARIES ${GTK2_LIBRARIES})
  if (LIBGLADE_SUPPORT)
   set (GTK_INCLUDE_DIRS "${GTK_INCLUDE_DIRS};${LIBGLADE_INCLUDES}")
   set (GTK_LIBRARIES "${GTK_LIBRARIES};${LIBGLADE_LIBRARY}")
   set (GTK_LIB_DIRS "${GTK_LIB_DIRS};${LIBGLADE_LIB_DIR}")
  endif (LIBGLADE_SUPPORT)
  if (GTKGLAREA_SUPPORT)
   set (GTK_INCLUDE_DIRS "${GTK_INCLUDE_DIRS};${GTKGLAREA_INCLUDES}")
   set (GTK_LIBRARIES "${GTK_LIBRARIES};${GTKGLAREA_LIBRARY}")
  endif (GTKGLAREA_SUPPORT)
 elseif (GTK3_SUPPORT)
  set (GTK_INCLUDE_DIRS ${GTK3_INCLUDE_DIRS})
  set (GTK_LIBRARIES ${GTK3_LIBRARIES})
  if (LIBGLADE_SUPPORT)
   set (GTK_INCLUDE_DIRS "${GTK_INCLUDE_DIRS};${LIBGLADE_INCLUDES}")
   set (GTK_LIBRARIES "${GTK_LIBRARIES};${LIBGLADE_LIBRARY}")
   set (GTK_LIB_DIRS "${GTK_LIB_DIRS};${LIBGLADE_LIB_DIR}")
  endif (LIBGLADE_SUPPORT)
 elseif (GTK4_SUPPORT)
  set (GTK_INCLUDE_DIRS ${GTK4_INCLUDE_DIRS})
  set (GTK_LIBRARIES ${GTK4_LIBRARIES})
  set (GTK_LIB_DIRS ${GTK4_LIB_DIRS})
 endif ()
endif (GTK_SUPPORT AND NOT GTK4_USE AND NOT GTK3_USE AND NOT GTK2_USE)
#message (STATUS "GTK_INCLUDE_DIRS: ${GTK_INCLUDE_DIRS}")
#message (STATUS "GTK_LIBRARIES: ${GTK_LIBRARIES}")
