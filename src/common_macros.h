/***************************************************************************
 *   Copyright (C) 2009 by Erik Sohns   *
 *   erik.sohns@web.de   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef COMMON_MACROS_H
#define COMMON_MACROS_H

#include "ace/config-lite.h"

// branch prediction //

#if defined (__GNUC__)
#define likely(X) __builtin_expect (!!(X), 1)
#define unlikely(X) __builtin_expect (!!(X), 0)
#else
#define likely(X) X
#define unlikely(X) X
#endif // __GNUC__

// exception handling //

#if defined (__GNUC__)
#if defined (EXCEPTION_CPP_USE)
#define COMMON_TRY try
#define COMMON_CATCH(X) catch (X)
#else
#define COMMON_TRY if (1)
#define COMMON_CATCH(X) while (0)
#endif // EXCEPTION_CPP_USE
#elif defined (_MSC_VER)
#if defined (EXCEPTION_CPP_USE)
#define COMMON_TRY try
#define COMMON_CATCH(X) catch (X)
#elif defined (EXCEPTION_SEH_USE)
#define COMMON_DEFAULT_SEH(X) common_win32_seh_filter (X)
#define COMMON_DEFAULT_SEH_ARG_1 GetExceptionCode ()
#define COMMON_DEFAULT_SEH_ARG_2 GetExceptionInformation ()
#define COMMON_TRY ACE_SEH_TRY
#define COMMON_SEH_CATCH(X) ACE_SEH_EXCEPT (X)
#define COMMON_CATCH(X) COMMON_SEH_CATCH (COMMON_DEFAULT_SEH(COMMON_DEFAULT_SEH_ARG_1, COMMON_DEFAULT_SEH_ARG_2))
#else
#define COMMON_TRY if (1)
#define COMMON_CATCH(X) while (0)
#endif
#else
#error invalid/unknown C++ compiler; not supported, check implementation
#endif

// platform (OS) //

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#define COMMON_OS_WIN32_SDK_VERSION(version)                  (_WIN32_WINNT && (_WIN32_WINNT >= version))
#define COMMON_OS_WIN32_TARGET_PLATFORM(version)              (WINVER && (WINVER >= version))
#elif defined (ACE_LINUX)
#define COMMON_OS_LINUX_IF_DISTRIBUTION_AT_LEAST(distribution, major,minor,micro)    \
  unsigned int major_i, minor_i, micro_i;                                            \
  if ((distribution == Common_Tools::getDistribution (major_i, minor_i, micro_i)) && \
      ((major_i > major)                                             ||              \
       ((major_i == major) && minor_i > minor)                       ||              \
       ((major_i == major) && (minor_i == minor) && (micro_i >= micro))))
#endif // ACE_WIN32 || ACE_WIN64

// tracing //

#define COMMON_TRACE_IMPL(X) ACE_Trace ____ (ACE_TEXT (X), __LINE__, ACE_TEXT (__FILE__))

// by default tracing is turned off
#if !defined (COMMON_NTRACE)
#  define COMMON_NTRACE 1
#endif /* COMMON_NTRACE */

#if (COMMON_NTRACE == 1)
#  define COMMON_TRACE(X)
#else
#  if !defined (COMMON_HAS_TRACE)
#    define COMMON_HAS_TRACE
#  endif /* COMMON_HAS_TRACE */
#  define COMMON_TRACE(X) COMMON_TRACE_IMPL(X)
#  include "ace/Trace.h"
#endif /* COMMON_NTRACE */

// compiler //

#if defined (__GNUC__)
#define GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#endif // __GNUC__
#define COMPILER_NAME Common_Tools::compilerName ()
#define COMPILER_VERSION (ACE::compiler_major_version() * 10000 + ACE::compiler_minor_version() * 100 + ACE::compiler_beta_version())
#define COMPILATION_DATE_TIME __DATE__ __TIME__

// strings //

#define COMMON_XSTRINGIZE(X) #X
#define COMMON_STRINGIZE(X) COMMON_XSTRINGIZE(X)

#define COMMON_MAKESTRING(M,L) M (L)

#define COMMON_STRING_APPEND(string,string_2) \
  string += ACE_TEXT_ALWAYS_CHAR (string_2)

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#if defined (UNICODE)
#define COMMON_TEXT(X) ACE_TEXT_WCHAR_TO_TCHAR (X)
#else
#define COMMON_TEXT(X) ACE_TEXT (X)
#endif // UNICODE
#else
#define COMMON_COMMAND_ADD_SWITCH(command,switch_)  \
  COMMON_STRING_APPEND(command," -");               \
  command += ACE_TEXT_ALWAYS_CHAR (switch_);
#define COMMON_COMMAND_START_IN_BACKGROUND(command) \
  command += COMMON_STRING_APPEND(command, &);
#endif // ACE_WIN32 || ACE_WIN64

// memory allocation
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#define MALLOC(x) HeapAlloc (GetProcessHeap (), 0, (x))
#define FREE(x) HeapFree (GetProcessHeap (), 0, (x))
#endif // ACE_WIN32 || ACE_WIN64

// application //

#define COMMON_CHECK_VERSION(major,minor,micro)                                                              \
  ((Common_VERSION_MAJOR > major)                                                                         || \
   ((Common_VERSION_MAJOR == major) && (Common_VERSION_MINOR > minor))                                    || \
   ((Common_VERSION_MAJOR == major) && (Common_VERSION_MINOR == minor) && (Common_VERSION_MICRO >= micro)))

// *NOTE*: arguments must be strings
#define COMMON_MAKE_VERSION_STRING_VARIABLE(program,version,variable) std::string variable; do {                      \
  variable = program; variable += ACE_TEXT_ALWAYS_CHAR (" ");                                                         \
  variable += version; variable += ACE_TEXT_ALWAYS_CHAR (" compiled on ");                                            \
  variable += ACE_TEXT_ALWAYS_CHAR (COMPILATION_DATE_TIME);                                                           \
  variable += ACE_TEXT_ALWAYS_CHAR (" host platform "); variable += Common_Tools::compilerPlatformName ();            \
  variable += ACE_TEXT_ALWAYS_CHAR (" with "); variable += Common_Tools::compilerName ();                             \
  variable += ACE_TEXT_ALWAYS_CHAR (" "); variable += Common_Tools::compilerVersion ();                               \
  variable += ACE_TEXT_ALWAYS_CHAR (" against ACE "); variable += Common_Tools::compiledVersion_ACE ();               \
  variable += ACE_TEXT_ALWAYS_CHAR (" , libCommon "); variable += ACE_TEXT_ALWAYS_CHAR (Common_PACKAGE_VERSION_FULL); \
} while (0)

#define COMMON_DEFINE_PRINTVERSION_FUNCTION(function,header,variable)                                          \
  void function (const std::string& programName_in) { header; std::cout << variable << std::endl << std::endl; }

#endif
