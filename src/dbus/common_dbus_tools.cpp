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

#if defined (SD_BUS_SUPPORT)
#include <vector>

#include "sd-bus.h"
#endif // SD_BUS_SUPPORT

#include "ace/Log_Msg.h"

#include "common_macros.h"
#include "common_process_tools.h"

#include "common_dbus_defines.h"
#include "common_dbus_common.h"

// initialize statics
#if defined (SD_BUS_SUPPORT)
struct sd_bus* Common_DBus_Tools::bus = NULL;
#endif // SD_BUS_SUPPORT

bool
Common_DBus_Tools::initialize ()
{
  COMMON_TRACE (ACE_TEXT ("Common_DBus_Tools::initialize"));

  bool result = false;

#if defined (SD_BUS_SUPPORT)
  int result_2 = -1;
#if defined (_DEBUG)
  const char* string_p = NULL;
#endif // _DEBUG

  if (unlikely (Common_DBus_Tools::bus))
    goto continue_;

  result_2 = sd_bus_open_system (&Common_DBus_Tools::bus);
  if (unlikely (result_2 < 0))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to sd_bus_open_system(): \"%s\", aborting\n"),
                ACE_TEXT (ACE_OS::strerror (-result_2))));
    goto error;
  } // end IF
  ACE_ASSERT (Common_DBus_Tools::bus);
  result_2 =
      sd_bus_set_allow_interactive_authorization (Common_DBus_Tools::bus, 1);
  if (unlikely (result_2 < 0))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to sd_bus_set_allow_interactive_authorization(1): \"%s\", aborting\n"),
                ACE_TEXT (ACE_OS::strerror (-result_2))));
    goto error;
  } // end IF
#if defined (_DEBUG)
  result_2 = sd_bus_get_unique_name (Common_DBus_Tools::bus, &string_p);
  ACE_ASSERT ((result_2 == 0) && string_p);
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("%s: dbus (is: %@) sender name: \"%s\"\n"),
              ACE_TEXT (Common_PACKAGE_NAME),
              Common_DBus_Tools::bus,
              ACE_TEXT (string_p)));
#endif // _DEBUG

continue_:
#endif // SD_BUS_SUPPORT
  result = true;

error:
  return result;
}
bool
Common_DBus_Tools::finalize ()
{
  COMMON_TRACE (ACE_TEXT ("Common_DBus_Tools::finalize"));

  bool result = false;

#if defined (SD_BUS_SUPPORT)
  int result_2 = -1;

  if (unlikely (!Common_DBus_Tools::bus))
    goto continue_;

  result_2 = sd_bus_try_close (Common_DBus_Tools::bus);
  if (unlikely (result_2 < 0))
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to sd_bus_try_close(%@): \"%s\", continuing\n"),
                Common_DBus_Tools::bus,
                ACE_TEXT (ACE_OS::strerror (-result))));
  sd_bus_unref (Common_DBus_Tools::bus);
  Common_DBus_Tools::bus = NULL;

continue_:
#endif // SD_BUS_SUPPORT
  result = true;

  return result;
}

void
Common_DBus_Tools::dumpValue (struct DBusMessageIter& iterator_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_DBus_Tools::dumpValue"));

  switch (dbus_message_iter_get_arg_type (&iterator_in))
  {
    case DBUS_TYPE_INVALID:
    {
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("invalid: %c\n"),
                  static_cast<char> (DBUS_TYPE_INVALID)));
      break;
    }
    case DBUS_TYPE_BYTE:
    {
      uint8_t byte_y = 0;
      dbus_message_iter_get_basic (&iterator_in, &byte_y);
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("byte: %c\n"),
                  byte_y));
      break;
    }
    case DBUS_TYPE_BOOLEAN:
    {
      dbus_bool_t boolean_b = 0;
      dbus_message_iter_get_basic (&iterator_in, &boolean_b);
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("boolean: %d\n"),
                  boolean_b));
      break;
    }
    case DBUS_TYPE_INT16:
    {
      dbus_int16_t signed_short_n = 0;
      dbus_message_iter_get_basic (&iterator_in, &signed_short_n);
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("int16: %d\n"),
                  static_cast<ACE_INT32> (signed_short_n)));
      break;
    }
    case DBUS_TYPE_UINT16:
    {
      dbus_uint16_t unsigned_short_q = 0;
      dbus_message_iter_get_basic (&iterator_in, &unsigned_short_q);
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("uint16: %u\n"),
                  static_cast<ACE_UINT32> (unsigned_short_q)));
      break;
    }
    case DBUS_TYPE_INT32:
    {
      dbus_int32_t signed_int_i = 0;
      dbus_message_iter_get_basic (&iterator_in, &signed_int_i);
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("int32: %d\n"),
                  signed_int_i));
      break;
    }
    case DBUS_TYPE_UINT32:
    {
      dbus_uint32_t unsigned_int_u = 0;
      dbus_message_iter_get_basic (&iterator_in, &unsigned_int_u);
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("uint32: %u\n"),
                  unsigned_int_u));
      break;
    }
    case DBUS_TYPE_INT64:
    {
      dbus_uint64_t signed_long_long_int_x = 0;
      dbus_message_iter_get_basic (&iterator_in, &signed_long_long_int_x);
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("int64: %q\n"),
                  signed_long_long_int_x));
      break;
    }
    case DBUS_TYPE_UINT64:
    {
      dbus_uint64_t unsigned_long_long_int_t = 0;
      dbus_message_iter_get_basic (&iterator_in, &unsigned_long_long_int_t);
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("uint64: %Q\n"),
                  unsigned_long_long_int_t));
      break;
    }
    case DBUS_TYPE_DOUBLE:
    {
      double double_d = 0;
      dbus_message_iter_get_basic (&iterator_in, &double_d);
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("double: %e\n"),
                  double_d));
      break;
    }
    case DBUS_TYPE_STRING:
    {
      char* string_s = NULL;
      dbus_message_iter_get_basic (&iterator_in, &string_s);
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("string: %s\n"),
                  ACE_TEXT (string_s)));
      break;
    }
    case DBUS_TYPE_OBJECT_PATH:
    {
      char* object_path_o = NULL;
      dbus_message_iter_get_basic (&iterator_in, &object_path_o);
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("object path: %s\n"),
                  ACE_TEXT (object_path_o)));
      break;
    }
    case DBUS_TYPE_SIGNATURE:
    {
      char* signature_g = NULL;
      dbus_message_iter_get_basic (&iterator_in, &signature_g);
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("signature: %s\n"),
                  ACE_TEXT (signature_g)));
      break;
    }
    case DBUS_TYPE_UNIX_FD:
    {
      ACE_HANDLE file_descriptor_h = 0;
      dbus_message_iter_get_basic (&iterator_in, &file_descriptor_h);
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("unix file descriptor: %d\n"),
                  file_descriptor_h));
      break;
    }
    // ---------------------------------
    case DBUS_TYPE_ARRAY:
    {
      struct DBusMessageIter iterator_2;
      dbus_message_iter_recurse (&iterator_in, &iterator_2);
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("array: [\n")));
      do
      {
        Common_DBus_Tools::dumpValue (iterator_2);
      } while (dbus_message_iter_next (&iterator_2));
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("]\n")));
      break;
    }
    case DBUS_TYPE_VARIANT:
    {
      struct DBusMessageIter iterator_2;
      dbus_message_iter_recurse (&iterator_in, &iterator_2);
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("variant: %c [\n"),
                  dbus_message_iter_get_arg_type (&iterator_2)));
      do
      {
        Common_DBus_Tools::dumpValue (iterator_2);
      } while (dbus_message_iter_next (&iterator_2));
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("]\n")));
      break;
    }
    case DBUS_STRUCT_BEGIN_CHAR:
    {
      struct DBusMessageIter iterator_2;
      dbus_message_iter_recurse (&iterator_in, &iterator_2);
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("struct: [\n"),
                  dbus_message_iter_get_arg_type (&iterator_2)));
      do
      {
        Common_DBus_Tools::dumpValue (iterator_2);
      } while (dbus_message_iter_next (&iterator_2));
      break;
    }
    case DBUS_DICT_ENTRY_BEGIN_CHAR:
    {
      struct DBusMessageIter iterator_2;
      dbus_message_iter_recurse (&iterator_in, &iterator_2);
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("dict: [\n"),
                  dbus_message_iter_get_arg_type (&iterator_2)));
      do
      {
        Common_DBus_Tools::dumpValue (iterator_2);
      } while (dbus_message_iter_next (&iterator_2));
      break;
    }
    case DBUS_STRUCT_END_CHAR:
    case DBUS_DICT_ENTRY_END_CHAR:
    {
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("]\n")));
      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown value type (was: %c), returning\n"),
                  static_cast<char> (dbus_message_iter_get_arg_type (&iterator_in))));
      break;
    }
  } // end SWITCH
}
void
Common_DBus_Tools::dump (struct DBusMessage& message_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_DBus_Tools::dump"));

  struct DBusMessageIter iterator;
  dbus_bool_t result = dbus_message_iter_init (&message_in, &iterator);
  ACE_ASSERT (result);

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("message %@ signature: \"%s\"; value(s):\n------------------------------\n"),
              &message_in,
              ACE_TEXT (dbus_message_iter_get_signature (&iterator))));

  do
  {
    Common_DBus_Tools::dumpValue (iterator);
  } while (dbus_message_iter_next (&iterator));
}

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
              ACE_TEXT ("unexpected dbus message argument type (was: '%c', expected: '%c'%s%s%s%s%s, aborting\n"),
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

