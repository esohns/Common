﻿#ifndef COMMON_MATH_TOOLS_H
#define COMMON_MATH_TOOLS_H

#include <bitset>
#include <cmath>
#include <string>

#include "ace/Basic_Types.h"
#include "ace/Global_Macros.h"

class Common_Math_Tools
{
 public:
  inline static size_t bits (ACE_UINT32 bitmask_in) { std::bitset<static_cast<size_t> (sizeof (ACE_UINT32) * 8)> bitset_m (static_cast<size_t> (bitmask_in)); return bitset_m.count (); }
  // *IMPORTANT NOTE*: the sign (if any) counts as an extra digit
  inline static size_t digitsBase10 (int i) { size_t sign_i = (i < 0 ? 1 : 0); return static_cast<int> (::log10 (static_cast<double> (i))) + 1 + sign_i; }

  static std::string encodeBase64 (const void*,   // data
                                   unsigned int); // length

 private:
  ACE_UNIMPLEMENTED_FUNC (Common_Math_Tools ())
  ACE_UNIMPLEMENTED_FUNC (~Common_Math_Tools ())
  ACE_UNIMPLEMENTED_FUNC (Common_Math_Tools (const Common_Math_Tools&))
  ACE_UNIMPLEMENTED_FUNC (Common_Math_Tools& operator= (const Common_Math_Tools&))
};

#endif
