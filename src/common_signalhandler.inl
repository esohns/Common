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
#include <string>

#include "ace/Log_Msg.h"
#include "ace/OS.h"

#include "common_isignal.h"
#include "common_macros.h"
#include "common_tools.h"

template <typename ConfigurationType>
Common_SignalHandler_T<ConfigurationType>::Common_SignalHandler_T (Common_ISignal* callback_in)
 : inherited ()
 , inherited2 (NULL,                           // default reactor
               ACE_Event_Handler::LO_PRIORITY) // priority
 , configuration_ (NULL)
 , isInitialized_ (false)
 , callback_ (callback_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_SignalHandler_T::Common_SignalHandler_T"));
    
}

template <typename ConfigurationType>
int
Common_SignalHandler_T<ConfigurationType>::handle_signal (int signal_in,
                                                          siginfo_t* siginfo_in,
                                                          ucontext_t* ucontext_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_SignalHandler_T::handle_signal"));

//  // initialize return value
//  int result = -1;

  // *NOTE*: on Win32, the second/third arguments seem to be void
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  ACE_UNUSED_ARG (siginfo_in);
  ACE_UNUSED_ARG (ucontext_in);
#endif

  std::string information_string;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  struct siginfo_t signal_information (ACE_INVALID_HANDLE);
//  signal_information.si_handle_ = ACE_INVALID_HANDLE;
#else
  siginfo_t signal_information;
//  ACE_OS::memset (&signal_information, 0, sizeof (siginfo_t));
  if (siginfo_in)
    signal_information = *siginfo_in;
#endif
  Common_Tools::retrieveSignalInfo (signal_in,
                                    signal_information,
#if defined (ACE_WIN32) || defined (ACE_WIN64)
                                    NULL,
#else
                                    ucontext_in,
#endif
                                    information_string);
  // *PORTABILITY*: tracing in a signal handler context is not portable
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("%D: received [%u/\"%S\"]: %s\n"),
              signal_in, signal_in,
              ACE_TEXT (information_string.c_str ())));

  // *IMPORTANT NOTE*: in signal context, many actions are forbidden (e.g.
  //                   tracing), so backup the context information and notify
  //                   the reactor / proactor for callback (see below)

//  // backup state
//  struct Common_SignalInformation* act_p = NULL;
//  ACE_NEW_NORETURN (act_p,
//                    Common_SignalInformation ());
//  if (!act_p)
//  {
//    // *PORTABILITY*: tracing in a signal handler context is not portable
////    ACE_DEBUG ((LM_CRITICAL,
////                ACE_TEXT ("failed to allocate memory: \"%m\", aborting\n")));
//    return -1;
//  } // end IF
//  act_p->signal = signal_in;
//  if (info_in)
//    act_p->sigInfo = *info_in;
//  if (context_in)
//    act_p->uContext = *context_in;

//#if defined (ACE_WIN32) || defined (ACE_WIN64)
//  act_p->sigInfo.si_handle_ = static_cast<ACE_HANDLE> (info_in);
//#endif

//  // schedule an event
//  // *NOTE*: fire-and-forget act_p here
//  if (useReactor_)
//  {
//    ACE_Reactor* reactor_p = inherited2::reactor ();
//    ACE_ASSERT (reactor_p);
//    result = reactor_p->notify (this,                           // event handler handle
//                                ACE_Event_Handler::EXCEPT_MASK, // mask
//                                NULL);                          // timeout
//    if (result == -1)
//    {
//      // *PORTABILITY*: tracing in a signal handler context is not portable
////      ACE_DEBUG ((LM_ERROR,
////                  ACE_TEXT ("failed to ACE_Reactor::notify: \"%m\", aborting\n")));

////      // clean up
////      delete act_p;
//    } // end IF
//  } // end IF
//  else
//  {
//    ACE_Proactor* proactor_p = ACE_Proactor::instance ();
//    ACE_ASSERT (proactor_p);
//    result =
//        proactor_p->schedule_timer (*this,                 // event handler
////                                    act_p,                 // act
//                                    NULL,                  // act
//                                    ACE_Time_Value::zero); // expire immediately
//    if (result == -1)
//    {
//      // *PORTABILITY*: tracing in a signal handler context is not portable
////      ACE_DEBUG ((LM_ERROR,
////                  ACE_TEXT ("failed to ACE_Proactor::schedule_timer: \"%m\", aborting\n")));

////      // clean up
////      delete act_p;
//    } // end IF
//  } // end ELSE

//  return result;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  return handle_exception (reinterpret_cast<ACE_HANDLE> (signal_in));
#else
  return handle_exception (static_cast<ACE_HANDLE> (signal_in));
#endif
}

template <typename ConfigurationType>
bool
Common_SignalHandler_T<ConfigurationType>::initialize (const ConfigurationType& configuration_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_SignalHandler_T::initialize"));

  configuration_ = &const_cast<ConfigurationType&> (configuration_in);
  isInitialized_ = true;

  return true;
}

//template <typename ConfigurationType>
//void
//Common_SignalHandler_T<ConfigurationType>::handle_time_out (const ACE_Time_Value& time_in,
//                                                            const void* act_in)
//{
//  COMMON_TRACE (ACE_TEXT ("Common_SignalHandler_T::handle_time_out"));
//
//  ACE_UNUSED_ARG (time_in);
////  ACE_UNUSED_ARG (act_in);
//
//  const struct Common_SignalInformation* information_p =
//    static_cast<const Common_SignalInformation*> (act_in);
//
//  int result = handle_exception (ACE_INVALID_HANDLE);
//  if (result == -1)
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("failed to Common_SignalHandler_T::handle_exception(): \"%m\", continuing\n")));
//
//  // clean up
//  if (information_p)
//    delete information_p;
//}

template <typename ConfigurationType>
int
Common_SignalHandler_T<ConfigurationType>::handle_exception (ACE_HANDLE handle_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_SignalHandler_T::handle_exception"));

  Common_ISignal* callback_p = (callback_ ? callback_ : this);
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  int signal = reinterpret_cast<int> (handle_in);
#else
  int signal = handle_in;
#endif

//  std::string information_string;
//  Common_Tools::retrieveSignalInfo (signal_,
//                                    siginfo_,
//                                    &ucontext_,
//                                    information);
//  ACE_DEBUG ((LM_DEBUG,
//              ACE_TEXT ("%D: received [%S]: %s\n"),
//              signal_,
//              ACE_TEXT (information_string.c_str ())));

  try {
    callback_p->handle (signal);
  } catch (...) {
    // *PORTABILITY*: tracing in a signal handler context is not portable
    // *TODO*
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("caught exception in Common_ISignal::handle(), continuing\n")));
  }

//  signal_ = -1;
//#if defined (ACE_WIN32) || defined (ACE_WIN64)
//  siginfo_ = ACE_INVALID_HANDLE;
//  ucontext_ = -1;
//#else
//  ACE_OS::memset (&siginfo_, 0, sizeof (siginfo_));
//  ACE_OS::memset (&ucontext_, 0, sizeof (ucontext_));
//#endif

  return 0;
}
