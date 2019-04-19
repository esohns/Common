if (UNIX)
 include (FindPkgConfig)

# set (OpenGL_Components "OpenGL EGL GLX")
 set (OpenGL_Components "OpenGL")
elseif (WIN32)
 set (OpenGL_Components "OpenGL")
endif ()
find_package (OpenGL MODULE
              COMPONENTS ${OpenGL_Components})
if (OPENGL_FOUND)
 set (OPENGL_SUPPORT ON CACHE BOOL "OpenGL support")
 add_definitions (-DOPENGL_SUPPORT)
endif (OPENGL_FOUND)
# *TODO*
#if (WIN32)
# set (OPENGL_SUPPORT ON CACHE BOOL "OpenGL support")
# add_definitions (-DOPENGL_SUPPORT)
#endif (WIN32)

##########################################

if (UNIX)
# *NOTE*: prefer find_package() over pkg_check_modules()
# *TODO*: FindGTK3.cmake is currently badly broken; use pkg_check_modules
# find_package (GTK3 MODULE)
# if (GTK3_QUARTZ_FOUND OR GTK3_X11_FOUND OR GTK3_WAYLAND_FOUND)
# pkg_check_modules (PKG_GTK3 gmodule-2.0 gthread-2.0 gtk+-3.0)
 pkg_check_modules (PKG_GTK_3 gtk+-3.0)
 if (PKG_GTK_3_FOUND)
  message (STATUS "found gtk3")
  set (GTK3_FOUND ON)

  # *TODO*: this is broken; PKG_GTK_3_CFLAGS and PKG_GTK_3_INCLUDE_DIRS are not set;
  #         only PKG_GTK_3_LIBRARIES is
  #   message (STATUS "PKG_GTK3_CFLAGS: ${PKG_GTK_3_CFLAGS}")
  #   message (STATUS "PKG_GTK3_INCLUDE_DIRS: ${PKG_GTK_3_INCLUDE_DIRS}")
  #   message (STATUS "PKG_GTK3_LIBRARIES: ${PKG_GTK_3_LIBRARIES}")
    set (pkgconfig_cmd "pkg-config")
    set (pkgconfig_arg "gtk+-3.0")
    unset (PKG_GTK_3_INCLUDE_DIRS)
    unset (PKG_GTK_3_ERROR)
    unset (PKG_GTK_3_RESULT)
  # *TODO*: this is totally broken; ('--libs' works, but '--cflags'|'--cflags-only-I' do not)
  #   execute_process (COMMAND ${pkgconfig_cmd} --cflags-only-I ${pkgconfig_arg}
  #                    RESULT_VARIABLE PKG_GTK3_RESULT
  #                    OUTPUT_VARIABLE PKG_GTK3_INCLUDE_DIRS
  #                    ERROR_VARIABLE PKG_GTK3_ERROR
  #                    OUTPUT_STRIP_TRAILING_WHITESPACE)
  #   exec_program (${pkgconfig_cmd}
  #                 ARGS --cflags-only-I ${pkgconfig_arg}
  #                 OUTPUT_VARIABLE PKG_GTK3_INCLUDE_DIRS
  #                 RETURN_VALUE PKG_GTK3_RESULT)
  #   if (NOT ${PKG_GTK3_RESULT} EQUAL 0)
  #    message (FATAL_ERROR "Command \"pkg-config --cflags-only-I gtk+-3.0\" failed with output:\n${PKG_GTK3_ERROR}\n${PKG_GTK3_RESULT}, aborting")
  #    message (FATAL_ERROR "Command \"pkg-config --cflags-only-I gtk+-3.0\" failed with output:\n${PKG_GTK3_INCLUDE_DIRS}\n${PKG_GTK3_RESULT}, aborting")
  #   endif (NOT ${PKG_GTK3_RESULT} EQUAL 0)
  #   message (STATUS "PKG_GTK3_INCLUDE_DIRS: ${PKG_GTK3_INCLUDE_DIRS}")
    set (PKG_GTK_3_INCLUDE_DIRS "-I/usr/include/gtk-3.0 -I/usr/include/pango-1.0 -I/usr/include/glib-2.0 -I/usr/lib64/glib-2.0/include -I/usr/include/fribidi -I/usr/include/cairo -I/usr/include/pixman-1 -I/usr/include/freetype2 -I/usr/include/libpng16 -I/usr/include/uuid -I/usr/include/harfbuzz -I/usr/include/gdk-pixbuf-2.0 -I/usr/include/gio-unix-2.0/ -I/usr/include/libdrm -I/usr/include/atk-1.0 -I/usr/include/at-spi2-atk/2.0 -I/usr/include/at-spi-2.0 -I/usr/include/dbus-1.0 -I/usr/lib64/dbus-1.0/include")
    string (REGEX REPLACE "-I([^ ]+) " "\\1;" PKG_GTK_3_INCLUDE_DIRS "${PKG_GTK_3_INCLUDE_DIRS}")
    string (REGEX REPLACE "-I([^ ]+)" "\\1" PKG_GTK_3_INCLUDE_DIRS "${PKG_GTK_3_INCLUDE_DIRS}")
  #  message (STATUS "PKG_GTK3_INCLUDE_DIRS: ${PKG_GTK3_INCLUDE_DIRS}")
