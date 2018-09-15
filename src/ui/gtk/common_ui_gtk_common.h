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

#include "gtk/gtk.h"

#include "ace/Synch_Traits.h"

#include "common_ui_common.h"
#include "common_ui_idefinition.h"

// forward declarations
#if defined (LIGBGLADE_SUPPORT)
struct _GladeXML;
#endif // LIGBGLADE_SUPPORT

//enum Common_UI_GTK_InterfaceDefinitionType : int
enum Common_UI_GTK_InterfaceDefinitionType
{
  COMMON_UI_GTK_DEFINITIONTYPE_BUILDER = 0,
#if defined (LIGBGLADE_SUPPORT)
  COMMON_UI_GTK_DEFINITIONTYPE_GLADE,
#endif // LIGBGLADE_SUPPORT
  ///////////////////////////////////////
  COMMON_UI_GTK_DEFINITIONTYPE_INVALID,
  COMMON_UI_GTK_DEFINITIONTYPE_MAX
};

typedef std::set<guint> Common_UI_GTK_EventSourceIds_t;
typedef Common_UI_GTK_EventSourceIds_t::iterator Common_UI_GTK_EventSourceIdsIterator_t;

typedef std::list<std::string> Common_UI_GTK_RCFiles_t;
typedef Common_UI_GTK_RCFiles_t::const_iterator Common_UI_GTK_RCFilesIterator_t;
#if GTK_CHECK_VERSION(3,0,0)
typedef std::map<std::string, GtkCssProvider*> Common_UI_GTK_CSSProviders_t;
typedef Common_UI_GTK_CSSProviders_t::iterator Common_UI_GTK_CSSProvidersIterator_t;
#endif // GTK_CHECK_VERSION(3,0,0)

typedef std::pair<std::string, GtkBuilder*> Common_UI_GTK_Builder_t;
typedef std::map<std::string, Common_UI_GTK_Builder_t> Common_UI_GTK_Builders_t;
typedef Common_UI_GTK_Builders_t::iterator Common_UI_GTK_BuildersIterator_t;
typedef Common_UI_GTK_Builders_t::const_iterator Common_UI_GTK_BuildersConstIterator_t;

//enum Common_UI_GTK_StatusContextType : int
enum Common_UI_GTK_StatusContextType
{
  COMMON_UI_GTK_STATUSCONTEXT_INVALID = -1,
  // *NOTE*: '0' is also a predefined global context id
  //GTK_STATUSCONTEXT_GLOBAL  = 0,
  COMMON_UI_GTK_STATUSCONTEXT_DATA,
  COMMON_UI_GTK_STATUSCONTEXT_INFORMATION,
  ///////////////////////////////////////
  COMMON_UI_GTK_STATUSCONTEXT_MAX
};
typedef std::map<enum Common_UI_GTK_StatusContextType, guint> Common_UI_GTK_StatusContextIds_t;
typedef Common_UI_GTK_StatusContextIds_t::iterator Common_UI_GTK_Common_UI_GTK_StatusContextIdsIterator_t;

#if defined (LIBGLADE_SUPPORT)
typedef std::pair<std::string, struct _GladeXML*> Common_UI_GladeXML_t;
typedef std::map<std::string, Common_UI_GladeXML_t> Common_UI_GladeXMLs_t;
typedef Common_UI_GladeXMLs_t::iterator Common_UI_GladeXMLsIterator_t;
typedef Common_UI_GladeXMLs_t::const_iterator Common_UI_GladeXMLsConstIterator_t;
#endif // LIBGLADE_SUPPORT

struct Common_UI_GTK_EventHookConfiguration
{
  Common_UI_GTK_EventHookConfiguration ()
   : dataHook (NULL)
   , eventHook (NULL)
   , initHook (NULL)
   , finiHook (NULL)
   , statisticHook (NULL)
  {}

  GSourceFunc dataHook;
  GSourceFunc eventHook;
  GSourceFunc initHook;
  GSourceFunc finiHook;
  GSourceFunc statisticHook;
};

struct Common_UI_GTK_State
 : Common_UI_State
{
  Common_UI_GTK_State ()
   : Common_UI_State ()
   , argc (0)
   , argv (NULL)
   , builders ()
   , contextIds ()
#if GTK_CHECK_VERSION(3,0,0)
   , CSSProviders ()
#endif // GTK_CHECK_VERSION(3,0,0)
//, cursor (NULL)
   , eventHooks ()
   , eventSourceIds ()
#if defined (LIBGLADE_SUPPORT)
   , gladeXML ()
#endif // LIBGLADE_SUPPORT
   , RCFiles ()
   ///////////////////////////////////////
   , userData (NULL)
  {}

  int                                         argc;
  ACE_TCHAR**                                 argv;
  Common_UI_GTK_Builders_t                    builders;
  Common_UI_GTK_StatusContextIds_t            contextIds; // status bar context ids
//GdkCursor*                    cursor;
#if GTK_CHECK_VERSION(3,0,0)
  Common_UI_GTK_CSSProviders_t                CSSProviders;
#endif // GTK_CHECK_VERSION(3,0,0)
  struct Common_UI_GTK_EventHookConfiguration eventHooks;
  Common_UI_GTK_EventSourceIds_t              eventSourceIds;
#if defined (LIBGLADE_SUPPORT)
  Common_UI_GladeXMLs_t                       gladeXML;
#endif // LIBGLADE_SUPPORT
  Common_UI_GTK_RCFiles_t                     RCFiles;

  ////////////////////////////////////////

  gpointer                                    userData; // cb user data
};

//////////////////////////////////////////

#if defined (GTKGL_SUPPORT)
typedef struct Common_UI_GTK_GLState Common_UI_GTK_State_t;
#else
typedef struct Common_UI_GTK_State Common_UI_GTK_State_t;
#endif // GTKGL_SUPPORT
typedef Common_UI_IDefinition_T<Common_UI_GTK_State_t> Common_UI_GTK_IDefinition_t;

typedef std::map<guint, ACE_Thread_ID> Common_UI_GTK_PendingActions_t;
typedef Common_UI_GTK_PendingActions_t::iterator Common_UI_GTK_PendingActionsIterator_t;
typedef std::set<guint> Common_UI_GTK_CompletedActions_t;
typedef Common_UI_GTK_CompletedActions_t::iterator Common_UI_GTK_CompletedActionsIterator_t;

struct Common_UI_GTK_ProgressData
{
  Common_UI_GTK_ProgressData ()
   : completedActions ()
//   , cursorType (GDK_LAST_CURSOR)
   , eventSourceId (0)
   , pendingActions ()
   , state (NULL)
  {}

  Common_UI_GTK_CompletedActions_t completedActions;
//  GdkCursorType                      cursorType;
  guint                            eventSourceId;
  Common_UI_GTK_PendingActions_t   pendingActions;
  Common_UI_GTK_State_t*           state;
};

#endif
