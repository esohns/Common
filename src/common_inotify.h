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

template <typename SessionIdType,
          typename SessionDataType,
          typename MessageType,
          typename SessionMessageType>
class Common_INotify_T
{
 public:
  virtual void start (SessionIdType,                   // session id
                      const SessionDataType&) = 0;     // session data
  virtual void notify (SessionIdType,                  // session id
                       const MessageType&) = 0;        // (protocol) data
  virtual void notify (SessionIdType,                  // session id
                       const SessionMessageType&) = 0; // session message
  virtual void end (SessionIdType) = 0;                // session id
};

#endif
