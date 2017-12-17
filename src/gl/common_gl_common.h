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

#ifndef COMMON_GL_COMMON_H
#define COMMON_GL_COMMON_H

#include <utility>

#include "glm/glm.hpp"

#include "ace/OS.h"

#include "common_gl_defines.h"

typedef std::pair<glm::vec3, glm::vec3> Common_GL_BoundingBox_t;

struct Common_GL_Camera
{
  Common_GL_Camera ()
   : zoom (0.0F)
   , rotation ()
   , translation ()
  {
    ACE_OS::memset (last, 0, sizeof (int[2]));
  };

  //glm::vec3 position;
  //glm::vec3 looking_at;
  //glm::vec3 up;

  float zoom;
  glm::vec3 rotation;
  glm::vec3 translation;
  int last[2];
};

struct Common_GL_Scene
{
  Common_GL_Scene ()
   : camera ()
   , boundingBox ()
   , center ()
   , orientation ()
  {};

  inline void resetCamera () { ACE_OS::memset (&camera, 0, sizeof (struct Common_GL_Camera)); camera.zoom = COMMON_GL_CAMERA_DEFAULT_ZOOM_FACTOR; }

  struct Common_GL_Camera camera;

  // scene
  Common_GL_BoundingBox_t boundingBox;
  glm::vec3               center;
  glm::vec3               orientation; // model/scene-
};

#endif
