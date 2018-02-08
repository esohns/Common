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

#include "ace/Synch.h"
#include "common_signal_tools.h"

#include <sstream>

#include "ace/Log_Msg.h"
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
#include "ace/POSIX_Proactor.h"
#endif
#include "ace/Proactor.h"
#include "ace/Reactor.h"

#include "common_defines.h"
#include "common_macros.h"

// initialize statics
ACE_Sig_Handler Common_Signal_Tools::signalHandler_;

//////////////////////////////////////////

bool
Common_Signal_Tools::preInitialize (ACE_Sig_Set& signals_inout,
                                    bool useReactor_in,
                                    Common_SignalActions_t& previousActions_out,
                                    sigset_t& originalMask_out)
{
  COMMON_TRACE (ACE_TEXT ("Common_Signal_Tools::preInitialize"));

  int result = -1;

  // initialize return value(s)
  previousActions_out.clear ();
  result = ACE_OS::sigemptyset (&originalMask_out);
  if (unlikely (result == - 1))
  {
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("failed to ACE_OS::sigemptyset(): \"%m\", aborting\n")));
    return false;
  } // end IF

  // *IMPORTANT NOTE*: "The signal disposition is a per-process attribute: in a
  // multithreaded application, the disposition of a particular signal is the
  // same for all threads." (see man(7) signal)

  // step1: ignore SIGPIPE: continue gracefully after a client suddenly
  // disconnects (i.e. application/system crash, etc...)
  // --> specify ignore action
  // *IMPORTANT NOTE*: do NOT restart system calls in this case (see manual)
  // *NOTE*: there is no need to keep this around after registration
  ACE_Sig_Action ignore_action (static_cast<ACE_SignalHandler> (SIG_IGN), // ignore action
                                ACE_Sig_Set (1),                          // mask of signals to be blocked when servicing
                                                                          // --> block them all (bar KILL/STOP; see manual)
                                0);                                       // flags
  ACE_Sig_Action previous_action;
  result = ignore_action.register_action (SIGPIPE,
                                          &previous_action);
  if (unlikely (result == -1))
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("failed to ACE_Sig_Action::register_action(%d: %S): \"%m\", continuing\n"),
                SIGPIPE, SIGPIPE));
  else
    previousActions_out[SIGPIPE] = previous_action;

  // step2: block certain signals
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
  // *IMPORTANT NOTE*: child threads inherit the signal mask of their parent
  //                   --> make sure this is the main thread

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

  // *IMPORTANT NOTE*: "...NPTL makes internal use of the first two real-time
  //                   signals (see also signal(7)); these signals cannot be
  //                   used in applications. ..." (see 'man 7 pthreads')
  //                   --> on POSIX platforms, ensure that ACE_SIGRTMIN == 34

  if (!useReactor_in)
  {
    ACE_POSIX_Proactor::Proactor_Type proactor_type;
    ACE_Proactor* proactor_p = ACE_Proactor::instance ();
    ACE_ASSERT (proactor_p);
    ACE_POSIX_Proactor* proactor_impl_p =
        dynamic_cast<ACE_POSIX_Proactor*> (proactor_p->implementation ());
    if (!proactor_impl_p)
    {
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("dynamic_cast<ACE_POSIX_Proactor> failed, continuing\n")));
      goto _continue;
    } // end IF
    proactor_type = proactor_impl_p->get_impl_type ();
    if (proactor_type != ACE_POSIX_Proactor::PROACTOR_SIG)
      goto _continue;

    sigset_t rt_signal_set;
    result = ACE_OS::sigemptyset (&rt_signal_set);
    if (unlikely (result == - 1))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_OS::sigemptyset(): \"%m\", aborting\n")));
      return false;
    } // end IF
    unsigned int number = 0;
    for (int i = ACE_SIGRTMIN;
         i <= ACE_SIGRTMAX;
         ++i, number++)
    {
      result = ACE_OS::sigaddset (&rt_signal_set, i);
      if (unlikely (result == -1))
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE_OS::sigaddset(%d: %S): \"%m\", continuing\n"),
                    i, i));
        number--;
      } // end IF

      // remove any RT signals from the signal set handled by the application
      if (signals_inout.is_member (i))
      {
        result = signals_inout.sig_del (i); // <-- let the event dispatch handle
                                            //     all RT signals
        if (unlikely (result == -1))
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to ACE_Sig_Set::sig_del(%d: %S): \"%m\", continuing\n"),
                      i, i));
