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

#include <limits>
#include <typeinfo>

#include "ace/Assert.h"
#include "ace/Basic_Types.h"
#include "ace/Log_Msg.h"
#include "ace/OS.h"

#include "common_macros.h"

template <typename ValueType>
std::enable_if_t<std::is_integral<ValueType>::value, ValueType>
Common_Tools::min (unsigned int numberOfBytes_in,
                   bool isSigned_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Tools::min"));

  switch (numberOfBytes_in)
  {
    case 1:
      return static_cast<ValueType> (isSigned_in ? std::numeric_limits<int8_t>::min ()
                                                 : std::numeric_limits<uint8_t>::min ());
    case 2:
      return static_cast<ValueType> (isSigned_in ? std::numeric_limits<int16_t>::min ()
                                                 : std::numeric_limits<uint16_t>::min ());
    case 4:
      return static_cast<ValueType> (isSigned_in ? std::numeric_limits<int32_t>::min ()
                                                 : std::numeric_limits<uint32_t>::min ());
    case 8:
      return static_cast<ValueType> (isSigned_in ? std::numeric_limits<int64_t>::min ()
                                                 : std::numeric_limits<uint64_t>::min ());
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown value size (was: %u), continuing\n"),
                  numberOfBytes_in));
      break;
    }
  } // end SWITCH

  return static_cast<ValueType> (isSigned_in ? std::numeric_limits<int64_t>::min ()
                                             : std::numeric_limits<uint64_t>::min ());
}

template <typename ValueType>
std::enable_if_t<std::is_integral<ValueType>::value, ValueType>
Common_Tools::max (unsigned int numberOfBytes_in,
                   bool isSigned_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Tools::max"));

  switch (numberOfBytes_in)
  {
    case 1:
      return static_cast<ValueType> (isSigned_in ? std::numeric_limits<int8_t>::max ()
                                                 : std::numeric_limits<uint8_t>::max ());
    case 2:
      return static_cast<ValueType> (isSigned_in ? std::numeric_limits<int16_t>::max ()
                                                 : std::numeric_limits<uint16_t>::max ());
    case 4:
      return static_cast<ValueType> (isSigned_in ? std::numeric_limits<int32_t>::max ()
                                                 : std::numeric_limits<uint32_t>::max ());
    case 8:
      return static_cast<ValueType> (isSigned_in ? std::numeric_limits<int64_t>::max ()
                                                 : std::numeric_limits<uint64_t>::max ());
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown value size (was: %u), continuing\n"),
                  numberOfBytes_in));
      break;
    }
  } // end SWITCH

  return static_cast<ValueType> (isSigned_in ? std::numeric_limits<int64_t>::max ()
                                             : std::numeric_limits<uint64_t>::max ());
}

template <typename ValueType>
std::enable_if_t<!std::is_integral<ValueType>::value, ValueType>
Common_Tools::min (unsigned int numberOfBytes_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Tools::min"));

  switch (numberOfBytes_in)
  {
    case 4:
    { ACE_ASSERT (ACE_SIZEOF_FLOAT == 4);
      return static_cast<ValueType> (std::numeric_limits<float>::min ());
    }
    case 8:
    { ACE_ASSERT (ACE_SIZEOF_DOUBLE == 8);
      return static_cast<ValueType> (std::numeric_limits<double>::min ());
    }
    case 16:
    { ACE_ASSERT (ACE_SIZEOF_LONG_DOUBLE == 16); // *TODO*: 8 on Win32
      return static_cast<ValueType> (std::numeric_limits<long double>::min ());
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown value size (was: %u), continuing\n"),
                  numberOfBytes_in));
      break;
    }
  } // end SWITCH

  return static_cast<ValueType> (-std::numeric_limits<long double>::infinity ());
}

template <typename ValueType>
std::enable_if_t<!std::is_integral<ValueType>::value, ValueType>
Common_Tools::max (unsigned int numberOfBytes_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Tools::max"));

  switch (numberOfBytes_in)
  {
    case 4:
    { ACE_ASSERT (ACE_SIZEOF_FLOAT == 4);
      return static_cast<ValueType> (std::numeric_limits<float>::max ());
    }
    case 8:
    { ACE_ASSERT (ACE_SIZEOF_DOUBLE == 8);
      return static_cast<ValueType> (std::numeric_limits<double>::max ());
    }
    case 16:
    { ACE_ASSERT (ACE_SIZEOF_LONG_DOUBLE == 16); // *TODO*: 8 on Win32
      return static_cast<ValueType> (std::numeric_limits<long double>::max ());
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown value size (was: %u), continuing\n"),
                  numberOfBytes_in));
      break;
    }
  } // end SWITCH

  return static_cast<ValueType> (std::numeric_limits<long double>::infinity ());
}

template <typename ValueType>
ValueType
Common_Tools::byteSwap (ValueType value_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Tools::byteSwap"));

  // *TODO*: preserve signedness ?
  switch (sizeof (ValueType))
  {
    case 1:
    { ACE_ASSERT (false);
      return value_in;
    }
    case 2:
      return static_cast<ValueType> (ACE_SWAP_WORD (*reinterpret_cast<uint16_t*> (&value_in)));
    case 4:
      return static_cast<ValueType> (ACE_SWAP_LONG (*reinterpret_cast<uint32_t*> (&value_in)));
    case 8:
      return static_cast<ValueType> (ACE_SWAP_LONG_LONG (*reinterpret_cast<uint64_t*> (&value_in)));
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown value size (was: %u), aborting\n"),
                  sizeof (ValueType)));
      break;
    }
  } // end SWITCH
  ACE_ASSERT (false);
  return 0;
}

template <typename Type,
          typename Type_2>
bool
Common_Tools::equalType (Type* pointer_in,
                         Type_2* pointer_2_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Tools::equalType"));

  // sanity check(s)
  ACE_ASSERT (pointer_in);
  ACE_ASSERT (pointer_2_in);

  std::type_info type_info_s = typeid (pointer_in);
  std::type_info type_info_2 = typeid (pointer_2_in);

  return !ACE_OS::strcmp (type_info_s.name,
                          type_info_2.name);
}

template <typename ValueType>
std::enable_if_t<std::is_integral<ValueType>::value, ValueType>
Common_Tools::getRandomNumber (ValueType begin_in,
                               ValueType end_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Tools::getRandomNumber"));

  // sanity check(s)
  ACE_ASSERT (begin_in <= end_in);

  if ((end_in - begin_in + 1) <= (RAND_MAX + 1)) // can rand() generate this range ?
    return (static_cast<ValueType> (ACE_OS::rand () % (end_in - begin_in + 1)) + begin_in);

////std::function<unsigned int ()> generator =
////    std::bind (distribution, engine);

//return generator ();
  ACE_ASSERT (std::is_integral<ValueType>::value);
  std::uniform_int_distribution<ValueType> distribution (begin_in, end_in);
  return Common_Tools::getRandomNumber (distribution);
}

template <typename ValueType>
std::enable_if_t<!std::is_integral<ValueType>::value, ValueType>
Common_Tools::getRandomNumber (ValueType begin_in,
                               ValueType end_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Tools::getRandomNumber"));

  // sanity check(s)
  ACE_ASSERT (begin_in <= end_in);

  std::uniform_real_distribution<ValueType> distribution (begin_in, end_in);
  return distribution (Common_Tools::randomEngine);
}
