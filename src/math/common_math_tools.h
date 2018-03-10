#ifndef COMMON_MATH_TOOLS_H
#define COMMON_MATH_TOOLS_H

#include <bitset>

#include "ace/Basic_Types.h"
#include "ace/Global_Macros.h"

class Common_Math_Tools
{
 public:
  inline size_t bits (ACE_UINT32 bitmask_in) { std::bitset<sizeof (ACE_UINT32) * 8> bitset_m (static_cast<unsigned long> (bitmask_in)); return bitset_m.count (); }

 private:
  ACE_UNIMPLEMENTED_FUNC (Common_Math_Tools ())
  ACE_UNIMPLEMENTED_FUNC (~Common_Math_Tools ())
  ACE_UNIMPLEMENTED_FUNC (Common_Math_Tools (const Common_Math_Tools&))
  ACE_UNIMPLEMENTED_FUNC (Common_Math_Tools& operator= (const Common_Math_Tools&))
};

#endif
