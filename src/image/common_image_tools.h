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
#include <list>
#include <string>

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#define __CGUID_H__ // *NOTE*: avoid compilation issue
#include <d3d9.h>
#include <d3dx9tex.h>
#endif // ACE_WIN32 || ACE_WIN64

#ifdef __cplusplus
extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavutil/pixdesc.h"
#include "libavutil/pixfmt.h"
}
#endif /* __cplusplus */

#include "ace/Global_Macros.h"

#include "common_image_common.h"

// forward declarations
struct _MagickWand;

typedef std::list<enum AVPixelFormat> Common_Image_FFMPEGPixelFormats_t;
typedef Common_Image_FFMPEGPixelFormats_t::const_iterator Common_Image_FFMPEGPixelFormatsIterator_t;
struct Common_Image_Tools_GetFormatCBData
{
  Common_Image_FFMPEGPixelFormats_t formats;
};

enum AVPixelFormat
common_image_tools_get_format_cb (struct AVCodecContext*,
                                  const enum AVPixelFormat*);

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
  static bool save (const Common_Image_Resolution_t&, // source resolution
                    enum AVPixelFormat,               // source pixel format
                    const uint8_t*[],                 // source buffer(s)
                    const std::string&);              // target file path
  // *TODO*: currently supports AV_PIX_FMT_RGB24 input only
  static bool savePNG (const Common_Image_Resolution_t&, // source resolution
                       enum AVPixelFormat,               // source pixel format
                       const uint8_t*,                   // source buffer(s)
                       const std::string&);              // target file path

  // *NOTE*: callers need to delete[] the returned memory buffer(s) (iff any)
  static bool load (const uint8_t*,             // source buffer(s)
                    unsigned int,               // source buffer(s) size
                    enum AVCodecID,             // source format {AV_CODEC_ID_NONE: deduce}
                    enum AVPixelFormat,         // target pixel format
                    Common_Image_Resolution_t&, // return value: target resolution
                    uint8_t*&);                 // return value: target buffer(s)
  static bool load (const std::string&,         // source file path
                    enum AVCodecID,             // source format {AV_CODEC_ID_NONE: deduce}
                    Common_Image_Resolution_t&, // return value: target resolution
                    enum AVPixelFormat&,        // return value: target pixel format
                    uint8_t*&);                 // return value: target buffer(s)
  static bool save (const Common_Image_Resolution_t&, // source resolution
                    enum AVPixelFormat,               // source pixel format
                    const uint8_t*,                   // source buffer(s)
                    enum AVCodecID,                   // target format
                    const std::string&);              // target file path

  // *NOTE*: callers need to delete[] the returned memory buffer(s) (iff any)
  static bool decode (const uint8_t*,             // source buffer(s)
                      unsigned int,               // source buffer(s) size
                      enum AVCodecID,             // source format {AV_CODEC_ID_NONE: deduce}
                      enum AVPixelFormat,         // target pixel format
                      Common_Image_Resolution_t&, // return value: target resolution
                      uint8_t*&);                 // return value: target buffer(s)
  static bool convert (const Common_Image_Resolution_t&, // source resolution
                       enum AVPixelFormat,               // source pixel format
                       const uint8_t*,                   // source buffer(s)
                       const Common_Image_Resolution_t&, // target resolution
                       enum AVPixelFormat,               // target pixel format
                       uint8_t*&);                       // return value: target buffer(s)
  static bool scale (const Common_Image_Resolution_t&, // source resolution
                     enum AVPixelFormat,               // source pixel format
                     const uint8_t*,                   // source buffer(s)
                     const Common_Image_Resolution_t&, // target resolution
                     uint8_t*&);                       // return value: target buffer(s)

  inline static std::string pixelFormatToString (enum AVPixelFormat format_in) { std::string result = ((format_in == AV_PIX_FMT_NONE) ? ACE_TEXT_ALWAYS_CHAR ("") : av_get_pix_fmt_name (format_in)); return result; }

  static std::string errorToString (int); // error code

  // --- ImageMagick ---
  static enum AVCodecID stringToCodecId (const std::string&);
  static std::string errorToString (struct _MagickWand*); // context

 private:
  // *NOTE*: callers need to delete[] the returned memory buffer(s) (iff any)
  static bool convert (struct SwsContext*,               // context ? : use sws_getCachedContext()
                       const Common_Image_Resolution_t&, // source resolution
                       enum AVPixelFormat,               // source pixel format
                       uint8_t*[],                       // source buffer(s)
                       const Common_Image_Resolution_t&, // target resolution
                       enum AVPixelFormat,               // target pixel format
                       uint8_t*&);                       // return value: target buffer(s)
  static bool scale (struct SwsContext*,               // context ? : use sws_getCachedContext()
                     const Common_Image_Resolution_t&, // source resolution
                     enum AVPixelFormat,               // source pixel format
                     uint8_t*[],                       // source buffer(s)
                     const Common_Image_Resolution_t&, // target resolution
                     uint8_t*&);                       // return value: target buffer(s)

  ACE_UNIMPLEMENTED_FUNC (Common_Image_Tools ())
  ACE_UNIMPLEMENTED_FUNC (~Common_Image_Tools ())
  ACE_UNIMPLEMENTED_FUNC (Common_Image_Tools (const Common_Image_Tools&))
  ACE_UNIMPLEMENTED_FUNC (Common_Image_Tools& operator= (const Common_Image_Tools&))
};

#endif
