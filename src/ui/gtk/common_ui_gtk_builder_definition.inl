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

#include "ace/ACE.h"
#include "ace/Guard_T.h"
#include "ace/Log_Msg.h"
#include "ace/Reverse_Lock_T.h"
#include "ace/Synch_Traits.h"

#include "gtk/gtk.h"

#include "common_file_tools.h"
#include "common_macros.h"

#include "common_ui_gtk_common.h"

template <typename StateType>
Common_UI_GtkBuilderDefinition_T<StateType>::Common_UI_GtkBuilderDefinition_T ()
 : state_ (NULL)
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_GtkBuilderDefinition_T::Common_UI_GtkBuilderDefinition_T"));

}

template <typename StateType>
Common_UI_GtkBuilderDefinition_T<StateType>::~Common_UI_GtkBuilderDefinition_T ()
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_GtkBuilderDefinition_T::~Common_UI_GtkBuilderDefinition_T"));

  // sanity check(s)
  if (unlikely (!state_))
    return;

  finalize (true);
}

template <typename StateType>
bool
Common_UI_GtkBuilderDefinition_T<StateType>::initialize (StateType& state_inout)
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_GtkBuilderDefinition_T::initialize"));

  state_ = &state_inout;

  // step1: load widget tree(s)
  GtkBuilder* builder_p = NULL;
  GError* error_p = NULL;
  { ACE_GUARD_RETURN (ACE_SYNCH_MUTEX, aGuard, state_->lock, false);
    for (Common_UI_GTK_BuildersIterator_t iterator = state_->builders.begin ();
         iterator != state_->builders.end ();
         iterator++)
    { ACE_ASSERT (!(*iterator).second.second);
      ACE_ASSERT (!builder_p);

      builder_p = gtk_builder_new ();
      if (unlikely (!builder_p))
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to gtk_builder_new(): \"%m\", aborting\n")));
        return false;
      } // end IF

#if GTK_CHECK_VERSION (4,0,0)
      GObject* object_p = G_OBJECT (g_object_new (G_TYPE_OBJECT, NULL));
      g_object_set_data (object_p, ACE_TEXT_ALWAYS_CHAR ("data"), (gpointer)state_->userData);

      gtk_builder_set_current_object (builder_p, object_p);
#endif // GTK_CHECK_VERSION (4,0,0)

      gtk_builder_add_from_file (builder_p,                         // builder handle
                                 (*iterator).second.first.c_str (), // definition file,
                                 &error_p);                         // error
      if (unlikely (error_p))
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to gtk_builder_add_from_file(\"%s\"): \"%s\", aborting\n"),
                    ACE::basename (ACE_TEXT ((*iterator).second.first.c_str ()), ACE_DIRECTORY_SEPARATOR_CHAR),
                    ACE_TEXT (error_p->message)));
        g_error_free (error_p); error_p = NULL;
        g_object_unref (G_OBJECT (builder_p)); builder_p = NULL;
        return false;
      } // end IF
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("loaded widget tree \"%s\": \"%s\"\n"),
                  ACE_TEXT ((*iterator).first.c_str ()),
                  ACE_TEXT ((*iterator).second.first.c_str ())));
      state_->builders[(*iterator).first] =
        std::make_pair ((*iterator).second.first, builder_p);
      builder_p = NULL;
    } // end FOR
  } // end lock scope

  return true;
}

template <typename StateType>
void
Common_UI_GtkBuilderDefinition_T<StateType>::finalize (bool clearPendingEventSources_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_GtkBuilderDefinition_T::finalize"));

  if (unlikely (!state_))
    return; // not initialized, nothing to do

  gboolean result = false;
  { ACE_GUARD (ACE_SYNCH_MUTEX, aGuard, state_->lock);
    // step1: free widget tree(s)
    for (Common_UI_GTK_BuildersIterator_t iterator = state_->builders.begin ();
         iterator != state_->builders.end ();
         iterator++)
      if (likely ((*iterator).second.second))
      {
        g_object_unref (G_OBJECT ((*iterator).second.second)); (*iterator).second.second = NULL;
      } // end IF
    state_->builders.clear ();

    // step2: clear active events ?
    if (likely (clearPendingEventSources_in))
    {
      // *TODO*: map source ids to the corresponding builder
      for (Common_UI_GTK_EventSourceIdsIterator_t iterator = state_->eventSourceIds.begin ();
           iterator != state_->eventSourceIds.end ();
           iterator++)
      {
        result = g_source_remove (*iterator);
        ACE_UNUSED_ARG (result);
      } // end FOR
      state_->eventSourceIds.clear ();
    } // end IF
  } // end lock scope

  state_ = NULL;
}
