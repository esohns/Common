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

#include "ace/Basic_Types.h"
#include "ace/Log_Msg.h"

#include "common_macros.h"

template <typename ValueType>
ValueType
Common_Tools::byteswap (ValueType value_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Tools::byteswap"));

  switch (sizeof (ValueType))
  {
    case 1:
      return value_in;
    case 2:
      return static_cast<ValueType> (ACE_SWAP_WORD (value_in));
    case 4:
      return static_cast<ValueType> (ACE_SWAP_LONG (value_in));
    case 8:
      return static_cast<ValueType> (ACE_SWAP_LONG_LONG (value_in));
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown value size (was: %u), aborting\n"),
                  sizeof (ValueType)));
      return 0;
    }
  } // end SWITCH
  ACE_ASSERT (false);
  ACE_NOTREACHED (return 0;)
}

template <typename ValueType>
ValueType
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
  if (std::is_integral<ValueType>::value)
  {
    std::uniform_int_distribution<ValueType> distribution (begin_in, end_in);
    return distribution (Common_Tools::randomEngine);
  } // end IF

  std::uniform_real_distribution<ValueType> distribution (begin_in, end_in);
  return distribution (Common_Tools::randomEngine);
}
