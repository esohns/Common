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

#include "common_dbus_tools.h"

#include "ace/Log_Msg.h"

#include "common_macros.h"

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
struct DBusMessage*
Common_DBus_Tools::exchange (struct DBusConnection* connection_in,
                             struct DBusMessage*& message_inout,
                             int timeout_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_DBus_Tools::exchange"));

  // initialize return value(s)
  struct DBusMessage* result = NULL;

  // sanity check(s)
  ACE_ASSERT (connection_in);
  ACE_ASSERT (message_inout);

  struct DBusPendingCall* pending_p = NULL;
  if (unlikely (!dbus_connection_send_with_reply (connection_in,
                                                  message_inout,
                                                  &pending_p,
                                                  timeout_in))) // timeout (ms)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to dbus_connection_send_with_reply(0x%@,%d), aborting\n"),
                connection_in,
                timeout_in));
    goto error;
  } // end IF
  if (unlikely (!pending_p))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to dbus_connection_send_with_reply(0x%@,%d), aborting\n"),
                connection_in,
                timeout_in));
    goto error;
  } // end IF
//  dbus_connection_flush (connection_in);
  dbus_message_unref (message_inout); message_inout = NULL;
  dbus_pending_call_block (pending_p);
  result = dbus_pending_call_steal_reply (pending_p);
  if (unlikely (!result))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to dbus_pending_call_steal_reply(0x%@), aborting\n"),
                pending_p));
    goto error;
  } // end IF
  dbus_pending_call_unref (pending_p); pending_p = NULL;

  goto continue_;

error:
  if (pending_p)
    dbus_pending_call_unref (pending_p);
  if (message_inout)
  {
    dbus_message_unref (message_inout);
    message_inout = NULL;
  } // end IF

continue_:
  return result;
}

bool
Common_DBus_Tools::validateType (struct DBusMessageIter& iterator_in,
                                 int expectedType_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_DBus_Tools::validateType"));

  if (likely (dbus_message_iter_get_arg_type (&iterator_in) == expectedType_in))
    return true;

  char* signature_string_p = dbus_message_iter_get_signature (&iterator_in);
  ACE_ASSERT (signature_string_p);
  bool is_error = !ACE_OS::strcmp (ACE_TEXT_ALWAYS_CHAR ("s"),
                                   signature_string_p);
  char* error_string_p = NULL;
  if (is_error)
  {
    dbus_message_iter_get_basic (&iterator_in, &error_string_p);
    ACE_ASSERT (error_string_p);
  } // end IF
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("unexpected dbus message argument type (was: %c, expected: %c%s%s%s%s%s, aborting\n"),
              dbus_message_iter_get_arg_type (&iterator_in), expectedType_in,
              (is_error ? ACE_TEXT ("; message signature: ") : ACE_TEXT (")")),
              (is_error ? ACE_TEXT (signature_string_p) : ACE_TEXT ("")),
              (is_error ? ACE_TEXT ("): \"") : ACE_TEXT ("")),
              (is_error ? ACE_TEXT (error_string_p) : ACE_TEXT ("")),
              (is_error ? ACE_TEXT ("\"") : ACE_TEXT (""))));

  // clean up
  dbus_free (signature_string_p);

  return false;
}
#endif
