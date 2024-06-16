#include "common_gl_shader.h"

#include "ace/Log_Msg.h"

#include "common_macros.h"

#include "common_gl_defines.h"
#include "common_gl_tools.h"

Common_GL_Shader::Common_GL_Shader ()
 : id_ (-1)
{
  COMMON_TRACE (ACE_TEXT ("Common_GL_Shader::Common_GL_Shader"));

}

Common_GL_Shader::Common_GL_Shader (const std::string& vertexShaderFilename_in,
                                    const std::string& fragmentShaderFilename_in)
 : id_ (-1)
{
  COMMON_TRACE (ACE_TEXT ("Common_GL_Shader::Common_GL_Shader"));

  if (unlikely (!loadFromFile (vertexShaderFilename_in,
                               fragmentShaderFilename_in)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_GL_Shader::loadFromFile(\"%s\",\"%s\"), returning\n"),
                ACE_TEXT (vertexShaderFilename_in.c_str ()),
                ACE_TEXT (fragmentShaderFilename_in.c_str ())));
    return;
  } // end IF
  ACE_ASSERT (id_ != -1);
}

Common_GL_Shader::~Common_GL_Shader ()
{
  COMMON_TRACE (ACE_TEXT ("Common_GL_Shader::~Common_GL_Shader"));

  if (likely (id_ != -1))
  {
    glDeleteProgram (id_);
    COMMON_GL_ASSERT;
  } // end IF
}

bool
Common_GL_Shader::loadFromFile (const std::string& vertexShaderFilename_in,
                                const std::string& fragmentShaderFilename_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_GL_Shader::loadFromFile"));

  // sanity check(s)
  if (id_ != -1)
    reset ();

  GLuint vertex_shader_id_i = -1, fragment_shader_id_i = -1;
  if (unlikely (!Common_GL_Tools::loadAndCompileShaderFile (vertexShaderFilename_in,
                                                            GL_VERTEX_SHADER,
                                                            vertex_shader_id_i)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_GL_Tools::loadAndCompileShaderFile(\"%s\"), aborting\n"),
                ACE_TEXT (vertexShaderFilename_in.c_str ())));
    return false;
  } // end IF
  if (unlikely (!Common_GL_Tools::loadAndCompileShaderFile (fragmentShaderFilename_in,
                                                            GL_FRAGMENT_SHADER,
                                                            fragment_shader_id_i)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_GL_Tools::loadAndCompileShaderFile(\"%s\"), aborting\n"),
                ACE_TEXT (fragmentShaderFilename_in.c_str ())));
    return false;
  } // end IF
  ACE_ASSERT (vertex_shader_id_i != -1 && fragment_shader_id_i != -1);

  id_ = glCreateProgram ();
  ACE_ASSERT (id_ != -1);
  glAttachShader (id_, vertex_shader_id_i);
  glAttachShader (id_, fragment_shader_id_i);
  glLinkProgram (id_);
  GLint success_i = 0;
  glGetProgramiv (id_, GL_LINK_STATUS, &success_i);
  if (success_i == GL_FALSE)
  {
    GLchar info_log_a[BUFSIZ * 4];
    GLsizei buf_size_i = 0;
    glGetProgramInfoLog (id_,
                         sizeof (GLchar[BUFSIZ * 4]),
                         &buf_size_i,
                         info_log_a);
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to link GL program: \"%s\", aborting\n"),
                ACE_TEXT (info_log_a)));

    glDetachShader (id_, vertex_shader_id_i);
    glDetachShader (id_, fragment_shader_id_i);
    glDeleteShader (vertex_shader_id_i);
    glDeleteShader (fragment_shader_id_i);
    glDeleteProgram (id_); id_ = -1;
    return false;
  } // end IF
  glDetachShader (id_, vertex_shader_id_i);
  glDetachShader (id_, fragment_shader_id_i);
  glDeleteShader (vertex_shader_id_i);
  glDeleteShader (fragment_shader_id_i);
  COMMON_GL_ASSERT;

  return true;
}

bool
Common_GL_Shader::loadFromString (const std::string& vertexShaderCode_in,
                                  const std::string& fragmentShaderCode_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_GL_Shader::loadFromString"));

  // sanity check(s)
  if (id_ != -1)
    reset ();

  GLuint vertex_shader_id_i = -1, fragment_shader_id_i = -1;
  if (unlikely (!Common_GL_Tools::loadAndCompileShaderString (vertexShaderCode_in,
                                                              GL_VERTEX_SHADER,
                                                              vertex_shader_id_i)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_GL_Tools::loadAndCompileShaderString(), aborting\n")));
    return false;
  } // end IF
  if (unlikely (!Common_GL_Tools::loadAndCompileShaderString (fragmentShaderCode_in,
                                                              GL_FRAGMENT_SHADER,
                                                              fragment_shader_id_i)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_GL_Tools::loadAndCompileShaderString(), aborting\n")));
    return false;
  } // end IF
  ACE_ASSERT (vertex_shader_id_i != -1 && fragment_shader_id_i != -1);

  id_ = glCreateProgram ();
  ACE_ASSERT (id_ != -1);
  glAttachShader (id_, vertex_shader_id_i);
  glAttachShader (id_, fragment_shader_id_i);
  glLinkProgram (id_);
  GLint success_i = 0;
  glGetProgramiv (id_, GL_LINK_STATUS, &success_i);
  if (success_i == GL_FALSE)
  {
    GLchar info_log_a[BUFSIZ * 4];
    GLsizei buf_size_i = 0;
    glGetProgramInfoLog (id_,
                         sizeof (GLchar[BUFSIZ * 4]),
                         &buf_size_i,
                         info_log_a);
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to link GL program: \"%s\", aborting\n"),
                ACE_TEXT (info_log_a)));

    glDetachShader (id_, vertex_shader_id_i);
    glDetachShader (id_, fragment_shader_id_i);
    glDeleteShader (vertex_shader_id_i);
    glDeleteShader (fragment_shader_id_i);
    glDeleteProgram (id_); id_ = -1;
    return false;
  } // end IF
  glDetachShader (id_, vertex_shader_id_i);
  glDetachShader (id_, fragment_shader_id_i);
  glDeleteShader (vertex_shader_id_i);
  glDeleteShader (fragment_shader_id_i);
  COMMON_GL_ASSERT;

  return true;
}

void
Common_GL_Shader::reset ()
{
  if (likely (id_ != -1))
  {
    glDeleteProgram (id_);
    COMMON_GL_ASSERT;
  } // end IF
}

void
Common_GL_Shader::use ()
{
  COMMON_TRACE (ACE_TEXT ("Common_GL_Shader::use"));

  // sanity check(s)
  ACE_ASSERT (id_ != -1);

  glUseProgram (id_);
  COMMON_GL_ASSERT;
}
