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

#ifndef COMMON_IINITIALIZE_H
#define COMMON_IINITIALIZE_H

template <typename Type>
class Common_SInitializeFinalize_T
{
 public:
  // convenient types
  typedef Common_SInitializeFinalize_T<Type> INITIALIZE_T;

  inline static bool initialize () { return Type::initialize (); }
  inline static bool finalize () { return Type::finalize (); }
};

//////////////////////////////////////////

class Common_IInitialize
{
 public:
  virtual bool initialize () = 0;
};

class Common_IInitializeP
{
 public:
  virtual bool initialize (const void*) = 0;
};

template <typename ConfigurationType>
class Common_IInitialize_T
{
 public:
  virtual bool initialize (const ConfigurationType&) = 0;
};

//template <typename ConfigurationType>
//class Common_IInitialize_2
//{
// public:
//  virtual bool initialize_2 (const ConfigurationType&) = 0;
//};

template <typename ConfigurationType>
class Common_IInitializeP_T
{
 public:
  virtual bool initialize (const ConfigurationType*) = 0;
};

//////////////////////////////////////////

class Common_IReset
{
 public:
  virtual void reset () = 0;
};

#endif
