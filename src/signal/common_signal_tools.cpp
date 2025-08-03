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

#include "common_signal_tools.h"

#include <sstream>

#include "ace/config-macros.h"
#include "ace/Log_Msg.h"
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
#include "ace/POSIX_Proactor.h"
#endif // ACE_WIN32 || ACE_WIN64
#include "ace/Proactor.h"
#include "ace/Reactor.h"

#include "common_macros.h"
#include "common_os_tools.h"

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
#include "common_error_tools.h"
#endif // ACE_WIN32 || ACE_WIN64

// initialize statics
ACE_Sig_Handler Common_Signal_Tools::signalHandler_;

extern "C"
void
common_default_rt_sig_handler_cb (int signum_in)
//                                  siginfo_t* info_in,
//                                  ucontext_t* context_in)
{
//  COMMON_TRACE (ACE_TEXT ("common_rt_sig_handler_cb"));

  ACE_UNUSED_ARG (signum_in);
//  ACE_UNUSED_ARG (info_in);
//  ACE_UNUSED_ARG (context_in);

  // *WARNING*: should never actually get here, because RT signals ought to be
  //            blocked early on in the main thread, BEFORE other threads have
  //            started. The Sig-Proactor can then unblock the signal in its
  //            individual worker thread(s) where they ought to be dispatched
  ACE_NOTREACHED (return;)
}

//////////////////////////////////////////

bool
Common_Signal_Tools::preInitialize (ACE_Sig_Set& signals_inout,
                                    enum Common_SignalDispatchType dispatchType_in,
                                    bool useNetworking_in,
                                    bool useAsynchTimers_in,
                                    Common_SignalActions_t& previousActions_out,
                                    ACE_Sig_Set& previousMask_out)
{
  COMMON_TRACE (ACE_TEXT ("Common_Signal_Tools::preInitialize"));

  int result = -1;

  // initialize return value(s)
  previousActions_out.clear ();
  result = previousMask_out.empty_set ();
  if (unlikely (result == - 1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Sig_Set::empty_set(): \"%m\", aborting\n")));
    return false;
  } // end IF

  // *IMPORTANT NOTE*: "The signal disposition is a per-process attribute: in a
  // multithreaded application, the disposition of a particular signal is the
  // same for all threads." (see man(7) signal)

  ACE_Sig_Action ignore_action (static_cast<ACE_SignalHandler> (SIG_IGN), // ignore action
                                ACE_Sig_Set (false),                      // mask of signals to be blocked when servicing --> none
                                0);                                       // flags
  ACE_Sig_Action previous_action;

  // step1: ignore certain signals
  if (!useNetworking_in)
    goto continue_;

  // step1a: ignore SIGPIPE: continue gracefully after a client suddenly
  // disconnects (i.e. application/system crash, etc...)
  // --> specify ignore action
  // *IMPORTANT NOTE*: specifically, do NOT restart system calls in this case (see manual)
  // *NOTE*: there is no need to keep this around after registration
  result = ignore_action.register_action (SIGPIPE,
                                          &previous_action);
  if (unlikely (result == -1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Sig_Action::register_action(%d: %S): \"%m\", aborting\n"),
                SIGPIPE, SIGPIPE));
    return false;
  } // end IF
  previousActions_out[SIGPIPE] = previous_action;
  ACE_DEBUG ((LM_WARNING,
              ACE_TEXT ("%t: ignoring signal %d: \"%S\"\n"),
              SIGPIPE, SIGPIPE));
  if (signals_inout.is_member (SIGPIPE) > 0)
  {
    result = signals_inout.sig_del (SIGPIPE);
    if (unlikely (result == -1))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_Sig_Set::sig_del(%d: \"%S\"): \"%m\", aborting\n"),
                  SIGPIPE, SIGPIPE));
      return false;
    } // end IF
    ACE_DEBUG ((LM_WARNING,
                ACE_TEXT ("%t: removed %d: \"%S\" from handled signals\n"),
                SIGPIPE, SIGPIPE));
  } // end IF

