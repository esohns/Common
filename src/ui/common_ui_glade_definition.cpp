/***************************************************************************
 *   Copyright (C) 2010 by Erik Sohns   *
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
#include "stdafx.h"

#include "common_ui_glade_definition.h"

#include "ace/Log_Msg.h"
#include "ace/Synch.h"

#include "glade/glade.h"
#include "gtk/gtk.h"

#include "common_file_tools.h"

#include "common_macros.h"

Common_UI_GladeDefinition::Common_UI_GladeDefinition (int argc_in,
                                                      ACE_TCHAR** argv_in)
 : argc_ (argc_in)
 , argv_ (argv_in)
 , GTKState_ (NULL)
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_GladeDefinition::Common_UI_GladeDefinition"));

  ACE_ASSERT (GTKState_);
}

Common_UI_GladeDefinition::~Common_UI_GladeDefinition ()
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_GladeDefinition::~Common_UI_GladeDefinition"));

}

bool
Common_UI_GladeDefinition::initialize (const std::string& filename_in,
                                       Common_UI_GTKState& GTKState_inout)
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_GladeDefinition::initialize"));

  // sanity check(s)
  if (!Common_File_Tools::isReadable (filename_in.c_str ()))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("UI definition file \"%s\" doesn't exist, aborting\n"),
                ACE_TEXT (filename_in.c_str ())));
    return false;
  } // end IF

  GTKState_ = &GTKState_inout;

  ACE_Guard<ACE_Thread_Mutex> aGuard (GTKState_inout.lock);

  // step1: load widget tree
  ACE_ASSERT (!GTKState_inout.XML);
  GTKState_inout.XML = glade_xml_new (filename_in.c_str (), // definition file
                                      NULL,                 // root widget --> construct all
                                      NULL);                // domain
  if (!GTKState_inout.XML)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to glade_xml_new(\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (filename_in.c_str ())));
    return false;
  } // end IF

  // step2: schedule UI initialization
  guint event_source_id = g_idle_add (GTKState_inout.InitializationHook,
                                      &GTKState_inout);
  if (event_source_id == 0)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to g_idle_add(): \"%m\", aborting\n")));
    return false;
  } // end IF
  else
    GTKState_inout.eventSourceIds.push_back (event_source_id);

  return true;
}

void
Common_UI_GladeDefinition::finalize ()
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_GladeDefinition::finalize"));

  // sanity check(s)
  ACE_ASSERT (GTKState_);

  ACE_Guard<ACE_Thread_Mutex> aGuard (GTKState_->lock);

  // schedule UI finalization
  gpointer user_data_p = GTKState_;
  guint event_source_id = g_idle_add (GTKState_->FinalizationHook,
                                      user_data_p);
  if (event_source_id == 0)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to g_idle_add(): \"%m\", continuing\n")));
  else
    GTKState_->eventSourceIds.push_back (event_source_id);
}
