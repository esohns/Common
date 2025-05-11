#include "stdafx.h"

#include "common_gl_texture.h"

#include "ace/Log_Msg.h"
#include "ace/Malloc.h"

#include "common_macros.h"

#include "common_gl_defines.h"
#include "common_gl_image_tools.h"
#include "common_gl_tools.h"

Common_GL_Texture::Common_GL_Texture ()
 : id_ (0)
 , path_ ()
 , type_ (Type::TYPE_INVALID)
{
  COMMON_TRACE (ACE_TEXT ("Common_GL_Texture::Common_GL_Texture"));

}

//Common_GL_Texture::Common_GL_Texture (enum Type type_in)
// : id_ (0)
// , path_ ()
// , type_ (type_in)
//{
//  COMMON_TRACE (ACE_TEXT ("Common_GL_Texture::Common_GL_Texture"));
//
//}

Common_GL_Texture::Common_GL_Texture (const std::string& path_in,
                                      enum Type type_in)
 : id_ (Common_GL_Tools::loadTexture (path_in, true))
 , path_ (id_ ? path_in : ACE_TEXT_ALWAYS_CHAR (""))
 , type_ (id_ ? type_in : Type::TYPE_INVALID)
{
  COMMON_TRACE (ACE_TEXT ("Common_GL_Texture::Common_GL_Texture"));

  if (unlikely (!id_))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_GL_Tools::loadTexture(\"%s\"), returning\n"),
                ACE_TEXT (path_in.c_str ())));
    return;
  } // end IF
}

Common_GL_Texture::~Common_GL_Texture ()
{
  COMMON_TRACE (ACE_TEXT ("Common_GL_Texture::~Common_GL_Texture"));

   if (unlikely (id_))
   { // *TODO*
     ACE_DEBUG ((LM_WARNING,
                 ACE_TEXT ("cannot free texture resources when out of context, continuing\n")));
     //glDeleteTextures (1, &id_);
   } // end IF
}

bool
Common_GL_Texture::load (GLint internalFormat_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_GL_Texture::load"));

  // sanity check(s)
  if (unlikely (!id_))
  {
    glGenTextures (1, &id_);
  } // end IF
  ACE_ASSERT (id_);

  GLint viewport_a[4];
  glGetIntegerv (GL_VIEWPORT, viewport_a);

  bind ();

  glCopyTexImage2D (GL_TEXTURE_2D, 0, internalFormat_in, viewport_a[0], viewport_a[1], viewport_a[2], viewport_a[3], 0);
  //GLfloat* pixels_p = NULL;
  //ACE_NEW_NORETURN (pixels_p,
  //                  GLfloat[4 * viewport_a[2] * viewport_a[3]]);
  //ACE_ASSERT (pixels_p);

  //glReadPixels (0, 0, viewport_a[2], viewport_a[3], GL_RGBA, GL_FLOAT, pixels_p);
  //glTexImage2D (GL_TEXTURE_2D, 0, internalFormat_in, viewport_a[2], viewport_a[3], 0, GL_RGBA, GL_FLOAT, pixels_p);
  //delete [] pixels_p;

  unbind ();

  return true;
}

bool
Common_GL_Texture::load (uint8_t* data_in,
                         const Common_Image_Resolution_t& resolution_in,
                         unsigned int depth_in,
                         bool update_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_GL_Texture::load"));

  // sanity check(s)
  if (unlikely (!id_))
  {
    glGenTextures (1, &id_);
  } // end IF
  ACE_ASSERT (id_);

  Common_GL_Tools::loadTexture (data_in,
#if defined (ACE_WIN32) || defined (ACE_WIN64)
                                resolution_in.cx,
                                resolution_in.cy,
#else
                                resolution_in.width,
                                resolution_in.height,
#endif // ACE_WIN32 || ACE_WIN64
                                depth_in,
                                id_,
                                !update_in);

  return true;
}

bool
Common_GL_Texture::load (const std::string& path_in,
                         bool flipImage_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_GL_Texture::load"));

  // sanity check(s)
  if (unlikely (id_))
    reset ();
  ACE_ASSERT (!id_);

  id_ = Common_GL_Tools::loadTexture (path_in,
                                      flipImage_in);
  if (unlikely (!id_))
  {
   ACE_DEBUG ((LM_ERROR,
               ACE_TEXT ("failed to Common_GL_Tools::loadTexture(\"%s\"), aborting\n"),
               ACE_TEXT (path_in.c_str ())));
   return false;
  } // end IF
  path_ = path_in;

  return true;
}

