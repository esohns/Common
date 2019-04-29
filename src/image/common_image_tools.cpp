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

#ifdef __cplusplus
extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavutil/imgutils.h"
#include "libswscale/swscale.h"
}
#endif /* __cplusplus */

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#define ssize_t ssize_t // *NOTE*: use ACEs' declaration
#include "MagickWand/MagickWand.h"
#else
#include "wand/magick_wand.h"
#endif // ACE_WIN32 || ACE_WIN64

#include "ace/Log_Msg.h"
#include "ace/OS.h"

#include "common_file_tools.h"
#include "common_macros.h"

#include "common_error_tools.h"

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
#endif // ACE_WIN32 || ACE_WIN64

bool
Common_Image_Tools::save (const Common_Image_Resolution_t& resolution_in,
                          enum AVPixelFormat format_in,
                          const uint8_t* sourceBuffers_in[],
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
                             const uint8_t* sourceBuffers_in,
                             const std::string& path_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Image_Tools::savePNG"));

  bool result = false;
  bool delete_buffer_b = false;
  const uint8_t* buffer_p = sourceBuffers_in;

  // *TODO*: this feature is broken
  if (unlikely (format_in != AV_PIX_FMT_RGB24))
  {
    uint8_t* data_p = NULL;
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
    buffer_p = data_p;
    delete_buffer_b = true;
  } // end IF

  if (!Common_Image_Tools::save (resolution_in,
                                 format_in,
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
Common_Image_Tools::load (const uint8_t* sourceBuffers_in,
                          unsigned int sourceBuffersSize_in,
                          enum AVCodecID codecId_in,
                          enum AVPixelFormat format_in,
                          Common_Image_Resolution_t& resolution_out,
                          uint8_t*& targetBuffers_out)
{
  COMMON_TRACE (ACE_TEXT ("Common_Image_Tools::savePNG"));

  // sanity check(s)
  ACE_ASSERT (codecId_in != AV_CODEC_ID_NONE); // *TODO*

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
  ACE_ASSERT (targetBuffers_out);

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
                          enum AVCodecID codecId_in,
                          Common_Image_Resolution_t& resolution_out,
                          enum AVPixelFormat& format_out,
                          uint8_t*& targetBuffers_out)
{
  COMMON_TRACE (ACE_TEXT ("Common_Image_Tools::load"));

  // sanity check(s)
  ACE_ASSERT (codecId_in != AV_CODEC_ID_NONE); // *TODO*

  bool result = false;
  int result_2 = -1;
  struct AVCodec* codec_p = NULL;
  struct AVCodecContext* codec_context_p = NULL;
  struct AVFrame* frame_p = NULL;
  int got_picture = 0;
  struct AVPacket packet_s;
  av_init_packet (&packet_s);
  unsigned int file_size_i = 0;
  if (!Common_File_Tools::load (path_in,
                                packet_s.data,
                                file_size_i))
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
//  codec_context_p->flags = 0;
//  codec_context_p->flags2 = 0;
//  codec_context_p->get_format = common_image_tools_get_format_cb;
//  codec_context_p->request_sample_fmt = AV_PIX_FMT_RGB24;
  codec_context_p->refcounted_frames = 1; // *NOTE*: caller 'owns' the frame buffer
  codec_context_p->workaround_bugs = 0xFFFFFFFF;
  codec_context_p->strict_std_compliance = FF_COMPLIANCE_UNOFFICIAL;
  codec_context_p->error_concealment = 0xFFFFFFFF;
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
  if (result_2 < 0)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to avcodec_open2(): \"%s\", aborting\n"),
                ACE_TEXT (Common_Image_Tools::errorToString (result_2).c_str ())));
    goto error;
  } // end IF

  result_2 = avcodec_decode_video2 (codec_context_p,
                                    frame_p,
                                    &got_picture,
                                    &packet_s);
  if ((result_2 < 0) || !got_picture)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to avcodec_decode_video2(): \"%s\", aborting\n"),
                ACE_TEXT (Common_Image_Tools::errorToString (result_2).c_str ())));
    goto error;
  } // end IF
#if defined (_DEBUG)
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("loaded image \"%s\" (%ux%u %s)\n"),
              ACE_TEXT (path_in.c_str ()),
              frame_p->width, frame_p->height,
              ACE_TEXT (Common_Image_Tools::pixelFormatToString (static_cast<enum AVPixelFormat> (frame_p->format)).c_str ())));
#endif // _DEBUG

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  resolution_out.cx = frame_p->width;
  resolution_out.cy = frame_p->height;
#else
  resolution_out.width = frame_p->width;
  resolution_out.height = frame_p->height;
