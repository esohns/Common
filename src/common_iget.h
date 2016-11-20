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

#ifndef COMMON_IGET_H
#define COMMON_IGET_H

template <typename Type>
class Common_IGet_T
{
 public:
  virtual const Type& get () const = 0; // return value: type
};

template <typename Type>
class Common_ISet_T
{
 public:
  virtual void set (const Type&) = 0;
};

template <typename Type>
class Common_IGetSet_T
 : public Common_IGet_T<Type>
 , public Common_ISet_T<Type>
{};

//////////////////////////////////////////

template <typename Type>
class Common_ISetP_T
{
 public:
  virtual void set (Type*) = 0;
};

template <typename Type>
class Common_IGetSetP_T
 : public Common_ISetP_T<Type>
{
 public:
  virtual const Type* const get () const = 0; // return value: type handle
};

//////////////////////////////////////////

template <typename Type>
class Common_ISetPP_T
{
 public:
  // *IMPORTANT NOTE*: fire-and-forget API
  virtual void set (Type*&) = 0;
};

template <typename Type>
class Common_IGetSetPP_T
 : public Common_ISetPP_T<Type>
{
 public:
  virtual const Type* const get () const = 0; // return value: type handle
};

#endif
