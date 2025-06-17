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

#if defined (ASSIMP_SUPPORT)
#include "assimp/cimport.h"
#include "assimp/scene.h"
#endif // ASSIMP_SUPPORT

#include "ace/Log_Msg.h"

#include "common_file_tools.h"
#include "common_macros.h"
#include "common_tools.h"

#include "common_image_common.h"
#include "common_image_defines.h"
#include "common_image_tools.h"

#if defined (ASSIMP_SUPPORT)
#include "common_gl_assimp_tools.h"
#endif // ASSIMP_SUPPORT
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

float
Common_GL_Tools::brightness (const Common_GL_Color_t& color_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_GL_Tools::brightness"));

  return (color_in.r * 0.2126f + color_in.g * 0.7152f + color_in.b * 0.0722f) / 255.0f;
}

Common_GL_Color_t
Common_GL_Tools::lerpRGB (const Common_GL_Color_t& color1_in,
                          const Common_GL_Color_t& color2_in,
                          float ratio_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_GL_Tools::lerpRGB"));

  Common_GL_Color_t result;

  // sanity check(s)
  ACE_ASSERT (ratio_in >= 0.0F && ratio_in <= 1.0F);

  result.r =
    color1_in.r + static_cast<uint8_t> ((color2_in.r - color1_in.r) * ratio_in);
  result.g =
    color1_in.g + static_cast<uint8_t> ((color2_in.g - color1_in.g) * ratio_in);
  result.b =
    color1_in.b + static_cast<uint8_t> ((color2_in.b - color1_in.b) * ratio_in);

  return result;
}

Common_GL_Color_t
Common_GL_Tools::toRGBColor (float color_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_GL_Tools::toRGBColor"));

  Common_GL_Color_t result;
  ACE_OS::memset (&result, 0, sizeof (Common_GL_Color_t));

  // sanity check(s)
  if (color_in < 0.0f)
    color_in = 0.0f;
  else if (color_in > 1.0f)
    color_in = 1.0f;

  static Common_GL_Color_t red = {255, 0, 0, 255};
  static Common_GL_Color_t green = {0, 255, 0, 255};
  static Common_GL_Color_t blue = {0, 0, 255, 255};
  if (color_in < 0.5f)
    result = Common_GL_Tools::lerpRGB (red, green, color_in * 2.0f);
  else if (color_in == 0.5f)
    result = green;
  else
    result = Common_GL_Tools::lerpRGB (green, blue, (color_in - 0.5f) * 2.0f);

  return result;
}

Common_GL_Color_t
Common_GL_Tools::randomColor ()
{
  COMMON_TRACE (ACE_TEXT ("Common_GL_Tools::randomColor"));

  Common_GL_Color_t result;
  result.r = static_cast<uint8_t> (Common_Tools::getRandomNumber (0, 255));
  result.g = static_cast<uint8_t> (Common_Tools::getRandomNumber (0, 255));
  result.b = static_cast<uint8_t> (Common_Tools::getRandomNumber (0, 255));
  result.a = 255U;

  return result;
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

#if defined (ASSIMP_SUPPORT)
  struct aiScene* scene_p = NULL;
  if (!Common_GL_Assimp_Tools::loadModel (path_in,
                                          scene_p,
                                          aiProcessPreset_TargetRealtime_MaxQuality))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_GL_Assimp_Tools::loadModel(\"%s\"), aborting\n"),
                ACE_TEXT (path_in.c_str ())));
    return return_value;
  } // end IF
  ACE_ASSERT (scene_p);
#endif // ASSIMP_SUPPORT

  return_value = glGenLists (1);
  // COMMON_GL_ASSERT;
  glNewList (return_value, GL_COMPILE);
  // COMMON_GL_ASSERT;
  // now begin at the root node of the imported data and traverse the scenegraph
  // by multiplying subsequent local transforms together on GLs' matrix stack
#if defined (ASSIMP_SUPPORT)
  Common_GL_Assimp_Tools::render (scene_p, scene_p->mRootNode);
