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

#include "common_os_tools.h"

#include <fstream>
#include <iostream>
#include <limits>
#include <regex>
#include <sstream>
#include <utility>
#include <vector>

#if defined (ACE_LINUX)
#include "sys/capability.h"
#include "sys/prctl.h"
#include "sys/utsname.h"

#include "linux/capability.h"
#include "linux/prctl.h"
#include "linux/securebits.h"

#include "aio.h"
#include "grp.h"
#elif defined (__sun) && defined (__SVR4)
#include "rctl.h"
#endif // ACE_LINUX || SUN_SOLARIS_11

#include "ace/Log_Msg.h"
#include "ace/OS.h"
#if defined (__sun) && defined (__SVR4)
#include "ace/OS_Memory.h"
#endif // SUN_SOLARIS_11
#include "ace/Time_Value.h"

#if defined (HAVE_CONFIG_H)
#include "Common_config.h"
#endif // HAVE_CONFIG_H

#include "common_defines.h"
#include "common_file_tools.h"
#include "common_macros.h"
#if defined (ACE_LINUX)
#include "common_process_tools.h"
#include "common_string_tools.h"
#endif // ACE_LINUX

#include "common_error_tools.h"

#include "common_signal_tools.h"

#include "common_time_common.h"

//////////////////////////////////////////

void
Common_OS_Tools::printLocales ()
{
  COMMON_TRACE (ACE_TEXT ("Common_OS_Tools::printLocales"));

  std::vector<std::string> locales;
  // *TODO*: this should work on most Linux systems, but is really a bad idea:
  //         - relies on local 'locale'
  //         - temporary files
  //         - system(3) call
  //         --> extremely inefficient; remove ASAP
  std::string filename_string =
      Common_File_Tools::getTempFilename (ACE_TEXT_ALWAYS_CHAR (""));
  std::string command_line_string =
      ACE_TEXT_ALWAYS_CHAR ("locale -a >> ");
  command_line_string += filename_string;

  int result = ACE_OS::system (ACE_TEXT (command_line_string.c_str ()));
  if (unlikely ((result == -1)      ||
                !WIFEXITED (result) ||
                WEXITSTATUS (result)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::system(\"%s\"): \"%m\" (result was: %d), aborting\n"),
                ACE_TEXT (command_line_string.c_str ()),
                WEXITSTATUS (result)));
    return;
  } // end IF
  unsigned char* data_p = NULL;
  ACE_UINT64 file_size_i = 0;
  if (unlikely (!Common_File_Tools::load (filename_string,
                                          data_p,
                                          file_size_i,
                                          0)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_File_Tools::load(\"%s\"), returning\n"),
                ACE_TEXT (filename_string.c_str ())));
    return;
  } // end IF
  if (unlikely (!Common_File_Tools::deleteFile (filename_string)))
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_File_Tools::deleteFile(\"%s\"), continuing\n"),
                ACE_TEXT (filename_string.c_str ())));

  std::string locales_string = reinterpret_cast<char*> (data_p);
  delete [] data_p;

  char buffer_a[BUFSIZ];
  std::istringstream converter;
  converter.str (locales_string);
  do {
    converter.getline (buffer_a, sizeof (char[BUFSIZ]));
    locales.push_back (buffer_a);
  } while (!converter.fail ());

  int index_i = 1;
  for (std::vector<std::string>::const_iterator iterator = locales.begin ();
       iterator != locales.end ();
       ++iterator, ++index_i)
    ACE_DEBUG ((LM_INFO,
                ACE_TEXT ("%d: \"%s\"\n"),
                index_i,
                ACE_TEXT ((*iterator).c_str ())));
}

std::string
Common_OS_Tools::capabilityToString (unsigned long capability_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_OS_Tools::capabilityToString"));

  std::string return_value;

  // sanity check(s)
  if (unlikely (!CAP_IS_SUPPORTED (capability_in)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("capability (was: %u) not supported: \"%m\", aborting\n"),
                capability_in));
    return return_value;
  } // end IF

  char* capability_name_string_p = ::cap_to_name (capability_in);
  if (!capability_name_string_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ::cap_to_name(%d): \"%m\", aborting\n"),
                capability_in));
    return return_value;
  } // end IF
  return_value = capability_name_string_p;

  // clean up
  int result = ::cap_free (capability_name_string_p);
  if (unlikely (result == -1))
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ::cap_free(): \"%m\", continuing\n")));

  return return_value;
}

bool
Common_OS_Tools::hasCapability (unsigned long capability_in,
                                cap_flag_t set_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_OS_Tools::hasCapability"));

  // sanity check(s)
  if (unlikely (!CAP_IS_SUPPORTED (capability_in)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("capability (was: %s (%u)) not supported: \"%m\", aborting\n"),
                ACE_TEXT (Common_Tools::capabilityToString (capability_in).c_str ()), capability_in));
    return false;
  } // end IF

  cap_t capabilities_p = NULL;
