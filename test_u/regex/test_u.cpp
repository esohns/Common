
#include <iostream>
#include <regex>
#include <string>

#include "ace/Assert.h"

bool
regex_test_1 ()
{
  std::string test_string (" interface \"wlp3s0\";");
  std::string regex_string ("^(?: interface \x22)([^\x22]+)(?:\x22;)$");
  std::regex regex (regex_string);
  std::smatch results;
  if (!std::regex_match (test_string,
                         results,
                         regex,
                         std::regex_constants::match_default))
  {
    std::cout << "failed!\n";
    return false;
  }
  std::cout << "success: \"" << results[1].str () << "\"\n";

  return true;
}

bool
regex_test_2 ()
{
  std::string test_string ("D:\\projects\\ACEStream\\test_u\\output.bin");
  std::string regex_string ("^([^_.]+)(?:_([[:digit:]]+))?(\\..+)?$");
  std::regex regex (regex_string,
                    std::regex::ECMAScript);
  std::smatch results;
  if (!std::regex_match (test_string,
                         results,
                         regex,
                         std::regex_constants::match_default))
  {
    std::cout << "failed!\n";
    return false;
  }
  ACE_ASSERT (!results.empty ());
  ACE_ASSERT (results[1].matched);
  ACE_ASSERT (results[3].matched);

  std::cout << "success: \"" << results[1].str () << "\"\n";

  return true;
}

int
main (int argc, char *argv[])
{
  if (!regex_test_1 ())
  {
    std::cout << "test 1 failed!\n";
    return -1;
  }
  std::cout << "test 1 success\n";

  if (!regex_test_2 ())
  {
    std::cout << "test 2 failed!\n";
    return -1;
  }
  std::cout << "test 2 success\n";

  return 0;
}
