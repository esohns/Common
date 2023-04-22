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

#include "common_image_tools.h"

#include <sstream>

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include "d3dx9tex.h"
#endif // ACE_WIN32 || ACE_WIN64

#if defined (FFMPEG_SUPPORT)
#ifdef __cplusplus
extern "C"
{
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
}
#endif /* __cplusplus */
#endif // FFMPEG_SUPPORT

#if defined (IMAGEMAGICK_SUPPORT)
#include "MagickWand/MagickWand.h"
#endif // IMAGEMAGICK_SUPPORT

#include "ace/Log_Msg.h"
#include "ace/OS.h"

#include "common_file_tools.h"
#include "common_macros.h"
#include "common_string_tools.h"

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include "common_error_tools.h"
#endif // ACE_WIN32 || ACE_WIN64

#if defined (FFMPEG_SUPPORT)
enum AVPixelFormat
common_image_tools_get_format_cb (struct AVCodecContext* context_in,
                                  const enum AVPixelFormat* formats_in)
{
  // sanity check(s)
  struct Common_Image_Tools_GetFormatCBData* cb_data_p =
      static_cast<struct Common_Image_Tools_GetFormatCBData*> (context_in->opaque);
  ACE_ASSERT (cb_data_p);
  ACE_ASSERT (formats_in);

  // *NOTE*: this prefers the highest quality over the relevance of the format
  Common_Image_FFMPEGPixelFormatsIterator_t iterator =
      cb_data_p->formats.end ();
  for (const enum AVPixelFormat* iterator_2 = formats_in;
       *iterator_2 != -1;
       ++iterator_2)
    for (iterator = cb_data_p->formats.begin ();
         iterator != cb_data_p->formats.end ();
         ++iterator)
      if (*iterator == *iterator_2)
        break;

  return (iterator != cb_data_p->formats.end () ? *iterator : AV_PIX_FMT_NONE);
}
#endif // FFMPEG_SUPPORT

enum Common_Image_FileType
Common_Image_Tools::fileExtensionToType (const std::string& path_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Image_Tools::fileExtensionToType"));

  std::string file_extension_string =
    Common_File_Tools::fileExtension (path_in,
                                      false);
  file_extension_string =
    Common_String_Tools::toupper (file_extension_string);
  
  if (!ACE_OS::strcmp (file_extension_string.c_str (),
                       ACE_TEXT_ALWAYS_CHAR ("BMP")))
    return COMMON_IMAGE_FILE_BMP;
  else if (!ACE_OS::strcmp (file_extension_string.c_str (),
                            ACE_TEXT_ALWAYS_CHAR ("GIF")))
    return COMMON_IMAGE_FILE_GIF;
  else if (!ACE_OS::strcmp (file_extension_string.c_str (),
                            ACE_TEXT_ALWAYS_CHAR ("JPG")))
    return COMMON_IMAGE_FILE_JPG;
  else if (!ACE_OS::strcmp (file_extension_string.c_str (),
                            ACE_TEXT_ALWAYS_CHAR ("PNG")))
    return COMMON_IMAGE_FILE_PNG;
  else
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("invalid/unknown file extension (was: \"%s\"), aborting\n"),
                ACE_TEXT (file_extension_string.c_str ())));

  return COMMON_IMAGE_FILE_INVALID;
}

#if defined (ACE_WIN32) || defined (ACE_WIN64)
bool
Common_Image_Tools::save (const std::string& path_in,
                          enum _D3DXIMAGE_FILEFORMAT format_in,
                          const IDirect3DSurface9* surface_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Image_Tools::save"));

  // sanity check(s)
  ACE_ASSERT ((format_in != D3DXIFF_PPM) && (format_in != D3DXIFF_TGA));
  ACE_ASSERT (surface_in);

  HRESULT result =
    D3DXSaveSurfaceToFile (            // filename
#if defined (UNICODE)
                           ACE_TEXT_ALWAYS_WCHAR (path_in.c_str ()),
#else
                           ACE_TEXT_ALWAYS_CHAR (path_in.c_str ()),
#endif // UNICODE
                           format_in,  // file format
                           const_cast<IDirect3DSurface9*> (surface_in), // surface
                           NULL,       // palette
                           NULL);      // rectangle
  if (unlikely (FAILED (result)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to D3DXSaveSurfaceToFile(): \"%s\", aborting\n"),
                ACE_TEXT (Common_Error_Tools::errorToString (result).c_str ())));
    return false;
  } // end IF

  return true;
}

#if defined (FFMPEG_SUPPORT)
bool
Common_Image_Tools::saveBMP (const Common_Image_Resolution_t& resolution_in,
                             enum AVPixelFormat format_in,
                             uint8_t* sourceBuffers_in[],
                             const std::string& path_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Image_Tools::saveBMP"));

  // sanity check(s)
  ACE_ASSERT (format_in == AV_PIX_FMT_RGB24);

  FILE* file_p = ACE_OS::fopen (path_in.c_str (),
                                ACE_TEXT_ALWAYS_CHAR ("wb"));
  if (unlikely (!file_p))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::fopen(\"%s\",wb): \"%m\", aborting\n"),
                ACE_TEXT (path_in.c_str())));
    return false;
  } // end IF

  BITMAPINFOHEADER bitmap_info_header_s;
  ACE_OS::memset (&bitmap_info_header_s, 0, sizeof (BITMAPINFOHEADER));
  bitmap_info_header_s.biSize = sizeof (BITMAPINFOHEADER);
  bitmap_info_header_s.biWidth = resolution_in.cx;
  bitmap_info_header_s.biHeight =
    (resolution_in.cy > 0 ? -resolution_in.cy : resolution_in.cy);
  bitmap_info_header_s.biPlanes = 1;
  bitmap_info_header_s.biBitCount = 24;
  bitmap_info_header_s.biCompression = BI_RGB;
  bitmap_info_header_s.biSizeImage = resolution_in.cx * resolution_in.cy * 3;

  BITMAPFILEHEADER bitmap_file_header_s;
  ACE_OS::memset (&bitmap_file_header_s, 0, sizeof (BITMAPFILEHEADER));
  bitmap_file_header_s.bfType = 'B' + ('M' << 8);
  bitmap_file_header_s.bfSize =
    sizeof (BITMAPFILEHEADER) + sizeof (BITMAPINFOHEADER) + bitmap_info_header_s.biSizeImage;
  bitmap_file_header_s.bfOffBits =
    sizeof (BITMAPFILEHEADER) + sizeof (BITMAPINFOHEADER);

  // write the bitmap file header
  size_t bytes_written = ACE_OS::fwrite (&bitmap_file_header_s,
                                         sizeof (BITMAPFILEHEADER),
                                         1,
                                         file_p);

  // ...and then the bitmap info header
  bytes_written += ACE_OS::fwrite (&bitmap_info_header_s,
                                   sizeof (BITMAPINFOHEADER),
                                   1,
                                   file_p);

  // finally, write the image data itself
  bytes_written += ACE_OS::fwrite (sourceBuffers_in[0],
                                   bitmap_info_header_s.biSizeImage,
                                   1,
                                   file_p);

  ACE_OS::fclose (file_p);

  return true;
}
#endif // FFMPEG_SUPPORT
#endif // ACE_WIN32 || ACE_WIN64

