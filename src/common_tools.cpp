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

#include "common_tools.h"

#if defined (ACE_LINUX)
#include <regex>
#endif // ACE_LINUX

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include "Security.h"
#if COMMON_OS_WIN32_TARGET_PLATFORM (0x0602) // _WIN32_WINNT_WIN8
#include "processthreadsapi.h"
#endif // COMMON_OS_WIN32_TARGET_PLATFORM(0x0602)
#elif defined (ACE_LINUX)
#include "sys/capability.h"
#include "sys/prctl.h"
#include "sys/utsname.h"

#include "linux/capability.h"
#include "linux/prctl.h"
#include "linux/securebits.h"

#include "grp.h"
#elif defined (__sun) && defined (__SVR4)
// *NOTE*: Solaris (11)-specific
#include "rctl.h"
#endif // ACE_WIN32 || ACE_WIN64

#include "ace/config.h"
#include "ace/Log_Msg.h"
#include "ace/OS.h"
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

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
#include "common_signal_tools.h"
#endif // ACE_WIN32 || ACE_WIN64

#include "common_time_common.h"

// initialize statics
#if defined (ACE_WIN32) || defined (ACE_WIN64)
bool Common_Tools::COMInitialized = false;
#endif // ACE_WIN32 || ACE_WIN64
COMMON_APPLICATION_RNG_ENGINE Common_Tools::randomEngine;
unsigned int Common_Tools::randomSeed = 0;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
char Common_Tools::randomStateBuffer[BUFSIZ];
#endif // ACE_WIN32 || ACE_WIN64

//////////////////////////////////////////

void
#if defined (ACE_WIN32) || defined (ACE_WIN64)
Common_Tools::initialize (bool initializeCOM_in,
                          bool initializeRandomNumberGenerator_in)
#else
Common_Tools::initialize (bool initializeRandomNumberGenerator_in)
#endif // ACE_WIN32 || ACE_WIN64
{
  COMMON_TRACE (ACE_TEXT ("Common_Tools::initialize"));

#if defined (VALGRIND_USE)
  if (RUNNING_ON_VALGRIND)
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("running under valgrind\n")));
#endif // VALGRIND_USE

  Common_Error_Tools::initialize ();

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  if (initializeCOM_in &&
      !Common_Tools::initializeCOM ())
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_Tools::initializeCOM(): \"%s\", returning\n")));
    return;
  } // end IF
#endif // ACE_WIN32 || ACE_WIN64

  //ACE_DEBUG ((LM_DEBUG,
  //            ACE_TEXT ("calibrating high-resolution timer...\n")));
  //ACE_High_Res_Timer::calibrate (500000, 10);
  //ACE_DEBUG ((LM_DEBUG,
  //            ACE_TEXT ("calibrating high-resolution timer...done\n")));

  if (unlikely (initializeRandomNumberGenerator_in))
  {
    // *TODO*: use randomSeed to seed std::random_device ?
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("initializing C++-style random seed\n")));
    std::random_device prng_device;
    //PRNG_SEED_ARRAY_T seed_a;
    //COMMON_APPLICATION_RNG_ENGINE::result_type seed_a[COMMON_APPLICATION_RNG_ENGINE::state_size];
    COMMON_APPLICATION_RNG_ENGINE::result_type seed_a[COMMON_APPLICATION_RNG_ENGINE_DEFAULT_STATE_SIZE];
    std::chrono::time_point<std::chrono::system_clock> time_point;
    //for (PRNG_SEED_ARRAY_ITERATOR_T iterator = seed_a.cbegin ();
    //     iterator != seed_a.cend ();
    for (COMMON_APPLICATION_RNG_ENGINE::result_type* iterator = std::begin (seed_a);
         iterator != std::end (seed_a);
         ++iterator)
    {
      // read from std::random_device
      *iterator = prng_device ();

      // mix with a C++ equivalent of time(NULL) - UNIX time in seconds
      time_point = std::chrono::system_clock::now ();
      *iterator ^=
        std::chrono::duration_cast<std::chrono::seconds> (time_point.time_since_epoch ()).count ();

      // mix with a high precision time in microseconds
      *iterator ^=
        std::chrono::duration_cast<std::chrono::microseconds> (time_point.time_since_epoch ()).count ();

      //*iterator ^= more_external_random_stuff;
    } // end FOR
    std::seed_seq seed_sequence (std::begin (seed_a), std::end (seed_a));
    //Common_Tools::randomEngine.seed (prng_device);
    Common_Tools::randomEngine.seed (seed_sequence);

    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("initializing C-style random seed (RAND_MAX: %d)\n"),
                RAND_MAX));
    Common_Tools::randomSeed = COMMON_TIME_NOW.usec ();
    // *PORTABILITY*: outside glibc, this is not very portable
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    ACE_OS::srand (static_cast<u_int> (randomSeed));
#else
    ACE_OS::memset (&Common_Tools::randomStateBuffer, 0, sizeof (char[BUFSIZ]));
    struct random_data random_data_s;
    ACE_OS::memset (&random_data_s, 0, sizeof (struct random_data));
    int result = ::initstate_r (Common_Tools::randomSeed,
                                Common_Tools::randomStateBuffer, sizeof (char[BUFSIZ]),
                                &random_data_s);
    if (unlikely (result == -1))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to initstate_r(): \"%s\", returning\n")));
      return;
    } // end IF
    result = ::srandom_r (Common_Tools::randomSeed, &random_data_s);
    if (unlikely (result == -1))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to initialize random seed: \"%s\", returning\n")));
      return;
    } // end IF
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("initializing random seed...DONE\n")));
#endif // ACE_WIN32 || ACE_WIN64
  } // end IF
}

