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

#include "common_gl_tools.h"

#include "png.h"

#include "assimp/cimport.h"
#include "assimp/postprocess.h"
#include "assimp/scene.h"

#include "ace/Log_Msg.h"

#include "common_file_tools.h"
#include "common_macros.h"

#include "common_image_defines.h"

#include "common_gl_defines.h"

std::string
Common_GL_Tools::errorToString (GLenum error_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_GL_Tools::errorToString"));

  // initialize return value
  std::string return_value;

  switch (error_in)
  {
    case GL_NO_ERROR: // 0
      break;
    case GL_INVALID_ENUM:
      return_value = ACE_TEXT_ALWAYS_CHAR ("An unacceptable value is specified for an enumerated argument"); break;
    case GL_INVALID_VALUE:
      return_value = ACE_TEXT_ALWAYS_CHAR ("A numeric argument is out of range"); break;
    case GL_INVALID_OPERATION:
      return_value = ACE_TEXT_ALWAYS_CHAR ("The specified operation is not allowed in the current state"); break;
    //case GL_INVALID_FRAMEBUFFER_OPERATION:
    //  return_value = ACE_TEXT_ALWAYS_CHAR ("The framebuffer object is not complete"); break;
    case GL_STACK_OVERFLOW:
      return_value = ACE_TEXT_ALWAYS_CHAR ("An attempt has been made to perform an operation that would cause an internal stack to overflow"); break;
    case GL_STACK_UNDERFLOW:
      return_value = ACE_TEXT_ALWAYS_CHAR ("An attempt has been made to perform an operation that would cause an internal stack to underflow"); break;
    case GL_OUT_OF_MEMORY:
      return_value = ACE_TEXT_ALWAYS_CHAR ("There is not enough memory left to execute the command"); break;
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown OpenGL error (was: %d), continuing\n"),
                  error_in));
      break;
    }
  } // end SWITCH

  return return_value;
}

GLuint
Common_GL_Tools::loadModel (const std::string& path_in,
                            Common_GL_BoundingBox_t& boundingBox_out,
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#if defined (_MSC_VER) && (_MSC_VER >= 1800)
                            glm::vec3& center_out)
#else
						    struct Common_GL_VectorF3& center_out)
#endif // _MSC_VER && (_MSC_VER >= 1800)
#endif // ACE_WIN32 || ACE_WIN64
{
  COMMON_TRACE (ACE_TEXT ("Common_GL_Tools::loadModel"));

  // initialize return value(s)
  GLuint return_value = 0;
  boundingBox_out.first.x = boundingBox_out.first.y = boundingBox_out.first.z = 0;
  boundingBox_out.second.x = boundingBox_out.second.y = boundingBox_out.second.z = 0;

  // sanity check(s)
  ACE_ASSERT (Common_File_Tools::isReadable (path_in));

  struct aiScene* scene_p = NULL;
  if (!Common_GL_Tools::loadModel (path_in,
                                   scene_p))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_GL_Tools::loadModel(\"%s\"), aborting\n"),
                ACE_TEXT (path_in.c_str ())));
    return return_value;
  } // end IF
  ACE_ASSERT (scene_p);

  return_value = glGenLists (1);
  COMMON_GL_ASSERT;
  glNewList (return_value, GL_COMPILE);
  COMMON_GL_ASSERT;
  // now begin at the root node of the imported data and traverse the scenegraph
  // by multiplying subsequent local transforms together on GLs' matrix stack
  Common_GL_Tools::render (scene_p, scene_p->mRootNode);
  glEndList ();
  COMMON_GL_ASSERT;

  // compute the scenes' bounding box / center
  aiVector3D min, max;
  Common_GL_Tools::boundingBox (scene_p,
                                &min, &max);

  boundingBox_out.first.x = min.x;
  boundingBox_out.first.y = min.y;
  boundingBox_out.first.z = min.z;

  boundingBox_out.second.x = max.x;
  boundingBox_out.second.y = max.y;
  boundingBox_out.second.z = max.z;

  center_out.x = (min.x + max.x) / 2.0F;
  center_out.y = (min.y + max.y) / 2.0F;
  center_out.z = (min.z + max.z) / 2.0F;

  // clean up
  aiReleaseImport (scene_p);

  return return_value;
}

