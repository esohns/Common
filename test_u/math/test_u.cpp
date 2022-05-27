#include "stdafx.h"

#include <iostream>
#include <vector>

#include "common_math_tools.h"

void
print_vector (std::vector<int>& vector_in)
{
  for (int i = 0;
       i < static_cast<int> (vector_in.size ());
       ++i)
    std::cout << vector_in[i];
  std::cout << std::endl;
}

void
print_vectors (std::vector<std::vector<int>>& vectors_in)
{
  for (int i = 0;
       i < static_cast<int> (vectors_in.size ());
       ++i)
    print_vector (vectors_in[i]);
}

int
main (int argc_in, char *argv_in[])
{
  std::vector<int> test_vector = {1, 2, 3, 4}, result;
  std::vector<std::vector<int>> results;
  results.push_back (test_vector);
  do
  {
    result = Common_Math_Tools::permute (test_vector);
    if (result.empty())
      break;
    results.push_back (result);
  } while (true);
  print_vectors (results);

  return 0;
}
