include (FindPkgConfig)

if (UNIX)
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
   
   #set (PKG_GTK_2_INCLUDE_DIRS "/usr/include/gtk-2.0;/usr/lib64/gtk-2.0/include;/usr/include/pango-1.0;/usr/include/glib-2.0;/usr/lib64/glib-2.0/include;/usr/include/fribidi;/usr/include/harfbuzz;/usr/include/freetype2;/usr/include/libpng16;/usr/include/uuid;/usr/include/cairo;/usr/include/pixman-1;/usr/include/gdk-pixbuf-2.0;/usr/include/libmount;/usr/include/blkid;/usr/include/atk-1.0")
#   message (STATUS "PKG_GTK_2_INCLUDE_DIRS: ${PKG_GTK_2_INCLUDE_DIRS}")
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
   option (LIBGLADE_SUPPORT "enable libglade support" OFF)
  endif (PKG_GLADE_FOUND)
  if (LIBGLADE_SUPPORT)
   add_definitions (-DLIBGLADE_SUPPORT)
   set (LIBGLADE_USE OFF CACHE BOOL "use libglade")
  endif (LIBGLADE_SUPPORT)
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
 if (VCPKG_SUPPORT)
#  find_package (GTK)
  set (PKG_CONFIG_USE_CMAKE_PREFIX_PATH 1)
  pkg_check_modules (PKG_GTK_4 gtk4)
#  if (GTK_FOUND)
  if (PKG_GTK_4_FOUND)
   set (GTK4_SUPPORT ON CACHE BOOL "GTK4 support")
   add_definitions (-DGTK4_SUPPORT)
#   set (GTK4_INCLUDE_DIRS ${GTK_INCLUDE_DIRS})
#   set (GTK4_LIBRARIES ${GTK_LIBRARIES})
   set (GTK4_INCLUDE_DIRS ${PKG_GTK_4_INCLUDE_DIRS})
   set (GTK4_LIBRARIES ${PKG_GTK_4_LIBRARIES})
   set (GTK4_LIB_DIRS "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/lib")

   message (STATUS "found Gtk version 4")
   set (GTK_GL_FOUND TRUE)
  else ()
   message (WARNING "could not find Gtk4, continuing")
#  endif (GTK_FOUND)
  endif (PKG_GTK_4_FOUND)
 else ()
