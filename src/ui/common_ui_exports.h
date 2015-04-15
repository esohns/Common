
// -*- C++ -*-
// $Id$
// Definition for Win32 Export directives.
// This file is generated automatically by generate_export_file.pl -n Common_UI
// ------------------------------
#ifndef COMMON_UI_EXPORT_H
#define COMMON_UI_EXPORT_H

#include "ace/config-all.h"

#if !defined (COMMON_UI_HAS_DLL)
#  define COMMON_UI_HAS_DLL 1
#endif /* ! COMMON_UI_HAS_DLL */

#if defined (COMMON_UI_HAS_DLL) && (COMMON_UI_HAS_DLL == 1)
#  if defined (COMMON_UI_BUILD_DLL)
#    define Common_UI_Export ACE_Proper_Export_Flag
#    define COMMON_UI_SINGLETON_DECLARATION(T) ACE_EXPORT_SINGLETON_DECLARATION (T)
#    define COMMON_UI_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK) ACE_EXPORT_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK)
#  else /* COMMON_UI_BUILD_DLL */
#    define Common_UI_Export ACE_Proper_Import_Flag
#    define COMMON_UI_SINGLETON_DECLARATION(T) ACE_IMPORT_SINGLETON_DECLARATION (T)
#    define COMMON_UI_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK) ACE_IMPORT_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK)
#  endif /* COMMON_UI_BUILD_DLL */
#else /* COMMON_UI_HAS_DLL == 1 */
#  define Common_UI_Export
#  define COMMON_UI_SINGLETON_DECLARATION(T)
#  define COMMON_UI_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK)
#endif /* COMMON_UI_HAS_DLL == 1 */

// Set COMMON_UI_NTRACE = 0 to turn on library specific tracing even if
// tracing is turned off for ACE.
#if !defined (COMMON_UI_NTRACE)
#  if (ACE_NTRACE == 1)
#    define COMMON_UI_NTRACE 1
#  else /* (ACE_NTRACE == 1) */
#    define COMMON_UI_NTRACE 0
#  endif /* (ACE_NTRACE == 1) */
#endif /* !COMMON_UI_NTRACE */

#if (COMMON_UI_NTRACE == 1)
#  define COMMON_UI_TRACE(X)
#else /* (COMMON_UI_NTRACE == 1) */
#  if !defined (ACE_HAS_TRACE)
#    define ACE_HAS_TRACE
#  endif /* ACE_HAS_TRACE */
#  define COMMON_UI_TRACE(X) ACE_TRACE_IMPL(X)
#  include "ace/Trace.h"
#endif /* (COMMON_UI_NTRACE == 1) */

#endif /* COMMON_UI_EXPORT_H */

// End of auto generated file.
