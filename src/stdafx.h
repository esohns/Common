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
#endif

// *NOTE*: nmake complains (see also:
//         C:\Program Files (x86)\Windows Kits\8.1\include\shared\sspi.h(64))
#if defined (_MSC_VER)
#define SECURITY_WIN32
#endif

// System Library Header Files
//#include "ace/streams.h"
#include <ace/Assert.h>
//#include "ace/ACE.h"
//#include "ace/CDR_Stream.h"
#include <ace/Log_Msg.h>
#include <ace/OS.h>
//#include "ace/Singleton.h"
//#include "ace/Synch.h"

// C RunTime Header Files
//#include <deque>
//#include <functional>
//#include <locale>
//#include <map>
//#include <string>
//#include <set>
//#include <sstream>
//#include <stack>
//#include <vector>

#if defined (LIBCOMMON_ENABLE_VALGRIND_SUPPORT)
#include <valgrind/memcheck.h>
#endif

// Local Header Files
#include "common_macros.h"
