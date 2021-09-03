// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//
#if defined (_MSC_VER)
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

// <limits> does not work correctly with these macros
#define NOMINMAX

// Windows Header Files
#include <windows.h>

#if defined (_DEBUG)
// *TODO*: currently, these do not work well with ACE...
//#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif // _MSC_VER

// *NOTE*: nmake complains (see also:
//         C:\Program Files (x86)\Windows Kits\8.1\include\shared\sspi.h(64))
#define SECURITY_WIN32
#endif

// C++ RunTime Header Files
#include <string>

// System Library Header Files

// Library Header Files
#include "ace/os_include/sys/os_types.h"
#include "wx/wx.h"

//#define ACE_HAS_SSIZE_T
#include "ace/config-lite.h"
//#undef ACE_LACKS_MODE_T
#include "ace/Global_Macros.h"
//#include "ace/Log_Msg.h"
#include "ace/OS.h"

#if defined (VALGRIND_SUPPORT)
#include "valgrind/valgrind.h"
#endif // VALGRIND_SUPPORT

// Local Header Files
#if defined (HAVE_CONFIG_H)
#include "Common_config.h"
#endif // HAVE_CONFIG_H
#include "common.h"
#include "common_macros.h"
#include "common_pragmas.h"
