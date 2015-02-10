// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//
#if defined _MSC_VER
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

// Windows Header Files
#include <windows.h>
#endif

// System Library Header Files
#include "ace/OS.h"
#include "ace/ACE.h"
#include "ace/Log_Msg.h"
#include "ace/Assert.h"
#include "ace/Synch.h"
#include "ace/Singleton.h"
#include "ace/CDR_Stream.h"

// C RunTime Header Files
#include <string>
#include <map>
#include <vector>
#include <set>
#include <deque>
#include <locale>
#include <sstream>
#include <stack>
#include <functional>

// Local Header Files
#include "common_macros.h"