#if defined (SD_BUS_SUPPORT)
int
common_dbus_polkit_checkauthorization_cb (struct sd_bus_message* message_in,
                                          void* CBData_in,
                                          sd_bus_error* error_out)
{
//  COMMON_TRACE (ACE_TEXT ("common_dbus_polkit_checkauthorization_cb"));

  ACE_UNUSED_ARG (error_out);

  // sanity check(s)
  ACE_ASSERT (CBData_in);

  struct Common_DBus_PolicyKit_CheckAuthorizationCBData* cb_data_p =
      static_cast<struct Common_DBus_PolicyKit_CheckAuthorizationCBData*> (CBData_in);
  cb_data_p->done = true;

  // sanity check(s)
  if (unlikely (sd_bus_message_is_method_error (message_in, NULL)))
  {
    const sd_bus_error* error_p = sd_bus_message_get_error (message_in);
    ACE_ASSERT (error_p);
    if (sd_bus_error_has_name (error_p,
                               SD_BUS_ERROR_SERVICE_UNKNOWN) ||                                                           // polkit not installed/running ?
        sd_bus_error_has_name (error_p,
                               ACE_TEXT_ALWAYS_CHAR (COMMON_DBUS_METHOD_POLICYKIT_AUTHORITY_ERROR_NOTAUTHORIZED_STRING))) // missing polkit rules ?
      ;
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("polkit method failed (errno was: %d): %s: \"%s\", aborting\n"),
                sd_bus_message_get_errno (message_in),
                ACE_TEXT (error_p->name), ACE_TEXT (error_p->message)));
    return -sd_bus_error_get_errno (error_p);
  } // end IF
  ACE_ASSERT (sd_bus_message_has_signature (message_in, ACE_TEXT_ALWAYS_CHAR (COMMON_DBUS_METHOD_POLICYKIT_AUTHORITY_CHECKAUTHORIZATION_OUT_SIGNATURE_STRING)));

  int result = -1;
  const char* string_p = NULL, *string_2 = NULL;

  result = sd_bus_message_enter_container (message_in,
                                           SD_BUS_TYPE_STRUCT,
                                           ACE_TEXT_ALWAYS_CHAR ("bba{ss}"));
  ACE_ASSERT (result >= 0);
  result =
      sd_bus_message_read (message_in,
                           ACE_TEXT_ALWAYS_CHAR ("bb"),
                           &cb_data_p->authorized,
                           &cb_data_p->challenge);
  if (unlikely (result < 0))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to sd_bus_message_read(): \"%s\", aborting\n"),
                ACE_TEXT (ACE_OS::strerror (-result))));
    return -result;
  } // end IF
  result = sd_bus_message_enter_container (message_in,
                                           SD_BUS_TYPE_ARRAY,
                                           ACE_TEXT_ALWAYS_CHAR ("{ss}"));
  ACE_ASSERT (result >= 0);
  do
  {
    result = sd_bus_message_enter_container (message_in,
                                             SD_BUS_TYPE_DICT_ENTRY,
                                             ACE_TEXT_ALWAYS_CHAR ("ss"));
    if (result < 0)
      break;

    result =
        sd_bus_message_read (message_in,
                             ACE_TEXT_ALWAYS_CHAR ("ss"),
                             &string_p,
                             &string_2);
    ACE_ASSERT (result >= 0);
    cb_data_p->details.push_back (std::make_pair (string_p, string_2));

    result = sd_bus_message_exit_container (message_in); // SD_BUS_TYPE_DICT_ENTRY
    ACE_ASSERT (result >= 0);
  } while (true);
  result = sd_bus_message_exit_container (message_in); // SD_BUS_TYPE_ARRAY
  ACE_ASSERT (result >= 0);
  result = sd_bus_message_exit_container (message_in); // SD_BUS_TYPE_STRUCT
  // *TODO*: there's something wrong here (result is -6); find out why
  //  ACE_ASSERT (result >= 0);

  return 1;
}

