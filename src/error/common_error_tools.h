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

#ifndef COMMON_ERROR_TOOLS_H
#define COMMON_ERROR_TOOLS_H

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include <string>
#endif // ACE_WIN32 || ACE_WIN64

#include "ace/Global_Macros.h"

#include "common_iinitialize.h"

#if defined (ACE_WIN32) || defined (ACE_WIN64)
LONG WINAPI
common_error_win32_seh_filter_core_dump (unsigned int,
                                         struct _EXCEPTION_POINTERS*);
LONG WINAPI
common_error_win32_default_seh_handler (struct _EXCEPTION_POINTERS*);

#if defined (_DEBUG)
int
common_error_win32_debugheap_message_hook (int,
                                           char*,
                                           int*);
#endif // _DEBUG
#endif // ACE_WIN32 || ACE_WIN64

void
common_error_default_terminate_function ();

class Common_Error_Tools
 : public Common_SInitializeFinalize_T<Common_Error_Tools>
{
 public:
  static void initialize ();
  static void finalize ();

  // --- debug ---
  static bool inDebugSession ();
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  // *WARNING*: limited to 9 characters
  static void setThreadName (const std::string&, // thread name
                             DWORD = 0);         // thread id (0: caller)

#if defined (_DEBUG)
  // debug heap
  static ACE_HANDLE              debugHeapLogFileHandle;
  static HMODULE                 debugHelpModule;
  typedef int (WINAPI *MiniDumpWriteDumpFunc_t)(HANDLE,
                                                DWORD,
                                                HANDLE,
                                                enum _MINIDUMP_TYPE,
                                                struct _MINIDUMP_EXCEPTION_INFORMATION*,
                                                struct _MINIDUMP_USER_STREAM_INFORMATION*,
                                                struct _MINIDUMP_CALLBACK_INFORMATION*
                                               );
  static MiniDumpWriteDumpFunc_t miniDumpWriteDumpFunc;
#endif // _DEBUG
#endif // ACE_WIN32 || ACE_WIN64

  // --- core dump ---
  static bool enableCoreDump (bool = true); // enable ? : disable
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  static bool generateCoreDump (const std::string&,                      // program name
                                const struct Common_ApplicationVersion&, // program version
                                struct _EXCEPTION_POINTERS*);            // return value of GetExceptionInformation()
#endif // ACE_WIN32 || ACE_WIN64

  // --- error messages ---
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  static std::string errorToString (DWORD,         // error
                                    bool = false,  // ? use AMGetErrorText() : use FormatMessage()
                                    bool = false); // WinInet error ?
#endif // ACE_WIN32 || ACE_WIN64

 private:
  ACE_UNIMPLEMENTED_FUNC (Common_Error_Tools ())
  ACE_UNIMPLEMENTED_FUNC (~Common_Error_Tools ())
  ACE_UNIMPLEMENTED_FUNC (Common_Error_Tools (const Common_Error_Tools&))
  ACE_UNIMPLEMENTED_FUNC (Common_Error_Tools& operator= (const Common_Error_Tools&))

  // helper methods
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#if defined (_DEBUG)
  static bool initializeDebugHeap ();
  static void finalizeDebugHeap ();
#endif // _DEBUG
#endif // ACE_WIN32 || ACE_WIN64
};

#endif
