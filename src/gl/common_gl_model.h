#ifndef COMMON_GL_MODEL_H
#define COMMON_GL_MODEL_H

#include <string>
#include <vector>

#if defined (GLM_SUPPORT)
#include "glm/glm.hpp"
#endif // GLM_SUPPORT

// #include "ace/Global_Macros.h"

#include "common_gl_camera.h"
#include "common_gl_common.h"
#include "common_gl_mesh.h"
#include "common_gl_shader.h"

// forward declarations
#if defined (ASSIMP_SUPPORT)
struct aiScene;
#endif // ASSIMP_SUPPORT

class Common_GL_Model
{
 public:
#if defined (ASSIMP_SUPPORT)
  static Common_GL_Model loadFromFile (const std::string&); // FQ scene path

  Common_GL_Model (const std::string&,     // FQ scene path
                   const struct aiScene&); // scene handle
#endif // ASSIMP_SUPPORT
  Common_GL_Model ();
  Common_GL_Model (const std::string&); // filename
  inline ~Common_GL_Model () {}

  void load (const std::string&); // filename
  void render (Common_GL_Shader&,
               Common_GL_Camera&,
               struct Common_GL_Perspective&, // perspective information
               glm::mat4 = glm::mat4 (1.0f), // model matrix
               glm::vec3 = glm::vec3 (0.0f, 0.0f, 0.0f), // translation
               glm::quat = glm::quat (1.0f, 0.0f, 0.0f, 0.0f), // rotation
               glm::vec3 = glm::vec3 (1.0f, 1.0f, 1.0f)); // scale
  void reset ();

  Common_GL_BoundingBox_t      box_;
  glm::vec3                    center_;
  std::vector <Common_GL_Mesh> meshes_;

 private:
  // ACE_UNIMPLEMENTED_FUNC (Common_GL_Model ())
  // ACE_UNIMPLEMENTED_FUNC (Common_GL_Model (const Common_GL_Model&))
  // ACE_UNIMPLEMENTED_FUNC (Common_GL_Model& operator= (const Common_GL_Model&))
};

#endif // Common_GL_Model_H
