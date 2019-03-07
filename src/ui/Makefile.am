ACLOCAL_AMFLAGS = \
  -I m4

DEFS = \
  -DCOMMON_UI_BUILD_DLL=1 \
  -DPACKAGE=\"@PACKAGE_NAME@\"

AM_CPPFLAGS = \
  -I$(top_srcdir)/../ATCD/ACE \
  $(PKGSDL_CFLAGS) \
  $(PKGGTK_CFLAGS)
if VALGRIND_SUPPORT
AM_CPPFLAGS += \
  $(PKGVALGRIND_CFLAGS)
endif

noinst_HEADERS = \
  stdafx.h
include_HEADERS = \
  common_ui_common.h \
  common_ui_defines.h \
  common_ui_exports.h \
  common_ui_tools.h \
  common_ui_types.h

lib_LTLIBRARIES = libCommonUI.la

libCommonUI_la_SOURCES = \
  common_ui_common.h \
  common_ui_defines.h \
  common_ui_exports.h \
  common_ui_tools.cpp \
  common_ui_tools.h \
  common_ui_types.h \
  stdafx.cpp \
  stdafx.h
libCommonUI_la_CPPFLAGS = \
  $(AM_CPPFLAGS) \
  -I$(srcdir)/.. \
  -I$(builddir)/..
libCommonUI_la_LDFLAGS = \
  -version-info @LIBCOMMON_LT_VERSION_INFO@

SUBDIRS = \
  . \
  gtk

