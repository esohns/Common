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
//#include <libswscale/swscale.h>
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
Common_Image_Tools::save (unsigned int width_in,
                          unsigned int height_in,
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
                                      width_in);
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
           i < height_in;
           ++i)
      {
        result_3 = ACE_OS::fwrite ((sourceBuffers_in[0] + (i * linesizes[0])),
                                   width_in,
                                   1,
                                   file_p);
        if (result_3 != 1)
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to ACE_OS::fwrite(%u) (file was: \"%s\"): \"%m\", aborting\n"),
                      width_in,
                      ACE_TEXT (path_in.c_str ())));
          goto clean;
        } // end IF
      } // end FOR
      // write U plane
      height_in >>= 1;
      width_in >>= 1;
      for (unsigned int i = 0;
           i < height_in;
           ++i)
      {
        result_3 = ACE_OS::fwrite ((sourceBuffers_in[1] + (i * linesizes[1])),
                                   width_in,
                                   1,
                                   file_p);
        if (result_3 != 1)
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to ACE_OS::fwrite(%u) (file was: \"%s\"): \"%m\", aborting\n"),
                      width_in,
                      ACE_TEXT (path_in.c_str ())));
          goto clean;
        } // end IF
      } // end FOR
      // write V plane
      for (unsigned int i = 0;
           i < height_in;
           ++i)
      {
        result_3 = ACE_OS::fwrite ((sourceBuffers_in[2] + (i * linesizes[2])),
                                   width_in,
                                   1,
                                   file_p);
        if (result_3 != 1)
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to ACE_OS::fwrite(%u) (file was: \"%s\"): \"%m\", aborting\n"),
                      width_in,
                      ACE_TEXT (path_in.c_str ())));
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
Common_Image_Tools::savePNG (unsigned int width_in,
                             unsigned int height_in,
                             enum AVPixelFormat format_in,
                             uint8_t* sourceBuffers_in[],
                             const std::string& path_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Image_Tools::savePNG"));

  bool result = false;

  // sanity check(s)
  ACE_ASSERT (format_in == AV_PIX_FMT_RGB24);

  int result_2 = -1;
  size_t result_3 = 0;
  struct AVCodec* codec_p = NULL;
  struct AVCodecContext* codec_context_p = NULL;
  int got_picture = 0;
  FILE* file_p = NULL;
  struct AVPacket packet_s;
  struct AVFrame* frame_p = av_frame_alloc ();
  if (!frame_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("av_frame_alloc() failed (result was: %d), aborting\n"),
                result_2));
    return false;
  } // end IF
  frame_p->format = format_in;
  frame_p->height = height_in;
  frame_p->width = width_in;
  result_2 = av_image_fill_linesizes (frame_p->linesize,
                                      format_in,
                                      static_cast<int> (width_in));
  ACE_ASSERT (result_2 >= 0);
  result_2 =
      av_image_fill_pointers (frame_p->data,
                              format_in,
                              static_cast<int> (height_in),
                              sourceBuffers_in[0],
                              frame_p->linesize);
  ACE_ASSERT (result_2 >= 0);

  codec_p = avcodec_find_encoder (AV_CODEC_ID_PNG);
  if (!codec_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("avcodec_find_encoder(AV_CODEC_ID_PNG) failed, aborting\n")));
    goto clean;
  } // end IF
  codec_context_p = avcodec_alloc_context3 (codec_p);
  if (!codec_context_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("avcodec_alloc_context3() failed, aborting\n")));
    goto clean;
  } // end IF

  codec_context_p->codec_type = AVMEDIA_TYPE_VIDEO;
  codec_context_p->height = height_in;
  codec_context_p->width = width_in;
  codec_context_p->pix_fmt = AV_PIX_FMT_RGB24;
  result_2 = avcodec_open2 (codec_context_p,
                            codec_p,
                            NULL);
  if (result_2 < 0)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("avcodec_open2() failed (result was: %d), aborting\n"),
                result_2));
    goto clean;
  } // end IF

  av_init_packet (&packet_s);
  packet_s.size = 0;
  packet_s.data = NULL;
  result_2 = avcodec_encode_video2 (codec_context_p,
                                    &packet_s,
                                    frame_p,
                                    &got_picture);
  if ((result_2 < 0) || !got_picture)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("avcodec_encode_video2() failed (result was: %d), aborting\n"),
                result_2));
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
    avcodec_close (codec_context_p);
    av_free (codec_context_p);
  } // end IF
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
