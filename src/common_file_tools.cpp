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

#include "common_file_tools.h"

#include <regex>

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include "Shlobj.h"
#include "Userenv.h"
#else
#include "sys/stat.h"
#endif // ACE_WIN32 || ACE_WIN64

#include "ace/ACE.h"
#include "ace/Dirent_Selector.h"
#include "ace/FILE_Addr.h"
#include "ace/FILE_Connector.h"
#include "ace/FILE_IO.h"
#include "ace/OS.h"
#include "ace/OS_NS_sys_sendfile.h"

#if defined (HAVE_CONFIG_H)
#include "Common_config.h"
#endif // HAVE_CONFIG_H

#include "common_defines.h"
#include "common_macros.h"
#include "common_os_tools.h"
#include "common_string_tools.h"

#include "common_error_tools.h"

// initialize statics
std::string Common_File_Tools::executable = ACE_TEXT_ALWAYS_CHAR ("");
std::string Common_File_Tools::executableBase = ACE_TEXT_ALWAYS_CHAR ("");

void
Common_File_Tools::initialize (const std::string& argv0_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_File_Tools::initialize"));

  // sanity check(s)
  ACE_ASSERT (Common_File_Tools::isValidFilename (argv0_in));

  Common_File_Tools::executable =
  ACE_TEXT_ALWAYS_CHAR (ACE::basename (argv0_in.c_str (),
                                       ACE_DIRECTORY_SEPARATOR_CHAR_A));

  Common_File_Tools::executableBase =
    ACE_TEXT_ALWAYS_CHAR (ACE::dirname (argv0_in.c_str(),
                                        ACE_DIRECTORY_SEPARATOR_CHAR_A));
  ACE_ASSERT (Common_File_Tools::isDirectory (Common_File_Tools::executableBase));
}

std::string
Common_File_Tools::addressToString (const ACE_FILE_Addr& address_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_File_Tools::addressToString"));

  std::string result;

  ACE_TCHAR buffer_a[PATH_MAX];
  int result_2 = address_in.addr_to_string (buffer_a,
                                            sizeof (ACE_TCHAR[PATH_MAX]));
  if (unlikely (result_2 == -1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_FILE_Addr::addr_to_string(): \"%m\", aborting\n")));
    return result;
  } // end IF
  result = ACE_TEXT_ALWAYS_CHAR (buffer_a);

  return result;
}

std::string
Common_File_Tools::cropExtension (const std::string& filename_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_File_Tools::fileExtension"));

  std::string return_value;

  std::string::size_type position =
    filename_in.rfind ('.', std::string::npos);
  if (position != std::string::npos)
  {
    return_value = filename_in;
    return_value.erase (position);
  } // end IF

  return return_value;
}

std::string
Common_File_Tools::fileExtension (const std::string& path_in,
                                  bool returnLeadingDot_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_File_Tools::fileExtension"));

  std::string return_value;

  std::string::size_type position =
      path_in.rfind ('.', std::string::npos);
  if (position != std::string::npos)
    return_value =
        path_in.substr ((returnLeadingDot_in ? position : position + 1),
                        std::string::npos);

  return return_value;
}

#if defined (ACE_WIN32) || defined (ACE_WIN64)
std::string
Common_File_Tools::escape (const std::string& path_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_File_Tools::escape"));

  // initialize return value(s)
  std::string return_value = path_in;

  std::string::size_type position = 0;
  while ((position = return_value.find ('\\', position)) != std::string::npos)
  {
    return_value.replace (position, 1, ACE_TEXT_ALWAYS_CHAR ("\\\\"));
    position += 2;
  } // end WHILE

  return return_value;
}
#endif // ACE_WIN32 || ACE_WIN64

bool
Common_File_Tools::exists (const std::string& path_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_File_Tools::exists"));

  int result = -1;
  ACE_stat stat_s;
  ACE_OS::memset (&stat_s, 0, sizeof (ACE_stat));
  result = ACE_OS::stat (ACE_TEXT (path_in.c_str ()),
                         &stat_s);
  if (unlikely (result == -1))
  {
    int error = ACE_OS::last_error ();
    if (error != ENOENT) // 2: not found
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_OS::stat(\"%s\"): \"%m\", aborting\n"),
                  ACE_TEXT (path_in.c_str ())));
    return false;
  } // end IF

  return true;
}

bool
Common_File_Tools::access (const std::string& path_in,
#if defined (ACE_WIN32) || defined (ACE_WIN64)
                           DWORD accessRights_in,
                           PSID SID_in)
#else
                           ACE_UINT32 mask_in)
#endif // ACE_WIN32 || ACE_WIN64
{
  COMMON_TRACE (ACE_TEXT ("Common_File_Tools::access"));

  bool result = false;

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  SECURITY_INFORMATION security_information_i = (OWNER_SECURITY_INFORMATION |
                                                 GROUP_SECURITY_INFORMATION |
                                                 DACL_SECURITY_INFORMATION);
  PSECURITY_DESCRIPTOR security_descriptor_p = NULL;
  DWORD length_needed_i = 0;
  DWORD desired_access_i = (TOKEN_IMPERSONATE |
                            TOKEN_QUERY       |
                            TOKEN_DUPLICATE   |
                            STANDARD_RIGHTS_READ);
  HANDLE token_h = ACE_INVALID_HANDLE, token_2 = ACE_INVALID_HANDLE;
  struct _GENERIC_MAPPING generic_mapping_s;
  struct _PRIVILEGE_SET privilege_set_s;
  DWORD granted_access_i = 0, length_i = sizeof (struct _PRIVILEGE_SET);
  BOOL result_2 = FALSE;

  if (unlikely (!::GetFileSecurity (path_in.c_str (),
                                    security_information_i,
                                    security_descriptor_p,
                                    0,
                                    &length_needed_i)))
  {
    DWORD error_i = ::GetLastError ();
    if (unlikely (error_i != ERROR_INSUFFICIENT_BUFFER))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to GetFileSecurity(\"%s\"): \"%s\", aborting\n"),
                  ACE_TEXT (path_in.c_str ()),
                  ACE_TEXT (Common_Error_Tools::errorToString (error_i, false).c_str ())));
      return false; // *TODO*: avoid false negatives
    } // end IF
  } // end IF
  ACE_ASSERT (length_needed_i);
  security_descriptor_p = ::LocalAlloc (LMEM_FIXED | LMEM_ZEROINIT,
                                        length_needed_i);
  if (unlikely (!security_descriptor_p))
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to LocalAlloc(%d): \"%s\", aborting\n"),
                length_needed_i,
                ACE_TEXT (Common_Error_Tools::errorToString (::GetLastError (), false).c_str ())));
    return false; // *TODO*: avoid false negatives
  } // end IF
  if (unlikely (!::GetFileSecurity (path_in.c_str (),
                                    security_information_i,
                                    security_descriptor_p,
                                    length_needed_i,
                                    &length_needed_i)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to GetFileSecurity(\"%s\"): \"%s\", aborting\n"),
                ACE_TEXT (path_in.c_str ()),
                ACE_TEXT (Common_Error_Tools::errorToString (::GetLastError (), false).c_str ())));
    goto clean;
  } // end IF

  if (unlikely (!::OpenProcessToken (::GetCurrentProcess (),
                                     desired_access_i,
                                     &token_h)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to OpenProcessToken(%d): \"%s\", aborting\n"),
                desired_access_i,
                ACE_TEXT (Common_Error_Tools::errorToString (::GetLastError (), false).c_str ())));
    goto clean;
  } // end IF
  ACE_ASSERT (token_h != ACE_INVALID_HANDLE);
  if (unlikely (SID_in))
  {
    ACE_ASSERT (::IsValidSid (SID_in));
    struct _TOKEN_USER token_user_s;
    ACE_OS::memset (&token_user_s, 0, sizeof (struct _TOKEN_USER));
    token_user_s.User.Sid = SID_in;
    if (unlikely (!::SetTokenInformation (token_h,
                                          TokenUser,
                                          &token_user_s,
                                          sizeof (struct _TOKEN_USER))))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to SetTokenInformation(%@): \"%s\", aborting\n"),
                  token_h,
                  ACE_TEXT (Common_Error_Tools::errorToString (::GetLastError (), false).c_str ())));
      goto clean;
    } // end IF
  } // end IF
  if (unlikely (!::DuplicateToken (token_h,
                                   SecurityImpersonation,
                                   &token_2)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to DuplicateToken(%@,%d): \"%s\", aborting\n"),
                token_h,
                SecurityImpersonation,
                ACE_TEXT (Common_Error_Tools::errorToString (::GetLastError (), false).c_str ())));
    goto clean;
  } // end IF
  ACE_ASSERT (token_2 != ACE_INVALID_HANDLE);
  ACE_OS::memset (&generic_mapping_s, 0, sizeof (struct _GENERIC_MAPPING));
  generic_mapping_s.GenericRead = FILE_GENERIC_READ;
  generic_mapping_s.GenericWrite = FILE_GENERIC_WRITE;
  generic_mapping_s.GenericExecute = FILE_GENERIC_EXECUTE;
  generic_mapping_s.GenericAll = FILE_ALL_ACCESS;
  ::MapGenericMask (&accessRights_in, &generic_mapping_s);
  ACE_OS::memset (&privilege_set_s, 0, sizeof (struct _PRIVILEGE_SET));
  if (unlikely (!::AccessCheck (security_descriptor_p,
                                token_2,
                                accessRights_in,
                                &generic_mapping_s,
                                &privilege_set_s,
                                &length_i,
                                &granted_access_i,
                                &result_2)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to AccessCheck(\"%s\"): \"%s\", aborting\n"),
                ACE_TEXT (path_in.c_str ()),
                ACE_TEXT (Common_Error_Tools::errorToString (::GetLastError (), false).c_str ())));
    goto clean;
  } // end IF
  result = (result_2 == TRUE);

clean:
  if (likely (token_2 != ACE_INVALID_HANDLE))
    if (unlikely (!CloseHandle (token_2)))
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to CloseHandle(%@): \"%s\", continuing\n"),
                  token_2,
                  ACE_TEXT (Common_Error_Tools::errorToString (::GetLastError (), false).c_str ())));
  if (likely (token_h != ACE_INVALID_HANDLE))
    if (unlikely (!CloseHandle (token_h)))
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to CloseHandle(%@): \"%s\", continuing\n"),
                  token_h,
                  ACE_TEXT (Common_Error_Tools::errorToString (::GetLastError (), false).c_str ())));
  if (likely (security_descriptor_p))
  {
    if (unlikely (LocalFree (security_descriptor_p)))
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to LocalFree(%@): \"%s\", continuing\n"),
                  security_descriptor_p,
                  ACE_TEXT (Common_Error_Tools::errorToString (::GetLastError (), false).c_str ())));
  } // end IF
#else
  ACE_UINT32 mask_i = (mask_in & ACCESSPERMS);
  // sanity check(s)
  if (unlikely (!mask_i))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("invalid argument (expected: 0x%x, was: 0x%x), aborting\n"),
                ACCESSPERMS,
                mask_in));
    return false; // *TODO*: avoid false negatives
  } // end IF

  int result_2 = -1;
  ACE_stat stat_s;
  ACE_OS::memset (&stat_s, 0, sizeof (ACE_stat));
  result_2 = ACE_OS::stat (ACE_TEXT (path_in.c_str ()),
                           &stat_s);
  if (unlikely (result_2 == -1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::stat(\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (path_in.c_str ())));
    return false; // *TODO*: avoid false negatives
  } // end IF
  result = stat_s.st_mode & mask_i;
