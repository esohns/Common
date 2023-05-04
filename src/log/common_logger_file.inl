/***************************************************************************
 *   Copyright (C) 2010 by Erik Sohns   *
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
#include <sstream>
#include <string>

#include "ace/Guard_T.h"
#include "ace/Log_Msg.h"

#include "common_defines.h"
#include "common_macros.h"
#include "common_tools.h"

template <ACE_SYNCH_DECL>
Common_Logger_File_T<ACE_SYNCH_USE>::Common_Logger_File_T ()
 : inherited ()
 , buffer_ (NULL)
 , isInitialized_ (false)
 , lock_ (NULL)
{
  COMMON_TRACE (ACE_TEXT ("Common_Logger_File_T::Common_Logger_File_T"));

  //// *NOTE*: see also: man 3 mkstemp
  //ACE_TCHAR buffer[6 + 1];
  //ACE_OS::strcpy (buffer, ACE_TEXT ("XXXXXX"));
  //ACE_HANDLE file_handle = ACE_OS::mkstemp (buffer);
  //if (file_handle == ACE_INVALID_HANDLE)
  //  ACE_DEBUG ((LM_ERROR,
  //              ACE_TEXT ("failed to ACE_OS::mkstemp(): \"%m\", continuing\n")));
  //buffer_ = ACE_OS::fdopen (file_handle, ACE_TEXT ("w"));
  //if (!buffer_)
  //{
  //  ACE_DEBUG ((LM_ERROR,
  //              ACE_TEXT ("failed to ACE_OS::fdopen(): \"%m\", continuing\n")));

  //  // clean up
  //  int result = ACE_OS::close (file_handle);
  //  if (result == -1)
  //    ACE_DEBUG ((LM_ERROR,
  //                ACE_TEXT ("failed to ACE_OS::close(): \"%m\", continuing\n")));
  //} // end IF
}

template <ACE_SYNCH_DECL>
Common_Logger_File_T<ACE_SYNCH_USE>::~Common_Logger_File_T ()
{
  COMMON_TRACE (ACE_TEXT ("Common_Logger_File_T::~Common_Logger_File_T"));

  if (buffer_)
  {
    int result = ACE_OS::fclose (buffer_);
    if (result == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_OS::fclose(): \"%m\", continuing\n")));
  } // end IF
}

template <ACE_SYNCH_DECL>
bool
Common_Logger_File_T<ACE_SYNCH_USE>::initialize (const std::string& fileName_in,
                                                 ACE_SYNCH_MUTEX_T* lock_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Logger_File_T::initialize"));

  ACE_ASSERT (false); // *TODO*
  lock_ = lock_in;

  isInitialized_ = true;

  return true;
}

template <ACE_SYNCH_DECL>
int
Common_Logger_File_T<ACE_SYNCH_USE>::open (const ACE_TCHAR* loggerKey_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Logger_File_T::open"));

  ACE_UNUSED_ARG (loggerKey_in);

  return 0;
}

template <ACE_SYNCH_DECL>
int
Common_Logger_File_T<ACE_SYNCH_USE>::reset ()
{
  COMMON_TRACE (ACE_TEXT ("Common_Logger_File_T::reset"));

  return 0;
}

template <ACE_SYNCH_DECL>
int
Common_Logger_File_T<ACE_SYNCH_USE>::close ()
{
  COMMON_TRACE (ACE_TEXT ("Common_Logger_File_T::close"));

  return 0;
}

template <ACE_SYNCH_DECL>
ssize_t
Common_Logger_File_T<ACE_SYNCH_USE>::log (ACE_Log_Record& record_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Logger_File_T::log"));

  int result = -1;

  // sanity check(s)
  ACE_ASSERT (buffer_);

  static std::string hostname_string = Common_Tools::getHostName ();
  result =
    record_in.print (ACE_TEXT (hostname_string.c_str ()),
                     (COMMON_LOG_VERBOSE ? ACE_Log_Msg::VERBOSE
                                         : ACE_Log_Msg::VERBOSE_LITE),
                     buffer_);
  if (unlikely (result == -1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Log_Record::print(): \"%m\", aborting\n")));
    return -1;
  } // end IF

  return 0;
}
