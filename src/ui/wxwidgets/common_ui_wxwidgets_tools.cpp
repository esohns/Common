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
#include "stdafx.h"

#include "ace/Synch.h"
#include "common_ui_wxwidgets_tools.h"

#include "wx/artprov.h"

#include "ace/Log_Msg.h"

#include "common_macros.h"

bool
Common_UI_WxWidgets_Tools::initialize ()
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_WxWidgets_Tools::initialize"));

  if (unlikely (!Common_UI_WxWidgets_Tools::initializeLogging ()))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_UI_WxWidgets_Tools::initializeLogging(), aborting\n")));
    return false;
  } // end IF

  wxModule::RegisterModules ();
  if (unlikely (!wxModule::InitializeModules ()))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to wxModule::InitializeModules(), aborting\n")));
    return false;
  } // end IF

  //wxArtProvider::InitStdProvider ();

  return true;
}
bool
Common_UI_WxWidgets_Tools::finalize ()
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_WxWidgets_Tools::finalize"));

  Common_UI_WxWidgets_Tools::finalizeLogging ();

  return true;
}

#if defined (ACE_WIN32) || defined (ACE_WIN64)
wxChar**
Common_UI_WxWidgets_Tools::convertArgV (ACE_TCHAR** argv_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_WxWidgets_Tools::convertArgV"));

  // initialize return value(s)
  wxChar** result_p = NULL;

  // sanity check(s)
  ACE_ASSERT (argv_in);

  return result_p;
}
#endif // ACE_WIN32 || ACE_WIN64

bool
Common_UI_WxWidgets_Tools::initializeLogging ()
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_WxWidgets_Tools::initializeLogging"));

  return true;
}
void
Common_UI_WxWidgets_Tools::finalizeLogging ()
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_WxWidgets_Tools::finalizeLogging"));

}
