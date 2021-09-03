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

#include "ace/Guard_T.h"
#include "ace/Log_Msg.h"
#include "ace/Synch_Traits.h"

#include "wx/xrc/xmlres.h"

#include "common_file_tools.h"
#include "common_macros.h"

#include "common_ui_defines.h"
#include "common_ui_wxwidgets_common.h"

template <typename StateType,
          const char* TopLevelClassName>
Common_UI_WxWidgetsXRCDefinition_T<StateType,
                                   TopLevelClassName>::Common_UI_WxWidgetsXRCDefinition_T (const std::string& name_in)
 : name_ (name_in)
 , state_ (NULL)
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_WxWidgetsXRCDefinition_T::Common_UI_WxWidgetsXRCDefinition_T"));

}

template <typename StateType,
          const char* TopLevelClassName>
bool
Common_UI_WxWidgetsXRCDefinition_T<StateType,
                                   TopLevelClassName>::initialize (StateType& state_inout)
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_WxWidgetsXRCDefinition_T::initialize"));

  state_ = &state_inout;

  // step0: initialize XRC
  wxXmlResource* resource_p = wxXmlResource::Get ();
  if (unlikely (!resource_p))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to wxXmlResource::Get(): \"%m\", aborting\n")));
    return false;
  } // end IF
  resource_p->InitAllHandlers();

  // load widget tree(s)
  wxObject* object_p = NULL;
#if wxCHECK_VERSION(3,0,0)
  wxFileName file_name;
#elif wxCHECK_VERSION(2,0,0)
  wxString file_name;
#endif // wxCHECK_VERSION
  { ACE_GUARD_RETURN (ACE_SYNCH_MUTEX, aGuard, state_->lock, false);
    for (Common_UI_wxWidgets_XmlResourcesIterator_t iterator = state_->resources.begin ();
         iterator != state_->resources.end ();
         ++iterator)
    { ACE_ASSERT (!(*iterator).second.second);
      ACE_ASSERT (!object_p);
#if wxCHECK_VERSION(3,0,0)
      file_name.Assign (ACE_TEXT_ALWAYS_CHAR ((*iterator).second.first.c_str ()),
                        wxPATH_NATIVE);
      if (unlikely (!resource_p->LoadFile (file_name)))
#elif wxCHECK_VERSION(2,0,0)
      file_name  = ACE_TEXT_ALWAYS_WCHAR ((*iterator).second.first.c_str ());
      if (unlikely (!resource_p->Load (file_name)))
#endif // wxCHECK_VERSION
      {
#if wxCHECK_VERSION(3,0,0)
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to wxXmlResource::LoadFile(\"%s\"): \"%m\", aborting\n"),
                    ACE_TEXT ((*iterator).second.first.c_str ())));
#elif wxCHECK_VERSION(2,0,0)
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to wxXmlResource::Load(\"%s\"): \"%m\", aborting\n"),
                    ACE_TEXT ((*iterator).second.first.c_str ())));
#endif // wxCHECK_VERSION
        return false;
      } // end IF

      if (!ACE_OS::strcmp ((*iterator).first.c_str (),
                           ACE_TEXT_ALWAYS_CHAR (COMMON_UI_DEFINITION_DESCRIPTOR_MAIN)))
      {
        object_p =
            resource_p->LoadDialog (dynamic_cast<wxWindow*> (state_->instance),
                                    wxString (ACE_TEXT_ALWAYS_WCHAR (name_.c_str ())));
//        if (unlikely (!resource_p->LoadObject (state_->instance,
//                                               NULL,                                   // parent widget handle
//                                               wxString (ACE_TEXT_ALWAYS_WCHAR (name_.c_str ())),
//                                               wxString (ACE_TEXT_ALWAYS_WCHAR (TopLevelClassName)))))
//        {
//          ACE_DEBUG ((LM_ERROR,
//                      ACE_TEXT ("failed to wxXmlResource::LoadObject(0x%@,\"%s\"): \"%m\", aborting\n"),
//                      state_->instance,
//                      ACE_TEXT (name_.c_str ())));
//          return false;
//        } // end IF
//        object_p = state_->instance;
      } // end IF
      else
      {
        object_p =
          resource_p->LoadObject (NULL,                                   // parent widget handle
                                  wxString (ACE_TEXT_ALWAYS_WCHAR (name_.c_str ())),
                                  wxString (ACE_TEXT_ALWAYS_CHAR (TopLevelClassName)));
      } // end ELSE
      if (unlikely (!object_p))
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to wxXmlResource::LoadObject(\"%s\"): \"%m\", aborting\n"),
                    ACE_TEXT (name_.c_str ())));
        return false;
      } // end IF
#if defined (_DEBUG)
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("loaded widget tree \"%s\": \"%s\"\n"),
                  ACE_TEXT ((*iterator).first.c_str ()),
                  ACE_TEXT ((*iterator).second.first.c_str ())));
#endif // _DEBUG
      state_->resources[(*iterator).first] =
        std::make_pair ((*iterator).second.first, object_p);
      object_p = NULL;
    } // end FOR
  } // end lock scope

  return true;
}

template <typename StateType,
          const char* TopLevelClassName>
void
Common_UI_WxWidgetsXRCDefinition_T<StateType,
                                   TopLevelClassName>::finalize ()
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_WxWidgetsXRCDefinition_T::finalize"));

  // sanity check(s)
  ACE_ASSERT (state_);

  { ACE_GUARD (ACE_SYNCH_MUTEX, aGuard, state_->lock);
    for (Common_UI_wxWidgets_XmlResourcesIterator_t iterator = state_->resources.begin ();
         iterator != state_->resources.end ();
         ++iterator)
    { ACE_ASSERT ((*iterator).second.second);
      (*iterator).second.second->UnRef (); (*iterator).second.second = NULL;
    } // end FOR
    state_->resources.clear ();
  } // end lock scope
}