bool
Common_DBus_Tools::policyKitAuthorize (struct sd_bus* bus_in,
                                       const std::string& actionId_in,
                                       const Common_DBus_PolicyKit_Details_t& actionDetails_in,
                                       enum Common_DBus_PolicyKit_SubjectType subject_in,
                                       bool allowUserInteraction_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_DBus_Tools::policyKitAuthorize"));

  // initialize return value(s)
  bool result = false;

  // sanity check(s)
  ACE_ASSERT (!actionId_in.empty ());
//  ACE_ASSERT (!actionDetails_in.empty ());

  struct sd_bus* bus_p = (bus_in ? bus_in : Common_DBus_Tools::bus);
  int result_2 = -1;
  sd_bus_error error_s = SD_BUS_ERROR_NULL;
  struct sd_bus_message* message_p = NULL;//, *message_2 = NULL;
  const char* string_p = NULL;//, *string_2 = NULL;
//  ACE_UINT32 pid_i = static_cast<ACE_UINT32> (ACE_OS::getpid ());
//  ACE_UINT64 start_time_i = 0;
  ACE_UINT32 flags_i = (allowUserInteraction_in ? 1 : 0); // AllowUserInteraction
//  int is_authorized_i = 0, is_challenge_i = 0;
//  std::vector<std::pair<std::string, std::string> > details_a;
  bool reset_interactive_authorization = false;
  struct sd_bus_creds* credentials_p = NULL;
//  ACE_UINT64 augmented_credentials_u = 0;
//  uid_t sender_euid_u = 0, euid_u = ACE_OS::geteuid ();
//  int capability_i = CAP_SYS_ADMIN;
  struct sd_bus_slot* slot_p = NULL;
  struct Common_DBus_PolicyKit_CheckAuthorizationCBData cb_data_s;
#if defined (_DEBUG)
  int i = 0;
#endif // _DEBUG

  // sanity check(s)
  ACE_ASSERT (bus_p);

  if (allowUserInteraction_in &&
      !sd_bus_get_allow_interactive_authorization (bus_p))
  {
    result_2 = sd_bus_set_allow_interactive_authorization (bus_p, 1);
    if (unlikely (result_2 < 0))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to sd_bus_set_allow_interactive_authorization(1): \"%s\", aborting\n"),
                  ACE_TEXT (ACE_OS::strerror (-result_2))));
      goto finish;
    } // end IF
    reset_interactive_authorization = true;
  } // end IF

  result_2 =
      sd_bus_message_new_method_call (bus_p,
                                      &message_p,
                                      ACE_TEXT_ALWAYS_CHAR (COMMON_DBUS_SERVICE_POLICYKIT_STRING),                              /* service */
                                      ACE_TEXT_ALWAYS_CHAR (COMMON_DBUS_OBJECT_PATH_POLICYKIT_STRING),                          /* object path */
                                      ACE_TEXT_ALWAYS_CHAR (COMMON_DBUS_INTERFACE_POLICYKIT_AUTHORITY_STRING),                  /* interface name */
                                      ACE_TEXT_ALWAYS_CHAR (COMMON_DBUS_METHOD_POLICYKIT_AUTHORITY_CHECKAUTHORIZATION_STRING)); /* method name */
  if (unlikely (result_2 < 0))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to sd_bus_message_new_method_call(%s): \"%s\", aborting\n"),
                ACE_TEXT (COMMON_DBUS_METHOD_POLICYKIT_AUTHORITY_CHECKAUTHORIZATION_STRING),
                ACE_TEXT (ACE_OS::strerror (-result_2))));
    goto finish;
  } // end IF
  ACE_ASSERT (message_p);

  switch (subject_in)
  {
    case COMMON_DBUS_POLICYKIT_SUBJECT_BUS_SYSTEM:
    case COMMON_DBUS_POLICYKIT_SUBJECT_BUS_USER:
    {
      result_2 = sd_bus_get_unique_name (bus_p, &string_p);
      ACE_ASSERT ((result_2 == 0) && string_p);
      result_2 =
          sd_bus_message_append (message_p,
                                 ACE_TEXT_ALWAYS_CHAR ("(sa{sv})"),
                                 ((subject_in == COMMON_DBUS_POLICYKIT_SUBJECT_BUS_SYSTEM) ? ACE_TEXT_ALWAYS_CHAR ("system-bus-name")
                                                                                           : ACE_TEXT_ALWAYS_CHAR ("user-bus-name")),
                                 1, ACE_TEXT_ALWAYS_CHAR ("name"), ACE_TEXT_ALWAYS_CHAR ("s"), string_p);
      break;
    }
    case COMMON_DBUS_POLICYKIT_SUBJECT_PROCESS:
    {
      result_2 =
          sd_bus_message_append (message_p,
                                 ACE_TEXT_ALWAYS_CHAR ("(sa{sv})"),
                                 ACE_TEXT_ALWAYS_CHAR ("unix-process"),
                                 3,
                                 ACE_TEXT_ALWAYS_CHAR ("pid"), ACE_TEXT_ALWAYS_CHAR ("u"), ACE_OS::getpid (),
                                 ACE_TEXT_ALWAYS_CHAR ("start-time"), ACE_TEXT_ALWAYS_CHAR ("t"), Common_Process_Tools::getStartTime (),
                                 ACE_TEXT_ALWAYS_CHAR ("uid"), ACE_TEXT_ALWAYS_CHAR ("i"), ACE_OS::getuid ());
      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown subject (was: %d), aborting\n"),
                  ACE_TEXT (COMMON_DBUS_METHOD_POLICYKIT_AUTHORITY_CHECKAUTHORIZATION_STRING),
                  subject_in));
      goto finish;
    }
  } // end SWITCH
  // subject [(sa{sv})]
  ACE_ASSERT (result_2 >= 0);
//  result_2 =
//      sd_bus_message_append_basic (message_p,
//                                   SD_BUS_TYPE_STRING,
//                                   ACE_TEXT_ALWAYS_CHAR ("unix-process"));
//  ACE_ASSERT (result_2 == 0);
//  result_2 = sd_bus_message_open_container (message_p,
//                                            SD_BUS_TYPE_ARRAY,
//                                            ACE_TEXT_ALWAYS_CHAR ("{sv}"));
//  ACE_ASSERT (result_2 == 0);

//  result_2 = sd_bus_message_open_container (message_p,
//                                            SD_BUS_TYPE_DICT_ENTRY,
//                                            ACE_TEXT_ALWAYS_CHAR ("sv"));
//  ACE_ASSERT (result_2 == 0);
//  result_2 = sd_bus_message_append_basic (message_p,
//                                          SD_BUS_TYPE_STRING,
//                                          ACE_TEXT_ALWAYS_CHAR ("pid"));
//  ACE_ASSERT (result_2 == 0);
//  result_2 = sd_bus_message_open_container (message_p,
//                                            SD_BUS_TYPE_VARIANT,
//                                            ACE_TEXT_ALWAYS_CHAR ("u"));
//  ACE_ASSERT (result_2 == 0);
//  result_2 = sd_bus_message_append_basic (message_p,
//                                          SD_BUS_TYPE_UINT32,
//                                          &pid_i);
//  ACE_ASSERT (result_2 == 0);
//  result_2 = sd_bus_message_close_container (message_p); // SD_BUS_TYPE_VARIANT
//  ACE_ASSERT (result_2 == 0);
//  result_2 = sd_bus_message_close_container (message_p); // SD_BUS_TYPE_DICT_ENTRY
//  ACE_ASSERT (result_2 == 0);

//  result_2 = sd_bus_message_open_container (message_p,
//                                            SD_BUS_TYPE_DICT_ENTRY,
//                                            ACE_TEXT_ALWAYS_CHAR ("sv"));
//  ACE_ASSERT (result_2 == 0);
//  result_2 = sd_bus_message_append_basic (message_p,
//                                          SD_BUS_TYPE_STRING,
//                                          ACE_TEXT_ALWAYS_CHAR ("start-time"));
//  ACE_ASSERT (result_2 == 0);
//  result_2 = sd_bus_message_open_container (message_p,
//                                            SD_BUS_TYPE_VARIANT,
//                                            ACE_TEXT_ALWAYS_CHAR ("t"));
//  ACE_ASSERT (result_2 == 0);
//  result_2 = sd_bus_message_append_basic (message_p,
//                                          SD_BUS_TYPE_UINT64,
//                                          &start_time_i);
//  ACE_ASSERT (result_2 == 0);
//  result_2 = sd_bus_message_close_container (message_p); // SD_BUS_TYPE_VARIANT
//  ACE_ASSERT (result_2 == 0);
//  result_2 = sd_bus_message_close_container (message_p); // SD_BUS_TYPE_DICT_ENTRY
//  ACE_ASSERT (result_2 == 0);

