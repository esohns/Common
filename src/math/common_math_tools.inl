#include <algorithm>

#include "ace/Log_Msg.h"

#include "common_macros.h"

template <typename ValueType>
ValueType
Common_Math_Tools::map (ValueType value_in,
                        ValueType fromStart_in,
                        ValueType fromEnd_in,
                        ValueType toStart_in,
                        ValueType toEnd_in,
                        int decimalPrecision_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Math_Tools::map"));

  // sanity check(s)
  ACE_ASSERT (value_in >= fromStart_in);
  ACE_ASSERT (value_in <= fromEnd_in);
  ACE_ASSERT (toStart_in < toEnd_in);

  ValueType delta_from = fromEnd_in - fromStart_in;
  ValueType delta_to = toEnd_in - toStart_in;
  ValueType result, offset;
  if (std::is_integral<ValueType>::value)
  {
    double scale = delta_to / (double)delta_from;
    offset = (-fromStart_in * static_cast<ValueType> (scale)) + toStart_in;
    result = (value_in * static_cast<ValueType> (scale)) + offset;
  } // end IF
  else
  {
    ValueType scale = delta_to / delta_from;
    offset = (-fromStart_in * scale) + toStart_in;
    result = (value_in * scale) + offset;
  } // end ELSE

  if (std::is_floating_point<ValueType>::value &&
      decimalPrecision_in >= 0)
  {
    int calc_scale = static_cast<int> (std::pow (10, decimalPrecision_in));
    return static_cast<ValueType> (std::round (result * calc_scale) / calc_scale);
  } // end IF

  return result;
}

template <typename ValueType>
std::enable_if_t<!std::is_integral<ValueType>::value, ValueType>
Common_Math_Tools::lerp (ValueType start_in,
                         ValueType end_in,
                         ValueType amount_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Math_Tools::lerp"));

  return (static_cast<ValueType> (1.0) - amount_in) * start_in + amount_in * end_in;
}

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

template <typename ContainerType>
void
Common_Math_Tools::combine (ContainerType& container_in,
                            std::vector<int>& positions_inout,
                            int depth_in,
                            std::vector<ContainerType>& results_inout)
{
  COMMON_TRACE (ACE_TEXT ("Common_Math_Tools::combine"));

  // sanity check(s)
  if (depth_in == static_cast<int> (container_in.size ()))
  {
    ContainerType result;
    for (int i = 0; i != depth_in; ++i)
      result.push_back (container_in[positions_inout[i]]);
    results_inout.push_back (result);
    return;
  } // end IF

  for (int i = 0 ; i != static_cast<int> (container_in.size ()); ++i)
  {
    positions_inout[depth_in] = i;
    Common_Math_Tools::combine (container_in,
                                positions_inout,
                                depth_in + 1,
                                results_inout);
  } // end FOR
}