#endif // ACE_WIN32 || ACE_WIN64

  return result;
}
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
bool
Common_File_Tools::protection (const std::string& path_in,
                               ACE_UINT32 mask_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_File_Tools::protection"));

  ACE_UINT32 mask_i = (mask_in & ALLPERMS);
  // sanity check(s)
  if (unlikely (!mask_i))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("invalid argument (expected: 0x%x, was: 0x%x), aborting\n"),
                ALLPERMS,
                mask_in));
    return false; // *TODO*: avoid false negatives
  } // end IF

  int result = -1;
  ACE_stat stat_s;
  ACE_OS::memset (&stat_s, 0, sizeof (ACE_stat));
  result = ACE_OS::stat (ACE_TEXT (path_in.c_str ()),
                         &stat_s);
  if (unlikely (result == -1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::stat(\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (path_in.c_str ())));
    return false; // *TODO*: avoid false negatives
  } // end IF

  return (stat_s.st_mode & mask_i);
}
#endif // ACE_WIN32 || ACE_WIN64
bool
Common_File_Tools::type (const std::string& path_in,
                         ACE_UINT32 mask_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_File_Tools::type"));

  ACE_UINT32 mask_i = (mask_in & S_IFMT);
  // sanity check(s)
  if (unlikely (!mask_i))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("invalid argument (expected: 0x%x, was: 0x%x), aborting\n"),
                S_IFMT,
                mask_in));
    return false; // *TODO*: avoid false negatives
  } // end IF

  int result = -1;
  ACE_stat stat_s;
  ACE_OS::memset (&stat_s, 0, sizeof (ACE_stat));
  result = ACE_OS::stat (ACE_TEXT (path_in.c_str ()),
                         &stat_s);
  if (unlikely (result == -1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::stat(\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (path_in.c_str ())));
    return false; // *TODO*: avoid false negatives
  } // end IF

  return (stat_s.st_mode & mask_i);
}

bool
Common_File_Tools::isReadable (const std::string& path_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_File_Tools::isReadable"));

  ACE_UINT32 mask_i = S_IFDIR | S_IFREG | S_IFLNK;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
  ACE_UINT32 mask_2 = S_IRUSR|S_IRGRP|S_IROTH;
#endif // ACE_WIN32 || ACE_WIN64

  return (Common_File_Tools::exists (path_in)       &&  // has a file system entry
          Common_File_Tools::type (path_in, mask_i) &&  // is a regular file system entry
#if defined (ACE_WIN32) || defined (ACE_WIN64)
          Common_File_Tools::access (path_in, GENERIC_READ));
#else
          Common_File_Tools::access (path_in, mask_2)); // any of owner,group,other has/have read access
#endif // ACE_WIN32 || ACE_WIN64
}
bool
Common_File_Tools::isWriteable (const std::string& path_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_File_Tools::isWriteable"));

  ACE_UINT32 mask_i = S_IFDIR|S_IFREG|S_IFLNK;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
  ACE_UINT32 mask_2 = S_IWUSR|S_IWGRP|S_IWOTH;
#endif // ACE_WIN32 || ACE_WIN64

  return (Common_File_Tools::exists (path_in)       &&  // has a file system entry
          Common_File_Tools::type (path_in, mask_i) &&  // is a regular file system entry
#if defined (ACE_WIN32) || defined (ACE_WIN64)
          Common_File_Tools::access (path_in, GENERIC_WRITE));
#else
          Common_File_Tools::access (path_in, mask_2)); // any of owner,group,other has/have write access
#endif // ACE_WIN32 || ACE_WIN64
}
bool
Common_File_Tools::isExecutable (const std::string& path_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_File_Tools::isExecutable"));

  ACE_UINT32 mask_i = S_IFREG|S_IFLNK;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
  ACE_UINT32 mask_2 = S_IXUSR|S_IXGRP|S_IXOTH;
#endif // ACE_WIN32 || ACE_WIN64

  return (Common_File_Tools::exists (path_in)       &&  // has a file system entry
          Common_File_Tools::type (path_in, mask_i) &&  // is a regular file or link
#if defined (ACE_WIN32) || defined (ACE_WIN64)
          Common_File_Tools::access (path_in, GENERIC_EXECUTE));
#else
          Common_File_Tools::access (path_in, mask_2)); // any of owner,group,other has/have execute access
#endif // ACE_WIN32 || ACE_WIN64
}

bool
Common_File_Tools::canRead (const std::string& path_in,
#if defined (ACE_WIN32) || defined (ACE_WIN64)
                            const std::string& accountName_in)
#else
                            uid_t userId_in)
#endif // ACE_WIN32 || ACE_WIN64
{
  COMMON_TRACE (ACE_TEXT ("Common_File_Tools::canRead"));

  // sanity check(s)
  if (!Common_File_Tools::isReadable (path_in))
    return false;

  bool result = false;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  if (likely (accountName_in.empty ()))
    return true; // *NOTE*: Common_File_Tools::isReadable() verifies access
                 //         rights for the current process

  // step1: check whether accountName_in is the 'current' user
  DWORD desired_access_i = (TOKEN_IMPERSONATE   |
                            TOKEN_QUERY         |
                            TOKEN_DUPLICATE     |
                            STANDARD_RIGHTS_READ);
  HANDLE token_h = ACE_INVALID_HANDLE;
  if (unlikely (!::OpenProcessToken (::GetCurrentProcess (),
                                     desired_access_i,
                                     &token_h)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to OpenProcessToken(%d): \"%s\", aborting\n"),
                desired_access_i,
                ACE_TEXT (Common_Error_Tools::errorToString (::GetLastError (), false).c_str ())));
    return false; // *TODO*: avoid false negatives
  } // end IF
  ACE_ASSERT (token_h != ACE_INVALID_HANDLE);

  struct _TOKEN_USER* token_user_p = NULL;
  DWORD buffer_size_i = 0;
  struct _SID_IDENTIFIER_AUTHORITY sid_identifier_authority_s =
    SECURITY_LOCAL_SID_AUTHORITY;
  PSID SID_p = NULL;
  if (unlikely (!::GetTokenInformation (token_h,
                                        TokenUser,
                                        token_user_p,
                                        0,
                                        &buffer_size_i)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to GetTokenInformation(%@,%d): \"%s\", aborting\n"),
                token_h,
                TokenUser,
                ACE_TEXT (Common_Error_Tools::errorToString (::GetLastError (), false).c_str ())));
    goto clean; // *TODO*: avoid false negatives
  } // end IF
  ACE_ASSERT (buffer_size_i);
  token_user_p = (struct _TOKEN_USER*)HeapAlloc (GetProcessHeap (),
                                                 HEAP_ZERO_MEMORY,
                                                 buffer_size_i);
  if (unlikely (!token_user_p))
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to allocate memory (%d byte(s)), aborting\n"),
                buffer_size_i));
    goto clean; // *TODO*: avoid false negatives
  } // end IF
  if (unlikely (!::GetTokenInformation (token_h,
                                        TokenUser,
                                        token_user_p,
                                        buffer_size_i,
                                        &buffer_size_i)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to GetTokenInformation(%@,%d): \"%s\", aborting\n"),
                token_h,
                TokenUser,
                ACE_TEXT (Common_Error_Tools::errorToString (::GetLastError (), false).c_str ())));
    goto clean; // *TODO*: avoid false negatives
  } // end IF

  if (unlikely (!AllocateAndInitializeSid (&sid_identifier_authority_s,
                                           1,
                                           SECURITY_INTERACTIVE_RID,
                                           0, 0, 0, 0, 0, 0, 0,
                                           &SID_p)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to AllocateAndInitializeSid(SECURITY_LOCAL_SID_AUTHORITY,%d): \"%s\", aborting\n"),
                SECURITY_INTERACTIVE_RID,
                ACE_TEXT (Common_Error_Tools::errorToString (::GetLastError (), false).c_str ())));
    goto clean; // *TODO*: avoid false negatives
  } // end IF
  ACE_ASSERT (SID_p);
  ACE_ASSERT (::IsValidSid (SID_p));
  DWORD SID_size_i = ::GetLengthSid (SID_p);
  ACE_ASSERT (SID_size_i);
#if defined (UNICODE)
#if defined (ACE_USES_WCHAR)
  ACE_TCHAR referenced_domain_name_a[BUFSIZ];
#else
  ACE_ANTI_TCHAR referenced_domain_name_a[BUFSIZ];
#endif // ACE_USES_WCHAR
#else
#if defined (ACE_USES_WCHAR)
  ACE_ANTI_TCHAR referenced_domain_name_a[BUFSIZ];
#else
  ACE_TCHAR referenced_domain_name_a[BUFSIZ];
#endif // ACE_USES_WCHAR
#endif // UNICODE
  ACE_OS::memset (&referenced_domain_name_a, 0, sizeof (ACE_TCHAR[BUFSIZ]));
  DWORD referenced_domain_name_size_i = sizeof (ACE_TCHAR[BUFSIZ]);
  enum _SID_NAME_USE SID_name_use_e;
#if defined (UNICODE)
  if (unlikely (!::LookupAccountName (NULL, // --> begin on the local system
                                      ACE_TEXT_ALWAYS_WCHAR (accountName_in.c_str ()),
                                      SID_p,
                                      &SID_size_i,
                                      referenced_domain_name_a,
                                      &referenced_domain_name_size_i,
                                      &SID_name_use_e)))
#else
  if (unlikely (!::LookupAccountName (NULL, // --> begin on the local system
                                      ACE_TEXT_ALWAYS_CHAR (accountName_in.c_str ()),
                                      SID_p,
                                      &SID_size_i,
                                      referenced_domain_name_a,
                                      &referenced_domain_name_size_i,
                                      &SID_name_use_e)))
#endif // UNICODE
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to LookupAccountName(\"%s\"): \"%s\", aborting\n"),
                ACE_TEXT (accountName_in.c_str ()),
                ACE_TEXT (Common_Error_Tools::errorToString (::GetLastError (), false).c_str ())));
    goto clean; // *TODO*: avoid false negatives
  } // end IF
  ACE_ASSERT (SID_name_use_e == SidTypeUser);
  if (likely (EqualSid (token_user_p->User.Sid, SID_p)))
  {
    result = true;
    goto clean;
  } // end IF

  // --> accoutName_in is not the 'current' user
  result = Common_File_Tools::access (path_in,
                                      GENERIC_READ,
                                      SID_p);

clean:
  if (likely (token_h != ACE_INVALID_HANDLE))
    if (unlikely (!CloseHandle (token_h)))
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to CloseHandle(%@): \"%s\", continuing\n"),
                  token_h,
                  ACE_TEXT (Common_Error_Tools::errorToString (::GetLastError (), false).c_str ())));
  if (token_user_p)
    if (unlikely (HeapFree (GetProcessHeap (), 0, (LPVOID)token_user_p)))
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to HeapFree(%@,0,%@): \"%s\", continuing\n"),
                  GetProcessHeap (),
                  token_user_p,
                  ACE_TEXT (Common_Error_Tools::errorToString (::GetLastError (), false).c_str ())));
  if (SID_p)
    if (unlikely (FreeSid (SID_p)))
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to FreeSid(%@): \"%s\", continuing\n"),
                  SID_p,
                  ACE_TEXT (Common_Error_Tools::errorToString (::GetLastError (), false).c_str ())));
#else
  int result_2 = -1;
  uid_t user_id =
      ((static_cast<int>(userId_in) == -1) ? ACE_OS::geteuid () : userId_in);
  ACE_stat stat_s;
  ACE_OS::memset (&stat_s, 0, sizeof (ACE_stat));
  result_2 = ACE_OS::stat (ACE_TEXT (path_in.c_str ()),
                           &stat_s);
  if (unlikely (result_2 == -1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::stat(\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (path_in.c_str ())));
    return false; // *TODO*: avoid false negatives
  } // end IF

  if (stat_s.st_uid == user_id)
    return (stat_s.st_mode & S_IRUSR);
  if (Common_OS_Tools::isGroupMember (user_id, stat_s.st_gid))
    return (stat_s.st_mode & S_IRGRP);
  result = (stat_s.st_mode & S_IROTH);
