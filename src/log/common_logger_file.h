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

#ifndef COMMON_LOGGER_FILE_T_H
#define COMMON_LOGGER_FILE_T_H

#include <string>

#include "ace/Global_Macros.h"
#include "ace/Log_Msg_Backend.h"
#include "ace/Log_Record.h"
#include "ace/Synch_Traits.h"

template <ACE_SYNCH_DECL>
class Common_Logger_File_T
 : public ACE_Log_Msg_Backend
{
  typedef ACE_Log_Msg_Backend inherited;

 public:
  Common_Logger_File_T ();
  virtual ~Common_Logger_File_T ();

  bool initialize (const std::string&,  // file name
                   ACE_SYNCH_MUTEX_T*); // lock handle {NULL: single-threaded application}

  // implement ACE_Log_Msg_Backend interface
  virtual int open (const ACE_TCHAR*); // logger key
  virtual int reset (void);
  virtual int close (void);
  virtual ssize_t log (ACE_Log_Record&); // record

 private:
  ACE_UNIMPLEMENTED_FUNC (Common_Logger_File_T (const Common_Logger_File_T&))
  ACE_UNIMPLEMENTED_FUNC (Common_Logger_File_T& operator= (const Common_Logger_File_T&))

  FILE*              buffer_;
  bool               isInitialized_;
  ACE_SYNCH_MUTEX_T* lock_;
};

// include template definition
#include "common_logger_file.inl"

//////////////////////////////////////////

typedef Common_Logger_File_T<ACE_MT_SYNCH> Common_Logger_File_t;
//typedef Common_Logger_T<ACE_NULL_SYNCH> Common_NullSynchLogger_t;

#endif
