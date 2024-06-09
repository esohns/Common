#ifndef COMMON_GL_TEXTURE_H
#define COMMON_GL_TEXTURE_H

#include <string>

#if defined (GLEW_SUPPORT)
#include "GL/glew.h"
#endif // GLEW_SUPPORT
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include "gl/GL.h"
#else
#include "GL/gl.h"
#endif // ACE_WIN32 || ACE_WIN64

#include "ace/Global_Macros.h"

class Common_GL_Texture
{
 public:
  Common_GL_Texture ();
  Common_GL_Texture (const std::string&); // filename
  ~Common_GL_Texture ();

  bool load (const std::string&); // filename
  void reset ();
  void bind (GLuint = 0); // texture unit
  void unbind ();

  GLuint id_;

 private:
  ACE_UNIMPLEMENTED_FUNC (Common_GL_Texture (const Common_GL_Texture&))
  ACE_UNIMPLEMENTED_FUNC (Common_GL_Texture& operator= (const Common_GL_Texture&))
};

#endif // COMMON_GL_TEXTURE_H
