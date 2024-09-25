#ifndef COMMON_MATH_PIDCONTROL_H
#define COMMON_MATH_PIDCONTROL_H

#include <cmath>
#include <limits>

#include "ace/Global_Macros.h"

#include "common_timer.h"

class Common_PIDControl
{
 public:
  Common_PIDControl (double = 1.0,                                  // kP
                     double = 0.0,                                  // kI
                     double = 0.0,                                  // kD
                     double = std::numeric_limits<double>::max (),  // uMax
                     double = std::numeric_limits<double>::min ()); // uMin
  inline virtual ~Common_PIDControl () {}

  double kP_, kI_, kD_; // controller constants
  double uMax_, uMin_; // actuator limits for anti-wind-up

  inline void reset () { errorIntegral_ = 0.0; errorPrevious_ = NAN; timer_.reset (); }

  // get actuator command from error, which is target minus actual control value
  double operator() (double,        // error
                     double = NAN); // error derivate

  // temporarily disable and re-enable the integrator e.g. before/after actuator
  // is ineffective
  inline void disableIntegrator () { integratorActive_ = false; }
  inline void enableIntegrator () { integratorActive_ = true; }

 protected:
  bool         integratorActive_;
  double       errorIntegral_; // integral of previous errors
  double       errorPrevious_; // error at last call
  Common_Timer timer_; // time since last call

 private:
  ACE_UNIMPLEMENTED_FUNC (Common_PIDControl ())
  ACE_UNIMPLEMENTED_FUNC (Common_PIDControl (const Common_PIDControl&))
  ACE_UNIMPLEMENTED_FUNC (Common_PIDControl& operator= (const Common_PIDControl&))
};

#endif // COMMON_MATH_PIDCONTROL_H