//  capabilities_p = cap_init ();
  capabilities_p = ::cap_get_proc ();
  if (unlikely (!capabilities_p))
  {
    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("failed to ::cap_init(): \"%m\", returning\n")));
                ACE_TEXT ("failed to ::cap_get_proc(): \"%m\", aborting\n")));
    return false;
  } // end IF

  cap_flag_value_t in_set = CAP_CLEAR;
  int result_2 = ::cap_get_flag (capabilities_p, capability_in,
                                 set_in, &in_set);
  if (unlikely (result_2 == -1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ::cap_get_flag(%s,%d): \"%m\", aborting\n"),
                ACE_TEXT (Common_Tools::capabilityToString (capability_in).c_str ()),
                set_in));
    goto clean;
  } // end IF

clean:
  result_2 = ::cap_free (capabilities_p);
  if (unlikely (result_2 == -1))
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ::cap_free(): \"%m\", continuing\n")));

  return (in_set == CAP_SET);
}

void
Common_OS_Tools::printCapabilities ()
{
  COMMON_TRACE (ACE_TEXT ("Common_OS_Tools::printCapabilities"));

  int result = -1;

  // step1: read 'securebits' flags of the calling thread
  result = ::prctl (PR_GET_SECUREBITS);
  if (unlikely (result == -1))
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ::prctl(PR_GET_SECUREBITS): \"%m\", continuing\n")));
  else
    ACE_DEBUG ((LM_INFO,
//                ACE_TEXT ("%P/%t: securebits set/locked:\nSECURE_NOROOT:\t\t%s/%s\nSECURE_NO_SETUID_FIXUP:\t%s/%s\nSECURE_KEEP_CAPS:\t%s/%s\n"),
                ACE_TEXT ("%P/%t: securebits set/locked:\nSECURE_NOROOT:\t\t%s/%s\nSECURE_NO_SETUID_FIXUP:\t%s/%s\nSECURE_KEEP_CAPS:\t\t%s/%s\nSECURE_NO_CAP_AMBIENT_RAISE:\t%s/%s\n"),
                ((result & SECBIT_NOROOT) ? ACE_TEXT ("yes") : ACE_TEXT ("no")),
                ((result & SECBIT_NOROOT_LOCKED) ? ACE_TEXT ("yes") : ACE_TEXT ("no")),
                ((result & SECBIT_NO_SETUID_FIXUP) ? ACE_TEXT ("yes") : ACE_TEXT ("no")),
                ((result & SECBIT_NO_SETUID_FIXUP_LOCKED) ? ACE_TEXT ("yes") : ACE_TEXT ("no")),
                ((result & SECBIT_KEEP_CAPS) ? ACE_TEXT ("yes") : ACE_TEXT ("no")),
                ((result & SECBIT_KEEP_CAPS_LOCKED) ? ACE_TEXT ("yes") : ACE_TEXT ("no")),//));
                ((result & SECBIT_NO_CAP_AMBIENT_RAISE) ? ACE_TEXT ("yes") : ACE_TEXT ("no")),
                ((result & SECBIT_NO_CAP_AMBIENT_RAISE_LOCKED) ? ACE_TEXT ("yes") : ACE_TEXT ("no"))));

  cap_t capabilities_p = NULL;
  bool in_bounding_set, in_ambient_set;
  cap_flag_value_t in_effective_set, in_inheritable_set, in_permitted_set;
  capabilities_p = cap_get_proc ();
  if (unlikely (!capabilities_p))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ::cap_get_proc(): \"%m\", returning\n")));
    return;
  } // end IF

//#if defined (_DEBUG)
//  ssize_t length_i = 0;
//  char* string_p = cap_to_text (capabilities_p, &length_i);
//  if (unlikely (!string_p))
//  {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("failed to ::cap_to_text(): \"%m\", returning\n")));
//    goto clean;
//  } // end IF
//  ACE_DEBUG ((LM_INFO,
//              ACE_TEXT ("%P:%t: capabilities \"%s\"\n"),
//              ACE_TEXT (string_p)));
//#endif // _DEBUG

  // step2: in 'ambient'/'bounding' set of the calling thread ?
  ACE_DEBUG ((LM_INFO,
              ACE_TEXT ("%P/%t: capability ambient/bounding\teffective/inheritable/permitted\n")));
  for (unsigned long capability = 0;
       capability <= CAP_LAST_CAP;
       ++capability)
  {
    // *TODO*: currently, this fails on Linux (Debian)...
//    result = 0;
    result = ::prctl (PR_CAP_AMBIENT, PR_CAP_AMBIENT_IS_SET,
                      capability,
                      0, 0);
    if (unlikely (result == -1))
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ::prctl(PR_CAP_AMBIENT,PR_CAP_AMBIENT_IS_SET,%s): \"%m\", continuing\n"),
                  ACE_TEXT (Common_Tools::capabilityToString (capability).c_str ())));
    in_ambient_set = (result == 1);

    result = ::prctl (PR_CAPBSET_READ,
                      capability);
    if (unlikely (result == -1))
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ::prctl(PR_CAPBSET_READ,%s): \"%m\", continuing\n"),
                  ACE_TEXT (Common_Tools::capabilityToString (capability).c_str ())));
    in_bounding_set = (result == 1);

    result = ::cap_get_flag (capabilities_p, capability,
                             CAP_EFFECTIVE, &in_effective_set);
    if (unlikely (result == -1))
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ::cap_get_flag(CAP_EFFECTIVE,%s): \"%m\", continuing\n"),
                  ACE_TEXT (Common_Tools::capabilityToString (capability).c_str ())));
    result = ::cap_get_flag (capabilities_p, capability,
                             CAP_INHERITABLE, &in_inheritable_set);
    if (unlikely (result == -1))
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ::cap_get_flag(CAP_INHERITABLE,%s): \"%m\", continuing\n"),
                  ACE_TEXT (Common_Tools::capabilityToString (capability).c_str ())));
    result = ::cap_get_flag (capabilities_p, capability,
                             CAP_PERMITTED, &in_permitted_set);
    if (unlikely (result == -1))
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ::cap_get_flag(CAP_PERMITTED,%s): \"%m\", continuing\n"),
                  ACE_TEXT (Common_Tools::capabilityToString (capability).c_str ())));

    ACE_DEBUG ((LM_INFO,
                ACE_TEXT ("%s (%d):\t\t%s/%s\t\t%s/%s/%s\n"),
                ACE_TEXT (Common_Tools::capabilityToString (capability).c_str ()), capability,
                (in_ambient_set ? ACE_TEXT ("yes") : ACE_TEXT ("no")),
                (in_bounding_set ? ACE_TEXT ("yes") : ACE_TEXT ("no")),
                ((in_effective_set == CAP_SET) ? ACE_TEXT ("yes") : ACE_TEXT ("no")),
                ((in_inheritable_set == CAP_SET) ? ACE_TEXT ("yes") : ACE_TEXT ("no")),
                ((in_permitted_set == CAP_SET) ? ACE_TEXT ("yes") : ACE_TEXT ("no"))));
  } // end FOR

  // clean up
