﻿/***************************************************************************
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
  virtual const Type get () const = 0; // return value: type
};

// *IMPORTANT NOTE*: use this to avoid C2555 on MSVC
// *TODO*: remove ASAP
template <typename Type>
class Common_IGet_2_T
{
 public:
  virtual const Type get_2 () const = 0; // return value: type handle
};

template <typename Type>
class Common_IGet_3_T
{
 public:
  virtual const Type get_3 () const = 0; // return value: type handle
};

template <typename Type>
class Common_IGet_4_T
{
 public:
  virtual const Type get_4 () const = 0; // return value: type handle
};

template <typename Type>
class Common_ISet_T
{
 public:
  virtual void set (const Type) = 0;
};

template <typename Type>
class Common_IGetSet_T
 : public Common_IGet_T<Type>
 , public Common_ISet_T<Type>
{};

//////////////////////////////////////////

template <typename Type>
class Common_IGetR_T
{
 public:
  virtual const Type& getR () const = 0; // return value: type
};

// *IMPORTANT NOTE*: use this to avoid C2555 on MSVC
// *TODO*: remove ASAP
template <typename Type>
class Common_IGetR_2_T
{
 public:
  virtual const Type& getR_2 () const = 0; // return value: type
};
// *IMPORTANT NOTE*: use this to avoid C2555 on MSVC
// *TODO*: remove ASAP
template <typename Type>
class Common_IGetR_3_T
{
 public:
  virtual const Type& getR_3 () const = 0; // return value: type
};
// *IMPORTANT NOTE*: use this to avoid C2555 on MSVC
// *TODO*: remove ASAP
template <typename Type>
class Common_IGetR_4_T
{
 public:
  virtual const Type& getR_4 () const = 0; // return value: type
};
// *IMPORTANT NOTE*: use this to avoid C2555 on MSVC
// *TODO*: remove ASAP
template <typename Type>
class Common_IGetR_5_T
{
 public:
  virtual const Type& getR_5 () const = 0; // return value: type
};

template <typename Type>
class Common_ISetR_T
{
 public:
  virtual void setR (const Type&) = 0;
};

template <typename Type>
class Common_IGetSetR_T
 : public Common_IGetR_T<Type>
 , public Common_ISetR_T<Type>
{};

//////////////////////////////////////////

template <typename Type>
class Common_IGetP_T
{
 public:
  virtual const Type* const getP () const = 0; // return value: type handle
};
// *IMPORTANT NOTE*: use this to avoid C2555 on MSVC
// *TODO*: remove ASAP
template <typename Type>
class Common_IGetP_2_T
{
 public:
  virtual const Type* const getP_2 () const = 0; // return value: type handle
};

template <typename Type>
class Common_ISetP_T
{
 public:
  virtual void setP (Type*) = 0;
};
// *IMPORTANT NOTE*: use this to avoid C2555 on MSVC
// *TODO*: remove ASAP
template <typename Type>
class Common_ISetP_2_T
{
 public:
  virtual void setP_2 (Type*) = 0;
};

template <typename Type>
class Common_IGetSetP_T
 : public Common_IGetP_T<Type>
 , public Common_ISetP_T<Type>
{};

//////////////////////////////////////////

template <typename Type>
class Common_ISetPR_T
{
 public:
  // *IMPORTANT NOTE*: fire-and-forget API
  virtual void setPR (Type*&) = 0;
};
template <typename Type>
class Common_ISetPR_2_T
{
 public:
  // *IMPORTANT NOTE*: fire-and-forget API
  virtual void setPR_2 (Type*&) = 0;
};

template <typename Type>
class Common_IGetSetPR_T
 : public Common_IGetP_T<Type>
 , public Common_ISetPR_T<Type>
{};

//----------------------------------------

template <typename Type,
          typename ReturnType>
class Common_IGet1RR_T
{
 public:
  virtual const ReturnType& get1RR (const Type&) const = 0; // return value: type
};

template <typename Type,
          typename ReturnType>
class Common_IGet1RR_2_T
{
 public:
  virtual const ReturnType& get1RR_2 (const Type&) const = 0; // return value: type
};

//----------------------------------------


template <typename Type1,
          typename Type2>
class Common_ISet2R_T
{
 public:
  virtual void set2R (const Type1&,
                      const Type2&) = 0;
};

template <typename Type1,
          typename Type2,
          typename Type3>
class Common_ISet3R_T
{
 public:
  virtual void set3R (const Type1&,
                      const Type2&,
                      const Type3&) = 0;
};

#endif
