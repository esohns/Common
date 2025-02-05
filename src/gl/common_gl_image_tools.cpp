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

#include "common_gl_image_tools.h"

#if defined (LIBPNG_SUPPORT)
#include "png.h"
#endif // LIBPNG_SUPPORT

#if defined (IMAGEMAGICK_SUPPORT)
#if defined (IMAGEMAGICK_IS_GRAPHICSMAGICK)
#include "wand/wand_api.h"
#else
#include "MagickWand/MagickWand.h"
#endif // IMAGEMAGICK_IS_GRAPHICSMAGICK
#endif // IMAGEMAGICK_SUPPORT

#if defined (STB_IMAGE_SUPPORT)
//#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
//#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#endif // STB_IMAGE_SUPPORT

#include "ace/Log_Msg.h"
#include "ace/OS.h"

#include "common_image_defines.h"
#include "common_image_tools.h"

#if defined (LIBPNG_SUPPORT)
bool
Common_GL_Image_Tools::loadPNG (const std::string& path_in,
                                unsigned int& width_out,
                                unsigned int& height_out,
                                bool& hasAlpha_out,
                                GLubyte*& data_out)
{
  COMMON_TRACE (ACE_TEXT ("Common_GL_Image_Tools::loadPNG"));

  // sanity check(s)
  ACE_ASSERT (!data_out);

  png_byte magic_a[COMMON_IMAGE_PNG_NUMBER_OF_MAGIC_BYTES];
  size_t bytes_read = 0;
  int is_png = 0;
  png_structp png_p = NULL;
  png_infop png_info_p = NULL;
  int color_type, interlace_type;
  FILE* file_p = NULL;
  int result = -1;
  size_t row_bytes = 0;
  png_bytepp row_pointers_pp = NULL;
  png_uint_32 width, height;
  int bit_depth, compression_method, filter_method;

  if (unlikely ((file_p = ACE_OS::fopen (path_in.c_str (),
                                         ACE_TEXT_ALWAYS_CHAR ("rb"))) == NULL))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::fopen(\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (path_in.c_str ())));
    return false;
  } // end IF

  // read the header magic, test if this could be a PNG file
  bytes_read = ACE_OS::fread (magic_a,
                              1,
                              COMMON_IMAGE_PNG_NUMBER_OF_MAGIC_BYTES,
                              file_p);
  if (unlikely (bytes_read != COMMON_IMAGE_PNG_NUMBER_OF_MAGIC_BYTES))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::fread(%d): \"%m\", aborting\n"),
                COMMON_IMAGE_PNG_NUMBER_OF_MAGIC_BYTES));
    goto error;
  } // end IF
  is_png = !png_sig_cmp (magic_a,
                         0,
                         COMMON_IMAGE_PNG_NUMBER_OF_MAGIC_BYTES);
  if (unlikely (!is_png))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to png_sig_cmp(): \"%m\", aborting\n")));
    goto error;
  } // end IF

  // create and initialize the png_struct with the desired error handler
  // functions. If you want to use the default stderr and longjump method, you
  // can supply NULL for the last three parameters. We also supply the compiler
  // header file version, so that we know if the application was compiled with a
  // compatible version of the library. REQUIRED
  png_p = png_create_read_struct (PNG_LIBPNG_VER_STRING,
                                  NULL, NULL, NULL);
  if (unlikely (!png_p))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to png_create_read_struct(): \"%m\", aborting\n")));
    goto error;
  } // end IF

  // allocate/initialize the memory for image information. REQUIRED
  png_info_p = png_create_info_struct (png_p);
  if (unlikely (!png_info_p))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to png_create_info_struct(): \"%m\", aborting\n")));
    goto error;
  } // end IF

  // set error handling if you are using the setjmp/longjmp method (this is the
  // normal method of doing things with libpng). REQUIRED unless you set up your
  // own error handlers in the png_create_read_struct() earlier
  if (unlikely (setjmp (png_jmpbuf (png_p))))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to setjmp(): \"%m\", aborting\n")));
    goto error;
  } // end IF

  // set up the output control if you are using standard C streams
  png_init_io (png_p, file_p);

  // if we have already read some of the signature
  ACE_ASSERT (bytes_read <= COMMON_IMAGE_PNG_NUMBER_OF_MAGIC_BYTES);
  png_set_sig_bytes (png_p, static_cast<int> (bytes_read));

  png_read_info (png_p, png_info_p);

  png_get_IHDR (png_p, png_info_p,
                &width, &height,
                &bit_depth, &color_type,
                &interlace_type, &compression_method, &filter_method);
  width_out = width; height_out = height;
  hasAlpha_out = (color_type == PNG_COLOR_TYPE_RGB_ALPHA);

  // update the png info struct
  png_read_update_info (png_p, png_info_p);

  row_bytes = png_get_rowbytes (png_p, png_info_p);
  // glTexImage2d requires rows to be 4-byte aligned
  row_bytes += 3 - ((row_bytes - 1) % 4);

  data_out = static_cast<GLubyte*> (malloc (row_bytes * height_out));
  if (unlikely (!data_out))
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to allocate memory: \"%m\", aborting\n")));
    goto error;
  } // end IF

  // row_pointers is for pointing to image_data for reading the png with libpng
  row_pointers_pp =
    static_cast<png_bytepp> (malloc (sizeof (png_bytep) * height_out));
  if (unlikely (!row_pointers_pp))
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to allocate memory: \"%m\", aborting\n")));
    goto error;
  } // end IF
  // set the individual row_pointers to point at the correct offsets of
  // image_data
  // *NOTE*: png is ordered top-to-bottom; OpenGL expects it bottom-to-top
  //         --> swap the order
  for (unsigned int i = 0; i < height_out; ++i)
    row_pointers_pp[height_out - 1 - i] = data_out + (i * row_bytes);

  png_read_image (png_p, row_pointers_pp);

  // clean up after the read, and free any memory allocated
  free (row_pointers_pp); row_pointers_pp = NULL;
  png_destroy_read_struct (&png_p, &png_info_p, NULL);

  // close the file
  result = ACE_OS::fclose (file_p);
  if (unlikely (result == -1))
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::fclose(): \"%m\", continuing\n")));

  return true;