void
Common_Image_Tools::RGBToHSV (const float& red_in,
                              const float& green_in,
                              const float& blue_in,
                              float& hue_out,
                              float& saturation_out,
                              float& value_out)
{
  COMMON_TRACE (ACE_TEXT ("Common_Image_Tools::RGBToHSV"));

  float min_f, max_f, delta_f;

  min_f = red_in < green_in ? red_in : green_in;
  min_f = min_f  < blue_in  ? min_f  : blue_in;
  max_f = red_in > green_in ? red_in : green_in;
  max_f = max_f  > blue_in  ? max_f  : blue_in;

  value_out = max_f;                              // v
  delta_f = max_f - min_f;
  if (delta_f < 0.00001F)
  {
    saturation_out = 0.0F;
    hue_out = 0.0F; // undefined, maybe nan?
    return;
  } // end IF

  ACE_ASSERT (max_f >= 0.0F);
  if (max_f > 0.0F) // NOTE: if Max is == 0, this divide would cause a crash
    saturation_out = (delta_f / max_f);           // s
  else
  {
    // if max is 0, then r = g = b = 0
    // s = 0, h is undefined
    saturation_out = 0.0F;
    hue_out = NAN; // its now undefined
    return;
  } // end ELSE

  if (red_in >= max_f) // > is bogus, just keeps compilor happy
    hue_out = (green_in - blue_in) / delta_f;      // between yellow & magenta
  else if (green_in >= max_f)
    hue_out = 2.0F + (blue_in - red_in) / delta_f;  // between cyan & yellow
  else
    hue_out = 4.0F + (red_in - green_in) / delta_f; // between magenta & cyan
  hue_out *= 60.0F; // degrees
  if (hue_out < 0.0F)
    hue_out += 360.0F;
}

#if defined (FFMPEG_SUPPORT)
bool
Common_Image_Tools::save (const Common_Image_Resolution_t& resolution_in,
                          enum AVPixelFormat format_in,
                          uint8_t* sourceBuffers_in[],
                          const std::string& path_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Image_Tools::save"));

  bool result = false;

  int result_2 = -1;
  size_t result_3 = 0;
  FILE* file_p = NULL;
  int linesizes[AV_NUM_DATA_POINTERS];
  result_2 = av_image_fill_linesizes (linesizes,
                                      format_in,
#if defined (ACE_WIN32) || defined (ACE_WIN64)
                                      resolution_in.cx);
#else
                                      resolution_in.width);
#endif // ACE_WIN32 || ACE_WIN64
  ACE_ASSERT (result_2 != -1);

  file_p = ACE_OS::fopen (path_in.c_str (),
                          ACE_TEXT_ALWAYS_CHAR ("wb"));
  if (!file_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::fopen(\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (path_in.c_str ())));
    goto clean;
  } // end IF

  switch (format_in)
  {
    case AV_PIX_FMT_YUV420P:
    {
      // write Y plane
      for (unsigned int i = 0;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
           i < static_cast<unsigned int> (resolution_in.cy);
#else
           i < resolution_in.height;
#endif // ACE_WIN32 || ACE_WIN64
           ++i)
      {
        result_3 = ACE_OS::fwrite ((sourceBuffers_in[0] + (i * linesizes[0])),
#if defined (ACE_WIN32) || defined (ACE_WIN64)
                                   resolution_in.cx,
#else
                                   resolution_in.width,
#endif // ACE_WIN32 || ACE_WIN64
                                   1,
                                   file_p);
        if (result_3 != 1)
        {
#if defined (ACE_WIN32) || defined (ACE_WIN64)
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to ACE_OS::fwrite(%u) (file was: \"%s\"): \"%m\", aborting\n"),
                      resolution_in.cx,
                      ACE_TEXT (path_in.c_str ())));
#else
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to ACE_OS::fwrite(%u) (file was: \"%s\"): \"%m\", aborting\n"),
                      resolution_in.width,
                      ACE_TEXT (path_in.c_str ())));
#endif // ACE_WIN32 || ACE_WIN64
          goto clean;
        } // end IF
      } // end FOR
      // write U plane
#if defined (ACE_WIN32) || defined (ACE_WIN64)
      const_cast<Common_Image_Resolution_t&> (resolution_in).cy >>= 1;
      const_cast<Common_Image_Resolution_t&> (resolution_in).cx >>= 1;
#else
      const_cast<Common_Image_Resolution_t&> (resolution_in).height >>= 1;
      const_cast<Common_Image_Resolution_t&> (resolution_in).width >>= 1;
#endif // ACE_WIN32 || ACE_WIN64
      for (unsigned int i = 0;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
           i < static_cast<unsigned int> (resolution_in.cy);
#else
           i < resolution_in.height;
#endif // ACE_WIN32 || ACE_WIN64
           ++i)
      {
        result_3 = ACE_OS::fwrite ((sourceBuffers_in[1] + (i * linesizes[1])),
#if defined (ACE_WIN32) || defined (ACE_WIN64)
                                   resolution_in.cx,
#else
                                   resolution_in.width,
#endif // ACE_WIN32 || ACE_WIN64
                                   1,
                                   file_p);
        if (result_3 != 1)
        {
#if defined (ACE_WIN32) || defined (ACE_WIN64)
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to ACE_OS::fwrite(%u) (file was: \"%s\"): \"%m\", aborting\n"),
                      resolution_in.cx,
                      ACE_TEXT (path_in.c_str ())));
#else
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to ACE_OS::fwrite(%u) (file was: \"%s\"): \"%m\", aborting\n"),
                      resolution_in.width,
                      ACE_TEXT (path_in.c_str ())));
#endif // ACE_WIN32 || ACE_WIN64
          goto clean;
        } // end IF
      } // end FOR
      // write V plane
      for (unsigned int i = 0;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
           i < static_cast<unsigned int> (resolution_in.cy);
#else
           i < resolution_in.height;
#endif // ACE_WIN32 || ACE_WIN64
           ++i)
      {
        result_3 = ACE_OS::fwrite ((sourceBuffers_in[2] + (i * linesizes[2])),
#if defined (ACE_WIN32) || defined (ACE_WIN64)
                                   resolution_in.cx,
#else
                                   resolution_in.width,
#endif // ACE_WIN32 || ACE_WIN64
                                   1,
                                   file_p);
        if (result_3 != 1)
        {
#if defined (ACE_WIN32) || defined (ACE_WIN64)
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to ACE_OS::fwrite(%u) (file was: \"%s\"): \"%m\", aborting\n"),
                      resolution_in.cx,
                      ACE_TEXT (path_in.c_str ())));
#else
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to ACE_OS::fwrite(%u) (file was: \"%s\"): \"%m\", aborting\n"),
                      resolution_in.width,
                      ACE_TEXT (path_in.c_str ())));