#  message (STATUS "GTK3_LIBRARIES: ${GTK3_LIBRARIES}")

  set (GTK3_INCLUDE_DIRS ${PKG_GTK_3_INCLUDE_DIRS})
  set (GTK3_LIBRARIES ${PKG_GTK_3_LIBRARIES})

# endif (GTK3_QUARTZ_FOUND OR GTK3_X11_FOUND OR GTK3_WAYLAND_FOUND)
 endif (PKG_GTK_3_FOUND)
# find_package (GTK2 MODULE)
 find_package (GTK MODULE)

 if (NOT GTK3_FOUND)
  pkg_check_modules (PKG_GTK_3 gmodule-2.0 gthread-2.0 gtk+-3.0)
  if (PKG_GTK_3_FOUND)
   message (STATUS "found gtk3")
   set (GTK3_FOUND TRUE)

   set (GTK3_INCLUDE_DIRS ${PKG_GTK_3_INCLUDE_DIRS})
   set (GTK3_LIBRARIES ${PKG_GTK_3_LIBRARIES})
  endif (PKG_GTK_3_FOUND)
 endif (NOT GTK3_FOUND)

 if (NOT GTK2_FOUND)
  pkg_check_modules (PKG_GTK_2 gmodule-2.0 gthread-2.0 gtk+-2.0)
  if (PKG_GTK_2_FOUND)
   message (STATUS "found gtk2")
   set (GTK2_FOUND TRUE)
   
   set (GTK2_INCLUDE_DIRS ${PKG_GTK_2_INCLUDE_DIRS})
   set (GTK2_LIBRARIES ${PKG_GTK_2_LIBRARIES})
  endif (PKG_GTK_2_FOUND)
 endif (NOT GTK2_FOUND)
# if (NOT GTK_FOUND)
#  pkg_check_modules (PKG_GTK gmodule-1.0 gthread-1.0 gtk+)
#  if (PKG_GTK_FOUND)
#   set (GTK_FOUND TRUE)
   
#   set (GTK_INCLUDE_DIRS ${PKG_GTK_INCLUDE_DIRS})
#   set (GTK_LIBRARIES ${PKG_GTK_LIBRARIES})
#  endif (PKG_GTK_FOUND)
# endif (NOT GTK_FOUND)

 if (GTK3_FOUND)
  set (GTK3_SUPPORT ON CACHE BOOL "GTK3 support")
  add_definitions (-DGTK3_SUPPORT)
 endif (GTK3_FOUND)

 if (GTK2_FOUND)
  set (GTK2_SUPPORT ON CACHE BOOL "GTK2 support")
  add_definitions (-DGTK2_SUPPORT)
 endif (GTK2_FOUND)

 if (GTK_FOUND OR GTK2_FOUND OR GTK3_FOUND)
  set (GTK_SUPPORT ON CACHE BOOL "GTK support")
  add_definitions (-DGTK_SUPPORT)
 else ()
  message (WARNING "failed to detect GTK, continuing")
 endif (GTK_FOUND OR GTK2_FOUND OR GTK3_FOUND)

# libglade for (early versions of-) gtk2
 if (GTK_FOUND OR GTK2_FOUND)