#endif // ACE_WIN32 || ACE_WIN64
  format_out = static_cast<enum AVPixelFormat> (frame_p->format);
  // *NOTE*: do not av_frame_unref the frame, keep the data
  targetBuffers_out = reinterpret_cast<uint8_t*> (frame_p->data);

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
Common_Image_Tools::save (const Common_Image_Resolution_t& sourceResolution_in,
                          enum AVPixelFormat sourcePixelFormat_in,
                          const uint8_t* sourceBuffers_in,
                          enum AVCodecID codecId_in,
                          const std::string& targetPath_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Image_Tools::save"));

  bool result = false;
  int result_2 = -1;
  struct AVCodec* codec_p = NULL;
  struct AVCodecContext* codec_context_p = NULL;
  int got_picture = 0;
  struct AVPacket packet_s;
  av_init_packet (&packet_s);
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
                              const_cast<uint8_t*> (sourceBuffers_in),
                              frame_p->linesize);
  ACE_ASSERT (result_2 >= 0);

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

  packet_s.size = 0;
  packet_s.data = NULL;
  result_2 = avcodec_encode_video2 (codec_context_p,
                                    &packet_s,
                                    frame_p,
                                    &got_picture);
  if ((result_2 < 0) || !got_picture)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to avcodec_encode_video2(): \"%s\", aborting\n"),
                ACE_TEXT (Common_Image_Tools::errorToString (result_2).c_str ())));
    goto error;
  } // end IF

  if (!Common_File_Tools::store (targetPath_in,
                                 packet_s.data,
                                 packet_s.size))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_File_Tools::store(\"%s\"), aborting\n"),
                ACE_TEXT (targetPath_in.c_str ())));
    goto error;
  } // end IF
#if defined (_DEBUG)
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("wrote file \"%s\" (%u byte(s)\n"),
              ACE_TEXT (targetPath_in.c_str ()),
              packet_s.size));
#endif // _DEBUG

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
Common_Image_Tools::decode (const uint8_t* sourceBuffers_in,
                            unsigned int sourceBuffersSize_in,
                            enum AVCodecID codecId_in,
                            enum AVPixelFormat format_in,
                            Common_Image_Resolution_t& resolution_out,
                            uint8_t*& targetBuffers_out)
{
  COMMON_TRACE (ACE_TEXT ("Common_Image_Tools::decode"));

  // sanity check(s)
  ACE_ASSERT (codecId_in != AV_CODEC_ID_NONE); // *TODO*

  bool result = false;
  int result_2 = -1;
  struct AVCodec* codec_p = NULL;
  struct AVCodecContext* codec_context_p = NULL;
  Common_Image_Tools_GetFormatCBData cb_data_s;
  cb_data_s.formats.push_back (format_in);
  struct AVFrame* frame_p = NULL;
  int got_picture = 0;
  struct AVPacket packet_s;
  av_init_packet (&packet_s);
  packet_s.data = const_cast<uint8_t*> (sourceBuffers_in);
  packet_s.size = sourceBuffersSize_in;

  frame_p = av_frame_alloc ();
  if (!frame_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to av_frame_alloc(): \"%m\", aborting\n")));
    goto error;
  } // end IF

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
//  codec_context_p->flags = 0;
//  codec_context_p->flags2 = 0;
  codec_context_p->time_base.den = 1;
  codec_context_p->time_base.num = 1;
  codec_context_p->width = 1;
  codec_context_p->height = 1;
  codec_context_p->pix_fmt = AV_PIX_FMT_RGB24;
  codec_context_p->opaque = &cb_data_s;
  codec_context_p->get_format = common_image_tools_get_format_cb;
//  codec_context_p->request_sample_fmt = AV_PIX_FMT_RGB24;
  codec_context_p->thread_count = 1;
  codec_context_p->refcounted_frames = 1; // *NOTE*: caller 'owns' the frame buffer
  codec_context_p->workaround_bugs = 0xFFFFFFFF;
  codec_context_p->strict_std_compliance = FF_COMPLIANCE_UNOFFICIAL;
//  codec_context_p->error_concealment = 0xFFFFFFFF;
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
  if (result_2 < 0)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to avcodec_open2(): \"%s\", aborting\n"),
                ACE_TEXT (Common_Image_Tools::errorToString (result_2).c_str ())));
    goto error;
  } // end IF

  result_2 = avcodec_decode_video2 (codec_context_p,
                                    frame_p,
                                    &got_picture,
                                    &packet_s);
  if ((result_2 < 0) || !got_picture)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to avcodec_decode_video2(): \"%s\", aborting\n"),
                ACE_TEXT (Common_Image_Tools::errorToString (result_2).c_str ())));
    goto error;
  } // end IF
  ACE_ASSERT (format_in == static_cast<enum AVPixelFormat> (frame_p->format));

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  resolution_out.cx = frame_p->width;
  resolution_out.cy = frame_p->height;
#else
  resolution_out.width = frame_p->width;
  resolution_out.height = frame_p->height;
