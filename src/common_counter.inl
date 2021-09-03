#include "common_macros.h"

template <ACE_SYNCH_DECL>
Common_Counter_T<ACE_SYNCH_USE>::Common_Counter_T (unsigned int initialCount_in)
 : inherited (initialCount_in, false)
 , initialCount_ (initialCount_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Counter_T::Common_Counter_T"));

}

template <ACE_SYNCH_DECL>
void
Common_Counter_T<ACE_SYNCH_USE>::Common_Counter_T::reset ()
{
  COMMON_TRACE (ACE_TEXT ("Common_Counter_T::reset"));

  { ACE_GUARD (ACE_SYNCH_MUTEX_T, aGuard, inherited::lock_);
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
  } // end lock scope
}
