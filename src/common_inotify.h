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

#ifndef COMMON_INOTIFY_H
#define COMMON_INOTIFY_H

class Common_IDispatch
{
 public:
  virtual void dispatch (void*) = 0; // user data
};

template <typename EventType>
class Common_IDispatch_T
{
 public:
  virtual void dispatch (const EventType&) = 0; // event (state/status change, ...)
};

template <typename IdType,
          typename EventType>
class Common_INotify_T
{
 public:
  virtual void notify (IdType,                // event id
                       const EventType&) = 0; // event (state/status change, ...)
};

#endif
