
// -*- C++ -*-
// $Id$
// Definition for Win32 Export directives.
// This file is generated automatically by generate_export_file.pl -d CommonERR
// ------------------------------
#ifndef COMMONERR_EXPORT_H
#define COMMONERR_EXPORT_H

#include "ace/config-all.h"

#if defined (ACE_AS_STATIC_LIBS) && !defined (COMMONERR_HAS_DLL)
#  define COMMONERR_HAS_DLL 0
#endif /* ACE_AS_STATIC_LIBS && COMMONERR_HAS_DLL */

#if !defined (COMMONERR_HAS_DLL)
#  define COMMONERR_HAS_DLL 1
#endif /* ! COMMONERR_HAS_DLL */

#if defined (COMMONERR_HAS_DLL) && (COMMONERR_HAS_DLL == 1)
#  if defined (COMMONERR_BUILD_DLL)
#    define CommonERR_Export ACE_Proper_Export_Flag
#    define COMMONERR_SINGLETON_DECLARATION(T) ACE_EXPORT_SINGLETON_DECLARATION (T)
#    define COMMONERR_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK) ACE_EXPORT_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK)
#  else /* COMMONERR_BUILD_DLL */
#    define CommonERR_Export ACE_Proper_Import_Flag
#    define COMMONERR_SINGLETON_DECLARATION(T) ACE_IMPORT_SINGLETON_DECLARATION (T)
#    define COMMONERR_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK) ACE_IMPORT_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK)
#  endif /* COMMONERR_BUILD_DLL */
#else /* COMMONERR_HAS_DLL == 1 */
#  define CommonERR_Export
#  define COMMONERR_SINGLETON_DECLARATION(T)
#  define COMMONERR_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK)
#endif /* COMMONERR_HAS_DLL == 1 */

// Set COMMONERR_NTRACE = 0 to turn on library specific tracing even if
// tracing is turned off for ACE.
#if !defined (COMMONERR_NTRACE)
#  if (ACE_NTRACE == 1)
#    define COMMONERR_NTRACE 1
#  else /* (ACE_NTRACE == 1) */
#    define COMMONERR_NTRACE 0
#  endif /* (ACE_NTRACE == 1) */
#endif /* !COMMONERR_NTRACE */

#if (COMMONERR_NTRACE == 1)
#  define COMMONERR_TRACE(X)
#else /* (COMMONERR_NTRACE == 1) */
#  if !defined (ACE_HAS_TRACE)
#    define ACE_HAS_TRACE
#  endif /* ACE_HAS_TRACE */
#  define COMMONERR_TRACE(X) ACE_TRACE_IMPL(X)
#  include "ace/Trace.h"
#endif /* (COMMONERR_NTRACE == 1) */

#endif /* COMMONERR_EXPORT_H */

// End of auto generated file.