//#if defined (_DEBUG)
//  if (likely (string_p))
//  {
//    result = cap_free (string_p);
//    if (unlikely (result == -1))
//      ACE_DEBUG ((LM_ERROR,
//                  ACE_TEXT ("failed to ::cap_free(): \"%m\", continuing\n")));
//  } // end IF
//#endif
  if (likely (capabilities_p))
  {
    result = cap_free (capabilities_p);
    if (unlikely (result == -1))
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ::cap_free(): \"%m\", continuing\n")));
  } // end IF
}

bool
Common_OS_Tools::setCapability (unsigned long capability_in,
                                cap_flag_t set_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_OS_Tools::setCapability"));

  bool result = false;

  // sanity check(s)
  if (unlikely (!CAP_IS_SUPPORTED (capability_in)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("capability (was: %s (%u)) not supported: \"%m\", aborting\n"),
                ACE_TEXT (Common_Tools::capabilityToString (capability_in).c_str ()), capability_in));
    return false;
  } // end IF

  cap_t capabilities_p = NULL;
//  capabilities_p = cap_init ();
  capabilities_p = ::cap_get_proc ();
  if (unlikely (!capabilities_p))
  {
    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("failed to ::cap_init(): \"%m\", returning\n")));
                ACE_TEXT ("failed to ::cap_get_proc(): \"%m\", aborting\n")));
    return false;
  } // end IF

  int result_2 = -1;
  if (likely (set_in == CAP_EFFECTIVE))
  {
    // verify that the capability is in the 'permitted' set
    if (!Common_Tools::hasCapability (capability_in, CAP_PERMITTED))
      ACE_DEBUG ((LM_WARNING,
                  ACE_TEXT ("%P/%t: capability (was: %s (%u)) not in 'permitted' set: cannot enable, continuing\n"),
                  ACE_TEXT (Common_Tools::capabilityToString (capability_in).c_str ()), capability_in));
  } // end IF

  cap_value_t capabilities_a[CAP_LAST_CAP + 1];
  ACE_OS::memset (&capabilities_a, 0, sizeof (cap_value_t[CAP_LAST_CAP + 1]));
  capabilities_a[0] = capability_in;
  result_2 = ::cap_set_flag (capabilities_p, set_in,
                             1, capabilities_a,
                             CAP_SET);
  if (unlikely (result_2 == -1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ::cap_set_flag(%s,%d,CAP_SET): \"%m\", aborting\n"),
                ACE_TEXT (Common_Tools::capabilityToString (capability_in).c_str ()),
                set_in));
    goto clean;
  } // end IF

  result_2 = ::cap_set_proc (capabilities_p);
  if (unlikely (result_2 == -1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ::cap_set_proc(%s)): \"%m\", aborting\n"),
                ACE_TEXT (Common_Tools::capabilityToString (capability_in).c_str ())));
    goto clean;
  } // end IF
  if (likely (Common_Tools::hasCapability (capability_in, set_in)))
    result = true;

clean:
  result_2 = ::cap_free (capabilities_p);
  if (unlikely (result_2 == -1))
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ::cap_free(): \"%m\", continuing\n")));

  return result;
}

bool
Common_OS_Tools::dropCapability (unsigned long capability_in,
                                 cap_flag_t set_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_OS_Tools::dropCapability"));

  bool result = false;

  // sanity check(s)
  if (unlikely (!CAP_IS_SUPPORTED (capability_in)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("capability (was: %s (%u)) not supported: \"%m\", aborting\n"),
                ACE_TEXT (Common_Tools::capabilityToString (capability_in).c_str ()), capability_in));
    return false;
  } // end IF

  cap_t capabilities_p = NULL;