error:
  if (data_out)
  {
    free (data_out); data_out = NULL;
  } // end IF
  if (row_pointers_pp)
    free (row_pointers_pp);
  if (png_p)
    png_destroy_read_struct (&png_p, &png_info_p, NULL);
  if (file_p)
  {
    result = ACE_OS::fclose (file_p);
    if (result == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_OS::fclose(): \"%m\", continuing\n")));
  } // end IF

  return false;
}
#endif // LIBPNG_SUPPORT

#if defined (IMAGEMAGICK_SUPPORT)
bool
Common_GL_Image_Tools::loadPNG (const std::string& path_in,
                                unsigned int& width_out,
                                unsigned int& height_out,
                                GLubyte*& data_out)
{
  COMMON_TRACE (ACE_TEXT ("Common_GL_Image_Tools::loadPNG"));

  // sanity check(s)
  ACE_ASSERT (!data_out);

  struct _MagickWand* context_p = NULL;
  unsigned int result = MagickTrue;
  unsigned char* blob_p = NULL;
  size_t file_size_i = 0;

  //MagickWandGenesis ();

  context_p = NewMagickWand ();
  if (unlikely (!context_p))
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to allocate memory, aborting\n")));
    goto error;
  } // end IF

//  MagickSetImageType (context_p, TrueColorType);
//  MagickSetImageColorspace (context_p, sRGBColorspace);

  result = MagickReadImage (context_p,
                            path_in.c_str ());
  if (unlikely (result != MagickTrue))
  { // see also: MAGICK_CODER_MODULE_PATH
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to MagickReadImage(): \"%s\", returning\n"),
                ACE_TEXT (Common_Image_Tools::errorToString (context_p).c_str ())));
    goto error;
  } // end IF
  width_out = static_cast<unsigned int> (MagickGetImageWidth (context_p));
  height_out = static_cast<unsigned int> (MagickGetImageHeight (context_p));

  result = MagickSetImageFormat (context_p, ACE_TEXT_ALWAYS_CHAR ("RGBA"));
  if (unlikely (result != MagickTrue))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to MagickSetImageFormat(RGBA): \"%s\", returning\n"),
                ACE_TEXT (Common_Image_Tools::errorToString (context_p).c_str ())));
    goto error;
  } // end IF

