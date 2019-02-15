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

#include "ace/Log_Msg.h"
#include "ace/OS.h"

#include "common_macros.h"

#include "common_error_tools.h"

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

  uint8_t* data_p = NULL;
  uint8_t* buffer_p = sourceBuffers_in[0];

  // *TODO*: this feature is broken
  if (unlikely (format_in != AV_PIX_FMT_RGB24))
  {
    if (!Common_Image_Tools::convert (NULL,
                                      resolution_in,
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
    buffer_p = data_p;
  } // end IF

  int result_2 = -1;
  size_t result_3 = 0;
  struct AVCodec* codec_p = NULL;
  struct AVCodecContext* codec_context_p = NULL;
  int got_picture = 0;
  FILE* file_p = NULL;
  struct AVPacket packet_s;
  av_init_packet (&packet_s);
  struct AVFrame* frame_p = av_frame_alloc ();
  if (!frame_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to av_frame_alloc(): \"%m\", aborting\n")));
    return false;
  } // end IF
  frame_p->format = AV_PIX_FMT_RGB24;

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  frame_p->height = resolution_in.cy;
  frame_p->width = resolution_in.cx;
#else
  frame_p->height = resolution_in.height;
  frame_p->width = resolution_in.width;
#endif // ACE_WIN32 || ACE_WIN64
  result_2 = av_image_fill_linesizes (frame_p->linesize,
                                      format_in,
#if defined (ACE_WIN32) || defined (ACE_WIN64)
                                      static_cast<int> (resolution_in.cx));
#else
                                      static_cast<int> (resolution_in.width));
#endif // ACE_WIN32 || ACE_WIN64
  ACE_ASSERT (result_2 >= 0);
  result_2 =
      av_image_fill_pointers (frame_p->data,
                              format_in,
#if defined (ACE_WIN32) || defined (ACE_WIN64)
                              static_cast<int> (resolution_in.cy),
#else
                              static_cast<int> (resolution_in.height),
#endif // ACE_WIN32 || ACE_WIN64
                              buffer_p,
                              frame_p->linesize);
  ACE_ASSERT (result_2 >= 0);

  codec_p = avcodec_find_encoder (AV_CODEC_ID_PNG);
  if (!codec_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to avcodec_find_encoder(%d): \"%m\", aborting\n"),
                AV_CODEC_ID_PNG));
    goto clean;
  } // end IF
  codec_context_p = avcodec_alloc_context3 (codec_p);
  if (!codec_context_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to avcodec_alloc_context3(): \"%m\", aborting\n")));
    goto clean;
  } // end IF

  codec_context_p->codec_type = AVMEDIA_TYPE_VIDEO;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  codec_context_p->height = resolution_in.cy;
  codec_context_p->width = resolution_in.cx;
#else
  codec_context_p->height = resolution_in.height;
  codec_context_p->width = resolution_in.width;
#endif // ACE_WIN32 || ACE_WIN64
  codec_context_p->pix_fmt = AV_PIX_FMT_RGB24;
  codec_context_p->time_base.num = codec_context_p->time_base.den = 1;
  result_2 = avcodec_open2 (codec_context_p,
                            codec_p,
                            NULL);
  if (result_2 < 0)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to avcodec_open2(): \"%s\", aborting\n"),
                ACE_TEXT (Common_Image_Tools::errorToString (result_2).c_str ())));
    goto clean;
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
    goto clean;
  } // end IF

  file_p = ACE_OS::fopen (path_in.c_str (),
                          ACE_TEXT_ALWAYS_CHAR ("wb"));
  if (!file_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::fopen(\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (path_in.c_str ())));
    goto clean;
  } // end IF

  result_3 = ACE_OS::fwrite (packet_s.data,
                             packet_s.size,
                             1,
                             file_p);
  if (result_3 != 1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::fwrite(%u) (file was: \"%s\"): \"%m\", aborting\n"),
                packet_s.size,
                ACE_TEXT (path_in.c_str ())));
    goto clean;
  } // end IF

  result = true;

clean:
  // clean up
  av_packet_unref (&packet_s);
  if (frame_p)
    av_frame_free (&frame_p);
  if (codec_context_p)
  {
    avcodec_close (codec_context_p); codec_context_p = NULL;
    av_free (codec_context_p); codec_context_p = NULL;
  } // end IF
  if (file_p)
  {
    result_2 = ACE_OS::fclose (file_p);
    if (result_2 == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_OS::fclose(\"%s\"): \"%m\", continuing\n"),
                  ACE_TEXT (path_in.c_str ())));
    file_p = NULL;
  } // end IF

#if defined (_DEBUG)
  if (result)
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("wrote file \"%s\"\n"),
                ACE_TEXT (path_in.c_str ())));
#endif // _DEBUG

  return result;
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
