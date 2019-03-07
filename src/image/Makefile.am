ACLOCAL_AMFLAGS = \
  -I m4

DEFS = \
  -DHAVE_CONFIG_H \
  -DCOMMON_BUILD_DLL=1 \
  -DPACKAGE=\"@PACKAGE_NAME@\"

AM_CPPFLAGS = \
  -I/usr/include/ffmpeg \
  -I$(top_srcdir)/../ATCD/ACE
#  $(PKGSDL_CFLAGS) \
#  $(PKGGTK_CFLAGS)
if VALGRIND_SUPPORT
AM_CPPFLAGS += \
  $(PKGVALGRIND_CFLAGS)
endif

noinst_HEADERS = \
  stdafx.h
include_HEADERS = \
  common_image_defines.h \
  common_image_exports.h \
  common_image_tools.h

lib_LTLIBRARIES = libCommonImage.la

libCommonImage_la_SOURCES = \
  common_image_defines.h \
  common_image_exports.h \
  common_image_tools.cpp \
  common_image_tools.h \
  stdafx.cpp \
  stdafx.h
libCommonImage_la_CPPFLAGS = \
  $(AM_CPPFLAGS) \
  -I$(srcdir)/..
#  -I$(builddir)/..
libCommonImage_la_LDFLAGS = \
  -version-info @LIBCOMMON_LT_VERSION_INFO@