#endif // ACE_WIN32 || ACE_WIN64
          goto clean;
        } // end IF
      } // end FOR

      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown pixel format (was: %d), aborting\n"),
                  format_in));
      goto clean;
    }
  } // end SWITCH

  result = true;

clean:
  // clean up
  if (file_p)
  {
    result_2 = ACE_OS::fclose (file_p);
    if (result_2 == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_OS::fclose(\"%s\"): \"%m\", continuing\n"),
                  ACE_TEXT (path_in.c_str ())));
  } // end IF

#if defined (_DEBUG)
  if (result)
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("wrote file \"%s\"\n"),
                ACE_TEXT (path_in.c_str ())));
#endif

  return result;
}

bool
Common_Image_Tools::savePNG (const Common_Image_Resolution_t& resolution_in,
                             enum AVPixelFormat format_in,
                             uint8_t* sourceBuffers_in[],
                             const std::string& path_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Image_Tools::savePNG"));

  bool result = false;
  enum AVPixelFormat format_e = format_in;
  bool delete_buffer_b = false;
  uint8_t** buffer_p = sourceBuffers_in;

  if (unlikely (format_in != AV_PIX_FMT_RGB24))// &&
//               (format_in != AV_PIX_FMT_RGB32))
  {
    uint8_t** data_p = NULL;
    if (!Common_Image_Tools::convert (resolution_in,
                                      format_in,
                                      sourceBuffers_in,
                                      resolution_in,
                                      AV_PIX_FMT_RGB24,
                                      data_p))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Common_Image_Tools::convert(), aborting\n")));
      return false;
    } // end IF
    ACE_ASSERT (data_p);
    format_e = AV_PIX_FMT_RGB24;
    buffer_p = data_p;
    delete_buffer_b = true;
  } // end IF

  if (!Common_Image_Tools::save (resolution_in,
                                 format_e,
                                 buffer_p,
                                 AV_CODEC_ID_PNG,
                                 path_in))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_Image_Tools::save(), aborting\n")));
    goto error;
  } // end IF

  result = true;

error:
  if (delete_buffer_b)
  {
    delete [] buffer_p; buffer_p = NULL;
  } // end IF

  return result;
}

bool
Common_Image_Tools::load (uint8_t* sourceBuffers_in[],
                          unsigned int sourceBuffersSize_in,
                          enum AVCodecID codecId_in,
                          enum AVPixelFormat format_in,
                          Common_Image_Resolution_t& resolution_out,
                          uint8_t* targetBuffers_out[])
{
  COMMON_TRACE (ACE_TEXT ("Common_Image_Tools::load"));

  // sanity check(s)
  ACE_ASSERT (codecId_in != AV_CODEC_ID_NONE); // *TODO*
  ACE_ASSERT (targetBuffers_out);
  ACE_ASSERT (!targetBuffers_out[0]);

//  enum AVPixelFormat pixel_format_e;
  if (!Common_Image_Tools::decode (sourceBuffers_in,
                                   sourceBuffersSize_in,
                                   codecId_in,
                                   format_in,
                                   resolution_out,
                                   targetBuffers_out))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_Image_Tools::decode(%d), aborting\n"),
                codecId_in));
    return false;
  } // end IF
  ACE_ASSERT (targetBuffers_out[0]);

//  if (pixel_format_e != format_in)
//  {
//    uint8_t* buffer_p = targetBuffers_out;
//    targetBuffers_out = NULL;
//    if (!Common_Image_Tools::convert (resolution_out, pixel_format_e,
//                                      buffer_p,
//                                      resolution_out, format_in,
//                                      targetBuffers_out))
//    {
//      ACE_DEBUG ((LM_ERROR,
//                  ACE_TEXT ("failed to Common_Image_Tools::convert(), returning\n")));
//      delete [] buffer_p; buffer_p = NULL;
//      return false;
//    } // end IF
//    delete [] buffer_p; buffer_p = NULL;
//    ACE_ASSERT (targetBuffers_out);
//  } // end IF

  return true;
}

bool
Common_Image_Tools::load (const std::string& path_in,
                          Common_Image_Resolution_t& resolution_out,
                          enum AVPixelFormat& format_out,
                          uint8_t* buffers_out[])
{
  COMMON_TRACE (ACE_TEXT ("Common_Image_Tools::load"));

  int result = -1;
  int index_i = -1;
  enum AVCodecID coded_id_e = AV_CODEC_ID_NONE;

  AVFormatContext* format_context_p = avformat_alloc_context ();
  if (unlikely (!format_context_p))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to avformat_alloc_context(): \"%m\", aborting\n")));
    return false;
  } // end IF

  result = avformat_open_input (&format_context_p,
                                path_in.c_str (),
                                NULL,
                                NULL);
  if (unlikely (result < 0))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to avformat_open_input(\"%s\"): \"%s\", aborting\n"),
                ACE_TEXT (path_in.c_str ()),
                ACE_TEXT (Common_Image_Tools::errorToString (result).c_str ())));
    goto error;
  } // end IF

  result = avformat_find_stream_info (format_context_p,
                                      NULL);
  if (unlikely (result < 0))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to avformat_find_stream_info(): \"%s\", aborting\n"),
                ACE_TEXT (Common_Image_Tools::errorToString (result).c_str ())));
    goto error;
  } // end IF
#if defined (_DEBUG)
  av_dump_format (format_context_p, 0, path_in.c_str (), 0);
#endif // _DEBUG

  index_i = av_find_best_stream (format_context_p,
                                 AVMEDIA_TYPE_VIDEO,
                                 -1,
                                 -1,
                                 NULL,
                                 0);
  if (unlikely (index_i < 0))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to av_find_best_stream(AVMEDIA_TYPE_VIDEO): \"%s\", aborting\n"),
                ACE_TEXT (Common_Image_Tools::errorToString (index_i).c_str ())));
    result = -1;
    goto error;
  } // end IF
  coded_id_e = format_context_p->streams[index_i]->codecpar->codec_id;

error:
  if (likely (format_context_p))
    avformat_close_input (&format_context_p);
  avformat_free_context (format_context_p); format_context_p = NULL;

  return ((result == 0) ? Common_Image_Tools::load (path_in,
                                                    coded_id_e,
                                                    resolution_out,
                                                    format_out,
                                                    buffers_out)
                        : false);
}