#endif // ACE_WIN32 || ACE_WIN64

  return result;
}
bool
Common_File_Tools::canWrite (const std::string& path_in,
                             uid_t userId_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_File_Tools::canWrite"));

  // sanity check(s)
  if (!Common_File_Tools::isWriteable (path_in))
    return false;

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  ACE_ASSERT (false);
  ACE_NOTSUP_RETURN (false);

  ACE_NOTREACHED (return false;) // *TODO*: avoid false negatives
#else
  int result = -1;
  uid_t user_id =
      ((static_cast<int>(userId_in) == -1) ? ACE_OS::geteuid () : userId_in);
  ACE_stat stat_s;
  ACE_OS::memset (&stat_s, 0, sizeof (ACE_stat));
  result = ACE_OS::stat (ACE_TEXT (path_in.c_str ()),
                         &stat_s);
  if (unlikely (result == -1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::stat(\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (path_in.c_str ())));
    return false; // *TODO*: avoid false negatives
  } // end IF

  if (stat_s.st_uid == user_id)
    return (stat_s.st_mode & S_IWUSR);
  if (Common_OS_Tools::isGroupMember (user_id, stat_s.st_gid))
    return (stat_s.st_mode & S_IWGRP);
  return (stat_s.st_mode & S_IWOTH);
#endif // ACE_WIN32 || ACE_WIN64
}
bool
Common_File_Tools::canExecute (const std::string& path_in,
                               uid_t userId_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_File_Tools::canExecute"));

  // sanity check(s)
  if (!Common_File_Tools::isExecutable (path_in))
    return false;

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  ACE_ASSERT (false);
  ACE_NOTSUP_RETURN (false);

  ACE_NOTREACHED (return false;) // *TODO*: avoid false negatives
#else
  int result = -1;
  uid_t user_id =
      ((static_cast<int>(userId_in) == -1) ? ACE_OS::geteuid () : userId_in);
  ACE_stat stat_s;
  ACE_OS::memset (&stat_s, 0, sizeof (ACE_stat));
  result = ACE_OS::stat (ACE_TEXT (path_in.c_str ()),
                         &stat_s);
  if (unlikely (result == -1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::stat(\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (path_in.c_str ())));
    return false; // *TODO*: avoid false negatives
  } // end IF

  if (stat_s.st_uid == user_id)
    return (stat_s.st_mode & S_IXUSR);
  if (Common_OS_Tools::isGroupMember (user_id, stat_s.st_gid))
    return (stat_s.st_mode & S_IXGRP);
  return (stat_s.st_mode & S_IXOTH);
#endif // ACE_WIN32 || ACE_WIN64
}

bool
Common_File_Tools::isEmpty (const std::string& path_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_File_Tools::isEmpty"));

  int result = -1;
  ACE_stat stat;
  ACE_OS::memset (&stat, 0, sizeof(stat));
  result = ACE_OS::stat (path_in.c_str (),
                         &stat);
  if (unlikely (result == -1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::stat(\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (path_in.c_str ())));
    return false;
  } // end IF

  return (stat.st_size == 0);
}

bool
Common_File_Tools::isFile (const std::string& path_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_File_Tools::isFile"));

  int result = -1;
  ACE_stat stat;
  ACE_OS::memset (&stat, 0, sizeof (ACE_stat));
  result = ACE_OS::stat (path_in.c_str (),
                         &stat);
  if (unlikely (result == -1))
  {
    int error = ACE_OS::last_error ();
    if (error != ENOENT) // 2:
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_OS::stat(\"%s\"): \"%m\", aborting\n"),
                  ACE_TEXT (path_in.c_str ())));
    // URI doesn't even exist --> NOT a file !
    return false;
  } // end IF

  return ((stat.st_mode & S_IFMT) & S_IFREG);
}

bool
Common_File_Tools::isDirectory (const std::string& directory_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_File_Tools::isDirectory"));

//  // *NOTE*: ACE_OS::stat does not currently work in Win32; this may (!) be
//  //         related to i18n %APPDATA%/%PROGRAMFILES%... path quirkyness
//  // *TODO*: find a portable solution and integrate it into ACE
//#if defined (ACE_WIN32) || defined (ACE_WIN64)
//  DWORD attributes = GetFileAttributes (directory_in.c_str ());
//
//  return (attributes != INVALID_FILE_ATTRIBUTES &&
//         (attributes & FILE_ATTRIBUTE_DIRECTORY));
//#else
  int result = -1;
  ACE_stat stat;
  ACE_OS::memset (&stat, 0, sizeof (ACE_stat));
  result = ACE_OS::stat (directory_in.c_str (),
                         &stat);
  if (unlikely (result == -1))
  {
    int error = ACE_OS::last_error ();
    if (error != ENOENT) // 2:
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_OS::stat(\"%s\"): \"%m\", aborting\n"),
                  ACE_TEXT (directory_in.c_str ())));

    // URI doesn't even exist --> NOT a directory !
    return false;
  } // end IF

  return ((stat.st_mode & S_IFMT) & S_IFDIR);
//#endif
}

bool
Common_File_Tools::isLink (const std::string& path_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_File_Tools::isLink"));

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  ACE_ASSERT (false); // *TODO*
  ACE_NOTSUP_RETURN (false);
  ACE_NOTREACHED (return false;)
#else
  int result = -1;
  ACE_stat stat;
  ACE_OS::memset (&stat, 0, sizeof (ACE_stat));
  result = ACE_OS::lstat (path_in.c_str (),
                          &stat);
  if (unlikely (result == -1))
  {
    int error = ACE_OS::last_error ();
    if (error != ENOENT) // 2:
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_OS::lstat(\"%s\"): \"%m\", aborting\n"),
                  ACE_TEXT (path_in.c_str ())));

    // URI doesn't even exist --> NOT a directory !
    return false;
  } // end IF

  return ((stat.st_mode & S_IFMT) & S_IFLNK);
#endif // ACE_WIN32 || ACE_WIN64
}

bool
Common_File_Tools::isEmptyDirectory (const std::string& directory_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_File_Tools::isEmptyDirectory"));

  // initialize return value
  bool return_value = false;

  int result = -1;
  ACE_Dirent_Selector entries;
  result = entries.open (ACE_TEXT (directory_in.c_str ()),
                         NULL,
                         NULL);
  if (unlikely (result == -1))
  {
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("failed to ACE_Dirent_Selector::open(\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (directory_in.c_str ())));
    return false;
  } // end IF
  return_value = (entries.length () == 0);

  // clean up
  result = entries.close ();
  if (unlikely (result == -1))
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Dirent_Selector::close(\"%s\"): \"%m\", continuing\n"),
                ACE_TEXT (directory_in.c_str ())));

  return return_value;
}

bool
Common_File_Tools::isValidFilename (const std::string& string_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_File_Tools::isValidFilename"));

  std::string directory, file_name;
  directory =
    ACE_TEXT_ALWAYS_CHAR (ACE::dirname (string_in.c_str (),
                                        ACE_DIRECTORY_SEPARATOR_CHAR_A));
  file_name =
    ACE_TEXT_ALWAYS_CHAR (ACE::basename (string_in.c_str (),
                                         ACE_DIRECTORY_SEPARATOR_CHAR_A));

  // *TODO*: this isn't entirely accurate
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  // *NOTE*: drivename: for root directories on Win32 cannot be stat()ed
  //         successfully
  //         --> append slash(es)
  if ((directory.size () == 2) && (directory[1] == ':'))
    directory += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif // ACE_WIN32 || ACE_WIN64

  return (!Common_File_Tools::isDirectory (string_in) &&
          (Common_File_Tools::isDirectory (directory) &&
           !file_name.empty ()));
}

bool
Common_File_Tools::isValidPath (const std::string& string_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_File_Tools::isValidPath"));

  // sanity check(s)
  // *TODO*: ACE::dirname() returns '.' on an empty argument; this isn't
  //         entirely accurate
  ACE_ASSERT (!string_in.empty ());

  std::string directory, file_name;
  directory =
    ACE_TEXT_ALWAYS_CHAR (ACE::dirname (string_in.c_str (),
                                        ACE_DIRECTORY_SEPARATOR_CHAR_A));
  file_name =
    ACE_TEXT_ALWAYS_CHAR (ACE::basename (string_in.c_str (),
                                         ACE_DIRECTORY_SEPARATOR_CHAR_A));

  return Common_File_Tools::isDirectory (directory);
}

bool
Common_File_Tools::isAbsolutePath (const std::string& path_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_File_Tools::isAbsolutePath"));

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  std::regex regex (ACE_TEXT_ALWAYS_CHAR ("^([A-Z])(?::\\\\)(?:([^\\\\]+)(?:\\\\))*([^\\\\]+)?$"));
#else
  std::regex regex (ACE_TEXT_ALWAYS_CHAR ("^(?:\\/)(?:([^\\/]+)(?:\\/))*([^\\/]+)?$"));
#endif // ACE_WIN32 || ACE_WIN64
  std::smatch match_results;
  
  if (!std::regex_match (path_in,
                         match_results,
                         regex,
                         std::regex_constants::match_default))
    return false;

  return true;
}

bool
Common_File_Tools::isRelativePath (const std::string& path_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_File_Tools::isRelativePath"));

  //std::string dot_slash_string;
  //dot_slash_string += '.';
  //dot_slash_string += ACE_TEXT_ALWAYS_CHAR (ACE_DIRECTORY_SEPARATOR_CHAR_A);

  //return Common_String_Tools::startswith (path_in, dot_slash_string);

  return !Common_File_Tools::isAbsolutePath (path_in);
}

std::string
Common_File_Tools::basename (const std::string& path_in,
                             bool stripSuffix_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_File_Tools::basename"));

  std::string return_value =
      ACE_TEXT_ALWAYS_CHAR (ACE::basename (path_in.c_str (),
                                           ACE_DIRECTORY_SEPARATOR_CHAR_A));
  if (stripSuffix_in)
  {
    std::string::size_type position = return_value.rfind ('.');
    if (position != std::string::npos)
      return_value.erase (position, std::string::npos);
  } // end IF

  return return_value;
}

bool
Common_File_Tools::isBasename (const std::string& path_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_File_Tools::isBasename"));

  std::string directory, file_name;
  directory =
    ACE_TEXT_ALWAYS_CHAR (ACE::dirname (path_in.c_str (),
                                        ACE_DIRECTORY_SEPARATOR_CHAR_A));
  file_name =
    ACE_TEXT_ALWAYS_CHAR (ACE::basename (path_in.c_str (),
                                         ACE_DIRECTORY_SEPARATOR_CHAR_A));

  return (!ACE_OS::strcmp (directory.c_str (),
                           ACE_TEXT_ALWAYS_CHAR (".")) &&
          !file_name.empty ());
}

bool
Common_File_Tools::create (const std::string& path_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_File_Tools::create"));

  // sanity check(s)
  if (unlikely (Common_File_Tools::isReadable (path_in)))
    return true; // nothing to do
  if (unlikely (path_in.empty ()))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("invalid argument (was empty), aborting\n")));
    return false;
  } // end IF
  // *TODO*

  std::string file_name = path_in;
  int result = -1;
  if (!Common_File_Tools::isValidFilename (file_name))
  {
    ACE_TCHAR buffer[PATH_MAX];
    ACE_OS::memset (buffer, 0, sizeof (buffer));
    if (unlikely (!ACE_OS::getcwd (buffer, sizeof (buffer))))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_OS::getcwd(): \"%m\", aborting\n")));
      return false;
    } // end IF

    file_name = ACE_TEXT_ALWAYS_CHAR (buffer);
    file_name += ACE_DIRECTORY_SEPARATOR_STR_A;
    file_name += path_in;
  } // end IF

  FILE* file_p = NULL;
  file_p = ACE_OS::fopen (ACE_TEXT (file_name.c_str ()),
                          ACE_TEXT ("wb"));
  if (unlikely (!file_p))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::fopen(\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (file_name.c_str ())));
    return false;
  } // end IF
  result = ACE_OS::fclose (file_p);
  if (unlikely (result == -1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::fclose(): \"%m\", aborting\n")));
    return false;
  } // end IF
  //ACE_DEBUG ((LM_DEBUG,
  //            ACE_TEXT ("created: \"%s\"\n"),
  //            ACE_TEXT (file_name.c_str ())));

  return true;
}