//        else
//          ACE_DEBUG ((LM_DEBUG,
//                      ACE_TEXT ("removed %d: %S from handled signals\n"),
//                      i, i));
      } // end IF
    } // end IF
    result = ACE_OS::thr_sigsetmask (SIG_BLOCK,
                                     &rt_signal_set,
                                     &originalMask_out);
    if (unlikely (result == -1))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_OS::thr_sigsetmask(): \"%m\", aborting\n")));
      return false;
    } // end IF
#if defined (_DEBUG)
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("%t: blocked %d real-time signal(s) [%d: %S - %d: %S]\n"),
                ACE_SIGRTMIN, ACE_SIGRTMIN, ACE_SIGRTMAX, ACE_SIGRTMAX,
                number));
#endif
  } // end IF
_continue:
#endif

  // *NOTE*: remove SIGSEGV to enable core dumps on non-Win32 systems
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
  if (signals_inout.is_member (SIGSEGV))
  {
    result = signals_inout.sig_del (SIGSEGV);
    if (unlikely (result == -1))
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_Sig_Set::sig_del(%d: %S): \"%m\", continuing\n"),
                  SIGSEGV, SIGSEGV));
//    else
//      ACE_DEBUG ((LM_DEBUG,
//                  ACE_TEXT ("removed %d: %S from handled signals\n"),
//                  SIGSEGV, SIGSEGV));
  } // end IF
#endif

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
    if (signals_in.is_member (i))
    {
      previous_action.retrieve_action (i);
      previousActions_out[i] = previous_action;
    } // end IF

  // step2: ignore[/block] certain signals

  // step2a: ignore certain signals
  // *NOTE*: there is no need to keep this around after registration
  ACE_Sig_Action ignore_action (static_cast<ACE_SignalHandler> (SIG_IGN), // ignore action
                                ACE_Sig_Set (1),                          // mask of signals to be blocked when servicing
                                                                          // --> block them all (bar KILL/STOP; see manual)
                                SA_RESTART);                              // flags
  int result = -1;
  for (int i = 1;
       i < ACE_NSIG;
       i++)
    if (ignoreSignals_in.is_member (i))
    {
      result = ignore_action.register_action (i,
                                              &previous_action);
      if (unlikely (result == -1))
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE_Sig_Action::register_action(%d: %S): \"%m\", continuing\n"),
                    i, i));
      else
      {
        previousActions_out[i] = previous_action;
//        ACE_DEBUG ((LM_DEBUG,
//                    ACE_TEXT ("ignoring signal %d: %S\n"),
//                    i, i));
      } // end IF
    } // end IF

  // step3: register (process-wide) signal handler
  // *NOTE*: there is no need to keep this around after registration
  ACE_Sig_Action default_action (static_cast<ACE_SignalHandler> (SIG_DFL), // default action
                                 ACE_Sig_Set (1),                          // mask of signals to be blocked when servicing
                                                                           // --> block them all (bar KILL/STOP; see manual)
                                 (SA_RESTART | SA_SIGINFO));               // flags
  switch (dispatch_in)
  {
    case COMMON_SIGNAL_DISPATCH_PROACTOR:
    case COMMON_SIGNAL_DISPATCH_SIGNAL:
    {
      ACE_Event_Handler* event_handler_p = NULL;
      ACE_Sig_Action previous_action;
      for (int i = 1;
           i < ACE_NSIG;
           ++i)
        if (signals_in.is_member (i))
        {
          result =
              Common_Signal_Tools::signalHandler_.register_handler (i,
                                                                    eventHandler_in,
                                                                    &default_action,
                                                                    &event_handler_p,
                                                                    &previous_action);
          if (unlikely (result == -1))
          {
            ACE_DEBUG ((LM_ERROR,
                        ACE_TEXT ("failed to ACE_Sig_Handler::register_handler(%d: %S): \"%m\", aborting\n"),
                        i, i));
            return false;
          } // end IF
//          else
//            ACE_DEBUG ((LM_DEBUG,
//                        ACE_TEXT ("handling signal %d: \"%S\"\n"),
//                        i, i));

          // clean up
          if (event_handler_p)
          {
            delete event_handler_p;
            event_handler_p = NULL;
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
                                            &default_action);
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
  for (int i = 1;
       i < ACE_NSIG;
       ++i)
    if (signals_in.is_member (i))
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("handling signal %d: \"%S\"\n"),
                  i, i));
#endif

  return true;
}

