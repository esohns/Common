#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

# version
m4_define([M4_LIBCOMMON_VERSION_MAJOR], [0])
m4_define([M4_LIBCOMMON_VERSION_MINOR], [0])
m4_define([M4_LIBCOMMON_VERSION_MICRO], [1])
dnl *TODO*: rpmbuild complains about the .spec file version string if it contains '-'
#m4_define([M4_LIBCOMMON_VERSION_DEVEL], [devel])
m4_define([M4_LIBCOMMON_VERSION],
          [M4_LIBCOMMON_VERSION_MAJOR.M4_LIBCOMMON_VERSION_MINOR.M4_LIBCOMMON_VERSION_MICRO[]m4_ifset([M4_LIBCOMMON_VERSION_DEVEL],-[M4_LIBCOMMON_VERSION_DEVEL])])
m4_define([M4_LIBCOMMON_VERSION_API], [1])

# libtool versioning
m4_define([M4_LIBCOMMON_LT_VERSION_AGE], [0])
m4_define([M4_LIBCOMMON_LT_VERSION_CURRENT], [M4_LIBCOMMON_VERSION_MAJOR])
m4_define([M4_LIBCOMMON_LT_VERSION_REVISION], [m4_eval(100 * M4_LIBCOMMON_VERSION_MAJOR + 10 * M4_LIBCOMMON_VERSION_MINOR + M4_LIBCOMMON_VERSION_MICRO)])
m4_define([M4_LIBCOMMON_LT_VERSION_INFO],
          [M4_LIBCOMMON_LT_VERSION_CURRENT:M4_LIBCOMMON_LT_VERSION_REVISION:M4_LIBCOMMON_LT_VERSION_AGE])

# version suffix
m4_ifset([M4_LIBCOMMON_VERSION_DEVEL],
         [AC_DEFINE(HAVE_VERSION_DEVEL, [1], [define if a version suffix is present])])

# (meta-)package name
m4_define([M4_PACKAGE_NAME], [libCommon])

# maintainer email address
m4_define([M4_MAINTAINER_EMAIL_ADDRESS], [eriksohns@123mail.org])

AC_PREREQ([2.69])
AC_INIT([M4_PACKAGE_NAME],
        [M4_LIBCOMMON_VERSION],
        [M4_MAINTAINER_EMAIL_ADDRESS],
        [M4_PACKAGE_NAME],
        [http://www.github.com/esohns/libCommon])
AC_CONFIG_AUX_DIR([aux_config])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR([config.h.in])
dnl AC_CONFIG_SUBDIRS([modules/ATCD])

LIBCOMMON_VERSION_API=M4_LIBCOMMON_VERSION_API
AC_SUBST([LIBCOMMON_VERSION_API])

LIBCOMMON_LT_VERSION_INFO=M4_LIBCOMMON_LT_VERSION_INFO
AC_SUBST([LIBCOMMON_LT_VERSION_INFO])

LIBCOMMON_VERSION=M4_LIBCOMMON_VERSION
AC_SUBST([LIBCOMMON_VERSION])
LIBCOMMON_VERSION_MAJOR=M4_LIBCOMMON_VERSION_MAJOR
AC_SUBST([LIBCOMMON_VERSION_MAJOR])
LIBCOMMON_VERSION_MINOR=M4_LIBCOMMON_VERSION_MINOR
AC_SUBST([LIBCOMMON_VERSION_MINOR])
LIBCOMMON_VERSION_MICRO=M4_LIBCOMMON_VERSION_MICRO
AC_SUBST([LIBCOMMON_VERSION_MICRO])

#AC_USE_SYSTEM_EXTENSIONS

# checks for programs
#AC_PROG_AWK
#AC_PROG_CC
AC_PROG_CPP
AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_RANLIB
AM_PROG_AR

# initialize other tools
AM_INIT_AUTOMAKE([tar-ustar -Wall -Werror foreign subdir-objects])
# build time sanity check...
AM_SANITY_CHECK
LT_INIT

# prefix config.h
m4_define([LIBCOMMON_CONFIG_H], [libCommon_config.h])
AX_PREFIX_CONFIG_H([LIBCOMMON_CONFIG_H], [M4_PACKAGE_NAME], [config.h])

#AC_LANG([C++])

## (re)set the cpp compiler flags
#CXXFLAGS_DEBUG="-fno-implicit-templates"
CXXFLAGS_DEBUG=
# --> enable debug support ?
AC_MSG_CHECKING([enable debugging])
debug_default="no"
AC_ARG_ENABLE([debug],
              [AS_HELP_STRING([--enable-debug], [turn on debugging [default="${debug_default}"]])],
              [case "${enableval}" in
                 yes) debug=yes ;;
                 no)  debug=no  ;;
                 *) AC_MSG_ERROR([bad value ${enableval} for --enable-debug]) ;;
               esac],
              [debug=$debug_default])
AM_CONDITIONAL([DEBUG], [test x$debug = xyes])
if test x$debug = xyes; then
  CXXFLAGS_DEBUG="-g -O0 -fno-inline -Wall -D_DEBUG"
  AC_MSG_RESULT([yes])
else
#  CXXFLAGS_DEBUG="-O3 -Werror -DACE_NDEBUG"
  CXXFLAGS_DEBUG="-O3 -Wall -DACE_NDEBUG"
  AC_MSG_RESULT([no])
fi