# *NOTE*: early versions of gtk do not support GtkBuilder
# *TODO*: --> find the version number(s) and auto-enable this feature for these
#             target systems
# *TODO*: retrieve the available gtk version number(s) from the pkg-config
#         output and pre-set this option accordingly
  pkg_check_modules (PKG_GLADE libglade-2.0)
  if (PKG_GLADE_FOUND)
   set (LIBGLADE_SUPPORT ON CACHE BOOL "libglade support")
   add_definitions (-DLIBGLADE_SUPPORT)
   option (LIBGLADE_SUPPORT "enable libglade support" OFF)
  endif (PKG_GLADE_FOUND)
  set (LIBGLADE_USE OFF CACHE BOOL "use libglade")
 endif (GTK_FOUND OR GTK2_FOUND)

 if (GTK3_FOUND)
  if (GTK3_SUPPORTS_QUARTZ)
   message (STATUS "found GTK 3 Quartz support")
   set (GTK3_INCLUDE_DIRS ${GTK3_QUARTZ_INCLUDE_DIRS})
  elseif (GTK3_SUPPORTS_X11)
   message (STATUS "found GTK 3 X11 support")
   set (GTK3_INCLUDE_DIRS ${GTK3_X11_INCLUDE_DIRS})
  elseif (GTK3_SUPPORTS_WAYLAND)
   message (STATUS "found GTK 3 Wayland support")
   set (GTK3_INCLUDE_DIRS ${GTK3_WAYLAND_INCLUDE_DIRS})
  endif ()

# *TODO*: Gtk < 3.16 do not have native opengl support
  set (GTK_GL_FOUND TRUE)
 endif (GTK3_FOUND)
elseif (WIN32)
# *TODO*: repair win32 module support
 find_library (GTK2_LIBRARY gtk-win32-2.0.lib
               HINTS $ENV{LIB_ROOT}/gtk2
               PATHS $ENV{LIB_ROOT}/gtk2
               PATH_SUFFIXES lib
               DOC "searching for gtk-win32-2.0.lib"
               NO_DEFAULT_PATH)
 if (GTK2_LIBRARY)
  set (GTK2_SUPPORT ON CACHE BOOL "GTK2 support")
  add_definitions (-DGTK2_SUPPORT)
  set (GTK2_INCLUDE_DIRS "$ENV{GTKDIR}/include/atk-1.0;$ENV{GTKDIR}/include/gdk-pixbuf-2.0;$ENV{GTKDIR}/include/cairo;$ENV{GTKDIR}/include/pango-1.0;$ENV{GTKDIR}/lib/glib-2.0/include;$ENV{GTKDIR}/include/glib-2.0;$ENV{GTKDIR}/lib/gtk-2.0/include;$ENV{GTKDIR}/include/gtk-2.0")
  set (GTK2_LIBRARIES "$ENV{GTKDIR}/lib/gio-2.0.lib;$ENV{GTKDIR}/lib/glib-2.0.lib;$ENV{GTKDIR}/lib/gobject-2.0.lib;$ENV{GTKDIR}/lib/gthread-2.0.lib;$ENV{GTKDIR}/lib/gdk_pixbuf-2.0.lib;$ENV{GTKDIR}/lib/gdk-win32-2.0.lib;$ENV{GTKDIR}/lib/gtk-win32-2.0.lib;$ENV{GTKDIR}/lib/pango-1.0.lib;$ENV{GTKDIR}/lib/cairo.lib")

  message (STATUS "found Gtk version 2")
 else ()
  message (WARNING "could not find Gtk2 (was: \"gtk-win32-2.0.lib\"), continuing")
 endif ()

 find_library (GTK3_LIBRARY gtk-win32-3.0.lib
               HINTS $ENV{LIB_ROOT}/gtk3
               PATHS $ENV{LIB_ROOT}/gtk3
               PATH_SUFFIXES lib
               DOC "searching for gtk-win32-3.0.lib"
               NO_DEFAULT_PATH)
 if (GTK3_LIBRARY)
  set (GTK3_SUPPORT ON CACHE BOOL "GTK3 support")
  add_definitions (-DGTK3_SUPPORT)
  set (GTK3_INCLUDE_DIRS "$ENV{GTK3DIR}/include/atk-1.0;$ENV{GTK3DIR}/include/gdk-pixbuf-2.0;$ENV{GTK3DIR}/include/cairo;$ENV{GTK3DIR}/include/pango-1.0;$ENV{GTK3DIR}/lib/glib-2.0/include;$ENV{GTK3DIR}/include/glib-2.0;$ENV{GTK3DIR}/include/gtk-3.0")
  set (GTK3_LIBRARIES "$ENV{GTK3DIR}/lib/gio-2.0.lib;$ENV{GTK3DIR}/lib/glib-2.0.lib;$ENV{GTK3DIR}/lib/gobject-2.0.lib;$ENV{GTK3DIR}/lib/gthread-2.0.lib;$ENV{GTK3DIR}/lib/gdk_pixbuf-2.0.lib;$ENV{GTK3DIR}/lib/gdk-win32-3.0.lib;$ENV{GTK3DIR}/lib/gtk-win32-3.0.lib;$ENV{GTK3DIR}/lib/pango-1.0.lib;$ENV{GTK3DIR}/lib/cairo.lib")

  message (STATUS "found Gtk version 3")
 else ()
  message (WARNING "could not find Gtk3 (was: \"gtk-win32-3.0.lib\"), continuing")
 endif ()

 if (GTK2_SUPPORT OR GTK3_SUPPORT)
  set (GTK_SUPPORT ON CACHE BOOL "GTK support")
  add_definitions (-DGTK_SUPPORT)
 endif (GTK2_SUPPORT OR GTK3_SUPPORT)

