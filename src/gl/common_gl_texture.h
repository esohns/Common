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

#include "common_image_common.h"

#include "common_gl_shader.h"

class Common_GL_Texture
{
 public:
  enum Type
  {
    TYPE_AMBIENT = 0,
    TYPE_DIFFUSE,
    TYPE_SPECULAR,
    TYPE_INVALID
  };

  Common_GL_Texture ();
  //Common_GL_Texture (enum Type = Type::TYPE_INVALID);
  Common_GL_Texture (const std::string&, // FQ path
                     enum Type = Type::TYPE_INVALID);
  ~Common_GL_Texture ();

  bool load (uint8_t*,                         // data
             const Common_Image_Resolution_t&, // resolution
             unsigned int,                     // depth
             bool = true);                     // update ? : initial
  bool load (const std::string&, // FQ path
             bool = true);       // flip image on load ? (required for OpenGL)
  void reset ();
  void bind (GLuint = 0); // texture unit (!), not -id (!!!)
  void unbind ();

  void set (Common_GL_Shader&,  // shader
            const std::string&, // uniform
            GLint = 0);         // texture unit (!), not -id (!!!)

  GLuint      id_; // texture id
  std::string path_;
  enum Type   type_;

 private:
  // ACE_UNIMPLEMENTED_FUNC (Common_GL_Texture ())
  // ACE_UNIMPLEMENTED_FUNC (Common_GL_Texture (const Common_GL_Texture&))
  // ACE_UNIMPLEMENTED_FUNC (Common_GL_Texture& operator= (const Common_GL_Texture&))
};

#endif // COMMON_GL_TEXTURE_H
