#  -I/usr/include/c++/3.4.6/backward
#  -I$(top_srcdir)/modules/ATCD/ACE
AM_CPPFLAGS = \
  -I$(top_srcdir)/../ATCD/ACE
#  $(PKGSDL_CFLAGS) \
#  $(PKGGTK_CFLAGS)
if VALGRIND_SUPPORT
AM_CPPFLAGS += \
  $(PKGVALGRIND_CFLAGS)
endif

DEFS = \
  -DHAVE_CONFIG_H
 # -DBASEDIR=\"$(datadir)/@PACKAGE_NAME@\"
if DEBUG
DEFS += \
  -DDEBUG_DEBUGGER
endif

bin_PROGRAMS = \
  features

noinst_HEADERS = stdafx.h
features_SOURCES = \
  test_u.cpp \
  stdafx.cpp \
  stdafx.h

#  -I$(top_srcdir)/modules/libCommon/src \
#  -I$(top_srcdir)/modules/libCommon/src/ui
features_CPPFLAGS = \
  $(AM_CPPFLAGS) \
  -I$(srcdir)/../../src \
  -I$(srcdir)/..
features_LDFLAGS = \
  -L$(abs_builddir)/../../src/.libs
#capabilities_LDADD = \
#  -lACE \
#  $(PKGGTK_LIBS) \
#  -lCommon
#  -lCommon_UI \
#  -lACEStream \
#  -lACEStream_File \
#  -lACEStream_Misc

