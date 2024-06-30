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
#include "gtkgl/gdkgl.h"
#include "gtkgl/gtkglarea.h"
//#else
//#error GTK 3 supports OpenGL natively from 3.16.0 onward; for earlier versions, try gtkglarea (see: git://git.gnome.org/gtkglarea)
#endif /* GTKGLAREA_SUPPORT */
#endif /* GTK_CHECK_VERSION (3,16,0) */
#elif GTK_CHECK_VERSION (2,0,0)
#if defined (GTKGLAREA_SUPPORT)
#include "gtkgl/gdkgl.h"
#include "gtkgl/gtkglarea.h"
#endif /* GTKGLAREA_SUPPORT */
#else
#include "gdk/gdkgl.h"
#endif /* GTK_CHECK_VERSION (x,0,0) */

#if GTK_CHECK_VERSION (3,0,0)
#if GTK_CHECK_VERSION (3,24,1)
typedef std::map<GtkGLArea*, GdkGLContext*> Common_UI_GTK_GLContexts_t;
#elif GTK_CHECK_VERSION (3,16,0)
typedef std::map<GdkWindow*, GdkGLContext*> Common_UI_GTK_GLContexts_t;
#else
#if defined (GTKGLAREA_SUPPORT)
typedef std::map<GglaArea*, GglaContext*> Common_UI_GTK_GLContexts_t;
#else
typedef std::map<GdkWindow*, gpointer> Common_UI_GTK_GLContexts_t;
#endif /* GTKGLAREA_SUPPORT */
#endif /* GTK_CHECK_VERSION (3,24,1) */
#elif GTK_CHECK_VERSION (2,0,0)
#if defined (GTKGLAREA_SUPPORT)
typedef std::map<GglaArea*, GglaContext*> Common_UI_GTK_GLContexts_t;
#else
typedef std::map<GdkWindow*, gpointer> Common_UI_GTK_GLContexts_t;
#endif /* GTKGLAREA_SUPPORT */
#endif /* GTK_CHECK_VERSION (3,0,0) */
typedef Common_UI_GTK_GLContexts_t::iterator Common_UI_GTK_GLContextsIterator_t;
#endif