void
Common_Tools::finalize ()
{
  COMMON_TRACE (ACE_TEXT ("Common_Tools::finalize"));

  Common_Error_Tools::finalize ();

#if defined(ACE_WIN32) || defined(ACE_WIN64)
  if (Common_Tools::COMInitialized) // *NOTE*: should be thread-specific
  {
    Common_Tools::finalizeCOM ();
    Common_Tools::COMInitialized = false;
  } // end IF
#endif // ACE_WIN32 || ACE_WIN64
}

unsigned int
Common_Tools::getNumberOfCPUs (bool logicalProcessors_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Tools::getNumberOfCPUs"));

  unsigned int result = 1;

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  if (logicalProcessors_in)
  {
    struct _SYSTEM_INFO system_info_s;
    GetSystemInfo (&system_info_s);
    result = system_info_s.dwNumberOfProcessors;
  } // end IF
  else
  {
#if COMMON_OS_WIN32_TARGET_PLATFORM(0x0601) // _WIN32_WINNT_WIN7
    DWORD size = 0;
    DWORD error = 0;
    BYTE* byte_p = NULL;
    struct _SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX* processor_information_p =
      NULL;
    GetLogicalProcessorInformationEx (RelationProcessorCore,
                                      NULL,
                                      &size);
    error = GetLastError ();
    if (unlikely (error != ERROR_INSUFFICIENT_BUFFER))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to GetLogicalProcessorInformationEx(): \"%s\", returning\n"),
                  ACE_TEXT (Common_Error_Tools::errorToString (error).c_str ())));
      return 1;
    } // end IF
    ACE_NEW_NORETURN (byte_p,
                      BYTE[size]);
    if (unlikely (!byte_p))
    {
      ACE_DEBUG ((LM_CRITICAL,
                  ACE_TEXT ("failed to allocate memory: \"%m\", returning\n")));
      return 1;
    } // end IF
    processor_information_p =
      reinterpret_cast<struct _SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX*> (byte_p);
    if (unlikely (!GetLogicalProcessorInformationEx (RelationProcessorCore,
                                                     processor_information_p,
                                                     &size)))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to GetLogicalProcessorInformationEx(): \"%s\", returning\n"),
                  ACE_TEXT (Common_Error_Tools::errorToString (GetLastError ()).c_str ())));
      delete [] byte_p; byte_p = NULL;
      return 1;
    } // end IF
    for (DWORD offset = 0;
         offset < (size / sizeof (struct _SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX));
         ++offset, ++processor_information_p)
    { ACE_ASSERT (processor_information_p);
      switch (processor_information_p->Relationship)
      {
        case RelationProcessorCore:
          ++result; break;
        default:
          break;
      } // end SWITCH
    } // end FOR
    delete [] byte_p; byte_p = NULL;
#else
    ACE_ASSERT (false);
    ACE_NOTSUP_RETURN (1);
    ACE_NOTREACHED (return 1;)
#endif // COMMON_OS_WIN32_TARGET_PLATFORM(0x0601)
  } // end ELSE
