// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//
#if defined (_MSC_VER)
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

// Windows Header Files
#include <windows.h>

#if defined (_DEBUG)
// *TODO*: currently, these do not work well with ACE...
//#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

// *NOTE*: nmake complains (see also:
//         C:\Program Files (x86)\Windows Kits\8.1\include\shared\sspi.h(64))
#define SECURITY_WIN32
#endif // _MSC_VER

// System Library Header Files
#include "ace/config-lite.h"
#include "ace/Global_Macros.h"
#include "ace/Log_Msg.h"

// C RunTime Header Files
//#include <deque>
//#include <functional>
//#include <locale>
//#include <map>
#include <string>
//#include <set>
//#include <sstream>
//#include <stack>
//#include <vector>

#if defined (VALGRIND_SUPPORT)
#include "valgrind/memcheck.h"
#endif // VALGRIND_SUPPORT

// Local Header Files
#if defined (HAVE_CONFIG_H)
#include "Common_config.h"
#endif // HAVE_CONFIG_H
#include "common_macros.h"
