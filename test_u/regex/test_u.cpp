
#include <iostream>
#include <regex>
#include <string>

int
main (int argc, char *argv[])
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
    return -1;
  }
  std::cout << "success: \"" << results[1].str () << "\"\n";

  return 0;
}
