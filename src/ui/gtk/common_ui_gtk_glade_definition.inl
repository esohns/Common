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

#include <fstream>

#include "gtk/gtk.h"

#include "glade/glade.h"

#include "ace/Guard_T.h"
#include "ace/Log_Msg.h"

#include "common_file_tools.h"
#include "common_macros.h"

#include "common_ui_gtk_common.h"

template <typename StateType>
Common_UI_GladeDefinition_T<StateType>::Common_UI_GladeDefinition_T ()
 : state_ (NULL)
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_GladeDefinition_T::Common_UI_GladeDefinition_T"));

}

template <typename StateType>
Common_UI_GladeDefinition_T<StateType>::~Common_UI_GladeDefinition_T ()
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_GladeDefinition_T::~Common_UI_GladeDefinition_T"));

  if (unlikely (!state_))
    return;

  finalize (true);
}

template <typename StateType>
bool
Common_UI_GladeDefinition_T<StateType>::initialize (StateType& state_inout)
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_GladeDefinition_T::initialize"));

  state_ = &state_inout;

  ACE_GUARD_RETURN (ACE_SYNCH_MUTEX, aGuard, state_inout.lock, false);

  // step1: load widget tree(s)
  struct _GladeXML* glade_XML_p = NULL;
  for (Common_UI_GladeXMLsIterator_t iterator = state_inout.gladeXML.begin ();
       iterator != state_inout.gladeXML.end ();
       iterator++)
  {
    if ((*iterator).second.second)
      continue; // already loaded

    glade_XML_p =
      glade_xml_new ((*iterator).second.first.c_str (), // definition file
                     NULL,                              // root widget --> construct all
                     NULL);                             // domain

    //// slurp whole file
    //std::ifstream file_stream ((*iterator).second.first.c_str (),
    //                           std::ios_base::in);
    //if (file_stream.fail ())
    //{
    //  ACE_DEBUG ((LM_ERROR,
    //              ACE_TEXT ("failed to open \"%s\": \"%m\", aborting\n"),
    //              ACE_TEXT ((*iterator).second.first.c_str ())));
    //  return false;
    //} // end IF
    //file_stream.seekg (0, std::ios::end);
    //char* buffer_p = NULL;
    //int size = file_stream.tellg ();
    //ACE_NEW_NORETURN (buffer_p,
    //                  char[size]);
    //if (!buffer_p)
    //{
    //  ACE_DEBUG ((LM_CRITICAL,
    //              ACE_TEXT ("failed to allocate memory: \"%m\", aborting\n")));

    //  // clean up
    //  file_stream.close ();
    //  if (file_stream.fail ())
    //    ACE_DEBUG ((LM_ERROR,
    //                ACE_TEXT ("failed to close \"%s\": \"%m\", continuing\n"),
    //                ACE_TEXT ((*iterator).second.first.c_str ())));

    //  return false;
    //} // end IF
    //file_stream.seekg (0, std::ios::beg);
    //file_stream.read (buffer_p, size);
    //if (file_stream.fail ())
    //{
    //  ACE_DEBUG ((LM_ERROR,
    //              ACE_TEXT ("failed to read \"%s\": \"%m\", aborting\n"),
    //              ACE_TEXT ((*iterator).second.first.c_str ())));

    //  // clean up
    //  file_stream.close ();
    //  if (file_stream.fail ())
    //    ACE_DEBUG ((LM_ERROR,
    //                ACE_TEXT ("failed to close \"%s\": \"%m\", continuing\n"),
    //                ACE_TEXT ((*iterator).second.first.c_str ())));

    //  return false;
    //} // end IF
    //file_stream.close ();
    //if (file_stream.fail ())
    //  ACE_DEBUG ((LM_ERROR,
    //              ACE_TEXT ("failed to close \"%s\": \"%m\", continuing\n"),
    //              ACE_TEXT ((*iterator).second.first.c_str ())));

    //glade_XML_p = glade_xml_new_from_buffer (buffer_p, size, // definition file / size
    //                                         NULL,           // root widget --> construct all
    //                                         NULL);          // domain
    if (!glade_XML_p)
    {
      ACE_DEBUG ((LM_ERROR,
                  //ACE_TEXT ("failed to glade_xml_new_from_buffer(\"%s\"): \"%m\", aborting\n"),
                  ACE_TEXT ("failed to glade_xml_new(\"%s\"): \"%m\", aborting\n"),
                  ACE_TEXT ((*iterator).second.first.c_str ())));

      //// clean up
      //delete[] buffer_p;

      return false;
    } // end IF
    //delete[] buffer_p;
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("loaded widget tree \"%s\": \"%s\"\n"),
                ACE_TEXT ((*iterator).first.c_str ()),
                ACE_TEXT ((*iterator).second.first.c_str ())));

    // connect (default) signals
    //glade_xml_signal_autoconnect (glade_XML_p);

    state_inout.gladeXML[(*iterator).first] =
      std::make_pair ((*iterator).second.first, glade_XML_p);
  } // end FOR

  return true;
}

template <typename StateType>
void
Common_UI_GladeDefinition_T<StateType>::finalize (bool clearPendingEventSources_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_GladeDefinition_T::finalize"));

  if (unlikely (!state_))
    return; // not initialized, nothing to do

  gboolean result = false;
  { ACE_GUARD (ACE_SYNCH_MUTEX, aGuard, state_->lock);
    // step1: free widget tree(s)
    for (Common_UI_GladeXMLsIterator_t iterator = state_->gladeXML.begin ();
         iterator != state_->gladeXML.end ();
         iterator++)
      if (likely ((*iterator).second.second))
      {
        g_object_unref (G_OBJECT ((*iterator).second.second)); (*iterator).second.second = NULL;
      } // end IF

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
    } // end IF
  } // end lock scope
}
