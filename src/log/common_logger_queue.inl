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
Common_Logger_Queue_T<ACE_SYNCH_USE>::Common_Logger_Queue_T ()
 : inherited ()
 , isInitialized_ (false)
 , lock_ (NULL)
 , queue_ (NULL)
{
  COMMON_TRACE (ACE_TEXT ("Common_Logger_Queue_T::Common_Logger_Queue_T"));

}

template <ACE_SYNCH_DECL>
bool
Common_Logger_Queue_T<ACE_SYNCH_USE>::initialize (Common_Log_MessageQueue_t* queue_in,
                                                  ACE_SYNCH_MUTEX_T* lock_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Logger_Queue_T::initialize"));

  lock_ = lock_in;
  queue_ = queue_in;

  isInitialized_ = true;

  return true;
}

template <ACE_SYNCH_DECL>
int
Common_Logger_Queue_T<ACE_SYNCH_USE>::open (const ACE_TCHAR* loggerKey_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Logger_Queue_T::open"));

  ACE_UNUSED_ARG (loggerKey_in);

  return 0;
}

template <ACE_SYNCH_DECL>
int
Common_Logger_Queue_T<ACE_SYNCH_USE>::reset ()
{
  COMMON_TRACE (ACE_TEXT ("Common_Logger_Queue_T::reset"));

  return 0;
}

template <ACE_SYNCH_DECL>
int
Common_Logger_Queue_T<ACE_SYNCH_USE>::close ()
{
  COMMON_TRACE (ACE_TEXT ("Common_Logger_Queue_T::close"));

  return 0;
}

template <ACE_SYNCH_DECL>
ssize_t
Common_Logger_Queue_T<ACE_SYNCH_USE>::log (ACE_Log_Record& record_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Logger_Queue_T::log"));

  int result = -1;
  static std::string hostname_string = Common_Tools::getHostName ();
  std::ostringstream string_stream;
  result =
      record_in.print (ACE_TEXT (hostname_string.c_str ()),
                       (COMMON_LOG_VERBOSE ? ACE_Log_Msg::VERBOSE
                                           : ACE_Log_Msg::VERBOSE_LITE),
                       string_stream);
  if (unlikely (result == -1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Log_Record::print(): \"%m\", aborting\n")));
    return -1;
  } // end IF

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
  if (likely (result == 0))
    acquire_lock = true;
  else
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Log_Msg::release(): \"%m\", continuing\n")));

  if (likely (queue_))
  {
    if (likely (lock_))
    {
      result = lock_->acquire ();
      if (unlikely (result == -1))
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE_SYNCH_MUTEX_T::acquire(): \"%m\", continuing\n")));
    } // end IF
    queue_->push_back (string_stream.str ());
    if (likely (lock_))
    {
      result = lock_->release ();
      if (unlikely (result == -1))
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE_SYNCH_MUTEX_T::release(): \"%m\", continuing\n")));
    } // end IF
  } // end IF

  if (likely (acquire_lock))
  {
    result = log_msg_p->acquire ();
    if (unlikely (result == -1))
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_Log_Msg::acquire(): \"%m\", continuing\n")));
  } // end IF

  return 0;
}
