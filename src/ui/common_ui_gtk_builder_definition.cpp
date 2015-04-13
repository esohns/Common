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

#include "common_ui_gtk_builder_definition.h"

#include "ace/Log_Msg.h"
#include "ace/Synch.h"

#include "gtk/gtk.h"

#include "common_file_tools.h"

#include "common_macros.h"

Common_UI_GtkBuilderDefinition::Common_UI_GtkBuilderDefinition (int argc_in,
                                                                ACE_TCHAR** argv_in)
 : argc_ (argc_in)
 , argv_ (argv_in)
 , GTKState_ (NULL)
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_GtkBuilderDefinition::Common_UI_GtkBuilderDefinition"));

  ACE_ASSERT (GTKState_);
}

Common_UI_GtkBuilderDefinition::~Common_UI_GtkBuilderDefinition ()
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_GtkBuilderDefinition::~Common_UI_GtkBuilderDefinition"));

}

bool
Common_UI_GtkBuilderDefinition::initialize (const std::string& filename_in,
                                            Common_UI_GTKState& GTKState_inout)
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_GtkBuilderDefinition::initialize"));

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

  // sanity check(s)
  if (!GTKState_->builder)
    GTKState_->builder = gtk_builder_new ();
  ACE_ASSERT (GTKState_inout.builder);

  // step1: load widget tree
  GError* error = NULL;
  gtk_builder_add_from_file (GTKState_->builder,
                             filename_in.c_str (),
                             &error);
  if (error)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to gtk_builder_add_from_file(\"%s\"): \"%s\", aborting\n"),
                ACE_TEXT (filename_in.c_str ()),
                ACE_TEXT (error->message)));

    // clean up
    g_error_free (error);

    return false;
  } // end IF

  // step2: schedule UI initialization
  guint event_source_id = g_idle_add (GTKState_inout.InitializationHook,
                                      GTKState_inout.CBUserData);
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
Common_UI_GtkBuilderDefinition::finalize ()
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_GtkBuilderDefinition::finalize"));

  // sanity check(s)
  ACE_ASSERT (GTKState_);

  ACE_Guard<ACE_Thread_Mutex> aGuard (GTKState_->lock);

  // schedule UI finalization
  guint event_source_id = g_idle_add (GTKState_->FinalizationHook,
                                      GTKState_->CBUserData);
  if (event_source_id == 0)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to g_idle_add(): \"%m\", continuing\n")));
  else
    GTKState_->eventSourceIds.push_back (event_source_id);
}
