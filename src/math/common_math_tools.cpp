#include "stdafx.h"

#include "common_math_tools.h"

#include "ace/Log_Msg.h"

#include "common_macros.h"

std::string
Common_Math_Tools::encodeBase64 (const void* data_in,
                                 unsigned int length_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Math_Tools::encodeBase64"));

  std::string result;

  static std::string alphabet_a =
    ACE_TEXT_ALWAYS_CHAR ("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/");

  union
  {
    ACE_UINT32 big_endian; // !
#if defined (_MSC_VER)
#pragma pack (push, 1)
#endif // _MSC_VER
    struct
    {
      ACE_UINT32 fourth : 6;
      ACE_UINT32 third : 6;
      ACE_UINT32 second : 6;
      ACE_UINT32 first  : 6;
#if defined (__GNUC__)
    } __attribute__ ((__packed__));
#else
    };
#endif // __GNUC__
#if defined (_MSC_VER)
#pragma pack (pop)
#endif // _MSC_VER
  } converter;
  const unsigned char* data_p =
    reinterpret_cast<const unsigned char*> (data_in);

  // sanity check
  if ((length_in == 1) || (length_in == 2))
    goto continue_;

  // process any byte-triplets
  for (;
       data_p < (reinterpret_cast<const unsigned char*> (data_in) + length_in);
       data_p += 3)
  {
    converter.big_endian =
      ((*data_p) << 16) | (*(data_p + 1) << 8) | *(data_p + 2);
    //if (ACE_BYTE_ORDER == ACE_LITTLE_ENDIAN)
    //  converter.big_endian = ACE_SWAP_LONG (converter.big_endian);
    //ACE_ASSERT (!converter.zero);
    result += alphabet_a[converter.first];
    result += alphabet_a[converter.second];
    result += alphabet_a[converter.third];
    result += alphabet_a[converter.fourth];
  } // end FOR
  data_p -= (3 - (length_in % 3));

continue_:
  // process any leftover byte(s)
  switch (length_in % 3)
  {
    case 1:
    { // --> add two sextets and two padding bytes
      result += alphabet_a[(*data_p & 0xFC) >> 2];
      result += alphabet_a[(*data_p & 0x03) << 4];
      result += ACE_TEXT_ALWAYS_CHAR ("==");
      break;
    }
    case 2:
    { // --> add three sextets and one padding byte
      result += alphabet_a[(*data_p & 0xFC) >> 2];
      result += alphabet_a[((*data_p & 0x03) << 4) | ((*(data_p + 1) & 0xF0) >> 4)];
      result += alphabet_a[(*(data_p + 1) & 0x0F) << 2];
      result += '=';
      break;
    }
    default:
      break;
  } // end SWITCH

  return result;
}
