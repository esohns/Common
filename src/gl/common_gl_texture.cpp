#include "common_gl_texture.h"

#include "ace/Log_Msg.h"

#include "common_macros.h"

#include "common_gl_tools.h"

Common_GL_Texture::Common_GL_Texture ()
 : id_ (0)
{
  COMMON_TRACE (ACE_TEXT ("Common_GL_Texture::Common_GL_Texture"));

}

Common_GL_Texture::Common_GL_Texture (const std::string& fileName_in)
 : id_ (Common_GL_Tools::loadTexture (fileName_in))
{
  COMMON_TRACE (ACE_TEXT ("Common_GL_Texture::Common_GL_Texture"));

  if (unlikely (!id_))
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

  if (likely (id_))
  {
    glDeleteTextures (1, &id_);
#if defined (_DEBUG)
    COMMON_GL_ASSERT;
#endif // _DEBUG
  } // end IF
}

bool
Common_GL_Texture::load (const std::string& fileName_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_GL_Texture::load"));

  // sanity check(s)
  ACE_ASSERT (!id_);

  id_ = Common_GL_Tools::loadTexture (fileName_in);
  if (unlikely (!id_))
  {
   ACE_DEBUG ((LM_ERROR,
               ACE_TEXT ("failed to Common_GL_Tools::loadTexture(\"%s\"), aborting\n"),
               ACE_TEXT (fileName_in.c_str ())));
   return false;
  } // end IF

  return true;
}

void
Common_GL_Texture::bind (unsigned int unit_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_GL_Texture::bind"));

  // sanity check(s)
  ACE_ASSERT (unit_in < 31);

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
  glActiveTexture (GL_TEXTURE0 + unit_in);
  COMMON_GL_ASSERT;
#endif // ACE_WIN32 || ACE_WIN64
  glBindTexture (GL_TEXTURE_2D, id_);
  COMMON_GL_ASSERT;
}
