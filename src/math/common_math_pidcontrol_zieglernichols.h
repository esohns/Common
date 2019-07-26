#ifndef COMMON_MATH_PIDCONTROL_ZIEGLERNICHOLS_H
#define COMMON_MATH_PIDCONTROL_ZIEGLERNICHOLS_H

#include <cmath>
#include <limits>

#include "ace/Global_Macros.h"
#include "ace/Thread_Mutex.h"

#include "common_math_pidcontrol.h"
#include "common_math_pt1_tracker.h"

class Common_PIDControl_ZieglerNichols
 // *NOTE*: parameters initially tuned according to "en.wikipedia.org/wiki/Ziegler%E2%80%93Nichols_method"
 //         using the PD-rule, i.e. Kp = 0.8 * Ku, Ki = 0, Kd = 0.1 * Kp * Tu
 : public Common_PIDControl
{
 typedef Common_PIDControl inherited;

 public:
  Common_PIDControl_ZieglerNichols (double,                                        // Ku
                                    double,                                        // Tu
                                    double = std::numeric_limits<double>::max (),  // uMax
                                    double = std::numeric_limits<double>::min ()); // uMin
  inline virtual ~Common_PIDControl_ZieglerNichols () {}

 protected:
  Common_PT1Tracker_T<ACE_MT_SYNCH> actual_;

 private:
  ACE_UNIMPLEMENTED_FUNC (Common_PIDControl_ZieglerNichols ())
  ACE_UNIMPLEMENTED_FUNC (Common_PIDControl_ZieglerNichols (const Common_PIDControl_ZieglerNichols&))
  ACE_UNIMPLEMENTED_FUNC (Common_PIDControl_ZieglerNichols& operator= (const Common_PIDControl_ZieglerNichols&))
};

#endif // COMMON_MATH_PIDCONTROL_ZIEGLERNICHOLS_H
