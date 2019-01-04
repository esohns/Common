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

#include <cstdint>
#include <string>

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include <d3d9.h>
#include <d3dx9tex.h>
#endif // ACE_WIN32 || ACE_WIN64

#ifdef __cplusplus
extern "C"
{
#include "libavutil/pixdesc.h"
#include "libavutil/pixfmt.h"
}
#endif /* __cplusplus */

#include "ace/Global_Macros.h"

class Common_Image_Tools
{
 public:
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  static bool save (const std::string&,         // target file path
                    enum _D3DXIMAGE_FILEFORMAT, // file format
                    const IDirect3DSurface9*);  // data
#endif // ACE_WIN32 || ACE_WIN64

  // --- libav/ffmpeg ---
  // *TODO*: currently supports AV_PIX_FMT_YUV420P only
  static bool save (unsigned int,        // source width
                    unsigned int,        // source height
                    enum AVPixelFormat,  // source pixel format
                    uint8_t*[],          // source buffer(s)
                    const std::string&); // target file path
  // *TODO*: currently supports AV_PIX_FMT_RGB24 input only
  static bool savePNG (unsigned int,        // source width
                       unsigned int,        // source height
                       enum AVPixelFormat,  // source pixel format
                       uint8_t*[],          // source buffer(s)
                       const std::string&); // target file path

  // *NOTE*: callers need to delete[] the returned memory buffer(s) (iff any)
  static bool convert (struct SwsContext*, // context ? : use sws_getCachedContext()
                       unsigned int,       // source width
                       unsigned int,       // source height
                       enum AVPixelFormat, // source pixel format
                       uint8_t*[],         // source buffer(s)
                       unsigned int,       // target width
                       unsigned int,       // target height
                       enum AVPixelFormat, // target pixel format
                       uint8_t*&);         // return value: target buffer(s)
  static bool scale (struct SwsContext*, // context ? : use sws_getCachedContext()
                     unsigned int,       // source width
                     unsigned int,       // source height
                     enum AVPixelFormat, // source pixel format
                     uint8_t*[],         // source buffer(s)
                     unsigned int,       // target width
                     unsigned int,       // target height
                     uint8_t*&);         // return value: target buffer(s)

  inline static std::string pixelFormatToString (enum AVPixelFormat format_in) { std::string result = ((format_in == AV_PIX_FMT_NONE) ? ACE_TEXT_ALWAYS_CHAR ("") : av_get_pix_fmt_name (format_in)); return result; }

  static std::string errorToString (int); // error code

 private:
  ACE_UNIMPLEMENTED_FUNC (Common_Image_Tools ())
  ACE_UNIMPLEMENTED_FUNC (~Common_Image_Tools ())
  ACE_UNIMPLEMENTED_FUNC (Common_Image_Tools (const Common_Image_Tools&))
  ACE_UNIMPLEMENTED_FUNC (Common_Image_Tools& operator= (const Common_Image_Tools&))
};

#endif
