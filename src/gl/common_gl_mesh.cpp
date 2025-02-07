#include "stdafx.h"

#include "common_gl_mesh.h"

#include <cstddef>

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
#include "common_gl_model.h"
#include "common_gl_texture.h"

#if defined (ASSIMP_SUPPORT)
Common_GL_Mesh*
Common_GL_Mesh::load (Common_GL_Model* model_in,
                      const std::string& path_in,
                      unsigned int index_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_GL_Mesh::load"));

  struct aiScene* scene_p = NULL;
  if (!Common_GL_Assimp_Tools::loadModel (path_in,
                                          scene_p,
                                          aiProcessPreset_TargetRealtime_Quality))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_GL_Assimp_Tools::loadModel(\"%s\",%d), aborting\n"),
                ACE_TEXT (path_in.c_str ()),
                aiProcessPreset_TargetRealtime_Quality));
    return NULL;
  } // end IF
  ACE_ASSERT (scene_p && scene_p->HasMeshes ());
  ACE_ASSERT (index_in < scene_p->mNumMeshes);

  const aiMesh& mesh_r = *scene_p->mMeshes[index_in];

  Common_GL_Mesh* return_value_p = NULL;
  ACE_NEW_NORETURN (return_value_p,
                    Common_GL_Mesh (model_in, mesh_r));
  ACE_ASSERT (return_value_p);
  if (!return_value_p->loadMaterial (path_in,
                                     *scene_p,
                                     mesh_r.mMaterialIndex))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_GL_Mesh::loadMaterial(\"%s\",%u), aborting\n"),
                ACE_TEXT (path_in.c_str ()),
                mesh_r.mMaterialIndex));
    aiReleaseImport (scene_p); scene_p = NULL;
    delete return_value_p;
    return NULL;
  } // end IF
  aiReleaseImport (scene_p); scene_p = NULL;

  return return_value_p;
}

Common_GL_Mesh::Common_GL_Mesh (Common_GL_Model* model_in,
                                const struct aiMesh& mesh_in)
 : vertices_ ()
 , indices_ ()
 , model_ (model_in)
 , primitiveType_ (GL_TRIANGLES)
 , textures_ ()
 , VAO_ (true)
 , VBO_ (true)
 , EBO_ (true)
{
  COMMON_TRACE (ACE_TEXT ("Common_GL_Mesh::Common_GL_Mesh"));

  // sanity check(s)
  ACE_ASSERT (mesh_in.mPrimitiveTypes == aiPrimitiveType_TRIANGLE);

  // ACE_DEBUG ((LM_DEBUG,
  //             ACE_TEXT ("loading mesh \"%s\"...\n"),
  //             ACE_TEXT (mesh_in.mName.C_Str ())));

  struct Common_GL_Vertex vertex_s;
  vertex_s.color = {1.0f, 1.0f, 1.0f, 1.0f}; // opaque white
  for (unsigned int i = 0; i < mesh_in.mNumVertices; ++i)
  {
    const aiVector3D& position_r = mesh_in.mVertices[i];
    vertex_s.position = {position_r.x, position_r.y, position_r.z};
    if (likely (mesh_in.HasNormals ()))
    {
      const aiVector3D& normal_r = mesh_in.mNormals[i];
      vertex_s.normal = {normal_r.x, normal_r.y, normal_r.z };
    } // end IF
    if (likely (mesh_in.HasVertexColors (0)))
    {
      const aiColor4D& color_r = mesh_in.mColors[0][i];
      vertex_s.color = {color_r.r, color_r.g, color_r.b, color_r.a };
    } // end IF
    // else
    //   vertex_s.color = {1.0f, 1.0f, 1.0f, 1.0f}; // opaque white
    if (likely (mesh_in.HasTextureCoords (0)))
    {
      const aiVector3D& tex_coord_r = mesh_in.mVertices[i];
      vertex_s.uv = {tex_coord_r.x, tex_coord_r.y };
    } // end IF
    // else
    //   vertex_s.uv = {0.0f, 0.0f};
    vertices_.push_back (vertex_s);
  } // end FOR

  for (unsigned int i = 0; i < mesh_in.mNumFaces; ++i)
  { const aiFace& face_r = mesh_in.mFaces[i];
    ACE_ASSERT (face_r.mNumIndices == 3);
    // if (face_r.mNumIndices != 3)
    //   break; // *NOTE*: iff aiProcess_SortByPType is set, all faces of a mesh
    //          //         have the same amount of indices
    //   switch (face_r.mNumIndices)
    //   {
    //     case 1:
    //       primitiveType_ = GL_POINTS; break;
    //     case 2:
    //       primitiveType_ = GL_LINES; break;
    //     case 3:
    //       primitiveType_ = GL_TRIANGLES; break;
    //     default:
    //       primitiveType_ = GL_POLYGON; break;
    //   } // end SWITCH

    // for (unsigned int j = 0; j < face_r.mNumIndices; ++j)
    // indices_.push_back (face_r.mIndices[j]);
    indices_.push_back (face_r.mIndices[0]);
    indices_.push_back (face_r.mIndices[1]);
    indices_.push_back (face_r.mIndices[2]);
  } // end FOR

  VAO_.bind ();

  VBO_.upload (vertices_);
  EBO_.upload (indices_);

  VAO_.linkAttrib (VBO_, 0, 3, GL_FLOAT, sizeof (struct Common_GL_Vertex), (void*)(0  * sizeof (float))); // position --> 0
  VAO_.linkAttrib (VBO_, 1, 3, GL_FLOAT, sizeof (struct Common_GL_Vertex), (void*)(3  * sizeof (float))); // normal   --> 1
  VAO_.linkAttrib (VBO_, 2, 4, GL_FLOAT, sizeof (struct Common_GL_Vertex), (void*)(6  * sizeof (float))); // color    --> 2
  VAO_.linkAttrib (VBO_, 3, 2, GL_FLOAT, sizeof (struct Common_GL_Vertex), (void*)(10 * sizeof (float))); // uv       --> 3

  VAO_.unbind ();

  // ACE_DEBUG ((LM_DEBUG,
  //             ACE_TEXT ("loading mesh \"%s\"...DONE\n"),
  //             ACE_TEXT (mesh_in.mName.C_Str ())));
}

