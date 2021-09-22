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

#include "common_gl_tools.h"

#include "assimp/cimport.h"
#include "assimp/scene.h"

#include "ace/Log_Msg.h"

#include "common_file_tools.h"
#include "common_macros.h"

#include "common_image_common.h"
#include "common_image_defines.h"
#include "common_image_tools.h"

#include "common_gl_assimp_tools.h"
#include "common_gl_defines.h"
#include "common_gl_image_tools.h"

std::string
Common_GL_Tools::errorToString (GLenum error_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_GL_Tools::errorToString"));

  // initialize return value
  std::string return_value;

  switch (error_in)
  {
    case GL_NO_ERROR: // 0
      break;
    case GL_INVALID_ENUM:
      return_value = ACE_TEXT_ALWAYS_CHAR ("An unacceptable value is specified for an enumerated argument"); break;
    case GL_INVALID_VALUE:
      return_value = ACE_TEXT_ALWAYS_CHAR ("A numeric argument is out of range"); break;
    case GL_INVALID_OPERATION:
      return_value = ACE_TEXT_ALWAYS_CHAR ("The specified operation is not allowed in the current state"); break;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
    case GL_INVALID_FRAMEBUFFER_OPERATION:
      return_value = ACE_TEXT_ALWAYS_CHAR ("The framebuffer object is not complete"); break;
#endif // ACE_WIN32 || ACE_WIN64
    case GL_STACK_OVERFLOW:
      return_value = ACE_TEXT_ALWAYS_CHAR ("An attempt has been made to perform an operation that would cause an internal stack to overflow"); break;
    case GL_STACK_UNDERFLOW:
      return_value = ACE_TEXT_ALWAYS_CHAR ("An attempt has been made to perform an operation that would cause an internal stack to underflow"); break;
    case GL_OUT_OF_MEMORY:
      return_value = ACE_TEXT_ALWAYS_CHAR ("There is not enough memory left to execute the command"); break;
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown OpenGL error (was: %d), continuing\n"),
                  error_in));
      break;
    }
  } // end SWITCH

  return return_value;
}

GLuint
Common_GL_Tools::loadModel (const std::string& path_in,
                            Common_GL_BoundingBox_t& boundingBox_out,
#if defined (GLM_SUPPORT)
                            glm::vec3& center_out)
#else
                            struct Common_GL_VectorF3& center_out)
#endif // GLM_SUPPORT
{
  COMMON_TRACE (ACE_TEXT ("Common_GL_Tools::loadModel"));

  // initialize return value(s)
  GLuint return_value = 0;
  boundingBox_out.first.x = boundingBox_out.first.y = boundingBox_out.first.z = 0;
  boundingBox_out.second.x = boundingBox_out.second.y = boundingBox_out.second.z = 0;

  // sanity check(s)
  ACE_ASSERT (Common_File_Tools::isReadable (path_in));

  struct aiScene* scene_p = NULL;
  if (!Common_GL_Assimp_Tools::loadModel (path_in,
                                          scene_p))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_GL_Assimp_Tools::loadModel(\"%s\"), aborting\n"),
                ACE_TEXT (path_in.c_str ())));
    return return_value;
  } // end IF
  ACE_ASSERT (scene_p);

  return_value = glGenLists (1);
  COMMON_GL_ASSERT;
  glNewList (return_value, GL_COMPILE);
  COMMON_GL_ASSERT;
  // now begin at the root node of the imported data and traverse the scenegraph
  // by multiplying subsequent local transforms together on GLs' matrix stack
  Common_GL_Assimp_Tools::render (scene_p, scene_p->mRootNode);
  glEndList ();
  COMMON_GL_ASSERT;

  // compute the scenes' bounding box / center
  aiVector3D min, max;
  Common_GL_Assimp_Tools::boundingBox (scene_p,
                                       &min, &max);

  boundingBox_out.first.x = min.x;
  boundingBox_out.first.y = min.y;
  boundingBox_out.first.z = min.z;

  boundingBox_out.second.x = max.x;
  boundingBox_out.second.y = max.y;
  boundingBox_out.second.z = max.z;

  center_out.x = (min.x + max.x) / 2.0F;
  center_out.y = (min.y + max.y) / 2.0F;
  center_out.z = (min.z + max.z) / 2.0F;

  // clean up
  aiReleaseImport (scene_p); scene_p = NULL;

  return return_value;
}

