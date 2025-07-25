include (FindPkgConfig)
set (CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_LIST_DIR}/../../gl/scripts")
include (OpenGL)

##########################################

if (UNIX)
# *NOTE*: prefer find_package() over pkg_check_modules()
 unset (CMAKE_LIBRARY_ARCHITECTURE)
 find_package (GTK3 MODULE)
 find_package (GTK2 MODULE COMPONENTS gtk)
 find_package (GTK MODULE)

 if (NOT GTK3_FOUND)
  pkg_check_modules (PKG_GTK_3 gtk+-3.0)
  if (PKG_GTK_3_FOUND)
   message (STATUS "found gtk3")
   set (GTK3_FOUND ON)

   set (GTK3_INCLUDE_DIRS ${PKG_GTK_3_INCLUDE_DIRS})
   set (GTK3_LIBRARIES ${PKG_GTK_3_LIBRARIES})
  endif (PKG_GTK_3_FOUND)
 endif (NOT GTK3_FOUND)

 if (NOT GTK2_FOUND)
  pkg_check_modules (PKG_GTK_2 gtk+-2.0)
  if (PKG_GTK_2_FOUND)
   message (STATUS "found gtk2")
   set (GTK2_FOUND TRUE)
   
   set (GTK2_INCLUDE_DIRS ${PKG_GTK_2_INCLUDE_DIRS})
   set (GTK2_LIBRARIES ${PKG_GTK_2_LIBRARIES})
  endif (PKG_GTK_2_FOUND)
 endif (NOT GTK2_FOUND)

 if (NOT GTK_FOUND)
  pkg_check_modules (PKG_GTK gtk+)
  if (PKG_GTK_FOUND)
   set (GTK_FOUND TRUE)
   
   set (GTK_INCLUDE_DIRS ${PKG_GTK_INCLUDE_DIRS})
   set (GTK_LIBRARIES ${PKG_GTK_LIBRARIES})
  endif (PKG_GTK_FOUND)
 endif (NOT GTK_FOUND)

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
 endif (GTK3_FOUND)
elseif (WIN32)
 if (VCPKG_USE)
 # find_package (GTK)
  set (PKG_CONFIG_USE_CMAKE_PREFIX_PATH 1)
  pkg_check_modules (PKG_GTK_4 gtk4)
#  if (GTK_FOUND)
  if (PKG_GTK_4_FOUND)
   message (STATUS "found gtk4")
   set (GTK4_FOUND ON)
   set (GTK4_INCLUDE_DIRS ${PKG_GTK_4_INCLUDE_DIRS})
   set (GTK4_LIBRARIES ${PKG_GTK_4_LINK_LIBRARIES})
#   set (GTK4_LIBRARIES "")
#   foreach (GTK4_LIBRARY ${PKG_GTK_4_LIBRARIES})
#    set (GTK4_LIBRARIES "${VCPKG_LIB_DIR}/${GTK4_LIBRARY};${GTK4_LIBRARIES}")
#   endforeach ()
   set (GTK4_LIB_DIRS ${VCPKG_BIN_DIR})
  else ()
   message (WARNING "could not find gtk4, continuing")
#  endif (GTK_FOUND)
  endif (PKG_GTK_4_FOUND)

  pkg_check_modules (PKG_GTK_3 gtk+-3.0)
#  if (GTK_FOUND)
  if (PKG_GTK_3_FOUND)
   message (STATUS "found gtk3")
   set (GTK3_FOUND ON)
   set (GTK3_INCLUDE_DIRS ${PKG_GTK_3_INCLUDE_DIRS})
   set (GTK3_LIBRARIES ${PKG_GTK_3_LINK_LIBRARIES})
#   message (STATUS "GTK3_LIBRARIES: ${GTK3_LIBRARIES}")
#   set (GTK3_LIBRARIES "")
#   foreach (GTK3_LIBRARY ${PKG_GTK_3_LIBRARIES})
#    set (GTK3_LIBRARIES "${VCPKG_LIB_DIR}/${GTK3_LIBRARY};${GTK3_LIBRARIES}")
#   endforeach ()
   set (GTK3_LIB_DIRS ${VCPKG_BIN_DIR})
  else ()
   message (WARNING "could not find gtk3, continuing")
