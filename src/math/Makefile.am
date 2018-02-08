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
  common_math_complex.h \
  common_math_exports.h \
  common_math_fft.h \
  common_math_sample.h \
  common_math_sample.inl

lib_LTLIBRARIES = libCommonMath.la

libCommonMath_la_SOURCES = \
  common_math_complex.h \
  common_math_exports.h \
  common_math_fft.cpp \
  common_math_fft.h \
  common_math_sample.cpp \
  common_math_sample.h \
  common_math_sample.inl \
  stdafx.cpp \
  stdafx.h
libCommonMath_la_CPPFLAGS = \
  $(AM_CPPFLAGS) \
  -I$(srcdir)/.. \
  -I$(builddir)/..
libCommonMath_la_LDFLAGS = \
  -version-info @LIBCOMMON_LT_VERSION_INFO@

