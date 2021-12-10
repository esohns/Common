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

#include "ace/Basic_Types.h"
#include "ace/Log_Msg.h"
#include "ace/OS.h"

#include "common_macros.h"

template <typename ValueType>
void
Common_Tools::min (uint8_t numberOfBytes_in,
                   bool isSigned_in,
                   std::enable_if_t<std::is_integral<ValueType>::value, ValueType&> value_out)
{
  COMMON_TRACE (ACE_TEXT ("Common_Tools::min"));

  switch (numberOfBytes_in)
  {
    case 1:
      value_out =
        static_cast<ValueType> (isSigned_in ? std::numeric_limits<int8_t>::min ()
                                            : std::numeric_limits<uint8_t>::min ());
      break;
    case 2:
      value_out =
        static_cast<ValueType> (isSigned_in ? std::numeric_limits<int16_t>::min ()
                                            : std::numeric_limits<uint16_t>::min ());
      break;
    case 4:
      value_out =
        static_cast<ValueType> (isSigned_in ? std::numeric_limits<int32_t>::min ()
                                            : std::numeric_limits<uint32_t>::min ());
      break;
    case 8:
      value_out =
        static_cast<ValueType> (isSigned_in ? std::numeric_limits<int64_t>::min ()
                                            : std::numeric_limits<uint64_t>::min ());
      break;
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown value size (was: %u), continuing\n"),
                  numberOfBytes_in));
      value_out =
        static_cast<ValueType> (isSigned_in ? std::numeric_limits<int64_t>::min ()
                                            : std::numeric_limits<uint64_t>::min ());
      break;
    }
  } // end SWITCH
}

template <typename ValueType>
void
Common_Tools::min (uint8_t numberOfBytes_in,
                   bool isSigned_in,
                   std::enable_if_t<!std::is_integral<ValueType>::value, ValueType&> value_out)
{
  COMMON_TRACE (ACE_TEXT ("Common_Tools::min"));

  // sanity check(s)
  ACE_ASSERT (ACE_SIZEOF_FLOAT == 4);
  ACE_ASSERT (ACE_SIZEOF_DOUBLE == 8);
  //ACE_ASSERT (ACE_SIZEOF_LONG_DOUBLE == 16); // *TODO*: 8 on Win32
  ACE_ASSERT (!isSigned_in);

  switch (numberOfBytes_in)
  {
    case 4:
      value_out =
        static_cast<ValueType> (std::numeric_limits<float>::min ());
      break;
    case 8:
      value_out =
        static_cast<ValueType> (std::numeric_limits<double>::min ());
      break;
    case 16:
      value_out =
        static_cast<ValueType> (std::numeric_limits<long double>::min ());
      break;
    default:
    {
      //ACE_DEBUG ((LM_ERROR,
      //            ACE_TEXT ("invalid/unknown value size (was: %u), continuing\n"),
      //            numberOfBytes_in));
      value_out =
        static_cast<ValueType> (-std::numeric_limits<long double>::infinity ());
      break;
    }
  } // end SWITCH
}

template <typename ValueType>
void
Common_Tools::max (uint8_t numberOfBytes_in,
                   bool isSigned_in,
                   std::enable_if_t<std::is_integral<ValueType>::value, ValueType&> value_out)
{
  COMMON_TRACE (ACE_TEXT ("Common_Tools::max"));

  switch (numberOfBytes_in)
  {
    case 1:
      value_out =
        static_cast<ValueType> (isSigned_in ? std::numeric_limits<int8_t>::max ()
                                            : std::numeric_limits<uint8_t>::max ());
      break;
    case 2:
      value_out =
        static_cast<ValueType> (isSigned_in ? std::numeric_limits<int16_t>::max ()
                                            : std::numeric_limits<uint16_t>::max ());
      break;
    case 4:
      value_out =
        static_cast<ValueType> (isSigned_in ? std::numeric_limits<int32_t>::max ()
                                            : std::numeric_limits<uint32_t>::max ());
      break;
    case 8:
      value_out =
        static_cast<ValueType> (isSigned_in ? std::numeric_limits<int64_t>::max ()
                                            : std::numeric_limits<uint64_t>::max ());
      break;
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown value size (was: %u), continuing\n"),
                  numberOfBytes_in));
      value_out =
        static_cast<ValueType> (isSigned_in ? std::numeric_limits<int64_t>::max ()
                                            : std::numeric_limits<uint64_t>::max ());
      break;
    }
  } // end SWITCH
}

template <typename ValueType>
void
Common_Tools::max (uint8_t numberOfBytes_in,
                   bool isSigned_in,
                   std::enable_if_t<!std::is_integral<ValueType>::value, ValueType&> value_out)
{
  COMMON_TRACE (ACE_TEXT ("Common_Tools::max"));

  // sanity check(s)
  ACE_ASSERT (ACE_SIZEOF_FLOAT == 4);
  ACE_ASSERT (ACE_SIZEOF_DOUBLE == 8);
  //ACE_ASSERT (ACE_SIZEOF_LONG_DOUBLE == 16); // *TODO*: 8 on Win32
  ACE_ASSERT (!isSigned_in);

  switch (numberOfBytes_in)
  {
    case 4:
      value_out =
        static_cast<ValueType> (std::numeric_limits<float>::max ());
      break;
    case 8:
      value_out =
        static_cast<ValueType> (std::numeric_limits<double>::max ());
      break;
    case 16:
      value_out =
        static_cast<ValueType> (std::numeric_limits<long double>::max ());
      break;
    default:
    {
      //ACE_DEBUG ((LM_ERROR,
      //            ACE_TEXT ("invalid/unknown value size (was: %u), continuing\n"),
      //            numberOfBytes_in));
      value_out =
        static_cast<ValueType> (std::numeric_limits<long double>::infinity ());
      break;
    }
  } // end SWITCH
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
      return value_in;
    case 2:
      return static_cast<ValueType> (ACE_SWAP_WORD (*(uint16_t*)&value_in));
    case 4:
      return static_cast<ValueType> (ACE_SWAP_LONG (*(uint32_t*)&value_in));
    case 8:
      return static_cast<ValueType> (ACE_SWAP_LONG_LONG (*(uint64_t*)&value_in));
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
