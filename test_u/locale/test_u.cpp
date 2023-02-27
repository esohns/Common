
#include <iostream>
#include <locale>
#include <string>

#include "ace/Log_Msg.h"

#include "common_defines.h"
#include "common_os_tools.h"

int
main (int argc, char *argv[])
{
  // *NOTE*: the default locale is the 'C' locale (as in:
  //         'std::setlocale(LC_ALL, "C")')
  //         --> replace with (C++-)US-ASCII
  std::locale locale;
  try {
    std::locale us_ascii_locale (ACE_TEXT_ALWAYS_CHAR ("C"));
    locale = us_ascii_locale;
  } catch (std::runtime_error& exception_in) {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("caught exception in std::locale(\"%s\"): \"%s\", aborting\n"),
                ACE_TEXT (COMMON_LOCALE_EN_US_ASCII_STRING),
                ACE_TEXT (exception_in.what ())));
    Common_OS_Tools::printLocales ();
    return -1;
  } catch (...) {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("caught exception in std::locale(\"%s\"), aborting\n"),
                ACE_TEXT (COMMON_LOCALE_EN_US_ASCII_STRING)));
    Common_OS_Tools::printLocales ();
    return -1;
  }

  std::string test_string ("Ù");
  for (std::string::const_iterator iterator = test_string.begin ();
       iterator != test_string.end ();
       ++iterator)
  {
    if (std::isalpha (*iterator, locale))
      std::cout << "isalpha: " << ACE_TEXT_ALWAYS_CHAR ("true") << "\n";
    else
      std::cout << "isalpha: " << ACE_TEXT_ALWAYS_CHAR ("false") << "\n";
  } // end FOR

  return 0;
}
