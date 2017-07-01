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

#include "ace/Guard_T.h"
#include "ace/Log_Msg.h"

#include "common_defines.h"
#include "common_macros.h"
#include "common_tools.h"

template <ACE_SYNCH_DECL>
Common_Logger_T<ACE_SYNCH_USE>::Common_Logger_T (Common_MessageStack_t* stack_in,
                                                 ACE_SYNCH_MUTEX_T* lock_in)
                                                 //ACE_Lock* lock_in)
 : inherited ()
 //, buffer_ (NULL)
 , lock_ (lock_in)
 , messageStack_ (stack_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Logger_T::Common_Logger_T"));

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
Common_Logger_T<ACE_SYNCH_USE>::~Common_Logger_T ()
{
  COMMON_TRACE (ACE_TEXT ("Common_Logger_T::~Common_Logger_T"));

  //if (buffer_)
  //{
  //  int result = ACE_OS::fclose (buffer_);
  //  if (result == -1)
  //    ACE_DEBUG ((LM_ERROR,
  //                ACE_TEXT ("failed to ACE_OS::fclose(): \"%m\", continuing\n")));
  //} // end IF
}

template <ACE_SYNCH_DECL>
int
Common_Logger_T<ACE_SYNCH_USE>::open (const ACE_TCHAR* loggerKey_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Logger_T::start"));

  ACE_UNUSED_ARG (loggerKey_in);

  return 0;
}

template <ACE_SYNCH_DECL>
int
Common_Logger_T<ACE_SYNCH_USE>::reset ()
{
  COMMON_TRACE (ACE_TEXT ("Common_Logger_T::reset"));

  return 0;
}

template <ACE_SYNCH_DECL>
int
Common_Logger_T<ACE_SYNCH_USE>::close ()
{
  COMMON_TRACE (ACE_TEXT ("Common_Logger_T::close"));

  return 0;
}

template <ACE_SYNCH_DECL>
ssize_t
Common_Logger_T<ACE_SYNCH_USE>::log (ACE_Log_Record& record_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Logger_T::log"));

  int result = -1;

  // sanity check(s)
  //ACE_ASSERT (buffer_);
  ACE_ASSERT (lock_);
  ACE_ASSERT (messageStack_);

  std::ostringstream string_stream;
  result =
      record_in.print (ACE_TEXT (Common_Tools::getHostName ().c_str ()),
                       (COMMON_LOG_VERBOSE ? ACE_Log_Msg::VERBOSE
                                           : ACE_Log_Msg::VERBOSE_LITE),
                       string_stream);
  //result =
  //  record_in.print (ACE_TEXT (Common_Tools::getHostName ().c_str ()),
  //                   (COMMON_LOG_VERBOSE ? ACE_Log_Msg::VERBOSE
  //                                       : ACE_Log_Msg::VERBOSE_LITE),
  //                   buffer_);
  if (result == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Log_Record::print(): \"%m\", aborting\n")));
    return -1;
  } // end IF
  //result = ACE_OS::fseek (buffer_, 0, SEEK_SET);
  //if (result == -1)
  //{
  //  ACE_DEBUG ((LM_ERROR,
  //              ACE_TEXT ("failed to ACE_OS::fseek(0x%@): \"%m\", aborting\n"),
  //              buffer_));
  //  return -1;
  //} // end IF
  //string_stream << buffer_;

  // *IMPORTANT NOTE*: this is called by the ACE logger (ACE_Log_Msg::log()).
  //                   At this time, the calling thread is holding onto
  //                   ACE_Log_Msg_Manager::lock_ (see: Log_Msg.cpp:2893). Note
  //                   how this could deadlock if a different thread has lock_
  //                   and tries to log something
  //                   --> temporarily release ACE_Log_Msg_Manager::lock_
  ACE_Log_Msg* log_msg_p = ACE_LOG_MSG;
  ACE_ASSERT (log_msg_p);
  bool acquire_lock = false;
  result = log_msg_p->release ();
  if (result == 0)
    acquire_lock = true;
  else
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Log_Msg::release(): \"%m\", continuing\n")));

  { ACE_GUARD_RETURN (ACE_SYNCH_MUTEX_T, aGuard, *lock_, -1);

    messageStack_->push_back (string_stream.str ());
  } // end lock scope

  if (acquire_lock)
  {
    result = log_msg_p->acquire ();
    if (result == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_Log_Msg::acquire(): \"%m\", continuing\n")));
  } // end IF

  return 0;
}
