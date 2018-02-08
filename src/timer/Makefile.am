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
  common_itimer.h \
  common_itimerhandler.h \
  common_timer_common.h \
  common_timer_defines.h \
  common_timer_handler.h \
  common_timer_manager.h \
  common_timer_manager.inl \
  common_timer_manager_common.h \
  common_timer_tools.h

lib_LTLIBRARIES = libCommonTimer.la

libCommonTimer_la_SOURCES = \
  common_itimer.h \
  common_itimerhandler.h \
  common_timer_common.h \
  common_timer_defines.h \
  common_timer_handler.cpp \
  common_timer_handler.h \
  common_timer_manager.cpp \
  common_timer_manager.h \
  common_timer_manager.inl \
  common_timer_manager_common.h \
  common_timer_tools.cpp \
  common_timer_tools.h \
  stdafx.cpp \
  stdafx.h
libCommonTimer_la_CPPFLAGS = \
  $(AM_CPPFLAGS) \
  -I$(srcdir)/.. \
  -I$(builddir)/..
libCommonTimer_la_LDFLAGS = \
  -version-info @LIBCOMMON_LT_VERSION_INFO@

