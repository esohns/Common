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

#ifndef COMMON_ISUBSCRIBE_H
#define COMMON_ISUBSCRIBE_H

class Common_IRegister
{
 public:
  // *IMPORTANT NOTE*: 'register' is a keyword an cannot be used as identifier
  virtual bool registerc () = 0;
  virtual void deregister () = 0;
};

template <typename InterfaceType>
class Common_IRegister_T
{
 public:
  // *IMPORTANT NOTE*: 'register' is a keyword an cannot be used as identifier
  virtual bool registerc (InterfaceType*) = 0;
  virtual void deregister (InterfaceType*) = 0;
};

//////////////////////////////////////////

template <typename InterfaceType>
class Common_ISubscribe_T
{
 public:
  virtual void subscribe (InterfaceType*) = 0;
  virtual void unsubscribe (InterfaceType*) = 0;
};

#endif