continue_:
  // step2: block certain signals
  ACE_Sig_Set blocked_signal_set;
  result = blocked_signal_set.empty_set ();
  if (unlikely (result == - 1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Sig_Set::empty_set(): \"%m\", aborting\n")));
    return false;
  } // end IF

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
  // *IMPORTANT NOTE*: child threads inherit the signal mask of their parent
  //                   --> make sure this is the main thread context

  // *IMPORTANT NOTE*: "...The default action for an unhandled real-time signal
  //                   is to terminate the receiving process. ..."
  //                   --> establish a default handler ? block them (see below)
  handleRealtimeSignals ();

  // step2a: block [SIGRTMIN,SIGRTMAX] iff the default[/current (!)] proactor
  // implementation happens to be ACE_POSIX_Proactor::PROACTOR_SIG (i.e. Linux,
  // ...)

  // *NOTE*: the ACE_POSIX_Proactor::PROACTOR_SIG implementation also blocks the
  //         RT signal(s) in its ctor
  // --> see also: POSIX_Proactor.cpp:1604,1653
  //         ...and later dispatches the signals in worker thread(s) running the
  //         event loop
  // --> see also: POSIX_Proactor.cpp:1864
  // --> see also: man sigwaitinfo, man 7 signal
  // --> when using the proactor, block the signals in the main thread
  if (dispatchType_in == COMMON_SIGNAL_DISPATCH_PROACTOR)
  {
    // *NOTE*: calling the instance() method starts several threads, which may
    //         receive RT signals and crash the whole process...
    // ACE_Proactor* proactor_p = ACE_Proactor::instance ();
    // ACE_ASSERT (proactor_p);
    // ACE_POSIX_Proactor* proactor_impl_p =
    //     static_cast<ACE_POSIX_Proactor*> (proactor_p->implementation ());
    // ACE_ASSERT (proactor_impl_p);
    // if (proactor_impl_p->get_impl_type () != ACE_POSIX_Proactor::PROACTOR_SIG)
    //   goto continue_2;

    unsigned int number = 0;
    for (int i = ACE_SIGRTMIN;
         i <= ACE_SIGRTMAX;
         ++i, ++number)
    {
      result = blocked_signal_set.sig_add (i);
      if (unlikely (result == -1))
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE_Sig_Set::sig_add(%d: \"%S\"): \"%m\", aborting\n"),
                    i, i));
        return false;
      } // end IF

      // remove any RT signals from the signal set handled by the application
      if (signals_inout.is_member (i) > 0)
      {
        result = signals_inout.sig_del (i); // <-- let the event dispatch handle
                                            //     all RT signals
        if (unlikely (result == -1))
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to ACE_Sig_Set::sig_del(%d: \"%S\"): \"%m\", aborting\n"),
                      i, i));
          return false;
        } // end IF
        ACE_DEBUG ((LM_WARNING,
                    ACE_TEXT ("%t: removed %d: \"%S\" from handled signals\n"),
                    i, i));
      } // end IF
    } // end IF
    ACE_DEBUG ((LM_WARNING,
                ACE_TEXT ("%t: blocking %u real-time signal(s) [%d: \"%S\" - %d: \"%S\"]\n"),
                number,
                ACE_SIGRTMIN, ACE_SIGRTMIN, ACE_SIGRTMAX, ACE_SIGRTMAX));
  } // end IF
// continue_2:
#if defined (ACE_LINUX)
  // *IMPORTANT NOTE*: "...NPTL makes internal use of the first two real-time
  //                   signals (see also signal(7)); these signals cannot be
  //                   used in applications. ..." (see 'man 7 pthreads')
  //                   --> on POSIX platforms, ensure that ACE_SIGRTMIN == 34
  if (signals_inout.is_member (32) > 0)
  {
    result = signals_inout.sig_del (32);
    if (unlikely (result == -1))
    {
      ACE_DEBUG ((LM_ERROR,
                 ACE_TEXT ("failed to ACE_Sig_Set::sig_del(%d: \"%S\"): \"%m\", aborting\n"),
                 32, 32));
      return false;
    } // end IF
    ACE_DEBUG ((LM_WARNING,
                ACE_TEXT ("%t: removed %d: \"%S\" from handled signals\n"),
                32, 32));
  } // end IF
  if (signals_inout.is_member (33) > 0)
  {
    result = signals_inout.sig_del (33);
    if (unlikely (result == -1))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_Sig_Set::sig_del(%d: \"%S\"): \"%m\", aborting\n"),
                  33, 33));
      return false;
    } // end IF
    ACE_DEBUG ((LM_WARNING,
                ACE_TEXT ("%t: removed %d: \"%S\" from handled signals\n"),
                33, 33));
  } // end IF
#endif // ACE_LINUX
#endif // ACE_WIN32 || ACE_WIN64

  // *NOTE*: on UNIX remove SIGSTOP/SIGKILL; these cannot be handled
  // *NOTE*: "...The SIGSTOP signal stops the process. It cannot be handled,
  //         ignored, or blocked. ..."
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
  if (signals_inout.is_member (SIGSTOP) > 0)
  {
    result = signals_inout.sig_del (SIGSTOP);
    if (unlikely (result == -1))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_Sig_Set::sig_del(%d: \"%S\"): \"%m\", aborting\n"),
                  SIGSTOP, SIGSTOP));
      return false;
    } // end IF
    ACE_DEBUG ((LM_WARNING,
                ACE_TEXT ("%t: removed %d: \"%S\" from handled signals\n"),
                SIGSTOP, SIGSTOP));
  } // end IF
  if (signals_inout.is_member (SIGKILL) > 0)
  {
    result = signals_inout.sig_del (SIGKILL);
    if (unlikely (result == -1))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_Sig_Set::sig_del(%d: \"%S\"): \"%m\", aborting\n"),
                  SIGKILL, SIGKILL));
      return false;
    } // end IF
    ACE_DEBUG ((LM_WARNING,
                ACE_TEXT ("%t: removed %d: \"%S\" from handled signals\n"),
                SIGKILL, SIGKILL));
  } // end IF