//  capabilities_p = cap_init ();
  capabilities_p = ::cap_get_proc ();
  if (unlikely (!capabilities_p))
  {
    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("failed to ::cap_init(): \"%m\", returning\n")));
                ACE_TEXT ("failed to ::cap_get_proc(): \"%m\", aborting\n")));
    return false;
  } // end IF

  cap_value_t capabilities_a[CAP_LAST_CAP + 1];
  capabilities_a[0] = capability_in;
  int result_2 = ::cap_set_flag (capabilities_p, set_in,
                                 1, capabilities_a,
                                 CAP_CLEAR);
  if (unlikely (result_2 == -1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ::cap_set_flag(%s,%d,CAP_CLEAR): \"%m\", aborting\n"),
                ACE_TEXT (Common_Tools::capabilityToString (capability_in).c_str ()),
                set_in));
    goto clean;
  } // end IF
  result = true;

clean:
  result_2 = ::cap_free (capabilities_p);
  if (unlikely (result_2 == -1))
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ::cap_free(): \"%m\", continuing\n")));

  return result;
}

bool
Common_OS_Tools::switchUser (uid_t userId_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_OS_Tools::switchUser"));

  uid_t  user_id =
      (userId_in == static_cast<uid_t> (-1) ? ACE_OS::getuid () : userId_in);
  // *IMPORTANT NOTE*: (on Linux) the process requires the CAP_SETUID capability
  //                   for this to work
  int result = ACE_OS::seteuid (user_id);
  if (unlikely (result == -1))
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::seteuid(%u): \"%m\", aborting\n"),
                user_id));
  else
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("%sset effective user id (is: %u)\n"),
                (userId_in == static_cast<uid_t> (-1) ? ACE_TEXT ("re") : ACE_TEXT ("")),
                user_id));

  return (result == 0);
}

Common_UserGroups_t
Common_OS_Tools::getUserGroups (uid_t userId_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_OS_Tools::getUserGroups"));

  // initialize return value(s)
  Common_UserGroups_t return_value;

  uid_t user_id =
      ((static_cast<int>(userId_in) == -1) ? ACE_OS::geteuid () : userId_in);
  std::string username_string, realname_string;
  Common_Tools::getUserName (user_id,
                             username_string,
                             realname_string);
  struct passwd passwd_s, *passwd_p = NULL;
  ACE_OS::memset (&passwd_s, 0, sizeof (struct passwd));
  char buffer_a[BUFSIZ];
  ACE_OS::memset (buffer_a, 0, sizeof (char[BUFSIZ]));
  struct group group_s, *group_p;
  ACE_OS::memset (&group_s, 0, sizeof (struct group));
  int result =
      ::getpwuid_r (user_id,               // user id
                    &passwd_s,             // passwd entry
                    buffer_a,              // buffer
                    sizeof (char[BUFSIZ]), // buffer size
                    &passwd_p);            // result (handle)
  if (unlikely ((result == -1) || !passwd_p))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ::getpwuid_r(%u): \"%m\", aborting\n"),
                user_id));
    return return_value;
  } // end IF
  return_value.push_back (Common_Tools::groupIdToString (passwd_s.pw_gid));

  setgrent ();
  do
  {
    group_p = NULL;
    result = ::getgrent_r (&group_s,
                           buffer_a,
                           sizeof (char[BUFSIZ]),
                           &group_p);
    if (unlikely (result || !group_p))
    {
      if (result != ENOENT)
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ::getgrent_r(): \"%m\", returning\n")));
      break;
    } // end IF
    ACE_ASSERT (group_s.gr_mem);
    for (char** string_pp = group_s.gr_mem;
         *string_pp;
         ++string_pp)
      if (!ACE_OS::strcmp (username_string.c_str (),
                           *string_pp))
        return_value.push_back (group_s.gr_name);
  } while (true);
  ::endgrent ();

  return return_value;
}

bool
Common_OS_Tools::addGroupMember (uid_t userId_in,
                                 gid_t groupId_in,
                                 bool persist_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_OS_Tools::addGroupMember"));

#if defined (_DEBUG)
  Common_Tools::printCapabilities ();
#endif // _DEBUG

  bool result = false;

  uid_t user_id =
      ((static_cast<int>(userId_in) == -1) ? ACE_OS::geteuid () : userId_in);
  std::string username_string, realname_string;
  Common_Tools::getUserName (user_id,
                             username_string,
                             realname_string);

  // sanity check(s)
  if (unlikely (Common_Tools::isGroupMember (userId_in, groupId_in)))
    return true; // nothing to do

  if (likely (persist_in))
  {
    // *TODO*: use putgrent(3)
    std::string command_output_string;
    std::string command_line_string =
        ACE_TEXT_ALWAYS_CHAR (COMMON_COMMAND_GPASSWD);
//        ACE_TEXT_ALWAYS_CHAR (COMMON_COMMAND_USERMOD_STRING);
    command_line_string += ACE_TEXT_ALWAYS_CHAR (" -a ");
    command_line_string += username_string;
//    command_line_string += ACE_TEXT_ALWAYS_CHAR (" -a -G ");
    command_line_string += ACE_TEXT_ALWAYS_CHAR (" ");
    command_line_string += Common_Tools::groupIdToString (groupId_in);
//    command_line_string += ACE_TEXT_ALWAYS_CHAR (" ");
//    command_line_string += username_string;
    int exit_status_i = 0;
    if (unlikely(!Common_Process_Tools::command (command_line_string,
                                                 exit_status_i,
                                                 command_output_string)))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Common_Tools::command(\"%s\"), aborting\n"),
                  ACE_TEXT (command_line_string.c_str ())));
      return false;
    } // end IF

    result = true;
  } // end IF
  else
  {
    // *TODO* use setgroups(2)
    ACE_ASSERT (false);
    ACE_NOTSUP_RETURN (false);

    ACE_NOTREACHED (return false;)
  } // end ELSE

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("added user \"%s\" (uid: %u) to group \"%s\" (gid: %u)...\n"),
              ACE_TEXT (username_string.c_str ()), user_id,
              ACE_TEXT (Common_Tools::groupIdToString (groupId_in).c_str ()), groupId_in));

  return result;
}