bool
Common_GL_Mesh::loadMaterial (const std::string& scenePath_in,
                              const struct aiScene& scene_in,
                              unsigned int materialIndex_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_GL_Mesh::loadMaterial"));

  // sanity check(s)
  ACE_ASSERT (model_);
  ACE_ASSERT (textures_.empty ());
  std::string directory_string = Common_File_Tools::directory (scenePath_in);
  ACE_ASSERT (!directory_string.empty ());
  ACE_ASSERT (scene_in.HasMaterials ());

  const aiMaterial& material_r = *scene_in.mMaterials[materialIndex_in];
  aiReturn result;
  aiString path_string;
  std::string path_string_2;
  for (unsigned int i = 0;
       i < material_r.GetTextureCount (aiTextureType_AMBIENT);
       ++i)
  {
    result = material_r.GetTexture (aiTextureType_AMBIENT, i,
                                    &path_string,
                                    NULL, NULL, NULL, NULL, NULL);
    if (unlikely (result != aiReturn_SUCCESS))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to aiMaterial::GetTexture(aiTextureType_AMBIENT, %u): \"%m\", continuing\n"),
                  i));
      continue;
    } // end IF

    path_string_2 = path_string.data;
    path_string_2 = Common_File_Tools::basename (path_string_2, false);
    path_string_2 =
      directory_string + ACE_DIRECTORY_SEPARATOR_CHAR_A + path_string_2;
    Common_GL_Texture* texture_p = model_->addTexture (path_string_2,
                                                       Common_GL_Texture::Type::TYPE_AMBIENT);
    if (unlikely (!texture_p))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Common_GL_Model::addTexture(\"%s\"), continuing\n"),
                  ACE_TEXT (path_string_2.c_str ())));
      continue;
    } // end IF
    textures_.push_back (texture_p);
  } // end FOR

  for (unsigned int i = 0;
       i < material_r.GetTextureCount (aiTextureType_DIFFUSE);
       ++i)
  {
    result = material_r.GetTexture (aiTextureType_DIFFUSE, i,
                                    &path_string,
                                    NULL, NULL, NULL, NULL, NULL);
    if (unlikely (result != aiReturn_SUCCESS))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to aiMaterial::GetTexture(aiTextureType_DIFFUSE, %u): \"%m\", continuing\n"),
                  i));
      continue;
    } // end IF

    path_string_2 = path_string.data;
    path_string_2 = Common_File_Tools::basename (path_string_2, false);
    path_string_2 =
      directory_string + ACE_DIRECTORY_SEPARATOR_CHAR_A + path_string_2;
    Common_GL_Texture* texture_p = model_->addTexture (path_string_2,
                                                       Common_GL_Texture::Type::TYPE_DIFFUSE);
    if (unlikely (!texture_p))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Common_GL_Model::addTexture(\"%s\"), continuing\n"),
                  ACE_TEXT (path_string_2.c_str ())));
      continue;
    } // end IF
    textures_.push_back (texture_p);
  } // end FOR

  for (unsigned int i = 0;
       i < material_r.GetTextureCount (aiTextureType_SPECULAR);
       ++i)
  {
    result = material_r.GetTexture (aiTextureType_SPECULAR, i,
                                    &path_string,
                                    NULL, NULL, NULL, NULL, NULL);
    if (unlikely (result != aiReturn_SUCCESS))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to aiMaterial::GetTexture(aiTextureType_SPECULAR, %u): \"%m\", continuing\n"),
                  i));
      continue;
    } // end IF

    path_string_2 = path_string.data;
    path_string_2 = Common_File_Tools::basename (path_string_2, false);
    path_string_2 =
      directory_string + ACE_DIRECTORY_SEPARATOR_CHAR_A + path_string_2;
    Common_GL_Texture* texture_p = model_->addTexture (path_string_2,
                                                       Common_GL_Texture::Type::TYPE_SPECULAR);
    if (unlikely (!texture_p))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Common_GL_Model::addTexture(\"%s\"), continuing\n"),
                  ACE_TEXT (path_string_2.c_str ())));
      continue;
    } // end IF
    textures_.push_back (texture_p);
  } // end FOR

  return true;
}
#endif // ASSIMP_SUPPORT

