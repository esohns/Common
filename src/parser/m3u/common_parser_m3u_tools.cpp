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
#include "stdafx.h"

#include "common_parser_m3u_tools.h"

#include <sstream>

#include "ace/Log_Msg.h"

#include "common_macros.h"

std::string
Common_Parser_M3U_Tools::ElementToString (const struct M3U_ExtInf_Element& element_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Parser_M3U_Tools::ElementToString"));

  std::string result;

  result += element_in.Artist;
  result += ACE_TEXT_ALWAYS_CHAR (" - ");
  result += element_in.Album;
  result += ACE_TEXT_ALWAYS_CHAR (" - ");
  result += element_in.Title;
  result += ACE_TEXT_ALWAYS_CHAR (" : ");
  std::ostringstream converter;
  converter << element_in.Length;
  result += converter.str ();
  result += ACE_TEXT_ALWAYS_CHAR (" --> ");
  result += element_in.URL;
  result += ACE_TEXT_ALWAYS_CHAR ("\n");

  return result;
}

std::string
Common_Parser_M3U_Tools::ElementToString (const struct M3U_Media_Element& element_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Parser_M3U_Tools::ElementToString"));

  std::string result;

  return result;
}

std::string
Common_Parser_M3U_Tools::ElementToString (const struct M3U_StreamInf_Element& element_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Parser_M3U_Tools::ElementToString"));

  std::string result;

  return result;
}

std::string
Common_Parser_M3U_Tools::PlaylistToString (const struct M3U_Playlist& playlist_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Parser_M3U_Tools::PlaylistToString"));

  std::string result;

  for (M3U_ExtInf_ElementsIterator_t iterator = playlist_in.ext_inf_elements.begin ();
       iterator != playlist_in.ext_inf_elements.end ();
       ++iterator)
    result += Common_Parser_M3U_Tools::ElementToString (*iterator);

  return result;
}
