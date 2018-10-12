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
#include "stdafx.h"

#include "common_error_tools.h"

#include <exception>
#include <sstream>

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include <DbgHelp.h>
//#include <DxErr.h>
#include <errors.h>
#include <strsafe.h>
#endif // ACE_WIN32 || ACE_WIN64

#include "ace/File_Connector.h"
#include "ace/Log_Msg.h"
#include "ace/OS.h"

#include "common_macros.h"

#include "common_log_tools.h"

#include "common_error_defines.h"
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include "common_error_directx.h"
#endif // ACE_WIN32 || ACE_WIN64

// initialize statics
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#if defined (_DEBUG)
ACE_HANDLE Common_Error_Tools::debugHeapLogFileHandle = ACE_INVALID_HANDLE;
HMODULE Common_Error_Tools::debugHelpModule = NULL;
Common_Error_Tools::MiniDumpWriteDumpFunc_t Common_Error_Tools::miniDumpWriteDumpFunc = NULL;
#endif // _DEBUG
#endif // ACE_WIN32 || ACE_WIN64

//////////////////////////////////////////

#if defined (ACE_WIN32) || defined (ACE_WIN64)
LONG WINAPI
common_error_win32_seh_filter_core_dump (unsigned int exceptionCode_in,
                                         struct _EXCEPTION_POINTERS* exceptionInformation_in)
{
  COMMON_TRACE (ACE_TEXT ("::common_error_win32_seh_filter_core_dump"));

  ACE_UNUSED_ARG (exceptionCode_in);

  // *TODO*: pass application information into the exception handler
  struct Common_ApplicationVersion application_version;
  ACE_OS::memset (&application_version, 0, sizeof (struct Common_ApplicationVersion));
  if (!Common_Error_Tools::generateCoreDump (ACE_TEXT_ALWAYS_CHAR (Common_PACKAGE_NAME),
                                             application_version,
                                             exceptionInformation_in))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_Error_Tools::generateCoreDump(), continuing\n")));
    return EXCEPTION_CONTINUE_SEARCH;
  } // end IF
#if defined (_DEBUG)
  else
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("caught Win32 structured exception, core dumped\n")));
#endif // _DEBUG

  return EXCEPTION_EXECUTE_HANDLER;
}

LONG WINAPI
common_error_win32_default_seh_handler (struct _EXCEPTION_POINTERS* exceptionInformation_in)
{
  COMMON_TRACE (ACE_TEXT ("::common_error_win32_default_seh_handler"));

  // sanity check(s)
  ACE_ASSERT (exceptionInformation_in);
  ACE_ASSERT (exceptionInformation_in->ExceptionRecord);

#ifdef _M_IX86
  if (exceptionInformation_in->ExceptionRecord->ExceptionCode == EXCEPTION_STACK_OVERFLOW)  
  {
    // be sure that we have enought space...
    static char MyStack[1024*128];  
    // it assumes that DS and SS are the same!!! (this is the case for Win32)
    // change the stack only if the selectors are the same (this is the case for Win32)
    //__asm push offset MyStack[1024*128];
    //__asm pop esp;
    __asm mov eax,offset MyStack[1024*128];
    __asm mov esp,eax;
  } // end IF
#endif // _M_IX86

  LONG result =
#if defined (_DEBUG)
    common_error_win32_seh_filter_core_dump (exceptionInformation_in->ExceptionRecord->ExceptionCode,
                                             exceptionInformation_in);
#else
    0;
#endif // _DEBUG

  // Optional display an error message
  FatalAppExit (-1, ACE_TEXT ("Application failed!"));

  // or return one of the following:
  // - EXCEPTION_CONTINUE_SEARCH
  // - EXCEPTION_CONTINUE_EXECUTION
  // - EXCEPTION_EXECUTE_HANDLER
  return result;
}