#endif // ASSIMP_SUPPORT
  glEndList ();
  // COMMON_GL_ASSERT;

  // compute the scenes' bounding box / center
#if defined (ASSIMP_SUPPORT)
  aiVector3D min, max;
  Common_GL_Assimp_Tools::boundingBox (*scene_p,
                                       min, max);

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
#endif // ASSIMP_SUPPORT

  return return_value;
}

GLuint
Common_GL_Tools::loadTexture (const std::string& path_in,
                              bool flipImage_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_GL_Tools::loadTexture"));

  GLuint return_value = 0;

  unsigned int width = 0, height = 0, channels = 0;
  bool has_alpha = false;
  GLubyte* image_p = NULL;
  GLint internal_format;
  GLenum format;

  switch (Common_Image_Tools::fileExtensionToType (path_in))
  {
    case COMMON_IMAGE_FILE_PNG:
    {
#if defined (LIBPNG_SUPPORT)
      if (!Common_GL_Image_Tools::loadPNG (path_in,
                                           width, height,
                                           channels,
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
        return 0;
      } // end IF
#if defined (LIBPNG_SUPPORT)
#elif defined (IMAGEMAGICK_SUPPORT)
      channels = 4;
      has_alpha = true;
#endif // LIBPNG_SUPPORT || IMAGEMAGICK_SUPPORT
      break;
    }
    case COMMON_IMAGE_FILE_GIF:
    case COMMON_IMAGE_FILE_JPG:
    {
#if defined (STB_IMAGE_SUPPORT)
      if (!Common_GL_Image_Tools::loadSTB (path_in,
                                           flipImage_in,
                                           width, height,
                                           channels,
                                           image_p))
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to Common_GL_Image_Tools::loadSTB(\"%s\"), aborting\n"),
                    ACE_TEXT (path_in.c_str ())));
        return 0;
      } // end IF
      has_alpha = channels >= 4;
#endif // STB_IMAGE_SUPPORT
      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown image file format (path was: \"%s\"), aborting\n"),
                  ACE_TEXT (path_in.c_str ())));
      return 0;
    }
  } // end SWITCH
  ACE_ASSERT (width && height && image_p);

  glGenTextures (1, &return_value);

  switch (channels)
  {
    case 1:
      internal_format = GL_R8;
      format = GL_R;
      break;
    case 2:
      internal_format = GL_RG8;
      format = GL_RG;
      break;
    case 3:
      ACE_ASSERT (!has_alpha);
      internal_format = GL_RGB8;
      format = GL_RGB;
      break;
    case 4:
      internal_format = GL_RGBA8;
      format = GL_RGBA;
      break;
    default:
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid number of channels (%d), aborting\n"),
                  channels));
      glDeleteTextures (1, &return_value);
      return 0;
  } // end SWITCH

  glBindTexture (GL_TEXTURE_2D, return_value);
  glTexImage2D (GL_TEXTURE_2D, 0, internal_format, width, height, 0, format, GL_UNSIGNED_BYTE, image_p);
  glBindTexture (GL_TEXTURE_2D, 0);

  // clean up
  free (image_p); image_p = NULL;

  return return_value;
}