#if defined (IMAGEMAGICK_IS_GRAPHICSMAGICK)
  ACE_NEW_NORETURN (data_out,
                    GLubyte[4 * MagickGetImageWidth (context_p) * MagickGetImageHeight (context_p)]);
  result = MagickGetImagePixels (context_p,
                                 0, 0,
                                 MagickGetImageWidth (context_p), MagickGetImageHeight (context_p),
                                 ACE_TEXT_ALWAYS_CHAR ("RGBA"),
                                 StorageType::CharPixel,
                                 data_out);
  ACE_ASSERT (result == MagickFalse);
#else
  blob_p = MagickGetImageBlob (context_p, // was: MagickWriteImageBlob
                               &file_size_i);
  if (unlikely (!blob_p))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to MagickGetImageBlob(): \"%s\", returning\n"),
                ACE_TEXT (Common_Image_Tools::errorToString (context_p).c_str ())));
    goto error;
  } // end IF
  ACE_ASSERT (file_size_i == (4 * width_out * height_out));
  data_out = static_cast<GLubyte*> (malloc (file_size_i));
  if (unlikely (!data_out))
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to allocate memory: \"%m\", aborting\n")));
    goto error;
  } // end IF
  ACE_OS::memcpy (data_out, blob_p, file_size_i);

  MagickRelinquishMemory (blob_p); blob_p = NULL;
#endif // IMAGEMAGICK_IS_GRAPHICSMAGICK

  DestroyMagickWand (context_p); context_p = NULL;
  //MagickWandTerminus ();

  return true;

error:
  if (blob_p)
    MagickRelinquishMemory (blob_p);
  if (context_p)
    DestroyMagickWand (context_p);
  //MagickWandTerminus ();

  return false;
}
#endif // IMAGEMAGICK_SUPPORT

#if defined (STB_IMAGE_SUPPORT)
bool
Common_GL_Image_Tools::loadSTB (const std::string& path_in,
                                bool flipImage_in,
                                unsigned int& width_out,
                                unsigned int& height_out,
                                unsigned int& channels_out,
                                GLubyte*& data_out)
{
  COMMON_TRACE (ACE_TEXT ("Common_GL_Image_Tools::loadSTB"));

  // sanity check(s)
  ACE_ASSERT (!data_out);

  // for OpenGL...
  if (flipImage_in)
    stbi_set_flip_vertically_on_load (1);

  int width_i, height_i, channels_i;
  data_out = stbi_load (path_in.c_str (),
                        &width_i, &height_i,
                        &channels_i,
                        STBI_rgb_alpha);
  if (unlikely (!data_out))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to stbi_load(\"%s\"): \"%s\", aborting\n"),
                ACE_TEXT (path_in.c_str ()),
                ACE_TEXT (stbi_failure_reason ())));
    return false;
  } // end IF

  width_out = static_cast<unsigned int> (width_i);
  height_out = static_cast<unsigned int> (height_i);
  channels_out = static_cast<unsigned int> (channels_i == 3 ? 4 : channels_i); // + alpha ?

  return true;
}

bool
Common_GL_Image_Tools::saveSTB (const std::string& path_in,
                                unsigned int width_in,
                                unsigned int height_in,
                                unsigned int numberOfChannels_in,
                                const GLubyte* data_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_GL_Image_Tools::saveSTB"));

  int result = -1;

  switch (Common_Image_Tools::fileExtensionToType (path_in))
  {
    case COMMON_IMAGE_FILE_PNG:
    {
      stbi_write_png_compression_level = 0;
      result =  stbi_write_png (path_in.c_str (),
                                static_cast<int> (width_in),
                                static_cast<int> (height_in),
                                static_cast<int> (numberOfChannels_in),
                                data_in,
                                static_cast<int> (width_in * numberOfChannels_in));
      break;
    }
    case COMMON_IMAGE_FILE_JPG:
    {
      result =  stbi_write_jpg (path_in.c_str (),
                                static_cast<int> (width_in),
                                static_cast<int> (height_in),
                                static_cast<int> (numberOfChannels_in),
                                data_in,
                                100);
      break;
    }
    default:
    {
//error:
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown image file format (path was: \"%s\"), aborting\n"),
                  ACE_TEXT (path_in.c_str ())));
      return false;
    }
  } // end SWITCH
  if (unlikely (result))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to stbi_write(\"%s\"): \"%s\", aborting\n"),
                ACE_TEXT (path_in.c_str ()),
                ACE_TEXT (stbi_failure_reason ())));
    return false;
  } // end IF

  return true;
}
#endif // STB_IMAGE_SUPPORT