#if defined (_DEBUG)
int
common_error_win32_debugheap_message_hook (int reportType_in,
                                           char* message_in,
                                           int* returnValue_out)
{
  COMMON_TRACE (ACE_TEXT ("::common_error_win32_debugheap_message_hook"));

  // translate loglevel
  ACE_Log_Priority log_priority = LM_ERROR;
  switch (reportType_in)
  {
    case _CRT_ASSERT:
    case _CRT_ERROR:
    case _CRT_ERRCNT:
      break;
    case _CRT_WARN:
      log_priority = LM_WARNING;
      break;
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown debug heap log level (was: %d), continuing\n"),
                  reportType_in));
      break;
    }
  } // end SWITCH

  // *WARNING*: this call holds the CRT lock. The logger waits for its lock,
  //            causing deadlock if another thread has the lock and tries to
  //            free objects to the CRT...
  ACE_DEBUG ((log_priority,
              ACE_TEXT ("debug heap: %s\n"),
              ACE_TEXT (message_in)));

  if (likely (returnValue_out))
    *returnValue_out = 0;

  return FALSE; // <-- do not stop
}
#endif // _DEBUG
#endif // ACE_WIN32 || ACE_WIN64

void
common_error_default_terminate_function ()
{
  COMMON_TRACE (ACE_TEXT ("common_error_default_terminate_function"));

  ACE_OS::abort ();
}

//////////////////////////////////////////

void
Common_Error_Tools::initialize ()
{
  COMMON_TRACE (ACE_TEXT ("Common_Error_Tools::initialize"));

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#if defined (_DEBUG)
  if (!COMMON_ERROR_WIN32_DEFAULT_DEBUGHEAP ||
      (Common_Error_Tools::debugHeapLogFileHandle != ACE_INVALID_HANDLE))
    goto continue_;

  if (!Common_Error_Tools::initializeDebugHeap ())
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_Error_Tools::initializeDebugHeap(), returning\n")));
    return;
  } // end IF
  ACE_ASSERT (Common_Error_Tools::debugHeapLogFileHandle != ACE_INVALID_HANDLE);

continue_:
#endif /* _DEBUG */
#endif /* ACE_WIN32 || ACE_WIN64 */

  std::set_terminate (common_error_default_terminate_function);
}

void
Common_Error_Tools::finalize ()
{
  COMMON_TRACE (ACE_TEXT ("Common_Error_Tools::finalize"));

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  if (Common_Error_Tools::debugHelpModule)
    Common_Error_Tools::enableCoreDump (false);
  ACE_ASSERT (Common_Error_Tools::debugHelpModule == ACE_INVALID_HANDLE);
#endif /* ACE_WIN32 || ACE_WIN64 */

  std::set_terminate (NULL);

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#if defined (_DEBUG)
  if (Common_Error_Tools::debugHeapLogFileHandle != ACE_INVALID_HANDLE)
    Common_Error_Tools::finalizeDebugHeap ();
  ACE_ASSERT (Common_Error_Tools::debugHeapLogFileHandle == ACE_INVALID_HANDLE);
#endif /* _DEBUG */
#endif /* ACE_WIN32 || ACE_WIN64 */
}

bool
Common_Error_Tools::inDebugSession ()
{
  COMMON_TRACE (ACE_TEXT ("Common_Error_Tools::inDebugSession"));

  bool result = false;

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  result = IsDebuggerPresent ();
#elif defined (ACE_LINUX)
  int status_fd = ACE_INVALID_HANDLE;
  ACE_TCHAR buffer_a[BUFSIZ];
  ssize_t bytes_read = -1;
  int result_2 = -1;
  static const ACE_TCHAR tracer_pid_string[] = ACE_TEXT ("TracerPid:");
  ACE_TCHAR* tracer_pid_p = NULL;

  status_fd = ACE_OS::open (ACE_TEXT_ALWAYS_CHAR ("/proc/self/status"),
                            O_RDONLY);
  if (unlikely (status_fd == -1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::open(%s): \"%m\", aborting\n"),
                ACE_TEXT ("/proc/self/status")));
    return false; // *WARNING*: potentially false negative
  } // end IF
  ACE_OS::memset (buffer_a, 0, sizeof (ACE_TCHAR[BUFSIZ]));
  bytes_read = ACE_OS::read (status_fd,
                             buffer_a,
                             sizeof (ACE_TCHAR[BUFSIZ]) - 1);
  if (unlikely (bytes_read == -1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::read(%s): \"%m\", aborting\n"),
                ACE_TEXT ("/proc/self/status")));
    goto clean;
  } // end IF
//  buffer_a[bytes_read] = 0;
  tracer_pid_p = ACE_OS::strstr (buffer_a, tracer_pid_string);
  if (unlikely (!tracer_pid_p))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::strstr(%s): \"%m\", aborting\n"),
                tracer_pid_string));
    goto clean;
  } // end IF
  result = !!ACE_OS::atoi (tracer_pid_p + sizeof (tracer_pid_string) - 1);