bool
Common_OS_Tools::isGroupMember (uid_t userId_in,
                                gid_t groupId_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_OS_Tools::isGroupMember"));

  uid_t user_id =
      ((static_cast<int>(userId_in) == -1) ? ACE_OS::geteuid () : userId_in);
  std::string username_string, realname_string;
  Common_Tools::getUserName (user_id,
                             username_string,
                             realname_string);
  char buffer_a[BUFSIZ];
  ACE_OS::memset (buffer_a, 0, sizeof (char[BUFSIZ]));
  struct passwd passwd_s;
  struct passwd* passwd_p = NULL;
  int result = -1;
  // step1: check whether the group happens to be the users' 'primary' group
  //        (the user would not appear in the member list of the 'group'
  //        database in this case)
  result = ::getpwuid_r (user_id,               // user id
                         &passwd_s,             // passwd entry
                         buffer_a,              // buffer
                         sizeof (char[BUFSIZ]), // buffer size
                         &passwd_p);            // result (handle)
  if (unlikely ((result == -1) || !passwd_p))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ::getpwuid_r(%u): \"%m\", aborting\n"),
                user_id));
    return false; // *TODO*: avoid false negatives
  } // end IF
  if (passwd_s.pw_gid == groupId_in)
    return true;

  // step2: check the 'secondary' member list of the 'group' database
  ACE_OS::memset (buffer_a, 0, sizeof (char[BUFSIZ]));
  struct group group_s;
  struct group* group_p = NULL;
  result = ::getgrgid_r (groupId_in,
                         &group_s,
                         buffer_a,
                         sizeof (char[BUFSIZ]),
                         &group_p);
  if (unlikely ((result == -1) || !group_p))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ::getgrgid_r(%u): \"%m\", aborting\n"),
                groupId_in));
    return false; // *TODO*: avoid false negatives
  } // end IF
  ACE_ASSERT (group_s.gr_mem);
//  if (!*group_s.gr_mem) // groupId_in is a 'primary group', i.e. a group with
//                        // only members that have it set as primary group in
//                        // the corresponding 'passwd' file entry (see: man
//                        // getpwnam(3))
//    return false;
  for (char** string_pp = group_s.gr_mem;
       *string_pp;
       ++string_pp)
    if (!ACE_OS::strcmp (username_string.c_str (),
                         *string_pp))
      return true;

  return false;
}

std::string
Common_OS_Tools::groupIdToString (gid_t groupId_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_OS_Tools::groupIdToString"));

  // initialize return value(s)
  std::string return_value;

  struct group group_s, *group_p;
  ACE_OS::memset (&group_s, 0, sizeof (struct group));
  char buffer_a[BUFSIZ];
  int result = -1;

  setgrent ();
  do
  {
    group_p = NULL;
    result = ::getgrent_r (&group_s,
                           buffer_a,
                           sizeof (char[BUFSIZ]),
                           &group_p);
    if (unlikely (result || !group_p))
    {
      if (result != ENOENT)
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ::getgrent_r(): \"%m\", returning\n")));
      break;
    } // end IF

    if (group_p->gr_gid == groupId_in)
    {
      return_value = group_p->gr_name;
      break;
    } // end IF
  } while (true);
  ::endgrent ();

  return return_value;
}

gid_t
Common_OS_Tools::stringToGroupId (const std::string& groupName_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_OS_Tools::stringToGroupId"));

  // initialize return value(s)
  gid_t return_value = 0;

  struct group group_s, *group_p;
  ACE_OS::memset (&group_s, 0, sizeof (struct group));
  char buffer_a[BUFSIZ];
  int result = -1;

  setgrent ();
  do
  {
    group_p = NULL;
    result = ::getgrent_r (&group_s,
                           buffer_a,
                           sizeof (char[BUFSIZ]),
                           &group_p);
    if (unlikely (result || !group_p))
    {
      if (result != ENOENT)
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ::getgrent_r(): \"%m\", returning\n")));
      break;
    } // end IF

    if (!ACE_OS::strcmp (groupName_in.c_str (),
                         group_p->gr_name))
    {
      return_value = group_p->gr_gid;
      break;
    } // end IF
  } while (true);
  ::endgrent ();

  return return_value;
}