#else
  ACE_UNUSED_ARG (logicalProcessors_in);
  long result_2 = ACE_OS::sysconf (_SC_NPROCESSORS_ONLN);
  if (unlikely (result_2 == -1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::sysconf(_SC_NPROCESSORS_ONLN): \"%m\", returning\n")));
    return 1;
  } // end IF
  result = static_cast<unsigned int> (result_2);
#endif // ACE_WIN32 || ACE_WIN64

  return result;
}

std::string
Common_Tools::getPlatformName ()
{
  COMMON_TRACE (ACE_TEXT ("Common_Tools::getPlatformName"));

  std::string return_value;

  // get system information
  int result = -1;
  ACE_utsname utsname_s;
  result = ACE_OS::uname (&utsname_s);
  if (unlikely (result == -1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::uname(): \"%m\", aborting\n")));
    return return_value;
  } // end IF
#if defined (_DEBUG)
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("'uname' output: %s (%s), %s %s, %s\n"),
              ACE_TEXT (utsname_s.nodename),
              ACE_TEXT (utsname_s.machine),
              ACE_TEXT (utsname_s.sysname),
              ACE_TEXT (utsname_s.release),
              ACE_TEXT (utsname_s.version)));
#endif // _DEBUG
  return_value += utsname_s.sysname;
  return_value += ACE_TEXT_ALWAYS_CHAR (" ");
  return_value += utsname_s.release;
  return_value += ACE_TEXT_ALWAYS_CHAR (" ");
  return_value += utsname_s.version;
  return_value += ACE_TEXT_ALWAYS_CHAR (" on ");
  return_value += utsname_s.machine;

  return return_value;
}

enum Common_OperatingSystemType
Common_Tools::getOperatingSystem ()
{
  COMMON_TRACE (ACE_TEXT ("Common_Tools::getOperatingSystem"));

  // get system information
  int result_2 = -1;
  ACE_utsname utsname_s;
  result_2 = ACE_OS::uname (&utsname_s);
  if (unlikely (result_2 == -1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::uname(): \"%m\", aborting\n")));
    return COMMON_OPERATINGSYSTEM_INVALID;
  } // end IF
//#if defined (_DEBUG)
//  ACE_DEBUG ((LM_DEBUG,
//              ACE_TEXT ("'uname' output: %s (%s), %s %s, %s\n"),
//              ACE_TEXT (utsname_s.nodename),
//              ACE_TEXT (utsname_s.machine),
//              ACE_TEXT (utsname_s.sysname),
//              ACE_TEXT (utsname_s.release),
//              ACE_TEXT (utsname_s.version)));
//#endif // _DEBUG

  std::string sysname_string (utsname_s.sysname);
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  if (sysname_string.find (ACE_TEXT_ALWAYS_CHAR (COMMON_OS_WIN32_UNAME_STRING),
                           0) == 0)
    return COMMON_OPERATINGSYSTEM_WIN32;
#elif defined (ACE_LINUX)
  if (sysname_string.find (ACE_TEXT_ALWAYS_CHAR (COMMON_OS_LINUX_UNAME_STRING),
                           0) == 0)
    return COMMON_OPERATINGSYSTEM_GNU_LINUX;
#else
  ACE_ASSERT (false); // *TODO*
  ACE_NOTSUP_RETURN (COMMON_OPERATINGSYSTEM_INVALID);
  ACE_NOTREACHED (return COMMON_OPERATINGSYSTEM_INVALID;)
#endif // ACE_WIN32 || ACE_WIN64 || ACE_LINUX

  return COMMON_OPERATINGSYSTEM_INVALID;
}

