#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_INIT([libCommon],[0.1],[eriksohns@123mail.org],[libCommon],[http://www.github.com/esohns/libCommon])
AC_PREREQ([2.69])
AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_AUX_DIR([aux_config])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

# define meta-package
META_PACKAGE_NAME=libCommon
AC_DEFINE([META_PACKAGE_NAME], ["libCommon"], [meta-package-name])

# get system information
AC_CANONICAL_TARGET

AC_LANG([C++])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_RANLIB
AM_PROG_AR
LT_INIT

# init other tools.
AM_INIT_AUTOMAKE
# build time sanity check...
AM_SANITY_CHECK
LT_INIT

# prefix config.h
AX_PREFIX_CONFIG_H

# reset the cpp compiler flags
CXXFLAGS=
# this macro is used to get the arguments supplied
# to the configure script (./configure --enable-debug)
# Check if we have enable debug support.
AC_MSG_CHECKING(whether to enable debugging)
debug_default="no"
AC_ARG_ENABLE(debug,
              [ --enable-debug=[no/yes] turn on debugging [default=$debug_default]],
              ,
              enable_debug=$debug_default)
if test "x$enable_debug" = "xyes"; then
  CXXFLAGS="-g -O0 -Wall -D_DEBUG"
  AC_MSG_RESULT(yes)
else
  CXXFLAGS="-O3 -Wall -Werror"
  AC_MSG_RESULT(no)
fi

# this macro is used to get the arguments supplied
# to the configure script (./configure --enable-tracing)
# Check if we have enable tracing support.
AC_MSG_CHECKING(whether to enable tracing)
tracing_default="no"
AC_ARG_ENABLE(tracing,
              [ --enable-tracing=[no/yes] turn on tracing [default=$tracing_default]],
              ,
              enable_tracing=$tracing_default)
if test "x$enable_tracing" = "xyes"; then
  CXXFLAGS="${CXXFLAGS} -DLIBCOMMON_NTRACE=0"
  AC_MSG_RESULT(yes)
else
  CXXFLAGS="${CXXFLAGS} -DLIBCOMMON_NTRACE=1"
  AC_MSG_RESULT(no)
fi

# export variables (so Makefile substitutions can be made)
AC_SUBST(META_PACKAGE_NAME)
AC_SUBST(CXXFLAGS)

# Checks for libraries.
# FIXME: Replace `main' with a function in `-lACE':
AC_CHECK_LIB([ACE], [main])

# Checks for header files.
AC_HEADER_STDBOOL

# Checks for typedefs, structures, and compiler characteristics.
dnl AC_C_INLINE
AC_TYPE_SIZE_T
AC_TYPE_UID_T

# Checks for library functions.
dnl AC_FUNC_ERROR_AT_LINE
AC_CHECK_FUNCS([strerror])
AC_CHECK_FUNCS([getcwd])
AC_CHECK_FUNCS([gettimeofday])
AC_CHECK_FUNCS([memset])
AC_CHECK_FUNCS([mkdir])
AC_CHECK_FUNCS([uname])
AC_CHECK_FUNCS([realpath])

AC_CONFIG_FILES([Makefile
                 scripts/Makefile
                 src/Makefile])

AC_OUTPUT
