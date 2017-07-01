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

#ifndef COMMON_UI_GTK_COMMON_H
#define COMMON_UI_GTK_COMMON_H

#include <list>
#include <map>
#include <set>
#include <string>
#include <utility>

#include "ace/Synch_Traits.h"

#include <gtk/gtk.h>
#if defined (GTKGL_SUPPORT)
#if GTK_CHECK_VERSION (3,0,0)
//#include <gtkgl/gdkgl.h>
#include <gtkgl/gtkglarea.h>
#else
#if defined (GTKGLAREA_SUPPORT)
#include <gtkgl/gtkglarea.h>
#else
#include <gdk/gdkgl.h>
#endif /* GTKGLAREA_SUPPORT */
#endif /* GTK_CHECK_VERSION (3,0,0) */
#endif /* GTKGL_SUPPORT */

#include "common_ui_common.h"
#include "common_ui_igtk.h"

// forward declarations
#if defined (LIGBGLADE_SUPPORT)
struct _GladeXML;
#endif

//enum Common_UI_GTKDefinitionType
//{
//  GTK_DEFINITIONTYPE_BUILDER = 0,
//  GTK_DEFINITIONTYPE_GLADE,
//  ///////////////////////////////////////
//  GTK_DEFINITIONTYPE_INVALID,
//  GTK_DEFINITIONTYPE_MAX
//};

typedef std::set<guint> Common_UI_GTKEventSourceIds_t;
typedef Common_UI_GTKEventSourceIds_t::iterator Common_UI_GTKEventSourceIdsIterator_t;

typedef std::list<std::string> Common_UI_GTKRCFiles_t;
typedef Common_UI_GTKRCFiles_t::const_iterator Common_UI_GTKRCFilesIterator_t;
#if GTK_CHECK_VERSION (3,0,0)
typedef std::map<std::string, GtkCssProvider*> Common_UI_GTKCSSProviders_t;
typedef Common_UI_GTKCSSProviders_t::iterator Common_UI_GTKCSSProvidersIterator_t;
#endif

typedef std::pair<std::string, GtkBuilder*> Common_UI_GTKBuilder_t;
typedef std::map<std::string, Common_UI_GTKBuilder_t> Common_UI_GTKBuilders_t;
typedef Common_UI_GTKBuilders_t::iterator Common_UI_GTKBuildersIterator_t;
typedef Common_UI_GTKBuilders_t::const_iterator Common_UI_GTKBuildersConstIterator_t;

enum Common_UI_GTKStatusContextType
{
  GTK_STATUSCONTEXT_INVALID = -1,
  //GTK_STATUSCONTEXT_GLOBAL = 0, // *NOTE*: 0 is a predefined global context id
  GTK_STATUSCONTEXT_DATA = 0,
  GTK_STATUSCONTEXT_INFORMATION,
  ///////////////////////////////////////
  GTK_STATUSCONTEXT_MAX
};
typedef std::map<enum Common_UI_GTKStatusContextType, guint> Common_UI_GTK_StatusContextIds_t;
typedef Common_UI_GTK_StatusContextIds_t::iterator Common_UI_GTK_Common_UI_GTK_StatusContextIdsIterator_t;

#if defined (LIBGLADE_SUPPORT)
typedef std::pair<std::string, struct _GladeXML*> Common_UI_GTKGladeXML_t;
typedef std::map<std::string, Common_UI_GTKGladeXML_t> Common_UI_GladeXMLs_t;
typedef Common_UI_GladeXMLs_t::iterator Common_UI_GladeXMLsIterator_t;
typedef Common_UI_GladeXMLs_t::const_iterator Common_UI_GladeXMLsConstIterator_t;
#endif

