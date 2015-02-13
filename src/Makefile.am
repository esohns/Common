ACLOCAL_AMFLAGS = -I m4

AM_CPPFLAGS = \
  -DLIBCOMMON_BUILD_DLL=1 \
  -DMETA_PACKAGE=\"@META_PACKAGE@\"

noinst_HEADERS = \
  stdafx.h
include_HEADERS = \
  common_defines.h \
  common_exports.h \
  common_file_tools.h \
  common_idumpstate.h \
  common_iclone.h \
  common_icontrol.h \
  common_icounter.h \
  common_inotify.h \
  common_irefcount.h \
  common_isignal.h \
  common_istatistic.h \
  common_isubscribe.h \
  common_itimer.h \
  common_macros.h \
  common_referencecounter_base.h \
  common_signalhandler.h \
  common_task_base.inl \
  common_task_base.h \
  common_timerhandler.h \
  common_timer_manager.h \
  common_tools.h \
  common.h

lib_LTLIBRARIES = libCommon.la
libCommon_la_LDFLAGS = -version-info 0:1:0
libCommon_la_SOURCES = \
  common_defines.h \
  common_exports.h \
  common_file_tools.cpp \
  common_file_tools.h \
  common_iclone.h \
  common_icontrol.h \
  common_icounter.h \
  common_idumpstate.h \
  common_inotify.h \
  common_irefcount.h \
  common_isignal.h \
  common_istatistic.h \
  common_isubscribe.h \
  common_itimer.h \
  common_macros.h \
  common_macros.h \
  common_referencecounter_base.cpp \
  common_referencecounter_base.h \
  common_signalhandler.cpp \
  common_signalhandler.h \
  common_task_base.cpp \
  common_task_base.inl \
  common_task_base.h \
  common_timerhandler.cpp \
  common_timerhandler.h \
  common_timer_manager.cpp \
  common_timer_manager.h \
  common_tools.cpp \
  common_tools.h \
  common.h \
  stdafx.cpp \
  stdafx.h
#  stdafx.h.gch

# (GNU-)make-rule for pch
#%.h.gch: %.h
#  $(CXX) -c $<
#stdafx.h.gch: stdafx.h
#   $(CXX) -c $<
#stdafx.h.gch: Makefile
#  $(CXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(CXXFLAGS) $(libCommon_la_CXXFLAGS) $(extra_CFLAGS_libCommon_la) -c stdafx.h -o stdafx.h.gch

