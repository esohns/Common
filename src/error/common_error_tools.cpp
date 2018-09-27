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

#if defined (ACE_WIN32) || defined (ACE_WIN64)
//#include <DxErr.h>
#include <Errors.h>
#endif // ACE_WIN32 || ACE_WIN64

#include "ace/Log_Msg.h"
#include "ace/OS.h"

#include "common_macros.h"

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include "common_directx_error.h"
#endif // ACE_WIN32 || ACE_WIN64

void
common_error_terminate_function ()
{
  //COMMON_TRACE (ACE_TEXT ("common_error_terminate_function"));

  ACE_OS::abort ();
}

//////////////////////////////////////////

void
Common_Error_Tools::initialize ()
{
  COMMON_TRACE (ACE_TEXT ("Common_Error_Tools::initialize"));

  std::set_terminate (common_error_terminate_function);
}

void
Common_Error_Tools::finalize ()
{
  COMMON_TRACE (ACE_TEXT ("Common_Error_Tools::finalize"));

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