bool
Common_File_Tools::createDirectory (const std::string& directory_in,
                                    bool createMissingSubdirectories_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_File_Tools::createDirectory"));

  // sanity check(s)
  ACE_ASSERT (!directory_in.empty ());

  int result = -1;
  //std::vector<std::string> subdirectories_a;
  //std::string subdirectory_string, base_directory_string;

//  if (!createMissingSubdirectories_in)
//    goto continue_;
//
//  // pre-processing
//  base_directory_string = directory_in;
//  while (!base_directory_string.empty ())
//  {
//    subdirectory_string =
//      ACE_TEXT_ALWAYS_CHAR (ACE::basename (ACE_TEXT (directory_in.c_str ()),
//                                           ACE_DIRECTORY_SEPARATOR_CHAR));
//    subdirectories_a.push_back (subdirectory_string);
//    base_directory_string = 
//      ACE_TEXT_ALWAYS_CHAR (ACE::dirname (ACE_TEXT (base_directory_string.c_str ()),
//                                          ACE_DIRECTORY_SEPARATOR_CHAR));
//  } // end WHILE
//  std::reverse (subdirectories_a.begin (), subdirectories_a.end ());
//
//continue_:
  result = ACE_OS::mkdir (ACE_TEXT (directory_in.c_str ()),
                          ACE_DEFAULT_DIR_PERMS);
  if (unlikely (result == -1))
  {
    int error = ACE_OS::last_error ();
    switch (error)
    {
      case ENOENT:
      {
        // OK: some base sub-directory doesn't seem to exist...
        // --> try to recurse
        std::string base_directory = ACE::dirname (ACE_TEXT (directory_in.c_str ()),
                                                   ACE_DIRECTORY_SEPARATOR_CHAR);
        // sanity check: don't recurse for "." !
        if ((base_directory != ACE_TEXT_ALWAYS_CHAR (".")) &&
            createMissingSubdirectories_in)
        {
          if (createDirectory (base_directory))
            return createDirectory (directory_in);
          else
            ACE_DEBUG ((LM_ERROR,
                        ACE_TEXT ("failed to Common_File_Tools::createDirectory(\"%s\"): \"%m\", aborting\n"),
                        ACE_TEXT (base_directory.c_str ())));
        } // end IF

        return false;
      }
      case EEXIST:
      {
        // entry already exists...
        // *IMPORTANT NOTE*: mkdir() returns EEXISTS if a file exists with the
        //                   same name --> error in this case
        if (Common_File_Tools::isFile (directory_in))
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("\"%s\" already exists as a regular file, aborting\n"),
                      ACE_TEXT (directory_in.c_str ())));
          return false;
        } // end IF
        ACE_DEBUG ((LM_WARNING,
                    ACE_TEXT ("\"%s\" already exists, returning\n"),
                    ACE_TEXT (directory_in.c_str ())));
        return true;
      }
      default:
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE_OS::mkdir(\"%s\"): \"%m\", aborting\n"),
                    ACE_TEXT (directory_in.c_str ())));
        return false;
      }
    } // end SWITCH
  } // end IF
  //ACE_DEBUG ((LM_DEBUG,
  //            ACE_TEXT ("created: \"%s\"\n"),
  //            ACE_TEXT (directory_in.c_str ())));

  return true;
}

bool
Common_File_Tools::copyFile (const std::string& path_in,
                             const std::string& directory_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_File_Tools::copyFile"));

  int result = -1;
  ACE_FILE_Addr source_address, target_address;

  result = source_address.set (ACE_TEXT_CHAR_TO_TCHAR (path_in.c_str ()));
  if (unlikely (result == -1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_FILE_Addr::set() file \"%s\": \"%m\", aborting\n"),
                ACE_TEXT (path_in.c_str ())));
    return false;
  } // end IF
  std::string target_filename =
      (directory_in.empty () ? ACE_TEXT_ALWAYS_CHAR (ACE::dirname (ACE_TEXT_CHAR_TO_TCHAR (path_in.c_str ()),
                                                                   ACE_DIRECTORY_SEPARATOR_CHAR))
                             : directory_in);
  target_filename += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  if (directory_in.empty ())
  {
    target_filename += Common_File_Tools::basename (path_in,
                                                    true);
    target_filename +=
        ACE_TEXT_ALWAYS_CHAR (COMMON_FILE_FILENAME_BACKUP_SUFFIX);
  } // end IF
  else
  {
    const ACE_TCHAR* filename_p =
      ACE::basename (ACE_TEXT_CHAR_TO_TCHAR (path_in.c_str ()),
                     ACE_DIRECTORY_SEPARATOR_CHAR);
    if (unlikely (!filename_p))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE::basename(\"%s\"): \"%m\", aborting\n"),
                  ACE_TEXT (path_in.c_str ())));
      return false;
    } // end IF
    target_filename += ACE_TEXT_ALWAYS_CHAR (filename_p);
  } // end ELSE
  result =
    target_address.set (ACE_TEXT_CHAR_TO_TCHAR (target_filename.c_str ()));
  if (unlikely (result == -1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_FILE_Addr::set() file \"%s\": \"%m\", aborting\n"),
                ACE_TEXT (target_filename.c_str ())));
    return false;
  } // end IF

  // don't block on file opening (see ACE doc)...
  ACE_FILE_Connector connector, receptor;
  ACE_FILE_IO source_file, target_file;
  result =
    connector.connect (source_file,                                         // file
                       source_address,                                      // remote SAP
                       const_cast<ACE_Time_Value*> (&ACE_Time_Value::zero), // timeout
                       ACE_Addr::sap_any,                                   // local SAP
                       0,                                                   // reuse address ?
                       (O_RDONLY | O_BINARY | O_EXCL),                      // flags
                       ACE_DEFAULT_FILE_PERMS);                             // permissions
  if (unlikely (result == -1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_FILE_Connector::connect() to file \"%s\": \"%m\", aborting\n"),
                ACE_TEXT (path_in.c_str ())));
    return false;
  } // end IF
  result =
    receptor.connect (target_file,                                         // file
                      target_address,                                      // remote SAP
                      const_cast<ACE_Time_Value*> (&ACE_Time_Value::zero), // timeout
                      ACE_Addr::sap_any,                                   // local SAP
                      0,                                                   // reuse address ?
                      (O_WRONLY | O_CREAT | O_BINARY | O_TRUNC),           // flags
                      ACE_DEFAULT_FILE_PERMS);                             // permissions
  if (unlikely (result == -1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_FILE_Connector::connect() to file \"%s\": \"%m\", aborting\n"),
                ACE_TEXT (target_filename.c_str ())));

    // clean up
    result = source_file.close ();
    if (result == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_FILE::close(\"%s\"): \"%m\", continuing\n"),
                  ACE_TEXT (path_in.c_str ())));

    return false;
  } // end IF

  // copy file
//  iovec io_vec;
//  ACE_OS::memset (&io_vec, 0, sizeof (io_vec));
//  if (source_file.recvv (&io_vec) == -1)
//  {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("failed to ACE_FILE_IO::recvv() file \"%s\": \"%m\", aborting\n"),
//                ACE_TEXT (path_in.c_str())));

//    // clean up
//    if (source_file.close () == -1)
//      ACE_DEBUG ((LM_ERROR,
//                  ACE_TEXT ("failed to ACE_FILE::close(\"%s\"): \"%m\", continuing\n"),
//                  ACE_TEXT (path_in.c_str ())));
//    if (target_file.remove () == -1)
//      ACE_DEBUG ((LM_ERROR,
//                  ACE_TEXT ("failed to ACE_FILE::remove(\"%s\"): \"%m\", continuing\n"),
//                  ACE_TEXT (target_filename.c_str ())));

//    return false;
//  } // end IF
//  ACE_ASSERT (io_vec.iov_base);
//  if (target_file.sendv_n (&io_vec, 1) == -1)
//  {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("failed to ACE_FILE_IO::sendv_n() file \"%s\": \"%m\", aborting\n"),
//                ACE_TEXT (target_filename.c_str ())));

//    // clean up
//    delete[] io_vec.iov_base;
//    if (source_file.close () == -1)
//      ACE_DEBUG ((LM_ERROR,
//                  ACE_TEXT ("failed to ACE_FILE::close(\"%s\"): \"%m\", continuing\n"),
//                  ACE_TEXT (path_in.c_str ())));
//    if (target_file.remove () == -1)
//      ACE_DEBUG ((LM_ERROR,
//                  ACE_TEXT ("failed to ACE_FILE::remove(\"%s\"): \"%m\", continuing\n"),
//                  ACE_TEXT (target_filename.c_str ())));

//    return false;
//  } // end IF
  ACE_FILE_Info file_info;
  result = source_file.get_info (file_info);
  if (unlikely (result == -1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_FILE_IO::get_info(\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (path_in.c_str ())));

    // clean up
    result = source_file.close ();
    if (result == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_FILE::close(\"%s\"): \"%m\", continuing\n"),
                  ACE_TEXT (path_in.c_str ())));
    result = target_file.remove ();
    if (result == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_FILE::remove(\"%s\"): \"%m\", continuing\n"),
                  ACE_TEXT (target_filename.c_str ())));

    return false;
  } // end IF
  ssize_t bytes_written =
    ACE_OS::sendfile (target_file.get_handle (),              // out_fd
                      source_file.get_handle (),              // in_fd
                      NULL,                                   // offset
                      static_cast<size_t> (file_info.size_)); // count
  if (unlikely (bytes_written != file_info.size_))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::sendfile(\"%s\",\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (path_in.c_str ()),
                ACE_TEXT (target_filename.c_str ())));

    // clean up
    result = source_file.close ();
    if (result == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_FILE::close(\"%s\"): \"%m\", continuing\n"),
                  ACE_TEXT (path_in.c_str ())));
    result = target_file.remove ();
    if (result == -1)
      ACE_DEBUG  ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_FILE::remove(\"%s\"): \"%m\", continuing\n"),
                  ACE_TEXT (target_filename.c_str ())));

    return false;
  } // end IF

  // clean up
//  delete[] io_vec.iov_base;
  result = source_file.close ();
  if (unlikely (result == -1))
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_FILE::close(\"%s\"): \"%m\", continuing\n"),
                ACE_TEXT (path_in.c_str ())));
  result = target_file.close ();
  if (unlikely (result == -1))
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_FILE::close(\"%s\"): \"%m\", continuing\n"),
                ACE_TEXT (target_filename.c_str ())));

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("copied \"%s\" --> \"%s\"\n"),
              ACE_TEXT (path_in.c_str ()),
              ACE_TEXT (target_filename.c_str ())));

  return true;
}

bool
Common_File_Tools::deleteFile (const std::string& path_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_File_Tools::deleteFile"));

  int result = -1;

  // connect to the file...
  ACE_FILE_Addr address;
  result = address.set (ACE_TEXT_CHAR_TO_TCHAR (path_in.c_str ()));
  if (unlikely (result == -1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_FILE_Addr::set() file \"%s\": \"%m\", aborting\n"),
                ACE_TEXT (path_in.c_str ())));
    return false;
  } // end IF

  // don't want to block on file opening (see ACE doc)...
  ACE_FILE_Connector connector;
  ACE_FILE_IO file;
  result =
    connector.connect (file,
                       address,
                       const_cast<ACE_Time_Value*> (&ACE_Time_Value::zero),
                       ACE_Addr::sap_any,
                       0,
                       (O_WRONLY | O_BINARY),
                       ACE_DEFAULT_FILE_PERMS);
  if (unlikely (result == -1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_FILE_Connector::connect() to file \"%s\": \"%m\", aborting\n"),
                ACE_TEXT (path_in.c_str ())));
    return false;
  } // end IF

  // delete file
  result = file.remove ();
  if (unlikely (result == -1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_FILE_IO::remove() file \"%s\": \"%m\", aborting\n"),
                ACE_TEXT (path_in.c_str ())));
    return false;
  } // end IF
  //ACE_DEBUG ((LM_DEBUG,
  //            ACE_TEXT ("deleted \"%s\"\n"),
  //            ACE_TEXT (path_in.c_str ())));

  return true;
}