Common_GL_Mesh::Common_GL_Mesh (Common_GL_Model* model_in)
 : vertices_ ()
 , indices_ ()
 , model_ (model_in)
 , primitiveType_ (GL_TRIANGLES)
 , textures_ ()
 , VAO_ (false)
 , VBO_ (false)
 , EBO_ (false)
{
  COMMON_TRACE (ACE_TEXT ("Common_GL_Mesh::Common_GL_Mesh"));
}

Common_GL_Mesh::Common_GL_Mesh (Common_GL_Model* model_in,
                                const std::string& scenePath_in)
 : vertices_ ()
 , indices_ ()
 , model_ (model_in)
 , primitiveType_ (GL_TRIANGLES)
 , textures_ ()
 , VAO_ (true)
 , VBO_ (true)
 , EBO_ (true)
{
  COMMON_TRACE (ACE_TEXT ("Common_GL_Mesh::Common_GL_Mesh"));

  // *TODO*: load mesh from file
  ACE_ASSERT (false);

  VAO_.bind ();

  VBO_.upload (vertices_);
  EBO_.upload (indices_);

  VAO_.linkAttrib (VBO_, 0, 3, GL_FLOAT, sizeof (struct Common_GL_Vertex), (void*)offsetof (Common_GL_Vertex, position)); // position --> 0
  VAO_.linkAttrib (VBO_, 1, 3, GL_FLOAT, sizeof (struct Common_GL_Vertex), (void*)offsetof (Common_GL_Vertex, normal));   // normal   --> 1
  VAO_.linkAttrib (VBO_, 2, 4, GL_FLOAT, sizeof (struct Common_GL_Vertex), (void*)offsetof (Common_GL_Vertex, color));    // color    --> 2
  VAO_.linkAttrib (VBO_, 3, 2, GL_FLOAT, sizeof (struct Common_GL_Vertex), (void*)offsetof (Common_GL_Vertex, uv));       // uv       --> 3

  VAO_.unbind ();
}

void
Common_GL_Mesh::render (Common_GL_Shader& shader_in,
                        Common_GL_Camera& camera_in,
                        struct Common_GL_Perspective& perspectiveInformation_in,
                        glm::mat4 modelMatrix_in,
                        glm::vec3 translation_in,
                        glm::quat rotation_in,
                        glm::vec3 scale_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_GL_Mesh::render"));

  VAO_.bind ();

  // Keep track of how many of each type of textures there are
  unsigned int num_diffuse_i = 0;
  unsigned int num_specular_i = 0;

  // activate and bind textures
  std::string type, num;
  for (unsigned int i = 0; i < textures_.size (); i++)
  {
    switch (textures_[i]->type_)
    {
      case Common_GL_Texture::TYPE_DIFFUSE:
        type = ACE_TEXT_ALWAYS_CHAR ("diffuse");
        num = std::to_string (num_diffuse_i++);
        break;
      case Common_GL_Texture::TYPE_SPECULAR:
        type = ACE_TEXT_ALWAYS_CHAR ("specular");
        num = std::to_string (num_specular_i++);
        break;
      default:
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("invalid/unknown texture type (was: %d), continuing\n"),
                    textures_[i]->type_));
        continue;
    } // end SWITCH
    glActiveTexture (GL_TEXTURE0 + i);
    textures_[i]->bind ();
    textures_[i]->set (shader_in, type + num, i);
  } // end FOR

  // Draw the actual mesh
  EBO_.bind ();
  glDrawElements (GL_TRIANGLES, static_cast<GLsizei> (indices_.size ()), GL_UNSIGNED_INT, 0);
  EBO_.unbind ();
  //VBO_.bind ();
  //glDrawArrays (GL_TRIANGLES, 0, static_cast<GLsizei> (vertices_.size ()));
  //VBO_.unbind ();

  for (unsigned int i = 0; i < textures_.size (); i++)
    textures_[i]->unbind ();

  VAO_.unbind ();
}

void
Common_GL_Mesh::reset ()
{
  COMMON_TRACE (ACE_TEXT ("Common_GL_Mesh::reset"));

  vertices_.clear ();
  indices_.clear ();
  textures_.clear ();
  EBO_.free ();
  VBO_.free ();
  VAO_.free ();
}