GLuint
Common_GL_Tools::loadCubeMap (const std::string& negativeZ_in,
                              const std::string& positiveZ_in,
                              const std::string& positiveY_in,
                              const std::string& negativeY_in,
                              const std::string& negativeX_in,
                              const std::string& positiveX_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_GL_Tools::loadCubeMap"));

  GLuint return_value = 0;

  glGenTextures (1, &return_value);
  ACE_ASSERT (return_value);

  glBindTexture (GL_TEXTURE_CUBE_MAP, return_value);

  unsigned int width = 0, height = 0, channels = 0;
  bool has_alpha = false;
  GLubyte* image_p = NULL;
  GLint internal_format;
  GLenum format;
  std::vector<std::string> files_a = { negativeZ_in, positiveZ_in,
                                       positiveY_in, negativeY_in,
                                       negativeX_in, positiveX_in };
  static std::vector<GLenum> orientation_a =
    { GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
      GL_TEXTURE_CUBE_MAP_POSITIVE_Y, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
      GL_TEXTURE_CUBE_MAP_NEGATIVE_X, GL_TEXTURE_CUBE_MAP_POSITIVE_X };
  std::vector<GLenum>::const_iterator orientation = orientation_a.begin ();

  for (std::vector<std::string>::const_iterator iterator = files_a.begin ();
       iterator != files_a.end ();
       ++iterator, ++orientation)
  {
    width = 0, height = 0, channels = 0;
    has_alpha = false;

    switch (Common_Image_Tools::fileExtensionToType (*iterator))
    {
      case COMMON_IMAGE_FILE_PNG:
      {
#if defined (LIBPNG_SUPPORT)
        if (!Common_GL_Image_Tools::loadPNG (*iterator,
                                             width, height,
                                             channels,
                                             has_alpha,
                                             image_p))
#elif defined (IMAGEMAGICK_SUPPORT)
        if (!Common_GL_Image_Tools::loadPNG (*iterator,
                                             width, height,
                                             image_p))
#endif // LIBPNG_SUPPORT || IMAGEMAGICK_SUPPORT
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to Common_GL_Image_Tools::loadPNG(\"%s\"), aborting\n"),
                      ACE_TEXT ((*iterator).c_str ())));
          glBindTexture (GL_TEXTURE_CUBE_MAP, 0);
          glDeleteTextures (1, &return_value);
          return 0;
        } // end IF
#if defined (LIBPNG_SUPPORT)
#elif defined (IMAGEMAGICK_SUPPORT)
        channels = 4;
        has_alpha = true;
#endif // LIBPNG_SUPPORT || IMAGEMAGICK_SUPPORT
        break;
      }
      case COMMON_IMAGE_FILE_GIF:
      case COMMON_IMAGE_FILE_JPG:
      {
#if defined (STB_IMAGE_SUPPORT)
        if (!Common_GL_Image_Tools::loadSTB (*iterator,
                                             false, // don't flip
                                             width, height,
                                             channels,
                                             image_p))
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to Common_GL_Image_Tools::loadSTB(\"%s\"), aborting\n"),
                      ACE_TEXT ((*iterator).c_str ())));
          glBindTexture (GL_TEXTURE_CUBE_MAP, 0);
          glDeleteTextures (1, &return_value);
          return 0;
        } // end IF
        has_alpha = channels >= 4;
#else
        ACE_UNUSED_ARG (channels);
#endif // STB_IMAGE_SUPPORT
        break;
      }
      default:
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("invalid/unknown image file format (path was: \"%s\"), aborting\n"),
                    ACE_TEXT ((*iterator).c_str ())));
        glBindTexture (GL_TEXTURE_CUBE_MAP, 0);
        glDeleteTextures (1, &return_value);
        return 0;
      }
    } // end SWITCH
    ACE_ASSERT (width && height && image_p);

    switch (channels)
    {
      case 1:
        internal_format = GL_R8;
        format = GL_R;
        break;
      case 2:
        internal_format = GL_RG8;
        format = GL_RG;
        break;
      case 3:
        ACE_ASSERT (!has_alpha);
        internal_format = GL_RGB8;
        format = GL_RGB;
        break;
      case 4:
        internal_format = GL_RGBA8;
        format = GL_RGBA;
        break;
      default:
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("invalid number of channels (%d), aborting\n"),
                    channels));
        glBindTexture (GL_TEXTURE_CUBE_MAP, 0);
        glDeleteTextures (1, &return_value);
        return 0;
    } // end SWITCH

    glTexImage2D (*orientation, 0, internal_format, width, height, 0, format, GL_UNSIGNED_BYTE, image_p);

    // clean up
    free (image_p); image_p = NULL;
  } // end FOR

  glBindTexture (GL_TEXTURE_CUBE_MAP, 0);

  return return_value;
}

