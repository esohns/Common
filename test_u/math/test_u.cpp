#include "stdafx.h"

#include <iostream>
#include <vector>

#include "common_math_tools.h"

template <typename T>
void
print_vector (std::vector<T>& vector_in)
{
  for (int i = 0;
       i < static_cast<int> (vector_in.size ());
       ++i)
    std::cout << vector_in[i];
  std::cout << std::endl;
}

template <typename T>
void
print_vectors (std::vector<std::vector<T>>& vectors_in)
{
  std::cout << vectors_in.size () << ACE_TEXT_ALWAYS_CHAR (" result(s)") << std::endl;
  for (int i = 0;
       i < static_cast<int> (vectors_in.size ());
       ++i)
    print_vector (vectors_in[i]);
}

int
main (int argc_in, char *argv_in[])
{
//  std::vector<int> test_vector = {1, 2, 3, 4, 5}, result;
//  std::vector<std::vector<int>> results;
//  results.push_back (test_vector);
//  do
//  {
//    result = Common_Math_Tools::permute (test_vector);
//    if (result.empty())
//      break;
//    results.push_back (result);
//  } while (true);
//  print_vectors (results);

  std::vector<int> test_vector = {1, 2, 3, 4};
  std::vector<std::vector<int>> results;
  std::vector<int> positions_a (test_vector.size (), 0);
  Common_Math_Tools::combine (test_vector,
                              positions_a,
                              0,
                              results);
  print_vectors (results);

  return 0;
}
