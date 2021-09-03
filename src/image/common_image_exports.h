
// -*- C++ -*-
// $Id$
// Definition for Win32 Export directives.
// This file is generated automatically by generate_export_file.pl -d Common_Image
// ------------------------------
#ifndef COMMON_IMAGE_EXPORT_H
#define COMMON_IMAGE_EXPORT_H

#include "ace/config-all.h"

#if defined (ACE_AS_STATIC_LIBS) && !defined (COMMON_IMAGE_HAS_DLL)
#  define COMMON_IMAGE_HAS_DLL 0
#endif /* ACE_AS_STATIC_LIBS && COMMON_IMAGE_HAS_DLL */

#if !defined (COMMON_IMAGE_HAS_DLL)
#  define COMMON_IMAGE_HAS_DLL 1
#endif /* ! COMMON_IMAGE_HAS_DLL */

#if defined (COMMON_IMAGE_HAS_DLL) && (COMMON_IMAGE_HAS_DLL == 1)
#  if defined (COMMON_IMAGE_BUILD_DLL)
#    define Common_Image_Export ACE_Proper_Export_Flag
#    define COMMON_IMAGE_SINGLETON_DECLARATION(T) ACE_EXPORT_SINGLETON_DECLARATION (T)
#    define COMMON_IMAGE_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK) ACE_EXPORT_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK)
#  else /* COMMON_IMAGE_BUILD_DLL */
#    define Common_Image_Export ACE_Proper_Import_Flag
#    define COMMON_IMAGE_SINGLETON_DECLARATION(T) ACE_IMPORT_SINGLETON_DECLARATION (T)
#    define COMMON_IMAGE_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK) ACE_IMPORT_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK)
#  endif /* COMMON_IMAGE_BUILD_DLL */
#else /* COMMON_IMAGE_HAS_DLL == 1 */
#  define Common_Image_Export
#  define COMMON_IMAGE_SINGLETON_DECLARATION(T)
#  define COMMON_IMAGE_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK)
#endif /* COMMON_IMAGE_HAS_DLL == 1 */

// Set COMMON_IMAGE_NTRACE = 0 to turn on library specific tracing even if
// tracing is turned off for ACE.
#if !defined (COMMON_IMAGE_NTRACE)
#  if (ACE_NTRACE == 1)
#    define COMMON_IMAGE_NTRACE 1
#  else /* (ACE_NTRACE == 1) */
#    define COMMON_IMAGE_NTRACE 0
#  endif /* (ACE_NTRACE == 1) */
#endif /* !COMMON_IMAGE_NTRACE */

#if (COMMON_IMAGE_NTRACE == 1)
#  define COMMON_IMAGE_TRACE(X)
#else /* (COMMON_IMAGE_NTRACE == 1) */
#  if !defined (ACE_HAS_TRACE)
#    define ACE_HAS_TRACE
#  endif /* ACE_HAS_TRACE */
#  define COMMON_IMAGE_TRACE(X) ACE_TRACE_IMPL(X)
#  include "ace/Trace.h"
#endif /* (COMMON_IMAGE_NTRACE == 1) */

#endif /* COMMON_IMAGE_EXPORT_H */

// End of auto generated file.