CXXFLAGS_TRACING=
# --> enable tracing support ?
AC_MSG_CHECKING([enable tracing])
tracing_default="no"
AC_ARG_ENABLE([tracing],
              [AS_HELP_STRING([--enable-tracing], [turn on tracing [default="${tracing_default}"]])],
              [case "${enableval}" in
                 yes) tracing=yes ;;
                 no)  tracing=no  ;;
                 *) AC_MSG_ERROR([bad value ${enableval} for --enable-tracing]) ;;
               esac],
              [tracing=$tracing_default])
AM_CONDITIONAL([TRACING], [test x$tracing = xyes])
if test x$tracing = xyes; then
#  CXXFLAGS_TRACING="-DACE_NTRACE=0 -DCOMMON_NTRACE=0"
  CXXFLAGS_TRACING="-DCOMMON_NTRACE=0"
  AC_MSG_RESULT([yes])
else
  CXXFLAGS_TRACING="-DACE_NTRACE=1 -DCOMMON_NTRACE=1"
  AC_MSG_RESULT([no])
fi

CXXFLAGS_VALGRIND=
# --> enable valgrind support ?
AC_MSG_CHECKING([enable valgrind support])
valgrind_support_default="no"
AC_ARG_ENABLE([valgrind-support],
              [AS_HELP_STRING([--enable-valgrind_support], [turn on valgrind support [default="$valgrind_support_default"]])],
              [case "${enableval}" in
                 yes) valgrind_support=yes ;;
                 no)  valgrind_support=no  ;;
                 *) AC_MSG_ERROR([bad value ${enableval} for --enable-valgrind-support]) ;;
               esac],
              [valgrind_support=$valgrind_support_default])
AM_CONDITIONAL([VALGRIND_SUPPORT], [test x$valgrind_support = xyes])
if test x$valgrind_support = xyes; then
  CXXFLAGS_VALGRIND="-DLIBCOMMON_ENABLE_VALGRIND_SUPPORT"
  AC_MSG_RESULT([yes])
else
  AC_MSG_RESULT([no])
fi

# export variables (so Makefile substitutions can be made)
#CXXFLAGS_BASE="-ansi -D_POSIX_C_SOURCE=199506L -D_REENTRANT -D_GNU_SOURCE"
CXXFLAGS_BASE="-ansi -std=c++11 -Wall -D_GNU_SOURCE"
CXXFLAGS="${CXXFLAGS_BASE} ${CXXFLAGS_DEBUG} ${CXXFLAGS_TRACING} ${CXXFLAGS_VALGRIND}"
AC_SUBST(CXXFLAGS)

# build modules ?
AC_MSG_CHECKING([enable module support])
module_support_default="yes"
AC_ARG_ENABLE([module-support],
              [AS_HELP_STRING([--enable-module_support], [compile submodules [default="$module_support_default"]])],
              [case "${enableval}" in
                 yes) module_support=yes ;;
                 no)  module_support=no  ;;
                 *) AC_MSG_ERROR([bad value ${enableval} for --enable-module-support]) ;;
               esac],
              [module_support=$module_support_default])
AM_CONDITIONAL([BUILD_MODULES], [test x$module_support = xyes])
if test x$module_support = xyes; then
  AC_MSG_RESULT([yes])
else
  AC_MSG_RESULT([no])
fi
dnl AM_COND_IF([BUILD_MODULES],
dnl           [AC_CONFIG_SUBDIRS([modules/ATCD])])
AM_COND_IF([BUILD_MODULES],
           [AC_CONFIG_FILES([modules/Makefile])])

# checks for typedefs, structures, and compiler characteristics
AC_TYPE_SIZE_T
AC_TYPE_UID_T

# checks for header files
AM_COND_IF([VALGRIND_SUPPORT],
           [AC_CHECK_HEADER([valgrind/valgrind.h])])
AC_CHECK_HEADER_STDBOOL

# checks for library functions
AC_CHECK_FUNCS([strerror])
AC_CHECK_FUNCS([getcwd])
AC_CHECK_FUNCS([gettimeofday])
AC_CHECK_FUNCS([memset])
AC_CHECK_FUNCS([mkdir])
AC_CHECK_FUNCS([uname])
AC_CHECK_FUNCS([realpath])

# checks for libraries
AM_COND_IF([VALGRIND_SUPPORT],
           [PKG_CHECK_MODULES([PKGVALGRIND], [valgrind])])
PKG_CHECK_MODULES([PKGSDL], [sdl])
PKG_CHECK_MODULES([PKGGTK], [libpng gmodule-2.0 gthread-2.0 gtk+-2.0 libglade-2.0])

# checks for other libraries (non-pkg-config)
# FIXME: Replace `main' with a function in `-lpthread':
#AC_CHECK_LIB([pthread], [pthread_create])
AC_SEARCH_LIBS([pthread_create], [pthread], , AC_MSG_ERROR([unable to find pthread library]))
# FIXME: Replace `main' with a function in `-lACE':
#AC_CHECK_LIB([ACE], [ACE_OS::memset])
#AC_SEARCH_LIBS([ACE_OS::memset], [ACE], , AC_MSG_ERROR([unable to find ACE library]))

# checks for tools

AC_CONFIG_HEADERS([config.h])

AC_CONFIG_FILES([Makefile
                 3rd_party/Makefile
                 3rd_party/ACE_wrappers/Makefile
                 3rd_party/ACE_wrappers/ace/Makefile
                 3rd_party/ACE_wrappers/include/Makefile
                 3rd_party/ACE_wrappers/include/makeinclude/Makefile
                 m4/Makefile
                 scripts/Makefile
                 src/Makefile
                 src/ui/Makefile])
AC_CONFIG_FILES([scripts/libCommon.pc])

AC_OUTPUT