clean:
  if (likely (status_fd != ACE_INVALID_HANDLE))
  {
    result_2 = ACE_OS::close (status_fd);
    if (unlikely (result_2 == -1))
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_OS::close(%s): \"%m\", continuing\n"),
                  ACE_TEXT ("/proc/self/status")));
  } // end IF
#else
  ACE_ASSERT (false);
  ACE_NOTSUP_RETURN (false);
  ACE_NOTREACHED (return false;)
#endif

  return result;
}

#if defined (ACE_WIN32) || defined (ACE_WIN64)
void
Common_Error_Tools::setThreadName (const std::string& name_in,
                                   DWORD threadId_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Error_Tools::setThreadName"));

  // sanity check(s)
  ACE_ASSERT (!name_in.empty ());

#if COMMON_OS_WIN32_TARGET_PLATFORM(0x0A00) // _WIN32_WINNT_WIN10
  HANDLE handle_h =
    (threadId_in ? reinterpret_cast<HANDLE> (threadId_in)
                 : ::GetCurrentThread ());
  HRESULT result =
    ::SetThreadDescription (handle_h,
                            ACE_TEXT_ALWAYS_WCHAR (name_in.c_str ()));
  if (unlikely (FAILED (result)))
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SetThreadDescription(%@): \"%s\", returning\n"),
                handle_h,
                ACE_TEXT (Common_Error_Tools::errorToString (result, false).c_str ())));
#else
#if defined (DEBUG_DEBUGGER)
  // *NOTE*: code based on MSDN article (see:
  //         https://msdn.microsoft.com/en-us/library/xcb2z8hs.aspx)
  const DWORD MS_VC_EXCEPTION = 0x406D1388;
#pragma pack (push, 8)
  typedef struct tagTHREADNAME_INFO
  {
    DWORD dwType;     // Must be 0x1000.
    LPCSTR szName;    // Pointer to name (in user addr space).
    DWORD dwThreadID; // Thread ID (-1=caller thread).
    DWORD dwFlags;    // Reserved for future use, must be zero.
  } THREADNAME_INFO;
#pragma pack (pop)
  THREADNAME_INFO info_s;
  info_s.dwType = 0x1000;
  info_s.szName = ACE_TEXT_ALWAYS_CHAR (name_in.c_str ());
  info_s.dwThreadID = (threadId_in ? threadId_in : -1);
  info_s.dwFlags = 0;
#pragma warning (push)
#pragma warning (disable: 6320 6322)
  __try {
    ::RaiseException (MS_VC_EXCEPTION,
                      0,
                      sizeof (struct tagTHREADNAME_INFO) / sizeof (ULONG_PTR),
                      (ULONG_PTR*)&info_s);
  } __except (EXCEPTION_EXECUTE_HANDLER) {}
#pragma warning (pop)
#endif // DEBUG_DEBUGGER
#endif // COMMON_OS_WIN32_TARGET_PLATFORM(0x0A00)
}
#endif // ACE_WIN32 || ACE_WIN64

