ACLOCAL_AMFLAGS = \
  -I m4

DEFS = \
  -DHAVE_CONFIG_H \
  -DCOMMON_BUILD_DLL=1 \
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
  common_gl_common.h \
  common_gl_defines.h \
  common_gl_exports.h \
  common_gl_tools.h

lib_LTLIBRARIES = libCommonGL.la

libCommonGL_la_SOURCES = \
  common_gl_common.h \
  common_gl_defines.h \
  common_gl_exports.h \
  common_gl_tools.cpp \
  common_gl_tools.h \
  stdafx.cpp \
  stdafx.h
libCommonGL_la_CPPFLAGS = \
  $(AM_CPPFLAGS) \
  -I$(srcdir)/.. \
  -I$(srcdir)/../image
#  -I$(builddir)/..
libCommonGL_la_LDFLAGS = \
  -version-info @LIBCOMMON_LT_VERSION_INFO@