bool
Common_File_Tools::deleteFiles (const Common_File_IdentifierList_t& identifiers_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_File_Tools::deleteFiles"));

  bool result = true;

  for (Common_File_IdentifierListIterator_t iterator = identifiers_in.begin ();
       iterator != identifiers_in.end ();
       ++iterator)
    result = result && Common_File_Tools::deleteFile ((*iterator).identifier);

  return result;
}

bool
Common_File_Tools::load (const std::string& path_in,
                         uint8_t*& file_out,
                         ACE_UINT64& fileSize_out,
                         ACE_UINT64 padding_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_File_Tools::load"));

  bool result = true;
  int result_2 = -1;
  size_t result_3 = 0;
  FILE* file_p = NULL;

  // initialize return value(s)
  file_out = NULL;
  fileSize_out = Common_File_Tools::size (path_in);

  // sanity check(s)
  if (unlikely (!fileSize_out && !padding_in))
  {
    result = true;
    goto continue_;
  } // end IF

  file_p = ::fopen (path_in.c_str (),
                    ACE_TEXT_ALWAYS_CHAR ("rb"));
  if (unlikely (!file_p))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::fopen(\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (path_in.c_str ())));
    return false;
  } // end IF

  // *PORTABILITY* allocate array
  ACE_NEW_NORETURN (file_out,
                    uint8_t[fileSize_out + padding_in]);
  if (unlikely (!file_out))
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to allocate memory (was: %Q byte(s)): \"%m\", aborting\n"),
                fileSize_out + padding_in));
    goto error;
  } // end IF
  ACE_OS::memset (file_out, 0, sizeof (uint8_t) * (fileSize_out + padding_in));

  // read data
  result_3 =
    ACE_OS::fread (static_cast<void*> (file_out), // target buffer
                   fileSize_out,                  // read everything ...
                   1,                             // ... at once
                   file_p);                       // stream handle
  if (unlikely (result_3 != static_cast<size_t> (1)))
  { // *NOTE*: apparently, virtual filesystem files (e.g. sysfs) do not report
    //         correct sizes; fread fails but reads what is there --> continue
    if (::feof (file_p))
      ACE_DEBUG ((LM_WARNING,
                  ACE_TEXT ("failed to ACE_OS::fread(\"%s\",%Q): \"%m\" (sysfs short read ?), continuing\n"),
                  ACE_TEXT (path_in.c_str ()),
                  fileSize_out));
    else
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_OS::fread(\"%s\",%Q): \"%m\", aborting\n"),
                  ACE_TEXT (path_in.c_str ()),
                  fileSize_out));
      goto error;
    } // end ELSE
  } // end IF

  goto continue_;

error:
  if (file_out)
  {
    delete [] file_out; file_out = NULL;
  } // end IF
  fileSize_out = 0;

  result = false;

continue_:
  if (likely (file_p))
  {
    result_2 = ACE_OS::fclose (file_p);
    if (unlikely (result_2 == -1))
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_OS::fclose(\"%s\"): \"%m\", continuing\n"),
                  ACE_TEXT (path_in.c_str ())));
  } // end IF

  return result;
}

bool
Common_File_Tools::open (const std::string& path_in,
                         int flags_in,
                         ACE_FILE_IO& stream_out)
{
  COMMON_TRACE (ACE_TEXT ("Common_File_Tools::open"));

  int result = -1;

  ACE_FILE_Addr file_address;
  result = file_address.set (path_in.c_str ());
  if (unlikely (result == -1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_FILE_Addr::set(\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (path_in.c_str ())));
    return false;
  } // end IF

  ACE_FILE_Connector file_connector;
  result =
    file_connector.connect (stream_out,              // stream
                            file_address,            // filename
                            NULL,                    // timeout (block)
                            ACE_Addr::sap_any,       // (local) filename: N/A
                            0,                       // reuse_addr: N/A
                            flags_in,                // flags
                            ACE_DEFAULT_FILE_PERMS); // permissions
  if (unlikely (result == -1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_FILE_Connector::connect(\"%s\",%d): \"%m\", aborting\n"),
                ACE_TEXT (path_in.c_str ()),
                flags_in));
    return false;
  } // end IF

  return true;
}

bool
Common_File_Tools::store (const std::string& path_in,
                          const uint8_t* buffer_in,
                          ACE_UINT64 size_in,
                          bool appendIfExists_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_File_Tools::store"));

  // initialize return value(s)
  bool result = false;

  // append ?
  bool file_exists = Common_File_Tools::exists (path_in);
  const char* mode_p =
      (file_exists && appendIfExists_in ? ACE_TEXT_ALWAYS_CHAR ("ab")   // append
                                        : ACE_TEXT_ALWAYS_CHAR ("wb")); // create/overwrite
  size_t result_2 = 0;
  int result_3 = -1;
  FILE* file_p = NULL;

  // create sub-directories ?
  std::string base_directory =
    ACE_TEXT_ALWAYS_CHAR (ACE::dirname (ACE_TEXT (path_in.c_str ()),
                                        ACE_DIRECTORY_SEPARATOR_CHAR));
  if (!Common_File_Tools::isDirectory (base_directory))
    if (!Common_File_Tools::createDirectory (base_directory))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Common_File_Tools::createDirectory(\"%s\"): \"%m\", aborting\n"),
                  ACE_TEXT (base_directory.c_str ())));
      return false;
    } // end IF

  file_p = ACE_OS::fopen (path_in.c_str (),
                          mode_p);
  if (unlikely (!file_p))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::fopen(\"%s\",\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (path_in.c_str ()),
                ACE_TEXT (mode_p)));
    return false;
  } // end IF

  result_2 = ACE_OS::fwrite (buffer_in, size_in, 1, file_p);
  if (unlikely (result_2 != 1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::fwrite(%u) (file was: \"%s\"): \"%m\", aborting\n"),
                size_in,
                ACE_TEXT (path_in.c_str ())));
    goto clean;
  } // end IF

  result = true;

clean:
  // clean up
  if (likely (file_p))
  {
    result_3 = ACE_OS::fclose (file_p);
    if (unlikely (result_3 == -1))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_OS::fclose(\"%s\"): \"%m\", aborting\n"),
                  ACE_TEXT (path_in.c_str ())));
      result = false;
    } // end IF
  } // end IF

  if (likely (result))
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("%s file \"%s\" (%u byte(s))\n"),
                ((file_exists && appendIfExists_in) ? ACE_TEXT ("appended") : ACE_TEXT ("wrote")),
                ACE_TEXT (path_in.c_str ()),
                size_in));

  return result;
}

ACE_UINT64
Common_File_Tools::size (const ACE_FILE_Addr& address_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_File_Tools::size"));

  int result = -1;
  ACE_FILE_Connector file_connector;
  ACE_FILE_IO file_io;
  ACE_TCHAR buffer_a[PATH_MAX];
  ACE_FILE_Info file_info;

  result =
    file_connector.connect (file_io,                 // stream
                            address_in,              // filename
                            NULL,                    // timeout (block)
                            ACE_Addr::sap_any,       // (local) filename: N/A
                            0,                       // reuse_addr: N/A
                            (O_RDONLY |
                             O_BINARY),              // flags --> open
                            ACE_DEFAULT_FILE_PERMS); // permissions --> open
  if (unlikely (result == -1))
  {
    result = address_in.addr_to_string (buffer_a,
                                        sizeof (ACE_TCHAR[PATH_MAX]));
    if (result == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_FILE_Addr::addr_to_string(): \"%m\", continuing\n")));
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_FILE_Connector::connect(\"%s\"): \"%m\", aborting\n"),
                buffer_a));
    return 0;
  } // end IF

  result = file_io.get_info (file_info);
  if (unlikely (result == -1))
  {
    result = address_in.addr_to_string (buffer_a,
                                        sizeof (ACE_TCHAR[PATH_MAX]));
    if (result == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_FILE_Addr::addr_to_string(): \"%m\", continuing\n")));
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_FILE_Connector::connect(\"%s\"): \"%m\", aborting\n"),
                buffer_a));

    // clean up
    result = file_io.close ();
    if (result == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_FILE_IO::close(): \"%m\", continuing\n")));

    return 0;
  } // end IF

  // clean up
  result = file_io.close ();
  if (unlikely (result == -1))
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_FILE_IO::close(): \"%m\", continuing\n")));

  return static_cast<ACE_UINT64> (file_info.size_);
}

ACE_UINT64
Common_File_Tools::size (const std::string& path_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_File_Tools::size"));

  ACE_UINT64 result;

  ACE_stat stat_s;
  ACE_OS::memset (&stat_s, 0, sizeof (ACE_stat));
  result = ACE_OS::stat (path_in.c_str (),
                         &stat_s);
  if (unlikely (result == static_cast<ACE_UINT64> (-1)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::stat(\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (path_in.c_str ())));
    return 0;
  } // end IF

  return static_cast<ACE_UINT64> (stat_s.st_size);
}

Common_File_IdentifierList_t
Common_File_Tools::files (const std::string& directory_in,
                          ACE_SCANDIR_SELECTOR selector_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_File_Tools::files"));

  Common_File_IdentifierList_t return_value;

  int result = -1;
  ACE_Dirent_Selector entries;
  result = entries.open (ACE_TEXT (directory_in.c_str ()),
                         selector_in,
                         NULL);
  if (unlikely (result == -1))
  {
    int error_i = ACE_OS::last_error ();
    ACE_DEBUG (((error_i == ENOENT ? LM_DEBUG : LM_ERROR),
                ACE_TEXT ("failed to ACE_Dirent_Selector::open(\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (directory_in.c_str ())));
    return return_value;
  } // end IF

  struct Common_File_Identifier file_identifier_s;
  for (int i = 0;
       i < entries.length ();
       ++i)
  {
    file_identifier_s.identifier = ACE_TEXT_ALWAYS_CHAR (entries[i]->d_name);
    return_value.push_back (file_identifier_s);
  } // end FOR

  // clean up
  result = entries.close ();
  if (unlikely (result == -1))
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Dirent_Selector::close(\"%s\"): \"%m\", continuing\n"),
                ACE_TEXT (directory_in.c_str ())));

  return return_value;
}

ACE_UINT64
Common_File_Tools::size (const Common_File_IdentifierList_t& identifiers_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_File_Tools::size"));

  ACE_UINT64 result = 0;

  for (Common_File_IdentifierListIterator_t iterator = identifiers_in.begin ();
       iterator != identifiers_in.end ();
       ++iterator)
    result += Common_File_Tools::size ((*iterator).identifier);

  return result;
}

std::string
Common_File_Tools::linkTarget (const std::string& path_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_File_Tools::linkTarget"));

  // initialize result(s)
  std::string return_value;

  char buffer_a[PATH_MAX];
  // *NOTE*: "...readlink() does not append a null byte to buf..."
  ACE_OS::memset (buffer_a, 0, sizeof (char[PATH_MAX]));
  if (unlikely (!ACE_OS::readlink (path_in.c_str (),
                                   buffer_a,
                                   sizeof (char[PATH_MAX]))))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::readlink(\"%s\"): %m, aborting\n"),
                ACE_TEXT (path_in.c_str ())));
    return return_value;
  } // end IF
  return_value = buffer_a;

  return return_value;
}