//  result_2 = sd_bus_message_close_container (message_p); // SD_BUS_TYPE_ARRAY
//  ACE_ASSERT (result_2 == 0);
//  result_2 = sd_bus_message_close_container (message_p); // SD_BUS_TYPE_STRUCT
//  ACE_ASSERT (result_2 == 0);
  // action id [s]
  result_2 = sd_bus_message_append_basic (message_p,
                                          SD_BUS_TYPE_STRING,
                                          actionId_in.c_str ());
  ACE_ASSERT (result_2 == 0);
  // details [a{ss}]
  result_2 = sd_bus_message_open_container (message_p,
                                            SD_BUS_TYPE_ARRAY,
                                            ACE_TEXT_ALWAYS_CHAR ("{ss}"));
  ACE_ASSERT (result_2 == 0);
  for (Common_DBus_PolicyKit_DetailsIterator_t iterator = actionDetails_in.begin ();
       iterator != actionDetails_in.end ();
       ++iterator)
  {
    result_2 = sd_bus_message_append (message_p,
                                      ACE_TEXT_ALWAYS_CHAR ("{ss}"),
                                      (*iterator).first.c_str (),
                                      (*iterator).second.c_str ());
    ACE_ASSERT (result_2 >= 0);
  } // end FOR
  result_2 = sd_bus_message_close_container (message_p); // SD_BUS_TYPE_ARRAY
  ACE_ASSERT (result_2 == 0);
  // flags [u]
  result_2 = sd_bus_message_append_basic (message_p,
                                          SD_BUS_TYPE_UINT32,
                                          &flags_i);
  ACE_ASSERT (result_2 == 0);
  // cancellation id [s]
  result_2 = sd_bus_message_append_basic (message_p,
                                          SD_BUS_TYPE_STRING,
                                          NULL);
  ACE_ASSERT (result_2 == 0);

//  result_2 = sd_bus_message_set_expect_reply (message_p, 1);
//  ACE_ASSERT (result_2 == 0);
//  result_2 = sd_bus_message_set_allow_interactive_authorization (message_p, 1);
//  ACE_ASSERT (result_2 == 0);

  // verify credentials
//  result_2 = sd_bus_query_sender_creds (message_p,
//                                        SD_BUS_CREDS_EUID,
//                                        &credentials_p);
//  ACE_ASSERT ((result_2 == 0) && credentials_p);
//  augmented_credentials_u = sd_bus_creds_get_augmented_mask (credentials_p);
//  /* don't trust augmented credentials for authorization */
//  if (unlikely (augmented_credentials_u & SD_BUS_CREDS_EUID))
//  {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("effective uid has been augmented (was: %u), aborting\n"),
//                augmented_credentials_u & SD_BUS_CREDS_EUID));
//    goto finish;
//  } // end IF
//  result_2 = sd_bus_creds_get_euid (credentials_p, &sender_euid_u);
//  ACE_ASSERT (result_2 == 0);
//  if (unlikely (sender_euid_u != euid_u))
//  {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("effective sender uid (was: %u) does not match effective process uid (was: %u), aborting\n"),
//                sender_euid_u, euid_u));
//    goto finish;
//  } // end IF

  // verify capabilities
//  result_2 = sd_bus_query_sender_privilege (message_p, capability_i);
//  ACE_ASSERT (result_2 >= 0);
//  if (result_2 > 0)
//  {
//    result = true;
//    goto finish;
//  } // end IFsd_bus_message_close_container

//retry:
//  result_2 = sd_bus_call (bus_p,         /* bus handle */
//                          message_p,     /* request message */
//                          0,
////                          static_cast<uint64_t> (-1), /* --> block */
//                          &error_s,      /* object to return error in */
//                          &message_2);   /* return message on success */
  result_2 = sd_bus_call_async (bus_p,           /* bus handle */
                                &slot_p,         /* slot handle */
                                message_p,       /* request message */
                                common_dbus_polkit_checkauthorization_cb,
                                &cb_data_s,      /* user data */
                                (uint64_t) - 1); /* */
  if (unlikely (result_2 < 0))
  {
//    if ((-result_2 == EINTR) ||   // 4  : a polkit authorization request dialog
//                                  //      should have appeared; delay and retry
//        (-result_2 == ETIMEDOUT)) // 110: timed out
//    {
//      sd_bus_error_free (&error_s);

//      if (-result_2 == EINTR)
//      {
//        ACE_Time_Value delay_s (COMMON_DBUS_POLICYKIT_AUTHORIZATION_DELAY, 0);
//        result_2 = ACE_OS::sleep (delay_s);
//        if (result_2 == -1)
//          ACE_DEBUG ((LM_ERROR,
//                      ACE_TEXT ("failed to ACE_OS::sleep(%#T): \"%m\", continuing\n"),
//                      &delay_s));
//      } // end IF
//      goto retry;
//    } // end IF
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to sd_bus_call_async(%s): \"%s\", aborting\n"),
                ACE_TEXT (COMMON_DBUS_METHOD_POLICYKIT_AUTHORITY_CHECKAUTHORIZATION_STRING),
                ACE_TEXT (error_s.message)));
    goto finish;
  } // end IF
//  ACE_ASSERT (message_2);
  ACE_ASSERT (slot_p);

//  result_2 =
//      sd_bus_message_read (message_2,
////                           ACE_TEXT_ALWAYS_CHAR (COMMON_DBUS_METHOD_POLICYKIT_AUTHORITY_CHECKAUTHORIZATION_OUT_SIGNATURE_STRING),
//                           ACE_TEXT_ALWAYS_CHAR ("bb"),
//                           &is_authorized_i,
//                           &is_challenge_i);
//  if (unlikely (result_2 < 0))
//  {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("failed to sd_bus_message_read(): \"%s\", aborting\n"),
//                ACE_TEXT (ACE_OS::strerror (-result_2))));
//    goto finish;
//  } // end IF
//  result_2 = sd_bus_message_enter_container (message_2,
//                                             SD_BUS_TYPE_ARRAY,
//                                             ACE_TEXT_ALWAYS_CHAR ("{ss}"));
//  ACE_ASSERT (result_2 == 0);
//  do
//  {
//    result_2 = sd_bus_message_open_container (message_p,
//                                              SD_BUS_TYPE_DICT_ENTRY,
//                                              ACE_TEXT_ALWAYS_CHAR ("ss"));
//    if (result_2 < 0)
//      break;

//    result_2 =
//        sd_bus_message_read (message_2,
//                             ACE_TEXT_ALWAYS_CHAR ("ss"),
//                             &string_p,
//                             &string_2);
//    ACE_ASSERT (result_2 == 0);
//    details_a.push_back (std::make_pair (string_p, string_2));

//    result_2 = sd_bus_message_close_container (message_2); // SD_BUS_TYPE_DICT_ENTRY
//    ACE_ASSERT (result_2 == 0);
//  } while (true);
//  result_2 = sd_bus_message_close_container (message_2); // SD_BUS_TYPE_ARRAY
//  ACE_ASSERT (result_2 == 0);

//  result = (is_authorized_i == 1);
  do
  {
    result_2 = sd_bus_process (bus_p, NULL);
    if (unlikely (result_2 < 0))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to sd_bus_process(%@): \"%s\", aborting\n"),
                  bus_p,
                  ACE_TEXT (ACE_OS::strerror (-result_2))));
      goto finish;
    } // end IF
    if (cb_data_s.done)
      break; // done

    result_2 = sd_bus_wait (bus_p, (uint64_t) - 1);
    if (unlikely (result_2 < 0))
    {
      if (-result_2 == EINTR) // 4
        continue;
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to sd_bus_wait(%@): \"%s\", aborting\n"),
                  bus_p,
                  ACE_TEXT (ACE_OS::strerror (-result_2))));
      goto finish;
    } // end IF
  } while (true);
