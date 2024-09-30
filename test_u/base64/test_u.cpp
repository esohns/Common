#include "stdafx.h"

#include <iostream>
#include <string>

#include "ace/OS.h"
#include "ace/OS_main.h"

#include "common_math_tools.h"

int
main (int argc,
      char* argv[])
{
  std::string test_string ("this is a test !");

  std::string base64_encoded =
    Common_Math_Tools::encodeBase64 (test_string.c_str (),
                                     static_cast<unsigned int> (test_string.size ()));
  std::cout << "\"" << test_string << "\" --> \"" << base64_encoded << "\"" << std::endl;

  ACE_ASSERT (!ACE_OS::strcmp (base64_encoded.c_str (), ACE_TEXT_ALWAYS_CHAR ("dGhpcyBpcyBhIHRlc3QgIQ==")));

  std::string base64_decoded =
    Common_Math_Tools::decodeBase64 (base64_encoded);
  std::cout << "\"" << base64_encoded << "\" --> \"" << base64_decoded << "\"" << std::endl;

  // sanity check(s)
  ACE_ASSERT (!ACE_OS::strcmp (test_string.c_str (), base64_decoded.c_str ()));

  return 0;
}
