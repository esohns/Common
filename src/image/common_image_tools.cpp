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

#include "png.h"

#include "ace/Log_Msg.h"
#include "ace/OS.h"

#include "common_macros.h"

bool
Common_Image_Tools::loadPNG2OpenGL (const std::string& filename_in,
                                    unsigned int& width_out,
                                    unsigned int& height_out,
                                    bool& hasAlpha_out,
                                    GLubyte*& data_out)
{
  COMMON_TRACE (ACE_TEXT ("Common_Image_Tools::loadPNG"));

  // sanity check(s)
  ACE_ASSERT (!data_out);

  png_byte magic[8];
  size_t bytes_read = 0;
  int is_png = 0;
  png_structp png_p = NULL;
  png_infop png_info_p = NULL;
  int color_type, interlace_type;
  FILE* file_p = NULL;
  int result = -1;
  unsigned int row_bytes = 0;
  png_bytepp row_pointers_pp = NULL;
  png_uint_32 width, height;
  int bit_depth, compression_method, filter_method;

  if ((file_p = ACE_OS::fopen (filename_in.c_str (), "rb")) == NULL)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::fopen(\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (filename_in.c_str ())));
    return false;
  } // end IF
 
  // read the header magic, test if this could be a PNG file
  bytes_read = ACE_OS::fread (magic, 1, 8, file_p);
  if (bytes_read != 8)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::fread(8): \"%m\", aborting\n")));
    goto error;
  } // end IF
  is_png = !png_sig_cmp (magic, 0, 8);
  if (!is_png)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to png_sig_cmp(): \"%m\", aborting\n")));
    goto error;
  } // end IF

  /* Create and initialize the png_struct
   * with the desired error handler
   * functions.  If you want to use the
   * default stderr and longjump method,
   * you can supply NULL for the last
   * three parameters.  We also supply the
   * the compiler header file version, so
   * that we know if the application
   * was compiled with a compatible version
   * of the library.  REQUIRED
   */
  png_p = png_create_read_struct (PNG_LIBPNG_VER_STRING,
                                  NULL, NULL, NULL);
  if (!png_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to png_create_read_struct(): \"%m\", aborting\n")));
    goto error;
  } // end IF
 
  /* Allocate/initialize the memory
   * for image information.  REQUIRED. */
  png_info_p = png_create_info_struct (png_p);
  if (!png_info_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to png_create_info_struct(): \"%m\", aborting\n")));
    goto error;
  } // end IF
 
  /* Set error handling if you are
   * using the setjmp/longjmp method
   * (this is the normal method of
   * doing things with libpng).
   * REQUIRED unless you  set up
   * your own error handlers in
   * the png_create_read_struct()
   * earlier.
   */
  if (setjmp (png_jmpbuf (png_p)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to setjmp(): \"%m\", aborting\n")));
    goto error;
  } // end IF
 
  /* Set up the output control if
   * you are using standard C streams */
  png_init_io (png_p, file_p);
 
  /* If we have already
   * read some of the signature */
  png_set_sig_bytes (png_p, bytes_read);
 
  png_read_info (png_p, png_info_p);

  png_get_IHDR (png_p, png_info_p,
                &width, &height,
                &bit_depth, &color_type,
                &interlace_type, &compression_method, &filter_method);
  width_out = width; height_out = height;
  hasAlpha_out = (color_type == PNG_COLOR_TYPE_RGB_ALPHA);

  // Update the png info struct.
  png_read_update_info (png_p, png_info_p);

  row_bytes = png_get_rowbytes (png_p, png_info_p);
  // glTexImage2d requires rows to be 4-byte aligned
  row_bytes += 3 - ((row_bytes - 1) % 4);

  data_out = (GLubyte*)malloc (row_bytes * height_out);
  if (!data_out)
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to allocate memory, aborting\n")));
    goto error;
  } // end IF

  //row_pointers is for pointing to image_data for reading the png with libpng
  row_pointers_pp = (png_bytepp)malloc (sizeof (png_bytep) * height_out);
  if (!row_pointers_pp)
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to allocate memory, aborting\n")));
    goto error;
  }
  // set the individual row_pointers to point at the correct offsets of image_data
  // note that png is ordered top to bottom, but OpenGL expect it bottom to top
  // so the order is swapped
  for (unsigned int i = 0; i < height_out; ++i)
    row_pointers_pp[height_out - 1 - i] = data_out + (i * row_bytes);

  png_read_image (png_p, row_pointers_pp);

  /* Clean up after the read,
   * and free any memory allocated */
  free (row_pointers_pp);
  png_destroy_read_struct (&png_p, &png_info_p, NULL);
 
  /* Close the file */
  result = ACE_OS::fclose (file_p);
  if (result == -1)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::fclose(): \"%m\", continuing\n")));

  return true;

error:
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
