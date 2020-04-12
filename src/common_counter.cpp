#include "common_counter.h"

#include "common_macros.h"

Common_Counter::Common_Counter (unsigned int initialCount_in)
 : inherited (initialCount_in, false)
 , initialCount_ (initialCount_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Counter::Common_Counter"));

}

void
Common_Counter::reset ()
{
  COMMON_TRACE (ACE_TEXT ("Common_Counter::reset"));

  unsigned int count_i = inherited::count ();
  unsigned int iterations_i =
    ((initialCount_ > count_i) ? initialCount_ - count_i
                               : count_i - initialCount_);

  if (count_i > initialCount_)
    for (unsigned int i = 0;
         i < iterations_i;
         ++i)
      inherited::decrement ();
  else
    for (unsigned int i = 0;
         i < iterations_i;
         ++i)
      inherited::increment ();
}