bool
Common_GL_Tools::loadModel (const std::string& path_in,
                            struct aiScene*& scene_inout)
{
  COMMON_TRACE (ACE_TEXT ("Common_GL_Tools::loadModel"));

  bool result = false;

  // sanity check(s)
  ACE_ASSERT (Common_File_Tools::isReadable (path_in));

#if defined (_DEBUG)
  struct aiLogStream log_stream =
      aiGetPredefinedLogStream (aiDefaultLogStream_FILE,
                                ACE_TEXT_ALWAYS_CHAR (COMMON_GL_ASSIMP_LOG_FILENAME_STRING));
  aiAttachLogStream (&log_stream);
#endif

  if (scene_inout)
  {
    aiReleaseImport (scene_inout);
    scene_inout = NULL;
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
#endif

  return result;
}

void
Common_GL_Tools::boundingBox (const struct aiScene* scene_in,
                              const struct aiNode* node_in,
                              aiVector3D* min_inout,
                              aiVector3D* max_inout,
                              aiMatrix4x4* tansformation_inout)
{
  COMMON_TRACE (ACE_TEXT ("Common_GL_Tools::boundingBox"));

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
    Common_GL_Tools::boundingBox (scene_in,
                                  node_in->mChildren[n],
                                  min_inout,
                                  max_inout,
                                  tansformation_inout);

  *tansformation_inout = previous_m;
}
void
Common_GL_Tools::boundingBox (const struct aiScene* scene_in,
                              aiVector3D* min_out,
                              aiVector3D* max_out)
{
  COMMON_TRACE (ACE_TEXT ("Common_GL_Tools::boundingBox"));

  // sanity check(s)
  ACE_ASSERT (min_out);
  ACE_ASSERT (max_out);

  aiMatrix4x4 transformation_m;
  aiIdentityMatrix4 (&transformation_m);

  min_out->x = min_out->y = min_out->z =  1e10f;
  max_out->x = max_out->y = max_out->z = -1e10f;

  Common_GL_Tools::boundingBox (scene_in,
                                scene_in->mRootNode,
                                min_out,
                                max_out,
                                &transformation_m);
}

void
Common_GL_Tools::applyMaterial (const struct aiMaterial* material_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_GL_Tools::applyMaterial"));

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

  Common_GL_Tools::set_float4 (c, 0.8f, 0.8f, 0.8f, 1.0f);
  if (AI_SUCCESS == aiGetMaterialColor (material_in,
                                        AI_MATKEY_COLOR_DIFFUSE,
                                        &diffuse))
    Common_GL_Tools::color4_to_float4 (&diffuse, c);
  glMaterialfv (GL_FRONT_AND_BACK, GL_DIFFUSE, c);
  Common_GL_Tools::set_float4 (c, 0.0f, 0.0f, 0.0f, 1.0f);
  if (AI_SUCCESS == aiGetMaterialColor (material_in,
                                        AI_MATKEY_COLOR_SPECULAR,
                                        &specular))
    Common_GL_Tools::color4_to_float4 (&specular, c);
  glMaterialfv (GL_FRONT_AND_BACK, GL_SPECULAR, c);
  Common_GL_Tools::set_float4 (c, 0.2f, 0.2f, 0.2f, 1.0f);
  if (AI_SUCCESS == aiGetMaterialColor (material_in,
                                        AI_MATKEY_COLOR_AMBIENT,
                                        &ambient))
    Common_GL_Tools::color4_to_float4 (&ambient, c);
  glMaterialfv (GL_FRONT_AND_BACK, GL_AMBIENT, c);
  Common_GL_Tools::set_float4 (c, 0.0f, 0.0f, 0.0f, 1.0f);
  if (AI_SUCCESS == aiGetMaterialColor (material_in,
                                        AI_MATKEY_COLOR_EMISSIVE,
                                        &emission))
    Common_GL_Tools::color4_to_float4 (&emission, c);
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
    Common_GL_Tools::set_float4 (c, 0.0f, 0.0f, 0.0f, 0.0f);
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
Common_GL_Tools::render (const struct aiScene* scene_in,
                         const struct aiNode* node_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_GL_Tools::render"));

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

    Common_GL_Tools::applyMaterial (scene_in->mMaterials[mesh_p->mMaterialIndex]);

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
    Common_GL_Tools::render (scene_in, node_in->mChildren[n]);

  glPopMatrix ();
}

GLuint
Common_GL_Tools::loadTexture (const std::string& path_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_GL_Tools::loadTexture"));

  GLuint return_value = 0;

  // sanity check(s)
  if (!Common_File_Tools::isReadable (path_in))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_File_Tools::isReadable(\"%s\"), aborting\n"),
                ACE_TEXT (path_in.c_str ())));
    return return_value;
  } // end IF
  std::string file_extension_string =
      Common_File_Tools::fileExtension (path_in, false);
  ACE_ASSERT (!ACE_OS::strcmp (file_extension_string.c_str (), ACE_TEXT_ALWAYS_CHAR (COMMON_IMAGE_PNG_FILE_EXTENSION)));

  unsigned int width = 0, height = 0;
  bool has_alpha = false;
  GLubyte* image_p = NULL;
  if (!Common_GL_Tools::loadPNG (path_in,
                                 width, height,
                                 has_alpha,
                                 image_p))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_GL_Tools::loadPNG(\"%s\"), aborting\n"),
                ACE_TEXT (path_in.c_str ())));
    return return_value;
  } // end IF
  ACE_ASSERT (width && height);
  ACE_ASSERT (image_p);
  ACE_UNUSED_ARG (has_alpha);

  glGenTextures (1, &return_value);
  COMMON_GL_ASSERT;
  glBindTexture (GL_TEXTURE_2D, return_value);
  COMMON_GL_ASSERT;
  glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0,
                (has_alpha ? GL_RGBA : GL_RGB),
                GL_UNSIGNED_BYTE, image_p);
  COMMON_GL_ASSERT;

  // clean up
  free (image_p);

  return return_value;
}

