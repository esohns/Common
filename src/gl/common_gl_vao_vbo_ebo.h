#ifndef COMMON_GL_VAO_VBO_EBO_H
#define COMMON_GL_VAO_VBO_EBO_H

#include <vector>

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
#endif // GLM_SUPPORT

#include "ace/Assert.h"

// *TODO*: move this away ASAP; it's not generic
struct Common_GL_Vertex
{
  Common_GL_Vertex ()
   : position ()
   , normal ()
   , color ()
   , uv ()
  {}

#if defined (GLM_SUPPORT)
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec4 color;
  glm::vec2 uv; // texture-
#else
  struct Common_GL_VectorF3 position;
  struct Common_GL_VectorF3 normal;
  struct Common_GL_Color    color;
  struct Common_GL_VectorF2 uv; // texture-
#endif // GLM_SUPPORT
};

struct Common_GL_VBO
{
  Common_GL_VBO (bool insideGLContext = false)
   : id_ (0)
  {
#if defined (GLEW_SUPPORT)
    if (insideGLContext)
    {
      glGenBuffers (1, &id_);
      ACE_ASSERT (id_);
    } // end IF
#endif // GLEW_SUPPORT
  }
  
  // *TODO*: move this away ASAP; it's not generic
  void upload (const std::vector<struct Common_GL_Vertex>& vertices_in)
  {
    bind ();
#if defined (GLEW_SUPPORT)
    glBufferData (GL_ARRAY_BUFFER, vertices_in.size () * sizeof (struct Common_GL_Vertex), vertices_in.data (), GL_STATIC_DRAW);
#endif // GLEW_SUPPORT
    unbind ();
  }

  inline void bind ()
  { ACE_ASSERT (id_);
#if defined (GLEW_SUPPORT)
    glBindBuffer (GL_ARRAY_BUFFER, id_);
#endif // GLEW_SUPPORT
  }
  inline void unbind ()
  {
#if defined (GLEW_SUPPORT)
    glBindBuffer (GL_ARRAY_BUFFER, 0);
#endif // GLEW_SUPPORT
  }
  inline void free ()
  {
#if defined (GLEW_SUPPORT)
    glDeleteBuffers (1, &id_);
#endif // GLEW_SUPPORT
    id_ = 0;
  }

  GLuint id_;
};

//////////////////////////////////////////

struct Common_GL_EBO
{
  Common_GL_EBO (bool insideGLContext = false)
   : id_ (0)
  {
#if defined (GLEW_SUPPORT)
    if (insideGLContext)
    {
      glGenBuffers (1, &id_);
      ACE_ASSERT (id_);
    } // end IF
#endif // GLEW_SUPPORT
  }

  // *TODO*: move this away ASAP; it's not generic
  void upload (const std::vector<GLuint>& indices_in)
  {
    bind ();
#if defined (GLEW_SUPPORT)
    glBufferData (GL_ELEMENT_ARRAY_BUFFER, indices_in.size () * sizeof (GLuint), indices_in.data (), GL_STATIC_DRAW);
#endif // GLEW_SUPPORT
    unbind ();
  }

  inline void bind ()
  { ACE_ASSERT (id_);
#if defined (GLEW_SUPPORT)
    glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, id_);
#endif // GLEW_SUPPORT
  }
  inline void unbind ()
  {
#if defined (GLEW_SUPPORT)
    glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, 0);
#endif // GLEW_SUPPORT
  }
  inline void free ()
  {
#if defined (GLEW_SUPPORT)
    glDeleteBuffers (1, &id_);
#endif // GLEW_SUPPORT
    id_ = 0;
  }

  GLuint id_;
};

//////////////////////////////////////////

struct Common_GL_VAO
{
  Common_GL_VAO (bool insideGLContext = false)
   : id_ (0)
  {
#if defined (GLEW_SUPPORT)
    if (insideGLContext)
    {
      glGenVertexArrays (1, &id_);
      ACE_ASSERT (id_);
    } // end IF
#endif // GLEW_SUPPORT
  }

  void linkAttrib (struct Common_GL_VBO& VBO,
                   GLuint index, GLint size, GLenum type, GLsizei stride, const void* pointer)
  {
    VBO.bind ();
#if defined (GLEW_SUPPORT)
    glVertexAttribPointer (index, size, type, GL_FALSE, stride, pointer);
    glEnableVertexAttribArray (index);
#endif // GLEW_SUPPORT
    VBO.unbind ();
  }

  inline void bind ()
  { ACE_ASSERT (id_);
#if defined (GLEW_SUPPORT)
    glBindVertexArray (id_);
#endif // GLEW_SUPPORT
  }
  inline void unbind ()
  {
#if defined (GLEW_SUPPORT)
    glBindVertexArray (0);
#endif // GLEW_SUPPORT
  }
  inline void free ()
  {
#if defined (GLEW_SUPPORT)
    glDeleteVertexArrays (1, &id_);
#endif // GLEW_SUPPORT
    id_ = 0;
  }

  GLuint id_;
};

#endif // COMMON_GL_VAO_VBO_EBO_H