#if defined (_DEBUG)
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("polkit reply for action %s: %s; %s\n"),
              ACE_TEXT (actionId_in.c_str ()),
              ((cb_data_s.authorized == 1) ? ACE_TEXT ("authorized") : ACE_TEXT ("not authorized")),
              ((cb_data_s.challenge == 1) ? ACE_TEXT ("may challenge") : ACE_TEXT ("may not challenge"))));
  for (Common_DBus_PolicyKit_DetailsIterator_t iterator = cb_data_s.details.begin ();
       iterator != cb_data_s.details.end ();
       ++iterator, ++i)
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("authorization detail #%d: \"%s\": \"%s\"\n"),
                i,
                ACE_TEXT ((*iterator).first.c_str ()),
                ACE_TEXT ((*iterator).second.c_str ())));
#endif // _DEBUG

  result = (cb_data_s.authorized == 1);

finish:
  sd_bus_error_free (&error_s);
  sd_bus_message_unref (message_p);
//  sd_bus_message_unref (message_2);
  sd_bus_creds_unref (credentials_p);
  sd_bus_slot_unref (slot_p);
  if (unlikely (reset_interactive_authorization))
  { ACE_ASSERT (bus_p);
    result_2 = sd_bus_set_allow_interactive_authorization (bus_p, 0);
    if (unlikely (result_2 < 0))
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to sd_bus_set_allow_interactive_authorization(0): \"%s\", continuing\n"),
                  ACE_TEXT (ACE_OS::strerror (-result_2))));
  } // end IF

  return result;
}

std::string
Common_DBus_Tools::unitToObjectPath (struct sd_bus* bus_in,
                                     const std::string& unitName_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_DBus_Tools::unitToObjectPath"));

  // initialize return value(s)
  std::string return_value;

  // sanity check(s)
  ACE_ASSERT (!unitName_in.empty ());

  struct sd_bus* bus_p = (bus_in ? bus_in : Common_DBus_Tools::bus);
  sd_bus_error error_s = SD_BUS_ERROR_NULL;
  struct sd_bus_message* message_p = NULL;
  const char* string_p = NULL;

  // sanity check(s)
  ACE_ASSERT (bus_p);

  int result_2 =
      sd_bus_call_method (bus_p,
                          ACE_TEXT_ALWAYS_CHAR (COMMON_SD_BUS_SERVICE_STRING),                      /* service */
                          ACE_TEXT_ALWAYS_CHAR (COMMON_SD_BUS_OBJECT_PATH_STRING),                  /* object path */
                          ACE_TEXT_ALWAYS_CHAR (COMMON_SD_BUS_INTERFACE_MANAGER_STRING),            /* interface name */
                          ACE_TEXT_ALWAYS_CHAR (COMMON_SD_BUS_METHOD_UNIT_GET_STRING),              /* method name */
                          &error_s,                                                                 /* object to return error in */
                          &message_p,                                                               /* return message on success */
                          ACE_TEXT_ALWAYS_CHAR (COMMON_SD_BUS_METHOD_UNIT_GET_IN_SIGNATURE_STRING), /* input signature */
                          unitName_in.c_str ());                                                    /* first argument */
  if (unlikely (result_2 < 0))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to sd_bus_call_method(%s): \"%s\", aborting\n"),
                ACE_TEXT (COMMON_SD_BUS_METHOD_UNIT_GET_STRING),
                ACE_TEXT (error_s.message)));
    goto finish;
  } // end IF
  ACE_ASSERT (message_p);

  result_2 =
      sd_bus_message_read (message_p,
                           ACE_TEXT_ALWAYS_CHAR (COMMON_SD_BUS_METHOD_UNIT_GET_OUT_SIGNATURE_STRING),
                           &string_p);
  if (unlikely (result_2 < 0))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to sd_bus_message_read(): \"%s\", aborting\n"),
                ACE_TEXT (ACE_OS::strerror (-result_2))));
    goto finish;
  } // end IF
  ACE_ASSERT (string_p);

  return_value = string_p;
//#if defined (_DEBUG)
//  ACE_DEBUG ((LM_DEBUG,
//              ACE_TEXT ("systemd unit (was: \"%s\") object path is: \"%s\"\n"),
//              ACE_TEXT (unitName_in.c_str ()),
//              ACE_TEXT (return_value.c_str ())));
//#endif // _DEBUG

finish:
  sd_bus_error_free (&error_s);
  sd_bus_message_unref (message_p);

  return return_value;
}

bool
Common_DBus_Tools::isUnitRunning (struct sd_bus* bus_in,
                                  const std::string& unitName_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_DBus_Tools::isUnitRunning"));

  bool result = false;

  struct sd_bus* bus_p = (bus_in ? bus_in : Common_DBus_Tools::bus);
  int result_2 = -1;
  sd_bus_error error_s = SD_BUS_ERROR_NULL;
  struct sd_bus_message* message_p = NULL;
  const char* string_p = NULL;
  std::string unit_object_path_string;

  // sanity check(s)
  ACE_ASSERT (bus_p);

  // step1: retrieve unit object path
  unit_object_path_string = Common_DBus_Tools::unitToObjectPath (bus_p,
                                                                 unitName_in);
  if (unit_object_path_string.empty ())
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_DBus_Tools::unitToObjectPath(%@,%s), aborting\n"),
                bus_p,
                ACE_TEXT (unitName_in.c_str ())));
    goto finish;
  } // end IF

  // step2: retrieve unit status
  result_2 =
      sd_bus_get_property (bus_p,
                           ACE_TEXT_ALWAYS_CHAR (COMMON_SD_BUS_SERVICE_STRING),                              /* service */
                           unit_object_path_string.c_str (),                                                 /* object path */
                           ACE_TEXT_ALWAYS_CHAR (COMMON_SD_BUS_INTERFACE_UNIT_STRING),                       /* interface name */
                           ACE_TEXT_ALWAYS_CHAR (COMMON_SD_BUS_PROPERTY_UNIT_ACTIVESTATE_STRING),            /* member name */
                           &error_s,                                                                         /* object to return error in */
                           &message_p,                                                                       /* return message on success */
                           ACE_TEXT_ALWAYS_CHAR (COMMON_SD_BUS_PROPERTY_UNIT_ACTIVESTATE_SIGNATURE_STRING)); /* property signature */
  if (unlikely (result_2 < 0))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to sd_bus_get_property(%s): \"%s\", aborting\n"),
                ACE_TEXT (COMMON_SD_BUS_PROPERTY_UNIT_ACTIVESTATE_STRING),
                ACE_TEXT (error_s.message)));
    goto finish;
  } // end IF
  ACE_ASSERT (message_p);

  string_p = NULL;
  result_2 =
      sd_bus_message_read (message_p,
                           ACE_TEXT_ALWAYS_CHAR (COMMON_SD_BUS_PROPERTY_UNIT_ACTIVESTATE_SIGNATURE_STRING),
                           &string_p);
  if (unlikely (result_2 < 0))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to sd_bus_message_read(): \"%s\", aborting\n"),
                ACE_TEXT (ACE_OS::strerror (-result_2))));
    goto finish;
  } // end IF
  ACE_ASSERT (string_p);

  sd_bus_message_unref (message_p);
  message_p = NULL;

  if (ACE_OS::strcmp (string_p,
                      ACE_TEXT_ALWAYS_CHAR (COMMON_SD_BUS_PROPERTY_UNIT_ACTIVESTATE_ACTIVE_STRING)))
    goto finish;

  // step3: retrieve unit sub-status
  result_2 =
      sd_bus_get_property (bus_p,
                           ACE_TEXT_ALWAYS_CHAR (COMMON_SD_BUS_SERVICE_STRING),                           /* service */
                           unit_object_path_string.c_str (),                                              /* object path */
                           ACE_TEXT_ALWAYS_CHAR (COMMON_SD_BUS_INTERFACE_UNIT_STRING),                    /* interface name */
                           ACE_TEXT_ALWAYS_CHAR (COMMON_SD_BUS_PROPERTY_UNIT_SUBSTATE_STRING),            /* member name */
                           &error_s,                                                                      /* object to return error in */
                           &message_p,                                                                    /* return message on success */
                           ACE_TEXT_ALWAYS_CHAR (COMMON_SD_BUS_PROPERTY_UNIT_SUBSTATE_SIGNATURE_STRING)); /* property signature */
  if (unlikely (result_2 < 0))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to sd_bus_get_property(%s): \"%s\", aborting\n"),
                ACE_TEXT (COMMON_SD_BUS_PROPERTY_UNIT_SUBSTATE_STRING),
                ACE_TEXT (error_s.message)));
    goto finish;
  } // end IF
  ACE_ASSERT (message_p);

  string_p = NULL;
  result_2 =
      sd_bus_message_read (message_p,
                           ACE_TEXT_ALWAYS_CHAR (COMMON_SD_BUS_PROPERTY_UNIT_SUBSTATE_SIGNATURE_STRING),
                           &string_p);
  if (unlikely (result_2 < 0))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to sd_bus_message_read(): \"%s\", aborting\n"),
                ACE_TEXT (ACE_OS::strerror (-result_2))));
    goto finish;
  } // end IF
  ACE_ASSERT (string_p);
  result =
      !ACE_OS::strcmp (string_p,
                       ACE_TEXT_ALWAYS_CHAR (COMMON_SD_BUS_PROPERTY_UNIT_SUBSTATE_RUNNING_STRING));

