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

  uid_t  real = getuid ();

  cap_t  caps;
  cap_value_t need_caps[1];
  std::stringstream converter;
  converter.str (argv_in[1]);
  int capability_i;
  converter >> capability_i;
  need_caps[0] = capability_i;
#if defined (_DEBUG)
  char* string_p = NULL;
#endif

  std::string command_line_string;
  if (argc_in == 2)
  {
    struct passwd* passwd_p = getpwuid (real);
    if (!passwd_p)
    {
      printf ("failed to getpwuid(%u): \"%s\", aborting\n",
              real,
              strerror (errno));
      return 1; /* Fatal error */
    } // end IF
    command_line_string = passwd_p->pw_shell;
  } // end IF
  else
    command_line_string = argv_in[2];

  /* Elevate privileges */
  if (setresuid (0, 0, 0))
  {
    printf ("failed to setresuid(0,0,0): \"%s\", aborting\n",
            strerror (errno));
    return 1; /* Fatal error, probably not setuid root */
  } // end IF

  /* Add need_caps to current capabilities. */
  caps = cap_get_proc ();
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
  if (setresuid (real, real, real))
    return 1; /* Fatal error */

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
  printf ("%s\n", string_p);
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