bool
Common_Error_Tools::enableCoreDump (bool enable_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Error_Tools::enableCoreDump"));

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  LPTOP_LEVEL_EXCEPTION_FILTER previous_handler_p = NULL;
  if (!enable_in)
  {
    // Deregister Unhandled Exception-Filter
    previous_handler_p = SetUnhandledExceptionFilter (NULL);
    ACE_UNUSED_ARG (previous_handler_p);
    // Additional call "PreventSetUnhandledExceptionFilter"...
    // See also: "SetUnhandledExceptionFilter" and VC8 (and later)
    // http://blog.kalmbachnet.de/?postid=75
#if defined (_DEBUG)
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("SetUnhandledExceptionFilter(NULL)\n")));
#endif // _DEBUG

    if (Common_Error_Tools::debugHelpModule)
    {
      if (!FreeLibrary (Common_Error_Tools::debugHelpModule))
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to FreeLibrary(0x%@): \"%s\", continuing\n"),
                    Common_Error_Tools::debugHelpModule,
                    ACE_TEXT (Common_Error_Tools::errorToString (GetLastError ()).c_str ())));
      Common_Error_Tools::debugHelpModule = NULL;
    } // end IF

    goto continue_;
  } // end IF

  // sanity check(s)
  if (Common_Error_Tools::debugHelpModule)
    goto continue_;

  // *NOTE*: initialize the member so the dll is not loaded after the exception
  //         has occured which might be not possible anymore
  Common_Error_Tools::debugHelpModule = LoadLibrary (ACE_TEXT ("dbghelp.dll"));
  if (!Common_Error_Tools::debugHelpModule)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to LoadLibrary(\"%s\"): \"%s\", aborting\n"),
                ACE_TEXT ("dbghelp.dll"),
                ACE_TEXT (Common_Error_Tools::errorToString (GetLastError ()).c_str ())));
    return false;
  } // end IF
  Common_Error_Tools::miniDumpWriteDumpFunc =
    reinterpret_cast<MiniDumpWriteDumpFunc_t> (GetProcAddress (Common_Error_Tools::debugHelpModule,
                                                               ACE_TEXT_ALWAYS_CHAR ("MiniDumpWriteDump")));
  if (!Common_Error_Tools::miniDumpWriteDumpFunc)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to GetProcAddress(\"%s\":\"%s\"): \"%s\", aborting\n"),
                ACE_TEXT ("dbghelp.dll"), ACE_TEXT ("MiniDumpWriteDump"),
                ACE_TEXT (Common_Error_Tools::errorToString (GetLastError ()).c_str ())));
    return false;
  } // end IF

  // Register Unhandled Exception-Filter
  previous_handler_p =
    SetUnhandledExceptionFilter (common_error_win32_default_seh_handler);
  ACE_UNUSED_ARG (previous_handler_p);
  // Additional call "PreventSetUnhandledExceptionFilter"...
  // See also: "SetUnhandledExceptionFilter" and VC8 (and later)
  // http://blog.kalmbachnet.de/?postid=75
#if defined (_DEBUG)
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("SetUnhandledExceptionFilter(common_error_win32_default_seh_handler)\n")));
#endif // _DEBUG
#else
  if (unlikely (!Common_Tools::setResourceLimits (false,
                                                  enable_in,
                                                  false)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_Tools::setResourceLimits(), aborting\n")));
    return false;
  } // end IF

  int result =
      ::prctl (PR_SET_DUMPABLE,
               (enable_in ? 1 : 0), 0, 0, 0);
//               (enable_in ? SUID_DUMP_USER : SUID_DUMP_DISABLE), 0, 0, 0);
  if (unlikely (result == -1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ::prctl(PR_SET_DUMPABLE): \"%m\", aborting\n")));
    return false;
  } // end IF
#endif
continue_:
#if defined (_DEBUG)
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("%s core dump\n"),
              (enable_in ? ACE_TEXT ("enabled") : ACE_TEXT ("disabled"))));