void
Common_GL_Tools::loadTexture (const uint8_t* data_in,
                              unsigned int width_in,
                              unsigned int height_in,
                              unsigned int depth_in,
                              GLuint textureIndex_in,
                              bool isFirst_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_GL_Tools::loadTexture"));

  // sanity check(s)
  ACE_ASSERT (depth_in == 3 || depth_in == 4);

  glBindTexture (GL_TEXTURE_2D, textureIndex_in);

  if (isFirst_in)
  {
    glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA8, width_in, height_in, 0,
                  (depth_in == 4 ? GL_RGBA : GL_RGB),
                  GL_UNSIGNED_BYTE, data_in);
  } // end IF
  else
  {
    glTexSubImage2D (GL_TEXTURE_2D, 0, 0, 0, width_in, height_in,
                     (depth_in == 4 ? GL_RGBA : GL_RGB),
                     GL_UNSIGNED_BYTE, data_in);
    //glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA8, width_in, height_in, 0,
    //              (depth_in == 4 ? GL_RGBA : GL_RGB),
    //              GL_UNSIGNED_BYTE, data_in);
  } // end ELSE

  glBindTexture (GL_TEXTURE_2D, 0);
}

bool
Common_GL_Tools::loadAndCompileShaderFile (const std::string& path_in,
                                           GLenum type_in,
                                           GLuint& id_out)
{
  COMMON_TRACE (ACE_TEXT ("Common_GL_Tools::loadAndCompileShaderFile"));

  // initialize return value(s)
  id_out = -1;

  id_out = glCreateShader (type_in);

  uint8_t* data_p = NULL;
  ACE_UINT64 file_size_i = 0;
  if (unlikely (!Common_File_Tools::load (path_in,
                                          data_p,
                                          file_size_i,
                                          0)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to load \"%s\", aborting\n"),
                ACE_TEXT (path_in.c_str ())));
    glDeleteShader (id_out); id_out = -1;
    return false;
  } // end IF

  GLchar* array_a[2];
  array_a[0] = reinterpret_cast<GLchar*> (data_p);
  array_a[1] = NULL;
  GLint array_2[2];
  array_2[0] = static_cast<GLint> (file_size_i);
  array_2[1] = static_cast<GLint> (NULL);
  glShaderSource (id_out, 1, array_a, array_2);
  delete [] data_p; data_p = NULL;

  glCompileShader (id_out);
  GLint success = 0;
  glGetShaderiv (id_out, GL_COMPILE_STATUS, &success);
  if (unlikely (success == GL_FALSE))
  {
    GLchar info_log_a[BUFSIZ * 4];
    GLsizei buf_size_i = 0;
    glGetShaderInfoLog (id_out,
                        sizeof (GLchar) * BUFSIZ * 4,
                        &buf_size_i,
                        info_log_a);
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to compile \"%s\": \"%s\", aborting\n"),
                ACE_TEXT (Common_File_Tools::basename (path_in, false).c_str ()),
                ACE_TEXT (info_log_a)));
    glDeleteShader (id_out); id_out = -1;
    return false;
  } // end IF

  return true;
}

bool
Common_GL_Tools::loadAndCompileShaderString (const std::string& shaderCode_in,
                                             GLenum type_in,
                                             GLuint& id_out)
{
  COMMON_TRACE (ACE_TEXT ("Common_GL_Tools::loadAndCompileShaderString"));

  // initialize return value(s)
  id_out = -1;

  id_out = glCreateShader (type_in);

  GLchar* array_a[2];
  array_a[0] =
    static_cast<GLchar*> (const_cast<char*> (shaderCode_in.c_str ()));
  array_a[1] = NULL;
  GLint array_2[2];
  array_2[0] = static_cast<GLint> (shaderCode_in.size ());
  array_2[1] = static_cast<GLint> (NULL);
  glShaderSource (id_out, 1, array_a, array_2);

  glCompileShader (id_out);
  GLint success = 0;
  glGetShaderiv (id_out, GL_COMPILE_STATUS, &success);
  if (unlikely (success == GL_FALSE))
  {
    GLchar info_log_a[BUFSIZ * 4];
    GLsizei buf_size_i = 0;
    glGetShaderInfoLog (id_out,
                        sizeof (GLchar) * BUFSIZ * 4,
                        &buf_size_i,
                        info_log_a);
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to compile %s shader: \"%s\", aborting\n"),
                ((type_in == GL_VERTEX_SHADER) ? ACE_TEXT ("vertex") : ACE_TEXT ("fragment")),
                ACE_TEXT (info_log_a)));
    glDeleteShader (id_out); id_out = -1;
    return false;
  } // end IF

  return true;
}

