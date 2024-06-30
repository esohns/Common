#include "common_gl_texture.h"

#include "ace/Log_Msg.h"

#include "common_macros.h"

#include "common_gl_defines.h"
#include "common_gl_tools.h"

Common_GL_Texture::Common_GL_Texture ()
 : id_ (-1)
{
  COMMON_TRACE (ACE_TEXT ("Common_GL_Texture::Common_GL_Texture"));

}

Common_GL_Texture::Common_GL_Texture (const std::string& fileName_in)
 : id_ (Common_GL_Tools::loadTexture (fileName_in))
{
  COMMON_TRACE (ACE_TEXT ("Common_GL_Texture::Common_GL_Texture"));

  if (unlikely (id_ == static_cast<GLuint> (-1)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_GL_Tools::loadTexture(\"%s\"), returning\n"),
                ACE_TEXT (fileName_in.c_str ())));
    return;
  } // end IF
}

Common_GL_Texture::~Common_GL_Texture ()
{
  COMMON_TRACE (ACE_TEXT ("Common_GL_Texture::~Common_GL_Texture"));

  if (id_ != static_cast<GLuint> (-1))
  {
    glDeleteTextures (1, &id_);
    COMMON_GL_ASSERT;
  } // end IF
}

bool
Common_GL_Texture::load (const std::string& fileName_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_GL_Texture::load"));

  // sanity check(s)
  if (id_ != static_cast<GLuint> (-1))
    reset ();

  id_ = Common_GL_Tools::loadTexture (fileName_in);
  if (unlikely (id_ == static_cast<GLuint> (-1)))
  {
   ACE_DEBUG ((LM_ERROR,
               ACE_TEXT ("failed to Common_GL_Tools::loadTexture(\"%s\"), aborting\n"),
               ACE_TEXT (fileName_in.c_str ())));
   return false;
  } // end IF

  return true;
}

void
Common_GL_Texture::reset ()
{
  COMMON_TRACE (ACE_TEXT ("Common_GL_Texture::reset"));

  if (likely (id_ != static_cast<GLuint> (-1)))
  {
    glDeleteTextures (1, &id_); id_ = -1;
    COMMON_GL_ASSERT;
  } // end IF
}

void
Common_GL_Texture::bind (GLuint unit_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_GL_Texture::bind"));

  // sanity check(s)
  ACE_ASSERT (id_ != static_cast<GLuint> (-1));
  ACE_ASSERT (unit_in < 31);

  glActiveTexture (GL_TEXTURE0 + unit_in);
  COMMON_GL_ASSERT;

  glBindTexture (GL_TEXTURE_2D, id_);
  COMMON_GL_ASSERT;
}

void
Common_GL_Texture::unbind ()
{
  COMMON_TRACE (ACE_TEXT ("Common_GL_Texture::unbind"));

  glBindTexture (GL_TEXTURE_2D, 0);
  COMMON_GL_ASSERT;
}