finish:
  sd_bus_error_free (&error_s);
  sd_bus_message_unref (message_p);

  return result;
}

int
common_dbus_sd_job_signal_handler_cb (struct sd_bus_message* message_in,
                                      void* CBData_in,
                                      sd_bus_error* error_out)
{
//  COMMON_TRACE (ACE_TEXT ("common_dbus_sd_job_signal_handler_cb"));

  // sanity check(s)
  ACE_ASSERT (CBData_in);

  struct Common_DBus_SignalSystemdJobCBData* cb_data_p =
      static_cast<struct Common_DBus_SignalSystemdJobCBData*> (CBData_in);

  // sanity check(s)
  ACE_ASSERT (!cb_data_p->objectPath.empty ());
  if (!sd_bus_message_is_signal (message_in,
                                 ACE_TEXT_ALWAYS_CHAR (COMMON_SD_BUS_INTERFACE_MANAGER_STRING),
                                 ACE_TEXT_ALWAYS_CHAR (COMMON_SD_BUS_SIGNAL_MANAGER_JOBREMOVED_STRING)))
    return 0;

  int result = -1;
  const char* string_p = NULL, *string_2 = NULL, *string_3 = NULL;

  result =
      sd_bus_message_read (message_in,
                           ACE_TEXT_ALWAYS_CHAR (COMMON_SD_BUS_SIGNAL_JOB_REMOVED_SIGNATURE_STRING),
                           &cb_data_p->id,
                           &string_p,
                           &string_2,
                           &string_3);
  if (unlikely (result < 0))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to sd_bus_message_read(): \"%s\", aborting\n"),
                ACE_TEXT (ACE_OS::strerror (-result))));
    return -1;
  } // end IF
  ACE_ASSERT (cb_data_p->id);
  ACE_ASSERT (string_p);
  ACE_ASSERT (string_2);
  ACE_ASSERT (string_3);

  if (!ACE_OS::strcmp (string_p,
                       cb_data_p->objectPath.c_str ()))
  {
    cb_data_p->unit = string_2;
    cb_data_p->result =string_3;
  } // end IF

  return 1;
}

bool
Common_DBus_Tools::toggleUnit (struct sd_bus* bus_in,
                               const std::string& unitName_in,
                               bool waitForCompletion_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_DBus_Tools::toggleUnit"));

  bool result = false;

  struct sd_bus* bus_p = (bus_in ? bus_in : Common_DBus_Tools::bus);
  int result_2 = -1;
  sd_bus_error error_s = SD_BUS_ERROR_NULL;
  struct sd_bus_message* message_p = NULL;
  const char* string_p = NULL;
  struct Common_DBus_SignalSystemdJobCBData cb_data_s;
  bool toggle_on = !Common_DBus_Tools::isUnitRunning (bus_in,
                                                      unitName_in);

  // sanity check(s)
  ACE_ASSERT (bus_p);

  if (unlikely (waitForCompletion_in))
  {
    result_2 =
//        sd_bus_match_signal (bus_p,
//                             NULL,
//                             ACE_TEXT_ALWAYS_CHAR (COMMON_SD_BUS_SERVICE_STRING),
//                             ACE_TEXT_ALWAYS_CHAR (COMMON_SD_BUS_OBJECT_PATH_STRING),
//                             ACE_TEXT_ALWAYS_CHAR (COMMON_SD_BUS_INTERFACE_MANAGER_STRING),
//                             ACE_TEXT_ALWAYS_CHAR (COMMON_SD_BUS_SIGNAL_MANAGER_JOBREMOVED_STRING),
        sd_bus_add_match (bus_p,
                          NULL,
                          ACE_TEXT_ALWAYS_CHAR (COMMON_SD_BUS_FILTER_SIGNAL_MANAGER_JOBREMOVED_STRING),
                          common_dbus_sd_job_signal_handler_cb,
                          &cb_data_s);
    if (unlikely (result_2 < 0))
    {
      ACE_DEBUG ((LM_ERROR,
//                  ACE_TEXT ("failed to sd_bus_match_signal(\"%s\"): \"%s\", aborting\n"),
                  ACE_TEXT ("failed to sd_bus_add_match(\"%s\"): \"%s\", aborting\n"),
                  ACE_TEXT (COMMON_SD_BUS_FILTER_SIGNAL_MANAGER_JOBREMOVED_STRING),
                  ACE_TEXT (ACE_OS::strerror (-result_2))));
      goto finish;
    } // end IF
  } // end IF

  // step1: toggle unit
retry:
  result_2 =
      sd_bus_call_method (bus_p,
                          ACE_TEXT_ALWAYS_CHAR (COMMON_SD_BUS_SERVICE_STRING),                                     /* service */
                          ACE_TEXT_ALWAYS_CHAR (COMMON_SD_BUS_OBJECT_PATH_STRING),                                 /* object path */
                          ACE_TEXT_ALWAYS_CHAR (COMMON_SD_BUS_INTERFACE_MANAGER_STRING),                           /* interface name */
                          (toggle_on ? ACE_TEXT_ALWAYS_CHAR (COMMON_SD_BUS_METHOD_UNIT_START_STRING)
                                     : ACE_TEXT_ALWAYS_CHAR (COMMON_SD_BUS_METHOD_UNIT_STOP_STRING)),              /* method name */
                          &error_s,                                                                                /* object to return error in */
                          &message_p,                                                                              /* return message on success */
                          (toggle_on ? ACE_TEXT_ALWAYS_CHAR (COMMON_SD_BUS_METHOD_UNIT_START_IN_SIGNATURE_STRING)
                                     : ACE_TEXT_ALWAYS_CHAR (COMMON_SD_BUS_METHOD_UNIT_STOP_IN_SIGNATURE_STRING)), /* input signature */
                          unitName_in.c_str (),                                                                    /* first argument */
                          ACE_TEXT_ALWAYS_CHAR (COMMON_SD_BUS_METHOD_UNIT_MODE_REPLACE_STRING));                   /* second argument */
  if (unlikely (result_2 < 0))
  { // *NOTE*: if the user/application does not have permission to manage the
    //         unit, a password dialog appears and EINTR is returned
    //         --> wait for a few seconds and retry
    if (-result_2 == EINTR) // 4
    {
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("failed to sd_bus_call_method(%s): \"%s\", retrying...\n"),
                  (toggle_on ? ACE_TEXT (COMMON_SD_BUS_METHOD_UNIT_START_STRING)
                             : ACE_TEXT (COMMON_SD_BUS_METHOD_UNIT_STOP_STRING)),
                  ACE_TEXT (error_s.message)));
      ACE_Time_Value one_second (1, 0);
      result_2 = ACE_OS::sleep (one_second);
      if (result_2 == -1)
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE_OS::sleep(%#T): \"%m\", continuing\n"),
                    &one_second));
      goto retry;
    } // end IF
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to sd_bus_call_method(%s): \"%s\", aborting\n"),
                (toggle_on ? ACE_TEXT (COMMON_SD_BUS_METHOD_UNIT_START_STRING)
                           : ACE_TEXT (COMMON_SD_BUS_METHOD_UNIT_STOP_STRING)),
                ACE_TEXT (error_s.message)));
    goto finish;
  } // end IF
  ACE_ASSERT (message_p);

  string_p = NULL;
  result_2 =
      sd_bus_message_read (message_p,
                           (toggle_on ? ACE_TEXT_ALWAYS_CHAR (COMMON_SD_BUS_METHOD_UNIT_START_OUT_SIGNATURE_STRING)
                                      : ACE_TEXT_ALWAYS_CHAR (COMMON_SD_BUS_METHOD_UNIT_STOP_OUT_SIGNATURE_STRING)),
                           &string_p);
  if (unlikely (result_2 < 0))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to sd_bus_message_read(): \"%s\", aborting\n"),
                ACE_TEXT (ACE_OS::strerror (-result_2))));
    goto finish;
  } // end IF
  ACE_ASSERT (string_p);

  cb_data_s.objectPath = string_p;

  sd_bus_message_unref (message_p);
  message_p = NULL;