std::string
Common_File_Tools::realPath (const std::string& path_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_File_Tools::realPath"));

  // initialize result(s)
  std::string return_value;

  char buffer_a[PATH_MAX];
  if (unlikely (!ACE_OS::realpath (path_in.c_str (),
                                   buffer_a)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::realpath(\"%s\"): %m, aborting\n"),
                ACE_TEXT (path_in.c_str ())));
    return return_value;
  } // end IF
  return_value = buffer_a;

  return return_value;
}

bool
Common_File_Tools::setWorkingDirectory (const std::string& path_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_File_Tools::setWorkingDirectory"));

  int result =
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    _chdir (path_in.c_str ());
#else
    chdir (path_in.c_str ());
#endif // ACE_WIN32 || ACE_WIN64
  if (unlikely (result == -1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ::chdir(\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (path_in.c_str ())));
    return false;
  } // end IF

  return true;
}

std::string
Common_File_Tools::getWorkingDirectory ()
{
  COMMON_TRACE (ACE_TEXT ("Common_File_Tools::getWorkingDirectory"));

  // initialize return value(s)
  std::string return_value;

  // retrieve working directory
  ACE_TCHAR buffer_a[PATH_MAX];
  ACE_OS::memset (buffer_a, 0, sizeof (ACE_TCHAR[PATH_MAX]));
  if (unlikely (!ACE_OS::getcwd (buffer_a,
                                 sizeof (ACE_TCHAR[PATH_MAX]))))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::getcwd(): \"%m\", aborting\n")));
    return ACE_TEXT_ALWAYS_CHAR ("");
  } // end IF
  return_value = ACE_TEXT_ALWAYS_CHAR (buffer_a);

  return return_value;
}

std::string
Common_File_Tools::getSourceDirectory (const std::string& packageName_in,
                                       const std::string& moduleName_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_File_Tools::getSourceDirectory"));

  // initialize return value(s)
  std::string return_value;

  // sanity check(s)
  ACE_ASSERT (!packageName_in.empty ());

  ACE_TCHAR buffer_a[PATH_MAX];
  ACE_OS::memset (buffer_a, 0, sizeof (ACE_TCHAR[PATH_MAX]));

  ACE_TCHAR* string_p =
    ACE_OS::getenv (ACE_TEXT (COMMON_ENVIRONMENT_DIRECTORY_ROOT_PROJECTS));
  if (!string_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::getenv(%s): \"%m\", aborting\n"),
                ACE_TEXT (COMMON_ENVIRONMENT_DIRECTORY_ROOT_PROJECTS)));
    return ACE_TEXT_ALWAYS_CHAR ("");
  } // end IF
  return_value = ACE_TEXT_ALWAYS_CHAR (string_p);
  return_value += ACE_DIRECTORY_SEPARATOR_STR_A;
  //return_value += ACE_TEXT_ALWAYS_CHAR (ACE_DLL_PREFIX);
  return_value += packageName_in;
  return_value += ACE_DIRECTORY_SEPARATOR_STR_A;
  return_value += ACE_TEXT_ALWAYS_CHAR (COMMON_LOCATION_SOURCE_SUBDIRECTORY);
  if (!moduleName_in.empty ())
  {
    return_value += ACE_DIRECTORY_SEPARATOR_STR_A;
    return_value += moduleName_in;
  } // end IF

  // sanity check(s)
  if (unlikely (!Common_File_Tools::isDirectory (return_value)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("not a directory (was: \"%s\"), aborting\n"),
                ACE_TEXT (return_value.c_str ())));
    return ACE_TEXT_ALWAYS_CHAR ("");
  } // end IF

  return return_value;
}

std::string
Common_File_Tools::getSystemConfigurationDataDirectory (const std::string& packageName_in,
                                                        const std::string& moduleName_in,
                                                        bool isConfiguration_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_File_Tools::getSystemConfigurationDataDirectory"));

  // initialize return value(s)
  std::string return_value;

  // sanity check(s)
  ACE_ASSERT (!packageName_in.empty ());

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#if defined (UNICODE)
#if defined (ACE_USES_WCHAR)
  ACE_TCHAR buffer_a[PATH_MAX];
  ACE_OS::memset (buffer_a, 0, sizeof (ACE_TCHAR[PATH_MAX]));
#else
  ACE_ANTI_TCHAR buffer_a[PATH_MAX];
  ACE_OS::memset (buffer_a, 0, sizeof (ACE_ANTI_TCHAR[PATH_MAX]));
#endif // ACE_USES_WCHAR
#else
#if defined (ACE_USES_WCHAR)
  ACE_ANTI_TCHAR buffer_a[PATH_MAX];
  ACE_OS::memset (buffer_a, 0, sizeof (ACE_ANTI_TCHAR[PATH_MAX]));
#else
  ACE_TCHAR buffer_a[PATH_MAX];
  ACE_OS::memset (buffer_a, 0, sizeof (ACE_TCHAR[PATH_MAX]));
#endif // ACE_USES_WCHAR
#endif // UNICODE

  HRESULT result_2 =
#if defined (UNICODE)
    SHGetFolderPathW (NULL,                                         // hwndOwner
#else
    SHGetFolderPathA (NULL,                                         // hwndOwner
#endif // UNICODE
                      CSIDL_LOCAL_APPDATA | CSIDL_FLAG_DONT_VERIFY, // nFolder
                      NULL,                                         // hToken
                      SHGFP_TYPE_CURRENT,                           // dwFlags
                      buffer_a);                                    // pszPath
  if (unlikely (FAILED (result_2)))
  {
    ACE_DEBUG ((LM_WARNING,
                ACE_TEXT ("failed to SHGetFolderPath(CSIDL_APPDATA): \"%s\", falling back\n"),
                ACE_TEXT (Common_Error_Tools::errorToString (static_cast<DWORD> (result_2)).c_str ())));
    return Common_File_Tools::getWorkingDirectory ();
  } // end IF
  return_value = ACE_TEXT_ALWAYS_CHAR (buffer_a);
  return_value += ACE_DIRECTORY_SEPARATOR_STR_A;
  return_value += packageName_in;
  return_value += ACE_DIRECTORY_SEPARATOR_STR_A;
  return_value +=
      (isConfiguration_in ? ACE_TEXT_ALWAYS_CHAR (COMMON_LOCATION_CONFIGURATION_SUBDIRECTORY)
                          : ACE_TEXT_ALWAYS_CHAR (COMMON_LOCATION_DATA_SUBDIRECTORY));
#else
  return_value =
    ACE_TEXT_ALWAYS_CHAR (COMMON_LOCATION_APPLICATION_STORAGE_ROOT_DIRECTORY);
  return_value += ACE_DIRECTORY_SEPARATOR_STR_A;
  return_value +=
    ACE_TEXT_ALWAYS_CHAR (COMMON_LOCATION_APPLICATION_STORAGE_LOCAL_SUBDIRECTORY);
  return_value += ACE_DIRECTORY_SEPARATOR_STR_A;
  return_value +=
    (isConfiguration_in ? ACE_TEXT_ALWAYS_CHAR (COMMON_LOCATION_CONFIGURATION_SUBDIRECTORY)
                        : ACE_TEXT_ALWAYS_CHAR (COMMON_LOCATION_APPLICATION_STORAGE_SUBDIRECTORY));
  return_value += ACE_DIRECTORY_SEPARATOR_STR_A;
  return_value += packageName_in;
  if (!moduleName_in.empty ())
  {
    return_value += ACE_DIRECTORY_SEPARATOR_STR_A;
    return_value += moduleName_in;
  } // end IF
#endif // ACE_WIN32 || ACE_WIN64

  return return_value;
}

std::string
Common_File_Tools::getConfigurationDataDirectory (const std::string& packageName_in,
                                                  const std::string& moduleName_in,
                                                  bool isConfiguration_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_File_Tools::getConfigurationDataDirectory"));

  // initialize return value(s)
  std::string return_value;

  // sanity check(s)
  ACE_ASSERT (!packageName_in.empty ());

  bool is_test_b =
    (!ACE_OS::strncmp (moduleName_in.c_str (), ACE_TEXT_ALWAYS_CHAR (COMMON_LOCATION_TEST_I_SUBDIRECTORY),
                       ACE_OS::strlen (ACE_TEXT_ALWAYS_CHAR (COMMON_LOCATION_TEST_I_SUBDIRECTORY))) ||
     !ACE_OS::strncmp (moduleName_in.c_str (), ACE_TEXT_ALWAYS_CHAR (COMMON_LOCATION_TEST_U_SUBDIRECTORY),
                       ACE_OS::strlen (ACE_TEXT_ALWAYS_CHAR (COMMON_LOCATION_TEST_U_SUBDIRECTORY))));

  if (Common_Error_Tools::inDebugSession ())
  {
    if (is_test_b)
    {
      // sanity check(s)
      ACE_ASSERT (!Common_File_Tools::executable.empty ());

      return_value =
        Common_File_Tools::getSourceDirectory (packageName_in,
                                               ACE_TEXT_ALWAYS_CHAR (""));
      return_value += ACE_DIRECTORY_SEPARATOR_STR_A;
      return_value +=
        ACE_TEXT_ALWAYS_CHAR (COMMON_LOCATION_PARENT_SUBDIRECTORY);
      return_value += ACE_DIRECTORY_SEPARATOR_STR_A;
      return_value += moduleName_in;
      return_value += ACE_DIRECTORY_SEPARATOR_STR_A;
      
      std::string executable_directory_string =
        Common_String_Tools::tolower (Common_File_Tools::basename (Common_File_Tools::executable, true));
      // *IMPORTANT NOTE*: strip any trailing "_%d"
      std::regex regex (ACE_TEXT_ALWAYS_CHAR ("^(?:.+)(?:_)(?:[[:digit:]]+)$"));
      std::smatch match_results;
      if (std::regex_match (executable_directory_string,
                            match_results,
                            regex,
                            std::regex_constants::match_default))
      {
        size_t position = executable_directory_string.rfind ('_');
        ACE_ASSERT (position != std::string::npos);
        executable_directory_string.erase (position, std::string::npos);
      } // end IF
      return_value += executable_directory_string;
      return_value += ACE_DIRECTORY_SEPARATOR_STR_A;
      return_value +=
        (isConfiguration_in ? ACE_TEXT_ALWAYS_CHAR (COMMON_LOCATION_CONFIGURATION_SUBDIRECTORY)
                            : ACE_TEXT_ALWAYS_CHAR (COMMON_LOCATION_DATA_SUBDIRECTORY));
    } // end IF
    else
    {
      return_value = Common_File_Tools::getSourceDirectory (packageName_in,
                                                            moduleName_in);
      return_value += ACE_DIRECTORY_SEPARATOR_STR_A;
      return_value +=
        (isConfiguration_in ? ACE_TEXT_ALWAYS_CHAR (COMMON_LOCATION_CONFIGURATION_SUBDIRECTORY)
                            : ACE_TEXT_ALWAYS_CHAR (COMMON_LOCATION_DATA_SUBDIRECTORY));
    } // end ELSE
    // sanity check(s)
    ACE_ASSERT (Common_File_Tools::isDirectory (return_value));

    return return_value;
  } // end IF

  // not running in a debug session

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  if (is_test_b)
  {
    return_value = Common_File_Tools::getWorkingDirectory ();
    return_value += ACE_DIRECTORY_SEPARATOR_STR_A;
    //return_value +=
    //  ACE_TEXT_ALWAYS_CHAR (COMMON_LOCATION_PARENT_SUBDIRECTORY);
    //return_value += ACE_DIRECTORY_SEPARATOR_STR_A;
    return_value +=
      (isConfiguration_in ? ACE_TEXT_ALWAYS_CHAR (COMMON_LOCATION_CONFIGURATION_SUBDIRECTORY)
                          : ACE_TEXT_ALWAYS_CHAR (COMMON_LOCATION_DATA_SUBDIRECTORY));
#if defined (_DEBUG)
#else
    return_value += ACE_DIRECTORY_SEPARATOR_STR_A;
    std::string executable_directory_string =
      Common_String_Tools::tolower (Common_File_Tools::basename (Common_File_Tools::executable, true));
    // *IMPORTANT NOTE*: strip any trailing "_%d"
    std::regex regex (ACE_TEXT_ALWAYS_CHAR ("^(?:.+)(?:_)(?:[[:digit:]]+)$"));
    std::smatch match_results;
    if (std::regex_match (executable_directory_string,
                          match_results,
                          regex,
                          std::regex_constants::match_default))
    {
      size_t position = executable_directory_string.rfind ('_');
      ACE_ASSERT (position != std::string::npos);
      executable_directory_string.erase (position, std::string::npos);
    } // end IF
    return_value += executable_directory_string;
#endif // _DEBUG
  } // end IF
  else
  {
    return_value = Common_File_Tools::getWorkingDirectory ();
    return_value += ACE_DIRECTORY_SEPARATOR_STR_A;
    //return_value +=
    //  ACE_TEXT_ALWAYS_CHAR (COMMON_LOCATION_PARENT_SUBDIRECTORY);
    //return_value += ACE_DIRECTORY_SEPARATOR_STR_A;
    return_value +=
        (isConfiguration_in ? ACE_TEXT_ALWAYS_CHAR (COMMON_LOCATION_CONFIGURATION_SUBDIRECTORY)
                            : ACE_TEXT_ALWAYS_CHAR (COMMON_LOCATION_DATA_SUBDIRECTORY));
    //return_value += ACE_DIRECTORY_SEPARATOR_STR_A;
    //return_value +=
    //  Common_String_Tools::tolower (Common_File_Tools::basename (Common_File_Tools::executable, true));
  } // end ELSE
#else
  if (is_test_b)
  {
    return_value = Common_File_Tools::getWorkingDirectory ();
    return_value += ACE_DIRECTORY_SEPARATOR_STR_A;
    return_value +=
      (isConfiguration_in ? ACE_TEXT_ALWAYS_CHAR (COMMON_LOCATION_CONFIGURATION_SUBDIRECTORY)
                          : ACE_TEXT_ALWAYS_CHAR (COMMON_LOCATION_DATA_SUBDIRECTORY));
#if defined (_DEBUG)
#else
    return_value += ACE_DIRECTORY_SEPARATOR_STR_A;
    return_value +=
      Common_String_Tools::tolower (Common_File_Tools::basename (Common_File_Tools::executable, true));
#endif // _DEBUG
  } // end IF
  else
  {
    return_value = Common_File_Tools::getWorkingDirectory ();
    return_value += ACE_DIRECTORY_SEPARATOR_STR_A;
    return_value +=
      (isConfiguration_in ? ACE_TEXT_ALWAYS_CHAR (COMMON_LOCATION_CONFIGURATION_SUBDIRECTORY)
                          : ACE_TEXT_ALWAYS_CHAR (COMMON_LOCATION_DATA_SUBDIRECTORY));
  } // end ELSE
  // std::string module_name_string =
  //     (moduleName_in.empty () ? ACE_TEXT_ALWAYS_CHAR ("")
  //                             : Common_String_Tools::tolower (Common_File_Tools::basename (Common_File_Tools::executable, true)));
  // return_value =
  //   Common_File_Tools::getSystemConfigurationDataDirectory (packageName_in,
  //                                                           module_name_string,
  //                                                           isConfiguration_in);
#endif // ACE_WIN32 || ACE_WIN64

  // sanity check(s)
  ACE_DEBUG ((LM_INFO, ACE_TEXT ("\"%s\"\n"), ACE_TEXT (return_value.c_str ())));
  ACE_ASSERT (Common_File_Tools::isDirectory (return_value));

  return return_value;
}

std::string
Common_File_Tools::getHomeDirectory (const std::string& userName_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_File_Tools::getHomeDirectory"));

  // initialize result value(s)
  std::string result;

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  HANDLE      token_h = ACE_INVALID_HANDLE;
  ACE_TCHAR   buffer_a[PATH_MAX];
  DWORD       buffer_size = sizeof (ACE_TCHAR[PATH_MAX]);
  ACE_OS::memset (buffer_a, 0, sizeof (ACE_TCHAR[PATH_MAX]));
#else
  int            result_2 = -1;
  struct passwd  passwd_s;
  struct passwd* passwd_p = NULL;
  char           buffer_a[BUFSIZ]; // _SC_GETPW_R_SIZE_MAX
  ACE_OS::memset (buffer_a, 0, sizeof (char[BUFSIZ]));
#endif // ACE_WIN32 || ACE_WIN64

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
  std::string username_string = userName_in;
  if (unlikely (username_string.empty ()))
  {
    // fallback --> use current user
    std::string real_username_string;
    Common_OS_Tools::getUserName (static_cast<uid_t> (-1),
                                  username_string,
                                  real_username_string);
    if (username_string.empty ())
    {
      ACE_DEBUG ((LM_WARNING,
                  ACE_TEXT ("failed to Common_OS_Tools::getUserName(), falling back\n")));
      goto fallback;
    } // end IF
  } // end IF
#endif // ACE_WIN32 || ACE_WIN64

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  if (unlikely (!OpenProcessToken (::GetCurrentProcess (),
                                   TOKEN_QUERY,
                                   &token_h)))
  {
    ACE_DEBUG ((LM_WARNING,
                ACE_TEXT ("failed to ::OpenProcessToken(): \"%s\", falling back\n"),
                ACE_TEXT (Common_Error_Tools::errorToString (::GetLastError (), false).c_str ())));
    goto fallback;
  } // end IF
  ACE_ASSERT (token_h != ACE_INVALID_HANDLE);

  // *TODO*: use ACE_TEXT_ macro for this (see also: ace_wchar.h)
#if defined (ACE_USES_WCHAR)
  if (unlikely (!GetUserProfileDirectoryW (token_h, buffer_a, &buffer_size)))
#else
  if (unlikely (!GetUserProfileDirectoryA (token_h, buffer_a, &buffer_size)))
#endif // ACE_USES_WCHAR
  {
    ACE_DEBUG ((LM_WARNING,
                ACE_TEXT ("failed to GetUserProfileDirectory(): \"%s\", falling back\n"),
                ACE_TEXT (Common_Error_Tools::errorToString (::GetLastError (), false).c_str ())));

    // clean up
    if (!CloseHandle (token_h))
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to CloseHandle(): \"%s\", continuing\n"),
                  ACE_TEXT (Common_Error_Tools::errorToString (::GetLastError (), false).c_str ())));

    goto fallback;
  } // end IF

  // clean up
  if (unlikely (!CloseHandle (token_h)))
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to CloseHandle(): \"%s\", continuing\n"),
                ACE_TEXT (Common_Error_Tools::errorToString (::GetLastError (), false).c_str ())));

  result = ACE_TEXT_ALWAYS_CHAR (buffer_a);
