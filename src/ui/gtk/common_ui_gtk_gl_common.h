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

#ifndef COMMON_UI_GTK_GL_COMMON_H
#define COMMON_UI_GTK_GL_COMMON_H

#include <map>

#include "gtk/gtk.h"
#if GTK_CHECK_VERSION (3,0,0)
#if GTK_CHECK_VERSION (3,16,0)
#else
#if defined (GTKGLAREA_SUPPORT)
//#include "gtkgl/gdkgl.h"
#include "gtkgl/gtkglarea.h"
#endif /* GTKGLAREA_SUPPORT */
#endif /* GTK_CHECK_VERSION (3,16,0) */
#else
#if defined (GTKGLAREA_SUPPORT)
#include "gtkgl/gtkglarea.h"
#else
#include "gdk/gdkgl.h"
#endif /* GTKGLAREA_SUPPORT */
#endif /* GTK_CHECK_VERSION (3,0,0) */

#include "common_ui_gtk_common.h"

typedef std::map<GdkWindow*, GdkGLContext*> Common_UI_GTK_GLContexts_t;
typedef Common_UI_GTK_GLContexts_t::iterator Common_UI_GTK_GLContextsIterator_t;

struct Common_UI_GTK_GLState
 : Common_UI_GTK_State
{
  Common_UI_GTK_GLState ()
   : Common_UI_GTK_State ()
   , OpenGLContexts ()
   , OpenGLWindow (NULL)
#if GTK_CHECK_VERSION (3,0,0)
#if GTK_CHECK_VERSION (3,16,0)
#endif /* GTK_CHECK_VERSION (3,16,0) */
#else
#if defined (GTKGLAREA_SUPPORT)
#else
//, OpenGLDrawable (NULL)
#endif /* GTKGLAREA_SUPPORT */
#endif /* GTK_CHECK_VERSION (3,0,0) */
  {};

  // *TODO*: as an application may support multiple OpenGL-capable windows, and
  //         each GdkGLContext is tied to a specific GdkWindow, move all of this
  //         into a separate 'presentation manager' object ASAP
  Common_UI_GTK_GLContexts_t OpenGLContexts;
#if GTK_CHECK_VERSION (3,0,0)
#if GTK_CHECK_VERSION (3,16,0)
  GdkWindow*                 OpenGLWindow;
#else
  GglaArea*                  OpenGLWindow;
#endif /* GTK_CHECK_VERSION (3,16,0) */
#else /* GTK_CHECK_VERSION (3,0,0) */
#if defined (GTKGLAREA_SUPPORT)
  GtkGLArea*                 OpenGLWindow;
#else
//GdkGLDrawable*               OpenGLDrawable;
  GdkWindow*                 OpenGLWindow;
#endif /* GTKGLAREA_SUPPORT */
#endif /* GTK_CHECK_VERSION (3,0,0) */
};

//void common_ui_gtk_opengl_cb (gpointer);

#endif
