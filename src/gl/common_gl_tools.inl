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

#include "ace/Assert.h"
#include "ace/Log_Msg.h"

#include "common_macros.h"

template <typename ValueType>
std::enable_if_t<std::is_integral<ValueType>::value, ValueType>
Common_GL_Tools::map (ValueType value_in,
                      ValueType fromBegin_in,
                      ValueType fromEnd_in,
                      ValueType toBegin_in,
                      ValueType toEnd_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Tools::map"));

  // sanity check(s)
  ACE_ASSERT (fromBegin_in < fromEnd_in);
  if (value_in < fromBegin_in) value_in = fromBegin_in;
  if (value_in > fromEnd_in) value_in = fromEnd_in;

  float temp = (value_in - fromBegin_in) *
               (static_cast<float> (toEnd_in - toBegin_in) /
                static_cast<float> (fromEnd_in - fromBegin_in));
  return static_cast<ValueType> (temp) + toBegin_in;
}

template <typename ValueType>
std::enable_if_t<!std::is_integral<ValueType>::value, ValueType>
Common_GL_Tools::map (ValueType value_in,
                      ValueType fromBegin_in,
                      ValueType fromEnd_in,
                      ValueType toBegin_in,
                      ValueType toEnd_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Tools::map"));

  // sanity check(s)
  ACE_ASSERT (fromBegin_in < fromEnd_in);
  if (value_in < fromBegin_in) value_in = fromBegin_in;
  if (value_in > fromEnd_in) value_in = fromEnd_in;

  return (value_in - fromBegin_in) * ((toEnd_in - toBegin_in) / (fromEnd_in - fromBegin_in)) + toBegin_in;
}