void
Common_OS_Tools::printUserIds ()
{
  COMMON_TRACE (ACE_TEXT ("Common_OS_Tools::printUserIds"));

  uid_t user_id = ACE_OS::getuid ();
  uid_t effective_user_id = ACE_OS::geteuid ();
  gid_t user_group = ACE_OS::getgid ();
  gid_t effective_user_group = ACE_OS::getegid ();

  ACE_DEBUG ((LM_INFO,
              ACE_TEXT ("%P:%t: real/effective user id/group: %u/%u\t%u/%u\n"),
              user_id, effective_user_id,
              user_group, effective_user_group));
}

std::string
Common_OS_Tools::environment (const std::string& variable_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_OS_Tools::environment"));

  std::string result;

  // sanity check(s)
  ACE_ASSERT (!variable_in.empty ());

  char* string_p = ACE_OS::getenv (variable_in.c_str ());
  if (string_p)
    result.assign (string_p);
  else
    ACE_DEBUG ((LM_WARNING,
                ACE_TEXT ("\"%s\" environment variable not set, returning\n"),
                ACE_TEXT (variable_in.c_str ())));

  return result;
}

bool
Common_OS_Tools::setResourceLimits (bool fileDescriptors_in,
                                    bool stackTraces_in,
                                    bool pendingSignals_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_OS_Tools::setResourceLimits"));

  int result = -1;
  rlimit resource_limit;

  if (fileDescriptors_in)
  {
    result = ACE_OS::getrlimit (RLIMIT_NOFILE,
                                &resource_limit);
    if (unlikely (result == -1))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_OS::getrlimit(RLIMIT_NOFILE): \"%m\", aborting\n")));
      return false;
    } // end IF

    //      ACE_DEBUG ((LM_DEBUG,
    //                  ACE_TEXT ("file descriptor limits (before) [soft: \"%u\", hard: \"%u\"]...\n"),
    //                  resource_limit.rlim_cur,
    //                  resource_limit.rlim_max));

    // *TODO*: really unset these limits; note that this probably requires
    // patching/recompiling the kernel...
    // *NOTE*: setting/raising the max limit probably requires CAP_SYS_RESOURCE
    resource_limit.rlim_cur = resource_limit.rlim_max;
    //      resource_limit.rlim_cur = RLIM_INFINITY;
    //      resource_limit.rlim_max = RLIM_INFINITY;
    result = ACE_OS::setrlimit (RLIMIT_NOFILE,
                                &resource_limit);
    if (unlikely (result == -1))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_OS::setrlimit(RLIMIT_NOFILE): \"%m\", aborting\n")));
      return false;
    } // end IF

    // verify...
    result = ACE_OS::getrlimit (RLIMIT_NOFILE,
                                &resource_limit);
    if (unlikely (result == -1))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_OS::getrlimit(RLIMIT_NOFILE): \"%m\", aborting\n")));
      return false;
    } // end IF
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("unset file descriptor limits, now: [soft: %u, hard: %u]\n"),
                resource_limit.rlim_cur,
                resource_limit.rlim_max));
  } // end IF

// -----------------------------------------------------------------------------

  if (stackTraces_in)
  {
    //  result = ACE_OS::getrlimit (RLIMIT_CORE,
    //                              &resource_limit);
    //  if (result == -1)
    //  {
    //    ACE_DEBUG ((LM_ERROR,
    //                ACE_TEXT ("failed to ACE_OS::getrlimit(RLIMIT_CORE): \"%m\", aborting\n")));
    //    return false;
    //  } // end IF
    //   ACE_DEBUG ((LM_DEBUG,
    //               ACE_TEXT ("corefile limits (before) [soft: \"%u\", hard: \"%u\"]...\n"),
    //               core_limit.rlim_cur,
    //               core_limit.rlim_max));

    // set soft/hard limits to unlimited...
    resource_limit.rlim_cur = RLIM_INFINITY;
    resource_limit.rlim_max = RLIM_INFINITY;
    result = ACE_OS::setrlimit (RLIMIT_CORE,
                                &resource_limit);
    if (unlikely (result == -1))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_OS::setrlimit(RLIMIT_CORE): \"%m\", aborting\n")));
      return false;
    } // end IF

    // verify...
    result = ACE_OS::getrlimit (RLIMIT_CORE,
                                &resource_limit);
    if (unlikely (result == -1))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_OS::getrlimit(RLIMIT_CORE): \"%m\", aborting\n")));
      return false;
    } // end IF
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("unset corefile limits, now: [soft: %u, hard: %u]\n"),
                resource_limit.rlim_cur,
                resource_limit.rlim_max));
  } // end IF

  if (pendingSignals_in)
  {
    //  result = ACE_OS::getrlimit (RLIMIT_SIGPENDING,
    //                              &resource_limit);
    //  if (result == -1)
    //  {
    //    ACE_DEBUG ((LM_ERROR,
    //                ACE_TEXT ("failed to ACE_OS::getrlimit(RLIMIT_SIGPENDING): \"%m\", aborting\n")));
    //    return false;
    //  } // end IF

    //   ACE_DEBUG ((LM_DEBUG,
    //               ACE_TEXT ("pending signals limit (before) [soft: \"%u\", hard: \"%u\"]...\n"),
    //               signal_pending_limit.rlim_cur,
    //               signal_pending_limit.rlim_max));

    // set soft/hard limits to unlimited...
    // *NOTE*: setting/raising the max limit probably requires CAP_SYS_RESOURCE
//    resource_limit.rlim_cur = RLIM_INFINITY;
//    resource_limit.rlim_max = RLIM_INFINITY;
//    resource_limit.rlim_cur = resource_limit.rlim_max;
//    result = ACE_OS::setrlimit (RLIMIT_SIGPENDING,
//                                &resource_limit);
//    if (result == -1)
//    {
//      ACE_DEBUG ((LM_ERROR,
//                  ACE_TEXT ("failed to ACE_OS::setrlimit(RLIMIT_SIGPENDING): \"%m\", aborting\n")));
//      return false;
//    } // end IF

    // verify...
// *NOTE*: Solaris (11)-specific
#if defined (__sun) && defined (__SVR4)
    rctlblk_t* block_p =
      static_cast<rctlblk_t*> (ACE_CALLOC_FUNC (1, rctlblk_size ()));
    if (unlikely (!block_p))
    {
      ACE_DEBUG ((LM_CRITICAL,
                  ACE_TEXT ("failed to allocate memory: \"%m\", aborting\n")));
      return false;
    } // end IF
    result = ::getrctl (ACE_TEXT_ALWAYS_CHAR ("process.max-sigqueue-size"),
                        NULL, block_p,
                        RCTL_USAGE);
    if (result == 0)
    {
      resource_limit.rlim_cur = rctlblk_get_value (block_p);
      resource_limit.rlim_max = rctlblk_get_value (block_p);
    } // end IF
    ACE_FREE_FUNC (block_p);
#else
    result = ACE_OS::getrlimit (RLIMIT_SIGPENDING,
                                &resource_limit);
#endif // __sun && __SVR4
    if (unlikely (result == -1))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_OS::getrlimit(RLIMIT_SIGPENDING): \"%m\", aborting\n")));
      return false;
    } // end IF
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("unset pending signal limits, now: [soft: %u, hard: %u]\n"),
                resource_limit.rlim_cur,
                resource_limit.rlim_max));
  } // end IF

  return true;
}

