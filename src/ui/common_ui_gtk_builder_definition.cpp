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

#include "ace/Guard_T.h"
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

}

Common_UI_GtkBuilderDefinition::~Common_UI_GtkBuilderDefinition ()
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_GtkBuilderDefinition::~Common_UI_GtkBuilderDefinition"));

  // clean up
  if (GTKState_)
  {
    ACE_Guard<ACE_SYNCH_RECURSIVE_MUTEX> aGuard (GTKState_->lock);

    // step1: free widget tree(s)
    for (Common_UI_GTKBuildersIterator_t iterator = GTKState_->builders.begin ();
         iterator != GTKState_->builders.end ();
         iterator++)
      if ((*iterator).second.second)
      {
        g_object_unref (G_OBJECT ((*iterator).second.second));
        (*iterator).second.second = NULL;
      } // end IF
    // *TODO*: builder definitions to the corresponding builder
    GTKState_->builders.clear ();

    // step2: clear active events
    gboolean result = false;
    // *TODO*: map source ids to the corresponding builder
    for (Common_UI_GTKEventSourceIdsIterator_t iterator = GTKState_->eventSourceIds.begin ();
         iterator != GTKState_->eventSourceIds.end ();
         iterator++)
    {
      result = g_source_remove (*iterator);
      ACE_UNUSED_ARG (result);
    } // end FOR
    GTKState_->eventSourceIds.clear ();
  } // end IF
}

bool
Common_UI_GtkBuilderDefinition::initialize (Common_UI_GTKState& GTKState_inout)
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_GtkBuilderDefinition::initialize"));

  GTKState_ = &GTKState_inout;

  // step1: load widget tree(s)
  GtkBuilder* builder_p = NULL;
  GError* error_p = NULL;
  for (Common_UI_GTKBuildersIterator_t iterator = GTKState_inout.builders.begin ();
       iterator != GTKState_inout.builders.end ();
       iterator++)
  {
    // sanity check(s)
    ACE_ASSERT (!(*iterator).second.second);

    builder_p = gtk_builder_new ();
    if (!builder_p)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to gtk_builder_new(): \"%m\", aborting\n")));
      return false;
    } // end IF

    gtk_builder_add_from_file (builder_p,                         // builder handle
                               (*iterator).second.first.c_str (), // definition file,
                               &error_p);                         // error
    if (error_p)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to gtk_builder_add_from_file(\"%s\"): \"%s\", aborting\n"),
                  ACE_TEXT ((*iterator).second.first.c_str ()),
                  ACE_TEXT (error_p->message)));

      // clean up
      g_object_unref (G_OBJECT (builder_p));
      g_error_free (error_p);

      return false;
    } // end IF
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("loaded widget tree \"%s\": \"%s\"\n"),
                ACE_TEXT ((*iterator).first.c_str ()),
                ACE_TEXT ((*iterator).second.first.c_str ())));

    GTKState_inout.builders[(*iterator).first] =
      std::make_pair ((*iterator).second.first, builder_p);
  } // end FOR

  // step2: schedule UI initialization
  guint event_source_id = g_idle_add (GTKState_inout.initializationHook,
                                      GTKState_inout.userData);
  if (!event_source_id)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to g_idle_add(): \"%m\", aborting\n")));
    return false;
  } // end IF
  GTKState_inout.eventSourceIds.insert (event_source_id);

  return true;
}

void
Common_UI_GtkBuilderDefinition::finalize ()
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_GtkBuilderDefinition::finalize"));

  if (GTKState_)
  {
    // schedule UI finalization
    ACE_Guard<ACE_SYNCH_RECURSIVE_MUTEX> aGuard (GTKState_->lock);

    guint event_source_id = g_idle_add (GTKState_->finalizationHook,
                                        GTKState_->userData);
    if (!event_source_id)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to g_idle_add(): \"%m\", returning")));
      return;
    } // end IF
    GTKState_->eventSourceIds.insert (event_source_id);
  } // end IF
}
