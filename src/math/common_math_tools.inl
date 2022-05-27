
#include "ace/Log_Msg.h"

#include "common_macros.h"

template <typename ContainerType>
void
Common_Math_Tools::rotate (ContainerType& container_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Math_Tools::rotate"));

  // sanity check(s)
  if (container_in.empty() || container_in.size () == 1)
    return;

  typename ContainerType::value_type first = container_in[0];
  int j = 0, i = 1;
  for (;
       i < static_cast<int> (container_in.size ());
       ++j, ++i)
    container_in[j] = container_in[i];
  container_in[j] = first;
}

template <typename ContainerType>
ContainerType
Common_Math_Tools::permute (ContainerType& container_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Math_Tools::permute"));

  ContainerType result;

  // sanity check(s)
  if (container_in.empty() || container_in.size () == 1)
    return result;

  if (std::next_permutation (container_in.begin (), container_in.end ()))
    result = container_in;

  return result;
}