# *NOTE*: early versions of gtk do not support GtkBuilder
# *TODO*: --> find the version number(s) and auto-enable this feature for these
#             target systems
# *NOTE*: on Win32 systems, the (binary) gtk3 applications have linkage issues
#         with the latest (2.6.4) libglade distribution (specifically, the
#         libxml2 dll cannot be easily shared between these two packages; also,
#         the 'embedded' library names do not match ('lib'-prefixes))
#         --> disable libglade support when targeting gtk3
# *TODO*: retrieve the available gtk version number(s) from the pkg-config
#         output and pre-set this option accordingly
#  set (LIBGLADE_SUPPORT OFF CACHE BOOL "libglade support")
#  add_definitions (-DLIBGLADE_SUPPORT)
#  option (LIBGLADE_SUPPORT "enable libglade support" OFF)
#  set (LIBGLADE_USE OFF CACHE BOOL "use libglade")
# endif (NOT GTK2_FOUND)
endif () # UNIX || WIN32

# gtk opengl support
if (GTK_SUPPORT AND OPENGL_FOUND)
 if (NOT GTK_GL_FOUND)
  if (UNIX)
   if (GTK3_FOUND)
# *TODO*: found out the distribution release versions that require this package
    pkg_check_modules (PKG_GTKGL3 gtkgl-3.0)
    if (PKG_GTKGL3_FOUND)
     set (GTKGL_SUPPORT ON CACHE BOOL "GTK GL support")
     add_definitions (-DGTKGL_SUPPORT)
     option (GTKGL_SUPPORT "enable GTK GL support" ON)
    endif (PKG_GTKGL3_FOUND)
   endif (GTK3_FOUND)
    
   if (GTK2_FOUND)
# *TODO*: found out the distribution release versions that require this package
    pkg_check_modules (PKG_GTKGL2 gtkgl-2.0)
    if (PKG_GTKGL2_FOUND)
     set (GTKGL_SUPPORT ON CACHE BOOL "GTK GL support")
     add_definitions (-DGTKGL_SUPPORT)
     option (GTKGL_SUPPORT "enable GTK GL support" ON)
    endif (PKG_GTKGL2_FOUND)
   endif (GTK2_FOUND)
    
   if (GTK_FOUND)
#    pkg_check_modules (PKG_GTKGLEXT gtkglext-libs gtkglext-devel)
    pkg_check_modules (PKG_GTKGLEXT gdkglext-1.0 gtkglext-1.0)
    if (PKG_GTKGLEXT_FOUND)
     set (GTKGL_SUPPORT ON CACHE BOOL "GTK GL support")
     add_definitions (-DGTKGL_SUPPORT)
     option (GTKGL_SUPPORT "enable GTK GL support" ON)
    endif (PKG_GTKGLEXT_FOUND)
   endif (GTK_FOUND)
  endif (UNIX)
 endif (NOT GTK_GL_FOUND)

 set (GTKGLAREA_DEFAULT OFF)