#  endif (GTK_FOUND)
  endif (PKG_GTK_3_FOUND)
 else ()
# *TODO*: repair win32 module support
  find_library (GTK2_LIBRARY gtk-win32-2.0.lib
                PATHS $ENV{LIB_ROOT}/gtk2
                PATH_SUFFIXES lib
                DOC "searching for gtk-win32-2.0.lib")
  if (GTK2_LIBRARY)
   message (STATUS "found gtk2")
   set (GTK2_FOUND ON)
   set (GTK2_INCLUDE_DIRS "$ENV{LIB_ROOT}/gtk2/include/atk-1.0;$ENV{LIB_ROOT}/gtk2/include/gdk-pixbuf-2.0;$ENV{LIB_ROOT}/gtk2/include/cairo;$ENV{LIB_ROOT}/gtk2/include/pango-1.0;$ENV{LIB_ROOT}/gtk2/lib/glib-2.0/include;$ENV{LIB_ROOT}/gtk2/include/glib-2.0;$ENV{LIB_ROOT}/gtk2/lib/gtk-2.0/include;$ENV{LIB_ROOT}/gtk2/include/gtk-2.0")
   set (GTK2_LIBRARIES "$ENV{LIB_ROOT}/gtk2/lib/gio-2.0.lib;$ENV{LIB_ROOT}/gtk2/lib/glib-2.0.lib;$ENV{LIB_ROOT}/gtk2/lib/gobject-2.0.lib;$ENV{LIB_ROOT}/gtk2/lib/gthread-2.0.lib;$ENV{LIB_ROOT}/gtk2/lib/gdk_pixbuf-2.0.lib;$ENV{LIB_ROOT}/gtk2/lib/gdk-win32-2.0.lib;$ENV{LIB_ROOT}/gtk2/lib/gtk-win32-2.0.lib;$ENV{LIB_ROOT}/gtk2/lib/pango-1.0.lib;$ENV{LIB_ROOT}/gtk2/lib/cairo.lib")
   set (GTK2_LIB_DIRS "$ENV{LIB_ROOT}/gtk2/bin")
  else ()
   message (WARNING "could not find gtk2 (was: \"gtk-win32-2.0.lib\"), continuing")
  endif (GTK2_LIBRARY)

  find_library (GTK3_LIBRARY gtk-win32-3.0.lib
                PATHS $ENV{LIB_ROOT}/gtk3
                PATH_SUFFIXES lib
                DOC "searching for gtk-win32-3.0.lib")
  if (GTK3_LIBRARY)
   message (STATUS "found gtk3")
   set (GTK3_FOUND ON)
 # *TODO*: Gtk < 3.16 does not have native opengl support
   set (GTKGL_FOUND ON)
   set (GTK3_INCLUDE_DIRS "$ENV{LIB_ROOT}/gtk3/include/atk-1.0;$ENV{LIB_ROOT}/gtk3/include/gdk-pixbuf-2.0;$ENV{LIB_ROOT}/gtk3/include/cairo;$ENV{LIB_ROOT}/gtk3/include/pango-1.0;$ENV{LIB_ROOT}/gtk3/lib/glib-2.0/include;$ENV{LIB_ROOT}/gtk3/include/glib-2.0;$ENV{LIB_ROOT}/gtk3/include/gtk-3.0")
   set (GTK3_LIBRARIES "$ENV{LIB_ROOT}/gtk3/lib/gio-2.0.lib;$ENV{LIB_ROOT}/gtk3/lib/glib-2.0.lib;$ENV{LIB_ROOT}/gtk3/lib/gobject-2.0.lib;$ENV{LIB_ROOT}/gtk3/lib/gthread-2.0.lib;$ENV{LIB_ROOT}/gtk3/lib/gdk_pixbuf-2.0.lib;$ENV{LIB_ROOT}/gtk3/lib/gdk-win32-3.0.lib;$ENV{LIB_ROOT}/gtk3/lib/gtk-win32-3.0.lib;$ENV{LIB_ROOT}/gtk3/lib/pango-1.0.lib;$ENV{LIB_ROOT}/gtk3/lib/pangocairo-1.0.lib;$ENV{LIB_ROOT}/gtk3/lib/cairo.lib")
   set (GTK3_LIB_DIRS "$ENV{LIB_ROOT}/gtk3/bin")
  else ()
   message (WARNING "could not find gtk3 (was: \"gtk-win32-3.0.lib\"), continuing")
  endif (GTK3_LIBRARY)
 endif (VCPKG_USE)
