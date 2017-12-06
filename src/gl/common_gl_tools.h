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

#ifndef COMMON_GL_TOOLS_H
#define COMMON_GL_TOOLS_H

#include <string>

#include "ace/config-lite.h"
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include <gl/GL.h>
#else
#include <GL/gl.h>
#endif

#include "ace/Global_Macros.h"

//#include "common_gl_exports.h"

//class Common_GL_Export Common_GL_Tools
class Common_GL_Tools
{
 public:
  static std::string errorToString (GLenum);

 private:
  ACE_UNIMPLEMENTED_FUNC (Common_GL_Tools ())
  ACE_UNIMPLEMENTED_FUNC (~Common_GL_Tools ())
  ACE_UNIMPLEMENTED_FUNC (Common_GL_Tools (const Common_GL_Tools&))
  ACE_UNIMPLEMENTED_FUNC (Common_GL_Tools& operator= (const Common_GL_Tools&))
};

#endif