std::string
Common_Tools::compilerName ()
{
  COMMON_TRACE (ACE_TEXT ("Common_Tools::compilerName"));

  // *TODO*: maintain proprietary information in a separate file and generate
  //         this method with a macro instead
#if defined (__BORLANDC__)
  return ACE_TEXT_ALWAYS_CHAR ("Embarcadero C++ Builder (TM)");
#elif defined (__GNUG__)
  return ACE_TEXT_ALWAYS_CHAR ("GNU g++");
#elif defined (_MSC_VER)
  return ACE_TEXT_ALWAYS_CHAR ("Microsoft VisualC++ (TM)");
#else
#error invalid/unknown C++ compiler; not supported, check implementation
#endif

  return ACE_TEXT_ALWAYS_CHAR ("");
}
std::string
Common_Tools::compilerPlatformName ()
{
  COMMON_TRACE (ACE_TEXT ("Common_Tools::compilerPlatformName"));

  // *TODO*: maintain proprietary information in a separate file and generate
  //         this method with a macro instead
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#if defined (ACE_WIN64)
  return ACE_TEXT_ALWAYS_CHAR ("x64");
#else
  return ACE_TEXT_ALWAYS_CHAR ("Win32 (x86)");
#endif // ACE_WIN64
#elif defined (ACE_LINUX)
  return ACE_TEXT_ALWAYS_CHAR ("Linux");
#else
#error invalid/unknown platform; not supported, check implementation
#endif // ACE_WIN32 || ACE_WIN64 || ACE_LINUX

  return ACE_TEXT_ALWAYS_CHAR ("");
}
std::string
Common_Tools::compilerVersion ()
{
  COMMON_TRACE (ACE_TEXT ("Common_Tools::compilerVersion"));

  std::ostringstream converter;
  converter << ACE::compiler_major_version ();
  converter << ACE_TEXT_ALWAYS_CHAR (".");
  converter << ACE::compiler_minor_version ();
  converter << ACE_TEXT_ALWAYS_CHAR (".");
  converter << ACE::compiler_beta_version ();

  return converter.str ();
}

#if defined (ACE_LINUX)
enum Common_OperatingSystemDistributionType
Common_Tools::getDistribution (unsigned int& majorVersion_out,
                               unsigned int& minorVersion_out,
                               unsigned int& microVersion_out)
{
  COMMON_TRACE (ACE_TEXT ("Common_Tools::getDistribution"));

  // initialize return value(s)
  enum Common_OperatingSystemDistributionType result =
      COMMON_OPERATINGSYSTEM_DISTRIBUTION_INVALID;
  majorVersion_out = 0;
  minorVersion_out = 0;
  microVersion_out = 0;

  // sanity check(s)
  if (unlikely (!Common_Tools::is (COMMON_OPERATINGSYSTEM_GNU_LINUX)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("this is not a (GNU-) Linux system, aborting\n")));
    return result;
  } // end IF

  // step1: retrieve distributor id
  std::string command_line_string =
      ACE_TEXT_ALWAYS_CHAR (COMMON_COMMAND_LSB_RELEASE);
  COMMON_COMMAND_ADD_SWITCH (command_line_string,COMMON_COMMAND_SWITCH_LSB_RELEASE_DISTRIBUTOR)
  int exit_status_i = 0;
  std::string command_output_string, distribution_id_string, release_string;
  std::string buffer_string;
  std::istringstream converter;
  char buffer_a [BUFSIZ];
  std::regex regex (ACE_TEXT_ALWAYS_CHAR ("^(Distributor ID:\t)(.+)$"));
  std::smatch match_results;
  if (unlikely (!Common_Process_Tools::command (command_line_string,
                                                exit_status_i,
                                                command_output_string)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_Tools::command(\"%s\"), aborting\n"),
                ACE_TEXT (command_line_string.c_str ())));
    return result;
  } // end IF
  ACE_ASSERT (!command_output_string.empty ());
  converter.str (command_output_string);
  do
  {
    converter.getline (buffer_a, sizeof (char[BUFSIZ]));
    buffer_string = buffer_a;
    if (!std::regex_match (buffer_string,
                           match_results,
                           regex,
                           std::regex_constants::match_default))
      continue;
    ACE_ASSERT (match_results.ready () && !match_results.empty ());
    ACE_ASSERT (match_results[2].matched);

    distribution_id_string = match_results[2];
    break;
  } while (!converter.fail ());
  if (unlikely (distribution_id_string.empty ()))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to retrieve (GNU-) Linux distributor id (command output was: \"%s\"), aborting\n"),
                ACE_TEXT (command_output_string.c_str ())));
    return result;
  } // end IF

  if (!ACE_OS::strcmp (distribution_id_string.c_str (),
                       ACE_TEXT_ALWAYS_CHAR (COMMON_OS_LSB_DEBIAN_STRING)))
    result = COMMON_OPERATINGSYSTEM_DISTRIBUTION_LINUX_DEBIAN;
  else if (!ACE_OS::strcmp (distribution_id_string.c_str (),
                            ACE_TEXT_ALWAYS_CHAR (COMMON_OS_LSB_OPENSUSE_STRING)))
    result = COMMON_OPERATINGSYSTEM_DISTRIBUTION_LINUX_SUSE;
  else if (!ACE_OS::strcmp (distribution_id_string.c_str (),
                            ACE_TEXT_ALWAYS_CHAR (COMMON_OS_LSB_FEDORA_STRING)))
    result = COMMON_OPERATINGSYSTEM_DISTRIBUTION_LINUX_REDHAT;
  else if (!ACE_OS::strcmp (distribution_id_string.c_str (),
                            ACE_TEXT_ALWAYS_CHAR (COMMON_OS_LSB_UBUNTU_STRING)))
    result = COMMON_OPERATINGSYSTEM_DISTRIBUTION_LINUX_UBUNTU;
  else
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to determine Linux distribution (command output was: \"%s\"), aborting\n"),
                ACE_TEXT (command_output_string.c_str ())));
    return result;
  } // end ELSE

  // step2: retrieve release version
  command_line_string =
      ACE_TEXT_ALWAYS_CHAR (COMMON_COMMAND_LSB_RELEASE);
  COMMON_COMMAND_ADD_SWITCH (command_line_string,COMMON_COMMAND_SWITCH_LSB_RELEASE_RELEASE)
  std::regex regex_2 (ACE_TEXT_ALWAYS_CHAR ("^(Release:\t)(.+)$"));
  if (unlikely (!Common_Process_Tools::command (command_line_string,
                                                exit_status_i,
                                                command_output_string)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_Tools::command(\"%s\"), aborting\n"),
                ACE_TEXT (command_line_string.c_str ())));
    return result;
  } // end IF
  ACE_ASSERT (!command_output_string.empty ());
  converter.str (command_output_string);
  do
  {
    converter.getline (buffer_a, sizeof (char[BUFSIZ]));
    buffer_string = buffer_a;
    if (!std::regex_match (buffer_string,
                           match_results,
                           regex_2,
                           std::regex_constants::match_default))
      continue;
    ACE_ASSERT (match_results.ready () && !match_results.empty ());
    ACE_ASSERT (match_results[2].matched);

    release_string = match_results[2];
    break;
  } while (!converter.fail ());
  if (unlikely (release_string.empty ()))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to retrieve (GNU-) Linux release version (command output was: \"%s\"), aborting\n"),
                ACE_TEXT (command_output_string.c_str ())));
    return result;
  } // end IF
  converter.str (release_string);
  converter >> majorVersion_out;
  converter >> minorVersion_out;
  converter >> microVersion_out;

  return result;
}
#endif // ACE_LINUX