#endif // _DEBUG

  return true;
}
#if defined (ACE_WIN32) || defined (ACE_WIN64)
bool
Common_Error_Tools::generateCoreDump (const std::string& programName_in,
                                      const struct Common_ApplicationVersion& programVersion_in,
                                      struct _EXCEPTION_POINTERS* exceptionInformation_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Error_Tools::generateCoreDump"));

  // sanity check(s)
  ACE_ASSERT (Common_Error_Tools::miniDumpWriteDumpFunc);

  bool result = false;
  DWORD result_2 = 0;
  HRESULT result_3 = E_FAIL;
  BOOL bMiniDumpSuccessful;
  ACE_TCHAR szPath[MAX_PATH];
  ACE_TCHAR szFileName[MAX_PATH];
  std::ostringstream converter (ACE_TEXT_ALWAYS_CHAR ("v"));
  converter << programVersion_in.majorVersion;
  converter << ACE_TEXT_ALWAYS_CHAR (".");
  converter << programVersion_in.minorVersion;
  converter << ACE_TEXT_ALWAYS_CHAR (".");
  converter << programVersion_in.microVersion;
  DWORD dwBufferSize = MAX_PATH;
  HANDLE hFile = ACE_INVALID_HANDLE;
  struct _SYSTEMTIME stLocalTime;
  enum _MINIDUMP_TYPE DumpType = MiniDumpWithFullMemory;
  struct _MINIDUMP_EXCEPTION_INFORMATION ExceptionParam;
  ExceptionParam.ThreadId = GetCurrentThreadId ();
  ExceptionParam.ExceptionPointers = exceptionInformation_in;
  ExceptionParam.ClientPointers = TRUE;
  //struct _MINIDUMP_USER_STREAM_INFORMATION UserStreamParam;
  //struct _MINIDUMP_CALLBACK_INFORMATION CallbackParam;

  GetLocalTime (&stLocalTime);
  result_2 = ACE_TEXT_GetTempPath (dwBufferSize, szPath);
  if (!result_2)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to GetTempPath(): \"%s\", aborting\n"),
                ACE_TEXT (Common_Error_Tools::errorToString (GetLastError ()))));
    return false;
  } // end IF
  result_3 = StringCchPrintf (szFileName,
                              MAX_PATH,
#if defined (_WIN32) && !defined (OLE2ANSI) // see <WTypes.h>
#if defined (UNICODE)
                              ACE_TEXT_ALWAYS_WCHAR ("%s%s"),
                              ACE_TEXT_ALWAYS_WCHAR (szPath),
#else
                              ACE_TEXT_ALWAYS_CHAR ("%s%s"),
                              ACE_TEXT_ALWAYS_CHAR (szPath),
#endif // UNICODE
#endif // _WIN32 && !OLE2ANSI
                              ACE_TEXT (programName_in.c_str ()));
  ACE_ASSERT (SUCCEEDED (result_3));
  if (!CreateDirectory (szFileName, NULL))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to CreateDirectory(\"%s\"): \"%s\", aborting\n"),
                ACE_TEXT (szFileName),
                ACE_TEXT (Common_Error_Tools::errorToString (GetLastError (), false))));
    return false;
  } // end IF
#if defined (_DEBUG)
  else
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("created directory \"%s\"\n"),
                ACE_TEXT (szFileName)));
#endif // _DEBUG

  result_3 =
    StringCchPrintf (szFileName,
                     MAX_PATH,
#if defined (UNICODE)
                     ACE_TEXT_ALWAYS_WCHAR ("%s%s\\%s-%04d%02d%02d-%02d%02d%02d-%ld-%ld.dmp"),
                     ACE_TEXT_ALWAYS_WCHAR (szPath),
                     ACE_TEXT_ALWAYS_WCHAR (programName_in.c_str ()),
                     ACE_TEXT_ALWAYS_WCHAR (converter.str ()),
#else
                     ACE_TEXT_ALWAYS_CHAR ("%s%s\\%s-%04d%02d%02d-%02d%02d%02d-%ld-%ld.dmp"),
                     ACE_TEXT_ALWAYS_CHAR (szPath),
                     ACE_TEXT_ALWAYS_CHAR (programName_in.c_str ()),
                     ACE_TEXT_ALWAYS_CHAR (converter.str ()),
#endif // UNICODE
                     stLocalTime.wYear, stLocalTime.wMonth, stLocalTime.wDay, 
                     stLocalTime.wHour, stLocalTime.wMinute, stLocalTime.wSecond,
                     GetCurrentProcessId (), GetCurrentThreadId ());
  ACE_ASSERT (SUCCEEDED (result_3));
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("creating dump file \"%s\"\n"),
              ACE_TEXT (szFileName)));
  hFile =
    ACE_TEXT_CreateFile (szFileName,
                         GENERIC_READ | GENERIC_WRITE, 
                         FILE_SHARE_WRITE | FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, 0);
  if (hFile == ACE_INVALID_HANDLE)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to CreateFile(\"%s\"): \"%s\", aborting\n"),
                ACE_TEXT (szFileName),
                ACE_TEXT (Common_Error_Tools::errorToString (GetLastError ()).c_str ())));
    return false;
  } // end IF

  bMiniDumpSuccessful =
    Common_Error_Tools::miniDumpWriteDumpFunc (GetCurrentProcess (),
                                               GetCurrentProcessId (), 
                                               hFile,
                                               DumpType,
                                               &ExceptionParam,
                                               NULL,            // &UserStreamParam,
                                               NULL);           // &CallbackParam
  if (!bMiniDumpSuccessful)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to MiniDumpWriteDump(): \"%s\", aborting\n"),
                ACE_TEXT (Common_Error_Tools::errorToString (GetLastError ()))));
    goto clean;
  } // end IF

  result = true;

