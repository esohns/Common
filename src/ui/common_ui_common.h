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

#ifndef COMMON_UI_COMMON_H
#define COMMON_UI_COMMON_H

#include <list>
#include <map>
#include <set>
#include <string>
#include <utility>

//#include "ace/Synch_Traits.h"
#include "ace/Synch.h"

#include "gtk/gtk.h"

#include "glade/glade.h"

#include "common_ui_igtk.h"

//// forward declarations
//struct GladeXML;

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

typedef std::list<std::string> Common_UI_UIRCFiles_t;
typedef Common_UI_UIRCFiles_t::const_iterator Common_UI_UIRCFilesIterator_t;

typedef std::pair<std::string, GtkBuilder*> Common_UI_GTKBuilder_t;
typedef std::map<std::string, Common_UI_GTKBuilder_t> Common_UI_GTKBuilders_t;
typedef Common_UI_GTKBuilders_t::iterator Common_UI_GTKBuildersIterator_t;
typedef Common_UI_GTKBuilders_t::const_iterator Common_UI_GTKBuildersConstIterator_t;

typedef std::pair<std::string, GladeXML*> Common_UI_GTKGladeXML_t;
typedef std::map<std::string, Common_UI_GTKGladeXML_t> Common_UI_GladeXMLs_t;
typedef Common_UI_GladeXMLs_t::iterator Common_UI_GladeXMLsIterator_t;
typedef Common_UI_GladeXMLs_t::const_iterator Common_UI_GladeXMLsConstIterator_t;

struct Common_UI_GTKState
{
  inline Common_UI_GTKState ()
   : builders ()
   , cursor (NULL)
   , eventSourceIds ()
   , finalizationHook ()
   , gladeXML ()
   , initializationHook ()
   , lock ()
   , RCFiles ()
   //////////////////////////////////////
   , userData (NULL)
  {};

  Common_UI_GTKBuilders_t       builders;
  GdkCursor*                    cursor;
  Common_UI_GTKEventSourceIds_t eventSourceIds;
  GSourceFunc                   finalizationHook;
  Common_UI_GladeXMLs_t         gladeXML;
  GSourceFunc                   initializationHook;
  // *TODO*: remove this ASAP
  ACE_Thread_Mutex              lock;
//  ACE_SYNCH_MUTEX               lock;
  Common_UI_UIRCFiles_t         RCFiles;
  ///////////////////////////////////////
  void*                         userData;
};

typedef Common_UI_IGTK_T<Common_UI_GTKState> Common_UI_IGTK_t;

#endif