//  // step2: retrieve the job id
//  result_2 =
//      sd_bus_get_property (bus_p,
//                           ACE_TEXT_ALWAYS_CHAR (COMMON_SD_BUS_SERVICE_STRING),                    /* service */
//                           cb_data_s.objectPath.c_str (),                                          /* object path */
//                           ACE_TEXT_ALWAYS_CHAR (COMMON_SD_BUS_INTERFACE_UNIT_STRING),             /* interface name */
//                           ACE_TEXT_ALWAYS_CHAR (COMMON_SD_BUS_PROPERTY_JOB_ID_STRING),            /* member name */
//                           &error_s,                                                               /* object to return error in */
//                           &message_p,                                                             /* return message on success */
//                           ACE_TEXT_ALWAYS_CHAR (COMMON_SD_BUS_PROPERTY_JOB_ID_SIGNATURE_STRING)); /* property signature */
//  if (unlikely (result_2 < 0))
//  { // *NOTE*: most likely reason: job was removed already
//    if (result_2 != -53) // unknown object
//      ACE_DEBUG ((LM_ERROR,
//                  ACE_TEXT ("failed to sd_bus_get_property(%s): \"%s\", aborting\n"),
//                  ACE_TEXT (COMMON_SD_BUS_PROPERTY_JOB_ID_STRING),
//                  ACE_TEXT (error_s.message)));
//    goto finish;
//  } // end IF
//  ACE_ASSERT (message_p);

//  result_2 =
//      sd_bus_message_read (message_p,
//                           ACE_TEXT_ALWAYS_CHAR (COMMON_SD_BUS_PROPERTY_JOB_ID_SIGNATURE_STRING),
//                           &cb_data_s.id);
//  if (unlikely (result_2 < 0))
//  {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("failed to sd_bus_message_read(): \"%s\", aborting\n"),
//                ACE_TEXT (ACE_OS::strerror (-result_2))));
//    goto finish;
//  } // end IF
//  ACE_ASSERT (cb_data_s.id);

  if (likely (!waitForCompletion_in))
  {
    result = true;
    goto finish;
  } // end IF

  do
  {
    /* process signal(s) */
    result_2 = sd_bus_process (bus_p, NULL);
    if (unlikely (result_2 < 0))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to sd_bus_process(%@): \"%s\", aborting\n"),
                  bus_p,
                  ACE_TEXT (ACE_OS::strerror (-result_2))));
      goto finish;
    } // end IF
    if (!cb_data_s.result.empty ())
      break; // done

    result_2 = sd_bus_wait (bus_p, (uint64_t) - 1);
    if (unlikely (result_2 < 0))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to sd_bus_wait(%@): \"%s\", aborting\n"),
                  bus_p,
                  ACE_TEXT (ACE_OS::strerror (-result_2))));
      goto finish;
    } // end IF
  } while (true);

  result =
      !ACE_OS::strcmp (cb_data_s.result.c_str (),
                       ACE_TEXT_ALWAYS_CHAR (COMMON_SD_BUS_PROPERTY_JOB_RESULT_DONE_STRING));

finish:
  sd_bus_error_free (&error_s);
  sd_bus_message_unref (message_p);

  return result;
}

bool
Common_DBus_Tools::isUnitActive (struct sd_bus* bus_in,
                                 const std::string& unitName_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_DBus_Tools::isUnitActive"));

  // initialize return value(s)
  bool result = false;

  // sanity check(s)
  ACE_ASSERT (!unitName_in.empty ());

  struct sd_bus* bus_p = (bus_in ? bus_in : Common_DBus_Tools::bus);
  sd_bus_error error_s = SD_BUS_ERROR_NULL;
  struct sd_bus_message* message_p = NULL;
  const char* string_p = NULL;

  // sanity check(s)
  ACE_ASSERT (bus_p);

  int result_2 =
      sd_bus_call_method (bus_p,
                          ACE_TEXT_ALWAYS_CHAR (COMMON_SD_BUS_SERVICE_STRING),                             /* service */
                          ACE_TEXT_ALWAYS_CHAR (COMMON_SD_BUS_OBJECT_PATH_STRING),                         /* object path */
                          ACE_TEXT_ALWAYS_CHAR (COMMON_SD_BUS_INTERFACE_MANAGER_STRING),                   /* interface name */
                          ACE_TEXT_ALWAYS_CHAR (COMMON_SD_BUS_METHOD_UNIT_FILE_STATE_STRING),              /* method name */
                          &error_s,                                                                        /* object to return error in */
                          &message_p,                                                                      /* return message on success */
                          ACE_TEXT_ALWAYS_CHAR (COMMON_SD_BUS_METHOD_UNIT_FILE_STATE_IN_SIGNATURE_STRING), /* input signature */
                          unitName_in.c_str ());                                                           /* first argument */
  if (unlikely (result_2 < 0))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to sd_bus_call_method(%s): \"%s\", aborting\n"),
                ACE_TEXT (COMMON_SD_BUS_METHOD_UNIT_FILE_STATE_STRING),
                ACE_TEXT (error_s.message)));
    goto finish;
  } // end IF
  ACE_ASSERT (message_p);

  result_2 =
      sd_bus_message_read (message_p,
                           ACE_TEXT_ALWAYS_CHAR (COMMON_SD_BUS_METHOD_UNIT_FILE_STATE_OUT_SIGNATURE_STRING),
                           &string_p);
  if (unlikely (result_2 < 0))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to sd_bus_message_read(): \"%s\", aborting\n"),
                ACE_TEXT (ACE_OS::strerror (-result_2))));
    goto finish;
  } // end IF
  ACE_ASSERT (string_p);

  result =
      !ACE_OS::strcmp (string_p,
                       ACE_TEXT_ALWAYS_CHAR (COMMON_SD_BUS_METHOD_UNIT_FILE_STATE_ENABLED_STRING));