GLuint
Common_GL_Tools::loadTexture (const std::string& path_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_GL_Tools::loadTexture"));

  GLuint return_value = 0;

  // sanity check(s)
  if (!Common_File_Tools::isReadable (path_in))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_File_Tools::isReadable(\"%s\"), aborting\n"),
                ACE_TEXT (path_in.c_str ())));
    return return_value;
  } // end IF

  unsigned int width = 0, height = 0;
  bool has_alpha = false;
  GLubyte* image_p = NULL;
  switch (Common_Image_Tools::fileExtensionToType (path_in))
  {
    case COMMON_IMAGE_FILE_PNG:
    {
#if defined (LIBPNG_SUPPORT)
      if (!Common_GL_Image_Tools::loadPNG (path_in,
                                           width, height,
                                           has_alpha,
                                           image_p))
#elif defined (IMAGEMAGICK_SUPPORT)
      if (!Common_GL_Image_Tools::loadPNG (path_in,
                                           width, height,
                                           image_p))
#endif // LIBPNG_SUPPORT || IMAGEMAGICK_SUPPORT
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to Common_GL_Image_Tools::loadPNG(\"%s\"), aborting\n"),
                    ACE_TEXT (path_in.c_str ())));
        return return_value;
      } // end IF
#if defined (LIBPNG_SUPPORT)
#elif defined (IMAGEMAGICK_SUPPORT)
      has_alpha = true;
#endif // LIBPNG_SUPPORT || IMAGEMAGICK_SUPPORT
      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown image file format (path was: \"%s\"), aborting\n"),
                  ACE_TEXT (path_in.c_str ())));
      return return_value;
    }
  } // end SWITCH
  ACE_ASSERT (width && height);
  ACE_ASSERT (image_p);
  ACE_UNUSED_ARG (has_alpha);
//#if defined (_DEBUG)
//  Common_Image_Resolution_t resolution_s;
//  resolution_s.width = width; resolution_s.height = height;
//  std::string path = Common_File_Tools::getTempDirectory ();
//  path += ACE_DIRECTORY_SEPARATOR_STR;
//  path += Common_File_Tools::basename (path_in, false);
//  Common_Image_Tools::savePNG (resolution_s,
//                               (has_alpha ? AV_PIX_FMT_RGB32 : AV_PIX_FMT_RGB24),
//                               image_p,
//                               path);
//#endif // _DEBUG

  glGenTextures (1, &return_value);
  COMMON_GL_ASSERT;
  glBindTexture (GL_TEXTURE_2D, return_value);
  COMMON_GL_ASSERT;

  glEnableClientState (GL_TEXTURE_COORD_ARRAY);
  COMMON_GL_ASSERT;
  glEnableClientState (GL_VERTEX_ARRAY);
  COMMON_GL_ASSERT;
  glTexEnvi (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  COMMON_GL_ASSERT;
  // select modulate to mix texture with color for shading
//    glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
//    COMMON_GL_ASSERT;

//#if !defined (GL_VERSION_1_2)
//  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP); // GL_CLAMP_TO_EDGE
//#else
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
//#endif // GL_VERSION_1_2
//  COMMON_GL_ASSERT;
//#if !defined (GL_VERSION_1_2)
//  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP); // GL_CLAMP_TO_EDGE
//#else
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
//#endif // GL_VERSION_1_1
  COMMON_GL_ASSERT;

  glTexImage2D (GL_TEXTURE_2D, 0, (has_alpha ? GL_RGBA8 : GL_RGB8), width, height, 0,
                (has_alpha ? GL_RGBA : GL_RGB),
                GL_UNSIGNED_BYTE, image_p);
  COMMON_GL_ASSERT;

  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  COMMON_GL_ASSERT;
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  COMMON_GL_ASSERT;
#if defined (GL_VERSION_1_4)
  glTexParameteri (GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
  COMMON_GL_ASSERT;
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
  COMMON_GL_ASSERT;
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 1000);
  COMMON_GL_ASSERT;
#endif // GL_VERSION_1_4

  glBindTexture (GL_TEXTURE_2D, 0);

  // clean up
  free (image_p); image_p = NULL;

  return return_value;
}