#else
  result_2 = ACE_OS::getpwnam_r (username_string.c_str (), // user name
                                 &passwd_s,                // passwd entry
                                 buffer_a,                 // buffer
                                 sizeof (char[BUFSIZ]),    // buffer size
                                 &passwd_p);               // result (handle)
  if (unlikely (!passwd_p))
  {
    if (result_2 == 0)
      ACE_DEBUG ((LM_WARNING,
                  ACE_TEXT ("user \"%s\" not found, falling back\n"),
                  ACE_TEXT (username_string.c_str ())));
    else
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_OS::getpwnam_r(\"%s\"): \"%m\", falling back\n"),
                  ACE_TEXT (username_string.c_str ())));
    goto fallback;
  } // end IF

  result = ACE_TEXT_ALWAYS_CHAR (passwd_s.pw_dir);
#endif // ACE_WIN32 || ACE_WIN64

  return result;

fallback:
  ACE_TCHAR* string_p =
      ACE_OS::getenv (ACE_TEXT (COMMON_LOCATION_TEMPORARY_STORAGE_VARIABLE)); // *TODO*: use $HOME on UNIX
  if (unlikely (!string_p))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::getenv(\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (COMMON_LOCATION_TEMPORARY_STORAGE_VARIABLE)));
    return result;
  } // end IF
  result = ACE_TEXT_ALWAYS_CHAR (string_p);

  return result;
}

std::string
Common_File_Tools::getUserConfigurationDirectory ()
{
  COMMON_TRACE (ACE_TEXT ("Common_File_Tools::getUserConfigurationDirectory"));

  std::string result;

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#if defined (UNICODE)
#if defined (ACE_USES_WCHAR)
  ACE_TCHAR buffer_a[PATH_MAX];
  ACE_OS::memset (buffer_a, 0, sizeof (ACE_TCHAR[PATH_MAX]));
#else
  ACE_ANTI_TCHAR buffer_a[PATH_MAX];
  ACE_OS::memset (buffer_a, 0, sizeof (ACE_ANTI_TCHAR[PATH_MAX]));
#endif // ACE_USES_WCHAR
#else
#if defined (ACE_USES_WCHAR)
  ACE_ANTI_TCHAR buffer_a[PATH_MAX];
  ACE_OS::memset (buffer_a, 0, sizeof (ACE_ANTI_TCHAR[PATH_MAX]));
#else
  ACE_TCHAR buffer_a[PATH_MAX];
  ACE_OS::memset (buffer_a, 0, sizeof (ACE_TCHAR[PATH_MAX]));
#endif // ACE_USES_WCHAR
#endif // UNICODE

  HRESULT result_2 =
#if defined (UNICODE)
    SHGetFolderPathW (NULL,                                   // hwndOwner
#else
    SHGetFolderPathA (NULL,                                   // hwndOwner
#endif // UNICODE
                      CSIDL_APPDATA | CSIDL_FLAG_DONT_VERIFY, // nFolder
                      NULL,                                   // hToken
                      SHGFP_TYPE_CURRENT,                     // dwFlags
                      buffer_a);                              // pszPath
  if (unlikely (FAILED (result_2)))
  {
    ACE_DEBUG ((LM_WARNING,
                ACE_TEXT ("failed to SHGetFolderPath(CSIDL_APPDATA): \"%s\", falling back\n"),
                ACE_TEXT (Common_Error_Tools::errorToString (static_cast<DWORD> (result_2), false).c_str ())));
    goto fallback;
  } // end IF

  result = ACE_TEXT_ALWAYS_CHAR (buffer_a);
  result += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#else
  std::string username_string;
  std::string real_username_string;
  Common_OS_Tools::getUserName (static_cast<uid_t> (-1),
                                username_string,
                                real_username_string);
  if (unlikely (username_string.empty ()))
  {
    ACE_DEBUG ((LM_WARNING,
                ACE_TEXT ("failed to Common_OS_Tools::getUserName(), falling back\n")));
    goto fallback;
  } // end IF

  result = Common_File_Tools::getHomeDirectory (username_string);
  if (unlikely (result.empty ()))
  {
    ACE_DEBUG ((LM_WARNING,
                ACE_TEXT("failed to Common_File_Tools::getHomeDirectory(\"%s\"), falling back\n"),
                ACE_TEXT (username_string.c_str ())));
    goto fallback;
  } // end IF

  result += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  result += '.';
#endif // ACE_WIN32 || ACE_WIN64
  result += ACE_TEXT_ALWAYS_CHAR (Common_PACKAGE_NAME);

  if (unlikely (!Common_File_Tools::isDirectory (result)))
  {
    if (!Common_File_Tools::createDirectory (result))
    {
      ACE_DEBUG ((LM_WARNING,
                  ACE_TEXT ("failed to Common_File_Tools::createDirectory(\"%s\"), falling back\n"),
                  ACE_TEXT (result.c_str ())));
      goto fallback;
    } // end IF
    else
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("created package directory \"%s\"\n"),
                  ACE_TEXT (result.c_str ())));
  } // end IF

  return result;