bool
Common_Image_Tools::load (const std::string& path_in,
                          enum AVCodecID codecId_in,
                          Common_Image_Resolution_t& resolution_out,
                          enum AVPixelFormat& format_out,
                          uint8_t* buffers_out[])
{
  COMMON_TRACE (ACE_TEXT ("Common_Image_Tools::load"));

  // sanity check(s)
  if (codecId_in == AV_CODEC_ID_NONE)
    return Common_Image_Tools::load (path_in,
                                     resolution_out,
                                     format_out,
                                     buffers_out);

  bool result = false;
  int result_2 = -1;
  const struct AVCodec* codec_p = NULL;
  struct AVCodecContext* codec_context_p = NULL;
  struct AVFrame* frame_p = NULL;
  struct AVPacket packet_s;
  av_init_packet (&packet_s);
  ACE_UINT64 file_size_i = 0;
  Common_Image_Tools_GetFormatCBData cb_data_s;
  cb_data_s.formats.push_back (AV_PIX_FMT_RGB24);

  if (!Common_File_Tools::load (path_in,
                                packet_s.data,
                                file_size_i,
                                0))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_File_Tools::load(\"%s\"), aborting\n"),
                ACE_TEXT (path_in.c_str ())));
    goto error;
  } // end IF
  packet_s.size = static_cast<int> (file_size_i);

  frame_p = av_frame_alloc ();
  if (!frame_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to av_frame_alloc(): \"%m\", aborting\n")));
    goto error;
  } // end IF

  codec_p = avcodec_find_decoder (codecId_in);
  if (unlikely (!codec_p))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to avcodec_find_decoder(%s [%d]): \"%m\", aborting\n"),
                ACE_TEXT (avcodec_get_name (codecId_in)), codecId_in));
    goto error;
  } // end IF
  // *NOTE*: fire-and-forget codec_p
  codec_context_p = avcodec_alloc_context3 (codec_p);
  if (unlikely (!codec_context_p))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to avcodec_alloc_context3(): \"%m\", aborting\n")));
    goto error;
  } // end IF

  codec_context_p->codec_type = AVMEDIA_TYPE_VIDEO;
//  codec_context_p->flags = 0;
//  codec_context_p->flags2 = 0;
  codec_context_p->opaque = &cb_data_s;
  codec_context_p->get_format = common_image_tools_get_format_cb;
  //codec_context_p->refcounted_frames = 1; // *NOTE*: caller 'owns' the frame buffer
  codec_context_p->workaround_bugs = 0xFFFFFFFF;
  codec_context_p->strict_std_compliance = FF_COMPLIANCE_UNOFFICIAL;
  codec_context_p->error_concealment = 0xFFFFFFFF;
  // *IMPORTANT NOTE*: apparently, this fixes some weird usecases
  codec_context_p->thread_count = 1;
#if defined (_DEBUG)
  codec_context_p->debug = 0xFFFFFFFF;
#endif // _DEBUG
//  codec_context_p->err_recognition = 0xFFFFFFFF;
//  hwaccel_context
//  idct_algo
//  bits_per_coded_sample
//  thread_count
//  thread_type
//  pkt_timebase
//  hw_frames_ctx
//  hw_device_ctx
//  hwaccel_flags

  result_2 = avcodec_open2 (codec_context_p,
                            codec_p,
                            NULL);
  if (unlikely (result_2 < 0))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to avcodec_open2(): \"%s\", aborting\n"),
                ACE_TEXT (Common_Image_Tools::errorToString (result_2).c_str ())));
    goto error;
  } // end IF

  result_2 = avcodec_send_packet (codec_context_p,
                                  &packet_s);
  if (unlikely (result_2))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to avcodec_send_packet(): \"%s\", aborting\n"),
                ACE_TEXT (Common_Image_Tools::errorToString (result_2).c_str ())));
    goto error;
  } // end IF
  result_2 = avcodec_receive_frame (codec_context_p,
                                    frame_p);
  if (unlikely (result_2))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to avcodec_receive_frame(): \"%s\", aborting\n"),
                ACE_TEXT (Common_Image_Tools::errorToString (result_2).c_str ())));
    goto error;
  } // end IF
  ACE_ASSERT (frame_p);
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("loaded image \"%s\" (%ux%u %s)\n"),
              ACE_TEXT (path_in.c_str ()),
              frame_p->width, frame_p->height,
              ACE_TEXT (Common_Image_Tools::pixelFormatToString (static_cast<enum AVPixelFormat> (frame_p->format)).c_str ())));

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  resolution_out.cx = frame_p->width;
  resolution_out.cy = frame_p->height;
#else
  resolution_out.width = frame_p->width;
  resolution_out.height = frame_p->height;
#endif // ACE_WIN32 || ACE_WIN64
  format_out = static_cast<enum AVPixelFormat> (frame_p->format);
  // *NOTE*: do not av_frame_unref the frame, keep the data
  ACE_OS::memcpy (buffers_out, frame_p->data, sizeof (uint8_t*[AV_NUM_DATA_POINTERS]));
  ACE_OS::memset (frame_p->data, 0, sizeof (uint8_t*[AV_NUM_DATA_POINTERS]));
  av_frame_unref (frame_p);

  result = true;

error:
  av_packet_unref (&packet_s);
  if (likely (frame_p))
    av_frame_free (&frame_p);
  if (likely (codec_context_p))
  {
    avcodec_close (codec_context_p); codec_context_p = NULL;
    av_free (codec_context_p); codec_context_p = NULL;
  } // end IF

  return result;
}

bool
Common_Image_Tools::save (const Common_Image_Resolution_t& sourceResolution_in,
                          enum AVPixelFormat sourcePixelFormat_in,
                          uint8_t* sourceBuffers_in[],
                          enum AVCodecID codecId_in,
                          const std::string& targetPath_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Image_Tools::save"));

  bool result = false;
  int result_2 = -1;
  const struct AVCodec* codec_p = NULL;
  struct AVCodecContext* codec_context_p = NULL;
  struct AVPacket* packet_p = NULL;
  struct AVFrame* frame_p = av_frame_alloc ();
  if (!frame_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to av_frame_alloc(): \"%m\", aborting\n")));
    goto error;
  } // end IF
  frame_p->format = sourcePixelFormat_in;

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  frame_p->height = sourceResolution_in.cy;
  frame_p->width = sourceResolution_in.cx;
#else
  frame_p->height = sourceResolution_in.height;
  frame_p->width = sourceResolution_in.width;
#endif // ACE_WIN32 || ACE_WIN64
  result_2 = av_image_fill_linesizes (frame_p->linesize,
                                      sourcePixelFormat_in,
#if defined (ACE_WIN32) || defined (ACE_WIN64)
                                      static_cast<int> (sourceResolution_in.cx));
#else
                                      static_cast<int> (sourceResolution_in.width));
#endif // ACE_WIN32 || ACE_WIN64
  ACE_ASSERT (result_2 >= 0);
  result_2 =
      av_image_fill_pointers (frame_p->data,
                              sourcePixelFormat_in,
#if defined (ACE_WIN32) || defined (ACE_WIN64)
                              static_cast<int> (sourceResolution_in.cy),
#else
                              static_cast<int> (sourceResolution_in.height),
#endif // ACE_WIN32 || ACE_WIN64
                              sourceBuffers_in[0], // *TODO*: this is probably wrong !!!
                              frame_p->linesize);
  ACE_ASSERT (result_2 >= 0);
  ACE_ASSERT (frame_p->data[0] == sourceBuffers_in[0]);
  ACE_ASSERT (frame_p->data[1] == sourceBuffers_in[1]);
  ACE_ASSERT (frame_p->data[2] == sourceBuffers_in[2]);
  ACE_ASSERT (frame_p->data[3] == sourceBuffers_in[3]);

  codec_p = avcodec_find_encoder (codecId_in);
  if (!codec_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to avcodec_find_encoder(%d): \"%m\", aborting\n"),
                codecId_in));
    goto error;
  } // end IF
  // *NOTE*: fire-and-forget codec_p
  codec_context_p = avcodec_alloc_context3 (codec_p);
  if (!codec_context_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to avcodec_alloc_context3(): \"%m\", aborting\n")));
    goto error;
  } // end IF

  codec_context_p->codec_type = AVMEDIA_TYPE_VIDEO;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  codec_context_p->height = sourceResolution_in.cy;
  codec_context_p->width = sourceResolution_in.cx;
