#include "stdafx.h"

#include "common_gl_model.h"

#if defined (ASSIMP_SUPPORT)
#include "assimp/cimport.h"
#include "assimp/material.h"
#include "assimp/mesh.h"
#include "assimp/scene.h"
#include "assimp/types.h"
#endif // ASSIMP_SUPPORT

#include "ace/Default_Constants.h"
#include "ace/Log_Msg.h"
#include "ace/OS_Memory.h"

#include "common_macros.h"
#include "common_file_tools.h"

#if defined (ASSIMP_SUPPORT)
#include "common_gl_assimp_tools.h"
#endif // ASSIMP_SUPPORT
#include "common_gl_common.h"

#if defined (ASSIMP_SUPPORT)
Common_GL_Model*
Common_GL_Model::loadFromFile (const std::string& path_in,
                               unsigned int flags_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_GL_Model::loadFromFile"));

  Common_GL_Model* return_value_p = NULL;

  struct aiScene* scene_p = NULL;
  if (!Common_GL_Assimp_Tools::loadModel (path_in,
                                          scene_p,
                                          flags_in))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_GL_Assimp_Tools::loadModel(\"%s\",%d), aborting\n"),
                ACE_TEXT (path_in.c_str ()),
                flags_in));
    return NULL;
  } // end IF
  ACE_ASSERT (scene_p);

  ACE_NEW_NORETURN (return_value_p,
                    Common_GL_Model (path_in, *scene_p));
  ACE_ASSERT (return_value_p);

  aiReleaseImport (scene_p); scene_p = NULL;

  return return_value_p;
}

Common_GL_Model::Common_GL_Model (const std::string& path_in,
                                  const struct aiScene& scene_in)
 : box_ ()
 , center_ ()
 , meshes_ ()
 , textures_ ()
{
  COMMON_TRACE (ACE_TEXT ("Common_GL_Model::Common_GL_Model"));

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("loading scene \"%s\"...\n"),
              ACE_TEXT (scene_in.mName.C_Str ())));

  for (unsigned int i = 0; i < scene_in.mNumMeshes; ++i)
  {
    // *NOTE*: load triangle meshes only
    if (scene_in.mMeshes[i]->mPrimitiveTypes != aiPrimitiveType_TRIANGLE)
    {
      ACE_DEBUG ((LM_WARNING,
                  ACE_TEXT ("mesh (was: #%u: \"%s\"), contains invalid primitives, skipping\n"),
                  i, ACE_TEXT (scene_in.mMeshes[i]->mName.C_Str ())));
      continue;
    } // end IF

    const aiMesh& mesh_r = *scene_in.mMeshes[i];
    Common_GL_Mesh mesh (this,
                         mesh_r);
    if (!mesh.loadMaterial (path_in,
                            scene_in,
                            mesh_r.mMaterialIndex))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Common_GL_Mesh::loadMaterial(\"%s\",%u), continuing\n"),
                  ACE_TEXT (path_in.c_str ()),
                  mesh_r.mMaterialIndex));
      continue;
    } // end IF

    meshes_.push_back (mesh);
  } // end FOR

  aiVector3D min, max;
  Common_GL_Assimp_Tools::boundingBox (scene_in,
                                       min, max);
  box_.first.x = min.x;
  box_.first.y = min.y;
  box_.first.z = min.z;

  box_.second.x = max.x;
  box_.second.y = max.y;
  box_.second.z = max.z;

  center_.x = (min.x + max.x) / 2.0f;
  center_.y = (min.y + max.y) / 2.0f;
  center_.z = (min.z + max.z) / 2.0f;

  // debug info
  size_t num_vertices_i = 0;
  size_t num_triangles_i = 0;
  for (std::vector<Common_GL_Mesh>::iterator iterator = meshes_.begin ();
       iterator != meshes_.end ();
       ++iterator)
  {
    num_vertices_i += (*iterator).vertices_.size ();
    num_triangles_i += (*iterator).indices_.size () / 3;
  } // end FOR

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("loading scene \"%s\"...DONE --> %Q triangles, %Q vertices\n"),
              ACE_TEXT (scene_in.mName.C_Str ()),
              num_triangles_i,
              num_vertices_i));
}
#endif // ASSIMP_SUPPORT

Common_GL_Model::Common_GL_Model ()
 : box_ ()
 , center_ ()
 , meshes_ ()
 , textures_ ()
{
  COMMON_TRACE (ACE_TEXT ("Common_GL_Model::Common_GL_Model"));
}

Common_GL_Model::Common_GL_Model (const std::string& path_in)
 : box_ ()
 , center_ ()
 , meshes_ ()
 , textures_ ()
{
  COMMON_TRACE (ACE_TEXT ("Common_GL_Model::Common_GL_Model"));

  load (path_in);
}

void
Common_GL_Model::load (const std::string& fileName_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_GL_Model::load"));

  // sanity check(s)
  if (!meshes_.empty ())
    reset ();
  ACE_ASSERT (meshes_.empty ());

#if defined (ASSIMP_SUPPORT)
  unsigned int flags_i = COMMON_GL_ASSIMP_IMPORT_FLAGS;
  Common_GL_Model* model_p = 
    Common_GL_Model::loadFromFile (fileName_in,
                                   flags_i);
  ACE_ASSERT (model_p);
  *this = *model_p;
  delete model_p; model_p = NULL;
#else
  // *TODO*: load mesh from file
  ACE_ASSERT (false);
#endif // ASSIMP_SUPPORT
}

