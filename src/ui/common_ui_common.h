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

#include "ace/Synch.h"

#include "glade/glade.h"
#include "gtk/gtk.h"

#include "common_ui_igtk.h"

typedef std::list<guint> Common_UI_GTKEventSourceIds_t;
typedef Common_UI_GTKEventSourceIds_t::const_iterator Common_UI_GTKEventSourceIdsIterator_t;

struct Common_UI_GTKState
{
  Common_UI_GTKEventSourceIds_t eventSourceIds;
  ACE_Thread_Mutex              lock;
  void*                         CBUserData;
  GladeXML*                     XML;
  GSourceFunc                   InitializationHook;
  GSourceFunc                   FinalizationHook;
};

typedef Common_UI_IGTK_T<Common_UI_GTKState> Common_UI_IGTK_t;

#endif