struct Common_UI_GTKState
 : Common_UI_State
{
  inline Common_UI_GTKState ()
   : Common_UI_State ()
   , argc (0)
   , argv (NULL)
   , builders ()
   , contextIds ()
//, cursor (NULL)
   , eventSourceIds ()
   , finalizationHook ()
#if defined (LIBGLADE_SUPPORT)
   , gladeXML ()
#endif
   , initializationHook ()
 #if GTK_CHECK_VERSION (3,0,0)
   , CSSProviders ()
#endif
#if defined (GTKGL_SUPPORT)
#if GTK_CHECK_VERSION (3,16,0)
   , OpenGLWindow (NULL)
#else
#if defined (GTKGLAREA_SUPPORT)
   , OpenGLWindow (NULL)
#else
   , OpenGLContext (NULL)
//, openGLDrawable (NULL)
   , OpenGLWindow (NULL)
#endif /* GTKGLAREA_SUPPORT */
#endif /* GTK_CHECK_VERSION (3,0,0) */
#endif /* GTKGL_SUPPORT */
   , RCFiles ()
   ///////////////////////////////////////
   , userData (NULL)
  {};

  int                              argc;
  ACE_TCHAR**                      argv;
  Common_UI_GTKBuilders_t          builders;
  Common_UI_GTK_StatusContextIds_t contextIds; // status bar context ids
//GdkCursor*                    cursor;
  Common_UI_GTKEventSourceIds_t    eventSourceIds;
  GSourceFunc                      finalizationHook;
#if defined (LIBGLADE_SUPPORT)
  Common_UI_GladeXMLs_t            gladeXML;
#endif
  GSourceFunc                      initializationHook;
#if GTK_CHECK_VERSION (3,0,0)
  Common_UI_GTKCSSProviders_t      CSSProviders;
#endif
#if defined (GTKGL_SUPPORT)
#if GTK_CHECK_VERSION (3,0,0)
#if GTK_CHECK_VERSION (3,16,0)
  GtkGLArea*                       OpenGLWindow;
#else
  GglaArea*                        OpenGLWindow;
#endif /* GTK_CHECK_VERSION (3,16,0) */
#else /* GTK_CHECK_VERSION (3,0,0) */
#if defined (GTKGLAREA_SUPPORT)
  GtkGLArea*                       OpenGLWindow;
#else
  // *TODO*: as an application may support multiple OpenGL-capable windows, and
  //         each GdkGLContext is tied to a GdkWindow, it probably makes sense
  //         to move all of this into a separate 'presentation manager' object
  GdkGLContext*                    OpenGLContext;
  //GdkGLDrawable* openGLDrawable;
  GdkWindow*                       OpenGLWindow;
#endif /* GTKGLAREA_SUPPORT */
#endif /* GTK_CHECK_VERSION (3,0,0) */
#endif /* GTKGL_SUPPORT */
  Common_UI_GTKRCFiles_t           RCFiles;

  ////////////////////////////////////////

  gpointer                         userData; // cb user data
};

typedef Common_UI_IGTK_T<struct Common_UI_GTKState> Common_UI_IGTK_t;

//////////////////////////////////////////

//#if defined (GTKGL_SUPPORT)
//struct Common_UI_GTKGLState
// : Common_UI_GTKState
//{
//  inline Common_UI_GTKGLState ()
//   : Common_UI_GTKState ()
//  {};
//
//};
//#endif

//////////////////////////////////////////

typedef std::map<guint, ACE_Thread_ID> Common_UI_GTK_PendingActions_t;
typedef Common_UI_GTK_PendingActions_t::iterator Common_UI_GTK_PendingActionsIterator_t;
typedef std::set<guint> Common_UI_GTK_CompletedActions_t;
typedef Common_UI_GTK_CompletedActions_t::iterator Common_UI_GTK_CompletedActionsIterator_t;

struct Common_UI_GTK_ProgressData
{
  inline Common_UI_GTK_ProgressData ()
   : completedActions ()
   //   , cursorType (GDK_LAST_CURSOR)
   , eventSourceID (0)
   , GTKState (NULL)
   , pendingActions ()
  {};

  Common_UI_GTK_CompletedActions_t completedActions;
  //  GdkCursorType                      cursorType;
  guint                            eventSourceID;
  struct Common_UI_GTKState*       GTKState;
  Common_UI_GTK_PendingActions_t   pendingActions;
};

void common_ui_gtk_opengl_cb (gpointer);

#endif