clean:
  if (hFile != ACE_INVALID_HANDLE)
    if (!CloseHandle (hFile))
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to CloseHandle(0x%@): \"%s\", continuing\n"),
                  hFile,
                  ACE_TEXT (Common_Error_Tools::errorToString (GetLastError ()))));

  return result;
}
#endif // ACE_WIN32 || ACE_WIN64

#if defined (ACE_WIN32) || defined (ACE_WIN64)
std::string
Common_Error_Tools::errorToString (DWORD error_in,
                                   bool useAMGetErrorText_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Error_Tools::errorToString"));

  std::string result;

  DWORD result_2 = 0;
  if (useAMGetErrorText_in)
  {
    TCHAR buffer_a[MAX_ERROR_TEXT_LEN];
    ACE_OS::memset (buffer_a, 0, sizeof (TCHAR[MAX_ERROR_TEXT_LEN]));
    result_2 = AMGetErrorText (static_cast<HRESULT> (error_in),
                               buffer_a,
                               MAX_ERROR_TEXT_LEN);
    if (!result_2)
    {
      ACE_DEBUG ((LM_WARNING,
                  ACE_TEXT ("failed to AMGetErrorText(0x%x): \"%s\", falling back\n"),
                  error_in,
                  ACE_TEXT (Common_Error_Tools::errorToString (::GetLastError ()).c_str ())));
      goto fallback;
    } // end IF
    result = ACE_TEXT_ALWAYS_CHAR (buffer_a);
    goto strip_newline;
  } // end IF

  ACE_TCHAR buffer_a[BUFSIZ];
  ACE_OS::memset (buffer_a, 0, sizeof (ACE_TCHAR[BUFSIZ]));
  result_2 =
    ACE_TEXT_FormatMessage (FORMAT_MESSAGE_FROM_SYSTEM,                 // dwFlags
                            NULL,                                       // lpSource
                            error_in,                                   // dwMessageId
                            MAKELANGID (LANG_NEUTRAL, SUBLANG_DEFAULT), // dwLanguageId
                            buffer_a,                                   // lpBuffer
                            sizeof (ACE_TCHAR[BUFSIZ]),                 // nSize
                            NULL);                                      // Arguments
  if (!result_2)
  {
    DWORD error = ::GetLastError ();
    if (unlikely (error != ERROR_MR_MID_NOT_FOUND))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to FormatMessage(0x%x): \"%s\", continuing\n"),
                  error_in,
                  ACE_TEXT (Common_Error_Tools::errorToString (error).c_str ())));
      return result;
    } // end IF

fallback:
    // try DirectX error messages
    // *TODO*: implement ascii variants of DXGetErrorString
    //ACE_TCHAR* string_p = ACE_TEXT_WCHAR_TO_TCHAR (DXGetErrorString (error_in));
    //if (!string_p)
    //{
    //  ACE_DEBUG ((LM_ERROR,
    //              ACE_TEXT ("failed to DXGetErrorString(0x%x): \"%s\", aborting\n"),
    //              error_in,
    //              ACE_TEXT (Common_Error_Tools::errorToString (::GetLastError ()).c_str ())));
    //  return result;
    //} // end IF
    //result = ACE_TEXT_ALWAYS_CHAR (string_p);
    //result += ACE_TEXT_ALWAYS_CHAR (": ");

    WCHAR buffer_2[BUFSIZ];
    ACE_OS::memset (buffer_2, 0, sizeof (WCHAR[BUFSIZ]));
    DXGetErrorDescription (error_in, buffer_2, BUFSIZ);
    ACE_Wide_To_Ascii converter (buffer_2);
    if (unlikely (!ACE_OS::strcpy (buffer_a,
                                   ACE_TEXT_CHAR_TO_TCHAR (converter.char_rep ()))))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_OS::strcpy(): \"%m\", aborting\n")));
      return result;
    } // end IF
  } // end IF
  result = ACE_TEXT_ALWAYS_CHAR (buffer_a);

  // strip trailing newline ?