void
Common_GL_Tools::drawCube (bool setTextureCoordinates_in,
                           bool setColor_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_GL_Tools::drawCube"));

  glBegin (GL_QUADS);

  // Front Face
  if (setTextureCoordinates_in)
    glTexCoord2f (0.0f, 0.0f);
  else if (setColor_in)
    glColor3f (1.0f, 0.0f, 0.0f);
  glVertex3f (-0.5f, -0.5f, 0.5f); // Bottom Left Of The Texture and Quad
  if (setTextureCoordinates_in)
    glTexCoord2f (1.0f, 0.0f);
  glVertex3f (0.5f, -0.5f, 0.5f); // Bottom Right Of The Texture and Quad
  if (setTextureCoordinates_in)
    glTexCoord2f (1.0f, 1.0f);
  glVertex3f (0.5f, 0.5f, 0.5f); // Top Right Of The Texture and Quad
  if (setTextureCoordinates_in)
    glTexCoord2f (0.0f, 1.0f);
  glVertex3f (-0.5f, 0.5f, 0.5f); // Top Left Of The Texture and Quad

  // Back Face
  if (setTextureCoordinates_in)
    glTexCoord2f (1.0f, 0.0f);
  else if (setColor_in)
    glColor3f (0.0f, 1.0f, 0.0f);
  glVertex3f (-0.5f, -0.5f, -0.5f); // Bottom Right Of The Texture and Quad
  if (setTextureCoordinates_in)
    glTexCoord2f (1.0f, 1.0f);
  glVertex3f (-0.5f, 0.5f, -0.5f); // Top Right Of The Texture and Quad
  if (setTextureCoordinates_in)
    glTexCoord2f (0.0f, 1.0f);
  glVertex3f (0.5f, 0.5f, -0.5f); // Top Left Of The Texture and Quad
  if (setTextureCoordinates_in)
    glTexCoord2f (0.0f, 0.0f);
  glVertex3f (0.5f, -0.5f, -0.5f); // Bottom Left Of The Texture and Quad

  // Top Face
  if (setTextureCoordinates_in)
    glTexCoord2f (0.0f, 1.0f);
  else if (setColor_in)
    glColor3f (0.0f, 0.0f, 1.0f);
  glVertex3f (-0.5f, 0.5f, -0.5f); // Top Left Of The Texture and Quad
  if (setTextureCoordinates_in)
    glTexCoord2f (0.0f, 0.0f);
  glVertex3f (-0.5f, 0.5f, 0.5f); // Bottom Left Of The Texture and Quad
  if (setTextureCoordinates_in)
    glTexCoord2f (1.0f, 0.0f);
  glVertex3f (0.5f, 0.5f, 0.5f); // Bottom Right Of The Texture and Quad
  if (setTextureCoordinates_in)
    glTexCoord2f (1.0f, 1.0f);
  glVertex3f (0.5f, 0.5f, -0.5f); // Top Right Of The Texture and Quad

  // Bottom Face
  if (setTextureCoordinates_in)
    glTexCoord2f (1.0f, 1.0f);
  else if (setColor_in)
    glColor3f (1.0f, 1.0f, 0.0f);
  glVertex3f (-0.5f, -0.5f, -0.5f); // Top Right Of The Texture and Quad
  if (setTextureCoordinates_in)
    glTexCoord2f (0.0f, 1.0f);
  glVertex3f (0.5f, -0.5f, -0.5f); // Top Left Of The Texture and Quad
  if (setTextureCoordinates_in)
    glTexCoord2f (0.0f, 0.0f);
  glVertex3f (0.5f, -0.5f, 0.5f); // Bottom Left Of The Texture and Quad
  if (setTextureCoordinates_in)
    glTexCoord2f (1.0f, 0.0f);
  glVertex3f (-0.5f, -0.5f, 0.5f); // Bottom Right Of The Texture and Quad

  // Right face
  if (setTextureCoordinates_in)
    glTexCoord2f (1.0f, 0.0f);
  else if (setColor_in)
    glColor3f (1.0f, 0.0f, 1.0f);
  glVertex3f (0.5f, -0.5f, -0.5f); // Bottom Right Of The Texture and Quad
  if (setTextureCoordinates_in)
    glTexCoord2f (1.0f, 1.0f);
  glVertex3f (0.5f, 0.5f, -0.5f); // Top Right Of The Texture and Quad
  if (setTextureCoordinates_in)
    glTexCoord2f (0.0f, 1.0f);
  glVertex3f (0.5f, 0.5f, 0.5f); // Top Left Of The Texture and Quad
  if (setTextureCoordinates_in)
    glTexCoord2f (0.0f, 0.0f);
  glVertex3f (0.5f, -0.5f, 0.5f); // Bottom Left Of The Texture and Quad

  // Left Face
  if (setTextureCoordinates_in)
    glTexCoord2f (0.0f, 0.0f);
  else if (setColor_in)
    glColor3f (1.0f, 1.0f, 1.0f);
  glVertex3f (-0.5f, -0.5f, -0.5f); // Bottom Left Of The Texture and Quad
  if (setTextureCoordinates_in)
    glTexCoord2f (1.0f, 0.0f);
  glVertex3f (-0.5f, -0.5f, 0.5f); // Bottom Right Of The Texture and Quad
  if (setTextureCoordinates_in)
    glTexCoord2f (1.0f, 1.0f);
  glVertex3f (-0.5f, 0.5f, 0.5f); // Top Right Of The Texture and Quad
  if (setTextureCoordinates_in)
    glTexCoord2f (0.0f, 1.0f);
  glVertex3f (-0.5f, 0.5f, -0.5f); // Top Left Of The Texture and Quad

  glEnd ();
}

