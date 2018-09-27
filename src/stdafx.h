// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//
#pragma once

#if defined (_MSC_VER)
// Exclude rarely-used stuff from Windows headers
#define WIN32_LEAN_AND_MEAN
// <limits> does not work correctly with these macros
#define NOMINMAX
// *NOTE*: nmake complains (see also:
//         C:\Program Files (x86)\Windows Kits\8.1\include\shared\sspi.h(64))
#define SECURITY_WIN32

#if defined (_DEBUG)
// *TODO*: currently, these do not work well with ACE
//#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif // _DEBUG

// Windows Header Files
// *NOTE*: <windows.h> #includes <sdkddkver.h>
#include <windows.h>
#if defined (_DEBUG)
#include "common_macros.h"
#include "common_pragmas.h"
COMMON_PRAGMA_MESSAGE("toolchain version (_MSC_VER): " COMMON_MAKESTRING(COMMON_XSTRINGIZE,_MSC_VER))
COMMON_PRAGMA_MESSAGE("SDK version (_WIN32_WINNT): " COMMON_MAKESTRING(COMMON_XSTRINGIZE,_WIN32_WINNT))
COMMON_PRAGMA_MESSAGE("target platform (WINVER): " COMMON_MAKESTRING(COMMON_XSTRINGIZE,WINVER))
#endif // _DEBUG
#endif // _MSC_VER

// C++ RunTime Header Files
#include <string>

// System Library Header Files
#include "ace/config-lite.h"
#include "ace/Global_Macros.h"
#include "ace/Log_Msg.h"

#if defined (VALGRIND_SUPPORT)
#include "valgrind/valgrind.h"
#endif // VALGRIND_SUPPORT

#if defined (HAVE_CONFIG_H)
#include "libCommon_config.h"
#endif // HAVE_CONFIG_H

// Local Header Files
#include "common.h"
#include "common_macros.h"
#include "common_pragmas.h"
