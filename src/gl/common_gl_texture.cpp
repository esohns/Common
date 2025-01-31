#include "stdafx.h"

#include "common_gl_texture.h"

#include "ace/Log_Msg.h"

#include "common_macros.h"

#include "common_gl_defines.h"
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
Common_GL_Texture::bind (GLuint unit_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_GL_Texture::bind"));

  // sanity check(s)
  ACE_ASSERT (id_);
  ACE_ASSERT (unit_in < 31);

  //glActiveTexture (GL_TEXTURE0 + unit_in);

  glBindTexture (GL_TEXTURE_2D, id_);
}

void
Common_GL_Texture::unbind ()
{
  COMMON_TRACE (ACE_TEXT ("Common_GL_Texture::unbind"));

  glBindTexture (GL_TEXTURE_2D, 0);
}

void
Common_GL_Texture::set (Common_GL_Shader& shader_in,
                        const std::string& uniform_in,
                        GLint unit_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_GL_Texture::set"));

  GLint texUniformLocation_i =
    glGetUniformLocation (shader_in.id_, uniform_in.c_str ());
  // sanity check(s)
  ACE_ASSERT (texUniformLocation_i);

  glProgramUniform1i (shader_in.id_, texUniformLocation_i, unit_in);
}