# *IMPORTANT NOTE*: to use gtkglarea on gtk2, check out the 'gtkglarea-2' branch
#                   of the project (instead of 'master')
 option (GTKGLAREA_SUPPORT "enable GtkGLArea support" ${GTKGLAREA_DEFAULT})
 if (GTKGLAREA_SUPPORT)
  add_definitions (-DGTKGLAREA_SUPPORT)
 endif (GTKGLAREA_SUPPORT)

 set (GTKGL_SUPPORT ON CACHE BOOL "GTK GL support")
 add_definitions (-DGTKGL_SUPPORT)
 option (GTKGL_SUPPORT "enable GTK OpenGL support" ON)
endif (GTK_SUPPORT AND OPENGL_FOUND)

##########################################

find_package (Qt5 CONFIG COMPONENTS Widgets)
if (Qt5_FOUND)
# message (STATUS "qt found")
 set (QT_SUPPORT ON CACHE BOOL "qt support")
 add_definitions (-DQT_SUPPORT)
endif (Qt5_FOUND)

##########################################

#unset (CYGWIN)
#unset (MSYS)
unset (CMAKE_CROSSCOMPILING)
if (CMAKE_BUILD_TYPE STREQUAL Debug)
 set (wxWidgets_USE_DEBUG ON)
endif (CMAKE_BUILD_TYPE STREQUAL Debug)
set (wxWidgets_USE_UNICODE OFF)
if (UNIX)
# set (wxWidgets_CONFIG_OPTIONS --toolkit=base --prefix=/usr)
elseif (WIN32)
# message (STATUS "wxWidgets_ROOT_DIR: ${wxWidgets_ROOT_DIR}")
 set (wxWidgets_ROOT_DIR "$ENV{LIB_ROOT}/wxwidgets")
 set (wxWidgets_LIB_DIR "${wxWidgets_ROOT_DIR}/lib/vc120_dll")
 set (wxWidgets_CONFIGURATION mswu)
 if (CMAKE_BUILD_TYPE STREQUAL Debug)
# *WARNING*: linking against debug versions incurs dependencies to the debug
#            vcrt libraries; these may not exist on your platform
#  set (wxWidgets_CONFIGURATION ${wxWidgets_CONFIGURATION}d)
 endif (CMAKE_BUILD_TYPE STREQUAL Debug)
endif ()
if (CMAKE_BUILD_TYPE STREQUAL Debug)
 add_definitions (-DwxUSE_LOG_DEBUG)
endif (CMAKE_BUILD_TYPE STREQUAL Debug)
find_package (wxWidgets MODULE
              COMPONENTS aui base core adv gl net html xml xrc)
if (wxWidgets_FOUND)
 message (STATUS "wxWidgets found: ${wxWidgets_ROOT_DIR}")
 include (${wxWidgets_USE_FILE})
 set (WXWIDGETS_SUPPORT ON CACHE BOOL "wxWidgets support")
 add_definitions (-DWXWIDGETS_SUPPORT)
endif (wxWidgets_FOUND)

##########################################

find_package (Curses MODULE)
if (CURSES_FOUND)
 set (CURSES_SUPPORT ON CACHE BOOL "curses support")
 add_definitions (-DCURSES_SUPPORT)
else ()
 if (UNIX)
  pkg_check_modules (PKG_CURSES ncurses panel)
  if (PKG_CURSES_FOUND)
   set (CURSES_SUPPORT ON CACHE BOOL "curses support")
   add_definitions (-DCURSES_SUPPORT)
  endif (PKG_CURSES_FOUND)
 elseif (WIN32)
  find_library (CURSES_LIBRARY pdcurses.lib
                PATHS ${PROJECT_SOURCE_DIR}/../pdcurses
                PATH_SUFFIXES win32
                DOC "searching for pdcurses.lib")
  if (NOT CURSES_LIBRARY)
   message (WARNING "could not find pdcurses.lib, continuing")
  endif (NOT CURSES_LIBRARY)
  find_library (PANEL_LIBRARY panel.lib
                PATHS ${PROJECT_SOURCE_DIR}/../pdcurses
                PATH_SUFFIXES win32
                DOC "searching for panel.lib")
  if (NOT PANEL_LIBRARY)
   message (WARNING "could not find panel.lib, continuing")
  endif (NOT PANEL_LIBRARY)
  if (CURSES_LIBRARY AND PANEL_LIBRARY)
   set (CURSES_SUPPORT ON CACHE BOOL "curses support")
   add_definitions (-DCURSES_SUPPORT)
  endif (CURSES_LIBRARY AND PANEL_LIBRARY)
 endif ()
endif (CURSES_FOUND)

