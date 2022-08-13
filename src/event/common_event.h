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

#ifndef COMMON_EVENT_H
#define COMMON_EVENT_H

#include "ace/Global_Macros.h"

class Common_Event
{
 public:
  Common_Event (HRESULT*);
  inline virtual ~Common_Event () { if (event_) CloseHandle (event_); }

  inline operator HANDLE () const { return event_; }

  inline void Set () { SetEvent (event_); }
  inline bool Wait (DWORD dwTimeout_in = INFINITE) { return (WaitForSingleObject (event_, dwTimeout_in) == WAIT_OBJECT_0); }
  inline void Reset () { ResetEvent (event_); }
  inline bool Check () { return Wait (0); }

 protected:
  HANDLE event_;

 private:
  ACE_UNIMPLEMENTED_FUNC (Common_Event (const Common_Event&));
  ACE_UNIMPLEMENTED_FUNC (Common_Event &operator=(const Common_Event&));
};

#endif /* COMMON_EVENT_H */