#else
  codec_context_p->height = sourceResolution_in.height;
  codec_context_p->width = sourceResolution_in.width;
#endif // ACE_WIN32 || ACE_WIN64
  codec_context_p->pix_fmt = sourcePixelFormat_in;
  codec_context_p->time_base.num = codec_context_p->time_base.den = 1;
  result_2 = avcodec_open2 (codec_context_p,
                            codec_p,
                            NULL);
  if (result_2 < 0)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to avcodec_open2(): \"%s\", aborting\n"),
                ACE_TEXT (Common_Image_Tools::errorToString (result_2).c_str ())));
    goto error;
  } // end IF

  result_2 = avcodec_send_frame (codec_context_p,
                                 frame_p);
  if (result_2)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to avcodec_send_frame(): \"%s\", aborting\n"),
                ACE_TEXT (Common_Image_Tools::errorToString (result_2).c_str ())));
    goto error;
  } // end IF
  ACE_OS::memset (frame_p->data, 0, sizeof (uint8_t * [8]));
  av_frame_unref (frame_p);
  result_2 = avcodec_receive_packet (codec_context_p,
                                     packet_p);
  if (result_2)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to avcodec_receive_packet(): \"%s\", aborting\n"),
                ACE_TEXT (Common_Image_Tools::errorToString (result_2).c_str ())));
    goto error;
  } // end IF
  ACE_ASSERT (packet_p);

  if (!Common_File_Tools::store (targetPath_in,
                                 packet_p->data,
                                 packet_p->size))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_File_Tools::store(\"%s\"), aborting\n"),
                ACE_TEXT (targetPath_in.c_str ())));
    goto error;
  } // end IF
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("wrote file \"%s\" (%u byte(s)\n"),
              ACE_TEXT (targetPath_in.c_str ()),
              packet_p->size));

  result = true;

error:
  av_packet_free (&packet_p);
  if (frame_p)
    av_frame_free (&frame_p);
  if (codec_context_p)
  {
    avcodec_close (codec_context_p); codec_context_p = NULL;
    av_free (codec_context_p); codec_context_p = NULL;
  } // end IF

  return result;
}

bool
Common_Image_Tools::decode (uint8_t* sourceBuffers_in[],
                            unsigned int sourceBuffersSize_in,
                            enum AVCodecID codecId_in,
                            enum AVPixelFormat format_in,
                            Common_Image_Resolution_t& resolution_out,
                            uint8_t* targetBuffers_out[])
{
  COMMON_TRACE (ACE_TEXT ("Common_Image_Tools::decode"));

  // sanity check(s)
  ACE_ASSERT (codecId_in != AV_CODEC_ID_NONE); // *TODO*

  bool result = false;
  int result_2 = -1;
  const struct AVCodec* codec_p = NULL;
  struct AVCodecContext* codec_context_p = NULL;
  Common_Image_Tools_GetFormatCBData cb_data_s;
  cb_data_s.formats.push_back (((codecId_in == AV_CODEC_ID_MJPEG) ? AV_PIX_FMT_YUVJ420P : format_in));
  struct AVFrame* frame_p = NULL;
  struct AVPacket packet_s;
  av_init_packet (&packet_s);
  packet_s.data = sourceBuffers_in[0]; // *TODO*: this is probably wrong !!!
  ACE_ASSERT (!sourceBuffers_in[1]);
  ACE_ASSERT (!sourceBuffers_in[2]);
  ACE_ASSERT (!sourceBuffers_in[3]);
  packet_s.size = sourceBuffersSize_in;

  //frame_p = av_frame_alloc ();
  //if (!frame_p)
  //{
  //  ACE_DEBUG ((LM_ERROR,
  //              ACE_TEXT ("failed to av_frame_alloc(): \"%m\", aborting\n")));
  //  goto error;
  //} // end IF

  codec_p = avcodec_find_decoder (codecId_in);
  if (!codec_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to avcodec_find_decoder(%d): \"%m\", aborting\n"),
                codecId_in));
    goto error;
  } // end IF
  // *NOTE*: fire-and-forget codec_p
  codec_context_p = avcodec_alloc_context3 (codec_p);
  if (!codec_context_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to avcodec_alloc_context3(): \"%m\", aborting\n")));
    goto error;
  } // end IF

//  codec_context_p->codec_type = AVMEDIA_TYPE_VIDEO;
//  codec_context_p->flags = 0;
//  codec_context_p->flags2 = 0;
//  codec_context_p->time_base.den = 1;
  codec_context_p->time_base.num = 1;
//  codec_context_p->width = 1;
//  codec_context_p->height = 1;
  codec_context_p->pix_fmt =
    ((codecId_in == AV_CODEC_ID_MJPEG) ? AV_PIX_FMT_YUVJ420P : AV_PIX_FMT_RGB24);
  codec_context_p->opaque = &cb_data_s;
  codec_context_p->get_format = common_image_tools_get_format_cb;
//  codec_context_p->request_sample_fmt = AV_PIX_FMT_RGB24;
//  codec_context_p->thread_count = 1;
  //codec_context_p->refcounted_frames = 1; // *NOTE*: caller 'owns' the frame buffer
  codec_context_p->workaround_bugs = 0xFFFFFFFF;
  codec_context_p->strict_std_compliance = FF_COMPLIANCE_UNOFFICIAL;
//  codec_context_p->error_concealment = 0xFFFFFFFF;
#if defined (_DEBUG)
  codec_context_p->debug = 0xFFFFFFFF;
#endif // _DEBUG
  codec_context_p->sw_pix_fmt =
      ((codecId_in == AV_CODEC_ID_MJPEG) ? AV_PIX_FMT_YUVJ420P : AV_PIX_FMT_RGB24);
//  codec_context_p->err_recognition = 0xFFFFFFFF;
//  hwaccel_context
//  idct_algo
//  bits_per_coded_sample
//  thread_count
//  thread_type
//  pkt_timebase
//  hw_frames_ctx
//  hw_device_ctx
//  hwaccel_flags

  result_2 = avcodec_open2 (codec_context_p,
                            codec_p,
                            NULL);
  if (result_2 < 0)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to avcodec_open2(): \"%s\", aborting\n"),
                ACE_TEXT (Common_Image_Tools::errorToString (result_2).c_str ())));
    goto error;
  } // end IF

  result_2 = avcodec_send_packet (codec_context_p,
                                  &packet_s);
  if (result_2)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to avcodec_send_packet(): \"%s\", aborting\n"),
                ACE_TEXT (Common_Image_Tools::errorToString (result_2).c_str ())));
    goto error;
  } // end IF
  result_2 = avcodec_receive_frame (codec_context_p,
                                    frame_p);
  if (result_2)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to avcodec_receive_frame(): \"%s\", aborting\n"),
                ACE_TEXT (Common_Image_Tools::errorToString (result_2).c_str ())));
    goto error;
  } // end IF
  ACE_ASSERT (frame_p);

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  resolution_out.cx = frame_p->width;
  resolution_out.cy = frame_p->height;
