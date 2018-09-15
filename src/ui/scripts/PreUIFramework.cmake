find_package (OpenGL MODULE
              COMPONENTS OpenGL EGL GLX)
if (OPENGL_FOUND)
 set (OPENGL_SUPPORT ON CACHE BOOL "OpenGL support")
 add_definitions (-DOPENGL_SUPPORT)
endif (OPENGL_FOUND)

##########################################

# *NOTE*: FindGTK.cmake doesn't run on WIN32
if (UNIX)
 find_package (GTK MODULE)
 if (GTK_FOUND)
  set (GTK_SUPPORT ON CACHE BOOL "GTK support")
  add_definitions (-DGTK_SUPPORT)
  find_package (GTK3 MODULE)
  if (NOT GTK3_FOUND)
   find_package (GTK2 MODULE)
   if (NOT GTK2_FOUND)
    message (FATAL_ERROR "invalid/unknown GTK version")
   else ()
    set (GTK2_SUPPORT ON CACHE BOOL "GTK2 support")
    add_definitions (-DGTK2_SUPPORT)
   endif (NOT GTK2_FOUND)
  else ()
   set (GTK3_SUPPORT ON CACHE BOOL "GTK3 support")
   add_definitions (-DGTK3_SUPPORT)
  endif (NOT GTK3_FOUND)
 endif (GTK_FOUND)
elseif (WIN32)
# *TODO*: repair win32 module support
# find_package (GTK2 MODULE)
# if (NOT GTK2_FOUND)
#  message (FATAL_ERROR "invalid/unknown GTK version")
# else ()
  set (GTK_FOUND ON CACHE BOOL "GTK found")
  set (GTK_SUPPORT ON CACHE BOOL "GTK support")
  add_definitions (-DGTK_SUPPORT)
  set (GTK3_SUPPORT ON CACHE BOOL "GTK3 support")
  add_definitions (-DGTK3_SUPPORT)
# endif (NOT GTK2_FOUND)
endif ()

##########################################

#unset (CYGWIN)
#unset (MSYS)
unset (CMAKE_CROSSCOMPILING)
if (WIN32)
# message (STATUS "wxWidgets_ROOT_DIR: ${wxWidgets_ROOT_DIR}")
 set (wxWidgets_ROOT_DIR "D:/projects/wxWidgets_win32/wxWidgets-3.0.0")
 set (wxWidgets_LIB_DIR "${wxWidgets_ROOT_DIR}/lib/vc100_dll")
 set (wxWidgets_CONFIGURATION mswu)
 if (CMAKE_BUILD_TYPE STREQUAL Debug)
  set (wxWidgets_CONFIGURATION ${wxWidgets_CONFIGURATION}d)
 endif (CMAKE_BUILD_TYPE STREQUAL Debug)
endif (WIN32)
find_package (wxWidgets MODULE
              COMPONENTS base core adv gl html xml xrc aui)
if (wxWidgets_FOUND)
 set (WXWIDGETS_SUPPORT ON CACHE BOOL "wxWidgets support")
 add_definitions (-DWXWIDGETS_SUPPORT)
endif (wxWidgets_FOUND)

find_package (curses MODULE)
if (CURSES_FOUND)
 set (CURSES_SUPPORT ON CACHE BOOL "curses support")
 add_definitions (-DCURSES_SUPPORT)
endif (CURSES_FOUND)