bool
Common_GL_Tools::loadPNG (const std::string& path_in,
                          unsigned int& width_out,
                          unsigned int& height_out,
                          bool& hasAlpha_out,
                          GLubyte*& data_out)
{
  COMMON_TRACE (ACE_TEXT ("Common_GL_Tools::loadPNG"));

  // sanity check(s)
  ACE_ASSERT (!data_out);

  png_byte magic[COMMON_IMAGE_PNG_NUMBER_OF_MAGIC_BYTES];
  size_t bytes_read = 0;
  int is_png = 0;
  png_structp png_p = NULL;
  png_infop png_info_p = NULL;
  int color_type, interlace_type;
  FILE* file_p = NULL;
  int result = -1;
  unsigned int row_bytes = 0;
  png_bytepp row_pointers_pp = NULL;
  png_uint_32 width, height;
  int bit_depth, compression_method, filter_method;

  if ((file_p = ACE_OS::fopen (path_in.c_str (),
                               ACE_TEXT_ALWAYS_CHAR ("rb"))) == NULL)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::fopen(\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (path_in.c_str ())));
    return false;
  } // end IF

  // read the header magic, test if this could be a PNG file
  bytes_read = ACE_OS::fread (magic,
                              1,
                              COMMON_IMAGE_PNG_NUMBER_OF_MAGIC_BYTES,
                              file_p);
  if (bytes_read != COMMON_IMAGE_PNG_NUMBER_OF_MAGIC_BYTES)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::fread(%d): \"%m\", aborting\n"),
                COMMON_IMAGE_PNG_NUMBER_OF_MAGIC_BYTES));
    goto error;
  } // end IF
  is_png = !png_sig_cmp (magic,
                         0,
                         COMMON_IMAGE_PNG_NUMBER_OF_MAGIC_BYTES);
  if (!is_png)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to png_sig_cmp(): \"%m\", aborting\n")));
    goto error;
  } // end IF

  // create and initialize the png_struct with the desired error handler
  // functions. If you want to use the default stderr and longjump method, you
  // can supply NULL for the last three parameters. We also supply the compiler
  // header file version, so that we know if the application was compiled with a
  // compatible version of the library. REQUIRED
  png_p = png_create_read_struct (PNG_LIBPNG_VER_STRING,
                                  NULL, NULL, NULL);
  if (!png_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to png_create_read_struct(): \"%m\", aborting\n")));
    goto error;
  } // end IF

  // allocate/initialize the memory for image information. REQUIRED
  png_info_p = png_create_info_struct (png_p);
  if (!png_info_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to png_create_info_struct(): \"%m\", aborting\n")));
    goto error;
  } // end IF

  // set error handling if you are using the setjmp/longjmp method (this is the
  // normal method of doing things with libpng). REQUIRED unless you set up your
  // own error handlers in the png_create_read_struct() earlier
  if (setjmp (png_jmpbuf (png_p)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to setjmp(): \"%m\", aborting\n")));
    goto error;
  } // end IF

  // set up the output control if you are using standard C streams
  png_init_io (png_p, file_p);

  // if we have already read some of the signature
  ACE_ASSERT (bytes_read <= COMMON_IMAGE_PNG_NUMBER_OF_MAGIC_BYTES);
  png_set_sig_bytes (png_p, bytes_read);

  png_read_info (png_p, png_info_p);

  png_get_IHDR (png_p, png_info_p,
                &width, &height,
                &bit_depth, &color_type,
                &interlace_type, &compression_method, &filter_method);
  width_out = width; height_out = height;
  hasAlpha_out = (color_type == PNG_COLOR_TYPE_RGB_ALPHA);

  // update the png info struct
  png_read_update_info (png_p, png_info_p);

  row_bytes = png_get_rowbytes (png_p, png_info_p);
  // glTexImage2d requires rows to be 4-byte aligned
  row_bytes += 3 - ((row_bytes - 1) % 4);

  data_out = static_cast<GLubyte*> (malloc (row_bytes * height_out));
  if (!data_out)
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to allocate memory: \"%m\", aborting\n")));
    goto error;
  } // end IF

  // row_pointers is for pointing to image_data for reading the png with libpng
  row_pointers_pp =
    static_cast<png_bytepp> (malloc (sizeof (png_bytep) * height_out));
  if (!row_pointers_pp)
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to allocate memory: \"%m\", aborting\n")));
    goto error;
  }
  // set the individual row_pointers to point at the correct offsets of
  // image_data
  // *NOTE*: png is ordered top-to-bottom; OpenGL expects it bottom-to-top
  //         --> swap the order
  for (unsigned int i = 0; i < height_out; ++i)
    row_pointers_pp[height_out - 1 - i] = data_out + (i * row_bytes);

  png_read_image (png_p, row_pointers_pp);

  // clean up after the read, and free any memory allocated
  free (row_pointers_pp);
  png_destroy_read_struct (&png_p, &png_info_p, NULL);

  // close the file
  result = ACE_OS::fclose (file_p);
  if (result == -1)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::fclose(): \"%m\", continuing\n")));

  return true;