#endif // ACE_WIN32 || ACE_WIN64

  // *NOTE*: on UNIX remove SIGINT/SIGTRAP/[SIGSTOP]/SIGCONT when running in a debugger
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
  if (!Common_Error_Tools::inDebugSession ())
    goto continue_3;
  if (signals_inout.is_member (SIGINT) > 0)
  {
    result = signals_inout.sig_del (SIGINT);
    if (unlikely (result == -1))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_Sig_Set::sig_del(%d: \"%S\"): \"%m\", aborting\n"),
                  SIGINT, SIGINT));
      return false;
    } // end IF
    ACE_DEBUG ((LM_WARNING,
                ACE_TEXT ("%t: removed %d: \"%S\" from handled signals\n"),
                SIGINT, SIGINT));
  } // end IF
  if (signals_inout.is_member (SIGSTOP) > 0)
  {
    result = signals_inout.sig_del (SIGSTOP);
    if (unlikely (result == -1))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_Sig_Set::sig_del(%d: \"%S\"): \"%m\", aborting\n"),
                  SIGSTOP, SIGSTOP));
      return false;
    } // end IF
    ACE_DEBUG ((LM_WARNING,
                ACE_TEXT ("%t: removed %d: \"%S\" from handled signals\n"),
                SIGSTOP, SIGSTOP));
  } // end IF
  if (signals_inout.is_member (SIGTRAP) > 0)
  {
    result = signals_inout.sig_del (SIGTRAP);
    if (unlikely (result == -1))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_Sig_Set::sig_del(%d: \"%S\"): \"%m\", aborting\n"),
                  SIGTRAP, SIGTRAP));
      return false;
    } // end IF
    ACE_DEBUG ((LM_WARNING,
                ACE_TEXT ("%t: removed %d: \"%S\" from handled signals\n"),
                SIGTRAP, SIGTRAP));
  } // end IF
  if (signals_inout.is_member (SIGCONT) > 0)
  {
    result = signals_inout.sig_del (SIGCONT);
    if (unlikely (result == -1))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_Sig_Set::sig_del(%d: \"%S\"): \"%m\", aborting\n"),
                  SIGCONT, SIGCONT));
      return false;
    } // end IF
    ACE_DEBUG ((LM_WARNING,
                ACE_TEXT ("%t: removed %d: \"%S\" from handled signals\n"),
                SIGCONT, SIGCONT));
  } // end IF
continue_3:
#endif // ACE_WIN32 || ACE_WIN64

  // *NOTE*: remove SIGSEGV to enable core dumps on non-Win32 systems
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
  if (signals_inout.is_member (SIGSEGV) > 0)
  {
    result = signals_inout.sig_del (SIGSEGV);
    if (unlikely (result == -1))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_Sig_Set::sig_del(%d: \"%S\"): \"%m\", aborting\n"),
                  SIGSEGV, SIGSEGV));
      return false;
    } // end iF
    ACE_DEBUG ((LM_WARNING,
                ACE_TEXT ("%t: removed %d: \"%S\" from handled signals\n"),
                SIGSEGV, SIGSEGV));
  } // end IF

#if defined (VALGRIND_USE)
  // *NOTE*: valgrind uses SIGRT32 (--> SIGRTMAX ?) and apparently will not work
  // if the application installs its own handler (see documentation)
  if (unlikely (RUNNING_ON_VALGRIND))
  {
    if (signals_inout.is_member (SIGRTMAX) > 0)
    {
      result = signals_inout.sig_del (SIGRTMAX);
      if (unlikely (result == -1))
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE_Sig_Set::sig_del(%d: \"%S\"): \"%m\", aborting\n"),
                    SIGRTMAX, SIGRTMAX));
        return false;
      } // end iF
      ACE_DEBUG ((LM_WARNING,
                 ACE_TEXT ("%t: removed %d: \"%S\" from handled signals\n"),
                 SIGRTMAX, SIGRTMAX));
    } // end IF
  } // end IF
#endif // VALGRIND_USE

  // *NOTE*: remove SIGALRM to enable (asynch) timers on non-Win32 systems
  if (useAsynchTimers_in &&
      signals_inout.is_member (SIGALRM) > 0)
  {
    result = signals_inout.sig_del (SIGALRM);
    if (unlikely (result == -1))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_Sig_Set::sig_del(%d: \"%S\"): \"%m\", aborting\n"),
                  SIGALRM, SIGALRM));
      return false;
    } // end iF
    ACE_DEBUG ((LM_WARNING,
                ACE_TEXT ("%t: removed %d: \"%S\" from handled signals\n"),
                SIGALRM, SIGALRM));
  } // end IF
#endif // ACE_WIN32 || ACE_WIN64

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
  result = ACE_OS::thr_sigsetmask (SIG_BLOCK,
                                   blocked_signal_set,
                                   previousMask_out);
  if (unlikely (result == -1))
  {
    ACE_DEBUG ((LM_ERROR,
               ACE_TEXT ("failed to ACE_OS::thr_sigsetmask(): \"%m\", aborting\n")));
    return false;
  } // end IF
#endif // ACE_WIN32 || ACE_WIN64

  return true;
}

