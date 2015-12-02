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

#ifndef COMMON_LOGGER_H
#define COMMON_LOGGER_H

//#include <cstdlib>

#include "ace/Global_Macros.h"
#include "ace/Log_Msg_Backend.h"
#include "ace/Log_Record.h"
#include "ace/Synch_Traits.h"

#include "common.h"
#include "common_exports.h"

class Common_Export Common_Logger
 : public ACE_Log_Msg_Backend
{
 public:
  Common_Logger (Common_MessageStack_t*,      // message stack handle
                 ACE_SYNCH_RECURSIVE_MUTEX*); // lock handle
  virtual ~Common_Logger ();

  // implement ACE_Log_Msg_Backend interface
  virtual int open (const ACE_TCHAR*); // logger key
  virtual int reset (void);
  virtual int close (void);
  virtual ssize_t log (ACE_Log_Record&); // record

 private:
  typedef ACE_Log_Msg_Backend inherited;

  ACE_UNIMPLEMENTED_FUNC (Common_Logger ())
  ACE_UNIMPLEMENTED_FUNC (Common_Logger (const Common_Logger&))
  ACE_UNIMPLEMENTED_FUNC (Common_Logger& operator= (const Common_Logger&))

//  FILE*                  buffer_;

  ACE_SYNCH_RECURSIVE_MUTEX* lock_;
  Common_MessageStack_t*     messageStack_;
};

#endif