error:
  if (png_p)
    png_destroy_read_struct (&png_p, &png_info_p, NULL);

  if (file_p)
  {
    result = ACE_OS::fclose (file_p);
    if (result == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_OS::fclose(): \"%m\", continuing\n")));
  } // end IF

  return false;
}

void
Common_GL_Tools::drawCube (bool setTextureCoordinates_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_GL_Tools::drawCube"));

  glBegin (GL_QUADS);

  // Front Face
  if (setTextureCoordinates_in)
    glTexCoord2f (0.0f, 0.0f);
  glVertex3f (-1.0f, -1.0f,  1.0f); // Bottom Left Of The Texture and Quad
  if (setTextureCoordinates_in)
    glTexCoord2f (1.0f, 0.0f);
  glVertex3f ( 1.0f, -1.0f,  1.0f); // Bottom Right Of The Texture and Quad
  if (setTextureCoordinates_in)
    glTexCoord2f (1.0f, 1.0f);
  glVertex3f ( 1.0f,  1.0f,  1.0f); // Top Right Of The Texture and Quad
  if (setTextureCoordinates_in)
    glTexCoord2f (0.0f, 1.0f);
  glVertex3f (-1.0f,  1.0f,  1.0f); // Top Left Of The Texture and Quad

  // Back Face
  if (setTextureCoordinates_in)
    glTexCoord2f (1.0f, 0.0f);
  glVertex3f (-1.0f, -1.0f, -1.0f); // Bottom Right Of The Texture and Quad
  if (setTextureCoordinates_in)
    glTexCoord2f (1.0f, 1.0f);
  glVertex3f (-1.0f,  1.0f, -1.0f); // Top Right Of The Texture and Quad
  if (setTextureCoordinates_in)
    glTexCoord2f (0.0f, 1.0f);
  glVertex3f ( 1.0f,  1.0f, -1.0f); // Top Left Of The Texture and Quad
  if (setTextureCoordinates_in)
    glTexCoord2f (0.0f, 0.0f);
  glVertex3f ( 1.0f, -1.0f, -1.0f); // Bottom Left Of The Texture and Quad

  // Top Face
  if (setTextureCoordinates_in)
    glTexCoord2f (0.0f, 1.0f);
  glVertex3f (-1.0f,  1.0f, -1.0f); // Top Left Of The Texture and Quad
  if (setTextureCoordinates_in)
    glTexCoord2f (0.0f, 0.0f);
  glVertex3f (-1.0f,  1.0f,  1.0f); // Bottom Left Of The Texture and Quad
  if (setTextureCoordinates_in)
    glTexCoord2f (1.0f, 0.0f);
  glVertex3f ( 1.0f,  1.0f,  1.0f); // Bottom Right Of The Texture and Quad
  if (setTextureCoordinates_in)
    glTexCoord2f (1.0f, 1.0f);
  glVertex3f ( 1.0f,  1.0f, -1.0f); // Top Right Of The Texture and Quad

  // Bottom Face
  if (setTextureCoordinates_in)
    glTexCoord2f (1.0f, 1.0f);
  glVertex3f (-1.0f, -1.0f, -1.0f); // Top Right Of The Texture and Quad
  if (setTextureCoordinates_in)
    glTexCoord2f (0.0f, 1.0f);
  glVertex3f ( 1.0f, -1.0f, -1.0f); // Top Left Of The Texture and Quad
  if (setTextureCoordinates_in)
    glTexCoord2f (0.0f, 0.0f);
  glVertex3f ( 1.0f, -1.0f,  1.0f); // Bottom Left Of The Texture and Quad
  if (setTextureCoordinates_in)
    glTexCoord2f (1.0f, 0.0f);
  glVertex3f (-1.0f, -1.0f,  1.0f); // Bottom Right Of The Texture and Quad

  // Right face
  if (setTextureCoordinates_in)
    glTexCoord2f (1.0f, 0.0f);
  glVertex3f ( 1.0f, -1.0f, -1.0f); // Bottom Right Of The Texture and Quad
  if (setTextureCoordinates_in)
    glTexCoord2f (1.0f, 1.0f);
  glVertex3f ( 1.0f,  1.0f, -1.0f); // Top Right Of The Texture and Quad
  if (setTextureCoordinates_in)
    glTexCoord2f (0.0f, 1.0f);
  glVertex3f ( 1.0f,  1.0f,  1.0f); // Top Left Of The Texture and Quad
  if (setTextureCoordinates_in)
    glTexCoord2f (0.0f, 0.0f);
  glVertex3f ( 1.0f, -1.0f,  1.0f); // Bottom Left Of The Texture and Quad

  // Left Face
  if (setTextureCoordinates_in)
    glTexCoord2f (0.0f, 0.0f);
  glVertex3f (-1.0f, -1.0f, -1.0f); // Bottom Left Of The Texture and Quad
  if (setTextureCoordinates_in)
    glTexCoord2f (1.0f, 0.0f);
  glVertex3f (-1.0f, -1.0f,  1.0f); // Bottom Right Of The Texture and Quad
  if (setTextureCoordinates_in)
    glTexCoord2f (1.0f, 1.0f);
  glVertex3f (-1.0f,  1.0f,  1.0f); // Top Right Of The Texture and Quad
  if (setTextureCoordinates_in)
    glTexCoord2f (0.0f, 1.0f);
  glVertex3f (-1.0f,  1.0f, -1.0f); // Top Left Of The Texture and Quad

  glEnd ();
}
