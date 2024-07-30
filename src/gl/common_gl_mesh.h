#ifndef COMMON_GL_MESH_H
#define COMMON_GL_MESH_H

#include <string>
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

#include "ace/Global_Macros.h"

#include "common_gl_camera.h"
#include "common_gl_common.h"
#include "common_gl_shader.h"
#include "common_gl_texture.h"
#include "common_gl_vao_vbo_ebo.h"

#if defined (ASSIMP_SUPPORT)
struct aiMesh;
struct aiScene;
#endif // ASSIMP_SUPPORT

class Common_GL_Mesh
{
 public:
#if defined (ASSIMP_SUPPORT)
  static Common_GL_Mesh load (const std::string&, // FQ scene path
                              unsigned int = 0);  // mesh index

  Common_GL_Mesh (const struct aiMesh&); // mesh handle
  // load material(s) (i.e. --> texture(s))
  bool init (const std::string&,     // FQ scene path
             const struct aiScene&); // scene handle
#endif // ASSIMP_SUPPORT
  Common_GL_Mesh ();
  Common_GL_Mesh (const std::string&); // filename
  inline ~Common_GL_Mesh () {}

  void render (Common_GL_Shader&,
               Common_GL_Camera&,
               struct Common_GL_Perspective&, // perspective information
               glm::mat4 = glm::mat4 (1.0f), // model matrix
               glm::vec3 = glm::vec3 (0.0f, 0.0f, 0.0f), // translation
               glm::quat = glm::quat (1.0f, 0.0f, 0.0f, 0.0f), // rotation
               glm::vec3 = glm::vec3 (1.0f, 1.0f, 1.0f)); // scale
  void reset ();

  std::vector <struct Common_GL_Vertex> vertices_;
  std::vector <GLuint>                  indices_;
  std::vector <Common_GL_Texture>       textures_;
  struct Common_GL_VAO                  VAO_;
  GLenum                                primitiveType_;

 private:
  // ACE_UNIMPLEMENTED_FUNC (Common_GL_Mesh ())
  // ACE_UNIMPLEMENTED_FUNC (Common_GL_Mesh (const Common_GL_Mesh&))
  // ACE_UNIMPLEMENTED_FUNC (Common_GL_Mesh& operator= (const Common_GL_Mesh&))
};

#endif // COMMON_GL_MESH_H