strip_newline:
  if (result[result.size () - 1] == '\n')
  {
    result.erase (--result.end ());
    result.erase (--result.end ());
  } // end IF

  return result;
}
#endif // ACE_WIN32 || ACE_WIN64

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#if defined (_DEBUG)
bool
Common_Error_Tools::initializeDebugHeap ()
{
  COMMON_TRACE (ACE_TEXT ("Common_Error_Tools::initializeDebugHeap"));

  int previous_heap_flags = -1;
  ACE_FILE_Addr file_address;
  ACE_FILE_Connector file_connector;
  ACE_FILE_IO file_IO;
  int result = -1;
  std::string package_name, file_name;
  ACE_HANDLE previous_file_handle = ACE_INVALID_HANDLE;

  // sanity check(s)
  ACE_ASSERT (Common_Error_Tools::debugHeapLogFileHandle == ACE_INVALID_HANDLE);

  //int current_debug_heap_flags = _CrtSetDbgFlag (_CRTDBG_REPORT_FLAG);
  //int debug_heap_flags = current_debug_heap_flags;
  int debug_heap_flags = (_CRTDBG_ALLOC_MEM_DF      |
                          _CRTDBG_CHECK_ALWAYS_DF   |
                          _CRTDBG_CHECK_CRT_DF      |
                          _CRTDBG_DELAY_FREE_MEM_DF |
                          _CRTDBG_LEAK_CHECK_DF);
  debug_heap_flags = (debug_heap_flags | _CRTDBG_CHECK_EVERY_16_DF);
  // Turn off CRT block checking bit
  //debug_heap_flags &= ~_CRTDBG_CHECK_CRT_DF;
  previous_heap_flags = _CrtSetDbgFlag (debug_heap_flags);
  // output to debug window
  file_name = Common_Log_Tools::getLogDirectory (package_name, 0);
  file_name += ACE_DIRECTORY_SEPARATOR_STR;
  file_name += ACE_TEXT_ALWAYS_CHAR (COMMON_DEBUG_DEBUGHEAP_LOG_FILE);
  result = file_address.set (ACE_TEXT (file_name.c_str ()));
  if (unlikely (result == -1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_FILE_Addr::set(\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (file_name.c_str ())));
    return false;
  } // end IF
  result =
    file_connector.connect (file_IO,                 // stream
                            file_address,            // filename
                            NULL,                    // timeout (block)
                            ACE_Addr::sap_any,       // (local) filename: N/A
                            0,                       // reuse_addr: N/A
                            (O_RDWR  |
                             O_TEXT  |
                             O_CREAT |
                             O_TRUNC),               // flags --> open
                            ACE_DEFAULT_FILE_PERMS); // permissions --> open
  if (unlikely (result == -1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_FILE_Connector::connect(\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (file_name.c_str ())));
    return false;
  } // end IF
  Common_Error_Tools::debugHeapLogFileHandle = file_IO.get_handle ();
  ACE_ASSERT (Common_Error_Tools::debugHeapLogFileHandle != ACE_INVALID_HANDLE);

  //result = _CrtSetReportHook2 (_CRT_RPTHOOK_INSTALL,
  //                             common_win32_debugheap_hook);
  //if (result == -1)
  //{
  //  ACE_DEBUG ((LM_ERROR,
  //              ACE_TEXT ("failed to _CrtSetReportHook2(): \"%m\", aborting\n")));
  //  return;
  //} // end IF
  //ACE_ASSERT (result == 1);
  _CrtSetReportMode (_CRT_ASSERT, _CRTDBG_MODE_FILE);
  _CrtSetReportMode (_CRT_ERROR,  _CRTDBG_MODE_FILE);
  _CrtSetReportMode (_CRT_WARN,   _CRTDBG_MODE_FILE);
  previous_file_handle =
    _CrtSetReportFile (_CRT_ASSERT,
                       Common_Error_Tools::debugHeapLogFileHandle);
  ACE_ASSERT (previous_file_handle != _CRTDBG_HFILE_ERROR);
  previous_file_handle =
    _CrtSetReportFile (_CRT_ERROR,
                       Common_Error_Tools::debugHeapLogFileHandle);
  ACE_ASSERT (previous_file_handle != _CRTDBG_HFILE_ERROR);
  previous_file_handle =
    _CrtSetReportFile (_CRT_WARN,
                       Common_Error_Tools::debugHeapLogFileHandle);
  ACE_ASSERT (previous_file_handle != _CRTDBG_HFILE_ERROR);

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("debug heap configuration:\n%s\t: %s\n%s\t: %s\n%s\t: %s\n%s\t: %s\n%s\t: %s\n%s\t: %s\n\n"),
              ACE_TEXT ("_CRTDBG_ALLOC_MEM_DF"),
              ((debug_heap_flags & _CRTDBG_ALLOC_MEM_DF) ? ACE_TEXT ("on")
                                                         : ACE_TEXT ("off")),
              ACE_TEXT ("_CRTDBG_DELAY_FREE_MEM_DF"),
              ((debug_heap_flags & _CRTDBG_DELAY_FREE_MEM_DF) ? ACE_TEXT ("on")
                                                              : ACE_TEXT ("off")),
              ACE_TEXT ("_CRTDBG_CHECK_ALWAYS_DF"),
              ((debug_heap_flags & _CRTDBG_CHECK_ALWAYS_DF) ? ACE_TEXT ("on")
                                                            : ACE_TEXT ("off")),
              ACE_TEXT ("_CRTDBG_RESERVED_DF"),
              ((debug_heap_flags & _CRTDBG_RESERVED_DF) ? ACE_TEXT ("on")
                                                        : ACE_TEXT ("off")),
              ACE_TEXT ("_CRTDBG_CHECK_CRT_DF"),
              ((debug_heap_flags & _CRTDBG_CHECK_CRT_DF) ? ACE_TEXT ("on")
                                                         : ACE_TEXT ("off")),
              ACE_TEXT ("_CRTDBG_LEAK_CHECK_DF"),
              ((debug_heap_flags & _CRTDBG_LEAK_CHECK_DF) ? ACE_TEXT ("on")
                                                          : ACE_TEXT ("off"))));
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("debug heap checking-frequency configuration:\n%s\t: %s\n%s\t: %s\n%s\t: %s\n%s\t: %s\n\n"),
              ACE_TEXT ("_CRTDBG_CHECK_EVERY_16_DF"),
              ((debug_heap_flags & _CRTDBG_CHECK_EVERY_16_DF) ? ACE_TEXT ("on")
                                                              : ACE_TEXT ("off")),
              ACE_TEXT ("_CRTDBG_CHECK_EVERY_128_DF"),
              ((debug_heap_flags & _CRTDBG_CHECK_EVERY_128_DF) ? ACE_TEXT ("on")
                                                               : ACE_TEXT ("off")),
              ACE_TEXT ("_CRTDBG_CHECK_EVERY_1024_DF"),
              ((debug_heap_flags & _CRTDBG_CHECK_EVERY_1024_DF) ? ACE_TEXT ("on")
                                                                : ACE_TEXT ("off")),
              ACE_TEXT ("_CRTDBG_CHECK_DEFAULT_DF"),
              (((debug_heap_flags & 0xFFFF0000) == _CRTDBG_CHECK_DEFAULT_DF) ? ACE_TEXT ("on")
                                                                             : ACE_TEXT ("off"))));
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("configured debug heap (%x, log file: \"%s\")\n"),
              debug_heap_flags,
              ACE_TEXT (file_name.c_str ())));

  return true;
}