void
Common_Signal_Tools::finalize (enum Common_SignalDispatchType dispatch_in,
                               const ACE_Sig_Set& signals_in,
                               const Common_SignalActions_t& previousActions_in,
                               const sigset_t& previousMask_in)
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
      ACE_Sig_Action current_action;
      Common_SignalActionsIterator_t iterator;
      for (int i = 1;
           i < ACE_NSIG;
           ++i)
        if (signals_in.is_member (i))
        {
          iterator = previousActions_in.find (i);
          if (likely (iterator != previousActions_in.end ()))
          {
            result =
                Common_Signal_Tools::signalHandler_.remove_handler (i,
                                                                    const_cast<ACE_Sig_Action*> (&(*iterator).second),
                                                                    &current_action,
                                                                    -1);
            if (unlikely (result == -1))
              ACE_DEBUG ((LM_ERROR,
                          ACE_TEXT ("failed to ACE_Sig_Handler::remove_handler(%d: %S): \"%m\", continuing\n"),
                          i, i));
          } // end IF
        } // end IF

      break;
    }
    case COMMON_SIGNAL_DISPATCH_REACTOR:
    {
      ACE_Reactor* reactor_p = ACE_Reactor::instance ();
      ACE_ASSERT (reactor_p);
      result = reactor_p->remove_handler (signals_in);
      if (unlikely (result == -1))
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE_Reactor::remove_handler(): \"%m\", continuing\n")));

      for (Common_SignalActionsIterator_t iterator = previousActions_in.begin ();
           iterator != previousActions_in.end ();
           iterator++)
      {
        result =
            const_cast<ACE_Sig_Action&> ((*iterator).second).register_action ((*iterator).first,
                                                                              NULL);
        if (unlikely (result == -1))
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to ACE_Sig_Action::register_action(%S): \"%m\", continuing\n"),
                      (*iterator).first));
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
                                   &previousMask_in,
                                   NULL);
  if (unlikely (result == -1))
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::thr_sigsetmask(): \"%m\", continuing\n")));
#endif
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
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  result += ACE_TEXT_ALWAYS_CHAR ("0x");
#endif
  converter << signal_in.siginfo.si_handle_;
  result += converter.str ();
  //result += ACE_TEXT_ALWAYS_CHAR (", array of signalled handle(s): ");
  //result += signal_in.siginfo.si_handles_;
#else
  int result_2 = -1;

  // step0: common information (on POSIX.1b)
  result += ACE_TEXT_ALWAYS_CHAR ("PID/UID: ");
  converter << signal_in.siginfo.si_pid;
  result += converter.str ();
  result += ACE_TEXT_ALWAYS_CHAR ("/");
  converter.str (ACE_TEXT_ALWAYS_CHAR (""));
  converter.clear ();
  converter << signal_in.siginfo.si_uid;
  result += converter.str ();

  // (try to) get user name
  ACE_TCHAR buffer_a[BUFSIZ];
  //  ACE_TCHAR buffer[L_cuserid];
  ACE_OS::memset (buffer_a, 0, sizeof (ACE_TCHAR[BUFSIZ]));
//  char* result_p = ACE_OS::cuserid (buffer_a);
//  if (!result_p)
  struct passwd passwd_s;
  struct passwd* passwd_p = NULL;
