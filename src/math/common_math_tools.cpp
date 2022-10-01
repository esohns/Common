#include "stdafx.h"

#include "common_math_tools.h"

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
    ACE_UINT32 integer;
#if defined (_MSC_VER)
#pragma pack (push, 1)
#endif // _MSC_VER
    struct
    {
      ACE_UINT32 fourth : 6;
      ACE_UINT32 third  : 6;
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
  const ACE_UINT8* data_p =
    reinterpret_cast<const ACE_UINT8*> (data_in);

  // sanity check
  if ((length_in == 1) || (length_in == 2))
    goto continue_;

  // process any byte-triplets
  for (unsigned int i = length_in;
       i >= 3;
       i -= 3, data_p += 3)
  {
    converter.integer =
      ((*data_p) << 16) | (*(data_p + 1) << 8) | *(data_p + 2);
    result += alphabet_a[converter.first];
    result += alphabet_a[converter.second];
    result += alphabet_a[converter.third];
    result += alphabet_a[converter.fourth];
  } // end FOR

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

std::string
Common_Math_Tools::decodeBase64 (const std::string& data_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Math_Tools::decodeBase64"));

  std::string result;

  static std::string alphabet_a =
    ACE_TEXT_ALWAYS_CHAR ("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/");

  union
  {
    ACE_UINT32 integer;
#if defined (_MSC_VER)
#pragma pack (push, 1)
#endif // _MSC_VER
    struct
    {
      ACE_UINT32 fourth : 6;
      ACE_UINT32 third  : 6;
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

  // process byte-quads
  for (size_t i = 0;
       i < data_in.size ();
       i += 4)
  {
    converter.first  = data_in[i]     == '=' ? 0 : alphabet_a.find (data_in[i]    , 0);
    converter.second = data_in[i + 1] == '=' ? 0 : alphabet_a.find (data_in[i + 1], 0);
    converter.third  = data_in[i + 2] == '=' ? 0 : alphabet_a.find (data_in[i + 2], 0);
    converter.fourth = data_in[i + 3] == '=' ? 0 : alphabet_a.find (data_in[i + 3], 0);

    result += (converter.integer >> 16) & 0xFF;
    result += (converter.integer >> 8)  & 0xFF;
    result += converter.integer         & 0xFF;
  } // end FOR

  return result;
}