#endif // ACE_WIN32 || ACE_WIN64
  // *NOTE*: do not av_frame_unref the frame, keep the data
  targetBuffers_out = reinterpret_cast<uint8_t*> (frame_p->data);

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
                             const uint8_t* sourceBuffers_in,
                             const Common_Image_Resolution_t& targetResolution_in,
                             enum AVPixelFormat targetPixelFormat_in,
                             uint8_t*& targetBuffers_out)
{
  COMMON_TRACE (ACE_TEXT ("Common_Image_Tools::convert"));

  // initialize return value(s)
  ACE_ASSERT (!targetBuffers_out);

  int line_sizes_a[AV_NUM_DATA_POINTERS];
  uint8_t* data_pointers_a[AV_NUM_DATA_POINTERS];
  ACE_OS::memset (&line_sizes_a, 0, sizeof (int[AV_NUM_DATA_POINTERS]));
  ACE_OS::memset (&data_pointers_a, 0, sizeof (uint8_t*[AV_NUM_DATA_POINTERS]));

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
                              const_cast<uint8_t*> (sourceBuffers_in),
                              line_sizes_a);
  ACE_ASSERT (result >= 0);
  if (unlikely (!Common_Image_Tools::convert (NULL,
                                              sourceResolution_in, sourcePixelFormat_in,
                                              data_pointers_a,
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
                           const uint8_t* sourceBuffers_in,
                           const Common_Image_Resolution_t& targetResolution_in,
                           uint8_t*& targetBuffers_out)
{
  COMMON_TRACE (ACE_TEXT ("Common_Image_Tools::scale"));

  // initialize return value(s)
  ACE_ASSERT (!targetBuffers_out);

  int line_sizes_a[AV_NUM_DATA_POINTERS];
  uint8_t* data_pointers_a[AV_NUM_DATA_POINTERS];
  ACE_OS::memset (&line_sizes_a, 0, sizeof (int[AV_NUM_DATA_POINTERS]));
  ACE_OS::memset (&data_pointers_a, 0, sizeof (uint8_t*[AV_NUM_DATA_POINTERS]));

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
                              const_cast<uint8_t*> (sourceBuffers_in),
                              line_sizes_a);
  ACE_ASSERT (result >= 0);
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
                             uint8_t*& targetBuffers_out)
{
  COMMON_TRACE (ACE_TEXT ("Common_Image_Tools::convert"));

  // initialize return value(s)
  ACE_ASSERT (!targetBuffers_out);

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
  int flags = (//SWS_BILINEAR | SWS_FAST_BILINEAR | // interpolation
               SWS_BICUBIC);
  struct SwsContext* context_p = NULL;
  int result_2 = -1;
  int in_linesize[AV_NUM_DATA_POINTERS];
  int out_linesize[AV_NUM_DATA_POINTERS];
  uint8_t* out_data[AV_NUM_DATA_POINTERS];
  int size_i =
      av_image_get_buffer_size (targetPixelFormat_in,
#if defined (ACE_WIN32) || defined (ACE_WIN64)
                                targetResolution_in.cx, targetResolution_in.cy,
#else
                                targetResolution_in.width, targetResolution_in.height,
#endif // ACE_WIN32 || ACE_WIN64
                                1); // *TODO*: linesize alignment

  ACE_NEW_NORETURN (targetBuffers_out,
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
                              targetBuffers_out,
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

bool
Common_Image_Tools::scale (struct SwsContext* context_in,
                           const Common_Image_Resolution_t& sourceResolution_in,
                           enum AVPixelFormat pixelFormat_in,
                           uint8_t* sourceBuffers_in[],
                           const Common_Image_Resolution_t& targetResolution_in,
                           uint8_t*& targetBuffers_out)
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

  ACE_NEW_NORETURN (targetBuffers_out,
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
                              targetBuffers_out,
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
Common_Image_Tools::errorToString (int errorCode_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Image_Tools::errorToString"));

  // initialize return value(s)
  std::string return_value;

  char buffer_a[AV_ERROR_MAX_STRING_SIZE];
  int result = av_strerror (errorCode_in,
                            buffer_a,
                            sizeof (char[AV_ERROR_MAX_STRING_SIZE]));
  if (result < 0)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to av_strerror(%d): \"%m\", aborting\n"),
                errorCode_in));
    return return_value;
  } // end IF
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
  else
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("invalid/unknown format (was: \"%s\"), aborting\n"),
                ACE_TEXT (format_in.c_str ())));

  return return_value;
}

std::string
Common_Image_Tools::errorToString (struct _MagickWand* context_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Image_Tools::errorToString"));

  // initialize return value(s)
  std::string return_value;

  ExceptionType severity;
  char* description_p = MagickGetException (context_in, &severity);
  ACE_ASSERT (description_p);
  return_value = description_p;
  MagickRelinquishMemory (description_p); description_p = NULL;

  return return_value;
}