void
Common_GL_Tools::screenShot (const std::string& path_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_GL_Tools::screenShot"));

  GLint viewport_a[4];
  glGetIntegerv (GL_VIEWPORT, viewport_a);

  GLubyte* data_p = NULL;
  ACE_NEW_NORETURN (data_p,
                    GLubyte[viewport_a[2] * viewport_a[3] * 4]);
  ACE_ASSERT (data_p);
  
  glReadPixels (viewport_a[0], viewport_a[1], viewport_a[2], viewport_a[3], GL_RGBA, GL_UNSIGNED_BYTE, data_p);

#if defined (STB_IMAGE_SUPPORT)
  if (unlikely (!Common_GL_Image_Tools::saveSTB (path_in,
                                                 static_cast<unsigned int> (viewport_a[2]),
                                                 static_cast<unsigned int> (viewport_a[3]),
                                                 4,
                                                 data_p)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_GL_Image_Tools::saveSTB(\"%s\"), returning\n"),
                ACE_TEXT (path_in.c_str ())));
    delete [] data_p;
    return;
  } // end IF
#elif defined (LIBPNG_SUPPORT)
  ACE_ASSERT (!ACE_OS::strcmp (Common_String_Tools::tolower (Common_File_Tools::fileExtension (path_in, false)).c_str (), ACE_TEXT_ALWAYS_CHAR ("png")));

  if (unlikely (!Common_GL_Image_Tools::savePNG (path_in,
                                                 static_cast<unsigned int> (viewport_a[2]),
                                                 static_cast<unsigned int> (viewport_a[3]),
                                                 4,
                                                 data_p)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_GL_Image_Tools::savePNG(\"%s\"), returning\n"),
                ACE_TEXT (path_in.c_str ())));
    delete [] data_p;
    return;
  } // end IF
#else
#error no STB image|libpng support, aborting
#endif // STB_IMAGE_SUPPORT|LIBPNG_SUPPORT

  delete [] data_p;
}