GLuint
Common_GL_Tools::loadTexture (const uint8_t* data_in,
                              unsigned int width_in,
                              unsigned int height_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_GL_Tools::loadTexture"));

  GLuint return_value = 0;

  glGenTextures (1, &return_value);
  //COMMON_GL_ASSERT;
  glBindTexture (GL_TEXTURE_2D, return_value);
  //COMMON_GL_ASSERT;

  glEnableClientState (GL_TEXTURE_COORD_ARRAY);
  //COMMON_GL_ASSERT;
  glEnableClientState (GL_VERTEX_ARRAY);
  //COMMON_GL_ASSERT;
  glTexEnvi (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  //COMMON_GL_ASSERT;
  // select modulate to mix texture with color for shading
//    glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
//    ACE_ASSERT (glGetError () == GL_NO_ERROR);

//#if !defined (GL_VERSION_1_2)
//  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP); // GL_CLAMP_TO_EDGE
//#else
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
//#endif // GL_VERSION_1_2
//  COMMON_GL_ASSERT;
//#if !defined (GL_VERSION_1_2)
//  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP); // GL_CLAMP_TO_EDGE
//#else
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
//#endif // GL_VERSION_1_1
  //COMMON_GL_ASSERT;

  glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA8, width_in, height_in, 0,
                GL_RGBA,
                GL_UNSIGNED_BYTE, data_in);
  //COMMON_GL_ASSERT;

  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  //COMMON_GL_ASSERT;
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  //COMMON_GL_ASSERT;
#if defined (GL_VERSION_1_4)
  glTexParameteri (GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
  //COMMON_GL_ASSERT;
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
  //COMMON_GL_ASSERT;
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 1000);
  //COMMON_GL_ASSERT;
#endif // GL_VERSION_1_4

  //glBindTexture (GL_TEXTURE_2D, 0);

  return return_value;
}