#else
  resolution_out.width = frame_p->width;
  resolution_out.height = frame_p->height;
#endif // ACE_WIN32 || ACE_WIN64
  if (format_in != static_cast<enum AVPixelFormat> (frame_p->format))
  {
    uint8_t* data_pointers_a[4];
    data_pointers_a[0] = frame_p->data[0];
    data_pointers_a[1] = frame_p->data[1];
    data_pointers_a[2] = frame_p->data[2];
    data_pointers_a[3] = frame_p->data[3];
    if (!Common_Image_Tools::convert (resolution_out,
                                      static_cast<enum AVPixelFormat> (frame_p->format),
                                      data_pointers_a,
                                      resolution_out,
                                      format_in,
                                      targetBuffers_out))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Common_Image_Tools::convert(), aborting\n")));
      goto error;
    } // end IF
  } // end IF
  else
  {
    // *NOTE*: do not av_frame_unref the frame, keep the data
    targetBuffers_out = frame_p->data;
  } // end ELSE
  ACE_OS::memset (frame_p->data, 0, sizeof (uint8_t* [8]));
  av_frame_unref (frame_p);

  result = true;

error:
  av_packet_unref (&packet_s);
  if (frame_p)
    av_frame_free (&frame_p);
  if (codec_context_p)
  {
    avcodec_close (codec_context_p); codec_context_p = NULL;
    av_free (codec_context_p); codec_context_p = NULL;
  } // end IF

  return result;
}

bool
Common_Image_Tools::convert (const Common_Image_Resolution_t& sourceResolution_in,
                             enum AVPixelFormat sourcePixelFormat_in,
                             uint8_t* sourceBuffers_in[],
                             const Common_Image_Resolution_t& targetResolution_in,
                             enum AVPixelFormat targetPixelFormat_in,
                             uint8_t* targetBuffers_out[])
{
  COMMON_TRACE (ACE_TEXT ("Common_Image_Tools::convert"));

  // initialize return value(s)
  ACE_ASSERT (!targetBuffers_out[0]);

  if (unlikely (!Common_Image_Tools::convert (NULL,
                                              sourceResolution_in, sourcePixelFormat_in,
                                              sourceBuffers_in,
                                              targetResolution_in, targetPixelFormat_in,
                                              targetBuffers_out)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_Image_Tools::convert(), returning\n")));
    return false;
  } // end IF

  return true;
}

bool
Common_Image_Tools::scale (const Common_Image_Resolution_t& sourceResolution_in,
                           enum AVPixelFormat sourcePixelFormat_in,
                           uint8_t* sourceBuffers_in[],
                           const Common_Image_Resolution_t& targetResolution_in,
                           uint8_t* targetBuffers_out[])
{
  COMMON_TRACE (ACE_TEXT ("Common_Image_Tools::scale"));

  // initialize return value(s)
  ACE_ASSERT (!targetBuffers_out);

  int line_sizes_a[AV_NUM_DATA_POINTERS];
  uint8_t* data_pointers_a[AV_NUM_DATA_POINTERS];
  ACE_OS::memset (line_sizes_a, 0, sizeof (int[AV_NUM_DATA_POINTERS]));
  ACE_OS::memset (data_pointers_a, 0, sizeof (uint8_t*[AV_NUM_DATA_POINTERS]));

  int result = av_image_fill_linesizes (line_sizes_a,
                                        sourcePixelFormat_in,
#if defined (ACE_WIN32) || defined (ACE_WIN64)
                                        static_cast<int> (sourceResolution_in.cx));
#else
                                        static_cast<int> (sourceResolution_in.width));
#endif // ACE_WIN32 || ACE_WIN64
  ACE_ASSERT (result >= 0);
  result =
      av_image_fill_pointers (data_pointers_a,
                              sourcePixelFormat_in,
#if defined (ACE_WIN32) || defined (ACE_WIN64)
                              static_cast<int> (sourceResolution_in.cy),
#else
                              static_cast<int> (sourceResolution_in.height),
#endif // ACE_WIN32 || ACE_WIN64
                              sourceBuffers_in[0], // *TODO*: this is probably wrong !!!
                              line_sizes_a);
  ACE_ASSERT (result >= 0); ACE_UNUSED_ARG (result);
  ACE_ASSERT (data_pointers_a[0] == sourceBuffers_in[0]);
  ACE_ASSERT (data_pointers_a[1] == sourceBuffers_in[1]);
  ACE_ASSERT (data_pointers_a[2] == sourceBuffers_in[2]);
  ACE_ASSERT (data_pointers_a[3] == sourceBuffers_in[3]);
  if (unlikely (!Common_Image_Tools::scale (NULL,
                                            sourceResolution_in, sourcePixelFormat_in,
                                            data_pointers_a,
                                            targetResolution_in,
                                            targetBuffers_out)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_Image_Tools::scale(), returning\n")));
    return false;
  } // end IF

  return true;
}

bool
Common_Image_Tools::convert (struct SwsContext* context_in,
                             const Common_Image_Resolution_t& sourceResolution_in,
                             enum AVPixelFormat sourcePixelFormat_in,
                             uint8_t* sourceBuffers_in[],
                             const Common_Image_Resolution_t& targetResolution_in,
                             enum AVPixelFormat targetPixelFormat_in,
                             uint8_t* targetBuffers_out[])
{
  COMMON_TRACE (ACE_TEXT ("Common_Image_Tools::convert"));

  // initialize return value(s)
  ACE_ASSERT (!targetBuffers_out[0]);

  // sanity check(s)
  if (unlikely (!sws_isSupportedInput (sourcePixelFormat_in) ||
                !sws_isSupportedOutput (targetPixelFormat_in)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("unsupported format conversion (was: %s --> %s), aborting\n"),
                ACE_TEXT (Common_Image_Tools::pixelFormatToString (sourcePixelFormat_in).c_str ()),
                ACE_TEXT (Common_Image_Tools::pixelFormatToString (targetPixelFormat_in).c_str ())));
    return false;
  } // end IF
  if (unlikely (sourcePixelFormat_in == targetPixelFormat_in))
    return Common_Image_Tools::scale (context_in,
                                      sourceResolution_in,
                                      sourcePixelFormat_in,
                                      sourceBuffers_in,
                                      targetResolution_in,
                                      targetBuffers_out);

  // *TODO*: define a balanced scaler parametrization that suits most
  //         applications, or expose this as a parameter
  int flags = (SWS_FAST_BILINEAR);
  struct SwsContext* context_p = NULL;
  int result_2 = -1;
  int in_linesize[AV_NUM_DATA_POINTERS];
  int out_linesize[AV_NUM_DATA_POINTERS];
  uint8_t* out_data[AV_NUM_DATA_POINTERS];
  ACE_OS::memset (&in_linesize, 0, sizeof (int[AV_NUM_DATA_POINTERS]));
  ACE_OS::memset (&out_linesize, 0, sizeof (int[AV_NUM_DATA_POINTERS]));
  ACE_OS::memset (&out_data, 0, sizeof (uint8_t*[AV_NUM_DATA_POINTERS]));
  int size_i =
      av_image_get_buffer_size (targetPixelFormat_in,
#if defined (ACE_WIN32) || defined (ACE_WIN64)
                                targetResolution_in.cx, targetResolution_in.cy,
#else
                                targetResolution_in.width, targetResolution_in.height,
#endif // ACE_WIN32 || ACE_WIN64
                                1); // *TODO*: linesize alignment

  ACE_NEW_NORETURN (targetBuffers_out[0],
                    uint8_t[size_i]);
  if (unlikely (!targetBuffers_out[0]))
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to allocate memory: %m, aborting\n")));
    return false;
  } // end IF

  context_p =
      (context_in ? context_in
                  : sws_getCachedContext (NULL,
#if defined (ACE_WIN32) || defined (ACE_WIN64)
                                          sourceResolution_in.cx, sourceResolution_in.cy, sourcePixelFormat_in,
                                          targetResolution_in.cx, targetResolution_in.cy, targetPixelFormat_in,
#else
                                          sourceResolution_in.width, sourceResolution_in.height, sourcePixelFormat_in,
                                          targetResolution_in.width, targetResolution_in.height, targetPixelFormat_in,
#endif // ACE_WIN32 || ACE_WIN64
                                          flags,                             // flags
                                          NULL, NULL,
                                          0));                               // parameters
  if (unlikely (!context_p))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to sws_getCachedContext(): \"%m\", aborting\n")));
    goto error;
  } // end IF

  result_2 = av_image_fill_linesizes (in_linesize,
                                      sourcePixelFormat_in,
#if defined (ACE_WIN32) || defined (ACE_WIN64)
                                      static_cast<int> (sourceResolution_in.cx));
