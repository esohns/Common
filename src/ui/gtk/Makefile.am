ACLOCAL_AMFLAGS = \
  -I m4

DEFS = \
  -DGTK_SUPPORT=1 \
  -DGTK3_SUPPORT=1 \
  -DGTKGL_SUPPORT=1 \
  -DCOMMON_UI_BUILD_DLL=1 \
  -DPACKAGE=\"@PACKAGE_NAME@\"
if LIBGLADE_SUPPORT
DEFS += \
  -DLIBGLADE_SUPPORT=1
endif

AM_CPPFLAGS = \
  $(PKGGTK3_CFLAGS) \
  -I$(top_srcdir)/../ATCD/ACE
if VALGRIND_SUPPORT
AM_CPPFLAGS += \
  $(PKGVALGRIND_CFLAGS)
endif
if LIBGLADE_SUPPORT
AM_CPPFLAGS += \
  $(PKGGLADE_CFLAGS)
endif

noinst_HEADERS = \
  stdafx.h
include_HEADERS = \
  common_ui_gtk_builder_definition.h \
  common_ui_gtk_builder_definition.inl \
  common_ui_gtk_common.h \
  common_ui_gtk_defines.h \
  common_ui_gtk_eventhandler.h \
  common_ui_gtk_eventhandler.inl \
  common_ui_gtk_exports.h \
  common_ui_gtk_glade_definition.h \
  common_ui_gtk_gl_common.h \
  common_ui_gtk_manager.h \
  common_ui_gtk_manager.inl \
  common_ui_gtk_manager_common.h \
  common_ui_gtk_tools.h \
  common_ui_igtk.h

lib_LTLIBRARIES = libCommonUI_GTK.la

libCommonUI_GTK_la_SOURCES = \
  common_ui_gtk_builder_definition.cpp \
  common_ui_gtk_builder_definition.h \
  common_ui_gtk_builder_definition.inl \
  common_ui_gtk_common.h \
  common_ui_gtk_defines.h \
  common_ui_gtk_eventhandler.cpp \
  common_ui_gtk_eventhandler.h \
  common_ui_gtk_eventhandler.inl \
  common_ui_gtk_exports.h \
  common_ui_gtk_glade_definition.cpp \
  common_ui_gtk_glade_definition.h \
  common_ui_gtk_gl_common.h \
  common_ui_gtk_manager.cpp \
  common_ui_gtk_manager.h \
  common_ui_gtk_manager.inl \
  common_ui_gtk_manager_common.h \
  common_ui_gtk_tools.cpp \
  common_ui_gtk_tools.h \
  common_ui_igtk.h \
  stdafx.cpp \
  stdafx.h
libCommonUI_GTK_la_CPPFLAGS = \
  $(AM_CPPFLAGS) \
  -I$(srcdir)/../.. \
  -I$(srcdir)/..
#  -I$(builddir)/..
libCommonUI_GTK_la_LDFLAGS = \
  -version-info @LIBCOMMON_LT_VERSION_INFO@

