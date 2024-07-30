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

#include "common_macros.h"
#include "common_file_tools.h"

#if defined (ASSIMP_SUPPORT)
#include "common_gl_assimp_tools.h"
#endif // ASSIMP_SUPPORT
#include "common_gl_common.h"

#if defined (ASSIMP_SUPPORT)
Common_GL_Model
Common_GL_Model::loadFromFile (const std::string& path_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_GL_Model::loadFromFile"));

  static Common_GL_Model dummy;

  struct aiScene* scene_p = NULL;
  if (!Common_GL_Assimp_Tools::loadModel (path_in,
                                          scene_p,
                                          aiProcessPreset_TargetRealtime_Quality))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_GL_Assimp_Tools::loadModel(\"%s\",%d), aborting\n"),
                ACE_TEXT (path_in.c_str ()),
                aiProcessPreset_TargetRealtime_Quality));
    return dummy;
  } // end IF
  ACE_ASSERT (scene_p);

  Common_GL_Model return_value (path_in,
                                *scene_p);

  aiReleaseImport (scene_p); scene_p = NULL;

  return return_value;
}

Common_GL_Model::Common_GL_Model (const std::string& path_in,
                                  const struct aiScene& scene_in)
 : box_ ()
 , center_ ()
 , meshes_ ()
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

    Common_GL_Mesh mesh (*scene_in.mMeshes[i]);
    if (!mesh.init (path_in,
                    scene_in))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Common_GL_Mesh::init(\"%s\"), continuing\n"),
                  ACE_TEXT (path_in.c_str ())));
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
  unsigned int num_vertices_i = 0;
  unsigned int num_triangles_i = 0;
  for (std::vector<Common_GL_Mesh>::iterator iterator = meshes_.begin ();
       iterator != meshes_.end ();
       ++iterator)
  {
    num_vertices_i += (*iterator).vertices_.size ();
    num_triangles_i += (*iterator).indices_.size () / 3;
  } // end FOR

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("loading scene \"%s\"...DONE --> %u triangles, %u vertices\n"),
              ACE_TEXT (scene_in.mName.C_Str ()),
              num_triangles_i,
              num_vertices_i));
}
#endif // ASSIMP_SUPPORT

Common_GL_Model::Common_GL_Model ()
 : box_ ()
 , center_ ()
 , meshes_ ()
{
  COMMON_TRACE (ACE_TEXT ("Common_GL_Model::Common_GL_Model"));
}

Common_GL_Model::Common_GL_Model (const std::string& fileName_in)
 : box_ ()
 , center_ ()
 , meshes_ ()
{
  COMMON_TRACE (ACE_TEXT ("Common_GL_Model::Common_GL_Model"));

#if defined (ASSIMP_SUPPORT)
  *this = Common_GL_Model::loadFromFile (fileName_in);
#else
  // *TODO*: load mesh from file
  ACE_ASSERT (false);
#endif // ASSIMP_SUPPORT
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
  *this = Common_GL_Model::loadFromFile (fileName_in);
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

  // manage camera uniforms
  glUniform3fv (glGetUniformLocation (shader_in.id_, ACE_TEXT_ALWAYS_CHAR ("camPos")),
                1, &(camera_in.position_.x));

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

  // Push the matrices to the vertex shader
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
}
