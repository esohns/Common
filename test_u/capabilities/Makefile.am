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
  capabilities \
  capabilities_2

noinst_HEADERS = stdafx.h
capabilities_SOURCES = \
  test_u.cpp \
  stdafx.cpp \
  stdafx.h

#  -I$(top_srcdir)/modules/libCommon/src \
#  -I$(top_srcdir)/modules/libCommon/src/ui
capabilities_CPPFLAGS = \
  $(AM_CPPFLAGS) \
  -I$(srcdir)/../../src \
  -I$(srcdir)/..
capabilities_LDFLAGS = \
  -L$(abs_builddir)/../../src/.libs
#capabilities_LDADD = \
#  -lACE \
#  $(PKGGTK_LIBS) \
#  -lCommon
#  -lCommon_UI \
#  -lACEStream \
#  -lACEStream_File \
#  -lACEStream_Misc

# ----------------------------------------

capabilities_2_SOURCES = \
  test_u_2.cpp \
  stdafx.cpp \
  stdafx.h

#  -I$(top_srcdir)/modules/libCommon/src \
#  -I$(top_srcdir)/modules/libCommon/src/ui
capabilities_2_CPPFLAGS = \
  $(AM_CPPFLAGS) \
  -I$(srcdir)/../../src \
  -I$(srcdir)/../../src/timer \
  -I$(srcdir)/.. \
  -I$(top_srcdir)
capabilities_2_LDFLAGS = \
  -L$(abs_builddir)/../../../ATCD/ACE/lib \
  -L$(abs_builddir)/../../src/.libs \
  -L$(abs_builddir)/../../src/timer/.libs
capabilities_2_LDADD = \
  -lcap \
  -lACE \
  -lCommon \
  -lCommonTimer