bool
Common_Signal_Tools::initialize (enum Common_SignalDispatchType dispatch_in,
                                 const ACE_Sig_Set& signals_in,
                                 const ACE_Sig_Set& ignoreSignals_in,
                                 ACE_Event_Handler* eventHandler_in,
                                 Common_SignalActions_t& previousActions_out)
{
  COMMON_TRACE (ACE_TEXT ("Common_Signal_Tools::initialize"));

  // initialize return value(s)
  previousActions_out.clear ();

  // *NOTE*: "The signal disposition is a per-process attribute: in a
  //         multithreaded application, the disposition of a particular signal
  //         is the same for all threads." (see man(7) signal)

  // step1: backup previous actions
  ACE_Sig_Action previous_action;
  for (int i = 1;
       i < ACE_NSIG;
       ++i)
    if (signals_in.is_member (i) > 0)
    {
      previous_action.retrieve_action (i);
      previousActions_out[i] = previous_action;
    } // end IF

  // step2: ignore[/block] certain signals

  // step2a: ignore certain signals
  // *NOTE*: there is no need to keep this around after registration
  ACE_Sig_Action ignore_action (static_cast<ACE_SignalHandler> (SIG_IGN), // ignore action
                                ACE_Sig_Set (false),                      // N/A
                                0);                                       // N/A
  int result = -1;
  for (int i = 1;
       i < ACE_NSIG;
       i++)
    if (ignoreSignals_in.is_member (i) > 0)
    {
      result = ignore_action.register_action (i,
                                              &previous_action);
      if (unlikely (result == -1))
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE_Sig_Action::register_action(%d: %S): \"%m\", continuing\n"),
                    i, i));
      } // end IF
      else
      {
        previousActions_out[i] = previous_action;
//        ACE_DEBUG ((LM_DEBUG,
//                    ACE_TEXT ("%t: ignoring signal %d: \"%S\"\n"),
//                    i, i));
        // sanity check(s)
        if (signals_in.is_member (i) > 0)
        {
          result = const_cast<ACE_Sig_Set&> (signals_in).sig_del (i);
          if (unlikely (result == -1))
          {
            ACE_DEBUG ((LM_ERROR,
                        ACE_TEXT ("failed to ACE_Sig_Set::sig_del(%d: %S): \"%m\", aborting\n"),
                        i, i));
            return false;
          } // end IF
          ACE_DEBUG ((LM_WARNING,
                      ACE_TEXT ("%t: removed %d: %S from handled signals\n"),
                      i, i));
        } // end IF
      } // end ELSE
    } // end IF

  // step3: register (process-wide) signal handler
  int flags_i = SA_SIGINFO   |
#if defined (ACE_WIN32) || defined (ACE_WIN64)
                SA_RESTART;
#else
                SA_RESTART   |
                SA_NOCLDSTOP |
// *WARNING*: this makes system() fail (errno: ECHILD)
//                SA_NOCLDWAIT |
                SA_NODEFER;
#endif // ACE_WIN32 || ACE_WIN64
  // *NOTE*: there is no need to keep this around after registration
  ACE_Sig_Action signal_action (NULL,                // N/A (reset in ACE_Sig_Handler::register_handler())
                                ACE_Sig_Set (false), // mask of signals to be blocked when servicing --> none
                                flags_i);            // flags
  switch (dispatch_in)
  {
    case COMMON_SIGNAL_DISPATCH_PROACTOR:
    case COMMON_SIGNAL_DISPATCH_SIGNAL:
    {
      ACE_Event_Handler* event_handler_p = NULL;
      for (int i = 1;
           i < ACE_NSIG;
           ++i)
        if (signals_in.is_member (i) > 0)
        {
          result =
            Common_Signal_Tools::signalHandler_.register_handler (i,
                                                                  eventHandler_in,
                                                                  &signal_action,
                                                                  &event_handler_p,
                                                                  &previous_action);
          if (unlikely (result == -1))
          {
            ACE_DEBUG ((LM_ERROR,
                        ACE_TEXT ("failed to ACE_Sig_Handler::register_handler(%d: %S): \"%m\", aborting\n"),
                        i, i));
            return false;
          } // end IF
          previousActions_out[i] = previous_action;

          // clean up
          if (event_handler_p)
          {
            delete event_handler_p; event_handler_p = NULL;
          } // end IF
        } // end IF

      break;
    }
    case COMMON_SIGNAL_DISPATCH_REACTOR:
    {
      ACE_Reactor* reactor_p = ACE_Reactor::instance ();
      ACE_ASSERT (reactor_p);
      result = reactor_p->register_handler (signals_in,
                                            eventHandler_in,
                                            &signal_action);
      if (unlikely (result == -1))
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE_Reactor::register_handler(): \"%m\", aborting\n")));
        return false;
      } // end IF

      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown signal dispatch mode (was: %d), aborting\n"),
                  dispatch_in));
      return false;
    }
  } // end SWITCH

#if defined (_DEBUG)
  Common_Signal_Tools::dump ();
#endif // _DEBUG

  return true;
}

void
Common_Signal_Tools::finalize (enum Common_SignalDispatchType dispatch_in,
                               const Common_SignalActions_t& previousActions_in,
                               const ACE_Sig_Set& previousMask_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Signal_Tools::finalize"));

  ACE_UNUSED_ARG (previousMask_in);

  int result = -1;

  // step1: restore previous signal handlers
  switch (dispatch_in)
  {
    case COMMON_SIGNAL_DISPATCH_PROACTOR:
    case COMMON_SIGNAL_DISPATCH_SIGNAL:
    {
      for (Common_SignalActionsIterator_t iterator = const_cast<Common_SignalActions_t&> (previousActions_in).begin ();
           iterator != const_cast<Common_SignalActions_t&> (previousActions_in).end ();
           ++iterator)
      {
        result =
            Common_Signal_Tools::signalHandler_.remove_handler ((*iterator).first,
                                                                const_cast<ACE_Sig_Action*> (&(*iterator).second),
                                                                NULL,
                                                                -1);
        if (unlikely (result == -1))
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to ACE_Sig_Handler::remove_handler(%d: \"%S\"): \"%m\", continuing\n"),
                      (*iterator).first, (*iterator).first));
      } // end FOR
      break;
    }
    case COMMON_SIGNAL_DISPATCH_REACTOR:
    {
      ACE_Reactor* reactor_p = ACE_Reactor::instance ();
      ACE_ASSERT (reactor_p);
      for (Common_SignalActionsIterator_t iterator = const_cast<Common_SignalActions_t&> (previousActions_in).begin ();
           iterator != const_cast<Common_SignalActions_t&> (previousActions_in).end ();
           ++iterator)
      {
        result =
            reactor_p->remove_handler ((*iterator).first,
                                       const_cast<ACE_Sig_Action*> (&(*iterator).second),
                                       NULL,
                                       -1);
        if (unlikely (result == -1))
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to ACE_Reactor::remove_handler(%d: \"%S\"): \"%m\", continuing\n"),
                      (*iterator).first, (*iterator).first));
      } // end FOR
      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown signal dispatch mode (was: %d), continuing\n"),
                  dispatch_in));
      break;
    }
  } // end SWITCH

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
  // step2: restore previous signal mask
  result = ACE_OS::thr_sigsetmask (SIG_SETMASK,
                                   const_cast<ACE_Sig_Set&> (previousMask_in),
                                   NULL);
  if (unlikely (result == -1))
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::thr_sigsetmask(): \"%m\", continuing\n")));
#endif // ACE_WIN32 || ACE_WIN64
}