endif () # UNIX || WIN32
if (GTK4_FOUND)
 set (GTK4_SUPPORT ON)
# add_definitions (-DGTK4_SUPPORT)
endif (GTK4_FOUND)
if (GTK3_FOUND)
 set (GTK3_SUPPORT ON)
# add_definitions (-DGTK3_SUPPORT)
endif (GTK3_FOUND)
if (GTK2_FOUND)
 set (GTK2_SUPPORT ON)
# add_definitions (-DGTK2_SUPPORT)
endif (GTK2_FOUND)

# libglade for (early versions of-) gtk2
if (GTK_FOUND OR GTK2_FOUND)
# *NOTE*: early versions of gtk do not support GtkBuilder
# *TODO*: --> find the version number(s) and auto-enable this feature for these
#             target systems
# *TODO*: retrieve the available gtk version number(s) from the pkg-config
#         output and pre-set this option accordingly
 set (LIBGLADE_DEFAULT OFF)
 if (UNIX)
  pkg_check_modules (PKG_GLADE libglade-2.0)
  if (PKG_GLADE_FOUND)
   set (LIBGLADE_FOUND TRUE)
   set (LIBGLADE_INCLUDES ${PKG_GLADE_INCLUDE_DIRS})
  endif (PKG_GLADE_FOUND)
 elseif (WIN32)
# *NOTE*: on Win32 systems, the (binary) gtk3 applications have linkage issues
#         with the latest (2.6.4) libglade distribution (specifically, the
#         libxml2 dll cannot be easily shared between these two packages; also,
#         the 'embedded' library names do not match ('lib'-prefixes))
#         --> disable libglade support when targeting gtk3
  find_library (LIBGLADE_LIBRARY
                NAMES libglade-2.0.dll.a libglade-2.0.lib
                PATHS $ENV{LIB_ROOT}/libglade
                PATH_SUFFIXES lib
                DOC "searching for libglade-2.0.lib"
                NO_DEFAULT_PATH)
  if (LIBGLADE_LIBRARY)
   message (STATUS "found libglade: ${LIBGLADE_LIBRARY}")
   set (LIBGLADE_FOUND TRUE)
   set (LIBGLADE_INCLUDES $ENV{LIB_ROOT}/libglade/include/libglade-2.0)
   set (LIBGLADE_LIB_DIR $ENV{LIB_ROOT}/libglade/bin)
  endif (LIBGLADE_LIBRARY)
 endif ()
 if (LIBGLADE_FOUND)
  option (LIBGLADE_SUPPORT "enable libglade support" ON)
 endif (LIBGLADE_FOUND)
 if (LIBGLADE_SUPPORT)
  add_definitions (-DLIBGLADE_SUPPORT)
  add_link_options ("$<$<CXX_COMPILER_ID:MSVC>:-SAFESEH:NO>")
  option (LIBGLADE_USE "use libglade" ${LIBGLADE_DEFAULT})
  if (LIBGLADE_USE)
   add_definitions (-DLIBGLADE_USE)
  endif (LIBGLADE_USE)
 endif (LIBGLADE_SUPPORT)
endif (GTK_FOUND OR GTK2_FOUND)

if (GTK_FOUND OR GTK2_FOUND OR GTK3_FOUND OR GTK4_FOUND)
 option (GTK_SUPPORT "gtk support" ON)
 if (GTK_SUPPORT)
  add_definitions (-DGTK_SUPPORT)
 endif (GTK_SUPPORT)
else ()
 message (WARNING "failed to detect gtk, continuing")
endif (GTK_FOUND OR GTK2_FOUND OR GTK3_FOUND OR GTK4_FOUND)

# gtk opengl support
if (GTK_SUPPORT AND OPENGL_SUPPORT)
 if (GTK4_FOUND)
  set (GTKGL_FOUND TRUE)
 endif (GTK4_FOUND)

 if (GTK3_FOUND)
