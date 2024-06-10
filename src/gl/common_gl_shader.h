#ifndef COMMON_GL_SHADER_H
#define COMMON_GL_SHADER_H

#include <string>

#if defined (GLEW_SUPPORT)
#include "GL/glew.h"
#endif // GLEW_SUPPORT
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include "gl/GL.h"
#else
#include "GL/gl.h"
#endif // ACE_WIN32 || ACE_WIN64
#if defined (GLM_SUPPORT)
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#endif // GLM_SUPPORT

#include "ace/Global_Macros.h"

class Common_GL_Shader
{
 public:
  Common_GL_Shader ();
  Common_GL_Shader (const std::string&,  // vertex shader filename
                    const std::string&); // fragment shader filename
  ~Common_GL_Shader ();

  bool loadFromFile (const std::string&,  // vertex shader filename
                     const std::string&); // fragment shader filename
  bool loadFromString (const std::string&,  // vertex shader code
                       const std::string&); // fragment shader code
  void reset ();
  void use ();

  inline void setBool  (const std::string& name, bool value) const  { glUniform1i (glGetUniformLocation (id_, name.c_str ()), (int)value); }
  inline void setInt   (const std::string& name, int value)  const  { glUniform1i (glGetUniformLocation (id_, name.c_str ()), value); }
  inline void setFloat (const std::string& name, float value) const { glUniform1f (glGetUniformLocation (id_, name.c_str ()), value); }
#if defined (GLM_SUPPORT)
  inline void setMat4  (const std::string& name, const glm::mat4& mat) const { glUniformMatrix4fv (glGetUniformLocation (id_, name.c_str ()), 1, GL_FALSE, glm::value_ptr (mat)); }
#endif // GLM_SUPPORT

  GLuint id_;

 private:
  ACE_UNIMPLEMENTED_FUNC (Common_GL_Shader (const Common_GL_Shader&))
  ACE_UNIMPLEMENTED_FUNC (Common_GL_Shader& operator= (const Common_GL_Shader&))
};

#endif // COMMON_GL_SHADER_H
