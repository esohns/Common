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
  if (argc_in < 2)
  {
    printf ("invalid number of arguments (was: %d), aborting\n",
            argc_in - 1);
    return 1;
  } // end IF

  uid_t ruid, euid, suid = 0;
//   uid_t rgid, egid, sgid = 0;

  if (getresuid (&ruid, &euid, &suid))
  {
    printf ("failed to getresuid(): \"%s\", aborting\n",
            strerror (errno));
    return 1; /* Fatal error, probably not setuid root */
  } // end IF

  cap_t  caps;
  cap_value_t need_caps[2];
  std::stringstream converter;
  converter.str (argv_in[1]);
  int capability_i;
  converter >> capability_i;
#if defined (_DEBUG)
  char* string_p = NULL;
#endif

  std::string command_line_string;
  if (argc_in == 2)
  {
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
    command_line_string = argv_in[2];

  /* enable capability ? */
  caps = cap_get_proc ();
  cap_flag_value_t in_effective_set = CAP_CLEAR;
  if (cap_get_flag (caps, CAP_SETUID, CAP_EFFECTIVE, &in_effective_set))
  {
    printf ("failed to cap_get_flag(CAP_SETUID,CAP_EFFECTIVE): \"%s\", aborting\n",
            strerror (errno));
    return 1; /* Fatal error */
  } // end IF
  if (in_effective_set != CAP_SET)
  {
    need_caps[0] = CAP_SETUID;
    need_caps[1] = CAP_SETGID;
    if (cap_set_flag (caps, CAP_EFFECTIVE,   2, need_caps, CAP_SET))
    {
      printf ("failed to cap_set_flag(CAP_EFFECTIVE,CAP_SETUID|CAP_SETGID): \"%s\", aborting\n",
              strerror (errno));
      return 1; /* Fatal error */
    } // end IF
    if (cap_get_flag (caps, CAP_SETUID, CAP_EFFECTIVE, &in_effective_set))
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
    in_effective_set = CAP_CLEAR;
    if (cap_get_flag (caps, CAP_SETGID, CAP_EFFECTIVE, &in_effective_set))
    {
      printf ("failed to cap_get_flag(CAP_SETGID,CAP_EFFECTIVE): \"%s\", aborting\n",
              strerror (errno));
      return 1; /* Fatal error */
    } // end IF
    if (in_effective_set != CAP_SET)
    {
      printf ("failed to enable CAP_SETGID in 'effective' set, aborting\n");
      return 1; /* Fatal error */
    } // end IF
    printf ("enabled CAP_SETGID in 'effective' set\n");

    // debug info
// #if defined (_DEBUG)
//     string_p = cap_to_text (caps, NULL);
//     if (!string_p)
//     {
//       printf ("failed to cap_to_text(): \"%s\", aborting\n",
//               strerror (errno));
//       return 1; /* Fatal error */
//     } // end IF
//     printf ("%s\n", string_p);
//     cap_free (string_p);
//     string_p = NULL;
// #endif
  } // end IF

//   printf ("ruid: %u, euid: %u, suid: %u\n",
//           ruid, euid, suid);
//   if (getresgid (&rgid, &egid, &sgid))
//   {
//     printf ("failed to getresgid(): \"%s\", aborting\n",
//             strerror (errno));
//     return 1; /* Fatal error, probably not setuid root */
//   } // end IF
//   printf ("rgid: %u, egid: %u, sgid: %u\n",
//           rgid, egid, sgid);

  /* Elevate privileges */
  if (setresuid (0, 0, 0))
  {
    printf ("failed to setresuid(0,0,0): \"%s\", aborting\n",
            strerror (errno));
    return 1; /* Fatal error, probably not setuid root */
  } // end IF

  /* Add need_caps to current capabilities. */
  need_caps[0] = capability_i;
  if (cap_set_flag (caps, CAP_PERMITTED,   1, need_caps, CAP_SET) ||
      cap_set_flag (caps, CAP_EFFECTIVE,   1, need_caps, CAP_SET) ||
      cap_set_flag (caps, CAP_INHERITABLE, 1, need_caps, CAP_SET))
    return 1; /* Fatal error */

  /* Update capabilities */
  if (cap_set_proc (caps))
    return 1; /* Fatal error */

  /* Retain capabilities over an identity change */
  if (prctl (PR_SET_KEEPCAPS, 1L))
    return 1; /* Fatal error */

  /* Return to original, real-user identity */
  if (setresuid (ruid, ruid, ruid))
    return 1; /* Fatal error */
//   printf ("ruid: %u, euid: %u, suid: %u\n",
//           getuid (), geteuid (), suid);

  /* Because the identity changed, re-install the effective set. */
  if (cap_set_proc (caps))
    return 1; /* Fatal error */

  // debug info
#if defined (_DEBUG)
  string_p = cap_to_text (caps, NULL);
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
#endif

  /* Capability set is no longer needed. */
  cap_free (caps);

  /* The process now has the given capability.
   * It will be retained over fork() and exec().
  */
  if (execv (command_line_string.c_str (),
             ((argc_in == 2) ? argv_in + 1 : argv_in + 2)))
    return 1; /* Fatal error */

  return 0;
}