# *TODO*: find out the distribution release versions that require this package
# *NOTE*: assume GL support is available for gtk3 (either native or via
#         gtkglarea, see below)
  set (GTKGL_FOUND TRUE)
 endif (GTK3_FOUND)

 if (GTK2_FOUND)
# *TODO*: find out the distribution release versions that require this package
  pkg_check_modules (PKG_GTKGL2 gtkgl-2.0)
  if (PKG_GTKGL2_FOUND)
   set (GTKGL_FOUND TRUE)
  endif (PKG_GTKGL2_FOUND)
 endif (GTK2_FOUND)

 if (GTK_FOUND)
#    pkg_check_modules (PKG_GTKGLEXT gtkglext-libs gtkglext-devel)
  pkg_check_modules (PKG_GTKGLEXT gdkglext-1.0 gtkglext-1.0)
  if (PKG_GTKGLEXT_FOUND)
   set (GTKGL_FOUND TRUE)
  endif (PKG_GTKGLEXT_FOUND)
 endif (GTK_FOUND)

 if (GTK2_SUPPORT OR GTK3_SUPPORT)
  if (UNIX)
   if (NOT GTK_VERSION)
    set (GTK_VERSION "gtk3")
   endif (NOT GTK_VERSION)
   if (${GTK_VERSION} STREQUAL "gtk3")
    # *NOTE*: check out the 'master' branch for gtk3-based applications
    set (GTKGLAREA_LIB_FILE libgtkgl-3.0.so)
   elseif (${GTK_VERSION} STREQUAL "gtk2")
    # *NOTE*: to use gtkglarea on gtk2, check out the 'gtkglarea-2' branch
    #         of the project (instead of 'master')
    set (GTKGLAREA_LIB_FILE libgtkgl-2.0.so)
   endif ()
   find_library (GTKGLAREA_LIBRARY ${GTKGLAREA_LIB_FILE}
                 PATHS $ENV{LIB_ROOT}/gtkglarea/gtkgl
                 PATH_SUFFIXES .libs
                 DOC "searching for ${GTKGLAREA_LIB_FILE}")
  elseif (WIN32)
   set (PATH_SUFFIX "Release")
   if (DEFINED CMAKE_BUILD_TYPE)
    set (PATH_SUFFIX ${CMAKE_BUILD_TYPE})
   endif (DEFINED CMAKE_BUILD_TYPE)
   set (GTKGLAREA_LIB_FILE gtkglarea.lib)
   find_library (GTKGLAREA_LIBRARY ${GTKGLAREA_LIB_FILE}
                 PATHS $ENV{LIB_ROOT}/gtkglarea/build
                 PATH_SUFFIXES ${PATH_SUFFIX}
                 DOC "searching for ${GTKGLAREA_LIB_FILE}")
  endif ()
  if (GTKGLAREA_LIBRARY)
   message (STATUS "found gtkglarea: ${GTKGLAREA_LIBRARY}")
   set (GTKGLAREA_DEFAULT ON)
   set (GTKGLAREA_INCLUDES $ENV{LIB_ROOT}/gtkglarea)
   set (GTKGLAREA_LIB_DIR $ENV{LIB_ROOT}/libepoxy/bin)
   set (GTKGL_FOUND TRUE)
   option (GTKGLAREA_SUPPORT "enable GtkGLArea support" ${GTKGLAREA_DEFAULT})
  else ()
   message (WARNING "gtkglarea not found: ${GTKGLAREA_LIB_FILE}")
  endif (GTKGLAREA_LIBRARY)
  if (GTKGLAREA_SUPPORT)
   add_definitions (-DGTKGLAREA_SUPPORT)
  endif (GTKGLAREA_SUPPORT)
 endif (GTK2_SUPPORT OR GTK3_SUPPORT)

 if (GTKGL_FOUND)
  option (GTKGL_SUPPORT "enable GTK GL support" ON)
 endif (GTKGL_FOUND)
 if (GTKGL_SUPPORT)
  message (STATUS "GTK has GL support")
  add_definitions (-DGTKGL_SUPPORT)
 endif (GTKGL_SUPPORT)
