// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//
#pragma once

#if defined (_MSC_VER)
// Exclude rarely-used stuff from Windows headers
#define WIN32_LEAN_AND_MEAN

// Windows Header Files
// *NOTE*: <windows.h> #includes <sdkddkver.h>
#include <windows.h>
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