std::string
Common_Tools::getHostName ()
{
  COMMON_TRACE (ACE_TEXT ("Common_Tools::getHostName"));

  // initialize return value(s)
  std::string return_value;

  int result = -1;
  ACE_TCHAR host_name[MAXHOSTNAMELEN + 1];
  ACE_OS::memset (host_name, 0, sizeof (host_name));
  result = ACE_OS::hostname (host_name, sizeof (host_name));
  if (unlikely (result == -1))
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::hostname(): \"%m\", aborting\n")));
  else
    return_value = ACE_TEXT_ALWAYS_CHAR (host_name);

  return return_value;
}

#if defined (ACE_WIN32) || defined (ACE_WIN64)
bool
Common_Tools::initializeCOM ()
{
  COMMON_TRACE (ACE_TEXT ("Common_Tools::initializeCOM"));

  HRESULT result =
    CoInitializeEx (NULL,
                    COMMON_WIN32_COM_INITIALIZATION_DEFAULT_FLAGS);
  if (FAILED (result)) // RPC_E_CHANGED_MODE : 0x80010106L
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to CoInitializeEx(NULL,0x%x): \"%s\", aborting\n"),
                COMMON_WIN32_COM_INITIALIZATION_DEFAULT_FLAGS,
                ACE_TEXT (Common_Error_Tools::errorToString (result).c_str ())));
    return false;
  } // end IF
  Common_Tools::COMInitialized = true;

  return true;
}
#endif // ACE_WIN32 || ACE_WIN64

bool
Common_Tools::testRandomProbability (float probability_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Tools::::testRandomProbability"));

  // sanity checks
  ACE_ASSERT ((probability_in >= 0.0F) && (probability_in <= 1.0F));

  // step0: fast path ?
  if (probability_in == 0.0F)
    return false;
  else if (probability_in == 1.0F)
    return true;

  //float limit_f = probability_in * static_cast<float> (RAND_MAX);

  //return (ACE_OS::rand_r (&Common_Tools::randomSeed) <= static_cast<int> (limit_f));

  static std::uniform_real_distribution<float> distribution (0.0F, 1.0F);
  float result_d = Common_Tools::getRandomNumber (distribution);

  return result_d <= probability_in;
}
