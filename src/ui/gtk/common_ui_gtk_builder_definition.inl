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

template <typename StateType,
          typename CallBackDataType>
Common_UI_GtkBuilderDefinition_T<StateType,
                                 CallBackDataType>::Common_UI_GtkBuilderDefinition_T (int argc_in,
                                                                                      ACE_TCHAR** argv_in,
                                                                                      CallBackDataType* CBData_in)
 : argc_ (argc_in)
 , argv_ (argv_in)
 , CBData_ (CBData_in)
 , state_ (NULL)
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_GtkBuilderDefinition_T::Common_UI_GtkBuilderDefinition_T"));

}

template <typename StateType,
          typename CallBackDataType>
Common_UI_GtkBuilderDefinition_T<StateType,
                                 CallBackDataType>::~Common_UI_GtkBuilderDefinition_T ()
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_GtkBuilderDefinition_T::~Common_UI_GtkBuilderDefinition_T"));

  if (unlikely (!state_))
    goto continue_;

  { ACE_GUARD (ACE_SYNCH_MUTEX, aGuard, state_->lock);
    // step1: free widget tree(s)
    for (Common_UI_GTK_BuildersIterator_t iterator = state_->builders.begin ();
         iterator != state_->builders.end ();
         iterator++)
      if (likely ((*iterator).second.second))
      {
        g_object_unref (G_OBJECT ((*iterator).second.second));
        (*iterator).second.second = NULL;
      } // end IF

    // step2: clear active events
    gboolean result = false;
    // *TODO*: map source ids to the corresponding builder
    for (Common_UI_GTK_EventSourceIdsIterator_t iterator = state_->eventSourceIds.begin ();
         iterator != state_->eventSourceIds.end ();
         iterator++)
    {
      result = g_source_remove (*iterator);
      ACE_UNUSED_ARG (result);
    } // end FOR
  } // end lock scope

continue_:
  return;
}

template <typename StateType,
          typename CallBackDataType>
bool
Common_UI_GtkBuilderDefinition_T<StateType,
                                 CallBackDataType>::initialize (StateType& state_inout)
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

      gtk_builder_add_from_file (builder_p,                         // builder handle
                                 (*iterator).second.first.c_str (), // definition file,
                                 &error_p);                         // error
      if (unlikely (error_p))
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to gtk_builder_add_from_file(\"%s\"): \"%s\", aborting\n"),
                    ACE_TEXT (ACE::basename (ACE_TEXT ((*iterator).second.first.c_str ()), ACE_DIRECTORY_SEPARATOR_CHAR)),
                    ACE_TEXT (error_p->message)));
        g_error_free (error_p); error_p = NULL;
        g_object_unref (G_OBJECT (builder_p)); builder_p = NULL;
        return false;
      } // end IF
#if defined (_DEBUG)
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("loaded widget tree \"%s\": \"%s\"\n"),
                  ACE_TEXT ((*iterator).first.c_str ()),
                  ACE_TEXT ((*iterator).second.first.c_str ())));
#endif // _DEBUG
      state_->builders[(*iterator).first] =
        std::make_pair ((*iterator).second.first, builder_p);
      builder_p = NULL;
    } // end FOR

    // step2: schedule UI initialization

    // sanity check(s)
    ACE_ASSERT (state_->eventHooks.initHook);

    guint event_source_id = g_idle_add (state_->eventHooks.initHook,
                                        CBData_);
    if (unlikely (!event_source_id))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to g_idle_add(): \"%m\", aborting\n")));
      return false;
    } // end IF
    state_->eventSourceIds.insert (event_source_id);
  } // end lock scope

  return true;
}

template <typename StateType,
          typename CallBackDataType>
void
Common_UI_GtkBuilderDefinition_T<StateType,
                                 CallBackDataType>::finalize ()
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_GtkBuilderDefinition_T::finalize"));

  if (unlikely (!state_))
    return; // not initialized, nothing to do

  // schedule UI finalization

  // sanity check(s)
  ACE_ASSERT (state_->eventHooks.finiHook);

  ACE_Reverse_Lock<ACE_SYNCH_MUTEX> reverse_lock (state_->lock);
  guint event_source_id = 0;
  { ACE_GUARD (ACE_SYNCH_MUTEX, aGuard, state_->lock);
    event_source_id = g_idle_add (state_->eventHooks.finiHook,
                                  CBData_);
    if (unlikely (!event_source_id))
    { ACE_GUARD (ACE_Reverse_Lock<ACE_SYNCH_MUTEX>, aGuard_2, reverse_lock);
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to g_idle_add(): \"%m\", returning")));
      return;
    } // end IF
    state_->eventSourceIds.insert (event_source_id);
  } // end lock scope
}
