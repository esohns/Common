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

#ifndef COMMON_IMAGE_COMMON_H
#define COMMON_IMAGE_COMMON_H

#include <list>

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include "windef.h"
#endif // ACE_WIN32 || ACE_WIN64

enum Common_Image_FileType
{
  COMMON_IMAGE_FILE_BMP = 0,
  COMMON_IMAGE_FILE_GIF,
  COMMON_IMAGE_FILE_JPG,
  COMMON_IMAGE_FILE_PNG,
  ///////////////////////////////////////
  COMMON_IMAGE_FILE_INVALID,
  COMMON_IMAGE_FILE_MAX
};

#if defined (ACE_WIN32) || defined (ACE_WIN64)
typedef struct tagSIZE Common_Image_Resolution_t;
struct common_image_resolution_less
{
  inline bool operator() (const Common_Image_Resolution_t& lhs_in, const Common_Image_Resolution_t& rhs_in) const { return ((lhs_in.cx == rhs_in.cx) ? (lhs_in.cy < rhs_in.cy) : (lhs_in.cx < rhs_in.cx)); }
};
struct common_image_resolution_equal
{
  inline bool operator() (const Common_Image_Resolution_t& lhs_in, const Common_Image_Resolution_t& rhs_in) const { return ((lhs_in.cx == rhs_in.cx) && (lhs_in.cy == rhs_in.cy)); }
};
#else
struct Common_Image_Resolution
{
  Common_Image_Resolution ()
   : width (0)
   , height (0)
  {}
  Common_Image_Resolution (unsigned int width_in,
                           unsigned int height_in)
   : width (width_in)
   , height (height_in)
  {}

  unsigned int width;
  unsigned int height;
};
typedef struct Common_Image_Resolution Common_Image_Resolution_t;
struct common_image_resolution_less
{
  inline bool operator() (const Common_Image_Resolution_t& lhs_in, const Common_Image_Resolution_t& rhs_in) const { return ((lhs_in.width == rhs_in.width) ? (lhs_in.height < rhs_in.height) : (lhs_in.width < rhs_in.width)); }
};
struct common_image_resolution_equal
{
  inline bool operator() (const Common_Image_Resolution_t& lhs_in, const Common_Image_Resolution_t& rhs_in) const { return ((lhs_in.width == rhs_in.width) && (lhs_in.height == rhs_in.height)); }
};
#endif // ACE_WIN32 || ACE_WIN64
typedef std::list<Common_Image_Resolution_t> Common_Image_Resolutions_t;
typedef Common_Image_Resolutions_t::iterator Common_Image_ResolutionsIterator_t;
typedef Common_Image_Resolutions_t::const_iterator Common_Image_ResolutionsConstIterator_t;

#endif
