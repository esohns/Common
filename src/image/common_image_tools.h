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

#ifndef COMMON_IMAGE_TOOLS_H
#define COMMON_IMAGE_TOOLS_H

#include <string>

#ifdef __cplusplus
extern "C"
{
#include "libavutil/pixfmt.h"
}
#endif /* __cplusplus */

#include "ace/Global_Macros.h"

//#include "common_image_exports.h"

class Common_Image_Tools
{
 public:
//  inline virtual ~Common_Image_Tools () {}

  // *TODO*: currently supports AV_PIX_FMT_YUV420P only
  static bool storeToFile (unsigned int,        // source width
                           unsigned int,        // source height
                           enum AVPixelFormat,  // source pixel format
                           uint8_t*[],          // source buffer(s)
                           const std::string&); // target file path
  // *TODO*: currently supports AV_PIX_FMT_RGB24 only
  static bool storeToPNG (unsigned int,        // source width
                          unsigned int,        // source height
                          enum AVPixelFormat,  // source pixel format
                          uint8_t*[],          // source buffer(s)
                          const std::string&); // target file path

 private:
  ACE_UNIMPLEMENTED_FUNC (Common_Image_Tools ());
  ACE_UNIMPLEMENTED_FUNC (~Common_Image_Tools ());
  ACE_UNIMPLEMENTED_FUNC (Common_Image_Tools (const Common_Image_Tools&));
  ACE_UNIMPLEMENTED_FUNC (Common_Image_Tools& operator= (const Common_Image_Tools&));
};

#endif
