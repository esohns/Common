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

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#if defined (_MSC_VER) && (_MSC_VER >= 1800)
#include "glm/glm.hpp"
#endif // _MSC_VER && (_MSC_VER >= 1800)
#else
#include "glm/glm.hpp"
#endif // ACE_WIN32 || ACE_WIN64

#include "ace/OS.h"

#include "common_gl_defines.h"

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#if defined (_MSC_VER) && (_MSC_VER >= 1800)
typedef std::pair<glm::vec3, glm::vec3> Common_GL_BoundingBox_t;
#else
struct Common_GL_VectorF3
{
  Common_GL_VectorF3 ()
   : x (0.0F)
   , y (0.0F)
   , z (0.0F)
  {}

  float x;
  float y;
  float z;
};
typedef std::pair<struct Common_GL_VectorF3, struct Common_GL_VectorF3> Common_GL_BoundingBox_t;
#endif // _MSC_VER && (_MSC_VER >= 1800)
#else
typedef std::pair<glm::vec3, glm::vec3> Common_GL_BoundingBox_t;
#endif // ACE_WIN32 || ACE_WIN64

struct Common_GL_Camera
{
  Common_GL_Camera ()
   : zoom (0.0F)
   , rotation ()
   , translation ()
  {
    ACE_OS::memset (last, 0, sizeof (int[2]));
  }

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#if defined (_MSC_VER) && (_MSC_VER >= 1800)
  //glm::vec3 position;
  //glm::vec3 looking_at;
  //glm::vec3 up;
#endif // _MSC_VER && (_MSC_VER >= 1800)
#endif // ACE_WIN32 || ACE_WIN64

  float zoom;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#if defined (_MSC_VER) && (_MSC_VER >= 1800)
  glm::vec3 rotation;
  glm::vec3 translation;
#else
  struct Common_GL_VectorF3 rotation;
  struct Common_GL_VectorF3 translation;
#endif // _MSC_VER && (_MSC_VER >= 1800)
#else
  glm::vec3 rotation;
  glm::vec3 translation;
#endif // ACE_WIN32 || ACE_WIN64
  int last[2];
};

struct Common_GL_Scene
{
  Common_GL_Scene ()
   : camera ()
   , boundingBox ()
   , center ()
   , orientation ()
  {}

  inline void resetCamera () { ACE_OS::memset (&camera, 0, sizeof (struct Common_GL_Camera)); camera.zoom = COMMON_GL_CAMERA_DEFAULT_ZOOM_FACTOR; }

  struct Common_GL_Camera camera;

  // scene
  Common_GL_BoundingBox_t boundingBox;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#if defined (_MSC_VER) && (_MSC_VER >= 1800)
  glm::vec3               center;
  glm::vec3               orientation; // model/scene-
#else
  struct Common_GL_VectorF3 center;
  struct Common_GL_VectorF3 orientation;
#endif // _MSC_VER && (_MSC_VER >= 1800)
#else
  glm::vec3               center;
  glm::vec3               orientation; // model/scene-
#endif // ACE_WIN32 || ACE_WIN64
};

#endif