# *TODO*: repair win32 module support
  find_library (GTK2_LIBRARY gtk-win32-2.0.lib
                PATHS $ENV{LIB_ROOT}/gtk2
                PATH_SUFFIXES lib
                DOC "searching for gtk-win32-2.0.lib")
  if (GTK2_LIBRARY)
   set (GTK2_SUPPORT ON CACHE BOOL "GTK2 support")
   add_definitions (-DGTK2_SUPPORT)
   set (GTK2_INCLUDE_DIRS "$ENV{LIB_ROOT}/gtk2/include/atk-1.0;$ENV{LIB_ROOT}/gtk2/include/gdk-pixbuf-2.0;$ENV{LIB_ROOT}/gtk2/include/cairo;$ENV{LIB_ROOT}/gtk2/include/pango-1.0;$ENV{LIB_ROOT}/gtk2/lib/glib-2.0/include;$ENV{LIB_ROOT}/gtk2/include/glib-2.0;$ENV{LIB_ROOT}/gtk2/lib/gtk-2.0/include;$ENV{LIB_ROOT}/gtk2/include/gtk-2.0")
   set (GTK2_LIBRARIES "$ENV{LIB_ROOT}/gtk2/lib/gio-2.0.lib;$ENV{LIB_ROOT}/gtk2/lib/glib-2.0.lib;$ENV{LIB_ROOT}/gtk2/lib/gobject-2.0.lib;$ENV{LIB_ROOT}/gtk2/lib/gthread-2.0.lib;$ENV{LIB_ROOT}/gtk2/lib/gdk_pixbuf-2.0.lib;$ENV{LIB_ROOT}/gtk2/lib/gdk-win32-2.0.lib;$ENV{LIB_ROOT}/gtk2/lib/gtk-win32-2.0.lib;$ENV{LIB_ROOT}/gtk2/lib/pango-1.0.lib;$ENV{LIB_ROOT}/gtk2/lib/cairo.lib")

   message (STATUS "found Gtk version 2")
  else ()
   message (WARNING "could not find Gtk2 (was: \"gtk-win32-2.0.lib\"), continuing")
  endif (GTK2_LIBRARY)

  find_library (GTK3_LIBRARY gtk-win32-3.0.lib
                PATHS $ENV{LIB_ROOT}/gtk3
                PATH_SUFFIXES lib
                DOC "searching for gtk-win32-3.0.lib")
  if (GTK3_LIBRARY)
   set (GTK3_SUPPORT ON CACHE BOOL "GTK3 support")
   add_definitions (-DGTK3_SUPPORT)
   set (GTK3_INCLUDE_DIRS "$ENV{LIB_ROOT}/gtk3/include/atk-1.0;$ENV{LIB_ROOT}/gtk3/include/gdk-pixbuf-2.0;$ENV{LIB_ROOT}/gtk3/include/cairo;$ENV{LIB_ROOT}/gtk3/include/pango-1.0;$ENV{LIB_ROOT}/gtk3/lib/glib-2.0/include;$ENV{LIB_ROOT}/gtk3/include/glib-2.0;$ENV{LIB_ROOT}/gtk3/include/gtk-3.0")
   set (GTK3_LIBRARIES "$ENV{LIB_ROOT}/gtk3/lib/gio-2.0.lib;$ENV{LIB_ROOT}/gtk3/lib/glib-2.0.lib;$ENV{LIB_ROOT}/gtk3/lib/gobject-2.0.lib;$ENV{LIB_ROOT}/gtk3/lib/gthread-2.0.lib;$ENV{LIB_ROOT}/gtk3/lib/gdk_pixbuf-2.0.lib;$ENV{LIB_ROOT}/gtk3/lib/gdk-win32-3.0.lib;$ENV{LIB_ROOT}/gtk3/lib/gtk-win32-3.0.lib;$ENV{LIB_ROOT}/gtk3/lib/pango-1.0.lib;$ENV{LIB_ROOT}/gtk3/lib/cairo.lib")

   message (STATUS "found Gtk version 3")

 # *TODO*: Gtk < 3.16 do not have native opengl support
   set (GTK_GL_FOUND TRUE)
  else ()
   message (WARNING "could not find Gtk3 (was: \"gtk-win32-3.0.lib\"), continuing")
  endif (GTK3_LIBRARY)
 endif (VCPKG_SUPPORT)

 if (GTK2_SUPPORT OR GTK3_SUPPORT OR GTK4_SUPPORT)
  set (GTK_SUPPORT ON CACHE BOOL "GTK support")
  add_definitions (-DGTK_SUPPORT)
 endif (GTK2_SUPPORT OR GTK3_SUPPORT OR GTK4_SUPPORT)

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
 if (GTK2_SUPPORT)
  set (LIBGLADE_DEFAULT OFF)
  find_library (LIBGLADE_LIBRARY
                NAMES libglade-2.0.dll.a libglade-2.0.lib
                PATHS $ENV{LIB_ROOT}/libglade
                PATH_SUFFIXES lib
                DOC "searching for libglade-2.0.dll.a"
                NO_DEFAULT_PATH)
  if (LIBGLADE_LIBRARY)
   set (LIBGLADE_DEFAULT ON)
   set (LIBGLADE_USE OFF CACHE BOOL "use libglade")
  endif (LIBGLADE_LIBRARY)
  option (LIBGLADE_SUPPORT "enable libglade support" ${LIBGLADE_DEFAULT})
  if (LIBGLADE_SUPPORT)
   add_definitions (-DLIBGLADE_SUPPORT)
   set (LIBGLADE_INCLUDES $ENV{LIB_ROOT}/libglade/include/libglade-2.0)
  endif (LIBGLADE_SUPPORT)
 endif (GTK2_SUPPORT)