void
Common_OS_Tools::getUserName (uid_t userId_in,
                              std::string& username_out,
                              std::string& realname_out)
{
  COMMON_TRACE (ACE_TEXT ("Common_OS_Tools::getUserName"));

  // initialize return value(s)
  username_out.clear ();
  realname_out.clear ();

  int            result = -1;
  struct passwd  passwd_s;
  struct passwd* passwd_p = NULL;
  char           buffer_a[BUFSIZ];
  ACE_OS::memset (buffer_a, 0, sizeof (char[BUFSIZ]));
//  size_t         bufsize = 0;
//  bufsize = sysconf (_SC_GETPW_R_SIZE_MAX);
//  if (bufsize == -1)        /* Value was indeterminate */
//    bufsize = 16384;        /* Should be more than enough */

  uid_t user_id =
      ((static_cast<int>(userId_in) == -1) ? ACE_OS::geteuid () : userId_in);
  result = ::getpwuid_r (user_id,               // user id
                         &passwd_s,             // passwd entry
                         buffer_a,              // buffer
                         sizeof (char[BUFSIZ]), // buffer size
                         &passwd_p);            // result (handle)
  if (unlikely (!passwd_p))
  {
    if (result == 0)
      ACE_DEBUG ((LM_WARNING,
                  ACE_TEXT ("user \"%u\" not found, %s\n"),
                  user_id,
                  ((user_id == ACE_OS::geteuid ()) ? ACE_TEXT_ALWAYS_CHAR ("falling back") : ACE_TEXT_ALWAYS_CHAR ("returning"))));
    else
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_OS::getpwuid_r(%u): \"%m\", %s\n"),
                  ((user_id == ACE_OS::geteuid ()) ? ACE_TEXT_ALWAYS_CHAR ("falling back") : ACE_TEXT_ALWAYS_CHAR ("returning"))));
    if (user_id == ACE_OS::geteuid ())
      username_out =
          ACE_TEXT_ALWAYS_CHAR (ACE_OS::getenv (ACE_TEXT (COMMON_ENVIRONMENT_USER_LOGIN_BASE)));
  } // end IF
  else
  {
    username_out = passwd_s.pw_name;
    if (ACE_OS::strlen (passwd_s.pw_gecos))
    {
      realname_out = passwd_s.pw_gecos;
      std::string::size_type position = realname_out.find (',');
      if (position != std::string::npos)
        realname_out.substr (0, position);
    } // end IF
  } // end ELSE
}

std::string
Common_OS_Tools::getUserName ()
{
  COMMON_TRACE (ACE_TEXT ("Common_OS_Tools::getUserName"));

  std::string return_value;

  ACE_TCHAR buffer_a[L_cuserid];
  ACE_OS::memset (buffer_a, 0, sizeof (ACE_TCHAR[L_cuserid]));
  char* result_p = ACE_OS::cuserid (buffer_a);
  if (unlikely (!result_p))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::cuserid() : \"%m\", returning\n")));
    return ACE_TEXT_ALWAYS_CHAR ("");
  } // end IF
  return_value = buffer_a;

  return return_value;
}

