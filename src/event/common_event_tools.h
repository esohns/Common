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

#ifndef COMMON_EVENT_TOOLS_H
#define COMMON_EVENT_TOOLS_H

#include "ace/Global_Macros.h"

ACE_THR_FUNC_RETURN
common_event_dispatch_function (void*);

class Common_Event_Tools
{
 public:
  // *WARNING*: the configuration may (!) be updated with platform-specific
  //            settings; hence the non-const argument
  static bool initializeEventDispatch (struct Common_EventDispatchConfiguration&); // configuration (i/o)
  // *NOTE*: the state handle is updated with the thread group ids (if any);
  //         hence the non-const argument
  // *WARNING*: iff any worker thread(s) is/are spawned, a handle to the first
  //            argument is passed to the dispatch thread function as argument
  //            --> ensure it does not fall off the stack prematurely
  static bool startEventDispatch (struct Common_EventDispatchState&); // thread data (i/o)
  static void dispatchEvents (struct Common_EventDispatchState&); // thread data (i/o)
  static void finalizeEventDispatch (struct Common_EventDispatchState&, // thread data (i/o)
                                     bool = false, // wait for completion ?
                                     bool = true); // release reactor/proactor singletons ?

 private:
  ACE_UNIMPLEMENTED_FUNC (Common_Event_Tools ())
  ACE_UNIMPLEMENTED_FUNC (~Common_Event_Tools ())
  ACE_UNIMPLEMENTED_FUNC (Common_Event_Tools (const Common_Event_Tools&))
  ACE_UNIMPLEMENTED_FUNC (Common_Event_Tools& operator= (const Common_Event_Tools&))

  // *IMPORTANT NOTE*: -in the sense of what COMMON_REACTOR_ACE_DEFAULT maps to
  //                   on this (!) platform (check ACE source code)
  static bool defaultPlatformReactorIsSelectBased ();
};

#endif
