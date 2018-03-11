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
#include "systemd/sd-bus.h"
#endif // SD_BUS_SUPPORT

#include "ace/Log_Msg.h"

#include "common_macros.h"

#include "common_dbus_defines.h"
#include "common_dbus_common.h"

#if defined (DBUS_SUPPORT)
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
#endif // DBUS_SUPPORT

#if defined (SD_BUS_SUPPORT)
std::string
Common_DBus_Tools::unitToObjectPath (struct sd_bus* bus_in,
                                     const std::string& unitName_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_DBus_Tools::unitToObjectPath"));

  // initialize return value(s)
  std::string return_value;

  // sanity check(s)
  ACE_ASSERT (bus_in);
  ACE_ASSERT (!unitName_in.empty ());

  sd_bus_error error_s = SD_BUS_ERROR_NULL;
  struct sd_bus_message* message_p = NULL;
  const char* string_p = NULL;

  int result_2 =
      sd_bus_call_method (bus_in,
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
Common_DBus_Tools::isUnitRunning (const std::string& unitName_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_DBus_Tools::isUnitRunning"));

  bool result = false;

  sd_bus_error error_s = SD_BUS_ERROR_NULL;
  struct sd_bus_message* message_p = NULL;
  struct sd_bus* bus_p = NULL;
  const char* string_p = NULL;
  std::string unit_object_path_string;

  int result_2 = sd_bus_open_system (&bus_p);
  if (unlikely (result_2 < 0))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to sd_bus_open_system(): \"%s\", aborting\n"),
                ACE_TEXT (ACE_OS::strerror (-result_2))));
    return false;
  } // end IF
  ACE_ASSERT (bus_p);

  // step1: retrieve unit object path
  unit_object_path_string = Common_DBus_Tools::unitToObjectPath (bus_p,
                                                                 unitName_in);
  if (unit_object_path_string.empty ())
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_DBus_Tools::unitToObjectPath(%@,%s): \"%s\", aborting\n"),
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
  sd_bus_unref (bus_p);

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

  struct Common_DBusSignalSystemdJobCBData* cb_data_p =
      static_cast<struct Common_DBusSignalSystemdJobCBData*> (CBData_in);

  // sanity check(s)
  ACE_ASSERT (!cb_data_p->objectPath.empty ());

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
    goto finish;
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

    result = -1; // done
  } // end IF
  else
    result = 0;

finish:
  sd_bus_message_unref (message_in);

  return result;
}

bool
Common_DBus_Tools::toggleUnit (const std::string& unitName_in,
                               bool waitForCompletion_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_DBus_Tools::toggleUnit"));

  bool result = false;

  sd_bus_error error_s = SD_BUS_ERROR_NULL;
  struct sd_bus_message* message_p = NULL;
  struct sd_bus* bus_p = NULL;
  struct sd_bus_slot* slot_p = NULL;
  const char* string_p = NULL;
  struct Common_DBusSignalSystemdJobCBData cb_data_s;
  bool toggle_on = !Common_DBus_Tools::isUnitRunning (unitName_in);

  int result_2 = sd_bus_open_system (&bus_p);
  if (unlikely (result_2 < 0))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to sd_bus_open_system(): \"%s\", aborting\n"),
                ACE_TEXT (ACE_OS::strerror (-result_2))));
    return false;
  } // end IF
  ACE_ASSERT (bus_p);

  if (unlikely (waitForCompletion_in))
  {
    result_2 =
        sd_bus_add_match (bus_p,
                          &slot_p,
                          ACE_TEXT_ALWAYS_CHAR (COMMON_SD_BUS_FILTER_SIGNAL_MANAGER_STRING),
                          common_dbus_sd_job_signal_handler_cb,
                          &cb_data_s);
    if (unlikely (result_2 < 0))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to sd_bus_add_match(\"%s\"): \"%s\", aborting\n"),
                  ACE_TEXT (COMMON_SD_BUS_FILTER_SIGNAL_MANAGER_STRING),
                  ACE_TEXT (ACE_OS::strerror (-result_2))));
      goto finish;
    } // end IF
    ACE_ASSERT (slot_p);
  } // end IF

  // step1: toggle unit
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
  {
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
  sd_bus_slot_unref (slot_p);
  sd_bus_unref (bus_p);

  return result;
}
#endif // SD_BUS_SUPPORT