#else
                                      static_cast<int> (sourceResolution_in.width));
#endif // ACE_WIN32 || ACE_WIN64
  ACE_ASSERT (result_2 >= 0);
  result_2 = av_image_fill_linesizes (out_linesize,
                                      targetPixelFormat_in,
#if defined (ACE_WIN32) || defined (ACE_WIN64)
                                      static_cast<int> (targetResolution_in.cx));
#else
                                      static_cast<int> (targetResolution_in.width));
#endif // ACE_WIN32 || ACE_WIN64
  ACE_ASSERT (result_2 >= 0);
  result_2 =
      av_image_fill_pointers (out_data,
                              targetPixelFormat_in,
#if defined (ACE_WIN32) || defined (ACE_WIN64)
                              targetResolution_in.cy,
#else
                              targetResolution_in.height,
#endif // ACE_WIN32 || ACE_WIN64
                              targetBuffers_out[0],
                              out_linesize);
  ACE_ASSERT (result_2 == size_i);

  result_2 = sws_scale (context_p,
                        sourceBuffers_in, in_linesize,
#if defined (ACE_WIN32) || defined (ACE_WIN64)
                        0, sourceResolution_in.cy,
#else
                        0, sourceResolution_in.height,
#endif // ACE_WIN32 || ACE_WIN64
                        out_data, out_linesize);
  if (unlikely (result_2 <= 0))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to sws_scale(): \"%s\", aborting\n"),
                ACE_TEXT (Common_Image_Tools::errorToString (result_2).c_str ())));
    goto error;
  } // end IF
  // *NOTE*: ffmpeg returns fewer than the expected number of rows in some cases
  // *TODO*: find out when and why
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  else if (unlikely (result_2 != static_cast<int> (targetResolution_in.cy)))
#else
  else if (unlikely (result_2 != static_cast<int> (targetResolution_in.height)))
#endif // ACE_WIN32 || ACE_WIN64
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    ACE_DEBUG ((LM_WARNING,
                ACE_TEXT ("sws_scale() returned: %d (expected: %u), continuing\n"),
                result_2, targetResolution_in.cy));
#else
    ACE_DEBUG ((LM_WARNING,
                ACE_TEXT ("sws_scale() returned: %d (expected: %u), continuing\n"),
                result_2, targetResolution_in.height));
#endif // ACE_WIN32 || ACE_WIN64

  if (unlikely (!context_in))
  {
    sws_freeContext (context_p); context_p = NULL;
  } // end IF

  return true;

error:
  if (targetBuffers_out)
  {
    delete [] targetBuffers_out; ACE_OS::memset (targetBuffers_out, 0, sizeof (uint8_t*[AV_NUM_DATA_POINTERS]));
  } // end IF
  if (unlikely (!context_in))
  {
    sws_freeContext (context_p); context_p = NULL;
  } // end IF

  return false;
}

bool
Common_Image_Tools::scale (struct SwsContext* context_in,
                           const Common_Image_Resolution_t& sourceResolution_in,
                           enum AVPixelFormat pixelFormat_in,
                           uint8_t* sourceBuffers_in[],
                           const Common_Image_Resolution_t& targetResolution_in,
                           uint8_t* targetBuffers_out[])
{
  COMMON_TRACE (ACE_TEXT ("Common_Image_Tools::scale"));

  // initialize return value(s)
  ACE_ASSERT (!targetBuffers_out);

  // sanity check(s)
  if (unlikely (!sws_isSupportedInput (pixelFormat_in) ||
                !sws_isSupportedOutput (pixelFormat_in)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("unsupported format conversion (was: %s --> %s), aborting\n"),
                ACE_TEXT (Common_Image_Tools::pixelFormatToString (pixelFormat_in).c_str ()),
                ACE_TEXT (Common_Image_Tools::pixelFormatToString (pixelFormat_in).c_str ())));
    return false;
  } // end IF

  // *TODO*: define a balanced scaler parametrization that suits most
  //         applications, or expose this as a parameter
  int flags = (//SWS_BILINEAR | SWS_FAST_BILINEAR | // interpolation
               SWS_BICUBIC);
  struct SwsContext* context_p = NULL;
  int result_2 = -1;
  int in_linesize[AV_NUM_DATA_POINTERS];
  int out_linesize[AV_NUM_DATA_POINTERS];
  uint8_t* out_data[AV_NUM_DATA_POINTERS];
  int size_i = av_image_get_buffer_size (pixelFormat_in,
#if defined (ACE_WIN32) || defined (ACE_WIN64)
                                         targetResolution_in.cx, targetResolution_in.cy,
#else
                                         targetResolution_in.width, targetResolution_in.height,
#endif // ACE_WIN32 || ACE_WIN64
                                         1); // *TODO*: linesize alignment

  ACE_NEW_NORETURN (targetBuffers_out[0],
                    uint8_t[size_i]);
  if (unlikely (!targetBuffers_out))
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to allocate memory: %m, aborting\n")));
    return false;
  } // end IF

  context_p =
      (context_in ? context_in
                  : sws_getCachedContext (NULL,
#if defined (ACE_WIN32) || defined (ACE_WIN64)
                                          sourceResolution_in.cx, sourceResolution_in.cy, pixelFormat_in,
                                          targetResolution_in.cx, targetResolution_in.cy, pixelFormat_in,
#else
                                          sourceResolution_in.width, sourceResolution_in.height, pixelFormat_in,
                                          targetResolution_in.width, targetResolution_in.height, pixelFormat_in,
#endif // ACE_WIN32 || ACE_WIN64
                                          flags,                             // flags
                                          NULL, NULL,
                                          0));                               // parameters
  if (unlikely (!context_p))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to sws_getCachedContext(): \"%m\", aborting\n")));
    goto error;
  } // end IF

  result_2 = av_image_fill_linesizes (in_linesize,
                                      pixelFormat_in,
#if defined (ACE_WIN32) || defined (ACE_WIN64)
                                      static_cast<int> (sourceResolution_in.cx));