void
Common_Signal_Tools::dump ()
{
  COMMON_TRACE (ACE_TEXT ("Common_Signal_Tools::dump"));

  int result = -1;

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
  // step1: retrieve blocked signals
  ACE_Sig_Set signals_a (false); // start with empty set
  result = ACE_OS::pthread_sigmask (0,
                                    NULL,
                                    signals_a);
  if (unlikely (result == -1))
  {
    ACE_DEBUG ((LM_ERROR,
               ACE_TEXT ("failed to ACE_OS::pthread_sigmask(): \"%m\", returning\n")));
    return;
  } // end IF
  for (int i = 1;
       i < ACE_NSIG;
       ++i)
    if (signals_a.is_member (i))
        ACE_DEBUG ((LM_DEBUG,
                    ACE_TEXT ("%t is blocking %d [\"%S\"]\n"),
                    i, i));
#endif // ACE_WIN32 || ACE_WIN64

  // step2: retrieve handled/ignored signals
  ACE_Sig_Action signal_action;
  for (int i = 1;
       i < ACE_NSIG;
       ++i)
  {
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    // *NOTE*: (currently,) this fails for all signals other than
    //         SIGINT, SIGILL, SIGABRT_COMPAT, SIGFPE, SIGSEGV, SIGTERM,
    //         SIGBREAK, SIGABRT
    if ((i != SIGINT) && (i != SIGILL) && (i != SIGABRT_COMPAT) &&
        (i != SIGFPE) && (i != SIGSEGV) && (i != SIGTERM) && (i != SIGBREAK) &&
        (i != SIGABRT))
      continue;
#endif // ACE_WIN32 || ACE_WIN64

    result = ACE_OS::sigaction (i,
                                NULL,
                                signal_action);
    if (unlikely (result == -1))
    {
#if defined (ACE_LINUX)
      // *NOTE*: (currently,) this fails for the NPTL signals 32,33
      int error_i = ACE_OS::last_error ();
      if (((i == 32) || (i == 33)) &&
          (error_i == EINVAL))
        continue;
#endif // ACE_LINUX
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_OS::sigaction(%d): \"%m\", returning\n"),
                  i));
      return;
    } // end IF
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("%t: %d [\"%S\"]\t\t\thandler: %s,\tflags: 0x%x\n"),
                i, i,
                ((signal_action.handler () == SIG_DFL) ? ACE_TEXT ("default")
                                                       : ((signal_action.handler () == SIG_IGN) ? ACE_TEXT ("ignored")
                                                                                                : signal_action.handler () ? ACE_TEXT ("set")
                                                                                                                           : ACE_TEXT ("not set"))),
               signal_action.flags ()));
  } // end FOR
}

std::string
Common_Signal_Tools::signalToString (const Common_Signal& signal_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Signal_Tools::signalToString"));

  // initialize return value
  std::string result;

  std::ostringstream converter;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  switch (signal_in.signal)
  {
    case SIGINT:
      result += ACE_TEXT_ALWAYS_CHAR ("SIGINT"); break;
    case SIGILL:
      result += ACE_TEXT_ALWAYS_CHAR ("SIGILL"); break;
    case SIGFPE:
      result += ACE_TEXT_ALWAYS_CHAR ("SIGFPE"); break;
    case SIGSEGV:
      result += ACE_TEXT_ALWAYS_CHAR ("SIGSEGV"); break;
    case SIGTERM:
      result += ACE_TEXT_ALWAYS_CHAR ("SIGTERM"); break;
    case SIGBREAK:
      result += ACE_TEXT_ALWAYS_CHAR ("SIGBREAK"); break;
    case SIGABRT:
      result += ACE_TEXT_ALWAYS_CHAR ("SIGABRT"); break;
    case SIGABRT_COMPAT:
      result += ACE_TEXT_ALWAYS_CHAR ("SIGABRT_COMPAT"); break;
    default:
    {
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("invalid/unknown signal: %S, continuing\n"),
                  signal_in.signal));
      break;
    }
  } // end SWITCH

  result += ACE_TEXT_ALWAYS_CHAR (", signalled handle: ");
  result += ACE_TEXT_ALWAYS_CHAR ("0x");
  converter << signal_in.siginfo.si_handle_;
  result += converter.str ();
  //result += ACE_TEXT_ALWAYS_CHAR (", array of signalled handle(s): ");
  //result += signal_in.siginfo.si_handles_;
