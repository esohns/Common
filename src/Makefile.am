ACLOCAL_AMFLAGS = \
  -I m4

DEFS += \
  -DHAVE_CONFIG_H \
  -DCOMMON_BUILD_DLL=1 \
  -DPACKAGE=\"@PACKAGE_NAME@\"

#  -I/usr/include/c++/3.4.6/backward
#  -I$(top_srcdir)/3rd_party/c++
AM_CPPFLAGS = \
  -I$(top_srcdir)/../ATCD/ACE

noinst_HEADERS = \
  stdafx.h
include_HEADERS = \
  common.h \
  common_configuration.h \
  common_defines.h \
  common_exports.h \
  common_file_tools.h \
  common_iclone.h \
  common_icontrol.h \
  common_icounter.h \
  common_idumpstate.h \
  common_iget.h \
  common_iinitialize.h \
  common_ilock.h \
  common_inotify.h \
  common_ireferencecount.h \
  common_iscanner.h \
  common_isignal.h \
  common_istatemachine.h \
  common_istatistic.h \
  common_isubscribe.h \
  common_itask.h \
  common_itaskcontrol.h \
  common_logger.h \
  common_logger.inl \
  common_macros.h \
  common_message_queue_iterator.h \
  common_message_queue_iterator.inl \
  common_referencecounter_base.h \
  common_signalhandler.h \
  common_signalhandler.inl \
  common_signal_tools.h \
  common_statemachine.h \
  common_statemachine.inl \
  common_statemachine_base.h \
  common_statemachine_base.inl \
  common_statistic_handler.h \
  common_statistic_handler.inl \
  common_task.h \
  common_task.inl \
  common_task_base.h \
  common_task_base.inl \
  common_time_common.h \
  common_tools.h

lib_LTLIBRARIES = libCommon.la
libCommon_la_SOURCES = \
  common.h \
  common_configuration.h \
  common_defines.h \
  common_exports.h \
  common_file_tools.cpp \
  common_file_tools.h \
  common_iclone.h \
  common_icontrol.h \
  common_icounter.h \
  common_idumpstate.h \
  common_iget.h \
  common_iinitialize.h \
  common_ilock.h \
  common_inotify.h \
  common_ireferencecount.h \
  common_iscanner.h \
  common_isignal.h \
  common_istatemachine.h \
  common_istatistic.h \
  common_isubscribe.h \
  common_itask.h \
  common_itaskcontrol.h \
  common_logger.cpp \
  common_logger.h \
  common_logger.inl \
  common_macros.h \
  common_message_queue_iterator.cpp \
  common_message_queue_iterator.h \
  common_message_queue_iterator.inl \
  common_referencecounter_base.cpp \
  common_referencecounter_base.h \
  common_signalhandler.cpp \
  common_signalhandler.h \
  common_signalhandler.inl \
  common_signal_tools.cpp \
  common_signal_tools.h \
  common_statemachine.cpp \
  common_statemachine.h \
  common_statemachine.inl \
  common_statemachine_base.cpp \
  common_statemachine_base.h \
  common_statemachine_base.inl \
  common_statistic_handler.cpp \
  common_statistic_handler.h \
  common_statistic_handler.inl \
  common_task.cpp \
  common_task.h \
  common_task.inl \
  common_task_base.cpp \
  common_task_base.h \
  common_task_base.inl \
  common_time_common.h \
  common_tools.cpp \
  common_tools.h \
  stdafx.cpp \
  stdafx.h

libCommon_la_CPPFLAGS = \
  $(AM_CPPFLAGS) \
  -I$(srcdir)/timer
libCommon_la_LDFLAGS = \
  -version-info @LIBCOMMON_LT_VERSION_INFO@

# (GNU-)make-rule for pch
#%.h.gch: %.h
#  $(CXX) -c $<
#stdafx.h.gch: stdafx.h
#   $(CXX) -c $<
#stdafx.h.gch: Makefile
#  $(CXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(CXXFLAGS) $(libCommon_la_CXXFLAGS) $(extra_CFLAGS_libCommon_la) -c stdafx.h -o stdafx.h.gch

SUBDIRS = \
  . \
  dbus \
  gl \
  image \
  math \
  timer \
  ui \
  xml