void
Common_GL_Model::render (Common_GL_Shader& shader_in,
                         Common_GL_Camera& camera_in,
                         struct Common_GL_Perspective& perspectiveInformation_in,
                         glm::mat4 modelMatrix_in,
                         glm::vec3 translation_in,
                         glm::quat rotation_in,
                         glm::vec3 scale_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_GL_Model::render"));

  shader_in.use ();

  // manage lighting
  static glm::vec4 light_color_s (1.0f); // opaque white
  static GLint lightColor_location_i =
    glGetUniformLocation (shader_in.id_, ACE_TEXT_ALWAYS_CHAR ("lightColor"));
  ACE_ASSERT (lightColor_location_i);
  glUniform4fv (lightColor_location_i, 1, &(light_color_s.x));

  // manage camera uniforms
  static GLint camPos_location_i =
    glGetUniformLocation (shader_in.id_, ACE_TEXT_ALWAYS_CHAR ("camPos"));
  ACE_ASSERT (camPos_location_i);
  glUniform3fv (camPos_location_i, 1, &(camera_in.position_.x));

  glm::mat4 view_matrix_s = camera_in.getViewMatrix ();
  glm::mat4 projection_matrix_s =
#if defined (ACE_WIN32) || defined (ACE_WIN32)
    Common_GL_Camera::getProjectionMatrix (perspectiveInformation_in.resolution.cx,
                                           perspectiveInformation_in.resolution.cy,
                                           perspectiveInformation_in.FOV,
                                           perspectiveInformation_in.zNear,
                                           perspectiveInformation_in.zFar);
#else
    Common_GL_Camera::getProjectionMatrix (perspectiveInformation_in.resolution.width,
                                           perspectiveInformation_in.resolution.height,
                                           perspectiveInformation_in.FOV,
                                           perspectiveInformation_in.zNear,
                                           perspectiveInformation_in.zFar);
#endif // ACE_WIN32 || ACE_WIN64
  glm::mat4 proj_x_view_s = projection_matrix_s * view_matrix_s;
  shader_in.setMat4 (ACE_TEXT_ALWAYS_CHAR ("camMatrix"), proj_x_view_s);

  // set mesh model matrix
  shader_in.setMat4 (ACE_TEXT_ALWAYS_CHAR ("model"), modelMatrix_in);

  // Initialize transformation matrices
  glm::mat4 trans = glm::mat4 (1.0f);
  glm::mat4 rot = glm::mat4 (1.0f);
  glm::mat4 sca = glm::mat4 (1.0f);

  // Transform the matrices to their correct form
  trans = glm::translate (trans, translation_in);
  rot = glm::mat4_cast (rotation_in);
  sca = glm::scale (sca, scale_in);

  // Push the model orientation + scale matrices to the vertex shader.
  // *NOTE*: the final model matrix is computed there (see ardrone_ui.vert)
  shader_in.setMat4 (ACE_TEXT_ALWAYS_CHAR ("translation"), trans);
  shader_in.setMat4 (ACE_TEXT_ALWAYS_CHAR ("rotation"), rot);
  shader_in.setMat4 (ACE_TEXT_ALWAYS_CHAR ("scale"), sca);

  for (std::vector<Common_GL_Mesh>::iterator iterator = meshes_.begin ();
       iterator != meshes_.end ();
       ++iterator)
    (*iterator).render (shader_in,
                        camera_in,
                        perspectiveInformation_in,
                        modelMatrix_in,
                        translation_in, rotation_in, scale_in);

  shader_in.unuse ();
}

void
Common_GL_Model::reset ()
{
  COMMON_TRACE (ACE_TEXT ("Common_GL_Model::reset"));

  for (std::vector<Common_GL_Mesh>::iterator iterator = meshes_.begin ();
       iterator != meshes_.end ();
       ++iterator)
    (*iterator).reset ();
  meshes_.clear ();

  for (std::vector<Common_GL_Texture*>::iterator iterator = textures_.begin ();
       iterator != textures_.end ();
       ++iterator)
  {
    (*iterator)->reset ();
    delete *iterator;
  } // end FOR
  textures_.clear ();
}

Common_GL_Texture*
Common_GL_Model::addTexture (const std::string& path_in,
                             enum Common_GL_Texture::Type type_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_GL_Model::addTexture"));

  // check cache first
  for (std::vector<Common_GL_Texture*>::iterator iterator = textures_.begin ();
       iterator != textures_.end ();
       ++iterator)
    if ((*iterator)->path_ == path_in)
    {
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("texture \"%s\" found in cache...\n"),
                  ACE_TEXT (path_in.c_str ())));
      return *iterator; // --> already loaded, return handle
    } // end IF

  // not cached --> load
  Common_GL_Texture* texture_p = NULL;
  ACE_NEW_NORETURN (texture_p,
                    Common_GL_Texture ());
  ACE_ASSERT (texture_p);
  texture_p->type_ = type_in;

  if (unlikely (!texture_p->load (path_in)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_GL_Texture::load(\"%s\"), aborting\n"),
                ACE_TEXT (path_in.c_str ())));
    delete texture_p;
    return NULL;
  } // end IF
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("loaded texture \"%s\"...\n"),
              ACE_TEXT (path_in.c_str ())));

  textures_.push_back (texture_p);

  return textures_.back ();
}