#else
  // step0: common information (on POSIX.1b)
  result += ACE_TEXT_ALWAYS_CHAR ("PID/UID: ");
  converter << signal_in.siginfo.si_pid;
  result += converter.str ();
  result += ACE_TEXT_ALWAYS_CHAR ("/");
  converter.str (ACE_TEXT_ALWAYS_CHAR (""));
  converter.clear ();
  converter << signal_in.siginfo.si_uid;
  result += converter.str ();
  std::string user_name_string, real_name_string;
  Common_OS_Tools::getUserName (signal_in.siginfo.si_uid,
                                user_name_string,
                                real_name_string);
  result += ACE_TEXT_ALWAYS_CHAR ("[\"");
  result += user_name_string;
  result += ACE_TEXT_ALWAYS_CHAR ("\"]");

  // "si_signo,  si_errno  and  si_code are defined for all signals..."
  result += ACE_TEXT_ALWAYS_CHAR (", errno: ");
  converter.str (ACE_TEXT_ALWAYS_CHAR (""));
  converter.clear ();
  converter << signal_in.siginfo.si_errno;
  result += converter.str ();
  if (unlikely (signal_in.siginfo.si_errno))
  {
    result += ACE_TEXT_ALWAYS_CHAR ("[\"");
    result += ACE_OS::strerror (signal_in.siginfo.si_errno);
    result += ACE_TEXT_ALWAYS_CHAR ("\"], code: ");
  } // end IF
  else
    result += ACE_TEXT_ALWAYS_CHAR (", code: ");

  // step1: retrieve signal code
  switch (signal_in.siginfo.si_code)
  {
// *NOTE*: Solaris (11)-specific
#if defined (__sun) && defined (__SVR4)
    case SI_NOINFO:
        result += ACE_TEXT_ALWAYS_CHAR ("SI_NOINFO"); break;
    case SI_DTRACE:
        result += ACE_TEXT_ALWAYS_CHAR ("SI_DTRACE"); break;
    case SI_RCTL:
        result += ACE_TEXT_ALWAYS_CHAR ("SI_RCTL"); break;
/////////////////////////////////////////
#endif // __sun && __SVR4
    case SI_USER:
        result += ACE_TEXT_ALWAYS_CHAR ("SI_USER"); break;
// *NOTE*: Solaris (11)-specific
#if defined (__sun) && defined (__SVR4)
    case SI_LWP:
        result += ACE_TEXT_ALWAYS_CHAR ("SI_LWP"); break;
#else
    case SI_KERNEL:
        result += ACE_TEXT_ALWAYS_CHAR ("SI_KERNEL"); break;
#endif // __sun && __SVR4
    case SI_QUEUE:
      result += ACE_TEXT_ALWAYS_CHAR ("SI_QUEUE"); break;
    case SI_TIMER:
      result += ACE_TEXT_ALWAYS_CHAR ("SI_TIMER"); break;
    case SI_ASYNCIO:
      result += ACE_TEXT_ALWAYS_CHAR ("SI_ASYNCIO"); break;
    case SI_MESGQ:
      result += ACE_TEXT_ALWAYS_CHAR ("SI_MESGQ"); break;
// *NOTE*: Solaris (11)-specific
#if defined (__sun) && defined (__SVR4)
    case SI_LWP_QUEUE:
        result += ACE_TEXT_ALWAYS_CHAR ("SI_LWP_QUEUE"); break;
#else
    case SI_SIGIO:
        result += ACE_TEXT_ALWAYS_CHAR ("SI_SIGIO"); break;
    case SI_TKILL:
      result += ACE_TEXT_ALWAYS_CHAR ("SI_TKILL"); break;
#endif // __sun && __SVR4
    default:
    { // (signal-dependant) codes...
      switch (signal_in.signal)
      {
        case SIGILL:
        {
          switch (signal_in.siginfo.si_code)
          {
            case ILL_ILLOPC:
              result += ACE_TEXT_ALWAYS_CHAR ("ILL_ILLOPC"); break;
            case ILL_ILLOPN:
              result += ACE_TEXT_ALWAYS_CHAR ("ILL_ILLOPN"); break;
            case ILL_ILLADR:
              result += ACE_TEXT_ALWAYS_CHAR ("ILL_ILLADR"); break;
            case ILL_ILLTRP:
              result += ACE_TEXT_ALWAYS_CHAR ("ILL_ILLTRP"); break;
            case ILL_PRVOPC:
              result += ACE_TEXT_ALWAYS_CHAR ("ILL_PRVOPC"); break;
            case ILL_PRVREG:
              result += ACE_TEXT_ALWAYS_CHAR ("ILL_PRVREG"); break;
            case ILL_COPROC:
              result += ACE_TEXT_ALWAYS_CHAR ("ILL_COPROC"); break;
            case ILL_BADSTK:
              result += ACE_TEXT_ALWAYS_CHAR ("ILL_BADSTK"); break;
            default:
            {
              ACE_DEBUG ((LM_ERROR,
                          ACE_TEXT ("invalid/unknown signal code: %d, continuing\n"),
                          signal_in.siginfo.si_code));
              break;
            }
          } // end SWITCH

          break;
        }
        case SIGFPE:
        {
          switch (signal_in.siginfo.si_code)
          {
            case FPE_INTDIV:
              result += ACE_TEXT_ALWAYS_CHAR ("FPE_INTDIV"); break;
            case FPE_INTOVF:
              result += ACE_TEXT_ALWAYS_CHAR ("FPE_INTOVF"); break;
            case FPE_FLTDIV:
              result += ACE_TEXT_ALWAYS_CHAR ("FPE_FLTDIV"); break;
            case FPE_FLTOVF:
              result += ACE_TEXT_ALWAYS_CHAR ("FPE_FLTOVF"); break;
            case FPE_FLTUND:
              result += ACE_TEXT_ALWAYS_CHAR ("FPE_FLTUND"); break;
            case FPE_FLTRES:
              result += ACE_TEXT_ALWAYS_CHAR ("FPE_FLTRES"); break;
            case FPE_FLTINV:
              result += ACE_TEXT_ALWAYS_CHAR ("FPE_FLTINV"); break;
            case FPE_FLTSUB:
              result += ACE_TEXT_ALWAYS_CHAR ("FPE_FLTSUB"); break;
            default:
            {
              ACE_DEBUG ((LM_ERROR,
                          ACE_TEXT ("invalid/unknown signal code: %d, continuing\n"),
                          signal_in.siginfo.si_code));
              break;
            }
          } // end SWITCH
          break;
        }
        case SIGSEGV:
        {
          switch (signal_in.siginfo.si_code)
          {
            case SEGV_MAPERR:
              result += ACE_TEXT_ALWAYS_CHAR ("SEGV_MAPERR"); break;
            case SEGV_ACCERR:
              result += ACE_TEXT_ALWAYS_CHAR ("SEGV_ACCERR"); break;
            default:
            {
              ACE_DEBUG ((LM_ERROR,
                          ACE_TEXT ("invalid/unknown signal code: %d, continuing\n"),
                          signal_in.siginfo.si_code));
              break;
            }
          } // end SWITCH
          break;
        }
        case SIGBUS:
        {
          switch (signal_in.siginfo.si_code)
          {
            case BUS_ADRALN:
              result += ACE_TEXT_ALWAYS_CHAR ("BUS_ADRALN"); break;
            case BUS_ADRERR:
              result += ACE_TEXT_ALWAYS_CHAR ("BUS_ADRERR"); break;
            case BUS_OBJERR:
              result += ACE_TEXT_ALWAYS_CHAR ("BUS_OBJERR"); break;
            default:
            {
              ACE_DEBUG ((LM_ERROR,
                          ACE_TEXT ("invalid/unknown signal code: %d, continuing\n"),
                          signal_in.siginfo.si_code));
              break;
            }
          } // end SWITCH
          break;
        }
        case SIGTRAP:
        {
          switch (signal_in.siginfo.si_code)
          {
            case TRAP_BRKPT:
              result += ACE_TEXT_ALWAYS_CHAR ("TRAP_BRKPT"); break;
            case TRAP_TRACE:
              result += ACE_TEXT_ALWAYS_CHAR ("TRAP_TRACE"); break;
            default:
            {
              ACE_DEBUG ((LM_ERROR,
                          ACE_TEXT ("invalid/unknown signal code: %d, continuing\n"),
                          signal_in.siginfo.si_code));
              break;
            }
          } // end SWITCH
          break;
        }
        case SIGCHLD:
        {
          switch (signal_in.siginfo.si_code)
          {
            case CLD_EXITED:
              result += ACE_TEXT_ALWAYS_CHAR ("CLD_EXITED"); break;
            case CLD_KILLED:
              result += ACE_TEXT_ALWAYS_CHAR ("CLD_KILLED"); break;
            case CLD_DUMPED:
              result += ACE_TEXT_ALWAYS_CHAR ("CLD_DUMPED"); break;
            case CLD_TRAPPED:
              result += ACE_TEXT_ALWAYS_CHAR ("CLD_TRAPPED"); break;
            case CLD_STOPPED:
              result += ACE_TEXT_ALWAYS_CHAR ("CLD_STOPPED"); break;
            case CLD_CONTINUED:
              result += ACE_TEXT_ALWAYS_CHAR ("CLD_CONTINUED"); break;
            default:
            {
              ACE_DEBUG ((LM_ERROR,
                          ACE_TEXT ("invalid/unknown signal code: %d, continuing\n"),
                          signal_in.siginfo.si_code));
              break;
            }
          } // end SWITCH
          break;
        }
        case SIGPOLL:
        {
          switch (signal_in.siginfo.si_code)
          {
            case POLL_IN:
              result += ACE_TEXT_ALWAYS_CHAR ("POLL_IN"); break;
            case POLL_OUT:
              result += ACE_TEXT_ALWAYS_CHAR ("POLL_OUT"); break;
            case POLL_MSG:
              result += ACE_TEXT_ALWAYS_CHAR ("POLL_MSG"); break;
            case POLL_ERR:
              result += ACE_TEXT_ALWAYS_CHAR ("POLL_ERR"); break;
            case POLL_PRI:
              result += ACE_TEXT_ALWAYS_CHAR ("POLL_PRI"); break;
            case POLL_HUP:
              result += ACE_TEXT_ALWAYS_CHAR ("POLL_HUP"); break;
            default:
            {
              ACE_DEBUG ((LM_ERROR,
                          ACE_TEXT ("invalid/unknown signal code: %d, continuing\n"),
                          signal_in.siginfo.si_code));
              break;
            }
          } // end SWITCH
          break;
        }
        default:
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("invalid/unknown signal code: %d, continuing\n"),
                      signal_in.siginfo.si_code));
          break;
        }
      } // end SWITCH
      break;
    }
  } // end SWITCH

  // step2: retrieve more (signal-dependant) information
  switch (signal_in.signal)
  {
    case SIGALRM:
    {
#if defined (ACE_LINUX)
      result += ACE_TEXT_ALWAYS_CHAR (", overrun: ");
      converter.str (ACE_TEXT_ALWAYS_CHAR (""));
      converter.clear ();
      converter << signal_in.siginfo.si_overrun;
      result += converter.str ();
      result += ACE_TEXT_ALWAYS_CHAR (", (internal) id: ");
      converter.str (ACE_TEXT_ALWAYS_CHAR (""));
      converter.clear ();
      converter << signal_in.siginfo.si_timerid;
      result += converter.str ();
#endif // ACE_LINUX
      break;
    }
    case SIGCHLD:
    {
      result += ACE_TEXT_ALWAYS_CHAR (", (exit) status: ");
      converter.str (ACE_TEXT_ALWAYS_CHAR (""));
      converter.clear ();
      converter << signal_in.siginfo.si_status;
      result += converter.str ();
      result += ACE_TEXT_ALWAYS_CHAR (", time consumed (user/system): ");
      converter.str (ACE_TEXT_ALWAYS_CHAR (""));
      converter.clear ();
      converter << signal_in.siginfo.si_utime;
      result += converter.str ();
      result += ACE_TEXT_ALWAYS_CHAR (" / ");
      converter.str (ACE_TEXT_ALWAYS_CHAR (""));
      converter.clear ();
      converter << signal_in.siginfo.si_stime;
      result += converter.str ();
      break;
    }
    case SIGILL:
    case SIGFPE:
    case SIGSEGV:
    case SIGBUS:
    {
      // *TODO*: more data ?
      result += ACE_TEXT_ALWAYS_CHAR (", fault at address: ");
      converter.str (ACE_TEXT_ALWAYS_CHAR (""));
      converter.clear ();
      converter << signal_in.siginfo.si_addr;
      result += converter.str ();
      break;
    }
    case SIGPOLL:
    {
      result += ACE_TEXT_ALWAYS_CHAR (", band event: ");
      converter.str (ACE_TEXT_ALWAYS_CHAR (""));
      converter.clear ();
      converter << signal_in.siginfo.si_band;
      result += converter.str ();
      result += ACE_TEXT_ALWAYS_CHAR (", (file) descriptor: ");
      converter.str (ACE_TEXT_ALWAYS_CHAR (""));
      converter.clear ();
      converter << signal_in.siginfo.si_fd;
      result += converter.str ();
      break;
    }
    default:
    {
      // *TODO*: handle more signals here ?
//       ACE_DEBUG ((LM_DEBUG,
//                   ACE_TEXT ("no additional information for signal: \"%S\"\n"),
//                   signal_in.signal));
      break;
    }
  } // end SWITCH
