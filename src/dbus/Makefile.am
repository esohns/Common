ACLOCAL_AMFLAGS = \
  -I m4

DEFS += \
  -DHAVE_CONFIG_H \
  -DCOMMON_BUILD_DLL=1 \
  -DPACKAGE=\"@PACKAGE_NAME@\"

#  -I/usr/include/c++/3.4.6/backward
#  -I$(top_srcdir)/3rd_party/c++
AM_CPPFLAGS = \
  -I/usr/include/dbus-1.0 \
  -I/usr/lib64/dbus-1.0/include \
  -I$(top_srcdir)/../ATCD/ACE

noinst_HEADERS = \
  stdafx.h
include_HEADERS = \
  common_dbus_tools.h

lib_LTLIBRARIES = libCommonDBus.la
libCommonDBus_la_SOURCES = \
  common_dbus_tools.cpp \
  common_dbus_tools.h \
  stdafx.cpp \
  stdafx.h
#  stdafx.h.gch

libCommonDBus_la_CPPFLAGS = \
  $(AM_CPPFLAGS) \
  -I$(srcdir)/..
libCommonDBus_la_LDFLAGS = \
  -version-info @LIBCOMMON_LT_VERSION_INFO@

# (GNU-)make-rule for pch
#%.h.gch: %.h
#  $(CXX) -c $<
#stdafx.h.gch: stdafx.h
#   $(CXX) -c $<
#stdafx.h.gch: Makefile
#  $(CXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(CXXFLAGS) $(libCommon_la_CXXFLAGS) $(extra_CFLAGS_libCommon_la) -c stdafx.h -o stdafx.h.gch