endif () # UNIX || WIN32

# gtk opengl support
if (GTK_SUPPORT AND OPENGL_SUPPORT)
 if (NOT GTK_GL_FOUND)
  if (UNIX)
   if (GTK3_FOUND)
# *TODO*: find out the distribution release versions that require this package
    pkg_check_modules (PKG_GTKGL3 gtkgl-3.0)
    if (PKG_GTKGL3_FOUND)
     add_definitions (-DGTKGL_SUPPORT)
     option (GTKGL_SUPPORT "enable GTK GL support" ON)
    endif (PKG_GTKGL3_FOUND)
   endif (GTK3_FOUND)

   if (GTK2_FOUND)
# *TODO*: find out the distribution release versions that require this package
    pkg_check_modules (PKG_GTKGL2 gtkgl-2.0)
    if (PKG_GTKGL2_FOUND)
     add_definitions (-DGTKGL_SUPPORT)
     option (GTKGL_SUPPORT "enable GTK GL support" ON)
    endif (PKG_GTKGL2_FOUND)
   endif (GTK2_FOUND)

   if (GTK_FOUND)
#    pkg_check_modules (PKG_GTKGLEXT gtkglext-libs gtkglext-devel)
    pkg_check_modules (PKG_GTKGLEXT gdkglext-1.0 gtkglext-1.0)
    if (PKG_GTKGLEXT_FOUND)
     add_definitions (-DGTKGL_SUPPORT)
     option (GTKGL_SUPPORT "enable GTK GL support" ON)
    endif (PKG_GTKGLEXT_FOUND)
   endif (GTK_FOUND)
  endif (UNIX)

  if (NOT GTKGL_SUPPORT)
   set (GTKGLAREA_DEFAULT OFF)
   if (UNIX)
    if (GTK3_FOUND)
     # *NOTE*: check out the 'master' branch for gtk3-based applications
     set (GTKGLAREA_LIB_FILE libgtkgl-3.0.so)
    elseif (GTK2_FOUND)
     # *NOTE*: to use gtkglarea on gtk2, check out the 'gtkglarea-2' branch
     #         of the project (instead of 'master')
     set (GTKGLAREA_LIB_FILE libgtkgl-2.0.so)
    endif ()
    find_library (GTKGLAREA_LIBRARY ${GTKGLAREA_LIB_FILE}
                  PATHS $ENV{LIB_ROOT}/gtkglarea/gtkgl
                  PATH_SUFFIXES .libs
                  DOC "searching for ${GTKGLAREA_LIB_FILE}")
   elseif (WIN32)
    set (GTKGLAREA_LIB_FILE gtkglarea.lib)
    get_filename_component (BUILD_PATH_SUFFIX ${CMAKE_BINARY_DIR} NAME)
    find_library (GTKGLAREA_LIBRARY ${GTKGLAREA_LIB_FILE}
                  PATHS $ENV{LIB_ROOT}/gtkglarea/${BUILD_PATH_SUFFIX}
                  PATH_SUFFIXES ${CMAKE_BUILD_TYPE}
                  DOC "searching for ${GTKGLAREA_LIB_FILE}")
   endif ()
   if (GTKGLAREA_LIBRARY)
    set (GTKGLAREA_DEFAULT ON)
    set (GTKGLAREA_INCLUDES $ENV{LIB_ROOT}/gtkglarea)
   endif (GTKGLAREA_LIBRARY)
   option (GTKGLAREA_SUPPORT "enable GtkGLArea support" ${GTKGLAREA_DEFAULT})
   if (GTKGLAREA_SUPPORT)
    add_definitions (-DGTKGL_SUPPORT)
    add_definitions (-DGTKGLAREA_SUPPORT)
    option (GTKGL_SUPPORT "enable GTK GL support" ON)
   endif (GTKGLAREA_SUPPORT)
  endif (NOT GTKGL_SUPPORT)
 else ()
  add_definitions (-DGTKGL_SUPPORT)
  option (GTKGL_SUPPORT "enable GTK GL support" ON)
 endif (NOT GTK_GL_FOUND)
