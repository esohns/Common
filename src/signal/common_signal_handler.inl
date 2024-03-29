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
#include "ace/Log_Msg.h"
#include "ace/Proactor.h"
#include "ace/Reactor.h"

#include "common_defines.h"
#include "common_macros.h"

#include "common_signal_tools.h"

template <typename ConfigurationType>
Common_SignalHandler_T<ConfigurationType>::Common_SignalHandler_T (Common_ISignal* callback_in)
 : inherited (ACE_Proactor::instance ())       // -->  default proactor
 , inherited2 (ACE_Reactor::instance (),       // -->  default reactor
               ACE_Event_Handler::LO_PRIORITY) // priority
 , configuration_ (NULL)
 , isInitialized_ (false)
 , lock_ ()
 , signals_ ()
 , callback_ (callback_in ? callback_in : this)
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

  // sanity check(s)
  ACE_ASSERT (configuration_);

  int result = 0;
  struct Common_Signal signal_s;
  signal_s.signal = signal_in;
  // *TODO*: WIN32 apparently does not support siginfo
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
  if (siginfo_in)
    signal_s.siginfo = *siginfo_in;
  if (ucontext_in)
    signal_s.ucontext = *ucontext_in;
#endif // ACE_WIN32 || ACE_WIN64

  // *IMPORTANT NOTE*: in signal context, many actions are forbidden (e.g.
  //                   tracing; see also: man 7 signal-safety)
  //                   --> Backup the context information and notify the
  //                   reactor / proactor for callback
  // *WARNING*:        grabbing a lock here is non-portable !!!
  switch (configuration_->mode)
  {
    case COMMON_SIGNAL_DISPATCH_PROACTOR:
    {
//      { ACE_GUARD_RETURN (ACE_SYNCH_RECURSIVE_MUTEX, aGuard, lock_, -1);
      signals_.push_back (signal_s);
//      } // end lock scope

      ACE_Proactor* proactor_p = ACE_Proactor::instance ();
      ACE_ASSERT (proactor_p);
      result =
          proactor_p->schedule_timer (*this,                 // event handler
                                      NULL,                  // act
                                      ACE_Time_Value::zero); // expire immediately
      if (unlikely (result == -1))
      {
        // *PORTABILITY*: tracing in a signal handler context is not portable
        //        ACE_DEBUG ((LM_ERROR,
        //                    ACE_TEXT ("failed to ACE_Proactor::schedule_timer(): \"%m\", aborting\n")));
        result = -1;
      } // end IF

      break;
    }
    case COMMON_SIGNAL_DISPATCH_REACTOR:
    {
//      { ACE_GUARD_RETURN (ACE_SYNCH_RECURSIVE_MUTEX, aGuard, *lock_, -1);
      signals_.push_back (signal_s);
//      } // end lock scope

      ACE_Reactor* reactor_p = inherited2::reactor ();
      ACE_ASSERT (reactor_p);
      result = reactor_p->notify (this,                           // event handler handle
                                  ACE_Event_Handler::EXCEPT_MASK, // mask
                                  NULL);                          // timeout
      if (unlikely (result == -1))
      {
        // *PORTABILITY*: tracing in a signal handler context is not portable
        //        ACE_DEBUG ((LM_ERROR,
        //                    ACE_TEXT ("failed to ACE_Reactor::notify(): \"%m\", aborting\n")));
        result = -1;
      } // end IF

      break;
    }
    case COMMON_SIGNAL_DISPATCH_SIGNAL:
    {
      // *PORTABILITY*: tracing in a signal handler context is not portable
//      ACE_DEBUG ((LM_DEBUG,
//                  ACE_TEXT ("%D: received [%u/\"%S\"]: %s\n"),
//                  signal_in, signal_in,
//                  ACE_TEXT (Common_Tools::signalToString (signal_s).c_str ())));

      ACE_ASSERT (callback_);
      try {
        callback_->handle (signal_s);
      } catch (...) {
        // *PORTABILITY*: tracing in a signal handler context is not portable
        //        ACE_DEBUG ((LM_ERROR,
        //                    ACE_TEXT ("caught exception in Common_ISignal::handle(), aborting\n")));
        result = -1;
      }

      break;
    }
    default:
    {
      // *PORTABILITY*: tracing in a signal handler context is not portable
      //      ACE_DEBUG ((LM_ERROR,
      //                  ACE_TEXT ("invalid/unknown signal dispatch mode (was: %d), aborting\n"),
      //                  dispatchMode_));
      result = -1;

      break;
    }
  } // end SWITCH

  // *IMPORTANT NOTE*: returning -1 will remove the signal handler !
  return result;
}

template <typename ConfigurationType>
bool
Common_SignalHandler_T<ConfigurationType>::initialize (const ConfigurationType& configuration_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_SignalHandler_T::initialize"));

  if (unlikely (isInitialized_))
  {
    { ACE_GUARD_RETURN (ACE_SYNCH_RECURSIVE_MUTEX, aGuard, lock_, false);
      signals_.clear ();
    } // end lock scope

    isInitialized_ = false;
  } // end IF

  configuration_ = &const_cast<ConfigurationType&> (configuration_in);
  isInitialized_ = true;

  return true;
}

template <typename ConfigurationType>
void
Common_SignalHandler_T<ConfigurationType>::handle_time_out (const ACE_Time_Value& time_in,
                                                            const void* act_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_SignalHandler_T::handle_time_out"));

  ACE_UNUSED_ARG (time_in);
  ACE_UNUSED_ARG (act_in);

  // sanity check(s)
  ACE_ASSERT (!act_in);

  int result = handle_exception (ACE_INVALID_HANDLE);
  if (unlikely (result == -1))
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_SignalHandler_T::handle_exception(): \"%m\", continuing\n")));
}

template <typename ConfigurationType>
int
Common_SignalHandler_T<ConfigurationType>::handle_exception (ACE_HANDLE handle_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_SignalHandler_T::handle_exception"));

  ACE_UNUSED_ARG (handle_in);

  Common_ISignal* callback_p = (callback_ ? callback_ : this);
  struct Common_Signal signal_s;

  { ACE_GUARD_RETURN (ACE_SYNCH_RECURSIVE_MUTEX, aGuard, lock_, -1);
    ACE_ASSERT (!signals_.empty ());
    signal_s = signals_.front ();
    signals_.erase (signals_.begin ());
  } // end lock scope
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("%D: received [%u/\"%S\"]: %s\n"),
              signal_s.signal, signal_s.signal,
              ACE_TEXT (Common_Signal_Tools::signalToString (signal_s).c_str ())));

  try {
    callback_p->handle (signal_s);
  } catch (...) {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("caught exception in Common_ISignal::handle(), continuing\n")));
  }

  return 0;
}