endif (GTK_SUPPORT AND OPENGL_SUPPORT)

##########################################

# *NOTE*: this finds qt3/qt4 only
find_package (Qt CONFIG COMPONENTS Core Widgets)
if (QT3_INSTALLED)
 message (STATUS "qt3 found")
 set (QT3_FOUND ON)
elseif (QT4_INSTALLED)
 message (STATUS "qt4 found")
 set (QT4_FOUND ON)
endif ()
set (Qt5_DIR $ENV{QT5_DIR})
find_package (Qt5 CONFIG COMPONENTS Core Widgets)
if (Qt5_FOUND)
 message (STATUS "qt5 found")
 set (QT5_FOUND ON)
# set (AUTOMOC_EXECUTABLE "$ENV{QT5_DIR}/../../../bin/moc.exe")
# set (AUTOUIC_EXECUTABLE "$ENV{QT5_DIR}/../../../bin/uic.exe")
# message (STATUS "Qt5Widgets_INCLUDE_DIRS: ${Qt5Widgets_INCLUDE_DIRS}")
 set (QT_INCLUDES_DIRS "${Qt5Core_INCLUDE_DIRS};${Qt5Widgets_INCLUDE_DIRS}")
 if (UNIX)
  set (QT_LIBRARIES "Qt5Core;Qt5Widgets")
 elseif (WIN32)
  set (QT5_CORE_LIB_FILE Qt5Core.lib)
  find_library (QT5_CORE_LIBRARY ${QT5_CORE_LIB_FILE}
                PATHS $ENV{LIB_ROOT}/qt/5.15.2/msvc2019_64
                PATH_SUFFIXES lib
                DOC "searching for ${QT5_CORE_LIB_FILE}")
  set (QT5_WIDGETS_LIB_FILE Qt5Widgets.lib)
  find_library (QT5_WIDGETS_LIBRARY ${QT5_WIDGETS_LIB_FILE}
                PATHS $ENV{LIB_ROOT}/qt/5.15.2/msvc2019_64
                PATH_SUFFIXES lib
                DOC "searching for ${QT5_WIDGETS_LIB_FILE}")
  set (QT_LIBRARIES "${QT5_CORE_LIBRARY};${QT5_WIDGETS_LIBRARY}")
 endif ()
 set (QT5_LIB_DIR "$ENV{QT5_DIR}/../../../bin")
endif (Qt5_FOUND)
find_package (Qt6 CONFIG COMPONENTS Core Widgets)
if (Qt6_FOUND)
 message (STATUS "qt6 found")
 set (QT6_FOUND ON)
 set (QT_INCLUDES_DIRS "${Qt6Core_INCLUDE_DIRS};${Qt6Widgets_INCLUDE_DIRS}")
 set (QT_LIBRARIES "Qt6Core;Qt6Widgets")
 set (QT6_LIB_DIR "$ENV{QT6_DIR}/../../../bin")
endif (Qt6_FOUND)
if (QT3_FOUND OR QT4_FOUND OR QT5_FOUND OR QT6_FOUND)
 option (QT_SUPPORT "enable qt support" ON)
 if (QT_SUPPORT)
  add_definitions (-DQT_SUPPORT)
 endif (QT_SUPPORT)
endif (QT3_FOUND OR QT4_FOUND OR QT5_FOUND OR QT6_FOUND)

##########################################

#unset (CYGWIN)
#unset (MSYS)
unset (CMAKE_CROSSCOMPILING)
set (wxWidgets_USE_DEBUG OFF)
if (CMAKE_BUILD_TYPE STREQUAL Debug)
 set (wxWidgets_USE_DEBUG ON)
endif (CMAKE_BUILD_TYPE STREQUAL Debug)
set (wxWidgets_USE_UNIVERSAL OFF)
set (wxWidgets_USE_STATIC OFF)
if (UNIX)
 set (wxWidgets_USE_UNICODE ON)
