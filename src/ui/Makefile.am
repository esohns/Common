ACLOCAL_AMFLAGS = \
  -I m4

DEFS = \
  -DCOMMON_UI_BUILD_DLL=1 \
  -DMETA_PACKAGE=\"@META_PACKAGE@\"

AM_CPPFLAGS = \
  -I$(srcdir)/..

noinst_HEADERS = \
  stdafx.h
include_HEADERS = \
  common_ui_defines.h \
  common_ui_exports.h \
  common_ui_gtk_manager.h \
  common_ui_igtk.h \
  common_ui_types.h

lib_LTLIBRARIES = libCommon_UI.la

libCommon_UI_la_SOURCES = \
  common_ui_defines.h \
  common_ui_exports.h \
  common_ui_gtk_manager.cpp \
  common_ui_gtk_manager.h \
  common_ui_igtk.h \
  common_ui_types.h \
  stdafx.cpp \
  stdafx.h

libCommon_UI_la_CPPFLAGS = \
  $(AM_CPPFLAGS) \
  $(PKGLIB_CFLAGS)
