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

#ifdef __GNUC__
#define likely(X) __builtin_expect (!!(X), 1)
#define unlikely(X) __builtin_expect (!!(X), 0)
#else
#define likely(X) X
#define unlikely(X) X
#endif

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
#  define COMMON_TRACE (X) COMMON_TRACE_IMPL(X)
#  include "ace/Trace.h"
#endif /* COMMON_NTRACE */

#ifdef __GNUC__
#define GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#endif
#define COMPILER_NAME ACE::compiler_name()
#define COMPILER_VERSION (ACE::compiler_major_version() * 10000 + ACE::compiler_minor_version() * 100 + ACE::compiler_beta_version())

#define COMMON_XSTRINGIZE(X) #X
#define COMMON_STRINGIZE(X) COMMON_XSTRINGIZE(X)

#endif