# set (wxWidgets_CONFIG_OPTIONS --toolkit=base --prefix=/usr)
elseif (WIN32)
 set (wxWidgets_USE_UNICODE OFF)
 set (ENV{wxWidgets_ROOT_DIR} "$ENV{WXWIN}")
 file (TO_CMAKE_PATH "$ENV{wxWidgets_ROOT_DIR}" ENV{wxWidgets_ROOT_DIR})
 set (wxWidgets_LIB_DIR "${wxWidgets_ROOT_DIR}/lib/vc_x64_dll")
 set (wxWidgets_CONFIGURATION msw)
 if (CMAKE_BUILD_TYPE STREQUAL Debug)
# *WARNING*: linking against debug versions incurs dependencies to the debug
#            vcrt libraries; these may not exist on your target platform
  set (wxWidgets_CONFIGURATION ${wxWidgets_CONFIGURATION}d)
 endif (CMAKE_BUILD_TYPE STREQUAL Debug)
endif ()
if (CMAKE_BUILD_TYPE STREQUAL Debug)
 add_definitions (-DwxUSE_LOG_DEBUG)
endif (CMAKE_BUILD_TYPE STREQUAL Debug)
find_package (wxWidgets MODULE
              COMPONENTS base core gl xrc
              OPTIONAL_COMPONENTS aui adv html media net propgrid qa richtext ribbon stc webview xml)
if (wxWidgets_FOUND)
 message (STATUS "wxWidgets found: ${wxWidgets_ROOT_DIR}")
# *NOTE*: this defines UNICODE; avoid
# include (${wxWidgets_USE_FILE})
# set (wxWidgets_INCLUDE_DIRS "${wxWidgets_ROOT_DIR}/include")
 option (WXWIDGETS_SUPPORT "enable wxWidgets support" ON)
 if (WXWIDGETS_SUPPORT)
  add_definitions (-DWXWIDGETS_SUPPORT)
  if (CMAKE_BUILD_TYPE STREQUAL Debug)
   foreach (DEFINITION ${wxWidgets_DEFINITIONS_DEBUG})
    add_definitions (-D${DEFINITION})
   endforeach (DEFINITION ${wxWidgets_DEFINITIONS_DEBUG})
  endif ()
  foreach (DEFINITION ${wxWidgets_DEFINITIONS})
   add_definitions (-D${DEFINITION})
  endforeach (DEFINITION ${wxWidgets_DEFINITIONS})
  if (UNIX)
   add_definitions (-DwxUSE_UNICODE=1)
  elseif (WIN32)
   add_definitions (-D_M_X64)
   add_definitions (-D__WXMSW__)
   add_definitions (-DwxHAS_MODE_T)
   add_definitions (-DwxUSE_UNICODE=0)
   add_definitions (-DWXUSINGDLL)
   if (MSVC)
#    add_definitions (-DwxMSVC_VERSION_AUTO)
#    add_definitions (-DwxMSVC_VERSION_ABI_COMPAT)
   endif (MSVC)
#   message (STATUS "wxWidgets_ROOT_DIR: ${wxWidgets_ROOT_DIR}")
#   set (wxWidgets_INCLUDE_DIRS "${wxWidgets_ROOT_DIR}/include/msvc;${wxWidgets_INCLUDE_DIRS}")
   if ($<CONFIG> STREQUAL Debug OR
       $<CONFIG> STREQUAL RelWithDebInfo)
    set (wxWidgets_INCLUDE_DIRS "${wxWidgets_ROOT_DIR}/lib/vc_x64_dll/mswd;${wxWidgets_INCLUDE_DIRS}")
   else ()
    set (wxWidgets_INCLUDE_DIRS "${wxWidgets_ROOT_DIR}/lib/vc_x64_dll/msw;${wxWidgets_INCLUDE_DIRS}")
   endif ($<CONFIG> STREQUAL Debug OR
          $<CONFIG> STREQUAL RelWithDebInfo)
  endif ()
  add_definitions (-DwxUSE_GUI=1)
  if (CMAKE_BUILD_TYPE STREQUAL Debug)
   add_definitions (-D__WXDEBUG__)
  endif (CMAKE_BUILD_TYPE STREQUAL Debug)
#  message (STATUS "wxWidgets_DEFINITIONS: ${wxWidgets_DEFINITIONS}")
#  message (STATUS "wxWidgets_DEFINITIONS_DEBUG: ${wxWidgets_DEFINITIONS_DEBUG}")
#  message (STATUS "wxWidgets_CXX_FLAGS: ${wxWidgets_CXX_FLAGS}")
#  message (STATUS "wxWidgets_USE_FILE: ${wxWidgets_USE_FILE}")
 endif (WXWIDGETS_SUPPORT)
