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
#include "d3d9.h"
#endif // ACE_WIN32 || ACE_WIN64

#if defined (FFMPEG_SUPPORT)
#ifdef __cplusplus
extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavutil/imgutils.h"
}
#endif /* __cplusplus */
#endif // FFMPEG_SUPPORT

#include "ace/Global_Macros.h"

#include "common_image_common.h"

// forward declarations
#if defined (ACE_WIN32) || defined (ACE_WIN64)
enum _D3DXIMAGE_FILEFORMAT;
#endif // ACE_WIN32 || ACE_WIN64
#if defined (FFMPEG_SUPPORT)
struct SwsContext;
#endif // FFMPEG_SUPPORT
#if defined (IMAGEMAGICK_SUPPORT)
struct _MagickWand;
#endif // IMAGEMAGICK_SUPPORT

#if defined (FFMPEG_SUPPORT)
typedef std::list<enum AVPixelFormat> Common_Image_FFMPEGPixelFormats_t;
typedef Common_Image_FFMPEGPixelFormats_t::const_iterator Common_Image_FFMPEGPixelFormatsIterator_t;
struct Common_Image_Tools_GetFormatCBData
{
  Common_Image_FFMPEGPixelFormats_t formats;
};

enum AVPixelFormat
common_image_tools_get_format_cb (struct AVCodecContext*,
                                  const enum AVPixelFormat*);
#endif // FFMPEG_SUPPORT

class Common_Image_Tools
{
 public:
  static enum Common_Image_FileType fileExtensionToType (const std::string&); // path

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  // DirectX
  static bool save (const std::string&,         // target file path
                    enum _D3DXIMAGE_FILEFORMAT, // file format
                    const IDirect3DSurface9*);  // data
#if defined (FFMPEG_SUPPORT)
  static bool saveBMP (const Common_Image_Resolution_t&, // source resolution
                       enum AVPixelFormat,               // source pixel format
                       uint8_t*[],                       // source buffer(s)
                       const std::string&);              // target file path
#endif // FFMPEG_SUPPORT
#endif // ACE_WIN32 || ACE_WIN64

  // color conversion
  static void RGBToHSV (float,   // r - normalized
                        float,   // g - normalized
                        float,   // b - normalized
                        float&,  // return value: h - degrees
                        float&,  // return value: s - normalized
                        float&); // return value: v - normalized
  static void HSVToRGB (float,     // h - degrees
                        float,     // s - normalized
                        float,     // v - normalized
                        float&,  // return value: r - normalized
                        float&,  // return value: g - normalized
                        float&); // return value: b - normalized

  // --- libav/ffmpeg ---
#if defined (FFMPEG_SUPPORT)
  // *TODO*: currently supports AV_PIX_FMT_YUV420P only
  static bool save (const Common_Image_Resolution_t&, // source resolution
                    enum AVPixelFormat,               // source pixel format
                    uint8_t*[],                       // source buffer(s)
                    const std::string&);              // target file path
  // *TODO*: currently supports AV_PIX_FMT_RGB24 input only
  static bool savePNG (const Common_Image_Resolution_t&, // source resolution
                       enum AVPixelFormat,               // source pixel format
                       uint8_t*[],                       // source buffer(s)
                       const std::string&);              // target file path

  // *NOTE*: callers need to delete[] the returned memory buffer(s) (iff any)
  static bool load (uint8_t*[],                 // source buffer(s)
                    unsigned int,               // source buffer(s) size
                    enum AVCodecID,             // source format {AV_CODEC_ID_NONE: deduce}
                    enum AVPixelFormat,         // target pixel format
                    Common_Image_Resolution_t&, // return value: resolution
                    uint8_t*[]);                // return value: buffer(s)
  static bool load (const std::string&,         // source file path
                    Common_Image_Resolution_t&, // return value: resolution
                    enum AVPixelFormat&,        // return value: pixel format
                    uint8_t*[]);                // return value: buffer(s)
  static bool load (const std::string&,         // source file path
                    enum AVCodecID,             // source format {AV_CODEC_ID_NONE: deduce}
                    Common_Image_Resolution_t&, // return value: resolution
                    enum AVPixelFormat&,        // return value: pixel format
                    uint8_t*[]);                // return value: buffer(s)
  static bool save (const Common_Image_Resolution_t&, // source resolution
                    enum AVPixelFormat,               // source pixel format
                    uint8_t*[],                       // source buffer(s)
                    enum AVCodecID,                   // target format
                    const std::string&);              // target file path

