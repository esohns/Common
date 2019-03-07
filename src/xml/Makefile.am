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
  common_xml_common.h \
  common_xml_defines.h \
  common_xml_handler.h \
  common_xml_handler_base.h \
  common_xml_handler_base.inl \
  common_xml_parser.h \
  common_xml_parser.inl \
  common_xml_tools.h

lib_LTLIBRARIES = libCommonXML.la

libCommonXML_la_SOURCES = \
  common_xml_common.h \
  common_xml_defines.h \
  common_xml_handler.cpp \
  common_xml_handler.h \
  common_xml_handler_base.cpp \
  common_xml_handler_base.h \
  common_xml_handler_base.inl \
  common_xml_parser.cpp \
  common_xml_parser.h \
  common_xml_parser.inl \
  common_xml_tools.cpp \
  common_xml_tools.h \
  stdafx.cpp \
  stdafx.h
libCommonXML_la_CPPFLAGS = \
  $(AM_CPPFLAGS) \
  -I$(srcdir)/.. \
  -I$(builddir)/..
libCommonXML_la_LDFLAGS = \
  -version-info @LIBCOMMON_LT_VERSION_INFO@

