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

#ifndef COMMON_GL_DEFINES_H
#define COMMON_GL_DEFINES_H

// *** ASSIMP ***
#define COMMON_GL_ASSIMP_LOG_FILENAME_STRING                 "assimp.log"
#define COMMON_GL_ASSIMP_MIN(x,y)                            (x<y?x:y)
#define COMMON_GL_ASSIMP_MAX(x,y)                            (y>x?y:x)

// *IMPORTANT NOTE*: do NOT use inside glBegin()/glEnd() sections
#if defined (_DEBUG)
#define COMMON_GL_ASSERT { GLenum error_e = GL_NO_ERROR; while ((error_e = glGetError ()) != GL_NO_ERROR) { ACE_DEBUG ((LM_ERROR, ACE_TEXT ("OpenGL error 0x%x: \"%s\", asserting\n"), error_e, ACE_TEXT (Common_GL_Tools::errorToString (error_e).c_str ()))); ACE_ASSERT (false); } }
#else
#define COMMON_GL_ASSERT (static_cast<void>(0))
#endif // _DEBUG

#define COMMON_GL_CLEAR_ERROR { GLenum error_e = GL_NO_ERROR; while ((error_e = glGetError ()) != GL_NO_ERROR) {} }
#define COMMON_GL_PRINT_ERROR { GLenum error_e = GL_NO_ERROR; while ((error_e = glGetError ()) != GL_NO_ERROR) { ACE_DEBUG ((LM_ERROR, ACE_TEXT ("OpenGL error 0x%x: \"%s\", continuing\n"), error_e, ACE_TEXT (Common_GL_Tools::errorToString (error_e).c_str ()))); } }

// camera
#define COMMON_GL_CAMERA_DEFAULT_ROTATION_FACTOR             0.8F
#define COMMON_GL_CAMERA_DEFAULT_TRANSLATION_FACTOR          0.01F
#define COMMON_GL_CAMERA_DEFAULT_ZOOM_FACTOR                 0.03F

#endif