endif (GTK_SUPPORT AND OPENGL_SUPPORT)

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
 set (ENV{wxWidgets_ROOT_DIR} "$ENV{LIB_ROOT}")
# set (wxWidgets_LIB_DIR "${wxWidgets_ROOT_DIR}/lib")
 set (wxWidgets_CONFIGURATION mswu)
 if (CMAKE_BUILD_TYPE STREQUAL Debug)
# *WARNING*: linking against debug versions incurs dependencies to the debug
#            vcrt libraries; these may not exist on your platform
  set (wxWidgets_CONFIGURATION ${wxWidgets_CONFIGURATION}d)
 endif (CMAKE_BUILD_TYPE STREQUAL Debug)
endif ()
if (CMAKE_BUILD_TYPE STREQUAL Debug)
 add_definitions (-DwxUSE_LOG_DEBUG)
endif (CMAKE_BUILD_TYPE STREQUAL Debug)
find_package (wxWidgets MODULE
              COMPONENTS aui base core adv gl net html xml xrc)
if (wxWidgets_FOUND)
 message (STATUS "wxWidgets found: ${wxWidgets_ROOT_DIR}")
# include (${wxWidgets_USE_FILE})
# set (wxWidgets_INCLUDE_DIRS "${wxWidgets_ROOT_DIR}/include")
 message (STATUS "wxWidgets_INCLUDE_DIRS: ${wxWidgets_INCLUDE_DIRS}")
 set (WXWIDGETS_SUPPORT ON CACHE BOOL "wxWidgets support")
 add_definitions (-DWXWIDGETS_SUPPORT)

 if (WIN32)
  add_definitions (-D__WXMSW__)
  add_definitions (-DWXUSINGDLL)
  add_definitions (-DWX_HIDE_MODE_T)
  add_definitions (-DUNICODE -D_UNICODE)
  add_definitions (-D_CRT_SECURE_NO_WARNINGS)
  if (MSVC)
   add_definitions (-DwxMSVC_VERSION_AUTO)
  endif (MSVC)
 endif (WIN32)
 add_definitions (-DwxUSE_GUI=1)
 if (CMAKE_BUILD_TYPE STREQUAL Debug)
  add_definitions (-D__WXDEBUG__)
 endif (CMAKE_BUILD_TYPE STREQUAL Debug)
endif (wxWidgets_FOUND)

##########################################

if (WIN32)
# unset (CURSES_INCLUDE_PATH)
 set (CURSES_INCLUDE_PATH "$ENV{LIB_ROOT}\\PDCurses" CACHE STRING "curses include path")
endif (WIN32)
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
                PATHS $ENV{LIB_ROOT}/PDCurses
                PATH_SUFFIXES wincon
                DOC "searching for pdcurses.lib")
  if (NOT CURSES_LIBRARY)
   message (WARNING "could not find pdcurses.lib, continuing")
  else ()
# *NOTE*: pdcurses.lib incorporates panel.lib
   set (PANEL_LIBRARY ${CURSES_LIBRARY})
  endif (NOT CURSES_LIBRARY)
  if (CURSES_LIBRARY AND PANEL_LIBRARY)
   set (CURSES_SUPPORT ON CACHE BOOL "curses support")
   add_definitions (-DCURSES_SUPPORT)
  endif (CURSES_LIBRARY AND PANEL_LIBRARY)
 endif ()
endif (CURSES_FOUND)
