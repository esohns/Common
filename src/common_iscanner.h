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

#ifndef COMMON_ISCANNER_H
#define COMMON_ISCANNER_H

#include <string>

// forward declarations
class ACE_Message_Block;

class Common_IScanner
{
 public:
  virtual ~Common_IScanner () {};

  virtual ACE_Message_Block* buffer () = 0;
  virtual bool debugScanner () const = 0;
  virtual bool isBlocking () const = 0;

  virtual void error (const std::string&) = 0;

  // *NOTE*: to be invoked by the scanner (ONLY !)
  virtual void offset (unsigned int) = 0; // offset (increment)
  virtual unsigned int offset () const = 0;

  // *IMPORTANT NOTE*: when the parser detects a frame end, it inserts a new
  //                   buffer to the continuation and passes 'true'
  //                   --> separate the current frame from the next
  virtual bool switchBuffer (bool = false) = 0; // unlink current buffer ?
  virtual void waitBuffer () = 0;
};

#endif