endif (wxWidgets_FOUND)

##########################################

set (CURSES_NEED_NCURSES TRUE FORCE)
set (CURSES_NEED_WIDE TRUE FORCE)
#find_package (Curses MODULE)
if (CURSES_FOUND)
# this finds (n)curses only, now find panel
 message (STATUS "curses found")
 pkg_check_modules (PKG_PANEL panelw)
 if (PKG_PANEL_FOUND)
  message (STATUS "panel found")
  set (CURSES_FOUND TRUE)
  set (CURSES_INCLUDE_DIRS "${CURSES_INCLUDE_DIRS};${PKG_PANEL_INCLUDE_DIRS}")
  set (CURSES_LIBRARIES "${CURSES_LIBRARIES};${PKG_PANEL_LIBRARIES}")
 endif (PKG_PANEL_FOUND)
else ()
 if (UNIX)
  pkg_check_modules (PKG_CURSES ncursesw panelw)
  if (PKG_CURSES_FOUND)
   message (STATUS "curses && panel found")
   set (CURSES_FOUND TRUE)
   set (CURSES_INCLUDE_DIRS "${PKG_CURSES_INCLUDE_DIRS}")
   set (CURSES_LIBRARIES "${PKG_CURSES_LIBRARIES}")
  endif (PKG_CURSES_FOUND)
 elseif (WIN32)
  if (VCPKG_USE)
#   find_package (PDCURSES)
   find_library (CURSES_LIBRARY pdcurses.lib
                 PATHS ${VCPKG_LIB_DIR}
                 DOC "searching for pdcurses.lib")
   if (CURSES_LIBRARY)
    set (CURSES_FOUND TRUE)
    set (CURSES_INCLUDE_DIRS ${VCPKG_INCLUDE_DIR})
    set (CURSES_LIBRARIES ${CURSES_LIBRARY})
    set (CURSES_LIB_DIR "${VCPKG_BIN_DIR}")
   endif (CURSES_LIBRARY)
  endif (VCPKG_USE)

  if (NOT CURSES_FOUND)
   set (PATH_SUFFIX "Release")
   if (DEFINED CMAKE_BUILD_TYPE)
    set (PATH_SUFFIX ${CMAKE_BUILD_TYPE})
   endif (DEFINED CMAKE_BUILD_TYPE)
   find_library (CURSES_LIBRARY pdcurses.lib
                 PATHS $ENV{LIB_ROOT}/PDCurses/wincon
                 PATH_SUFFIXES ${PATH_SUFFIX}
                 DOC "searching for pdcurses.lib")
   if (NOT CURSES_LIBRARY)
    message (WARNING "could not find pdcurses.lib, continuing")
   else ()
    message (STATUS "curses found")
# *NOTE*: pdcurses.lib incorporates panel.lib
    set (PANEL_LIBRARY ${CURSES_LIBRARY})
    set (CURSES_INCLUDE_DIRS "$ENV{LIB_ROOT}/PDCurses")
    set (CURSES_LIBRARIES "${CURSES_LIBRARY}")
    set (CURSES_LIB_DIR "$ENV{LIB_ROOT}/PDCurses/wincon/${CMAKE_BUILD_TYPE}")
   endif (NOT CURSES_LIBRARY)
   if (CURSES_LIBRARY AND PANEL_LIBRARY)
    set (CURSES_FOUND TRUE)
   endif (CURSES_LIBRARY AND PANEL_LIBRARY)
  endif (NOT CURSES_FOUND)
 endif ()
endif (CURSES_FOUND)
if (CURSES_FOUND)
 option (CURSES_SUPPORT "enable (n)curses support" ON)
 if (CURSES_SUPPORT)
  add_definitions (-DCURSES_SUPPORT)
  if (UNIX)
# *WORKAROUND*
   add_definitions (-DNCURSES_INTERNALS)
  endif (UNIX)
 endif (CURSES_SUPPORT)
endif (CURSES_FOUND)