void
Common_Error_Tools::finalizeDebugHeap ()
{
  COMMON_TRACE (ACE_TEXT ("Common_Error_Tools::finalizeDebugHeap"));

  int result = -1;
  if (Common_Error_Tools::debugHeapLogFileHandle)
  {
    ACE_HANDLE previous_file_handle =
      _CrtSetReportFile (_CRT_WARN, NULL);
    ACE_ASSERT (previous_file_handle != _CRTDBG_HFILE_ERROR);
    previous_file_handle =
      _CrtSetReportFile (_CRT_ERROR, NULL);
    ACE_ASSERT (previous_file_handle != _CRTDBG_HFILE_ERROR);
    previous_file_handle =
      _CrtSetReportFile (_CRT_ASSERT, NULL);
    ACE_ASSERT (previous_file_handle != _CRTDBG_HFILE_ERROR);

    result = ACE_OS::close (Common_Error_Tools::debugHeapLogFileHandle);
    if (result == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_OS::close(0x%@): \"%m\", continuing\n"),
                  Common_Error_Tools::debugHeapLogFileHandle));
    else
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("closed debug heap log file\n")));
    Common_Error_Tools::debugHeapLogFileHandle = ACE_INVALID_HANDLE;
  } // end IF
}
#endif /* _DEBUG */
#endif /* ACE_WIN32 || ACE_WIN64 */