#else
                                      static_cast<int> (sourceResolution_in.width));
#endif // ACE_WIN32 || ACE_WIN64
  ACE_ASSERT (result_2 >= 0);
  result_2 = av_image_fill_linesizes (out_linesize,
                                      pixelFormat_in,
#if defined (ACE_WIN32) || defined (ACE_WIN64)
                                      static_cast<int> (targetResolution_in.cx));
#else
                                      static_cast<int> (targetResolution_in.width));
#endif // ACE_WIN32 || ACE_WIN64
  ACE_ASSERT (result_2 >= 0);
  result_2 =
      av_image_fill_pointers (out_data,
                              pixelFormat_in,
#if defined (ACE_WIN32) || defined (ACE_WIN64)
                              targetResolution_in.cy,
#else
                              targetResolution_in.height,
#endif // ACE_WIN32 || ACE_WIN64
                              targetBuffers_out[0],
                              out_linesize);
  ACE_ASSERT (result_2 == size_i);

  result_2 = sws_scale (context_p,
                        sourceBuffers_in, in_linesize,
#if defined (ACE_WIN32) || defined (ACE_WIN64)
                        0, sourceResolution_in.cy,
#else
                        0, sourceResolution_in.height,
#endif // ACE_WIN32 || ACE_WIN64
                        out_data, out_linesize);
  if (unlikely (result_2 <= 0))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to sws_scale(): \"%s\", aborting\n"),
                ACE_TEXT (Common_Image_Tools::errorToString (result_2).c_str ())));
    goto error;
  } // end IF
  // *NOTE*: ffmpeg returns fewer than the expected number of rows in some cases
  // *TODO*: find out when and why
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  else if (unlikely (result_2 != static_cast<int> (targetResolution_in.cy)))
#else
  else if (unlikely (result_2 != static_cast<int> (targetResolution_in.height)))
#endif // ACE_WIN32 || ACE_WIN64
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    ACE_DEBUG ((LM_WARNING,
                ACE_TEXT ("sws_scale() returned: %d (expected: %u), continuing\n"),
                result_2, targetResolution_in.cy));
#else
    ACE_DEBUG ((LM_WARNING,
                ACE_TEXT ("sws_scale() returned: %d (expected: %u), continuing\n"),
                result_2, targetResolution_in.height));
#endif // ACE_WIN32 || ACE_WIN64

  if (unlikely (!context_in))
  {
    sws_freeContext (context_p); context_p = NULL;
  } // end IF

  return true;

error:
  if (targetBuffers_out)
  {
    delete [] targetBuffers_out; targetBuffers_out = NULL;
  } // end IF
  if (unlikely (!context_in))
  {
    sws_freeContext (context_p); context_p = NULL;
  } // end IF

  return false;
}

std::string
Common_Image_Tools::pixelFormatToString (enum AVPixelFormat format_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Image_Tools::pixelFormatToString"));

  std::string result;

  result =
    ((format_in == AV_PIX_FMT_NONE) ? ACE_TEXT_ALWAYS_CHAR ("") 
                                    : av_get_pix_fmt_name (format_in));
  
  return result;
}

std::string
Common_Image_Tools::errorToString (int errorCode_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Image_Tools::errorToString"));

  // initialize return value(s)
  std::string return_value;

  char buffer_a[AV_ERROR_MAX_STRING_SIZE];
  ACE_OS::memset (buffer_a, 0, sizeof (char[AV_ERROR_MAX_STRING_SIZE]));
  int result = av_strerror (errorCode_in,
                            buffer_a,
                            sizeof (char[AV_ERROR_MAX_STRING_SIZE]));
  if (unlikely (result))
    ACE_DEBUG ((LM_ERROR,
                ((result < 0) ? ACE_TEXT ("failed to av_strerror(%d), cannot find error description: \"%m\", continuing\n")
                              : ACE_TEXT ("failed to av_strerror(%d): \"%m\", continuing\n")),
                errorCode_in));
  return_value = buffer_a;

  return return_value;
}

enum AVCodecID
Common_Image_Tools::stringToCodecId (const std::string& format_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Image_Tools::stringToCodecId"));

  // initialize return value(s)
  enum AVCodecID return_value = AV_CODEC_ID_NONE;

  if (format_in == ACE_TEXT_ALWAYS_CHAR ("RGB"))
    return_value = AV_CODEC_ID_NONE;
  else if (format_in == ACE_TEXT_ALWAYS_CHAR ("RGBA"))
    return_value = AV_CODEC_ID_NONE;
  else if (format_in == ACE_TEXT_ALWAYS_CHAR ("PNG"))
    return_value = AV_CODEC_ID_PNG;
  else if (format_in == ACE_TEXT_ALWAYS_CHAR ("JPG"))
    return_value = AV_CODEC_ID_MJPEG;
  else if (format_in == ACE_TEXT_ALWAYS_CHAR ("XC"))
    return_value = AV_CODEC_ID_NONE;
  else
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("invalid/unknown format (was: \"%s\"), aborting\n"),
                ACE_TEXT (format_in.c_str ())));

  return return_value;
}

std::string
Common_Image_Tools::codecIdToString (enum AVCodecID codecId_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Image_Tools::codecIdToString"));

  std::string result;

  const AVCodecDescriptor* descriptor_p =
      avcodec_descriptor_get (codecId_in);
  if (unlikely (!descriptor_p))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to avcodec_descriptor_get(%d): \"%m\", aborting\n"),
                codecId_in));
    result = avcodec_get_name (codecId_in);
    return result;
  } // end IF
  result = descriptor_p->name;

  return result;
}
#endif // FFMPEG_SUPPORT

#if defined (IMAGEMAGICK_SUPPORT)
std::string
Common_Image_Tools::errorToString (struct _MagickWand* context_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Image_Tools::errorToString"));

  // initialize return value(s)
  std::string return_value;

  ExceptionType severity = UndefinedException;
  char* description_p = MagickGetException (context_in, &severity);
  ACE_ASSERT (description_p);
  return_value = description_p;
  MagickRelinquishMemory (description_p); description_p = NULL;

  return return_value;
}
#endif // IMAGEMAGICK_SUPPORT