bool
Common_GL_Texture::load (const std::string& negativeZ_in,
                         const std::string& positiveZ_in,
                         const std::string& positiveY_in,
                         const std::string& negativeY_in,
                         const std::string& negativeX_in,
                         const std::string& positiveX_in)
{
  // sanity check(s)
  if (unlikely (id_))
    reset ();
  ACE_ASSERT (!id_);

  id_ = Common_GL_Tools::loadCubeMap (negativeZ_in,
                                      positiveZ_in,
                                      positiveY_in,
                                      negativeY_in,
                                      negativeX_in,
                                      positiveX_in);
  if (unlikely (!id_))
  {
   ACE_DEBUG ((LM_ERROR,
               ACE_TEXT ("failed to Common_GL_Tools::loadCubeMap(\"%s\"), aborting\n"),
               ACE_TEXT (negativeZ_in.c_str ())));
   return false;
  } // end IF
  path_ = negativeZ_in;

  return true;
}

bool
Common_GL_Texture::save (const std::string& path_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_GL_Texture::save"));

  bind ();

  int width_i, height_i;
  glGetTexLevelParameteriv (GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width_i);
  glGetTexLevelParameteriv (GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height_i);

  GLubyte* data_p = NULL;
  ACE_NEW_NORETURN (data_p,
                    GLubyte[width_i * height_i * 4]);
  ACE_ASSERT (data_p);

  glGetTexImage (GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, data_p);

  unbind ();

#if defined (STB_IMAGE_SUPPORT)
  if (unlikely (!Common_GL_Image_Tools::saveSTB (path_in,
                                                 static_cast<unsigned int> (width_i),
                                                 static_cast<unsigned int> (height_i),
                                                 4,
                                                 data_p)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_GL_Image_Tools::saveSTB(\"%s\"), aborting\n"),
                ACE_TEXT (path_in.c_str ())));
    delete[] data_p;
    return false;
  } // end IF
#elif defined (LIBPNG_SUPPORT)
  ACE_ASSERT (!ACE_OS::strcmp (Common_String_Tools::tolower (Common_File_Tools::fileExtension (path_in, false)).c_str (), ACE_TEXT_ALWAYS_CHAR ("png")));

  if (unlikely (!Common_GL_Image_Tools::savePNG (path_in,
                                                 static_cast<unsigned int> (width_i),
                                                 static_cast<unsigned int> (height_i),
                                                 4,
                                                 data_p)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_GL_Image_Tools::savePNG(\"%s\"), aborting\n"),
                ACE_TEXT (path_in.c_str ())));
    delete[] data_p;
    return false;
  } // end IF
#else
#error no STB image|libpng support, aborting
#endif // STB_IMAGE_SUPPORT|LIBPNG_SUPPORT

  delete [] data_p;

  return true;
}

void
Common_GL_Texture::reset ()
{
  COMMON_TRACE (ACE_TEXT ("Common_GL_Texture::reset"));

  if (likely (id_))
  {
    glDeleteTextures (1, &id_); id_ = 0;
  } // end IF
}

void
Common_GL_Texture::bind ()
{
  COMMON_TRACE (ACE_TEXT ("Common_GL_Texture::bind"));

  // sanity check(s)
  ACE_ASSERT (id_);

  glBindTexture (GL_TEXTURE_2D, id_);
}

void
Common_GL_Texture::unbind ()
{
  COMMON_TRACE (ACE_TEXT ("Common_GL_Texture::unbind"));

  glBindTexture (GL_TEXTURE_2D, 0);
}

void
Common_GL_Texture::set (GLuint program_in,
                        const std::string& uniform_in,
                        GLint unit_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_GL_Texture::set"));

  GLint texUniformLocation_i =
    glGetUniformLocation (program_in, uniform_in.c_str ());
  // sanity check(s)
  ACE_ASSERT (texUniformLocation_i);

  glProgramUniform1i (program_in, texUniformLocation_i, unit_in);
}
