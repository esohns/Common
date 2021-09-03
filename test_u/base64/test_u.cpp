
#include <iostream>
#include <string>

#include "common_math_tools.h"

int
main (int argc, char *argv[])
{
  std::string test_string ("this is a test !");
  std::string base64_encoded =
    Common_Math_Tools::encodeBase64 (test_string.c_str (),
                                     test_string.size ());
  std::cout << "\"" << base64_encoded << "\"" << std::endl;

  return 0;
}