  // *NOTE*: callers need to delete[] the returned memory buffer(s) (iff any)
  static bool decode (uint8_t*[],                 // source buffer(s)
                      unsigned int,               // source buffer(s) size
                      enum AVCodecID,             // source format {AV_CODEC_ID_NONE: deduce}
                      enum AVPixelFormat,         // target pixel format
                      Common_Image_Resolution_t&, // return value: target resolution
                      uint8_t*[]);                // return value: target buffer(s)
  static bool convert (const Common_Image_Resolution_t&, // source resolution
                       enum AVPixelFormat,               // source pixel format
                       uint8_t*[],                       // source buffer(s)
                       const Common_Image_Resolution_t&, // target resolution
                       enum AVPixelFormat,               // target pixel format
                       uint8_t*[]);                      // return value: target buffer(s)
  static bool scale (const Common_Image_Resolution_t&, // source resolution
                     enum AVPixelFormat,               // source pixel format
                     uint8_t*[],                       // source buffer(s)
                     const Common_Image_Resolution_t&, // target resolution
                     uint8_t*[]);                      // return value: target buffer(s)

  static std::string pixelFormatToString (enum AVPixelFormat);
  static enum AVCodecID stringToCodecId (const std::string&);
  static std::string codecIdToString (enum AVCodecID);

  static std::string errorToString (int); // error code
#endif // FFMPEG_SUPPORT

  // --- ImageMagick ---
#if defined (IMAGEMAGICK_SUPPORT)
  // *NOTE*: callers need to MagickRelinquishMemory() the returned memory buffer (iff any)
  static bool load (const std::string&,         // source file path
                    const std::string&,         // blob output format (e.g. "RGB", "RGBA", etc)
                    Common_Image_Resolution_t&, // return value: resolution
                    uint8_t*&);                 // return value: buffer

  static std::string errorToString (struct _MagickWand*); // context
#endif // IMAGEMAGICK_SUPPORT

 private:
#if defined (FFMPEG_SUPPORT)
   // *NOTE*: callers need to delete[] the returned memory buffer(s) (iff any)
  static bool convert (struct SwsContext*,               // context ? : use sws_getCachedContext()
                       const Common_Image_Resolution_t&, // source resolution
                       enum AVPixelFormat,               // source pixel format
                       uint8_t*[],                       // source buffer(s)
                       const Common_Image_Resolution_t&, // target resolution
                       enum AVPixelFormat,               // target pixel format
                       uint8_t*[]);                      // return value: target buffer(s)
  static bool scale (struct SwsContext*,               // context ? : use sws_getCachedContext()
                     const Common_Image_Resolution_t&, // source resolution
                     enum AVPixelFormat,               // source pixel format
                     uint8_t*[],                       // source buffer(s)
                     const Common_Image_Resolution_t&, // target resolution
                     uint8_t*[]);                      // return value: target buffer(s)
#endif // FFMPEG_SUPPORT

  ACE_UNIMPLEMENTED_FUNC (Common_Image_Tools ())
  ACE_UNIMPLEMENTED_FUNC (~Common_Image_Tools ())
  ACE_UNIMPLEMENTED_FUNC (Common_Image_Tools (const Common_Image_Tools&))
  ACE_UNIMPLEMENTED_FUNC (Common_Image_Tools& operator= (const Common_Image_Tools&))
};

#endif