fallback:
  ACE_TCHAR* string_p =
      ACE_OS::getenv (ACE_TEXT (COMMON_LOCATION_TEMPORARY_STORAGE_VARIABLE));
  if (unlikely (!string_p))
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::getenv(\"%s\"): \"%m\", continuing\n"),
                ACE_TEXT (COMMON_LOCATION_TEMPORARY_STORAGE_VARIABLE)));
  result = ACE_TEXT_ALWAYS_CHAR (string_p);

  return result;
}

std::string
Common_File_Tools::getUserDownloadDirectory (const std::string& userName_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_File_Tools::getUserDownloadDirectory"));

  std::string result;

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#if COMMON_OS_WIN32_TARGET_PLATFORM (0x0600) // _WIN32_WINNT_VISTA
  PWSTR buffer_p = NULL;
  HRESULT result_2 =
    SHGetKnownFolderPath (FOLDERID_Downloads, // rfid
                          KF_FLAG_DEFAULT,    // dwFlags
                          NULL,               // hToken
                          &buffer_p);         // ppszPath
  if (unlikely (FAILED (result_2)))
  {
    ACE_DEBUG ((LM_WARNING,
                ACE_TEXT ("failed to SHGetKnownFolderPath(FOLDERID_Downloads): \"%s\", aborting\n"),
                ACE_TEXT (Common_Error_Tools::errorToString (static_cast<DWORD> (result_2), false).c_str ())));
     CoTaskMemFree (buffer_p); buffer_p = NULL;
     return result;
  } // end IF
  ACE_ASSERT (buffer_p);
  result = ACE_TEXT_ALWAYS_CHAR (ACE_TEXT_WCHAR_TO_TCHAR (buffer_p));
  CoTaskMemFree (buffer_p); buffer_p = NULL;
#else
  ACE_ASSERT (false); // *TODO*
  ACE_NOTSUP_RETURN (result);
  ACE_NOTREACHED (return result;)
#endif // _WIN32_WINNT_VISTA
#else
  std::string username_string = userName_in;
  if (unlikely (username_string.empty ()))
  {
    // fallback --> use current user
    std::string real_username_string;
    Common_OS_Tools::getUserName (static_cast<uid_t> (-1),
                                  username_string,
                                  real_username_string);
    if (username_string.empty ())
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Common_OS_Tools::getUserName(), aborting\n")));
      return result;
    } // end IF
  } // end IF

  result = Common_File_Tools::getHomeDirectory (username_string);
  result += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  result += ACE_TEXT_ALWAYS_CHAR (COMMON_LOCATION_DOWNLOAD_STORAGE_SUBDIRECTORY);
#endif // ACE_WIN32 || ACE_WIN64

  // sanity check(s): directory exists ?
  // No ? --> (try to) create it then
  if (unlikely (!Common_File_Tools::isDirectory (result)))
  {
    if (!Common_File_Tools::createDirectory (result))
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Common_File_Tools::createDirectory(\"%s\"), continuing\n"),
                  ACE_TEXT (result.c_str ())));
    else
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("created directory: \"%s\"\n"),
                  ACE_TEXT (result.c_str ())));
  } // end IF

  return result;
}

std::string
Common_File_Tools::getTempDirectory ()
{
  COMMON_TRACE (ACE_TEXT ("Common_File_Tools::getTempDirectory"));

  // initialize return value(s)
  std::string result;

  unsigned int fallback_level = 0;
  std::string environment_variable =
      ACE_TEXT_ALWAYS_CHAR (COMMON_LOCATION_TEMPORARY_STORAGE_VARIABLE);

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  DWORD result_2 = 0;
  std::string::size_type position = std::string::npos;
use_environment:
#endif // ACE_WIN32 || ACE_WIN64
  ACE_TCHAR* string_p =
    ACE_OS::getenv (ACE_TEXT (environment_variable.c_str ()));
  if (unlikely (!string_p))
  {
    ACE_DEBUG ((LM_WARNING,
                ACE_TEXT ("failed to ACE_OS::getenv(\"%s\"): \"%m\", falling back\n"),
                ACE_TEXT (environment_variable.c_str ())));
    goto fallback;
  } // end IF
  result = ACE_TEXT_ALWAYS_CHAR (string_p);
  goto continue_;

#if defined (ACE_WIN32) || defined (ACE_WIN64)
use_system_api:
  // *NOTE*: %USERPROFILE% is not localized
  ACE_TCHAR buffer_a[PATH_MAX];
  ACE_OS::memset (buffer_a, 0, sizeof (ACE_TCHAR[PATH_MAX]));
  //HRESULT result_2 = S_OK;

  //result_2 =
  //  SHGetFolderPath (NULL,                                         // hwndOwner
  //                   CSIDL_LOCAL_APPDATA | CSIDL_FLAG_DONT_VERIFY, // nFolder
  //                   NULL,                                         // hToken
  //                   SHGFP_TYPE_CURRENT,                           // dwFlags
  //                   buffer);                                      // pszPath
  result_2 = ACE_TEXT_GetTempPath (sizeof (ACE_TCHAR[PATH_MAX]), buffer_a);
  //if (FAILED (result_2))
  if (unlikely (result_2 == 0))
  {
    //ACE_DEBUG ((LM_WARNING,
    //            ACE_TEXT ("failed to SHGetFolderPath(CSIDL_LOCAL_APPDATA): \"%s\", falling back\n"),
    //            buffer));
    ACE_DEBUG ((LM_WARNING,
                ACE_TEXT ("failed to GetTempPath(): \"%s\", falling back\n"),
                ACE_TEXT (Common_Error_Tools::errorToString (::GetLastError (), false).c_str ())));
    goto fallback;
  } // end IF
  result = ACE_TEXT_ALWAYS_CHAR (buffer_a);

  // strip trailing backslashes
  position =
    result.find_last_of ('\\',
                         std::string::npos); // begin searching at the end !
  if (likely (position != std::string::npos))
    result = result.substr (0, position);
#else
use_path:
#endif // ACE_WIN32 || ACE_WIN64

continue_:
  // sanity check(s): directory exists ?
  // No ? --> (try to) create it then
  if (unlikely (!Common_File_Tools::isDirectory (result)))
  {
    if (!Common_File_Tools::createDirectory (result))
    {
      ACE_DEBUG ((LM_WARNING,
                  ACE_TEXT ("failed to Common_File_Tools::createDirectory(\"%s\"), falling back\n"),
                  ACE_TEXT (result.c_str ())));
      goto fallback;
    } // end IF
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("created directory: \"%s\"\n"),
                ACE_TEXT (result.c_str ())));
  } // end IF

  return result;

fallback:
  ++fallback_level;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  switch (fallback_level)
  {
    case 1:
    {
      environment_variable =
          ACE_TEXT_ALWAYS_CHAR (COMMON_LOCATION_TEMPORARY_STORAGE_VARIABLE_2);
      goto use_environment;
    }
    case 2:
      goto use_system_api;
    default:
      break;
  } // end SWITCH
  ACE_ASSERT (false);
  // *TODO*: implement fallback levels dependent on host Windows (TM) version
  //         see: https://en.wikipedia.org/wiki/Environment_variable#Windows
#else
  switch (fallback_level)
  {
    case 1:
      result =
          ACE_TEXT_ALWAYS_CHAR (COMMON_LOCATION_TEMPORARY_STORAGE_DIRECTORY);
      goto use_path;
    case 2:
      result =
          ACE_TEXT_ALWAYS_CHAR (COMMON_LOCATION_TEMPORARY_STORAGE_DIRECTORY_2);
      goto use_path;
    default:
      break;
  } // end SWITCH
  ACE_ASSERT (false);
  // *TODO*: implement fallback levels dependent on host platform/version
  //         see e.g. https://en.wikipedia.org/wiki/Filesystem_Hierarchy_Standard
#endif // ACE_WIN32 || ACE_WIN64

  return result;
}

std::string
Common_File_Tools::getTempFilename (const std::string& prefix_in,
                                    bool fullPath_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_File_Tools::getTempFilename"));

  std::string result;

  if (likely (fullPath_in))
  {
    result = Common_File_Tools::getTempDirectory ();
    if (unlikely (result.empty ()))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Common_File_Tools::getTempDirectory(), aborting\n")));
      return result;
    } // end IF
    result += ACE_DIRECTORY_SEPARATOR_STR_A;
  } // end IF

  // sanity check(s)
  ACE_ASSERT (prefix_in.size () <= (BUFSIZ - 6 + 1));

  // *NOTE*: see also: man 3 mkstemp
  ACE_TCHAR buffer_a[BUFSIZ];
  size_t offset_i = prefix_in.size ();
  char* result_p = NULL;
  if (unlikely (!prefix_in.empty ()))
  {
    ACE_OS::strcpy (buffer_a, ACE_TEXT (prefix_in.c_str ()));
    ACE_OS::strcpy (buffer_a + prefix_in.size (), ACE_TEXT ("_"));
    ++offset_i;
  } // end IF
  ACE_OS::strcpy (buffer_a + offset_i, ACE_TEXT ("XXXXXX"));
  result_p = ACE_OS::mktemp (buffer_a);
  if (unlikely (!result_p))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::mktemp(\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (buffer_a)));
    return ACE_TEXT_ALWAYS_CHAR ("");
  } // end IF
  result += ACE_TEXT_ALWAYS_CHAR (buffer_a);

  return result;
}

struct Common_File_Entry
Common_File_Tools::parseFileEntry (const std::string& entry_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_File_Tools::parseFileEntry"));

  struct Common_File_Entry result;

  // *NOTE*: e.g. "drwxrwxr-x   6 ftpfau   ftpfau       4096 Jan 15 16:35 almalinux"
  //              "drwxr-xr-x  11 ftpfau   ftpfau       4096 Apr 30  2021 cdn.media.ccc.de"
  std::string regex_string = // drwxrwxr-x                    6                  ftpfau                 ftpfau                 4096               Jan                             15                               16            :    35                                   almalinux
    ACE_TEXT_ALWAYS_CHAR ("^([[:alpha:]-]{10})(?:[[:space:]]+)([[:digit:]]+)(?: )([^ ]+)(?:[[:space:]]+)([^ ]+)(?:[[:space:]]+)([[:digit:]]+)(?: )([[:alpha:]]{3})(?:[[:space:]]+)([[:digit:]]+)(?:[[:space:]]+)(?:([[:digit:]]+)(?::)([[:digit:]]+)|([[:digit:]]{4}))(?: )(.+)$");
  std::regex regex;
  std::regex::flag_type flags = std::regex_constants::ECMAScript;
  regex.assign (regex_string, flags);
  std::smatch match_results;

  if (!std::regex_match (entry_in,
                         match_results,
                         regex,
                         std::regex_constants::match_default))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to parse file entry string (was: \"%s\"), aborting\n"),
                ACE_TEXT (entry_in.c_str ())));
    return result;
  } // end IF
  ACE_ASSERT (match_results.ready () && !match_results.empty ());

  ACE_ASSERT (match_results[1].matched);
  switch (match_results[1].str ()[0])
  {
    case 'd':
      result.type = Common_File_Entry::DIRECTORY; break;
    case '-':
    //case 'f':
      result.type = Common_File_Entry::FILE; break;
    case 'l':
      result.type = Common_File_Entry::LINK; break;
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown file entry type (was: \"%s\"), aborting\n"),
                  ACE_TEXT (match_results[1].str ().c_str ())));
      return result;
    }
  } // end SWITCH

  ACE_ASSERT (match_results[3].matched);
  result.owner = match_results[3].str ();

  ACE_ASSERT (match_results[4].matched);
  result.group = match_results[4].str ();

  ACE_ASSERT (match_results[5].matched);
  std::istringstream converter (match_results[5].str ());
  converter >> result.size;

  // *TODO*: process date
  bool has_year_b = !match_results[6].matched;
  ACE_UNUSED_ARG (has_year_b);

  ACE_ASSERT (match_results[11].matched);
  result.name = match_results[11].str ();

  return result;
}