#endif // ACE_WIN32 || ACE_WIN64

  return result;
}

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
void
Common_Signal_Tools::unblockRealtimeSignals (sigset_t& originalMask_out)
{
  COMMON_TRACE (ACE_TEXT ("Common_Signal_Tools::unblockRealtimeSignals"));

  int result = -1;

  // initialize return value(s)
  result = ACE_OS::sigemptyset (&originalMask_out);
  if (result == - 1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::sigemptyset(): \"%m\", returning\n")));
    return;
  } // end IF

  sigset_t signal_set;
  result = ACE_OS::sigemptyset (&signal_set);
  if (result == - 1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::sigemptyset(): \"%m\", returning\n")));
    return;
  } // end IF
  for (int i = ACE_SIGRTMIN;
       i <= ACE_SIGRTMAX;
       i++)
  {
    result = ACE_OS::sigaddset (&signal_set, i);
    if (result == -1)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_OS::sigaddset(): \"%m\", returning\n")));
      return;
    } // end IF
  } // end FOR

  result = ACE_OS::thr_sigsetmask (SIG_UNBLOCK,
                                   &signal_set,
                                   &originalMask_out);
  if (result == -1)
  {
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("failed to ACE_OS::thr_sigsetmask(SIG_UNBLOCK): \"%m\", returning\n")));
    return;
  } // end IF
}

void
Common_Signal_Tools::handleRealtimeSignals ()
{
  COMMON_TRACE (ACE_TEXT ("Common_Signal_Tools::handleRealtimeSignals"));

  int result = -1;

  ACE_Sig_Set signal_set (false);
  for (int i = ACE_SIGRTMIN;
       i <= ACE_SIGRTMAX;
       i++)
  {
    result = signal_set.sig_add (i);
    if (result == -1)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_Sig_Set::sig_add(): \"%m\", returning\n")));
      return;
    } // end IF
  } // end FOR

  int flags_i = //SA_SIGINFO   |
#if defined (ACE_WIN32) || defined (ACE_WIN64)
                SA_RESTART;
#else
                SA_RESTART   |
                SA_NOCLDSTOP |
// *WARNING*: this makes system() fail (errno: ECHILD)
//                SA_NOCLDWAIT |
                SA_NODEFER;
#endif // ACE_WIN32 || ACE_WIN64
  ACE_Sig_Action signal_action (signal_set,
                                common_default_rt_sig_handler_cb,
//                                ACE_SIGNAL_C_FUNC (common_default_rt_sig_handler_cb),
                                NULL,
                                flags_i);
}
#endif // ACE_WIN32 || ACE_WIN64