bool
Common_OS_Tools::isInstalled (const std::string& executableName_in,
                              std::string& executablePath_out)
{
  COMMON_TRACE (ACE_TEXT ("Common_OS_Tools::isInstalled"));

  // initialize return value(s)
  executablePath_out.clear ();
  bool result = false;

  // sanity check(s)
  ACE_ASSERT (!executableName_in.empty ());

#if defined (ACE_LINUX)
  // sanity check(s)
  unsigned int major_i = 0, minor_i = 0, micro_i = 0;
  enum Common_OperatingSystemDistributionType linux_distribution_e =
      Common_Tools::getDistribution (major_i, minor_i, micro_i);
  ACE_UNUSED_ARG (major_i); ACE_UNUSED_ARG (minor_i); ACE_UNUSED_ARG (micro_i);
  if (unlikely (linux_distribution_e == COMMON_OPERATINGSYSTEM_DISTRIBUTION_INVALID))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_Tools::getDistribution(), aborting\n")));
    return result; // *TODO*: avoid false negatives
  } // end IF

  std::string command_line_string;
  std::string command_output_string;
  switch (linux_distribution_e)
  {
    case COMMON_OPERATINGSYSTEM_DISTRIBUTION_LINUX_DEBIAN:
    case COMMON_OPERATINGSYSTEM_DISTRIBUTION_LINUX_REDHAT:
    case COMMON_OPERATINGSYSTEM_DISTRIBUTION_LINUX_SUSE:
    case COMMON_OPERATINGSYSTEM_DISTRIBUTION_LINUX_UBUNTU:
    {
      // sanity check(s)
      ACE_ASSERT (Common_Tools::findProgram (ACE_TEXT_ALWAYS_CHAR (COMMON_COMMAND_FIND)));
      ACE_ASSERT (Common_Tools::findProgram (ACE_TEXT_ALWAYS_CHAR (COMMON_COMMAND_LOCATE)));
      ACE_ASSERT (Common_Tools::findProgram (ACE_TEXT_ALWAYS_CHAR (COMMON_COMMAND_XARGS)));
      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unsupported linux distribution (was: %d), aborting\n"),
                  linux_distribution_e));
      return result; // *TODO*: avoid false negatives
    }
  } // end SWITCH

  command_line_string = ACE_TEXT_ALWAYS_CHAR (COMMON_COMMAND_LOCATE);
  command_line_string += ACE_TEXT_ALWAYS_CHAR (" -b '\\");
  command_line_string +=
      ACE_TEXT_ALWAYS_CHAR (ACE::basename (ACE_TEXT (executableName_in.c_str ()),
                                           ACE_DIRECTORY_SEPARATOR_CHAR));
  command_line_string += ACE_TEXT_ALWAYS_CHAR ("' -e | ");
  command_line_string += ACE_TEXT_ALWAYS_CHAR (COMMON_COMMAND_XARGS);
  command_line_string += ACE_TEXT_ALWAYS_CHAR (" -ri ");
  command_line_string += ACE_TEXT_ALWAYS_CHAR (COMMON_COMMAND_FIND);
  command_line_string += ACE_TEXT_ALWAYS_CHAR (" {} -prune -type f -executable");
  int exit_status_i = 0;
  if (unlikely (!Common_Process_Tools::command (command_line_string,
                                                exit_status_i,
                                                command_output_string)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_Tools::command(\"%s\"), aborting\n"),
                ACE_TEXT (command_line_string.c_str ())));
    return result; // *TODO*: avoid false negatives
  } // end IF
  if (command_output_string.empty())
    return result;

  std::istringstream converter;
  converter.str (command_output_string);
  char buffer_a [BUFSIZ];
  do
  {
    converter.getline (buffer_a, sizeof (char[BUFSIZ]));
    if (converter.eof ())
      break; // done
    if (executablePath_out.empty ())
      executablePath_out = buffer_a;
#if defined (_DEBUG)
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("found executable (was: \"%s\"): \"%s\"\n"),
                ACE_TEXT (executableName_in.c_str ()),
                ACE_TEXT (buffer_a)));
#endif // _DEBUG
  } while (true);

  result = !executablePath_out.empty ();
#else
  ACE_ASSERT (false);
  ACE_NOTSUP_RETURN (result);
  ACE_NOTREACHED (return result;)
#endif // ACE_LINUX

  return result;
}

bool
Common_OS_Tools::findProgram (const std::string& executableName_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_OS_Tools::findProgram"));

  std::string command_line_string;
  command_line_string = ACE_TEXT_ALWAYS_CHAR (COMMON_COMMAND_WHICH);
  command_line_string += ACE_TEXT_ALWAYS_CHAR (" ");
  command_line_string += executableName_in;
  int exit_status_i = 0;
  std::string command_output_string;
  if (unlikely (!Common_Process_Tools::command (command_line_string,
                                                exit_status_i,
                                                command_output_string)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_Tools::command(\"%s\"), aborting\n"),
                ACE_TEXT (command_line_string.c_str ())));
    return false; // *TODO*: avoid false negatives
  } // end IF
  if (unlikely (command_output_string.empty ()))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to 'which' \"%s\", aborting\n"),
                ACE_TEXT (executableName_in.c_str ())));
    return false; // *TODO*: avoid false negatives
  } // end IF
  command_line_string = Common_String_Tools::strip (command_output_string);
  ACE_ASSERT (Common_File_Tools::isExecutable (command_line_string));

  return true;
}