// *PORTABILITY*: this isn't very portable (man getpwuid_r)
  result_2 = ::getpwuid_r (signal_in.siginfo.si_uid,
                           &passwd_s,
                           buffer_a,
                           sizeof (ACE_TCHAR[BUFSIZ]),
                           &passwd_p);
  if (unlikely (result_2 || !passwd_p))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ::getpwuid_r(%u) : \"%m\", continuing\n"),
                signal_in.siginfo.si_uid));
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("failed to ACE_OS::cuserid() : \"%m\", continuing\n")));
  } // end IF
  else
  {
    result += ACE_TEXT_ALWAYS_CHAR ("[\"");
    result += passwd_s.pw_name;
//    information << buffer;
    result += ACE_TEXT_ALWAYS_CHAR ("\"]");
  } // end ELSE

  // "si_signo,  si_errno  and  si_code are defined for all signals..."
  result += ACE_TEXT_ALWAYS_CHAR (", errno: ");
  converter.str (ACE_TEXT_ALWAYS_CHAR (""));
  converter.clear ();
  converter << signal_in.siginfo.si_errno;
  result += converter.str ();
  result += ACE_TEXT_ALWAYS_CHAR ("[\"");
  result += ACE_OS::strerror (signal_in.siginfo.si_errno);
  result += ACE_TEXT_ALWAYS_CHAR ("\"], code: ");

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
#endif
  case SI_USER:
      result += ACE_TEXT_ALWAYS_CHAR ("SI_USER"); break;
// *NOTE*: Solaris (11)-specific
#if defined (__sun) && defined (__SVR4)
  case SI_LWP:
      result += ACE_TEXT_ALWAYS_CHAR ("SI_LWP"); break;
#else
  case SI_KERNEL:
      result += ACE_TEXT_ALWAYS_CHAR ("SI_KERNEL"); break;
#endif
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
#endif
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
#if defined (__linux__)
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
#endif
      break;
    }
    case SIGCHLD:
    {
      result += ACE_TEXT_ALWAYS_CHAR (", (exit) status: ");
      converter.str (ACE_TEXT_ALWAYS_CHAR (""));
      converter.clear ();
      converter << signal_in.siginfo.si_status;
      result += converter.str ();
      result += ACE_TEXT_ALWAYS_CHAR (", time consumed (user): ");
      converter.str (ACE_TEXT_ALWAYS_CHAR (""));
      converter.clear ();
      converter << signal_in.siginfo.si_utime;
      result += converter.str ();
      result += ACE_TEXT_ALWAYS_CHAR (" / (system): ");
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
#endif

  return result;
}

//void
//Common_Signal_Tools::unblockRealtimeSignals (sigset_t& originalMask_out)
//{
//  COMMON_TRACE (ACE_TEXT ("Common_Signal_Tools::unblockRealtimeSignals"));
//
//  int result = -1;
//
//  // initialize return value(s)
//  result = ACE_OS::sigemptyset (&originalMask_out);
//  if (result == - 1)
//  {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("failed to ACE_OS::sigemptyset(): \"%m\", returning\n")));
//    return;
//  } // end IF
//
//  sigset_t signal_set;
//  result = ACE_OS::sigemptyset (&signal_set);
//  if (result == - 1)
//  {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("failed to ACE_OS::sigemptyset(): \"%m\", returning\n")));
//    return;
//  } // end IF
//  for (int i = ACE_SIGRTMIN;
//       i <= ACE_SIGRTMAX;
//       i++)
//  {
//    result = ACE_OS::sigaddset (&signal_set, i);
//    if (result == -1)
//    {
//      ACE_DEBUG ((LM_ERROR,
//                  ACE_TEXT ("failed to ACE_OS::sigaddset(): \"%m\", returning\n")));
//      return;
//    } // end IF
//  } // end FOR
//
//  result = ACE_OS::thr_sigsetmask (SIG_UNBLOCK,
//                                   &signal_set,
//                                   &originalMask_out);
//  if (result == -1)
//  {
//    ACE_DEBUG ((LM_DEBUG,
//                ACE_TEXT ("failed to ACE_OS::thr_sigsetmask(SIG_UNBLOCK): \"%m\", returning\n")));
//    return;
//  } // end IF
//}
