#pragma once

#include "ace/Global_Macros.h"

#include "common_icounter.h"
#include "common_referencecounter_base.h"

class Common_Counter
 : public Common_ReferenceCounterBase
 , virtual Common_ICounter
{
  typedef Common_ReferenceCounterBase inherited;
 
 public:
  Common_Counter (unsigned int = 0); // initial count
  inline virtual ~Common_Counter () {}

  // implement Common_ICounter
  // *WARNING*: ATM this implementation is not thread-safe
  virtual void reset ();

 private:
  ACE_UNIMPLEMENTED_FUNC (Common_Counter ())
  ACE_UNIMPLEMENTED_FUNC (Common_Counter (const Common_Counter&))
  ACE_UNIMPLEMENTED_FUNC (Common_Counter& operator= (const Common_Counter&))

  unsigned int initialCount_;
};
