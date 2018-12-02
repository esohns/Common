// *NOTE*: see also: https://stackoverflow.com/questions/17743062/changing-user-ids-for-assigning-additional-capabilities

#include <sys/types.h>
#include <sys/capability.h>
#include <sys/prctl.h>
#include <sys/types.h>
#include <pwd.h>
#include <string.h>
#include <unistd.h>

#include <sstream>
#include <string>

int
main (int argc_in,
      char** argv_in)
{
  // sanity check(s)
  unsigned int last_capability_argument_index_i = 1;
  if (argc_in < 2)
  {
    printf ("invalid number of arguments (was: %d), aborting\n",
            argc_in - 1);
    return 1;
  } // end ELSE IF

  uid_t ruid, euid, suid = 0;
  if (getresuid (&ruid, &euid, &suid))
  {
    printf ("failed to getresuid(): \"%s\", aborting\n",
            strerror (errno));
    return 1; /* Fatal error, probably not setuid root */
  } // end IF
//  printf ("ruid: %u, euid: %u, suid: %u\n",
//          ruid, euid, suid);
  uid_t rgid, egid, sgid = 0;
  if (getresgid (&rgid, &egid, &sgid))
  {
   printf ("failed to getresgid(): \"%s\", aborting\n",
           strerror (errno));
   return 1; /* Fatal error, probably not setuid root */
  } // end IF
//  printf ("rgid: %u, egid: %u, sgid: %u\n",
//         rgid, egid, sgid);

  cap_t caps_p = NULL;
  cap_value_t caps_a[BUFSIZ];
  std::stringstream converter;
  int capability_i, index_i = 0;
  for (unsigned int i = 1;
       i < static_cast<unsigned int> (argc_in);
       ++i)
  {
    capability_i = 0;
    converter.str (ACE_TEXT_ALWAYS_CHAR (""));
    converter.clear ();
    converter.str (argv_in[i]);
    converter >> capability_i;
    if (!capability_i)
      break; // done
    caps_a[index_i++] = capability_i;
    ++last_capability_argument_index_i;
  } // end FOR

  std::string command_line_string;
  if (last_capability_argument_index_i == static_cast<unsigned int> (argc_in))
  { // no command --> run shell
    struct passwd* passwd_p = getpwuid (ruid);
    if (!passwd_p)
    {
      printf ("failed to getpwuid(%u): \"%s\", aborting\n",
              ruid,
              strerror (errno));
      return 1; /* Fatal error */
    } // end IF
    command_line_string = passwd_p->pw_shell;
  } // end IF
  else
    command_line_string = argv_in[last_capability_argument_index_i];

  cap_value_t caps_a_2[2];
  /* enable capability ? */
  caps_p = cap_get_proc ();
//  ACE_ASSERT (caps_p);
  cap_flag_value_t in_effective_set = CAP_CLEAR;
  if (cap_get_flag (caps_p, CAP_SETUID, CAP_EFFECTIVE, &in_effective_set))
  {
    printf ("failed to cap_get_flag(CAP_SETUID,CAP_EFFECTIVE): \"%s\", aborting\n",
            strerror (errno));
    return 1; /* Fatal error */
  } // end IF
  if (in_effective_set != CAP_SET)
  {
    caps_a_2[0] = CAP_SETUID;
    if (cap_set_flag (caps_p, CAP_EFFECTIVE, 1, caps_a_2, CAP_SET))
    {
      printf ("failed to cap_set_flag(CAP_EFFECTIVE,CAP_SETUID): \"%s\", aborting\n",
              strerror (errno));
      return 1; /* Fatal error */
    } // end IF
    if (cap_get_flag (caps_p, CAP_SETUID, CAP_EFFECTIVE, &in_effective_set))
    {
      printf ("failed to cap_get_flag(CAP_SETUID,CAP_EFFECTIVE): \"%s\", aborting\n",
              strerror (errno));
      return 1; /* Fatal error */
    } // end IF
    if (in_effective_set != CAP_SET)
    {
      printf ("failed to enable CAP_SETUID in 'effective' set, aborting\n");
      return 1; /* Fatal error */
    } // end IF
    printf ("enabled CAP_SETUID in 'effective' set\n");
  } // end IF

  in_effective_set = CAP_CLEAR;
  if (cap_get_flag (caps_p, CAP_SETGID, CAP_EFFECTIVE, &in_effective_set))
  {
    printf ("failed to cap_get_flag(CAP_SETGID,CAP_EFFECTIVE): \"%s\", aborting\n",
            strerror (errno));
    return 1; /* Fatal error */
  } // end IF
  if (in_effective_set != CAP_SET)
  {
    caps_a_2[0] = CAP_SETGID;
    if (cap_set_flag (caps_p, CAP_EFFECTIVE, 1, caps_a_2, CAP_SET))
    {
      printf ("failed to cap_set_flag(CAP_EFFECTIVE,CAP_SETGID): \"%s\", aborting\n",
              strerror (errno));
      return 1; /* Fatal error */
    } // end IF
    if (cap_get_flag (caps_p, CAP_SETUID, CAP_EFFECTIVE, &in_effective_set))
    {
      printf ("failed to cap_get_flag(CAP_SETUID,CAP_EFFECTIVE): \"%s\", aborting\n",
              strerror (errno));
      return 1; /* Fatal error */
    } // end IF
    if (in_effective_set != CAP_SET)
    {
      printf ("failed to enable CAP_SETGID in 'effective' set, aborting\n");
      return 1; /* Fatal error */
    } // end IF
    printf ("enabled CAP_SETGID in 'effective' set\n");
  } // end IF
#if defined (_DEBUG)
  char* string_p = NULL;
//  string_p = cap_to_text (caps_p, NULL);
//  if (!string_p)
//  {
//    printf ("failed to cap_to_text(): \"%s\", aborting\n",
//            strerror (errno));
//    return 1; /* Fatal error */
//  } // end IF
//  printf ("%s\n", string_p);
//  cap_free (string_p);
//  string_p = NULL;
#endif // _DEBUG

  if (cap_clear (caps_p))
    return 1; /* Fatal error */
  caps_a_2[0] = CAP_SETUID;
  if (cap_set_flag (caps_p, CAP_PERMITTED, 1, caps_a_2, CAP_SET) ||
      cap_set_flag (caps_p, CAP_EFFECTIVE, 1, caps_a_2, CAP_SET))
  {
    printf ("failed to cap_set_flag(CAP_PERMITTED|CAP_EFFECTIVE,CAP_SETUID): \"%s\", aborting\n",
            strerror (errno));
    return 1; /* Fatal error */
  } // end IF
  /* set specified capabilities */
  if (cap_set_flag (caps_p, CAP_PERMITTED,   index_i, caps_a, CAP_SET) ||
      cap_set_flag (caps_p, CAP_EFFECTIVE,   index_i, caps_a, CAP_SET) ||
      cap_set_flag (caps_p, CAP_INHERITABLE, index_i, caps_a, CAP_SET))
    return 1; /* Fatal error */

  /* Elevate privileges */
  if (setresuid (0, 0, 0))
  {
    printf ("failed to setresuid(0,0,0): \"%s\", aborting\n",
            strerror (errno));
    return 1; /* Fatal error, probably not setuid root */
  } // end IF
//  if (getresuid (&ruid, &euid, &suid))
//  {
//    printf ("failed to getresuid(): \"%s\", aborting\n",
//            strerror (errno));
//    return 1; /* Fatal error, probably not setuid root */
//  } // end IF
  printf ("setuid root...\n");

  /* Update capabilities */
  if (cap_set_proc (caps_p))
  {
    printf ("failed to cap_set_proc(): \"%s\", aborting\n",
            strerror (errno));
    return 1; /* Fatal error */
  } // end IF

  /* Retain capabilities over an identity change */
  if (prctl (PR_SET_KEEPCAPS, 1L))
    return 1; /* Fatal error */

  /* Return to original, real-user identity */
//  if (setresuid (ruid, euid, suid))
  if (setresuid (ruid, ruid, ruid))
  {
    printf ("failed to setresuid(%u,%u,%u): \"%s\", aborting\n",
//            ruid, euid, suid,
            ruid, ruid, ruid,
            strerror (errno));
    return 1; /* Fatal error */
  } // end IF
  printf ("reset uids: ruid: %u, euid: %u, suid: %u\n",
//          ruid, euid, suid);
          ruid, ruid, ruid);
//  if (setresgid (rgid, egid, sgid))
  if (setresgid (rgid, rgid, rgid))
  {
    printf ("failed to setresgid(%u,%u,%u): \"%s\", aborting\n",
//            rgid, egid, sgid,
            rgid, rgid, rgid,
            strerror (errno));
    return 1; /* Fatal error */
  } // end IF
  printf ("reset gids: rgid: %u, egid: %u, sgid: %u\n",
//          rgid, egid, sgid);
          rgid, rgid, rgid);

  // *NOTE*: apparently, setuid(0) clears a kernel security flag that allows
  //         core dumps (, and thus attaching a debugger after switching users)
  //         (see: man prctl(2): "...Processes that are not dumpable can
  //         not be attached via ptrace(2) PTRACE_ATTACH...")
  //         (see also: /proc/sys/kernel/yama/ptrace_scope)
  //         (see also: /proc/sys/fs/suid_dumpable)
  //         (see also: 'CoreDumping' setting in /proc/PPID/status)
  //         --> reset the dumpable flag
  if (prctl (PR_SET_DUMPABLE, 1L)) // 1: SUID_DUMP_USER
    return 1; /* Fatal error */
  printf ("reset dumpable flag\n");

  /* Because the identity changed, re-install the effective set. */
  if (cap_set_proc (caps_p))
    return 1; /* Fatal error */

  // debug info
#if defined (_DEBUG)
  string_p = cap_to_text (caps_p, NULL);
  if (!string_p)
  {
    printf ("failed to cap_to_text(): \"%s\", aborting\n",
            strerror (errno));
    return 1; /* Fatal error */
  } // end IF
  printf ("%d %s\n",
          getpid (),
          string_p);
  cap_free (string_p);
#endif // _DEBUG

  /* Capability set is no longer needed. */
  cap_free (caps_p);

  /* The process now has the given capabilities;
   * these will be retained over fork() and exec() */
//  printf ("command line: \"%s\"\n",
//          command_line_string.c_str ());
  std::string command_line_arguments_string;
  char* argv_a[BUFSIZ];
  index_i = 1;
  for (unsigned int i = last_capability_argument_index_i + 1;
       i < static_cast<unsigned int> (argc_in);
       ++i)
  {
//    printf ("command line arguments: [%u]; \"%s\"\n",
//            index_i, argv_in[i]);
    argv_a[index_i++] = argv_in[i];
  } // end FOR
//      command_line_arguments_string += argv_in[i];
//        (std::string (argv_in[i]) + ACE_TEXT_ALWAYS_CHAR (" "));
//  printf ("command line arguments: \"%s\"\n",
//          command_line_arguments_string.c_str ());
  if (execv (command_line_string.c_str (),
             argv_a))
//             argv_in + (sizeof (char*) * (last_capability_argument_index_i + 1))))
    return 1; /* Fatal error */

  return 0;
}