void
Common_GL_Tools::drawCube (bool setTextureCoordinates_in)
{
//  COMMON_TRACE (ACE_TEXT ("Common_GL_Tools::drawCube"));

  glBegin (GL_QUADS);

  // Front Face
  if (setTextureCoordinates_in)
    glTexCoord2f (0.0f, 0.0f);
  else
    glColor3f (1.0f, 0.0f, 0.0f);
  glVertex3f (-1.0f, -1.0f,  1.0f); // Bottom Left Of The Texture and Quad
  if (setTextureCoordinates_in)
    glTexCoord2f (1.0f, 0.0f);
  glVertex3f ( 1.0f, -1.0f,  1.0f); // Bottom Right Of The Texture and Quad
  if (setTextureCoordinates_in)
    glTexCoord2f (1.0f, 1.0f);
  glVertex3f ( 1.0f,  1.0f,  1.0f); // Top Right Of The Texture and Quad
  if (setTextureCoordinates_in)
    glTexCoord2f (0.0f, 1.0f);
  glVertex3f (-1.0f,  1.0f,  1.0f); // Top Left Of The Texture and Quad

  // Back Face
  if (setTextureCoordinates_in)
    glTexCoord2f (1.0f, 0.0f);
  else
    glColor3f (0.0f, 1.0f, 0.0f);
  glVertex3f (-1.0f, -1.0f, -1.0f); // Bottom Right Of The Texture and Quad
  if (setTextureCoordinates_in)
    glTexCoord2f (1.0f, 1.0f);
  glVertex3f (-1.0f,  1.0f, -1.0f); // Top Right Of The Texture and Quad
  if (setTextureCoordinates_in)
    glTexCoord2f (0.0f, 1.0f);
  glVertex3f ( 1.0f,  1.0f, -1.0f); // Top Left Of The Texture and Quad
  if (setTextureCoordinates_in)
    glTexCoord2f (0.0f, 0.0f);
  glVertex3f ( 1.0f, -1.0f, -1.0f); // Bottom Left Of The Texture and Quad

  // Top Face
  if (setTextureCoordinates_in)
    glTexCoord2f (0.0f, 1.0f);
  else
    glColor3f (0.0f, 0.0f, 1.0f);
  glVertex3f (-1.0f,  1.0f, -1.0f); // Top Left Of The Texture and Quad
  if (setTextureCoordinates_in)
    glTexCoord2f (0.0f, 0.0f);
  glVertex3f (-1.0f,  1.0f,  1.0f); // Bottom Left Of The Texture and Quad
  if (setTextureCoordinates_in)
    glTexCoord2f (1.0f, 0.0f);
  glVertex3f ( 1.0f,  1.0f,  1.0f); // Bottom Right Of The Texture and Quad
  if (setTextureCoordinates_in)
    glTexCoord2f (1.0f, 1.0f);
  glVertex3f ( 1.0f,  1.0f, -1.0f); // Top Right Of The Texture and Quad

  // Bottom Face
  if (setTextureCoordinates_in)
    glTexCoord2f (1.0f, 1.0f);
  else
    glColor3f (1.0f, 1.0f, 0.0f);
  glVertex3f (-1.0f, -1.0f, -1.0f); // Top Right Of The Texture and Quad
  if (setTextureCoordinates_in)
    glTexCoord2f (0.0f, 1.0f);
  glVertex3f ( 1.0f, -1.0f, -1.0f); // Top Left Of The Texture and Quad
  if (setTextureCoordinates_in)
    glTexCoord2f (0.0f, 0.0f);
  glVertex3f ( 1.0f, -1.0f,  1.0f); // Bottom Left Of The Texture and Quad
  if (setTextureCoordinates_in)
    glTexCoord2f (1.0f, 0.0f);
  glVertex3f (-1.0f, -1.0f,  1.0f); // Bottom Right Of The Texture and Quad

  // Right face
  if (setTextureCoordinates_in)
    glTexCoord2f (1.0f, 0.0f);
  else
    glColor3f (1.0f, 0.0f, 1.0f);
  glVertex3f ( 1.0f, -1.0f, -1.0f); // Bottom Right Of The Texture and Quad
  if (setTextureCoordinates_in)
    glTexCoord2f (1.0f, 1.0f);
  glVertex3f ( 1.0f,  1.0f, -1.0f); // Top Right Of The Texture and Quad
  if (setTextureCoordinates_in)
    glTexCoord2f (0.0f, 1.0f);
  glVertex3f ( 1.0f,  1.0f,  1.0f); // Top Left Of The Texture and Quad
  if (setTextureCoordinates_in)
    glTexCoord2f (0.0f, 0.0f);
  glVertex3f ( 1.0f, -1.0f,  1.0f); // Bottom Left Of The Texture and Quad

  // Left Face
  if (setTextureCoordinates_in)
    glTexCoord2f (0.0f, 0.0f);
  else
    glColor3f (1.0f, 1.0f, 1.0f);
  glVertex3f (-1.0f, -1.0f, -1.0f); // Bottom Left Of The Texture and Quad
  if (setTextureCoordinates_in)
    glTexCoord2f (1.0f, 0.0f);
  glVertex3f (-1.0f, -1.0f,  1.0f); // Bottom Right Of The Texture and Quad
  if (setTextureCoordinates_in)
    glTexCoord2f (1.0f, 1.0f);
  glVertex3f (-1.0f,  1.0f,  1.0f); // Top Right Of The Texture and Quad
  if (setTextureCoordinates_in)
    glTexCoord2f (0.0f, 1.0f);
  glVertex3f (-1.0f,  1.0f, -1.0f); // Top Left Of The Texture and Quad

  glEnd ();
}