finish:
  sd_bus_error_free (&error_s);
  sd_bus_message_unref (message_p);

  return result;
}

bool
Common_DBus_Tools::toggleUnitActive (struct sd_bus* bus_in,
                                     const std::string& unitName_in,
                                     bool runTime_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_DBus_Tools::toggleUnitActive"));

  bool result = false;

  struct sd_bus* bus_p = (bus_in ? bus_in : Common_DBus_Tools::bus);
  int result_2 = -1;
  sd_bus_error error_s = SD_BUS_ERROR_NULL;
  struct sd_bus_message* message_p = NULL;
  const char* string_p = NULL, *string_2 = NULL, *string_3 = NULL;
  bool toggle_on = false;

  // sanity check(s)
  ACE_ASSERT (bus_p);

  toggle_on = !Common_DBus_Tools::isUnitActive (bus_p,
                                                unitName_in);

  // toggle unit
retry:
  result_2 =
      sd_bus_call_method (bus_p,
                          ACE_TEXT_ALWAYS_CHAR (COMMON_SD_BUS_SERVICE_STRING),                                             /* service */
                          ACE_TEXT_ALWAYS_CHAR (COMMON_SD_BUS_OBJECT_PATH_STRING),                                         /* object path */
                          ACE_TEXT_ALWAYS_CHAR (COMMON_SD_BUS_INTERFACE_MANAGER_STRING),                                   /* interface name */
                          (toggle_on ? ACE_TEXT_ALWAYS_CHAR (COMMON_SD_BUS_METHOD_UNIT_FILE_ENABLE_STRING)
                                     : ACE_TEXT_ALWAYS_CHAR (COMMON_SD_BUS_METHOD_UNIT_FILE_DISABLE_STRING)),              /* method name */
                          &error_s,                                                                                        /* object to return error in */
                          &message_p,                                                                                      /* return message on success */
                          (toggle_on ? ACE_TEXT_ALWAYS_CHAR (COMMON_SD_BUS_METHOD_UNIT_FILE_ENABLE_IN_SIGNATURE_STRING)
                                     : ACE_TEXT_ALWAYS_CHAR (COMMON_SD_BUS_METHOD_UNIT_FILE_DISABLE_IN_SIGNATURE_STRING)), /* input signature */
                          unitName_in.c_str (),                                                                            /* first argument */
                          (runTime_in ? 1 : 0));                                                                           /* second argument */
  if (unlikely (result_2 < 0))
  { // *NOTE*: if the user/application does not have permission to manage the
    //         unit, a password dialog appears and EINTR is returned
    //         --> wait for a few seconds and retry
    if (-result_2 == EINTR) // 4
    {
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("failed to sd_bus_call_method(%s): \"%s\", retrying...\n"),
                  (toggle_on ? ACE_TEXT_ALWAYS_CHAR (COMMON_SD_BUS_METHOD_UNIT_FILE_ENABLE_STRING)
                             : ACE_TEXT_ALWAYS_CHAR (COMMON_SD_BUS_METHOD_UNIT_FILE_DISABLE_STRING)),
                  ACE_TEXT (error_s.message)));
      ACE_Time_Value one_second (1, 0);
      result_2 = ACE_OS::sleep (one_second);
      if (result_2 == -1)
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE_OS::sleep(%#T): \"%m\", continuing\n"),
                    &one_second));
      goto retry;
    } // end IF
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to sd_bus_call_method(%s): \"%s\", aborting\n"),
                (toggle_on ? ACE_TEXT_ALWAYS_CHAR (COMMON_SD_BUS_METHOD_UNIT_FILE_ENABLE_STRING)
                           : ACE_TEXT_ALWAYS_CHAR (COMMON_SD_BUS_METHOD_UNIT_FILE_DISABLE_STRING)),
                ACE_TEXT (error_s.message)));
    goto finish;
  } // end IF
  ACE_ASSERT (message_p);

  result_2 =
      sd_bus_message_enter_container (message_p,
                                      SD_BUS_TYPE_ARRAY,
                                      ACE_TEXT_ALWAYS_CHAR ("sss"));
  if (unlikely (result_2 < 0))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to sd_bus_message_enter_container(): \"%s\", aborting\n"),
                ACE_TEXT (ACE_OS::strerror (-result_2))));
    goto finish;
  } // end IF
  result_2 =
      sd_bus_message_read (message_p,
                           ACE_TEXT_ALWAYS_CHAR ("sss"),
                           string_p,
                           string_2,
                           string_3);
  if (unlikely (result_2 < 0))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to sd_bus_message_read(): \"%s\", aborting\n"),
                ACE_TEXT (ACE_OS::strerror (-result_2))));
    goto finish;
  } // end IF
  ACE_ASSERT (string_p);
  ACE_ASSERT (string_2);
  ACE_ASSERT (string_3);

  result =
      !ACE_OS::strcmp (string_p,
                       ACE_TEXT_ALWAYS_CHAR (COMMON_SD_BUS_PROPERTY_JOB_RESULT_DONE_STRING));

finish:
  sd_bus_error_free (&error_s);
  sd_bus_message_unref (message_p);

  return result;
}

//bool
//Net_Common_Tools::isNetworkManagerRunning ()
//{
//  NETWORK_TRACE (ACE_TEXT ("Net_Common_Tools::isNetworkManagerRunning"));

//  std::string commandline_string = ACE_TEXT_ALWAYS_CHAR ("systemctl ");
//  commandline_string +=
//      ACE_TEXT_ALWAYS_CHAR (COMMON_SYSTEMD_UNIT_COMMAND_STATUS);
//  commandline_string +=
//      ACE_TEXT_ALWAYS_CHAR (" ");
//  commandline_string +=
//      ACE_TEXT_ALWAYS_CHAR (COMMON_SYSTEMD_UNIT_NETWORKMANAGER);
//  std::string output_string;
//  if (unlikely (!Common_Tools::command (commandline_string,
//                                        output_string)))
//  {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("failed to Common_Tools::command(\"%s\"), aborting\n"),
//                ACE_TEXT (commandline_string.c_str ())));
//    return false; // *TODO*: avoid false negative
//  } // end IF

//  std::istringstream converter;
//  char buffer [BUFSIZ];
//  std::string regex_string =
//      ACE_TEXT_ALWAYS_CHAR ("^   Active: (.+) \\((.+)\\) since (.+); (.+) ago$");
//  std::regex regex (regex_string);
//  std::cmatch match_results;
//  converter.str (output_string);
//  std::string status_string;
//  do
//  {
//    converter.getline (buffer, sizeof (buffer));
//    if (likely (!std::regex_match (buffer,
//                                   match_results,
//                                   regex,
//                                   std::regex_constants::match_default)))
//      continue;
//    ACE_ASSERT (match_results.ready () && !match_results.empty ());
//    ACE_ASSERT (match_results[1].matched);
//    ACE_ASSERT (match_results[2].matched);
//    ACE_ASSERT (match_results[3].matched);
//    ACE_ASSERT (match_results[4].matched);

//    status_string = match_results[1];
//    if (ACE_OS::strcmp (status_string.c_str (),
//                        ACE_TEXT_ALWAYS_CHAR ("active")))
//      break;
//    status_string = match_results[2];
//    if (ACE_OS::strcmp (status_string.c_str (),
//                        ACE_TEXT_ALWAYS_CHAR ("running")))
//      break;
//    return true;
//  } while (!converter.fail ());

//  return false;
//}
#endif // SD_BUS_SUPPORT
