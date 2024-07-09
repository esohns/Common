/***************************************************************************
 *   Copyright (C) 2009 by Erik Sohns   *
 *   erik.sohns@web.de   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "stdafx.h"

#include "common_gl_assimp_tools.h"

#include "ace/config-lite.h"
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include "gl/GL.h"
#else
#include "GL/gl.h"
#endif // ACE_WIN32 || ACE_WIN64

#include "assimp/cimport.h"
#include "assimp/postprocess.h"
#include "assimp/scene.h"

#include "ace/Log_Msg.h"

#include "common_macros.h"
#include "common_file_tools.h"

#include "common_gl_defines.h"

void
Common_GL_Assimp_Tools::boundingBox (const struct aiScene* scene_in,
                                     const struct aiNode* node_in,
                                     aiVector3D* min_inout,
                                     aiVector3D* max_inout,
                                     aiMatrix4x4* tansformation_inout)
{
  COMMON_TRACE (ACE_TEXT ("Common_GL_Assimp_Tools::boundingBox"));

  // sanity check(s)
  ACE_ASSERT (scene_in);
  ACE_ASSERT (node_in);
  ACE_ASSERT (min_inout);
  ACE_ASSERT (max_inout);
  ACE_ASSERT (tansformation_inout);

  aiMatrix4x4 previous_m;
  unsigned int n = 0, t;
  const struct aiMesh* mesh_p = NULL;
  aiVector3D vector3d_s;

  previous_m = *tansformation_inout;
  aiMultiplyMatrix4 (tansformation_inout, &node_in->mTransformation);

  for (;
       n < node_in->mNumMeshes;
       ++n)
  {
    mesh_p = scene_in->mMeshes[node_in->mMeshes[n]];
    ACE_ASSERT (mesh_p);

    for (t = 0;
         t < mesh_p->mNumVertices;
         ++t)
    {
      vector3d_s = mesh_p->mVertices[t];
      aiTransformVecByMatrix4 (&vector3d_s, tansformation_inout);

      min_inout->x = COMMON_GL_ASSIMP_MIN (min_inout->x, vector3d_s.x);
      min_inout->y = COMMON_GL_ASSIMP_MIN (min_inout->y, vector3d_s.y);
      min_inout->z = COMMON_GL_ASSIMP_MIN (min_inout->z, vector3d_s.z);

      max_inout->x = COMMON_GL_ASSIMP_MAX (max_inout->x, vector3d_s.x);
      max_inout->y = COMMON_GL_ASSIMP_MAX (max_inout->y, vector3d_s.y);
      max_inout->z = COMMON_GL_ASSIMP_MAX (max_inout->z, vector3d_s.z);
    } // end FOR
  } // end FOR

  for (n = 0;
       n < node_in->mNumChildren;
       ++n)
    Common_GL_Assimp_Tools::boundingBox (scene_in,
                                         node_in->mChildren[n],
                                         min_inout,
                                         max_inout,
                                         tansformation_inout);

  *tansformation_inout = previous_m;
}

void
Common_GL_Assimp_Tools::boundingBox (const struct aiScene* scene_in,
                                     aiVector3D* min_out,
                                     aiVector3D* max_out)
{
  COMMON_TRACE (ACE_TEXT ("Common_GL_Assimp_Tools::boundingBox"));

  // sanity check(s)
  ACE_ASSERT (min_out);
  ACE_ASSERT (max_out);

  aiMatrix4x4 transformation_m;
  aiIdentityMatrix4 (&transformation_m);

  min_out->x = min_out->y = min_out->z =  1e10f;
  max_out->x = max_out->y = max_out->z = -1e10f;

  Common_GL_Assimp_Tools::boundingBox (scene_in,
                                       scene_in->mRootNode,
                                       min_out,
                                       max_out,
                                       &transformation_m);
}

bool
Common_GL_Assimp_Tools::loadModel (const std::string& path_in,
                                   struct aiScene*& scene_inout)
{
  COMMON_TRACE (ACE_TEXT ("Common_GL_Assimp_Tools::loadModel"));

  bool result = false;

  // sanity check(s)
  ACE_ASSERT (Common_File_Tools::isReadable (path_in));

#if defined (_DEBUG)
  struct aiLogStream log_stream =
      aiGetPredefinedLogStream (aiDefaultLogStream_FILE,
                                ACE_TEXT_ALWAYS_CHAR (COMMON_GL_ASSIMP_LOG_FILENAME_STRING));
  aiAttachLogStream (&log_stream);
#endif // _DEBUG

  if (scene_inout)
  {
    aiReleaseImport (scene_inout); scene_inout = NULL;
  } // end IF

  scene_inout =
      const_cast<struct aiScene*> (aiImportFile (path_in.c_str (),
                                                 aiProcessPreset_TargetRealtime_MaxQuality));
  if (!scene_inout)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to aiImportFile(\"%s\",%d), aborting\n"),
                ACE_TEXT (path_in.c_str ()),
                aiProcessPreset_TargetRealtime_MaxQuality));
    goto error;
  } // end IF

  result = true;

error:
#if defined (_DEBUG)
  aiDetachAllLogStreams ();
#endif // _DEBUG

  return result;
}

void
Common_GL_Assimp_Tools::applyMaterial (const struct aiMaterial* material_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_GL_Assimp_Tools::applyMaterial"));

  float c[4];
  GLenum fill_mode_e;
  int ret1, ret2;
  aiColor4D diffuse;
  aiColor4D specular;
  aiColor4D ambient;
  aiColor4D emission;
//  ai_real shininess, strength;
  float shininess, strength;
  int two_sided;
  int wireframe;
  unsigned int max = 1;

  Common_GL_Assimp_Tools::set_float4 (c, 0.8f, 0.8f, 0.8f, 1.0f);
  if (AI_SUCCESS == aiGetMaterialColor (material_in,
                                        AI_MATKEY_COLOR_DIFFUSE,
                                        &diffuse))
    Common_GL_Assimp_Tools::color4_to_float4 (&diffuse, c);
  glMaterialfv (GL_FRONT_AND_BACK, GL_DIFFUSE, c);
  Common_GL_Assimp_Tools::set_float4 (c, 0.0f, 0.0f, 0.0f, 1.0f);
  if (AI_SUCCESS == aiGetMaterialColor (material_in,
                                        AI_MATKEY_COLOR_SPECULAR,
                                        &specular))
    Common_GL_Assimp_Tools::color4_to_float4 (&specular, c);
  glMaterialfv (GL_FRONT_AND_BACK, GL_SPECULAR, c);
  Common_GL_Assimp_Tools::set_float4 (c, 0.2f, 0.2f, 0.2f, 1.0f);
  if (AI_SUCCESS == aiGetMaterialColor (material_in,
                                        AI_MATKEY_COLOR_AMBIENT,
                                        &ambient))
    Common_GL_Assimp_Tools::color4_to_float4 (&ambient, c);
  glMaterialfv (GL_FRONT_AND_BACK, GL_AMBIENT, c);
  Common_GL_Assimp_Tools::set_float4 (c, 0.0f, 0.0f, 0.0f, 1.0f);
  if (AI_SUCCESS == aiGetMaterialColor (material_in,
                                        AI_MATKEY_COLOR_EMISSIVE,
                                        &emission))
    Common_GL_Assimp_Tools::color4_to_float4 (&emission, c);
  glMaterialfv (GL_FRONT_AND_BACK, GL_EMISSION, c);

  ret1 = aiGetMaterialFloatArray (material_in,
                                  AI_MATKEY_SHININESS,
                                  &shininess, &max);
  if (ret1 == AI_SUCCESS)
  {
    max = 1;
    ret2 = aiGetMaterialFloatArray (material_in,
                                    AI_MATKEY_SHININESS_STRENGTH,
                                    &strength, &max);
    if (ret2 == AI_SUCCESS)
      glMaterialf (GL_FRONT_AND_BACK, GL_SHININESS, shininess * strength);
    else
      glMaterialf (GL_FRONT_AND_BACK, GL_SHININESS, shininess);
  } // end IF
  else
  {
    glMaterialf (GL_FRONT_AND_BACK, GL_SHININESS, 0.0f);
    Common_GL_Assimp_Tools::set_float4 (c, 0.0f, 0.0f, 0.0f, 0.0f);
    glMaterialfv (GL_FRONT_AND_BACK, GL_SPECULAR, c);
  } // end ELSE

  max = 1;
  if (AI_SUCCESS == aiGetMaterialIntegerArray (material_in,
                                               AI_MATKEY_ENABLE_WIREFRAME,
                                               &wireframe, &max))
    fill_mode_e = wireframe ? GL_LINE : GL_FILL;
  else
    fill_mode_e = GL_FILL;
  glPolygonMode (GL_FRONT_AND_BACK, fill_mode_e);

  max = 1;
  if ((AI_SUCCESS == aiGetMaterialIntegerArray (material_in,
                                                AI_MATKEY_TWOSIDED,
                                                &two_sided, &max)) && two_sided)
    glDisable (GL_CULL_FACE);
  else
    glEnable (GL_CULL_FACE);
}

void
Common_GL_Assimp_Tools::render (const struct aiScene* scene_in,
                                const struct aiNode* node_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_GL_Assimp_Tools::render"));

  unsigned int i, index;
  unsigned int n = 0, t;
  aiMatrix4x4 matrix = node_in->mTransformation;
  const struct aiMesh* mesh_p = NULL;
  const struct aiFace* face_p = NULL;
  GLenum face_mode_e;

  /* update transform */
  aiTransposeMatrix4 (&matrix);
  glPushMatrix ();
  glMultMatrixf (reinterpret_cast<GLfloat*> (&matrix));

  /* draw all meshes assigned to this node */
  for (;
       n < node_in->mNumMeshes;
       ++n)
  {
    mesh_p = scene_in->mMeshes[node_in->mMeshes[n]];
    ACE_ASSERT (mesh_p);

    Common_GL_Assimp_Tools::applyMaterial (scene_in->mMaterials[mesh_p->mMaterialIndex]);

    if (!mesh_p->mNormals)
      glDisable (GL_LIGHTING);
    else
      glEnable (GL_LIGHTING);

    for (t = 0;
         t < mesh_p->mNumFaces;
         ++t)
    {
      face_p = &mesh_p->mFaces[t];
      ACE_ASSERT (face_p);

      switch (face_p->mNumIndices)
      {
        case 1:
          face_mode_e = GL_POINTS; break;
        case 2:
          face_mode_e = GL_LINES; break;
        case 3:
          face_mode_e = GL_TRIANGLES; break;
        default:
          face_mode_e = GL_POLYGON; break;
      } // end SWITCH

      glBegin (face_mode_e);

      for (i = 0;
           i < face_p->mNumIndices;
           ++i)
      {
        index = face_p->mIndices[i];
        if (mesh_p->mColors[0])
          glColor4fv (reinterpret_cast<GLfloat*> (&mesh_p->mColors[0][index]));
        if (mesh_p->mNormals)
          glNormal3fv (&mesh_p->mNormals[index].x);
        glVertex3fv (&mesh_p->mVertices[index].x);
      } // end FOR

      glEnd ();
    } // end FOR
  } // end FOR

  /* draw all children */
  for (n = 0;
       n < node_in->mNumChildren;
       ++n)
    Common_GL_Assimp_Tools::render (scene_in, node_in->mChildren[n]);

  glPopMatrix ();
}
