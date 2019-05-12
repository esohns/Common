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

#ifndef COMMON_GL_ASSIMP_TOOLS_H
#define COMMON_GL_ASSIMP_TOOLS_H

#include <string>

#include "assimp/color4.h"
#include "assimp/matrix4x4.h"
#include "assimp/vector3.h"

#include "ace/Global_Macros.h"

// forward declarations
struct aiMaterial;
struct aiNode;
struct aiScene;

class Common_GL_Assimp_Tools
{
 public:
  static void boundingBox (const struct aiScene*, // scene handle
                           const struct aiNode*,  // node handle
                           aiVector3D*,           // return value: min
                           aiVector3D*,           // return value: max
                           aiMatrix4x4*);         // return value: transformation
  static void boundingBox (const struct aiScene*, // scene handle
                           aiVector3D*,           // return value: min
                           aiVector3D*);          // return value: max
  static bool loadModel (const std::string&, // path
                         struct aiScene*&);  // return value: scene handle

  inline static void color4_to_float4 (const aiColor4D* c, float f[4]) { f[0] = c->r; f[1] = c->g; f[2] = c->b; f[3] = c->a; }
  inline static void set_float4 (float f[4], float a, float b, float c, float d) { f[0] = a; f[1] = b; f[2] = c; f[3] = d; }
  static void applyMaterial (const struct aiMaterial*); // material handle
  static void render (const struct aiScene*, // scene handle
                      const struct aiNode*); // node handle

 private:
  ACE_UNIMPLEMENTED_FUNC (Common_GL_Assimp_Tools ())
  ACE_UNIMPLEMENTED_FUNC (~Common_GL_Assimp_Tools ())
  ACE_UNIMPLEMENTED_FUNC (Common_GL_Assimp_Tools (const Common_GL_Assimp_Tools&))
  ACE_UNIMPLEMENTED_FUNC (Common_GL_Assimp_Tools& operator= (const Common_GL_Assimp_Tools&))
};

#endif
